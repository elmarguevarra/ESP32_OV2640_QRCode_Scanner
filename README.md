# ESP32-S3 OV2640 Camera Stream Project

This project implements a WiFi-enabled camera streaming server using the ESP32-S3 microcontroller and an OV2640 camera module.

## Features

- **Live video streaming** over WiFi
- **Web interface** for easy viewing
- **PSRAM support** for better performance
- **Robust error handling** and connection monitoring
- **Configurable camera settings**
- **Real-time diagnostics** via serial output

## Hardware Requirements

- ESP32-S3 Development Board (with PSRAM recommended)
- OV2640 Camera Module
- Proper wiring connections (see pin configuration below)

## Pin Configuration

The current pin configuration in `src/camera_pins.h` is set for a common ESP32-S3 camera module setup:

```cpp
// Camera pins for ESP32-S3
#define XCLK_GPIO_NUM    10  // Master Clock
#define SIOD_GPIO_NUM    40  // I2C Data
#define SIOC_GPIO_NUM    39  // I2C Clock

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

**⚠️ Important:** Adjust these pins according to your specific hardware wiring!

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

| Error | Solution |
|-------|----------|
| `Camera init failed` | Check wiring and pin configuration |
| `Camera capture failed` | Verify power supply and connections |
| `WiFi connection failed` | Check credentials and network availability |
| `No PSRAM found` | Normal for boards without PSRAM (reduces performance) |

## Development

### Project Structure

```
├── src/
│   ├── main.cpp           # Main application logic
│   ├── app_httpd.cpp      # HTTP server implementation
│   ├── app_httpd.h        # HTTP server header
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