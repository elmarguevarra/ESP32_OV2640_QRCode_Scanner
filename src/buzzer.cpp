#include <esp32-hal-ledc.h>
#include <esp32-hal.h>

void beepSuccess() {
  ledcWriteTone(0, 2000);  // 2kHz tone
  vTaskDelay(200 / portTICK_PERIOD_MS);
  ledcWriteTone(0, 0);     // stop
}

void beepDenied() {
  ledcWriteTone(0, 400);   // 400Hz low buzz
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ledcWriteTone(0, 0);     // stop
}

void beepScanned() {
  ledcWriteTone(0, 1000);  // 1kHz tone
  delay(100);              // 100 ms
  ledcWriteTone(0, 0);     // stop
}

void beepDetect() {
  ledcWriteTone(0, 600);   // 600 Hz low-mid tone
  vTaskDelay(100 / portTICK_PERIOD_MS);
  ledcWriteTone(0, 0);
}