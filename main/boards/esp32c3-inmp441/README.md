# ESP32-C3 with INMP441 + MAX98357A + OLED

A XiaoZhi AI board for ESP32-C3 using:
- **Microphone**: INMP441 (I2S)
- **Speaker**: MAX98357A (I2S)
- **Display**: SSD1306 OLED 128x64 (I2C)
- **Button**: Push-to-talk

## Wiring

| ESP32-C3 Pin | Device | Function |
|--------------|--------|----------|
| GPIO 5 | INMP441 + MAX98357A | BCLK (shared) |
| GPIO 6 | INMP441 + MAX98357A | WS/LRC (shared) |
| GPIO 4 | INMP441 | SD (Mic Data) |
| GPIO 7 | MAX98357A | DIN (Speaker Data) |
| GPIO 3 | Button | Push-to-talk |
| GPIO 8 | SSD1306 OLED | SDA |
| GPIO 9 | SSD1306 OLED | SCL |
| 5V | MAX98357A | VIN |
| 3V3 | INMP441, SSD1306 | VDD |
| GND | All | GND |

## Usage

1. **First boot**: Click button to enter WiFi config mode
2. **After WiFi configured**: Hold button to talk, release to let AI respond
