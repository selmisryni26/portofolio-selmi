#define BLYNK_TEMPLATE_ID "TMPL6ytPpWbiz"
#define BLYNK_TEMPLATE_NAME "Monitoring dan Treatment Aquarium"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

// WiFi Credentials
char ssid[] = "Agnesss";  // Ganti dengan SSID WiFi
char pass[] = "YOUR_WIFI_PASSWORD";  // Ganti dengan password WiFi

// Pin Definitions
#define SENSOR_PIN 34  // Pin untuk sensor turbidity
#define RELAY1_PIN 4   // Pin untuk relay 1
#define RELAY2_PIN 5   // Pin untuk relay 2
#define LED_PIN 26     // Pin untuk LED indikator
#define ONE_WIRE_BUS 14 // Pin untuk sensor suhu DS18B20

// Variabel global
float temperatureC;
int ntu;
int read_ADC;
unsigned long previousMillis = 0;
const long interval = 300;
int currentDisplay = 0;

// Inisialisasi LCD dan sensor suhu
LiquidCrystal_I2C lcd(0x27, 16, 2);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Pesan selamat datang untuk scrolling text
String scrollingMessage = "Sistem Monitoring dan Treatment Aquarium IoT";

void setup() {
  Serial.begin(9600);

  // Inisialisasi LCD dan sensor
  lcd.init();
  lcd.backlight();
  lcd.clear();
  sensors.begin();
  
  pinMode(SENSOR_PIN, INPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Pastikan relay mati pada awal
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  // Menyambungkan ke WiFi dan Blynk
  Serial.println("Connecting to WiFi...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  while (!Blynk.connected()) {
    Serial.println("Connecting to Blynk...");
    delay(1000);
  }
  Serial.println("Connected to Blynk!");
}

void loop() {
  Blynk.run();

  // Fungsi untuk scrolling text
  scrollingText(scrollingMessage, 300);

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Membaca suhu
    sensors.requestTemperatures();
    temperatureC = sensors.getTempCByIndex(0);
    
    if (temperatureC != DEVICE_DISCONNECTED_C) {
      Serial.print("Suhu: ");
      Serial.print(temperatureC);
      Serial.println(" C");
      
      Blynk.virtualWrite(V1, temperatureC);
      
      lcd.setCursor(0, 1);
      lcd.print("Suhu: ");
      lcd.print(temperatureC);
      lcd.print(" C");
    }

    // Membaca turbidity
    read_ADC = analogRead(SENSOR_PIN);
    ntu = map(read_ADC, 1500, 2222, 300, 0);
    if (ntu < 0) ntu = 0;

    Serial.print("Nilai ADC: ");
    Serial.print(read_ADC);
    Serial.print(" | NTU: ");
    Serial.println(ntu);
    Blynk.virtualWrite(V2, ntu);

    // Menampilkan informasi di LCD
    lcd.setCursor(0, 1);
    if (currentDisplay == 0) {
      lcd.print("Keruh: ");
      lcd.print(ntu);
      lcd.print(" NTU ");
      currentDisplay = 1;
    } else {
      lcd.print("Suhu: ");
      lcd.print(temperatureC);
      lcd.print(" C   ");
      currentDisplay = 0;
    }
     if (temperatureC <= 27) {
        digitalWrite(LED_PIN, HIGH);
      } else {
        digitalWrite(LED_PIN, LOW);
      }

    // Mengontrol relay berdasarkan NTU
    if (ntu >= 250) {
      Serial.println("Air Sangat Kotor!");

      // Nyalakan Relay 1 Pompa 1 air terkuras
      Serial.println("Relay 1 ON");
      digitalWrite(RELAY1_PIN, HIGH);
      delay(50000);
      digitalWrite(RELAY1_PIN, LOW);
      Serial.println("Relay 1 OFF");

      // Nyalakan Relay 2 pompa 2 air terisi
      Serial.println("Relay 2 ON");
      digitalWrite(RELAY2_PIN, HIGH);
      delay(50000);
      digitalWrite(RELAY2_PIN, LOW);
      Serial.println("Relay 2 OFF");
    }
    delay(1000);
  }
}

// Fungsi untuk scrolling text
void scrollingText(const String &message, int delayTime) {
  static int scrollIndex = 0;
  int messageLength = message.length();
  
  lcd.setCursor(0, 0);  // Mulai dari baris pertama LCD
  for (int i = 0; i < 16; i++) {
    lcd.print(message[(scrollIndex + i) % messageLength]);
  }
  scrollIndex++;
  if (scrollIndex >= messageLength) scrollIndex = 0;
  delay(150);
}
