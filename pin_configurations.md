# ESP32-S3 WROOM Camera Pin Configuration Options

**For ESP32-S3 WROOM development boards with OV2640 camera modules.**

## Current Working Configuration ✅

This project uses the **ESP32-S3 WROOM standard** pin mapping (in `src/camera_pins.h`):

```cpp
// ESP32-S3 WROOM with OV2640 - CURRENT WORKING CONFIG
#define PWDN_GPIO_NUM    -1  // Power down not used
#define RESET_GPIO_NUM   -1  // Reset not used
#define XCLK_GPIO_NUM    15  // Master Clock
#define SIOD_GPIO_NUM    4   // I2C Data (SDA)
#define SIOC_GPIO_NUM    5   // I2C Clock (SCL)

// Data pins (D0-D7)
#define Y2_GPIO_NUM      11  // D0
#define Y3_GPIO_NUM      9   // D1
#define Y4_GPIO_NUM      8   // D2
#define Y5_GPIO_NUM      10  // D3
#define Y6_GPIO_NUM      12  // D4
#define Y7_GPIO_NUM      18  // D5
#define Y8_GPIO_NUM      17  // D6
#define Y9_GPIO_NUM      16  // D7

// Control pins
#define VSYNC_GPIO_NUM   6   // VSYNC
#define HREF_GPIO_NUM    7   // HREF
#define PCLK_GPIO_NUM    13  // Pixel Clock
```

**This configuration is verified working with:**
- ✅ ESP32-S3 WROOM with 8MB PSRAM
- ✅ CH343 USB-to-serial converter
- ✅ Standard OV2640 camera modules

## Alternative Configuration #1: Original ESP32-S3 Generic

**If the current config doesn't work, try this older mapping:**

```cpp
#define PWDN_GPIO_NUM    -1  // Power down not used
#define RESET_GPIO_NUM   -1  // Reset not used
#define XCLK_GPIO_NUM    10  // Master Clock
#define SIOD_GPIO_NUM    40  // I2C Data (SDA)
#define SIOC_GPIO_NUM    39  // I2C Clock (SCL)

// Data pins (D0-D7)
#define Y2_GPIO_NUM      11  // D0
#define Y3_GPIO_NUM      9   // D1
#define Y4_GPIO_NUM      8   // D2
#define Y5_GPIO_NUM      7   // D3
#define Y6_GPIO_NUM      6   // D4
#define Y7_GPIO_NUM      5   // D5
#define Y8_GPIO_NUM      4   // D6
#define Y9_GPIO_NUM      3   // D7

// Control pins
#define VSYNC_GPIO_NUM   2   // VSYNC
#define HREF_GPIO_NUM    42  // HREF
#define PCLK_GPIO_NUM    1   // Pixel Clock
```

## Alternative Configuration #2: XIAO ESP32-S3 Sense

**For XIAO ESP32-S3 Sense boards (different pinout):**

```cpp
#define PWDN_GPIO_NUM    -1  // Power down not used
#define RESET_GPIO_NUM   -1  // Reset not used
#define XCLK_GPIO_NUM    10  // Master Clock
#define SIOD_GPIO_NUM    40  // I2C Data (SDA)
#define SIOC_GPIO_NUM    39  // I2C Clock (SCL)

// Data pins (D0-D7) - XIAO specific
#define Y2_GPIO_NUM      48  // D0
#define Y3_GPIO_NUM      11  // D1
#define Y4_GPIO_NUM      12  // D2
#define Y5_GPIO_NUM      14  // D3
#define Y6_GPIO_NUM      16  // D4
#define Y7_GPIO_NUM      18  // D5
#define Y8_GPIO_NUM      17  // D6
#define Y9_GPIO_NUM      15  // D7

// Control pins - XIAO specific
#define VSYNC_GPIO_NUM   38  // VSYNC
#define HREF_GPIO_NUM    47  // HREF
#define PCLK_GPIO_NUM    13  // Pixel Clock
```

## Alternative Configuration #3: Custom Wiring

**If you have custom camera module wiring:**

```cpp
#define PWDN_GPIO_NUM    -1  // Power down not used
#define RESET_GPIO_NUM   -1  // Reset not used
#define XCLK_GPIO_NUM    21  // Master Clock - adjust as needed
#define SIOD_GPIO_NUM    26  // I2C Data (SDA) - adjust as needed
#define SIOC_GPIO_NUM    27  // I2C Clock (SCL) - adjust as needed

// Data pins (D0-D7) - adjust all as needed
#define Y2_GPIO_NUM      35  // D0
#define Y3_GPIO_NUM      34  // D1
#define Y4_GPIO_NUM      5   // D2
#define Y5_GPIO_NUM      39  // D3
#define Y6_GPIO_NUM      18  // D4
#define Y7_GPIO_NUM      36  // D5
#define Y8_GPIO_NUM      19  // D6
#define Y9_GPIO_NUM      4   // D7

// Control pins - adjust as needed
#define VSYNC_GPIO_NUM   25  // VSYNC
#define HREF_GPIO_NUM    23  // HREF
#define PCLK_GPIO_NUM    22  // Pixel Clock
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