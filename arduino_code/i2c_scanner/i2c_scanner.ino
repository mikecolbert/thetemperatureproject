#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nI2C Scanner");

  byte error, address;
  int count = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("✅ I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      count++;
    }
  }

  if (count == 0) Serial.println("❌ No I2C devices found");
  else Serial.println("✅ Done.");
}

void loop() {
  // Nothing here
}
