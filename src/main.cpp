#include <ESP32QRCodeReader.h>
#include <Arduino.h>
#include <camera_pins.h>
#include <lcd.h>
#include <WiFi.h>
#include <HTTPClient.h>

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

void onQrCodeTask(void *pvParameters) {
  struct QRCodeData qrCodeData;
  // This loop will run until a valid QR code is found and processed
  while (true) {
    if (reader.receiveQrCode(&qrCodeData, 100)) {
      Serial.println("Found QRCode");
      if (qrCodeData.valid) {
        // A valid QR code was found, so we can exit the loop after processing
        // and prevent the cam_hal errors
        
        Serial.print("Payload: ");
        Serial.println((const char *)qrCodeData.payload);
        lcdPrint("Processing...");

        // Perform the HTTP GET request
        HTTPClient http;
        http.begin((const char *)qrCodeData.payload);
        http.setTimeout(10000); // 10-second timeout
        
        Serial.println("Sending HTTP GET request...");
        int httpCode = http.GET();
        
        if (httpCode > 0) {
          String payload = http.getString();
          Serial.print("HTTP Response Code: ");
          Serial.println(httpCode);
          Serial.print("HTTP Payload: ");
          Serial.println(payload);
          lcdPrint("ACCESS GRANTED");
        } else {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
          lcdPrint("ACCESS DENIED");
        }
        
        http.end();
        vTaskDelay(500 / portTICK_PERIOD_MS); // Wait 500ms
        break; 

      } else {
        Serial.print("Invalid: ");
        Serial.println((const char *)qrCodeData.payload);
        lcdPrint("Scanning...");
      }
    } 
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }

  // Once the while loop is broken, the task suspends itself
  // to save CPU and power, as its job is done.
  vTaskSuspend(NULL);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  Wire.begin(1, 2);
  lcdInit();
  lcdPrint("Starting", 0, true);

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
  reader.beginOnCore(1);
  xTaskCreate(onQrCodeTask, "onQrCode", 16 * 1024, NULL, 4, NULL);
  lcdPrint("Scan a QRCode", 0, true);
  Serial.println("Setup done");
}

void loop() {
  // Your program's main loop can be empty since FreeRTOS tasks handle all logic.
}