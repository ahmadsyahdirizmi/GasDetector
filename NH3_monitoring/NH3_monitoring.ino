#define BLYNK_TEMPLATE_ID "TMPL6iDxlhKjV"
#define BLYNK_TEMPLATE_NAME "GoKillS"
#define BLYNK_AUTH_TOKEN "mRtMAcsmHVG0-aDHkHD8R9ymWrcMaX8d"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <driver/ledc.h> // Pustaka LEDC untuk ESP32
#include <HTTPClient.h>  // Library untuk HTTP client
#include <ArduinoJson.h>
#include <math.h> // Untuk fungsi log dan pow

// Blynk Auth Token
char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials
char ssid[] = "Ayam Pedas";
char pass[] = "rizmisyahdi";

// MQ135 sensor pin
#define MQ135_PIN 34
#define Vc 5.0 // Tegangan sumber (5V)

// Buzzer pin
#define BUZZER_PIN 4

// LED RGB pins
#define LED_RED_PIN 25
#define LED_GREEN_PIN 26
#define LED_BLUE_PIN 27

// LCD I2C address and dimensions
LiquidCrystal_I2C lcd(0x27, 16, 2);

BlynkTimer timer;

// Batas aman PPM
#define SAFE_PPM_LIMIT 25

// Nama perangkat ESP32
const char* deviceName = "B43";

// Variabel untuk mengontrol waktu buzzer
unsigned long startTime = 0; // Waktu mulai penyalakan buzzer
bool buzzerActive = false;   // Status buzzer aktif atau tidak
bool conditionChanged = false; // Flag untuk melacak perubahan kondisi

// HTTP API untuk Telegram
const char* telegramBotToken = "6558496248:AAG08hBSZQVpi0ys7SnzvtsHvX2YwP2UmOU"; // Ganti dengan token bot Telegram Anda
const char* telegramChatId = "7011787366"; // Ganti dengan chat ID Anda

// Function to read MQ135 sensor and calculate PPM
float readMQ135() {
  int sensorValue = analogRead(MQ135_PIN);
  // Konversi nilai analog (0-4095) ke tegangan (0-5V)
  float Vout = sensorValue * (Vc / 4095.0);

  // Asumsikan nilai Ro yang diukur pada lingkungan Anda sendiri
  float Ro = 10; // Ganti dengan nilai Ro yang sesuai dengan lingkungan Anda

  // Hitung Rs
  float sensorResistance = ((Vc / Vout) - 1.0) * Ro;

  // Gunakan persamaan logaritmik untuk menghitung PPM
  float ratio = sensorResistance / Ro;
  float m = -0.417;
  float b = log10(2); // log(2) karena rasio Rs/Ro = 3.6 di udara bersih
  float ppm = pow(10, (log10(ratio) - b) / m);
  
  return ppm;
}

// Function to control LED RGB color
void setLedColor(int red, int green, int blue) {
  analogWrite(LED_RED_PIN, red);
  analogWrite(LED_GREEN_PIN, green);
  analogWrite(LED_BLUE_PIN, blue);
}

// Function to send data to Blynk, update LCD, and Serial Monitor
void sendSensorData() {
  float ppm = readMQ135();

  // Send data to Blynk
  Blynk.virtualWrite(V0, ppm);
  Blynk.virtualWrite(V2, ppm);

  // Update LCD display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("NH3: ");
  lcd.print(ppm);
  lcd.print(" ppm");
  
  // Determine status based on ppm
  String status;
  if (ppm < SAFE_PPM_LIMIT) {
    status = "bersih";
    conditionChanged = true; // Set conditionChanged to true when the air is clean
    setLedColor(0, 255, 0); // Set LED to green
    sendStatus(status);
  } else {
    status = "kotor";
    setLedColor(255, 0, 0); // Set LED to red
    sendStatus(status);
    if (conditionChanged && !buzzerActive) { // Only activate buzzer if the condition has changed to "kotor"
      ledcWriteTone(0, 1000); // Nyalakan buzzer jika belum aktif
      startTime = millis();   // Catat waktu mulai penyalakan buzzer
      buzzerActive = true;    // Set status buzzer aktif
      conditionChanged = false; // Reset conditionChanged since the buzzer has been activated
      // Send Telegram message
      sendMessageTelegram(ppm);
    }
  }
  
  // Print data to Serial Monitor
  Serial.print("NH3 PPM: ");
  Serial.print(ppm);
  Serial.print(" ppm | Status: ");
  Serial.println(status);

  // Matikan buzzer setelah 15 detik
  if (buzzerActive && millis() - startTime >= 15000) { // 15000 ms = 15 detik
    ledcWriteTone(0, 0);   // Matikan buzzer setelah 15 detik
    buzzerActive = false;  // Set status buzzer tidak aktif
  }
}

void sendStatus(String status) {
  // Send status to Blynk
  Blynk.virtualWrite(V1, status);
  // Update LCD display
  lcd.setCursor(0, 1);
  lcd.print("Status: ");
  lcd.print(status);
}

void sendMessageTelegram(float ppm) {
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + String(telegramBotToken) + "/sendMessage";

  // Buat objek JSON untuk pesan
  StaticJsonDocument<200> doc;
  doc["chat_id"] = telegramChatId;
  doc["text"] = "Wc di lokasi " + String(deviceName) + " memerlukan perhatian, konsentrasi amonia terdeteksi " + String(ppm) + " ppm.\n\nStatus: kotor.";

  // Serialize JSON ke String
  String jsonPayload;
  serializeJson(doc, jsonPayload);

  // Mengatur header HTTP
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  // Kirim HTTP POST request dengan payload JSON
  int httpResponseCode = http.POST(jsonPayload);

  // Tanggapan HTTP
  if (httpResponseCode > 0) {
    Serial.print("Telegram message sent successfully. Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error sending Telegram message. HTTP error code: ");
    Serial.println(httpResponseCode);
  }

  // Menutup koneksi
  http.end();
}

void setup() {
  // Debug console
  Serial.begin(115200);

  // Blynk setup
  Blynk.begin(auth, ssid, pass);

  // LCD setup
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("GoKillS");
  lcd.setCursor(0, 1);
  lcd.print("Ajibb Gann");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WC location: ");
  lcd.print(deviceName);
  lcd.setCursor(0, 1);
  lcd.print("Connecting...");

  // Konfigurasi LEDC untuk menghasilkan bunyi dengan buzzer
  ledcSetup(0, 5000, 8);   // Konfigurasi channel LEDC dengan frekuensi 5kHz dan resolusi 8-bit
  ledcAttachPin(BUZZER_PIN, 0);  // Hubungkan pin buzzer ke channel LEDC

  // Setup pin LED RGB sebagai output
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);

  // Timer setup to send data every second
  timer.setInterval(1000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
