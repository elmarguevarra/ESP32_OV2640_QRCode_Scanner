#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "lcd.h"
#include "esp_camera.h"
#include "ESP32QRCodeReader.h"
#include "camera_pins.h"

// WiFi credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// LCD pins
#define LCD_SDA 1
#define LCD_SCL 2

// QR code reader
ESP32QRCodeReader qrReader;

// Forward declarations
void startCameraServer();
void qrTask(void* pvParameters);

void setup() {
  Serial.begin(115200);

  // Init I2C (only once here)
  Wire.begin(LCD_SDA, LCD_SCL);
  lcdInit();
  delay(2000);

  Serial.println("\n==================================================");
  Serial.println("ESP32-S3 Camera + QR Reader");
  Serial.println("==================================================");

  // --- System Info ---
  Serial.printf("Chip: %s Rev %d | Cores: %d | Flash: %d bytes\n",
                ESP.getChipModel(), ESP.getChipRevision(),
                ESP.getChipCores(), ESP.getFlashChipSize());

  if (psramFound()) {
    Serial.printf("✓ PSRAM found: %d bytes total, %d free\n",
                  ESP.getPsramSize(), ESP.getFreePsram());
  } else {
    Serial.println("✗ No PSRAM found!");
  }

  // --- WiFi Connection ---
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(1000);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  WiFi.setTxPower(WIFI_POWER_19_5dBm);

  Serial.print("Connecting");
  lcdScroll("Connecting to WiFi");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("✓ WiFi connected! IP: %s\n", WiFi.localIP().toString().c_str());

    // --- QR Code Reader (camera init inside) ---
    camera_config_t config = {
      .pin_pwdn     = PWDN_GPIO_NUM,
      .pin_reset    = RESET_GPIO_NUM,
      .pin_xclk     = XCLK_GPIO_NUM,
      .pin_sccb_sda = SIOD_GPIO_NUM,
      .pin_sccb_scl = SIOC_GPIO_NUM,
      .pin_d7       = Y9_GPIO_NUM,
      .pin_d6       = Y8_GPIO_NUM,
      .pin_d5       = Y7_GPIO_NUM,
      .pin_d4       = Y6_GPIO_NUM,
      .pin_d3       = Y5_GPIO_NUM,
      .pin_d2       = Y4_GPIO_NUM,
      .pin_d1       = Y3_GPIO_NUM,
      .pin_d0       = Y2_GPIO_NUM,
      .pin_vsync    = VSYNC_GPIO_NUM,
      .pin_href     = HREF_GPIO_NUM,
      .pin_pclk     = PCLK_GPIO_NUM,

      .xclk_freq_hz = 20000000,
      .ledc_timer   = LEDC_TIMER_0,
      .ledc_channel = LEDC_CHANNEL_0,
      .pixel_format = PIXFORMAT_RGB565,   // good for QR
      .frame_size   = FRAMESIZE_QVGA,     // 320x240, fast enough
      .jpeg_quality = 12,
      .fb_count     = 2,
      .fb_location  = CAMERA_FB_IN_PSRAM,
      .grab_mode    = CAMERA_GRAB_WHEN_EMPTY
    };

    qrReader.setup(&config);
    qrReader.beginOnCore(1);

    // Create QR task (bigger stack size)
    xTaskCreate(qrTask, "QRTask", 8192, NULL, 1, NULL);

    // --- Camera Streaming Server ---
    Serial.println("\n--- Starting Camera Server ---");
    startCameraServer();

    // --- LCD Info ---
    lcdPrint("System is running!", 0, true);
    lcdPrint(WiFi.localIP().toString().c_str(), 1, false);
  } else {
    Serial.println("✗ WiFi connection failed!");
    lcdPrint("WiFi Connect Fail", 0, true);
  }
}

void loop() {
  // Nothing, QR task runs in background
}

// --- QR Task ---
void qrTask(void* pvParameters) {
  QRCodeData qrCodeData;

  while (true) {
    if (qrReader.receiveQrCode(&qrCodeData, 100)) {  // wait up to 100 ms
      if (qrCodeData.valid) {
        Serial.printf("QR Code detected: %s\n", (const char*)qrCodeData.payload);

        lcdClear();
        lcdPrint("QR Code:", 0, true);
        lcdPrint((const char*)qrCodeData.payload, 1, false);
      } else {
        Serial.println("Invalid QR code");
      }
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}
