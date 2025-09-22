#include <esp32-hal-ledc.h>
#include <esp32-hal.h>

#define BUZZER_PIN     21          // Set your buzzer GPIO pin here
#define BUZZER_CHANNEL 0           // LEDC channel
#define BUZZER_FREQ    2000        // Default freq for LEDC timer (not tone freq)
#define BUZZER_RES     8           // PWM resolution (8 bits)
#define TONE_DEFAULT_DELAY 80      // Recommended default tone duration in ms

// Initialize buzzer PWM channel, attach pin - call this once in setup()
void buzzerInit() {
  ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RES);
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
  ledcWriteTone(BUZZER_CHANNEL, 0);  // start silent
}

// Helper to play a single tone, frequency (Hz) and duration (ms)
void playTone(uint32_t frequency, uint32_t duration_ms) {
  if (frequency == 0 || duration_ms == 0) return;
  ledcWriteTone(BUZZER_CHANNEL, frequency);
  vTaskDelay(duration_ms / portTICK_PERIOD_MS);
  ledcWriteTone(BUZZER_CHANNEL, 0);
  vTaskDelay(20 / portTICK_PERIOD_MS); // small gap to separate tones
}

void beepStartup() {
  playTone(1000, 120);   // Mid tone
  vTaskDelay(50 / portTICK_PERIOD_MS);
  playTone(2000, 200);   // Higher tone, feels fresh/modern
}


void beepShutdown() {
  playTone(800, 100);
  vTaskDelay(50 / portTICK_PERIOD_MS);
  playTone(600, 100);
}

void beepSuccess() {
  playTone(1000, 100);
  playTone(1400, 100);
  playTone(1800, 120);
}


void beepFail() {
  playTone(800, 200);
  playTone(500, 300);
}


void beepProcess() {
  playTone(1000, 60);
  playTone(1200, 60);
}

void beepDetect() {
  playTone(750, 40);
}

void beepWarning() {
  playTone(500, 100);
}