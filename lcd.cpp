#include <Arduino_GFX_Library.h>

#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST   4
#define TFT_SCK  18
#define TFT_MOSI 23
#define TFT_MISO 19

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO);
Arduino_GFX *gfx = new Arduino_ILI9488_18bit(bus, TFT_RST, 1 /* landscape */, false);

void setup() {
  gfx->begin();
  gfx->fillScreen(BLACK);

  gfx->setTextColor(WHITE);
  gfx->setTextSize(3);
  gfx->setCursor(20, 20);
  gfx->println("Hello ESP32!");

  gfx->setTextSize(2);
  gfx->setTextColor(CYAN);
  gfx->setCursor(20, 70);
  gfx->println("ILI9488 480x320 TFT");

  gfx->fillRect(20, 120, 120, 80, RED);
  gfx->fillCircle(230, 160, 40, GREEN);
  gfx->fillTriangle(320, 200, 380, 120, 440, 200, BLUE);
  gfx->drawRect(10, 10, 460, 300, YELLOW);
}

void loop() {
  uint16_t colors[] = {RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA};
  for (int i = 0; i < 7; i++) {
    gfx->fillRect(20 + i * 62, 240, 60, 50, colors[i]);
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
