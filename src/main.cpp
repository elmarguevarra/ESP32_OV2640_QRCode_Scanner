#include <ESP32QRCodeReader.h>
#include <Arduino.h>
#include <camera_pins.h>

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

void onQrCodeTask(void *pvParameters)
{
  struct QRCodeData qrCodeData;

  while (true)
  {
    if (reader.receiveQrCode(&qrCodeData, 100))
    {
      Serial.println("Found QRCode");
      if (qrCodeData.valid)
      {
        Serial.print("Payload: ");
        Serial.println((const char *)qrCodeData.payload);
      }
      else
      {
        Serial.print("Invalid: ");
        Serial.println((const char *)qrCodeData.payload);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  reader.setup();
  reader.beginOnCore(1);
  xTaskCreate(onQrCodeTask, "onQrCode", 4 * 1024, NULL, 4, NULL);
}

// Add this empty loop() function to satisfy the Arduino framework
void loop() {
  // Your program's main loop. Since you're using a separate FreeRTOS task,
  // this function can be empty.
}