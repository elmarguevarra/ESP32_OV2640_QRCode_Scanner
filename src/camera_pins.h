// Camera pin configuration for ESP32-S3 WROOM with OV2640
// Standard pin mapping for ESP32-S3 WROOM development boards
// Adjust according to your specific camera module wiring

const int PWDN_GPIO_NUM    = -1;  // Power down not used
const int RESET_GPIO_NUM   = -1;  // Reset not used
const int XCLK_GPIO_NUM    = 15;  // XCLK (Master Clock)
const int SIOD_GPIO_NUM    = 4;   // SDA (I2C Data)
const int SIOC_GPIO_NUM    = 5;   // SCL (I2C Clock)

// Data pins (D0-D7) - ESP32-S3 WROOM standard mapping
const int Y2_GPIO_NUM      = 11;  // D0
const int Y3_GPIO_NUM      = 9;   // D1
const int Y4_GPIO_NUM      = 8;   // D2
const int Y5_GPIO_NUM      = 10;  // D3
const int Y6_GPIO_NUM      = 12;  // D4
const int Y7_GPIO_NUM      = 18;  // D5
const int Y8_GPIO_NUM      = 17;  // D6
const int Y9_GPIO_NUM      = 16;  // D7

// Control pins - ESP32-S3 WROOM standard
const int VSYNC_GPIO_NUM   = 6;   // VSYNC
const int HREF_GPIO_NUM    = 7;   // HREF
const int PCLK_GPIO_NUM    = 13;  // PCLK (Pixel Clock)
