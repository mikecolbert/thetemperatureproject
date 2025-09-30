#include <WiFi.h>

// Wi-Fi credentials
const char* ssid = "**** ADD SSID HERE ****";
const char* password = "**** ADD PASSWORD HERE ****";

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Set Wi-Fi mode to station (client)
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✅ WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC address: ");
    Serial.println(WiFi.macAddress());
  } else {
    Serial.println();
    Serial.println("❌ Failed to connect to WiFi.");
  }
}

void loop() {
  // Nothing to do in the loop
}
