#include <Wire.h>
#include <WiFi.h>
#include <time.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
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
const unsigned long interval = 15UL * 60UL * 1000UL;
uint8_t detectedAddress = 0;

// ---- Sensor Config ----
const int sensor_id = 0;  // Change this for each sensor unit

// ---- Wi-Fi & Time Config ----
const char* ssid = "30WOP";
const char* password = "Quick*Trick";
const char* ntpServer = "pool.ntp.org";
const char* tz = "CST6CDT,M3.2.0/2,M11.1.0/2";  // US Central Time with DST

// ---- Function to Get Local Time (for display) ----
String getLocalTimeString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "Time error";
  char buf[20];
  strftime(buf, sizeof(buf), "%m/%d %I:%M %p", &timeinfo);  // US-style
  return String(buf);
}

// ---- Function to Send Data to API ----
void sendToAPI(float temp, float hum, float pres) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ Not connected to WiFi, skipping API post.");
    return;
  }

  // Get UTC time in seconds since epoch
  time_t raw;
  time(&raw);

  // Convert to struct tm in UTC
  struct tm gmt;
  gmtime_r(&raw, &gmt);  // this fills gmt with UTC time

  // Format UTC as ISO-like string
  char timeBuf[25];
  strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", &gmt);

  String mac = WiFi.macAddress();
  String payload = "{";
  payload += "\"sensor_id\":\"" + String(sensor_id) + "\",";
  payload += "\"temperature_f\":" + String(temp, 1) + ",";
  payload += "\"humidity\":" + String(hum, 1) + ",";
  payload += "\"pressure\":" + String(pres, 1) + ",";
  payload += "\"read_time\":\"" + String(timeBuf) + "\"";
  payload += "}";

  Serial.println("📤 Sending to API:");
  Serial.println(payload);

  WiFiClientSecure client;
  client.setInsecure();  // Use with caution (OK for testing)

  HTTPClient https;
  https.begin(client, "https://thetemperatureproject.com/api/v1/temperatures");
  https.addHeader("Content-Type", "application/json");

  int httpCode = https.POST(payload);

  if (httpCode > 0) {
    Serial.printf("✅ API Response: %d\n", httpCode);
    String response = https.getString();
    Serial.println(response);
  } else {
    Serial.printf("❌ POST failed, error: %s\n", https.errorToString(httpCode).c_str());
  }

  https.end();
}

// ---- I2C Scan to Find BME280 ----
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

// ---- Setup ----
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("📡 BME280 + OLED + Time + API");

  // ---- Wi-Fi ----
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi connected");

  // ---- NTP Time ----
  configTzTime(tz, ntpServer);
  Serial.println("🕒 NTP time sync configured");

  // ---- Sensor and Display ----
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

// ---- Main Loop ----
void loop() {
  unsigned long now = millis();
  if (now - lastReadTime >= interval || lastReadTime == 0) {
    lastReadTime = now;

    float tempC = bme.readTemperature();
    float tempF = (tempC * 1.8) + 32;
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;

    // Serial output
    Serial.println("🌡️ Environmental Readings:");
    Serial.printf("Temperature: %.2f °C / %.2f °F\n", tempC, tempF);
    Serial.printf("Humidity:    %.2f %%\n", humidity);
    Serial.printf("Pressure:    %.2f hPa\n", pressure);
    Serial.printf("Timestamp:   %s\n", getLocalTimeString().c_str());
    Serial.println("---------------------------");

    // OLED output
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Temp:");
    display.setTextSize(1);
    display.setCursor(40, 0);
    display.printf("%.1fF", tempF);

    display.setTextSize(1);
    display.setCursor(0, 18);
    display.print("Hum:");
    display.setCursor(40, 18);
    display.printf("%.1f%%", humidity);

    display.setCursor(0, 36);
    display.print("Pres:");
    display.setCursor(40, 36);
    display.printf("%.1f hPa", pressure);

    display.setCursor(0, 54);
    display.print("Time:");
    display.setCursor(40, 54);
    display.print(getLocalTimeString());

    display.display();

    // Send to cloud API
    sendToAPI(tempF, humidity, pressure);
  }

  delay(1000);
}