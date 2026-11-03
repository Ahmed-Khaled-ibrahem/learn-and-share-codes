#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9488 _panel;
  lgfx::Bus_SPI _bus;

public:
  LGFX() {
    {
      auto cfg = _bus.config();
      cfg.spi_host   = SPI3_HOST;   // VSPI
      cfg.spi_mode   = 0;
      cfg.freq_write = 20000000;
      cfg.pin_sclk   = 18;
      cfg.pin_mosi   = 23;
      cfg.pin_miso   = 19;
      cfg.pin_dc     = 2;
      _bus.config(cfg);
      _panel.setBus(&_bus);
    }
    {
      auto cfg = _panel.config();
      cfg.pin_cs       = 15;
      cfg.pin_rst      = 4;
      cfg.panel_width  = 320;
      cfg.panel_height = 480;
      cfg.readable     = false;
      cfg.invert       = false;
      cfg.rgb_order    = false;
      _panel.config(cfg);
    }
    setPanel(&_panel);
  }
};

LGFX lcd;

void setup() {
  lcd.init();
  lcd.setRotation(1);          // landscape 480x320
  lcd.fillScreen(TFT_BLACK);

  lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  lcd.setTextSize(3);
  lcd.setCursor(20, 20);
  lcd.println("Hello ESP32!");

  lcd.setTextSize(2);
  lcd.setTextColor(TFT_CYAN, TFT_BLACK);
  lcd.setCursor(20, 70);
  lcd.println("ILI9488 480x320 TFT");

  lcd.fillRect(20, 120, 120, 80, TFT_RED);
  lcd.fillCircle(230, 160, 40, TFT_GREEN);
  lcd.fillTriangle(320, 200, 380, 120, 440, 200, TFT_BLUE);
  lcd.drawRect(10, 10, 460, 300, TFT_YELLOW);
}

void loop() {
  uint16_t colors[] = {TFT_RED, TFT_ORANGE, TFT_YELLOW,
                       TFT_GREEN, TFT_CYAN, TFT_BLUE, TFT_MAGENTA};
  for (int i = 0; i < 7; i++) {
    lcd.fillRect(20 + i * 62, 240, 60, 50, colors[i]);
    delay(300);
  }
}
// Display pin	ESP32 pin
// VCC	3.3V
// GND	GND
// CS	GPIO 15
// RESET	GPIO 4
// DC / RS	GPIO 2
// SDI (MOSI)	GPIO 23
// SCK	GPIO 18
// LED	3.3V
// SDO (MISO)	GPIO 19 (optional)
