# Xiaozhi AI - ESP32-C3 Edition

Affordable solution for Xiaozhi AI using ESP32-C3.

## Features

- 🎤 **Voice Input** - INMP441 I2S microphone
- 🔊 **Voice Output** - MAX98357A I2S amplifier
- 📺 **Optional OLED** - SSD1306 128x64 display
- 🔘 **Push-to-Talk** - Simple button control
- 📶 **WiFi** - Easy configuration via hotspot

## Hardware Required

| Component | Description |
|-----------|-------------|
| ESP32-C3 | Any ESP32-C3 dev board (4MB flash) |
| INMP441 | I2S MEMS microphone |
| MAX98357A | I2S amplifier + speaker |
| Push Button | Momentary switch |
| SSD1306 OLED | 128x64 I2C display (optional) |

## Wiring Diagram

```
ESP32-C3          INMP441 (Mic)      MAX98357A (Speaker)
─────────         ─────────────      ───────────────────
GPIO 5  ────────► SCK                BCLK
GPIO 6  ────────► WS                 LRC  
GPIO 4  ────────► SD                 
GPIO 7  ──────────────────────────►  DIN
3.3V    ────────► VDD                
5V      ──────────────────────────►  VIN
GND     ────────► GND                GND
        ────────► L/R → GND

ESP32-C3          Push Button
─────────         ───────────
GPIO 3  ────────► One side
GND     ────────► Other side

ESP32-C3          SSD1306 OLED (Optional)
─────────         ──────────────────────
GPIO 8  ────────► SDA
GPIO 9  ────────► SCL (or SCK)
3.3V    ────────► VCC
GND     ────────► GND
```

### Pin Summary

| ESP32-C3 Pin | Function |
|--------------|----------|
| GPIO 3 | Push-to-talk button |
| GPIO 4 | Mic data (INMP441 SD) |
| GPIO 5 | I2S BCLK (shared) |
| GPIO 6 | I2S WS/LRC (shared) |
| GPIO 7 | Speaker data (MAX98357A DIN) |
| GPIO 8 | OLED SDA (optional) |
| GPIO 9 | OLED SCL (optional) |

## Installation

### Prerequisites

- [ESP-IDF v5.5+](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/)
- Git

### Build & Flash

```bash
# Clone the repository
git clone https://github.com/mniroy/xiaozhi-ai-esp32-c3.git
cd xiaozhi-ai-esp32-c3

# Set target to ESP32-C3
idf.py set-target esp32c3

# Build the project
idf.py build

# Flash to device (replace COM6 with your port)
idf.py -p COM6 flash monitor
```

## Usage

### First Boot - WiFi Setup

1. Power on the device
2. **Click the button** during startup (within 5 seconds)
3. Connect to the WiFi hotspot: `Xiaozhi-XXXX`
4. Open http://192.168.4.1 in browser
5. Enter your WiFi credentials
6. Device will restart and connect

### Talking to AI

1. **Hold the button** and speak
2. **Release the button** when done
3. Wait for AI response

## Troubleshooting

### No Sound Output
- Check MAX98357A is powered by **5V** (not 3.3V)
- Verify GPIO 7 is connected to DIN
- Add a 100µF capacitor between VIN and GND

### Mic Not Working
- Ensure INMP441 L/R pin is connected to **GND** (left channel)
- Check GPIO 4 is connected to SD

### OLED Not Detected
- Verify I2C wiring (SDA→GPIO8, SCL→GPIO9)
- Try adding 4.7kΩ pull-up resistors on SDA and SCL to 3.3V
- Device works fine without OLED

## License

Based on [xiaozhi-esp32](https://github.com/78/xiaozhi-esp32) project.

## Credits

- Original Xiaozhi project by [78](https://github.com/78)
- ESP32-C3 port by [mniroy](https://github.com/mniroy)
