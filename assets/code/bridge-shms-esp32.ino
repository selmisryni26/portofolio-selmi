#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <HX711.h>
#include <LiquidCrystal_I2C.h>

// === Load Cell ===
#define DOUT 18
#define CLK 19
HX711 scale;
float calibration_factor = 1000.0;

// === MPU6050 ===
Adafruit_MPU6050 mpu;

// === WiFi ===
const char* ssid = "Kontrakan Baru";
const char* password = "YOUR_WIFI_PASSWORD";

// === MQTT ===
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_topic = "kerjapraktik";

// === PHP Server ===
const char* serverUrl = "http://192.168.1.127/postdata.php";

WiFiClient espClient;
PubSubClient client(espClient);

// === LCD I2C ===
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === Fisika Material ===
const float GRAVITY = 9.81;              // m/s²
const float AREA = 0.0001;               // m² (1 cm x 1 cm)
const float YOUNG_MODULUS = 200e9;       // Pa (misal baja)

unsigned long lastSwitch = 0;
int displayMode = 0;
unsigned long lastScroll = 0;
int scrollIndex = 0;
String scrollText = "";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Menghubungkan ke WiFi...");
  }
  Serial.println("Terhubung ke WiFi");

  client.setServer(mqtt_server, mqtt_port);
  reconnectMQTT();

  if (!mpu.begin()) {
    Serial.println("MPU6050 tidak ditemukan!");
    while (1) delay(10);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor);
  scale.tare();

  lcd.init();
  lcd.backlight();
  lcd.clear();
  delay(1000);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // === Sensor MPU6050 ===
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x / 50.0;
  float ay = a.acceleration.y / 10.0;
  float az = 0.11 - (a.acceleration.z / 100.0);
  float gx = g.gyro.x;
  float gy = g.gyro.y;
  float gz = g.gyro.z;

  // === Load Cell HX711 ===
  float weight_grams = scale.get_units(5);
  float weight_mg = weight_grams * 1000.0;
  float weight_kg = weight_grams / 1000.0;
  float force_newtons = weight_kg * GRAVITY;
  float stress_pascal = force_newtons / AREA;
  float strain = stress_pascal / YOUNG_MODULUS;

  // === Serial Monitoring ===
  Serial.println("===================================");
  Serial.printf("AX: %.2f | AY: %.2f | AZ: %.2f\n", ax, ay, az);
  Serial.printf("GX: %.2f | GY: %.2f | GZ: %.2f\n", gx, gy, gz);
  Serial.printf("Berat: %.2f g | %.2f mg\n", weight_grams, weight_mg);
  Serial.printf("Gaya: %.2f N | Tegangan: %.2f Pa | Regangan: %.6f\n", force_newtons, stress_pascal, strain);

  // === Kirim ke MQTT dan HTTP ===
  kirimDataMQTT(ax, ay, az, gx, gy, gz, weight_grams, force_newtons, stress_pascal, strain);
  kirimData(ax, ay, az, gx, gy, gz, weight_grams, force_newtons, stress_pascal, strain);

  // === LCD Display Scroll ===
  if (millis() - lastSwitch > 5000) {
    displayMode = (displayMode + 1) % 3;
    lastSwitch = millis();
    scrollIndex = 0;
    lcd.clear();

    if (displayMode == 0) {
      lcd.setCursor(0, 0);
      lcd.print("Accelerometer");
      scrollText = " ax:" + String(ax, 2) + " ay:" + String(ay, 2) + " az:" + String(az, 2) + "   ";
    } else if (displayMode == 1) {
      lcd.setCursor(0, 0);
      lcd.print("Gyroscope");
      scrollText = " gx:" + String(gx, 2) + " gy:" + String(gy, 2) + " gz:" + String(gz, 2) + "   ";
    } else if (displayMode == 2) {
      lcd.setCursor(0, 0);
      lcd.print("Load Cell");
      scrollText = " Berat:" + String(weight_grams, 2) + "g Gaya:" + String(force_newtons, 2) + "N   ";
    }

    lcd.setCursor(0, 1);
    lcd.print(scrollText.substring(0, 16));
  }

  // Scrolling Text
  if (displayMode <= 2) {
    if (millis() - lastScroll > 300) {
      lastScroll = millis();
      scrollIndex++;
      if (scrollIndex > scrollText.length()) scrollIndex = 0;

      String tampil = scrollText.substring(scrollIndex) + scrollText.substring(0, scrollIndex);
      lcd.setCursor(0, 1);
      lcd.print(tampil.substring(0, 16));
    }
  }

  delay(500);
}

// === MQTT Publish ===
void kirimDataMQTT(float ax, float ay, float az, float gx, float gy, float gz,
                   float berat, float gaya, float tegangan, float regangan) {
  String payload = "{";
  payload += "\"ax\":" + String(ax, 2) + ",";
  payload += "\"ay\":" + String(ay, 2) + ",";
  payload += "\"az\":" + String(az, 2) + ",";
  payload += "\"gx\":" + String(gx, 2) + ",";
  payload += "\"gy\":" + String(gy, 2) + ",";
  payload += "\"gz\":" + String(gz, 2) + ",";
  payload += "\"berat\":" + String(berat, 2) + ",";
  payload += "\"gaya\":" + String(gaya, 2) + ",";
  payload += "\"tegangan\":" + String(tegangan, 2) + ",";
  payload += "\"regangan\":" + String(regangan, 6);
  payload += "}";

  client.publish(mqtt_topic, payload.c_str());
  Serial.println("MQTT terkirim:");
  Serial.println(payload);
}

// === HTTP POST ke PHP Server ===
void kirimData(float ax, float ay, float az, float gx, float gy, float gz,
               float berat, float gaya, float tegangan, float regangan) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "ax=" + String(ax) +
                      "&ay=" + String(ay) +
                      "&az=" + String(az) +
                      "&gx=" + String(gx) +
                      "&gy=" + String(gy) +
                      "&gz=" + String(gz) +
                      "&berat=" + String(berat) +
                      "&gaya=" + String(gaya) +
                      "&tegangan=" + String(tegangan) +
                      "&regangan=" + String(regangan);

    int httpResponseCode = http.POST(postData);
    Serial.print("HTTP Response: ");
    Serial.println(httpResponseCode);

    http.end();
  } else {
    Serial.println("WiFi tidak terhubung");
  }
}

// === MQTT Reconnect ===
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("terhubung");
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 5 detik");
      delay(5000);
    }
  }
}