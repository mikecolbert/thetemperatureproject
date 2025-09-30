#include <Wire.h>
#include <WiFi.h>
#include <time.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---- Display & Sensor Config ----
#define SEALEVELPRESSURE_HPA (1013.25)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_BME280 bme;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---- Timing ----
unsigned long lastReadTime = 0;
const unsigned long interval = 5UL * 60UL * 1000UL;
uint8_t detectedAddress = 0;

// ---- Wi-Fi & Time Config ----
const char* ssid = "****** ADD SSID HERE *****";
const char* password = "****** ADD PASSWORD HERE *****";
const char* ntpServer = "pool.ntp.org";
const char* tz = "CST6CDT,M3.2.0/2,M11.1.0/2";  // US Central Time with DST

// ---- Function to Get Local Time String ----
String getLocalTimeString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "Time error";
  char buf[20];
  strftime(buf, sizeof(buf), "%m/%d %I:%M %p", &timeinfo);  // US-style
  return String(buf);
}

void scanI2C() {
  Serial.println("🔍 Scanning I2C bus...");
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("✅ I2C device at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      if (address == 0x76 || address == 0x77) {
        detectedAddress = address;
        Serial.println("🎯 Possible BME280 detected.");
      }
    }
  }
  if (detectedAddress == 0) {
    Serial.println("❌ No BME280 found. Halting.");
    while (1);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("📡 BME280 + OLED Display + Time");

  // ---- Wi-Fi Connection ----
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi connected");

  // ---- Time Setup ----
  configTzTime(tz, ntpServer);
  Serial.println("🕒 NTP time sync configured");

  // ---- Sensor and Display Setup ----
  Wire.begin();
  Wire.setClock(100000);
  scanI2C();

  if (!bme.begin(detectedAddress)) {
    Serial.println("❌ BME280 init failed!");
    while (1);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
    Serial.println("❌ SSD1306 OLED not found!");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("🌡️ Sensor Booted!");
  display.display();
  delay(2000);
}

void loop() {
  unsigned long now = millis();
  if (now - lastReadTime >= interval || lastReadTime == 0) {
    lastReadTime = now;

    float tempC = bme.readTemperature();
    float tempF = (tempC * 1.8) + 32;
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;

    // Serial Output
    Serial.println("🌡️ Environmental Readings:");
    Serial.printf("Temperature: %.2f °C / %.2f °F\n", tempC, tempF);
    Serial.printf("Humidity:    %.2f %%\n", humidity);
    Serial.printf("Pressure:    %.2f hPa\n", pressure);
    Serial.printf("Timestamp:   %s\n", getLocalTimeString().c_str());
    Serial.println("---------------------------");

    // OLED Output
    display.clearDisplay();

    // Temperature
    display.setCursor(20, 0);
    display.setTextSize(2);
    display.printf("%.1fF", tempF);

    // Humidity
    display.setCursor(20, 24);
    display.setTextSize(1);
    display.printf("Hum: %.1f%%", humidity);

    // Pressure
    display.setCursor(20, 39);
    display.setTextSize(1);
    display.printf("Pres: %.1f hPa", pressure);

    // Timestamp
    display.setCursor(20, 54);
    display.setTextSize(1);
    display.print(getLocalTimeString());

    display.display();
  }

  delay(1000);
}

