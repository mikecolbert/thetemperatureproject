#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_BME280 bme;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);  // -1 for no reset pin

unsigned long lastReadTime = 0;
const unsigned long interval = 5UL * 60UL * 1000UL;  // 5 minutes
uint8_t detectedAddress = 0;

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
  Serial.println("📡 BME280 + OLED Test");

  Wire.begin();
  scanI2C();

  if (!bme.begin(detectedAddress)) {
    Serial.println("❌ BME280 init failed!");
    while (1);
  }
  Serial.println("✅ BME280 initialized.");

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // 0x3C is common OLED address
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

    // Serial output
    Serial.println("🌡️ Environmental Readings:");
    Serial.printf("Temperature: %.2f °C / %.2f °F\n", tempC, tempF);
    Serial.printf("Humidity:    %.2f %%\n", humidity);
    Serial.printf("Pressure:    %.2f hPa\n", pressure);
    Serial.println("---------------------------");

    // OLED display output
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.printf("Temp: %.1f C / %.1f F\n", tempC, tempF);
    display.printf("Humidity: %.1f %%\n", humidity);
    display.printf("Pressure: %.1f hPa\n", pressure);
    display.display();
  }

  delay(1000);
}
