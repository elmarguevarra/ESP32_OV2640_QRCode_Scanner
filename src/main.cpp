#include <ESP32QRCodeReader.h>
#include <Arduino.h>
#include <camera_pins.h>
#include <lcd.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "buzzer.h"
#include <esp_sleep.h> 

// ---------------------- CONFIG ----------------------
#define QR_DETECT_TIMEOUT_MS    100     // max wait for QR code from camera
#define QR_TASK_SLEEP_MS        100     // main loop delay when idle
#define ENQUEUE_TIMEOUT_MS      500     // max wait to enqueue URL
#define QR_DEBOUNCE_MS          10000   // ignore same QR for 10s
#define INVALID_DEBOUNCE_MS     3000    // ignore repeated invalid QR for 3s
#define RESULT_DISPLAY_MS       2000    // show ACCESS GRANTED/DENIED
#define POST_PROCESS_COOLDOWN   2000    // extra cooldown before re-enabling scan
#define NO_QR_CLEAR_DELAY       500     // wait 500ms of no detection before clearing
#define FLUSH_BUFFER_DELAY_MS   100     // delay between flushing camera frames
#define BUZZER_PIN              21      // GPIO pin for buzzer
#define SHUTDOWN_AFTER_MS       30000   // shutdown after inactivity
#define LCD_QUEUE_TIMEOUT_MS    100     // max wait to enqueue LCD message
#define RESTART_BUTTON_PIN      14      // Using GPIO 14 as our button input

// ---------------------- CAMERA CONFIG ----------------------
const CameraPins camPins = {
  .PWDN_GPIO_NUM  = PWDN_GPIO_NUM,
  .RESET_GPIO_NUM = RESET_GPIO_NUM,
  .XCLK_GPIO_NUM  = XCLK_GPIO_NUM,
  .SIOD_GPIO_NUM  = SIOD_GPIO_NUM,
  .SIOC_GPIO_NUM  = SIOC_GPIO_NUM,
  .Y9_GPIO_NUM    = Y9_GPIO_NUM,
  .Y8_GPIO_NUM    = Y8_GPIO_NUM,
  .Y7_GPIO_NUM    = Y7_GPIO_NUM,
  .Y6_GPIO_NUM    = Y6_GPIO_NUM,
  .Y5_GPIO_NUM    = Y5_GPIO_NUM,
  .Y4_GPIO_NUM    = Y4_GPIO_NUM,
  .Y3_GPIO_NUM    = Y3_GPIO_NUM,
  .Y2_GPIO_NUM    = Y2_GPIO_NUM,
  .VSYNC_GPIO_NUM = VSYNC_GPIO_NUM,
  .HREF_GPIO_NUM  = HREF_GPIO_NUM,
  .PCLK_GPIO_NUM  = PCLK_GPIO_NUM
};

// ---------------------- GLOBAL OBJECTS ----------------------
ESP32QRCodeReader reader(camPins);

// Queues
QueueHandle_t urlQueue;
QueueHandle_t lcdQueue;

// State flags
volatile bool processingLock = false;  // true while HTTP task is working
volatile bool scanCooldown   = false;  // true after HTTP result, before new scan

// Payload tracking
char lastPayload[256] = {0};
unsigned long lastDetectMs = 0;
unsigned long lastInvalidMs = 0;  
unsigned long lastSeenQrMs = 0;

// ---------------------- MESSAGE STRUCTS ----------------------
struct LcdMessage {
  char text[32];
  uint8_t line;
  bool clearFirst;
};

struct UrlMessage {
  char url[256];
};

// ---------------------- FUNCTIONS ----------------------
void flushCameraBuffer();

// ---------------------- SHUTDOWN TASK ----------------------
void shutdownTask(void *pvParameters) {
    
    while (true) {
        unsigned long now = millis();
        unsigned long elapsedTime = now - lastSeenQrMs;
        unsigned long remainingTime = SHUTDOWN_AFTER_MS - elapsedTime;
        unsigned long remainingSeconds = remainingTime / 1000;
        if(remainingSeconds <= 10) {
          beepWarning();
          Serial.printf("Time to shutdown: %lu seconds\n", remainingSeconds);
        }

        if (elapsedTime > SHUTDOWN_AFTER_MS) {
            Serial.println("Shutdown: time expired. Entering deep sleep.");

            LcdMessage shutdownMsg = {"Shutting down...", 0, true};
            xQueueSend(lcdQueue, &shutdownMsg, LCD_QUEUE_TIMEOUT_MS / portTICK_PERIOD_MS);

            // 3. Wait for a moment so the user can see the message
            vTaskDelay(2000 / portTICK_PERIOD_MS);

            LcdMessage offMsg = {"", 0, true};
            xQueueSend(lcdQueue, &offMsg, 0);
            beepShutdown();

            vTaskDelay(100 / portTICK_PERIOD_MS);
            
            // Place the wake-up code here before going to sleep
            const gpio_num_t wakeUpPin = GPIO_NUM_14; // GPIO pin for button
            esp_sleep_enable_ext1_wakeup(1ULL << wakeUpPin, ESP_EXT1_WAKEUP_ANY_HIGH);

            // Go into deep sleep
            esp_deep_sleep_start();
        }
        
        // This task doesn't need to run often, so sleep for a while
        vTaskDelay(1000 / portTICK_PERIOD_MS); 
    }
}

// ---------------------- LCD TASK ----------------------
void lcdTask(void *pvParameters) {
  LcdMessage msg;
  while (true) {
    if (xQueueReceive(lcdQueue, &msg, portMAX_DELAY) == pdPASS) {
      if (strlen(msg.text) == 0) {
        // This is our special "turn off" message
        lcd.noDisplay();
        lcd.noBacklight();
        Serial.println("LCD: Turned off.");
      } else {
        if (msg.clearFirst) lcdClear();
        lcdPrint(msg.text, msg.line, false);
      }
    }
  }
}

// ---------------------- HTTP TASK ----------------------
void httpTask(void *pvParameters) {
  UrlMessage urlMsg;
  while (true) {
    if (xQueueReceive(urlQueue, &urlMsg, portMAX_DELAY) == pdPASS) {
      Serial.printf("HTTP: processing URL -> %s\n", urlMsg.url);

      // LCD feedback
      LcdMessage msg = {"processing...", 1, false};
      xQueueSend(lcdQueue, &msg, LCD_QUEUE_TIMEOUT_MS / portTICK_PERIOD_MS);

      HTTPClient http;
      http.begin(urlMsg.url);
      http.setTimeout(10000);

      int httpCode = http.GET();
      if (httpCode > 0) {
        String payload = http.getString();
        Serial.printf("HTTP %d, payload: %s\n", httpCode, payload.c_str());
        strcpy(msg.text, "ACCESS GRANTED");
        beepSuccess();
      } else {
        Serial.printf("HTTP failed: %s\n", http.errorToString(httpCode).c_str());
        strcpy(msg.text, "ACCESS DENIED");
        beepFail();
      }
      msg.line = 1;
      msg.clearFirst = true;
      xQueueSend(lcdQueue, &msg, LCD_QUEUE_TIMEOUT_MS / portTICK_PERIOD_MS);
      http.end();

      // Result visible
      vTaskDelay(RESULT_DISPLAY_MS / portTICK_PERIOD_MS);

      // Enter cooldown
      scanCooldown = true;
      vTaskDelay(POST_PROCESS_COOLDOWN / portTICK_PERIOD_MS);

      // Flush old frames or wait till no QR detected
      flushCameraBuffer();

      scanCooldown = false;

      // Unlock for next scan
      processingLock = false;
      Serial.println("HTTP: finished, ready for next scan.");

      // Prompt
      beepStartup();
      strcpy(msg.text, "[Scan QRCode]");
      msg.line = 0;
      msg.clearFirst = true;
      xQueueSend(lcdQueue, &msg, LCD_QUEUE_TIMEOUT_MS / portTICK_PERIOD_MS);
    }
  }
}

// ---------------------- QR TASK ----------------------
void qrCodeTask(void *pvParameters) {
  QRCodeData qrCodeData;
  while (true) {
    if (!processingLock && !scanCooldown) {
      if (reader.receiveQrCode(&qrCodeData, QR_DETECT_TIMEOUT_MS)) {
        unsigned long now = millis();
        lastSeenQrMs = now; 
        LcdMessage lm = {"scanning...", 1, false};
        xQueueSend(lcdQueue, &lm, LCD_QUEUE_TIMEOUT_MS / portTICK_PERIOD_MS);
        beepDetect();
    
        // ---------- Debounce check (valid + invalid) ----------
        if (qrCodeData.valid) {
          beepProcess();
          const char *payload = (const char *)qrCodeData.payload;

          if (strcmp(payload, lastPayload) == 0 && (now - lastDetectMs < QR_DEBOUNCE_MS)) {
            Serial.println("QR: duplicate valid QR ignored (debounce).");
            continue;
          }
        } else {
          if (now - lastInvalidMs < INVALID_DEBOUNCE_MS) {
            Serial.println("QR: duplicate invalid QR ignored (debounce).");
            continue;
          }
        }
        
        // ---------- Handle QR after debounce ----------
        if (qrCodeData.valid) {
          const char *payload = (const char *)qrCodeData.payload;

          UrlMessage urlMsg;
          strncpy(urlMsg.url, payload, sizeof(urlMsg.url) - 1);
          urlMsg.url[sizeof(urlMsg.url) - 1] = '\0';

          if (xQueueSend(urlQueue, &urlMsg, ENQUEUE_TIMEOUT_MS / portTICK_PERIOD_MS) == pdPASS) {
            processingLock = true;  // lock until HTTP finishes
            strncpy(lastPayload, payload, sizeof(lastPayload) - 1);
            lastPayload[sizeof(lastPayload) - 1] = '\0';
            lastDetectMs = now;

            Serial.printf("QR: accepted -> %s\n", payload);
          }
        } else {
          Serial.println("QR: invalid QR.");
          lastInvalidMs = now;
        }
      } else {  
        // -------- No QR detected → clear LCD line --------
        unsigned long now = millis();
        if (now - lastSeenQrMs > NO_QR_CLEAR_DELAY) {
          LcdMessage lm = {"                    ", 1, false};  // clear line
          xQueueSend(lcdQueue, &lm, 0);
        }
      }
    }
    vTaskDelay(QR_TASK_SLEEP_MS / portTICK_PERIOD_MS);
  } 
}

// ---------------------- BUTTON RESTART TASK ----------------------
void restartTask(void *pvParameters) {
  while (true) {
    int buttonState = digitalRead(RESTART_BUTTON_PIN);
    // With a pull-down resistor, a press registers as HIGH
    if (buttonState == HIGH) {
      vTaskDelay(500 / portTICK_PERIOD_MS); // debounce delay
      ESP.restart();
    }

    // Delay to prevent the task from consuming too much CPU
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}


// ---------------------- SETUP ----------------------
void setup() {
  Serial.begin(115200);
  delay(10);

  Wire.begin(1, 2);
  lcdInit();

  urlQueue = xQueueCreate(1, sizeof(UrlMessage));
  lcdQueue = xQueueCreate(5, sizeof(LcdMessage));

  if (!urlQueue || !lcdQueue) {
    Serial.println("ERROR: queue creation failed");
    while (true) { delay(1000); }
  }

  // LCD boot
  LcdMessage boot = {"Starting...", 0, true};
  xQueueSend(lcdQueue, &boot, 0);

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  lcdClear();
  lcd.setCursor(0, 0); lcd.print("WiFi Connected!");
  lcd.setCursor(0, 1); lcd.print(WiFi.localIP());

  // Camera
  reader.setup();
  reader.beginOnCore(0);   // 🔹 run camera/QR task on Core 0

  // Buzzer
  buzzerInit();

  //Button
  pinMode(RESTART_BUTTON_PIN, INPUT_PULLDOWN);

  // Tasks pinned to Core 1 (application logic)
  xTaskCreatePinnedToCore(restartTask, "Button_Test_Task", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(shutdownTask, "Shutdown_Task", 2048, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(qrCodeTask, "QR_Task", 10 * 1024, NULL, 6, NULL, 1);
  xTaskCreatePinnedToCore(httpTask, "HTTP_Task", 12 * 1024, NULL, 4, NULL, 1);
  xTaskCreatePinnedToCore(lcdTask, "LCD_Task", 6 * 1024, NULL, 3, NULL, 1);


  // Prompt
  beepStartup();
  LcdMessage ready = {"[Scan QRCode]", 0, true};
  xQueueSend(lcdQueue, &ready, 0);

  Serial.println("Setup done.");
}

void loop() {
  // Nothing here, tasks do the work
}

void flushCameraBuffer() {
  QRCodeData flushData;
  while (reader.receiveQrCode(&flushData, 50)) {
    // just discard frames until none left
    vTaskDelay(FLUSH_BUFFER_DELAY_MS / portTICK_PERIOD_MS); 
  }
}
