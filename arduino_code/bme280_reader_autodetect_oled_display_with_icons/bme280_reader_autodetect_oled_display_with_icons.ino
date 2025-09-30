#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_BME280 bme;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long lastReadTime = 0;
const unsigned long interval = 5UL * 60UL * 1000UL;
uint8_t detectedAddress = 0;

// --- Simple 16x16 icon bitmaps ---
const unsigned char icon_temp[] PROGMEM = {
  0x18, 0x00,
  0x3C, 0x00,
  0x3C, 0x00,
  0x3C, 0x00,
  0x3C, 0x00,
  0x3C, 0x00,
  0x3C, 0x00,
  0x3C, 0x00,
  0x3C, 0x00,
  0x3C, 0x00,
  0x3C, 0x00,
  0x7E, 0x00,
  0xFF, 0x00,
  0xFF, 0x00,
  0x7E, 0x00,
  0x3C, 0x00
};


const unsigned char icon_hum[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x03, 0xC0,
  0x07, 0xE0, 0x0F, 0xF0, 0x1F, 0xF8, 0x3F, 0xFC,
  0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x1F, 0xF8,
  0x0E, 0x70, 0x04, 0x20, 0x00, 0x00, 0x00, 0x00
};

const unsigned char icon_press[] PROGMEM = {
  0x00, 0x00, 0x07, 0xE0, 0x08, 0x10, 0x13, 0x88,
  0x14, 0x28, 0x10, 0x08, 0x10, 0x08, 0x7F, 0xFE,
  0x10, 0x08, 0x10, 0x08, 0x14, 0x28, 0x13, 0x88,
  0x08, 0x10, 0x07, 0xE0, 0x00, 0x00, 0x00, 0x00
};

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
  Serial.println("📡 BME280 + OLED Display with Icons");

  Wire.begin();
  Wire.setClock(100000);
  scanI2C();

  if (!bme.begin(detectedAddress)) {
    Serial.println("❌ BME280 init failed!");
    while (1);
  }
  Serial.println("✅ BME280 initialized.");

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
    Serial.println("---------------------------");

    // OLED Output
    display.clearDisplay();

    // Temp
    display.drawBitmap(0, 0, icon_temp, 16, 16, SSD1306_WHITE);
    display.setCursor(20, 0);
    display.setTextSize(2);
    display.printf("%.1fF", tempF);

    // Humidity
    display.drawBitmap(0, 22, icon_hum, 16, 16, SSD1306_WHITE);
    display.setCursor(20, 24);
    display.setTextSize(1);
    display.printf("Hum: %.1f%%", humidity);

    // Pressure
    display.drawBitmap(0, 42, icon_press, 16, 16, SSD1306_WHITE);
    display.setCursor(20, 46);
    display.printf("Pres: %.1f hPa", pressure);

    display.display();
  }

  delay(1000);
}
