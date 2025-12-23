/**
 * ESP32-C3 with INMP441 Microphone, MAX98357A Speaker, and SSD1306 OLED
 * 
 * Wiring:
 * GPIO 5 - BCLK (shared by mic and speaker)
 * GPIO 6 - WS/LRC (shared by mic and speaker)
 * GPIO 4 - INMP441 SD (Mic Data In)
 * GPIO 7 - MAX98357A DIN (Speaker Data Out)
 * GPIO 3 - Push-to-talk button
 * GPIO 8 - OLED SDA
 * GPIO 9 - OLED SCL
 */

#include "wifi_board.h"
#include "codecs/no_audio_codec.h"
#include "display/oled_display.h"
#include "application.h"
#include "button.h"
#include "config.h"

#include <esp_log.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_ssd1306.h>
#include <driver/i2c_master.h>

#define TAG "Esp32c3Inmp441Board"

class Esp32c3Inmp441Board : public WifiBoard {
private:
    Button boot_button_;
    i2c_master_bus_handle_t i2c_bus_ = nullptr;

    void InitializeI2c() {
        i2c_master_bus_config_t i2c_bus_cfg = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = DISPLAY_SDA_PIN,
            .scl_io_num = DISPLAY_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = true,
            },
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus_));
        ESP_LOGI(TAG, "I2C bus initialized (SDA: %d, SCL: %d)", DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
    }

    void InitializeButtons() {
        // Press down to start listening, release to stop (push-to-talk)
        boot_button_.OnPressDown([this]() {
            Application::GetInstance().StartListening();
        });
        boot_button_.OnPressUp([this]() {
            Application::GetInstance().StopListening();
        });
        
        // Click during startup to enter WiFi config mode
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
            }
        });
    }

public:
    Esp32c3Inmp441Board() : boot_button_(BOOT_BUTTON_GPIO) {
        ESP_LOGI(TAG, "Initializing ESP32-C3 INMP441 Board with OLED");
        ESP_LOGI(TAG, "  BCLK: GPIO %d", AUDIO_I2S_GPIO_BCLK);
        ESP_LOGI(TAG, "  WS:   GPIO %d", AUDIO_I2S_GPIO_WS);
        ESP_LOGI(TAG, "  DIN:  GPIO %d (Mic)", AUDIO_I2S_GPIO_DIN);
        ESP_LOGI(TAG, "  DOUT: GPIO %d (Speaker)", AUDIO_I2S_GPIO_DOUT);
        ESP_LOGI(TAG, "  Button: GPIO %d", BOOT_BUTTON_GPIO);
        ESP_LOGI(TAG, "  OLED: SDA=%d, SCL=%d (%dx%d)", 
                 DISPLAY_SDA_PIN, DISPLAY_SCL_PIN, DISPLAY_WIDTH, DISPLAY_HEIGHT);
        
        InitializeI2c();
        InitializeButtons();
    }

    virtual AudioCodec* GetAudioCodec() override {
        // NoAudioCodecDuplex for INMP441 mic + MAX98357A speaker
        // Uses same I2S bus with shared BCLK/WS
        static NoAudioCodecDuplex audio_codec(
            AUDIO_INPUT_SAMPLE_RATE, 
            AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_BCLK, 
            AUDIO_I2S_GPIO_WS, 
            AUDIO_I2S_GPIO_DOUT, 
            AUDIO_I2S_GPIO_DIN
        );
        return &audio_codec;
    }

    virtual Display* GetDisplay() override {
        static Display* display = nullptr;
        
        if (display == nullptr) {
            // Try to initialize SSD1306 OLED - first try 0x3C, then 0x3D
            uint8_t i2c_addresses[] = {0x3C, 0x3D};
            esp_lcd_panel_io_handle_t panel_io = nullptr;
            esp_lcd_panel_handle_t panel = nullptr;
            
            for (int i = 0; i < 2; i++) {
                uint8_t addr = i2c_addresses[i];
                ESP_LOGI(TAG, "Trying OLED at I2C address 0x%02X", addr);
                
                esp_lcd_panel_io_i2c_config_t io_config = {
                    .dev_addr = addr,
                    .on_color_trans_done = nullptr,
                    .user_ctx = nullptr,
                    .control_phase_bytes = 1,
                    .dc_bit_offset = 6,
                    .lcd_cmd_bits = 8,
                    .lcd_param_bits = 8,
                    .flags = {
                        .dc_low_on_data = false,
                        .disable_control_phase = false,
                    },
                    .scl_speed_hz = 400000,
                };
                
                if (esp_lcd_new_panel_io_i2c(i2c_bus_, &io_config, &panel_io) != ESP_OK) {
                    continue;
                }

                esp_lcd_panel_dev_config_t panel_config = {
                    .reset_gpio_num = GPIO_NUM_NC,
                    .bits_per_pixel = 1,
                    .flags = {
                        .reset_active_high = false,
                    },
                    .vendor_config = nullptr,
                };
                
                if (esp_lcd_new_panel_ssd1306(panel_io, &panel_config, &panel) != ESP_OK) {
                    esp_lcd_panel_io_del(panel_io);
                    panel_io = nullptr;
                    continue;
                }
                
                if (esp_lcd_panel_reset(panel) != ESP_OK ||
                    esp_lcd_panel_init(panel) != ESP_OK ||
                    esp_lcd_panel_disp_on_off(panel, true) != ESP_OK) {
                    esp_lcd_panel_del(panel);
                    esp_lcd_panel_io_del(panel_io);
                    panel = nullptr;
                    panel_io = nullptr;
                    continue;
                }
                
                // Success!
                ESP_LOGI(TAG, "OLED initialized at address 0x%02X", addr);
                display = new OledDisplay(panel_io, panel, 
                                          DISPLAY_WIDTH, DISPLAY_HEIGHT, 
                                          DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);
                break;
            }
            
            if (display == nullptr) {
                // OLED not found, use no display
                ESP_LOGW(TAG, "OLED not found, continuing without display");
                static NoDisplay no_display;
                display = &no_display;
            }
        }
        return display;
    }
};

DECLARE_BOARD(Esp32c3Inmp441Board);
