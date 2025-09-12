#include <WiFi.h>

// Simple WiFi scanner - replace main.cpp content with this temporarily
// to see what networks your ESP32 can detect

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("ESP32-S3 WiFi Scanner");
  Serial.println("=====================");
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.println("Scanning for WiFi networks...");
  
  int n = WiFi.scanNetworks();
  Serial.printf("Found %d networks:\n\n", n);
  
  if (n == 0) {
    Serial.println("No networks found!");
  } else {
    Serial.println("Nr | SSID                        | RSSI | Encryption");
    Serial.println("---|-----------------------------|----- |-----------");
    
    for (int i = 0; i < n; ++i) {
      Serial.printf("%2d | %-27s | %4d | ", 
                    i + 1, 
                    WiFi.SSID(i).c_str(), 
                    WiFi.RSSI(i));
      
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN: Serial.println("Open"); break;
        case WIFI_AUTH_WEP: Serial.println("WEP"); break;
        case WIFI_AUTH_WPA_PSK: Serial.println("WPA"); break;
        case WIFI_AUTH_WPA2_PSK: Serial.println("WPA2"); break;
        case WIFI_AUTH_WPA_WPA2_PSK: Serial.println("WPA/WPA2"); break;
        case WIFI_AUTH_WPA2_ENTERPRISE: Serial.println("WPA2-Enterprise"); break;
        default: Serial.println("Unknown"); break;
      }
    }
  }
  
  Serial.println("\nNOTE: Only 2.4GHz networks will work with ESP32!");
  Serial.println("Look for networks without '5G' in the name.");
}

void loop() {
  delay(5000);
  Serial.println("\nRescanning...");
  setup(); // Rescan every 5 seconds
}