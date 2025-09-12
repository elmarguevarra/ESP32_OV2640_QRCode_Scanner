# ESP32-S3 OV2640 Camera Stream Project

This project implements a WiFi-enabled camera streaming server using the ESP32-S3 microcontroller and an OV2640 camera module. **Specifically optimized for ESP32-S3 WROOM with CH343 USB-to-serial converter.**

## Features

- **Live video streaming** over WiFi with MJPEG format
- **Web interface** for easy viewing
- **8MB PSRAM support** for high-resolution streaming
- **Robust error handling** and connection monitoring
- **Configurable camera settings** with automatic optimization
- **Real-time diagnostics** via serial output
- **ESP32-S3 WROOM optimized** WiFi and PSRAM configuration
- **CH343 USB-serial support** with high-speed uploads

## Hardware Requirements

- **ESP32-S3 WROOM** Development Board with 8MB PSRAM
- **OV2640 Camera Module** (or compatible)
- **CH343 USB-to-serial converter** (or compatible)
- Proper wiring connections (see pin configuration below)

### Verified Hardware

- ✅ ESP32-S3 WROOM with 8MB embedded PSRAM
- ✅ CH343 USB-to-serial chip
- ✅ OV2640 camera sensor

## Pin Configuration

### ESP32-S3 WROOM Standard Pin Mapping

The current pin configuration in `src/camera_pins.h` is optimized for **ESP32-S3 WROOM** development boards:

```cpp
// Camera pin configuration for ESP32-S3 WROOM with OV2640
// Standard pin mapping for ESP32-S3 WROOM development boards
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
```

**⚠️ Important:** These pins are configured for standard ESP32-S3 WROOM boards. Adjust according to your specific camera module wiring if different.

## CRITICAL: Board Configuration for PSRAM

### Why Board Configuration Matters

Your ESP32-S3 WROOM has **8MB embedded PSRAM**, but PlatformIO board definitions vary:

| Board Definition           | PSRAM Support | Recommended                |
| -------------------------- | ------------- | -------------------------- |
| `esp32-s3-devkitc-1`       | ❌ No PSRAM   | ❌ Wrong for your hardware |
| `freenove_esp32_s3_wroom`  | ✅ 8MB PSRAM  | ✅ **Use this one**        |
| `rymcu-esp32-s3-devkitc-1` | ⚠️ 2MB PSRAM  | ⚠️ Partial support         |

### Verify Your Configuration

Your `platformio.ini` should use:

```ini
[env:esp32-s3-devkitc-1]
platform = espressif32
board = freenove_esp32_s3_wroom  ; Matches ESP32-S3 WROOM with 8MB PSRAM
framework = arduino
```

**Without correct board configuration:**

- ❌ PSRAM not detected by software
- ❌ Limited to VGA resolution (640x480)
- ❌ Single frame buffer (choppy streaming)
- ❌ Poor camera performance

**With correct board configuration:**

- ✅ Full 8MB PSRAM available
- ✅ SVGA resolution (800x600) or higher
- ✅ Multiple frame buffers (smooth streaming)
- ✅ Optimal camera performance

## Setup Instructions

### 1. Configure WiFi Credentials

Edit the WiFi credentials in `src/main.cpp`:

```cpp
const char* ssid = "your_wifi_name";
const char* password = "your_wifi_password";
```

### 2. Build and Upload

```bash
# Install dependencies
platformio lib install

# Build the project
platformio run

# Upload to ESP32-S3
platformio run --target upload

# Monitor serial output
platformio device monitor
```

### 3. Access the Camera Stream

Once uploaded and connected to WiFi, the ESP32-S3 will print its IP address to the serial monitor. You can then:

- **Web Interface**: Open `http://[ESP32_IP]:81/` in your browser
- **Direct Stream**: Access the raw MJPEG stream at `http://[ESP32_IP]:81/stream`

## Usage

1. Power up the ESP32-S3
2. Watch the serial monitor for connection status and IP address
3. Open the web interface in your browser
4. The camera stream should start automatically

## Configuration Options

### Camera Settings

In `src/main.cpp`, you can adjust:

- **Frame size**: `FRAMESIZE_VGA`, `FRAMESIZE_SVGA`, etc.
- **JPEG quality**: 1-63 (lower = better quality, higher file size)
- **Frame buffer count**: Number of buffers for smoother streaming

### PSRAM Configuration

The project automatically detects PSRAM and configures accordingly:

- **With PSRAM**: Higher resolution (SVGA), multiple buffers
- **Without PSRAM**: Lower resolution (VGA), single buffer

## Troubleshooting

### Camera Not Detected

- Verify pin connections match your hardware
- Check power supply (3.3V for camera module)
- Ensure I2C pullup resistors are present

### WiFi Connection Issues

- Double-check WiFi credentials
- Ensure 2.4GHz network (ESP32 doesn't support 5GHz)
- Check signal strength

### Streaming Issues

- Verify camera initialization in serial monitor
- Try reducing frame size or quality
- Check browser compatibility (Chrome/Firefox recommended)

### Common Error Messages

| Error                    | Solution                                              |
| ------------------------ | ----------------------------------------------------- |
| `Camera init failed`     | Check wiring and pin configuration                    |
| `Camera capture failed`  | Verify power supply and connections                   |
| `WiFi connection failed` | Check credentials and network availability            |
| `No PSRAM found`         | Normal for boards without PSRAM (reduces performance) |

## Development

### Project Structure

```
├── src/
│   ├── main.cpp           # Main application logic
│   ├── app_httpd.cpp      # HTTP server implementation
│   └── camera_pins.h      # Pin configuration
├── platformio.ini         # PlatformIO configuration
└── README.md             # This file
```

### Key Functions

- `initCamera()`: Initialize camera with optimal settings
- `startCameraServer()`: Start HTTP streaming server
- `stream_handler()`: Handle MJPEG streaming requests
- `index_handler()`: Serve web interface

## Performance Tips

1. **Use PSRAM**: Significantly improves performance
2. **Optimize WiFi**: Use strong signal, avoid congestion
3. **Adjust quality**: Lower JPEG quality for smoother streaming
4. **Monitor resources**: Watch heap usage in serial output

## License

This project is open source. Feel free to modify and distribute according to your needs.

## Support

For issues and questions:

1. Check the troubleshooting section
2. Review serial monitor output for error messages
3. Verify hardware connections match pin configuration
