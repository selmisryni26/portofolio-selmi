#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <time.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

/* ===================== PIN ===================== */

#define DS18B20_PIN    14
#define TURBIDITY_PIN  34

#define RELAY_HEATER   4
#define RELAY_POMPA1   18
#define RELAY_POMPA2   5

#define SERVO_PIN      26

/* ===================== WIFI ===================== */

const char* ssid = "Barokah";
const char* password = "YOUR_WIFI_PASSWORD";

/* ===================== FIREBASE ===================== */

#define API_KEY "YOUR_FIREBASE_API_KEY"
#define DATABASE_URL "YOUR_FIREBASE_DATABASE_URL"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;

/* ===================== DS18B20 ===================== */

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

/* ===================== SERVO ===================== */

Servo feeder;

/* ===================== PENGATURAN SERVO ===================== */

int sudutAwalServo = 0;

int sudutPakanServo = 90;

int durasiServo = 15; // detik

/* ===================== VARIABEL ===================== */

float suhu = 0;
float ntu = 0;

int tahapGantiAir = 0;
unsigned long waktuPompa = 0;

const unsigned long DURASI_POMPA1 = 120000; // 2 menit
const unsigned long DURASI_POMPA2 = 120000; // 2 menit

bool prosesGantiAir = false;

String modeSistem = "AUTO";

bool pompaManual = false;
bool heaterManual = false;

bool feedNow = false;
String jadwalPagi = "05:00";
String jadwalSiang = "12:00";
String jadwalSore = "17:00";

String jadwalTerakhir = "";

unsigned long lastJadwalRead = 0;

bool statusPompa = false;
bool statusHeater = false;
unsigned long lastHistorySave = 0;
const unsigned long intervalHistory = 60000; // 1 menit

bool servoAktif = false;

unsigned long waktuServo = 0;

unsigned long lastSensorRead = 0;
unsigned long lastFirebaseSend = 0;

unsigned long lastControlRead = 0;

const unsigned long intervalControl = 2000;

const unsigned long intervalSensor = 2000;    // baca sensor tiap 2 detik
const unsigned long intervalFirebase = 2000;  // kirim firebase tiap 2 detik
/* ===================== KALIBRASI TURBIDITY ===================== */

int adcAirBersih = 0;

const float targetNTU = 0.0;

// Semakin besar nilai semakin lambat kenaikan NTU
const float FAKTOR_PEMBAGI = 4095.0;

/* =====================================================
   BACA TURBIDITY
   ===================================================== */

float bacaNTU()
{
  long total = 0;

  for (int i = 0; i < 50; i++)
  {
    total += analogRead(TURBIDITY_PIN);
    delay(2);
  }

  int adcValue = total / 50;

  int selisihADC = adcAirBersih - adcValue;

  float ntuValue = targetNTU;

  if (selisihADC > 15)
  {
    ntuValue =
      targetNTU +
      (((float)selisihADC / FAKTOR_PEMBAGI) * 100.0) +
      1;
  }
  else
  {
    ntuValue = 0.0;
  }

  if (ntuValue < 0) ntuValue = 0;
  if (ntuValue > 1000) ntuValue = 1000;

  return ntuValue;
}

/* =====================================================
   GANTI AIR
   ===================================================== */

void gantiAir()
{
    if (prosesGantiAir) return;

    Serial.println("\n================================");
    Serial.println("      PROSES GANTI AIR");
    Serial.println("================================");

    prosesGantiAir = true;
    tahapGantiAir = 1;

    digitalWrite(RELAY_HEATER, HIGH);
    statusHeater = false;

    digitalWrite(RELAY_POMPA1, LOW);

    Serial.println("Pompa 1 ON");

    waktuPompa = millis();
}

void prosesPompa()
{
    if (!prosesGantiAir) return;

    if (
        tahapGantiAir == 1 &&
        millis() - waktuPompa >= DURASI_POMPA1
       )
    {
        digitalWrite(RELAY_POMPA1, HIGH);

        Serial.println("Pompa 1 OFF");

        digitalWrite(RELAY_POMPA2, LOW);

        Serial.println("Pompa 2 ON");

        tahapGantiAir = 2;
        waktuPompa = millis();
    }

    else if (
        tahapGantiAir == 2 &&
        millis() - waktuPompa >= DURASI_POMPA2
    )
    {
        digitalWrite(RELAY_POMPA2, HIGH);

        Serial.println("Pompa 2 OFF");

        Serial.println("=== GANTI AIR SELESAI ===");

        tahapGantiAir = 0;
        prosesGantiAir = false;
    }
}

void stopGantiAir()
{
    digitalWrite(RELAY_POMPA1, HIGH);
    digitalWrite(RELAY_POMPA2, HIGH);

    prosesGantiAir = false;
    tahapGantiAir = 0;

    Serial.println("=== GANTI AIR DIHENTIKAN MANUAL ===");
}

/* =====================================================
   PAKAN
   ===================================================== */

void beriPakan()
{
    if (servoAktif) return;

    Serial.println("\n================================");
    Serial.println("       MEMBERI PAKAN");
    Serial.println("================================");

    feeder.write(sudutPakanServo);

    servoAktif = true;

    waktuServo = millis();
}

void prosesServo()
{
    if (!servoAktif) return;

    if (millis() - waktuServo >= (durasiServo * 1000))
    {
        feeder.write(sudutAwalServo);

        servoAktif = false;

        Serial.println("Servo Menutup");
        Serial.println("Pakan Berhasil Diberikan");
    }
}

/* =====================================================
   CEK JADWAL PAKAN
   ===================================================== */

void cekJadwalPakan()
{
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo))
    return;

  char jamSekarang[6];

  sprintf(
      jamSekarang,
      "%02d:%02d",
      timeinfo.tm_hour,
      timeinfo.tm_min
  );

  String sekarang = String(jamSekarang);

  if (
      (sekarang == jadwalPagi ||
       sekarang == jadwalSiang ||
       sekarang == jadwalSore)
      &&
      jadwalTerakhir != sekarang
     )
  {
      Serial.println("JADWAL PAKAN AKTIF");

      beriPakan();

      jadwalTerakhir = sekarang;
  }

  if (
      sekarang != jadwalPagi &&
      sekarang != jadwalSiang &&
      sekarang != jadwalSore
     )
  {
      jadwalTerakhir = "";
  }
}

void bacaJadwalFirebase()
{
  if (Firebase.RTDB.getString(
        &fbdo,
        "/jadwal/pagi"))
  {
    jadwalPagi = fbdo.stringData();
  }

  if (Firebase.RTDB.getString(
        &fbdo,
        "/jadwal/siang"))
  {
    jadwalSiang = fbdo.stringData();
  }

  if (Firebase.RTDB.getString(
        &fbdo,
        "/jadwal/sore"))
  {
    jadwalSore = fbdo.stringData();
  }
}

void bacaSettingServo()
{
  if (Firebase.RTDB.getInt(
        &fbdo,
        "/servo/sudut"))
  {
    sudutPakanServo = fbdo.intData();
  }

  if (Firebase.RTDB.getInt(
        &fbdo,
        "/servo/durasi"))
  {
    durasiServo = fbdo.intData();
  }
}

void bacaSensor()
{
    sensors.requestTemperatures();

    float temp = sensors.getTempCByIndex(0);

    if (temp > -50 && temp < 100)
    {
        suhu = temp;
    }

    ntu = bacaNTU();

    Serial.print("Suhu Update : ");
    Serial.println(suhu);

    Serial.print("NTU Update  : ");
    Serial.println(ntu);
}

void kirimFirebase()
{
    if (!Firebase.ready() || !signupOK)
        return;

    Firebase.RTDB.setFloat(
        &fbdo,
        "/sensor/suhu",
        suhu
    );

    Firebase.RTDB.setFloat(
        &fbdo,
        "/sensor/ntu",
        ntu
    );

    // =====================
    // SIMPAN HISTORY
    // =====================

    if (millis() - lastHistorySave >= intervalHistory)
    {
        lastHistorySave = millis();

        unsigned long waktu = time(nullptr);

        String key = String(waktu);

        Firebase.RTDB.setFloat(
            &fbdo,
            "/history/" + key + "/suhu",
            suhu
        );

        Firebase.RTDB.setFloat(
            &fbdo,
            "/history/" + key + "/ntu",
            ntu
        );

        Firebase.RTDB.setInt(
            &fbdo,
            "/history/" + key + "/waktu",
            waktu
        );

        Serial.println("History tersimpan");
    }

    Serial.println("Firebase Update");
}

void bacaControlFirebase()
{
    if (!Firebase.ready() || !signupOK)
        return;

    if (Firebase.RTDB.getString(&fbdo, "/control/mode"))
    {
        modeSistem = fbdo.stringData();
    }

    if (Firebase.RTDB.getBool(&fbdo, "/control/pompa"))
{
    pompaManual = fbdo.boolData();

    Serial.print("Status Pompa Firebase: ");
    Serial.println(pompaManual);
}
else
{
    Serial.println("Gagal membaca status pompa");
}

    if (Firebase.RTDB.getBool(&fbdo, "/control/heater"))
    {
        heaterManual = fbdo.boolData();
    }

    if (Firebase.RTDB.getBool(&fbdo, "/control/feedNow"))
    {
        feedNow = fbdo.boolData();
    }
}

/* =====================================================
   SETUP
   ===================================================== */
void setup()
{
  Serial.begin(115200);
  feeder.attach(SERVO_PIN);


  sensors.begin();

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  /* ================= KALIBRASI AIR BERSIH ================= */

long totalInisiasi = 0;

for (int i = 0; i < 100; i++)
{
  totalInisiasi += analogRead(TURBIDITY_PIN);
  delay(10);
}

adcAirBersih = totalInisiasi / 100;

Serial.print("ADC Air Bersih = ");
Serial.println(adcAirBersih);

Serial.println("Kalibrasi Selesai");
delay(2000);

  pinMode(RELAY_HEATER, OUTPUT);
  pinMode(RELAY_POMPA1, OUTPUT);
  pinMode(RELAY_POMPA2, OUTPUT);

  // Relay OFF saat startup
  digitalWrite(RELAY_HEATER, HIGH);
  digitalWrite(RELAY_POMPA1, HIGH);
  digitalWrite(RELAY_POMPA2, HIGH);


  feeder.write(sudutAwalServo);

  /* ================= WIFI ================= */

  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWiFi Terhubung");

  config.api_key = API_KEY;
config.database_url = DATABASE_URL;

if (Firebase.signUp(&config, &auth, "", ""))
{
  signupOK = true;
  Serial.println("Firebase Connected");
}
else
{
  Serial.println(config.signer.signupError.message.c_str());
}

Firebase.begin(&config, &auth);
Firebase.reconnectWiFi(true);

  configTime(7 * 3600, 0, "pool.ntp.org");

struct tm timeinfo;

  while (!getLocalTime(&timeinfo))
  {
      Serial.println("Menunggu waktu NTP...");
      delay(1000);
  }

  Serial.println("Waktu NTP berhasil");
  }

/* =====================================================
   LOOP
   ===================================================== */

void loop()
{

  prosesPompa();

  prosesServo();

  if (millis() - lastControlRead >= intervalControl)
{
    lastControlRead = millis();

    bacaControlFirebase();
}

  // ======================
// BACA SENSOR
// ======================

if (millis() - lastSensorRead >= intervalSensor)
{
    lastSensorRead = millis();

    bacaSensor();
}

// ======================
// KIRIM FIREBASE
// ======================

if (millis() - lastFirebaseSend >= intervalFirebase)
{
    lastFirebaseSend = millis();

    kirimFirebase();
}

  if (millis() - lastJadwalRead > 60000)
{
    lastJadwalRead = millis();

    bacaJadwalFirebase();

    bacaSettingServo();

    Serial.println("\n=== DATA FIREBASE ===");

    Serial.print("Pagi  : ");
    Serial.println(jadwalPagi);

    Serial.print("Siang : ");
    Serial.println(jadwalSiang);

    Serial.print("Sore  : ");
    Serial.println(jadwalSore);

    Serial.print("Sudut Servo : ");
    Serial.println(sudutPakanServo);

    Serial.print("Durasi Servo : ");
    Serial.println(durasiServo);
}

    long totalADC = 0;

for (int i = 0; i < 50; i++)
{
  totalADC += analogRead(TURBIDITY_PIN);
}

int adcNow = totalADC / 50;

float voltage =
  (adcNow * 3.3) / 4095.0;

  Serial.println("\n================================");

Serial.print("Suhu Air : ");
Serial.print(suhu);
Serial.println(" °C");

Serial.print("ADC Air Bersih : ");
Serial.println(adcAirBersih);

Serial.print("ADC Saat Ini   : ");
Serial.println(adcNow);

Serial.print("Tegangan       : ");
Serial.print(voltage, 3);
Serial.println(" V");

Serial.print("Kekeruhan      : ");
Serial.print(ntu);
Serial.println(" NTU");

Serial.print("Mode Sistem : ");
Serial.println(modeSistem);

  if (modeSistem == "AUTO")
{
    /* ================= SUHU ================= */

    if (suhu <= 25)
  {
      digitalWrite(RELAY_HEATER, LOW);
      statusHeater = true;

      Serial.println("Status : HEATER ON");
  }
    else if (suhu > 25 && suhu <= 27)
  {
      digitalWrite(RELAY_HEATER, HIGH);
      statusHeater = false;

      Serial.println("Status : SUHU NORMAL");
  }
    else
  {
      digitalWrite(RELAY_HEATER, HIGH);
      statusHeater = false;

      Serial.println("Status : SUHU TINGGI");

      gantiAir();
  }

    /* ================= KEKERUHAN ================= */

    if (ntu <= 10)
    {
        Serial.println("Kondisi Air : JERNIH");
    }
    else if (ntu <= 25)
    {
        Serial.println("Kondisi Air : KERUH");
        gantiAir();
    }
    else
    {
        Serial.println("Kondisi Air : KOTOR");
        gantiAir();
    }

    cekJadwalPakan();
  }
  else
{
    Serial.println("MODE MANUAL AKTIF");

    // ===== POMPA =====
    if (pompaManual)
    {
        if (!prosesGantiAir)
        {
            gantiAir();
        }
    }
    else
    {
        if (prosesGantiAir)
        {
            stopGantiAir();
        }
    }

    // ===== HEATER =====
    if (heaterManual)
    {
        digitalWrite(RELAY_HEATER, LOW);
        statusHeater = true;
    }
    else
    {
        digitalWrite(RELAY_HEATER, HIGH);
        statusHeater = false;
    }

    // ===== PAKAN =====
    if (feedNow)
    {
        beriPakan();

        Firebase.RTDB.setBool(
            &fbdo,
            "/control/feedNow",
            false
        );

        feedNow = false;
    }
}
}