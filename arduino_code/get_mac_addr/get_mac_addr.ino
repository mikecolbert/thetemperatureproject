#include <WiFi.h>
#include "esp_wifi.h"  // For security type info

const char* ssid = "****** ADD SSID HERE *****";
const char* password = "****** ADD PASSWORD HERE *****";


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.printf("Connecting to %s...\n", ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

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

    // Print security type
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
      Serial.print("Security: ");
      switch (ap_info.authmode) {
        case WIFI_AUTH_OPEN:         Serial.println("Open"); break;
        case WIFI_AUTH_WEP:          Serial.println("WEP"); break;
        case WIFI_AUTH_WPA_PSK:      Serial.println("WPA"); break;
        case WIFI_AUTH_WPA2_PSK:     Serial.println("WPA2"); break;
        case WIFI_AUTH_WPA_WPA2_PSK: Serial.println("WPA/WPA2 Mixed"); break;
        case WIFI_AUTH_WPA2_ENTERPRISE: Serial.println("WPA2 Enterprise"); break;
        case WIFI_AUTH_WPA3_PSK:     Serial.println("WPA3"); break;
        case WIFI_AUTH_WPA2_WPA3_PSK:Serial.println("WPA2/WPA3 Mixed"); break;
        default:                     Serial.println("Unknown");
      }
    } else {
      Serial.println("⚠️ Unable to determine security type.");
    }

  } else {
    Serial.println();
    Serial.println("❌ Failed to connect to WiFi.");
  }
}

void loop() {
  // Nothing here
}
