#include "esp_camera.h"
#include <WiFi.h>
#include "camera_pins.h"
#include "esp_log.h"
#include "lcd.h"


#ifndef WIFI_SSID
  #define WIFI_SSID "default_ssid"
#endif

#ifndef WIFI_PASSWORD
  #define WIFI_PASSWORD "default_password"
#endif

static const char* TAG = "camera_main";

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Function declarations
void startCameraServer();
bool initCamera();
void printCameraInfo();
bool testBasicCameraInit();

void setup() {
  Serial.begin(115200);

  lcdInit();

  delay(2000); // Give time for serial monitor to connect
  
  Serial.println();
  Serial.println("==================================================");
  Serial.println("ESP32-S3 Camera Diagnostic Mode");
  Serial.println("==================================================");
  
  // Print system info
  Serial.printf("ESP32 Chip model: %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("Cores: %d\n", ESP.getChipCores());
  Serial.printf("Flash size: %d bytes\n", ESP.getFlashChipSize());
  Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
  
  // Check PSRAM
  Serial.println("\n--- PSRAM Check ---");
  if (psramFound()) {
    Serial.printf("✓ PSRAM found! Total: %d bytes, Free: %d bytes\n", ESP.getPsramSize(), ESP.getFreePsram());
  } else {
    Serial.println("✗ No PSRAM found! This will limit camera performance.");
  }
  
  // Test basic camera initialization
  Serial.println("\n--- Camera Initialization Test ---");
  if (testBasicCameraInit()) {
    Serial.println("✓ Camera basic initialization successful");
    
    // Test full camera initialization
    Serial.println("\n--- Full Camera Initialization ---");
    if (initCamera()) {
      Serial.println("✓ Camera fully initialized");
      printCameraInfo();
      
      // Test WiFi
      Serial.println("\n--- WiFi Connection Test ---");
      Serial.printf("Attempting to connect to: %s\n", ssid);
      
      // ESP32-S3 WROOM specific WiFi initialization
      WiFi.mode(WIFI_STA);
      WiFi.disconnect(true);
      delay(1000);
      
      // Configure WiFi with specific settings for ESP32-S3 WROOM
      WiFi.begin(ssid, password);
      WiFi.setSleep(false);
      WiFi.setTxPower(WIFI_POWER_19_5dBm); // Max power for better connection
      
      Serial.print("Connecting");
      lcdScroll("Connecting to WiFi");
      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(1000);
        Serial.print(".");
        attempts++;
      }
      
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✓ WiFi connected successfully!");
        Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("MAC Address: %s\n", WiFi.macAddress().c_str());
        
        // Start camera server
        Serial.println("\n--- Starting Camera Server ---");
        startCameraServer();
        Serial.println("✓ Camera server started!");
        Serial.printf("\nCamera stream URLs:\n");
        Serial.printf("- Web Interface: http://%s:81/\n", WiFi.localIP().toString().c_str());
        Serial.printf("- Direct Stream: http://%s:81/stream\n", WiFi.localIP().toString().c_str());
        Serial.println();
        Serial.println("==================================================");
        Serial.println("System is running! Check the URLs above.");
        Serial.println("==================================================");
        lcdPrint("System is running!");
        lcdPrint(WiFi.localIP().toString().c_str(), 1, false);
      } else {
        Serial.println("\n✗ WiFi connection failed!");
        Serial.println("Please check:");
        Serial.println("1. WiFi credentials in main.cpp");
        Serial.println("2. 2.4GHz network availability");
        Serial.println("3. WiFi signal strength");
      }
    } else {
      Serial.println("✗ Full camera initialization failed!");
      Serial.println("This suggests a wiring or pin configuration issue.");
    }
  } else {
    Serial.println("✗ Basic camera initialization failed!");
    Serial.println("Possible issues:");
    Serial.println("1. Camera module not connected");
    Serial.println("2. Incorrect pin configuration");
    Serial.println("3. Power supply issues (camera needs 3.3V)");
    Serial.println("4. Faulty camera module");
    
    Serial.println("\nCurrent pin configuration:");
    Serial.printf("XCLK: GPIO%d\n", XCLK_GPIO_NUM);
    Serial.printf("SIOD: GPIO%d (I2C SDA)\n", SIOD_GPIO_NUM);
    Serial.printf("SIOC: GPIO%d (I2C SCL)\n", SIOC_GPIO_NUM);
    Serial.printf("VSYNC: GPIO%d\n", VSYNC_GPIO_NUM);
    Serial.printf("HREF: GPIO%d\n", HREF_GPIO_NUM);
    Serial.printf("PCLK: GPIO%d\n", PCLK_GPIO_NUM);
    Serial.printf("Data pins: GPIO%d-GPIO%d\n", Y2_GPIO_NUM, Y9_GPIO_NUM);
  }
}

void loop() {
  // Monitor WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost! Attempting to reconnect...");
    WiFi.reconnect();
    delay(5000);
  }
  delay(10000); // Check every 10 seconds
}

bool initCamera() {
  camera_config_t config;
  
  // Camera configuration
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  
  // Frame buffer configuration based on PSRAM availability
  if (psramFound()) {
    config.frame_size = FRAMESIZE_SVGA;  // 800x600
    config.jpeg_quality = 10;
    config.fb_count = 2;
    Serial.println("Using PSRAM for camera buffers");
  } else {
    config.frame_size = FRAMESIZE_VGA;   // 640x480
    config.jpeg_quality = 12;
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_DRAM;
    Serial.println("Using DRAM for camera buffers (limited performance)");
  }

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x: %s\n", err, esp_err_to_name(err));
    return false;
  }

  // Test camera by capturing a frame
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture test failed");
    return false;
  }
  
  Serial.printf("Camera capture test succeeded - Frame size: %dx%d, length: %d bytes\n", 
                fb->width, fb->height, fb->len);
  esp_camera_fb_return(fb);
  
  return true;
}

void printCameraInfo() {
  sensor_t * s = esp_camera_sensor_get();
  if (s) {
    Serial.printf("Camera sensor: %s\n", 
                  s->id.PID == OV2640_PID ? "OV2640" :
                  s->id.PID == OV3660_PID ? "OV3660" :
                  s->id.PID == OV5640_PID ? "OV5640" : "Unknown");
    Serial.printf("Camera PID: 0x%02X\n", s->id.PID);
  } else {
    Serial.println("Failed to get camera sensor info");
  }
}

bool testBasicCameraInit() {
  // Very basic camera configuration to test if pins are working
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;  // Lower frequency for testing
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  
  // Use minimum settings
  config.frame_size = FRAMESIZE_QVGA;  // 320x240 - smallest size
  config.jpeg_quality = 15;  // Lower quality
  config.fb_count = 1;
  config.fb_location = CAMERA_FB_IN_DRAM;  // Use DRAM for basic test
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Basic camera init failed: 0x%x (%s)\n", err, esp_err_to_name(err));
    return false;
  }
  
  // Don't test capture yet, just successful initialization
  esp_camera_deinit();
  return true;
}