#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

// Audio Configuration
// INMP441 Mic: 16kHz input
// MAX98357A Speaker: 24kHz output (will be resampled internally)
#define AUDIO_INPUT_SAMPLE_RATE  16000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

// I2S Configuration - Duplex mode with shared clocks
// ESP32-C3 has only ONE I2S peripheral, mic and speaker share BCLK/WS
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_6   // Word Select (shared)
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_5   // Bit Clock (shared)
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_4   // INMP441 SD (Mic Data In)
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_7   // MAX98357A DIN (Speaker Data Out)

// Button Configuration
#define BOOT_BUTTON_GPIO    GPIO_NUM_3   // Push-to-talk button

// LED Configuration (optional - set to NC if not used)
#define BUILTIN_LED_GPIO    GPIO_NUM_NC  // No built-in LED

// OLED Display Configuration (SSD1306 128x64 via I2C)
#define DISPLAY_SDA_PIN     GPIO_NUM_8   // I2C SDA
#define DISPLAY_SCL_PIN     GPIO_NUM_9   // I2C SCL
#define DISPLAY_WIDTH       128
#define DISPLAY_HEIGHT      64
#define DISPLAY_MIRROR_X    false
#define DISPLAY_MIRROR_Y    false

#endif // _BOARD_CONFIG_H_
