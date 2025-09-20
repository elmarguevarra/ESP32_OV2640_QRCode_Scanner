#include <esp32-hal-ledc.h>
#include <esp32-hal.h>

void beepStartup() {
  int tones[3] = {800, 1200, 1600};
  for (int i = 0; i < 3; i++) {
    ledcWriteTone(0, tones[i]);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  ledcWriteTone(0, 0);
}

void beepSuccess() {
  // Two-tone ascending success beep
  ledcWriteTone(0, 1800);
  vTaskDelay(100 / portTICK_PERIOD_MS);
  ledcWriteTone(0, 2200);
  vTaskDelay(100 / portTICK_PERIOD_MS);
  ledcWriteTone(0, 0); // stop
}

void beepDenied() {
  // Low buzz with a short higher tone at the end
  ledcWriteTone(0, 350);
  vTaskDelay(300 / portTICK_PERIOD_MS);
  ledcWriteTone(0, 400);
  vTaskDelay(150 / portTICK_PERIOD_MS);
  ledcWriteTone(0, 0); // stop
}

void beepScanned() {
  ledcWriteTone(0, 1000);  // 1kHz tone
  delay(100);              // 100 ms
  ledcWriteTone(0, 0);     // stop
}

void beepDetect() {
  ledcWriteTone(0, 750);   // 600 Hz low-mid tone
  vTaskDelay(40 / portTICK_PERIOD_MS);
  ledcWriteTone(0, 0);
}