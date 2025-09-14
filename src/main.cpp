#include <ESP32QRCodeReader.h>
#include <Arduino.h>
#include <camera_pins.h>
#include <lcd.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

// Define the correct pinout for the Freenove ESP32-S3 WROOM board
const CameraPins camPins = {
  .PWDN_GPIO_NUM = PWDN_GPIO_NUM,
  .RESET_GPIO_NUM = RESET_GPIO_NUM,
  .XCLK_GPIO_NUM = XCLK_GPIO_NUM,
  .SIOD_GPIO_NUM = SIOD_GPIO_NUM,
  .SIOC_GPIO_NUM = SIOC_GPIO_NUM,
  .Y9_GPIO_NUM = Y9_GPIO_NUM,
  .Y8_GPIO_NUM = Y8_GPIO_NUM,
  .Y7_GPIO_NUM = Y7_GPIO_NUM,
  .Y6_GPIO_NUM = Y6_GPIO_NUM,
  .Y5_GPIO_NUM = Y5_GPIO_NUM,
  .Y4_GPIO_NUM = Y4_GPIO_NUM,
  .Y3_GPIO_NUM = Y3_GPIO_NUM,
  .Y2_GPIO_NUM = Y2_GPIO_NUM,
  .VSYNC_GPIO_NUM = VSYNC_GPIO_NUM,
  .HREF_GPIO_NUM = HREF_GPIO_NUM,
  .PCLK_GPIO_NUM = PCLK_GPIO_NUM
};

// Instantiate the reader with the custom pinout
ESP32QRCodeReader reader(camPins);

// Declare the queue handle for communication between tasks
QueueHandle_t urlQueue;

// Task to handle HTTP requests
void httpTask(void *pvParameters) {
  char url[256];
  while (true) {
    // Wait for a URL from the queue (this blocks until a URL is available)
    if (xQueueReceive(urlQueue, &url, portMAX_DELAY) == pdPASS) {
      Serial.println("Received URL from queue. Sending HTTP request...");
      lcdPrint("Processing...");

      HTTPClient http;
      http.begin(url);
      http.setTimeout(10000); // 10-second timeout

      int httpCode = http.GET();
      
      if (httpCode > 0) {
        String payload = http.getString();
        Serial.printf("HTTP Response Code: %d\n", httpCode);
        Serial.printf("HTTP Payload: %s\n", payload.c_str());
        lcdPrint("ACCESS GRANTED");
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        lcdPrint("ACCESS DENIED");
      }
      http.end();
      // Wait for a moment to let the user see the result on the LCD
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      lcdPrint("Scan a QRCode");
    }
  }
}

// Task to handle QR code scanning
void qrCodeTask(void *pvParameters) {
  struct QRCodeData qrCodeData;
  while (true) {
    // The reader is non-blocking here; it returns true only if a QR code is found
    if (reader.receiveQrCode(&qrCodeData, 100)) {
      Serial.println("Found QRCode");
      if (qrCodeData.valid) {
        Serial.printf("Payload: %s\n", (const char *)qrCodeData.payload);
        
        // Send the URL to the HTTP task for processing
        // Use a short timeout to prevent this task from blocking
        xQueueSend(urlQueue, (const char *)qrCodeData.payload, 100 / portTICK_PERIOD_MS);
        
      } else {
        Serial.println("Invalid QR code detected.");
      }
    } else {
        // This is where you would display "Scan a QR Code" if no code is found
        // However, since the httpTask already manages the LCD for the 'Scanning' state,
        // we won't add it here to avoid race conditions.
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  Wire.begin(1, 2);
  lcdInit();
  lcdPrint("Starting...", 0, true);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int dotCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.setCursor((8 + dotCount) % 16, 0);
    lcd.print(".");
    dotCount++;
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  reader.setup();
  reader.beginOnCore(1); // Run on core 1 for performance

  // Create the FreeRTOS queue to pass URLs
  urlQueue = xQueueCreate(10, 256); // A queue that can hold 10 URLs, each up to 256 chars

  // Create the two tasks
  // The QR code task gets a higher priority to ensure it's responsive
  xTaskCreate(qrCodeTask, "QR_Task", 16 * 1024, NULL, 5, NULL); 
  // The HTTP task gets a slightly lower priority as it can afford to be slow
  xTaskCreate(httpTask, "HTTP_Task", 16 * 1024, NULL, 4, NULL); 

  lcdPrint("Scan a QRCode");
  Serial.println("Setup done");
}

void loop() {
  // Your program's main loop can be empty since FreeRTOS tasks handle all logic.
}