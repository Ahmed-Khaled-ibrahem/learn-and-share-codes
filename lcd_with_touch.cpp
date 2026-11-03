#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9488 _panel;
  lgfx::Bus_SPI _bus;
  lgfx::Touch_XPT2046 _touch;

public:
  LGFX() {
    {
      auto cfg = _bus.config();
      cfg.spi_host   = SPI3_HOST;
      cfg.spi_mode   = 0;
      cfg.freq_write = 20000000;
      cfg.freq_read  = 8000000;
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
      cfg.bus_shared   = true;     // touch shares the SPI bus
      _panel.config(cfg);
    }
    {
      auto cfg = _touch.config();
      cfg.x_min = 300;
      cfg.x_max = 3900;
      cfg.y_min = 200;
      cfg.y_max = 3900;
      cfg.pin_int = 22;            // T_IRQ
      cfg.bus_shared = true;
      cfg.offset_rotation = 0;
      cfg.spi_host = SPI3_HOST;
      cfg.freq = 1000000;
      cfg.pin_sclk = 18;
      cfg.pin_mosi = 23;
      cfg.pin_miso = 19;
      cfg.pin_cs = 21;             // T_CS
      _touch.config(cfg);
      _panel.setTouch(&_touch);
    }
    setPanel(&_panel);
  }
};

LGFX lcd;

void drawHeader() {
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  lcd.setTextSize(2);
  lcd.setCursor(10, 10);
  lcd.println("Touch test: touch the screen");
  lcd.drawRect(0, 0, 480, 320, TFT_YELLOW);
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.setRotation(1);
  drawHeader();
}

void loop() {
  uint16_t x, y;

  if (lcd.getTouch(&x, &y)) {
    // Draw a dot where you touch
    lcd.fillCircle(x, y, 4, TFT_GREEN);

    // Show coordinates
    lcd.fillRect(10, 50, 300, 30, TFT_BLACK);
    lcd.setTextColor(TFT_CYAN, TFT_BLACK);
    lcd.setCursor(10, 50);
    lcd.printf("X: %d  Y: %d", x, y);

    Serial.printf("X=%d Y=%d\n", x, y);
  }

  // Touch the top-right corner to clear the screen
  if (lcd.getTouch(&x, &y) && x > 420 && y < 40) {
    drawHeader();
  }
}
