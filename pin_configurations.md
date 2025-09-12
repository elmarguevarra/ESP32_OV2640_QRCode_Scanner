# ESP32-S3 Camera Pin Configuration Options

Your project currently uses these pins (in `src/camera_pins.h`):

## Current Configuration
```cpp
#define XCLK_GPIO_NUM    10  // Master Clock
#define SIOD_GPIO_NUM    40  // I2C Data
#define SIOC_GPIO_NUM    39  // I2C Clock
#define Y2_GPIO_NUM      11  // D0
#define Y3_GPIO_NUM      9   // D1
#define Y4_GPIO_NUM      8   // D2
#define Y5_GPIO_NUM      7   // D3
#define Y6_GPIO_NUM      6   // D4
#define Y7_GPIO_NUM      5   // D5
#define Y8_GPIO_NUM      4   // D6
#define Y9_GPIO_NUM      3   // D7
#define VSYNC_GPIO_NUM   2   // VSYNC
#define HREF_GPIO_NUM    42  // HREF
#define PCLK_GPIO_NUM    1   // Pixel Clock
```

## Alternative Configuration #1 (Common ESP32-S3-EYE)
```cpp
#define XCLK_GPIO_NUM    15
#define SIOD_GPIO_NUM    4
#define SIOC_GPIO_NUM    5
#define Y2_GPIO_NUM      11
#define Y3_GPIO_NUM      9
#define Y4_GPIO_NUM      8
#define Y5_GPIO_NUM      10
#define Y6_GPIO_NUM      12
#define Y7_GPIO_NUM      18
#define Y8_GPIO_NUM      17
#define Y9_GPIO_NUM      16
#define VSYNC_GPIO_NUM   6
#define HREF_GPIO_NUM    7
#define PCLK_GPIO_NUM    13
```

## Alternative Configuration #2 (XIAO ESP32-S3 Sense)
```cpp
#define XCLK_GPIO_NUM    10
#define SIOD_GPIO_NUM    40
#define SIOC_GPIO_NUM    39
#define Y2_GPIO_NUM      48
#define Y3_GPIO_NUM      11
#define Y4_GPIO_NUM      12
#define Y5_GPIO_NUM      14
#define Y6_GPIO_NUM      16
#define Y7_GPIO_NUM      18
#define Y8_GPIO_NUM      17
#define Y9_GPIO_NUM      15
#define VSYNC_GPIO_NUM   38
#define HREF_GPIO_NUM    47
#define PCLK_GPIO_NUM    13
```

## Alternative Configuration #3 (ESP32-CAM style for S3)
```cpp
#define XCLK_GPIO_NUM    21
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27
#define Y2_GPIO_NUM      4
#define Y3_GPIO_NUM      5
#define Y4_GPIO_NUM      18
#define Y5_GPIO_NUM      19
#define Y6_GPIO_NUM      36
#define Y7_GPIO_NUM      39
#define Y8_GPIO_NUM      34
#define Y9_GPIO_NUM      35
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22
```

## How to Test Different Configurations

1. Replace the pin definitions in `src/camera_pins.h`
2. Run: `platformio run --target upload`
3. Monitor the output to see diagnostics

## Common Issues and Solutions

### Camera Not Detected
- **Wrong pins**: Try alternative configurations above
- **Power**: Ensure 3.3V supply to camera module
- **Connections**: Check all wires are properly connected
- **I2C pullups**: Some modules need 4.7k resistors on SDA/SCL

### Error Codes
- `0x20001`: Camera not found (usually pin issue)
- `0x20002`: I2C communication failed
- `0x105`: No PSRAM (will work but limited performance)

### Manual Testing Steps

1. **Monitor Serial Output**:
   ```bash
   screen /dev/cu.usbmodem5A840041551 115200
   ```
   
2. **Press Reset Button** on ESP32

3. **Look for these messages**:
   - ✓ PSRAM found (good)
   - ✓ Camera basic initialization successful (pins OK)
   - ✓ Camera fully initialized (camera working)
   - ✓ WiFi connected (network OK)

4. **Exit screen**: Press `Ctrl+A` then `k` then `y`

If camera initialization fails, try the alternative pin configurations above.