// Camera pin configuration for ESP32-S3 WROOM with OV2640
// Standard pin mapping for ESP32-S3 WROOM development boards
// Adjust according to your specific camera module wiring

#define PWDN_GPIO_NUM    -1  // Power down not used
#define RESET_GPIO_NUM   -1  // Reset not used
#define XCLK_GPIO_NUM    15  // XCLK (Master Clock)
#define SIOD_GPIO_NUM    4   // SDA (I2C Data)
#define SIOC_GPIO_NUM    5   // SCL (I2C Clock)

// Data pins (D0-D7) - ESP32-S3 WROOM standard mapping
#define Y2_GPIO_NUM      11  // D0
#define Y3_GPIO_NUM      9   // D1
#define Y4_GPIO_NUM      8   // D2
#define Y5_GPIO_NUM      10  // D3
#define Y6_GPIO_NUM      12  // D4
#define Y7_GPIO_NUM      18  // D5
#define Y8_GPIO_NUM      17  // D6
#define Y9_GPIO_NUM      16  // D7

// Control pins - ESP32-S3 WROOM standard
#define VSYNC_GPIO_NUM   6   // VSYNC
#define HREF_GPIO_NUM    7   // HREF
#define PCLK_GPIO_NUM    13  // PCLK (Pixel Clock)
