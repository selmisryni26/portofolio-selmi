#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>

// Inisialisasi LCD I2C
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

const char* ssid = "OPPO A31";
const char* password = "halonisa";
// const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;

// Topik MQTT
const char* topic_publish = "kel11/kadarCO"; // Topik untuk mengirim pesan

unsigned long previousMillis = 0;
const long interval = 300;  // Interval pengiriman data

WiFiClient espClient;
PubSubClient client(espClient);

// Pesan scrolling untuk LCD
String scrollingMessage = "Monitoring dan Kendali Kadar CO  ";

// Fungsi untuk menghubungkan ke WiFi
void setupWiFi() {
  delay(10);
  Serial.println("\nConnecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Fungsi untuk menghubungkan ke broker MQTT
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker!");
    } else {
      Serial.print("Failed to connect, rc=");
      Serial.print(client.state());
      Serial.println("\nRetrying in 5 seconds...");
      delay(5000);  // Tunggu sebelum mencoba lagi
    }
  }
}

// Inisialisasi PIN
int Gas_analog = 34;
int relaykip = 16;
int relaypom = 17;

void setup() {
  Serial.begin(115200);
  setupWiFi();
  client.setServer(mqtt_server, mqtt_port);

  lcd.init();  // Inisialisasi LCD
  lcd.backlight();  // Menyalakan backlight LCD
  lcd.clear();  // Menghapus tampilan LCD

  pinMode(Gas_analog, INPUT);
  pinMode(relaykip, OUTPUT);
  pinMode(relaypom, OUTPUT);
}

void loop() {
  int gassensorAnalog = analogRead(Gas_analog);

  // Fungsi untuk scrolling text
  scrollingText(scrollingMessage, 300);

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Cek koneksi MQTT dan lakukan reconnect jika diperlukan
    if (!client.connected()) {
      reconnectMQTT();
    }
    client.loop();

    // Mengonversi nilai sensor gas menjadi ppm
    int ppm = map(gassensorAnalog, 0, 1023, 0, 1000);

    // Menyiapkan payload yang akan dikirim
    char payload1[50];
    snprintf(payload1, sizeof(payload1), "%d", ppm);  // Mengirimkan nilai ppm

    // Mengirimkan data ke topik MQTT
    Serial.print("Payload yang dikirim ke MQTT: ");
    Serial.println(payload1);
    client.publish(topic_publish, payload1);

    // Menampilkan data di LCD
    lcd.clear();
    lcd.setCursor(0, 0);  // Baris pertama untuk scrolling text
    scrollingText(scrollingMessage, 300);

    lcd.setCursor(0, 1);  // Baris kedua untuk nilai ppm
    lcd.print("Kadar: ");
    lcd.print(ppm);
    lcd.print(" ppm");

    // Pengaturan relay berdasarkan pembacaan sensor
    if (gassensorAnalog >= 200) {
  Serial.println("Gas Terdeteksi (Tingkat Tinggi)");
  digitalWrite(relaykip, HIGH); // Kipas menyala
  digitalWrite(relaypom, HIGH); // Pompa menyala
} else if (gassensorAnalog > 100 && gassensorAnalog <=199) {
  Serial.println("Gas Terdeteksi (Tingkat Rendah)");
  digitalWrite(relaykip, HIGH); // Kipas menyala
  digitalWrite(relaypom, LOW);  // Pompa mati
} else {
  Serial.println("No Gas");
  digitalWrite(relaykip, LOW);  // Kipas mati
  digitalWrite(relaypom, LOW);  // Pompa mati
}
  }

  delay(200);  // Mengurangi jeda agar pengiriman data lebih cepat
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
  delay(delayTime);
}
