#include <ESP32QRCodeReader.h>
#include <Arduino.h>

// Define the correct pinout for the Freenove ESP32-S3 WROOM board
const CameraPins camPins = {
  .PWDN_GPIO_NUM = -1,
  .RESET_GPIO_NUM = -1,
  .XCLK_GPIO_NUM = 15,
  .SIOD_GPIO_NUM = 4,
  .SIOC_GPIO_NUM = 5,
  .Y9_GPIO_NUM = 16,
  .Y8_GPIO_NUM = 17,
  .Y7_GPIO_NUM = 18,
  .Y6_GPIO_NUM = 12,
  .Y5_GPIO_NUM = 10,
  .Y4_GPIO_NUM = 8,
  .Y3_GPIO_NUM = 9,
  .Y2_GPIO_NUM = 11,
  .VSYNC_GPIO_NUM = 6,
  .HREF_GPIO_NUM = 7,
  .PCLK_GPIO_NUM = 13
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