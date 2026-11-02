#define DISPLAY_ILI9488  // real 3.5" display
// #define DISPLAY_ILI9341   // Wokwi simulation

#if defined(DISPLAY_ILI9488) && defined(DISPLAY_ILI9341)
#error "Choose only ONE display define"
#elif !defined(DISPLAY_ILI9488) && !defined(DISPLAY_ILI9341)
#error "Choose a display: DISPLAY_ILI9488 or DISPLAY_ILI9341"
#endif

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// =====================================================================
//  PINS
// =====================================================================
// Display (SPI) — same for both
#define PIN_SCLK 18
#define PIN_MOSI 23
#define PIN_MISO 19
#define PIN_DC 2
#define PIN_CS 15
#define PIN_RST 4
// miso not connected
// T_IRQ not connected
// T_CLK on 18
// T_DO on 19

// Touch
#ifdef DISPLAY_ILI9488
#define PIN_T_CS 21  // XPT2046 chip select (shares SCLK/MOSI/MISO)
#else
#define PIN_SDA 21  // FT6206 I2C
#define PIN_SCL 22
#endif

// VCC	3V3 (or 5V if your module has a regulator, so check its label)
// GND	GND
// CS	GPIO 15
// RESET	GPIO 4
// DC / RS	GPIO 2
// SDI (MOSI)	GPIO 23
// SCK	GPIO 18
// LED (backlight)	3V3
// SDO (MISO)	not connected
// T_CLK	GPIO 18
// T_CS	GPIO 21
// T_DIN	GPIO 23
// T_DO	GPIO 19
// T_IRQ	not connected

// =====================================================================
//  DISPLAY + TOUCH SETUP
// =====================================================================
class LGFX : public lgfx::LGFX_Device {
#ifdef DISPLAY_ILI9488
  lgfx::Panel_ILI9488 _panel;
  lgfx::Touch_XPT2046 _touch;
#else
  lgfx::Panel_ILI9341 _panel;
  lgfx::Touch_FT5x06 _touch;  // FT6206 is FT5x06-compatible
#endif
  lgfx::Bus_SPI _bus;

public:
  LGFX() {
    // ---------- SPI bus ----------
    {
      auto cfg = _bus.config();
      cfg.spi_host = SPI3_HOST;  // VSPI
      cfg.spi_mode = 0;
      cfg.freq_write = 27000000;  // lower to 20000000 if you see glitches
      cfg.freq_read = 16000000;
      cfg.spi_3wire = false;
      cfg.use_lock = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = PIN_SCLK;
      cfg.pin_mosi = PIN_MOSI;
      cfg.pin_miso = PIN_MISO;
      cfg.pin_dc = PIN_DC;
      _bus.config(cfg);
      _panel.setBus(&_bus);
    }

    // ---------- Panel ----------
    {
      auto cfg = _panel.config();
      cfg.pin_cs = PIN_CS;
      cfg.pin_rst = PIN_RST;
      cfg.pin_busy = -1;
#ifdef DISPLAY_ILI9488
      cfg.panel_width = 320;
      cfg.panel_height = 480;
      cfg.readable = false;   // display SDO not connected
      cfg.bus_shared = true;  // shared with XPT2046
#else
      cfg.panel_width = 240;
      cfg.panel_height = 320;
      cfg.readable = true;
      cfg.bus_shared = false;
#endif
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.invert = false;     // set true if colors look inverted
      cfg.rgb_order = false;  // set true if red/blue are swapped
      cfg.dlen_16bit = false;
      _panel.config(cfg);
    }

    // ---------- Touch ----------
    {
      auto cfg = _touch.config();
#ifdef DISPLAY_ILI9488
      cfg.x_min = 300;  // raw values, refined by calibration
      cfg.x_max = 3900;
      cfg.y_min = 200;
      cfg.y_max = 3800;
      cfg.pin_int = -1;
      cfg.bus_shared = true;
      cfg.offset_rotation = 0;
      cfg.spi_host = SPI3_HOST;
      cfg.freq = 1000000;
      cfg.pin_sclk = PIN_SCLK;
      cfg.pin_mosi = PIN_MOSI;
      cfg.pin_miso = PIN_MISO;
      cfg.pin_cs = PIN_T_CS;
#else
      cfg.x_min = 0;
      cfg.x_max = 239;
      cfg.y_min = 0;
      cfg.y_max = 319;
      cfg.pin_int = -1;
      cfg.bus_shared = false;
      cfg.offset_rotation = 0;
      cfg.i2c_port = 0;
      cfg.i2c_addr = 0x38;
      cfg.pin_sda = PIN_SDA;
      cfg.pin_scl = PIN_SCL;
      cfg.freq = 400000;
#endif
      _touch.config(cfg);
      _panel.setTouch(&_touch);
    }

    setPanel(&_panel);
  }
};

LGFX lcd;

// =====================================================================
//  APP
// =====================================================================
#ifdef DISPLAY_ILI9488
const char* DISPLAY_NAME = "ILI9488 OK";
#else
const char* DISPLAY_NAME = "ILI9341 OK";
#endif

uint16_t calData[8];

const int BTN_W = 90;
const int BTN_H = 40;
int btnX, btnY;
int headerBottom;

void centerText(const char* text, int y, const lgfx::IFont* font,
                uint16_t fg, uint16_t bg) {
  lcd.setFont(font);
  lcd.setTextColor(fg, bg);
  lcd.setTextDatum(textdatum_t::middle_center);
  lcd.drawString(text, lcd.width() / 2, y);
}

// ---------- 1) Display test ----------
void colorTest() {
  struct {
    uint16_t color;
    const char* name;
    uint16_t text;
  } tests[] = {
    { TFT_RED, "RED", TFT_WHITE },
    { TFT_GREEN, "GREEN", TFT_BLACK },
    { TFT_BLUE, "BLUE", TFT_WHITE },
    { TFT_WHITE, "WHITE", TFT_BLACK },
    { TFT_BLACK, "BLACK", TFT_WHITE },
  };

  for (auto& t : tests) {
    lcd.fillScreen(t.color);
    centerText(t.name, lcd.height() / 2, &fonts::Font4, t.text, t.color);
    delay(700);
  }
}

void shapesTest() {
  const int w = lcd.width();
  const int h = lcd.height();

  lcd.fillScreen(TFT_BLACK);
  lcd.drawRect(0, 0, w, h, TFT_WHITE);  // full-screen border

  centerText(DISPLAY_NAME, h * 0.08, &fonts::Font4, TFT_CYAN, TFT_BLACK);

  char size[24];
  snprintf(size, sizeof(size), "%d x %d", w, h);
  centerText(size, h * 0.17, &fonts::Font2, TFT_WHITE, TFT_BLACK);

  // Shapes scale with screen size
  const int s = min(w, h);
  lcd.fillCircle(w * 0.17, h * 0.50, s / 7, TFT_RED);
  lcd.fillRoundRect(w * 0.36, h * 0.36, w * 0.23, h * 0.28, 12, TFT_GREEN);
  lcd.fillTriangle(w * 0.69, h * 0.64, w * 0.79, h * 0.36, w * 0.90, h * 0.64, TFT_BLUE);

  // Gradient bar
  const int gx0 = 20, gx1 = w - 20;
  for (int x = gx0; x < gx1; x++) {
    uint8_t v = map(x, gx0, gx1, 0, 255);
    lcd.drawFastVLine(x, h * 0.74, h * 0.09, lcd.color565(v, 0, 255 - v));
  }

  centerText("Touch test starts in 3s...", h - 18, &fonts::Font2, TFT_YELLOW, TFT_BLACK);
  delay(3000);
}

// ---------- 2) Touch calibration (resistive only) ----------
void calibrateTouch() {
#ifdef DISPLAY_ILI9488
  lcd.fillScreen(TFT_BLACK);
  centerText("Touch each corner arrow", lcd.height() / 2 - 12, &fonts::Font2, TFT_WHITE, TFT_BLACK);
  centerText("as it appears", lcd.height() / 2 + 12, &fonts::Font2, TFT_WHITE, TFT_BLACK);

  lcd.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
  lcd.setTouchCalibrate(calData);

  // Print values so you can hard-code them later
  Serial.print("uint16_t calData[8] = { ");
  for (int i = 0; i < 8; i++) {
    Serial.print(calData[i]);
    if (i < 7) Serial.print(", ");
  }
  Serial.println(" };");
#else
  Serial.println("Capacitive touch: no calibration needed");
#endif
}

// ---------- 3) Paint screen ----------
void drawPaintScreen() {
  lcd.fillScreen(TFT_BLACK);

  lcd.setFont(&fonts::Font2);
  lcd.setTextColor(TFT_GREEN, TFT_BLACK);
  lcd.setTextDatum(textdatum_t::top_left);
  lcd.drawString("Touch OK - draw here", 10, 18);

  btnX = lcd.width() - BTN_W - 8;
  btnY = 6;
  lcd.fillRoundRect(btnX, btnY, BTN_W, BTN_H, 8, TFT_RED);
  lcd.setTextColor(TFT_WHITE, TFT_RED);
  lcd.setTextDatum(textdatum_t::middle_center);
  lcd.drawString("CLEAR", btnX + BTN_W / 2, btnY + BTN_H / 2);

  headerBottom = btnY + BTN_H + 6;
  lcd.drawFastHLine(0, headerBottom, lcd.width(), TFT_DARKGREY);
}

void showCoords(int x, int y) {
  char buf[32];
  snprintf(buf, sizeof(buf), "X:%d  Y:%d", x, y);
  lcd.setFont(&fonts::Font2);
  lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
  lcd.setTextDatum(textdatum_t::bottom_left);
  lcd.setTextPadding(160);
  lcd.drawString(buf, 10, lcd.height() - 6);
  lcd.setTextPadding(0);
}

// =====================================================================
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.printf("\n%s - TFT + touch test (LovyanGFX)\n", DISPLAY_NAME);

  lcd.init();
  lcd.setRotation(1);  // landscape — try 3 if upside down

  colorTest();
  shapesTest();
  calibrateTouch();
  drawPaintScreen();
}

void loop() {
  int32_t x, y;

  if (lcd.getTouch(&x, &y)) {
    // CLEAR button
    if (x >= btnX && x <= btnX + BTN_W && y >= btnY && y <= btnY + BTN_H) {
      drawPaintScreen();
      delay(250);
      return;
    }

    if (y > headerBottom + 4) {
      lcd.fillCircle(x, y, 3, TFT_CYAN);
    }

    showCoords(x, y);
    Serial.printf("Touch: x=%ld  y=%ld\n", (long)x, (long)y);
  }
}
