#pragma once
// ============================================================================
//  ui.h  -  screen layout (landscape / portrait), drawing and animations
// ============================================================================

// ---------------------------------------------------------------- layout ----
struct Layout {
  int W, H;
  bool portrait;
  int div2Y;
  int clockX, clockY, clockW, clockH;
  int dateX, dateY, hijriY;  // dateX = left edge (landscape) / center (portrait)
  int mosqueCx, mosqueBase;
  bool mosqueWide;
  int wxX, wxY, wxW, wxH;          // weather panel
  int nextX, nextY, nextW, nextH;  // next prayer bar
  int pX, pY, pW, pH, pGap;        // prayer cards (landscape) / rows (portrait)
  int pillY, pillH, pillX[3], pillW[3];
  const int8_t* starDx;
  const int8_t* starDy;
  int starCount;
};
Layout L;

// twinkling star positions, relative to mosque center / base
const int8_t STARS_WIDE_DX[8] = { -60, -30, -26, 32, 26, 60, 64, -64 };
const int8_t STARS_WIDE_DY[8] = { -98, -88, -64, -90, -62, -100, -74, -70 };
const int8_t STARS_SLIM_DX[7] = { -44, 40, -24, 24, -14, 16, -48 };
const int8_t STARS_SLIM_DY[7] = { -100, -98, -78, -80, -104, -103, -56 };

void setupLayout(bool portrait) {
  L.portrait = portrait;
  if (!portrait) {  // ---- landscape 480 x 320
    L.W = 480;
    L.H = 320;
    L.div2Y = 155;
    L.clockX = 18;
    L.clockY = 44;
    L.clockW = 260;
    L.clockH = 72;
    L.dateX = 20;
    L.dateY = 116;
    L.hijriY = 134;
    L.mosqueCx = 420;
    L.mosqueBase = 150;
    L.mosqueWide = false;
    L.wxX = 284;
    L.wxY = 46;
    L.wxW = 82;
    L.wxH = 104;
    L.nextX = 14;
    L.nextY = 160;
    L.nextW = 452;
    L.nextH = 34;
    L.pX = 15;
    L.pY = 200;
    L.pW = 86;
    L.pH = 76;
    L.pGap = 5;
    L.pillY = 282;
    L.pillH = 28;
    L.pillX[0] = 14;
    L.pillX[1] = 246;
    L.pillX[2] = 376;  // unused (fire pill removed)
    L.pillW[0] = 220;
    L.pillW[1] = 220;
    L.pillW[2] = 90;  // unused (fire pill removed)
    L.starDx = STARS_SLIM_DX;
    L.starDy = STARS_SLIM_DY;
    L.starCount = 7;
  } else {  // ---- portrait 320 x 480
    L.W = 320;
    L.H = 480;
    L.div2Y = 268;
    L.clockX = 12;
    L.clockY = 42;
    L.clockW = 296;
    L.clockH = 72;
    L.dateX = 160;
    L.dateY = 116;
    L.hijriY = 134;
    L.mosqueCx = 84;
    L.mosqueBase = 262;
    L.mosqueWide = true;
    L.wxX = 160;
    L.wxY = 162;
    L.wxW = 148;
    L.wxH = 100;
    L.nextX = 12;
    L.nextY = 274;
    L.nextW = 296;
    L.nextH = 34;
    L.pX = 12;
    L.pY = 314;
    L.pW = 296;
    L.pH = 22;
    L.pGap = 2;
    L.pillY = 438;
    L.pillH = 28;
    L.pillX[0] = 12;
    L.pillX[1] = 164;
    L.pillX[2] = 218;  // unused (fire pill removed)
    L.pillW[0] = 144;
    L.pillW[1] = 144;
    L.pillW[2] = 90;  // unused (fire pill removed)
    L.starDx = STARS_WIDE_DX;
    L.starDy = STARS_WIDE_DY;
    L.starCount = 8;
  }
}

LGFX_Sprite sprClock(&lcd), sprNext(&lcd), sprCard(&lcd), sprPill(&lcd);

enum CardState { CARD_NORMAL,
                 CARD_PASSED,
                 CARD_NEXT,
                 CARD_ACTIVE };

// --------------------------------------------------------------- helpers ----
uint16_t blend565(uint16_t a, uint16_t b, uint8_t t) {  // t: 0 = a, 255 = b
  int ar = (a >> 11) & 31, ag = (a >> 5) & 63, ab = a & 31;
  int br = (b >> 11) & 31, bg = (b >> 5) & 63, bb = b & 31;
  int r = ar + (br - ar) * t / 255, g = ag + (bg - ag) * t / 255, bl = ab + (bb - ab) * t / 255;
  return (uint16_t)((r << 11) | (g << 5) | bl);
}

uint8_t pulse8(uint32_t periodMs, uint32_t offsetMs = 0) {  // 0 -> 255 -> 0
  uint32_t t = (millis() + offsetMs) % periodMs, half = periodMs / 2;
  return t < half ? t * 255 / half : (periodMs - t) * 255 / half;
}

bool makeSprite(LGFX_Sprite& s, int w, int h) {
  s.setColorDepth(16);
  if (s.createSprite(w, h)) return true;
  s.setColorDepth(8);  // low memory fallback
  return s.createSprite(w, h);
}

void fillDiamond(LovyanGFX& g, int cx, int cy, int r, uint16_t c) {
  g.fillTriangle(cx - r, cy, cx, cy - r, cx + r, cy, c);
  g.fillTriangle(cx - r, cy, cx, cy + r, cx + r, cy, c);
}

void fillStar8(LovyanGFX& g, int cx, int cy, int r, uint16_t c) {  // 8 point star
  int s = r * 707 / 1000;
  g.fillRect(cx - s, cy - s, 2 * s + 1, 2 * s + 1, c);
  fillDiamond(g, cx, cy, r, c);
}

void drawRubElHizb(LovyanGFX& g, int cx, int cy, int r, uint16_t c, uint16_t inner) {
  fillStar8(g, cx, cy, r, c);
  g.fillCircle(cx, cy, r * 45 / 100, inner);
  g.drawCircle(cx, cy, r * 45 / 100, c);
}

void fillCrescent(LovyanGFX& g, int cx, int cy, int r, uint16_t c, uint16_t bg) {
  g.fillCircle(cx, cy, r, c);
  g.fillCircle(cx + r / 2, cy - r / 4, r * 80 / 100, bg);
}

const int8_t RAY_DX[8] = { 10, 7, 0, -7, -10, -7, 0, 7 };
const int8_t RAY_DY[8] = { 0, 7, 10, 7, 0, -7, -10, -7 };
void drawRays(LovyanGFX& g, int cx, int cy, int r1, int r2, uint16_t c, bool upperOnly) {
  for (int i = 0; i < 8; i++) {
    if (upperOnly && RAY_DY[i] > 0) continue;
    g.drawLine(cx + RAY_DX[i] * r1 / 10, cy + RAY_DY[i] * r1 / 10,
               cx + RAY_DX[i] * r2 / 10, cy + RAY_DY[i] * r2 / 10, c);
  }
}

// draws a number with a small degree circle after it (datum = middle_center)
void drawDegrees(LovyanGFX& g, const char* txt, int cx, int cy, uint16_t c) {
  int w = g.textWidth(txt), hgt = g.fontHeight();
  g.setTextColor(c);
  g.setTextDatum(textdatum_t::middle_center);
  g.drawString(txt, cx - 3, cy);
  g.drawCircle(cx - 3 + w / 2 + 5, cy - hgt / 4 - 1, 3, c);
  g.drawCircle(cx - 3 + w / 2 + 5, cy - hgt / 4 - 1, 2, c);
}

// ----------------------------------------------------------- prayer icons ----
void drawPrayerIcon(LovyanGFX& g, int i, int cx, int cy, uint16_t c, uint16_t bg) {
  switch (i) {
    case 0:  // Fajr: sun rising
      g.fillCircle(cx, cy + 3, 6, c);
      g.fillRect(cx - 9, cy + 4, 19, 6, bg);
      g.drawFastHLine(cx - 11, cy + 4, 23, c);
      drawRays(g, cx, cy + 3, 8, 11, c, true);
      break;
    case 1:  // Dhuhr: full sun
      g.fillCircle(cx, cy, 5, c);
      drawRays(g, cx, cy, 7, 10, c, false);
      break;
    case 2:  // Asr: sun with ring
      g.fillCircle(cx, cy, 5, c);
      g.drawCircle(cx, cy, 8, c);
      break;
    case 3:  // Maghrib: sun setting
      g.fillCircle(cx, cy + 4, 7, c);
      g.fillRect(cx - 9, cy + 5, 19, 8, bg);
      g.drawFastHLine(cx - 11, cy + 5, 23, c);
      g.drawFastHLine(cx - 7, cy + 8, 15, c);
      break;
    default:  // Isha: crescent + star
      fillCrescent(g, cx - 2, cy, 7, c, bg);
      fillStar8(g, cx + 7, cy - 4, 3, c);
      break;
  }
}

// ---------------------------------------------------------- weather icons ----
// WMO code -> 0 clear, 1 partly, 2 cloudy, 3 fog, 4 rain, 5 snow, 6 storm, -1 unknown
int weatherType(int code) {
  if (code == 0) return 0;
  if (code == 1 || code == 2) return 1;
  if (code == 3) return 2;
  if (code == 45 || code == 48) return 3;
  if ((code >= 51 && code <= 67) || (code >= 80 && code <= 82)) return 4;
  if ((code >= 71 && code <= 77) || code == 85 || code == 86) return 5;
  if (code >= 95 && code <= 99) return 6;
  return -1;
}

const char* weatherText(int code) {
  if (code >= 51 && code <= 57) return "Drizzle";
  if (code >= 80 && code <= 82) return "Showers";
  switch (weatherType(code)) {
    case 0: return "Clear";
    case 1: return "Partly cloudy";
    case 2: return "Cloudy";
    case 3: return "Fog";
    case 4: return "Rain";
    case 5: return "Snow";
    case 6: return "Thunderstorm";
  }
  return "Weather...";
}

void fillCloud(LovyanGFX& g, int cx, int cy, int r, uint16_t c) {
  g.fillCircle(cx - r * 6 / 10, cy + r * 2 / 10, r * 7 / 10, c);
  g.fillCircle(cx + r * 1 / 10, cy - r * 2 / 10, r * 9 / 10, c);
  g.fillCircle(cx + r * 8 / 10, cy + r * 25 / 100, r * 6 / 10, c);
  g.fillRoundRect(cx - r * 13 / 10, cy + r * 2 / 10, r * 27 / 10, r * 7 / 10, r * 3 / 10, c);
}

void drawSun(LovyanGFX& g, int cx, int cy, int r) {
  g.fillCircle(cx, cy, r, C_SUN);
  drawRays(g, cx, cy, r + 2, r + 3 + r / 2, C_SUN, false);
}

// r = size unit (8 small, 12 large)
void drawWeatherIcon(LovyanGFX& g, int code, bool isDay, int cx, int cy, int r) {
  switch (weatherType(code)) {
    case 0:
      if (isDay) drawSun(g, cx, cy, r);
      else fillCrescent(g, cx, cy, r + 2, C_GOLD_LIGHT, C_PANEL);
      break;
    case 1:
      if (isDay) drawSun(g, cx - r / 2, cy - r / 2, r * 6 / 10);
      else fillCrescent(g, cx - r / 2, cy - r / 2, r * 8 / 10, C_GOLD_LIGHT, C_PANEL);
      fillCloud(g, cx + r / 4, cy + r / 3, r * 8 / 10, C_CLOUD);
      break;
    case 2:
      fillCloud(g, cx + r / 3, cy - r / 3, r * 7 / 10, C_MUTED);
      fillCloud(g, cx - r / 6, cy + r / 6, r, C_CLOUD);
      break;
    case 3:
      for (int k = 0; k < 4; k++) {
        int y = cy - r * 6 / 10 + k * r * 4 / 10, x = cx - r + (k % 2) * r / 3;
        g.fillRoundRect(x, y, r * 2 - r / 3, 2, 1, C_CLOUD);
      }
      break;
    case 4:
      fillCloud(g, cx, cy - r / 3, r, C_CLOUD);
      for (int k = -1; k <= 1; k++)
        g.drawLine(cx + k * r / 2, cy + r * 7 / 10, cx + k * r / 2 - r / 4, cy + r * 12 / 10, C_WATER);
      break;
    case 5:
      fillCloud(g, cx, cy - r / 3, r, C_CLOUD);
      for (int k = -1; k <= 1; k++) g.fillCircle(cx + k * r / 2, cy + r, 1 + r / 8, C_CREAM);
      break;
    case 6:
      fillCloud(g, cx, cy - r / 3, r, C_MUTED);
      g.fillTriangle(cx + r / 6, cy + r / 3, cx - r / 3, cy + r * 8 / 10, cx + r / 12, cy + r * 8 / 10, C_SUN);
      g.fillTriangle(cx - r / 12, cy + r * 7 / 10, cx + r / 3, cy + r * 7 / 10, cx - r / 4, cy + r * 13 / 10, C_SUN);
      break;
    default:
      fillCloud(g, cx, cy, r, C_PANEL_HI);
      break;
  }
}

// --------------------------------------------------------- static parts ----
void drawFrame(uint16_t c) {
  lcd.drawRect(2, 2, L.W - 4, L.H - 4, c);
  lcd.drawRect(3, 3, L.W - 6, L.H - 6, c);
  lcd.drawRect(7, 7, L.W - 14, L.H - 14, C_GOLD_DIM);
  const int cx[4] = { 7, L.W - 8, 7, L.W - 8 }, cy[4] = { 7, 7, L.H - 8, L.H - 8 };
  for (int i = 0; i < 4; i++) fillDiamond(lcd, cx[i], cy[i], 5, c);
}

void drawDivider(int y, bool centerStar) {
  lcd.drawFastHLine(18, y, L.W - 36, C_GOLD_DIM);
  for (int x = 30; x < L.W - 20; x += 24) fillDiamond(lcd, x, y, 3, C_GOLD_DIM);
  if (centerStar) drawRubElHizb(lcd, L.W / 2, y, 6, C_GOLD, C_BG);
}

// wide = full mosque (portrait), slim = narrower (landscape)
void drawMosque(int cx, int base, bool wide) {
  uint16_t body = C_GOLD_DIM, dome = C_GOLD, dark = C_BG;
  int mo = wide ? 46 : 38;  // minaret offset
  int hw = wide ? 32 : 26;  // hall half width
  int cw = wide ? 36 : 29;  // cornice half width
  int dw = wide ? 24 : 19;  // drum half width
  int dr = wide ? 20 : 18;  // dome radius
  int wo = wide ? 20 : 16;  // window offset

  lcd.fillRoundRect(cx - mo - 18, base - 2, 2 * (mo + 18), 3, 1, C_GOLD_DIM);  // ground
  for (int s = -1; s <= 1; s += 2) {                                           // minarets
    int x = cx + s * mo;
    lcd.fillRect(x - 4, base - 64, 9, 62, body);
    lcd.fillRect(x - 7, base - 46, 15, 3, dome);
    lcd.fillRect(x - 6, base - 67, 13, 3, dome);
    lcd.fillTriangle(x - 5, base - 67, x + 5, base - 67, x, base - 84, dome);
    lcd.fillCircle(x, base - 86, 2, dome);
    lcd.fillRoundRect(x - 1, base - 38, 3, 8, 1, dark);
    lcd.fillRoundRect(x - 1, base - 60, 3, 8, 1, dark);
  }
  lcd.fillCircle(cx, base - 60, dr, dome);  // onion dome
  lcd.fillTriangle(cx - 11, base - 60 - dr + 5, cx + 11, base - 60 - dr + 5, cx, base - 90, dome);
  lcd.drawFastVLine(cx, base - 96, 7, dome);
  fillCrescent(lcd, cx, base - 99, 4, dome, C_BG);
  lcd.fillRect(cx - dw, base - 46, 2 * dw + 1, 7, body);   // drum
  lcd.fillRect(cx - cw, base - 40, 2 * cw + 1, 4, dome);   // cornice
  lcd.fillRect(cx - hw, base - 36, 2 * hw + 1, 34, body);  // hall
  lcd.fillRect(cx - 8, base - 20, 17, 18, dark);           // door
  lcd.fillCircle(cx, base - 20, 8, dark);
  for (int s = -1; s <= 1; s += 2) {  // windows
    int x = cx + s * wo;
    lcd.fillRect(x - 3, base - 26, 7, 12, dark);
    lcd.fillCircle(x, base - 26, 3, dark);
  }
}

// twinkling stars (animation)
void drawStars() {
  for (int i = 0; i < L.starCount; i++) {
    uint8_t b = pulse8(1800 + i * 230, i * 400);
    uint16_t c = blend565(C_PANEL, C_GOLD_LIGHT, b);
    int x = L.mosqueCx + L.starDx[i], y = L.mosqueBase + L.starDy[i], r = 1 + b / 110;
    lcd.fillRect(x - 3, y - 3, 7, 7, C_BG);
    lcd.drawFastHLine(x - r, y, 2 * r + 1, c);
    lcd.drawFastVLine(x, y - r, 2 * r + 1, c);
  }
}

// ------------------------------------------------------------- top bar ----
void drawWifiIcon(int x, int y) {  // x,y = bottom-left
  int bars = !wifiConnected ? 0 : wifiRssi > -55 ? 4
                                : wifiRssi > -65 ? 3
                                : wifiRssi > -75 ? 2
                                                 : 1;
  for (int i = 0; i < 4; i++) {
    int h = 4 + i * 4;
    lcd.fillRect(x + i * 6, y - h, 4, h, i < bars ? C_GOLD : C_PANEL_HI);
  }
  if (!wifiConnected) {
    lcd.drawLine(x, y - 16, x + 22, y, C_FIRE);
    lcd.drawLine(x + 1, y - 16, x + 23, y, C_FIRE);
  }
}

void drawTopBar() {
  lcd.fillRect(12, 10, L.W - 24, 24, C_BG);
  drawRubElHizb(lcd, 24, 22, 8, C_GOLD, C_BG);
  lcd.setFont(&fonts::FreeSansBold9pt7b);
  lcd.setTextColor(C_GOLD);
  lcd.setTextDatum(textdatum_t::middle_left);
  lcd.drawString(L.portrait ? "MAKKAH" : "MAKKAH AL-MUKARRAMAH", 40, 22);

  // data source tag
  const char* src = timesSource == SRC_LIVE ? "LIVE" : timesSource == SRC_SAVED ? "SAVED"
                                                                                : "DEFAULT";
  uint16_t srcC = timesSource == SRC_LIVE ? C_OK : timesSource == SRC_SAVED ? C_WARN
                                                                            : C_FIRE;
  lcd.setFont(&fonts::DejaVu12);
  int w = lcd.textWidth(src) + 14;
  int x = L.W - 48 - w;
  lcd.drawRoundRect(x, 14, w, 17, 8, srcC);
  lcd.setTextColor(srcC);
  lcd.setTextDatum(textdatum_t::middle_center);
  lcd.drawString(src, x + w / 2, 23);

  // wifi status text
  if (!wifiConnected) {
    lcd.setTextColor(wifiEverConnected ? C_FIRE : C_WARN);
    lcd.setTextDatum(textdatum_t::middle_right);
    lcd.drawString(wifiEverConnected ? "NO WIFI" : "WIFI...", x - 8, 23);
  }
  drawWifiIcon(L.W - 38, 30);
}

// ---------------------------------------------------------------- clock ----
void drawClock() {
  LGFX_Sprite& s = sprClock;
  s.fillScreen(C_BG);
  s.setTextDatum(textdatum_t::top_left);
  s.setFont(&fonts::DejaVu72);
  if (!timeValid) {
    s.setTextColor(C_MUTED);
    int x = L.portrait ? (L.clockW - s.textWidth("--:--")) / 2 : 0;
    s.drawString("--:--", x, 0);
    s.pushSprite(L.clockX, L.clockY);
    return;
  }
  char hm[8];
  const char* ampm;
  formatMinute(nowMinuteOfDay(), hm, sizeof(hm), &ampm);
  char sec[4];
  snprintf(sec, sizeof(sec), "%02d", nowTm.tm_sec);

  int hmW = s.textWidth(hm);
  s.setFont(&fonts::DejaVu24);
  int secW = s.textWidth(sec);
  s.setFont(&fonts::FreeSansBold12pt7b);
  int apW = ampm[0] ? s.textWidth(ampm) : 0;
  int total = hmW + 8 + (secW > apW ? secW : apW);
  int x0 = L.portrait ? (L.clockW - total) / 2 : 0;

  s.setFont(&fonts::DejaVu72);
  s.setTextColor(C_CREAM);
  s.drawString(hm, x0, 0);
  int x = x0 + hmW + 8;
  s.setFont(&fonts::DejaVu24);
  s.setTextColor(C_GOLD);
  s.drawString(sec, x, 12);
  if (ampm[0]) {
    s.setFont(&fonts::FreeSansBold12pt7b);
    s.setTextColor(C_GOLD_LIGHT);
    s.drawString(ampm, x, 40);
  }
  s.pushSprite(L.clockX, L.clockY);
}

void drawDates() {
  lcd.fillRect(L.clockX, L.dateY, L.clockW, 36, C_BG);
  lcd.setFont(&fonts::DejaVu18);
  lcd.setTextDatum(L.portrait ? textdatum_t::top_center : textdatum_t::top_left);
  if (timeValid) {
    char buf[40];
    strftime(buf, sizeof(buf), "%A  %d %b %Y", &nowTm);
    lcd.setTextColor(C_CREAM);
    lcd.drawString(buf, L.dateX, L.dateY);
  }
  if (hijriText[0]) {
    char buf[40];
    snprintf(buf, sizeof(buf), "%s AH", hijriText);
    lcd.setTextColor(C_GOLD);
    lcd.drawString(buf, L.dateX, L.hijriY);
  } else {
    lcd.setTextColor(C_MUTED);
    lcd.drawString("Waiting for internet...", L.dateX, L.hijriY);
  }
}

// -------------------------------------------------------------- weather ----
void drawWeather() {
  int x = L.wxX, y = L.wxY, w = L.wxW, h = L.wxH;
  bool ok = weather.valid;
  char t[32];
  lcd.fillRoundRect(x, y, w, h, 8, C_PANEL);
  lcd.drawRoundRect(x, y, w, h, 8, C_GOLD_DIM);

  if (ok) snprintf(t, sizeof(t), "%d", (int)lroundf(weather.temp));
  else snprintf(t, sizeof(t), "--");

  if (!L.portrait) {  // compact column
    drawWeatherIcon(lcd, ok ? weather.code : -1, weather.isDay, x + w / 2, y + 22, 9);
    lcd.setFont(&fonts::FreeSansBold18pt7b);
    drawDegrees(lcd, t, x + w / 2, y + 54, C_CREAM);
    lcd.setFont(&fonts::DejaVu9);
    lcd.setTextDatum(textdatum_t::middle_center);
    lcd.setTextColor(C_GOLD);
    lcd.drawString(ok ? weatherText(weather.code) : "Weather...", x + w / 2, y + 76);
    if (ok) {
      snprintf(t, sizeof(t), "H %d  L %d", (int)lroundf(weather.tMax), (int)lroundf(weather.tMin));
      lcd.setTextColor(C_CREAM);
      lcd.drawString(t, x + w / 2, y + 87);
      snprintf(t, sizeof(t), "Hum %d%%", weather.humidity);
      lcd.setTextColor(C_WATER);
      lcd.drawString(t, x + w / 2, y + 97);
    }
    return;
  }
  // large panel (portrait)
  drawWeatherIcon(lcd, ok ? weather.code : -1, weather.isDay, x + 32, y + 30, 13);
  lcd.setFont(&fonts::FreeSansBold18pt7b);
  drawDegrees(lcd, t, x + 96, y + 26, C_CREAM);
  lcd.setFont(&fonts::DejaVu12);
  lcd.setTextDatum(textdatum_t::middle_center);
  if (ok) {
    snprintf(t, sizeof(t), "H %d  L %d", (int)lroundf(weather.tMax), (int)lroundf(weather.tMin));
    lcd.setTextColor(C_CREAM);
    lcd.drawString(t, x + 98, y + 50);
  }
  lcd.setTextDatum(textdatum_t::middle_left);
  lcd.setTextColor(C_GOLD);
  lcd.drawString(ok ? weatherText(weather.code) : "Loading...", x + 12, y + 70);
  if (ok) {
    lcd.fillCircle(x + 15, y + 89, 3, C_WATER);  // small drop = humidity
    lcd.fillTriangle(x + 12, y + 88, x + 18, y + 88, x + 15, y + 83, C_WATER);
    lcd.setFont(&fonts::DejaVu9);
    snprintf(t, sizeof(t), "%d%%", weather.humidity);
    lcd.setTextColor(C_WATER);
    lcd.drawString(t, x + 22, y + 88);
    snprintf(t, sizeof(t), "Wind %d km/h", (int)lroundf(weather.wind));
    lcd.setTextColor(C_MUTED);
    lcd.setTextDatum(textdatum_t::middle_right);
    lcd.drawString(t, x + w - 12, y + 88);
  }
}

// ---------------------------------------------------- next prayer bar ----
void drawNextBar() {
  LGFX_Sprite& s = sprNext;
  int W = L.nextW, H = L.nextH;
  s.fillScreen(C_BG);
  int act = timeValid ? activePrayer() : -1;
  uint16_t fill = act >= 0 ? blend565(C_PANEL, C_GOLD_DIM, pulse8(1200)) : C_PANEL;
  s.fillRoundRect(0, 0, W, H, 8, fill);
  s.drawRoundRect(0, 0, W, H, 8, act >= 0 ? C_GOLD : C_GOLD_DIM);

  if (!timeValid) {  // boot / offline messages
    char msg[64];
    if (!wifiConnected && !wifiEverConnected) snprintf(msg, sizeof(msg), "Connecting to %s...", WIFI_SSID);
    else if (!wifiConnected) snprintf(msg, sizeof(msg), "WiFi lost - reconnecting...");
    else snprintf(msg, sizeof(msg), "Syncing Makkah time...");
    s.setFont(&fonts::FreeSansBold9pt7b);
    if (s.textWidth(msg) > W - 20) s.setFont(&fonts::DejaVu12);
    s.setTextColor(wifiConnected ? C_CREAM : C_WARN);
    s.setTextDatum(textdatum_t::middle_center);
    s.drawString(msg, W / 2, H / 2);
    int dotX = (millis() / 40) % (W - 40) + 20;  // moving dot
    s.fillCircle(dotX, H - 4, 2, C_GOLD);
    s.pushSprite(L.nextX, L.nextY);
    return;
  }

  if (act >= 0) {  // prayer time now
    char msg[40];
    snprintf(msg, sizeof(msg), "TIME FOR %s PRAYER", prayers[act].name);
    for (char* c = msg; *c; c++) *c = toupper(*c);
    s.setFont(&fonts::FreeSansBold12pt7b);
    bool big = s.textWidth(msg) <= W - 80;
    if (!big) s.setFont(&fonts::FreeSansBold9pt7b);
    s.setTextColor(blend565(C_GOLD_LIGHT, C_CREAM, pulse8(800)));
    s.setTextDatum(textdatum_t::middle_center);
    s.drawString(msg, W / 2, H / 2 + 1);
    if (big) {
      fillStar8(s, 26, H / 2, 7, C_GOLD_LIGHT);
      fillStar8(s, W - 26, H / 2, 7, C_GOLD_LIGHT);
    }
    s.pushSprite(L.nextX, L.nextY);
    return;
  }

  int nx = nextPrayer();
  int x = 12;
  s.setTextDatum(textdatum_t::middle_left);
  if (!L.portrait) {
    s.setFont(&fonts::DejaVu12);
    s.setTextColor(C_MUTED);
    s.drawString("NEXT", x, 14);
    x = 50;
  }
  s.setFont(L.portrait ? &fonts::FreeSansBold9pt7b : &fonts::FreeSansBold12pt7b);
  s.setTextColor(C_GOLD);
  s.drawString(prayers[nx].name, x, 14);
  x += s.textWidth(prayers[nx].name) + (L.portrait ? 8 : 12);

  char hm[8];
  const char* ampm;
  formatMinute(prayers[nx].minute, hm, sizeof(hm), &ampm);
  s.setFont(&fonts::DejaVu18);
  s.setTextColor(C_CREAM);
  s.drawString(hm, x, 14);
  x += s.textWidth(hm) + 4;
  s.setFont(&fonts::DejaVu12);
  s.drawString(ampm, x, 16);

  long left = secondsToPrayer(nx);
  char cd[20];
  if (left >= 3600) snprintf(cd, sizeof(cd), "in %ld:%02ld:%02ld", left / 3600, (left / 60) % 60, left % 60);
  else snprintf(cd, sizeof(cd), "in %ld:%02ld", left / 60, left % 60);
  s.setFont(&fonts::DejaVu18);
  s.setTextColor(left < 600 ? C_GOLD_LIGHT : C_CREAM);
  s.setTextDatum(textdatum_t::middle_right);
  s.drawString(cd, W - 12, 14);

  // progress bar with moving shine
  const int bx = 12, by = 26, bw = W - 24, bh = 4;
  int fillW = bw * progressToNext(nx) / 1000;
  s.fillRect(bx, by, bw, bh, C_PANEL_DARK);
  s.fillRect(bx, by, fillW, bh, C_GOLD);
  if (fillW > 10) {
    int shine = (millis() / 12) % (fillW + 40) - 20;
    for (int i = -20; i <= 20; i++) {
      int px = shine + i;
      if (px < 0 || px >= fillW) continue;
      uint8_t t = 255 - abs(i) * 12;
      s.drawFastVLine(bx + px, by, bh, blend565(C_GOLD, C_CREAM, t));
    }
  }
  s.fillCircle(bx + fillW, by + 2, 3, C_GOLD_LIGHT);
  s.pushSprite(L.nextX, L.nextY);
}

// ----------------------------------------------- prayer cards / rows ----
int cardStateOf(int i) {  // returns a CardState
  if (!timeValid) return CARD_NORMAL;
  int act = activePrayer();
  if (i == act) return CARD_ACTIVE;
  if (act < 0 && i == nextPrayer()) return CARD_NEXT;
  if (act >= 0 && i == (act + 1) % 5) return CARD_NEXT;
  if (prayers[i].minute <= nowMinuteOfDay()) return CARD_PASSED;
  return CARD_NORMAL;
}

struct CardColors {
  uint16_t border, fill, name, time, icon;
  int bw;
};

CardColors cardColors(int st) {
  uint8_t p = pulse8(1000);
  switch (st) {
    case CARD_ACTIVE: return { blend565(C_GOLD, C_CREAM, p), blend565(C_GOLD, C_GOLD_LIGHT, p), C_BG, C_BG, C_BG, 3 };
    case CARD_NEXT: return { C_GOLD, C_PANEL_HI, C_GOLD_LIGHT, C_CREAM, C_GOLD_LIGHT, 3 };
    case CARD_PASSED: return { C_PANEL_HI, C_PANEL_DARK, C_MUTED, C_MUTED, C_MUTED, 2 };
    default: return { C_GOLD_DIM, C_PANEL, C_GOLD, C_CREAM, C_GOLD, 2 };
  }
}

// landscape: arch shaped card
void drawCard(int i) {
  LGFX_Sprite& s = sprCard;
  CardColors c = cardColors(cardStateOf(i));
  const int W = L.pW, H = L.pH, cx = W / 2, ay = 28, ry = 22, bw = c.bw;
  s.fillScreen(C_BG);
  fillDiamond(s, cx, 4, 4, c.border);           // finial
  s.fillEllipse(cx, ay, cx - 1, ry, c.border);  // arch outline
  s.fillRoundRect(1, ay, W - 2, H - ay, 6, c.border);
  s.fillEllipse(cx, ay, cx - 1 - bw, ry - bw, c.fill);  // arch inside
  s.fillRoundRect(1 + bw, ay, W - 2 - 2 * bw, H - ay - bw, 5, c.fill);

  drawPrayerIcon(s, i, cx, 21, c.icon, c.fill);

  s.setTextDatum(textdatum_t::middle_center);
  s.setFont(&fonts::FreeSansBold9pt7b);
  s.setTextColor(c.name);
  s.drawString(prayers[i].name, cx, 42);

  char hm[8];
  const char* ampm;
  formatMinute(prayers[i].minute, hm, sizeof(hm), &ampm);
  s.setFont(&fonts::DejaVu18);
  int tw = s.textWidth(hm);
  s.setFont(&fonts::DejaVu9);
  int aw = ampm[0] ? s.textWidth(ampm) + 2 : 0;
  int tx = cx - (tw + aw) / 2;
  s.setTextDatum(textdatum_t::middle_left);
  s.setFont(&fonts::DejaVu18);
  s.setTextColor(c.time);
  s.drawString(hm, tx, 61);
  if (ampm[0]) {
    s.setFont(&fonts::DejaVu9);
    s.drawString(ampm, tx + tw + 2, 64);
  }
  s.pushSprite(L.pX + i * (L.pW + L.pGap), L.pY);
}

// portrait: one row per prayer
void drawRow(int i) {
  LGFX_Sprite& s = sprCard;
  int st = cardStateOf(i);
  CardColors c = cardColors(st);
  const int W = L.pW, H = L.pH, bw = c.bw - 1;
  s.fillScreen(C_BG);
  s.fillRoundRect(0, 0, W, H, 7, c.border);
  s.fillRoundRect(bw, bw, W - 2 * bw, H - 2 * bw, 6, c.fill);
  fillDiamond(s, 5, H / 2, 3, c.border);

  drawPrayerIcon(s, i, 24, H / 2 - 1, c.icon, c.fill);

  s.setTextDatum(textdatum_t::middle_left);
  s.setFont(&fonts::FreeSansBold9pt7b);
  s.setTextColor(c.name);
  s.drawString(prayers[i].name, 44, H / 2);
  if (st == CARD_NEXT || st == CARD_ACTIVE) {
    int nx = 44 + s.textWidth(prayers[i].name) + 8;
    s.setFont(&fonts::DejaVu9);
    s.setTextColor(st == CARD_ACTIVE ? C_BG : C_GOLD);
    s.drawString(st == CARD_ACTIVE ? "NOW" : "NEXT", nx, H / 2 + 1);
  }

  char hm[8];
  const char* ampm;
  formatMinute(prayers[i].minute, hm, sizeof(hm), &ampm);
  s.setTextDatum(textdatum_t::middle_right);
  s.setFont(&fonts::DejaVu9);
  int aw = ampm[0] ? s.textWidth(ampm) + 3 : 0;
  s.setTextColor(c.time);
  if (ampm[0]) s.drawString(ampm, W - 10, H / 2 + 2);
  s.setFont(&fonts::DejaVu18);
  s.drawString(hm, W - 10 - aw, H / 2 + 1);
  s.pushSprite(L.pX, L.pY + i * (L.pH + L.pGap));
}

void drawPrayer(int i) {
  if (L.portrait) drawRow(i);
  else drawCard(i);
}
void drawAllPrayers() {
  for (int i = 0; i < 5; i++) drawPrayer(i);
}

// ------------------------------------------- status pills (bottom bar) ----
void pillBase(int w, uint16_t fill, uint16_t border) {
  sprPill.fillScreen(C_BG);
  sprPill.fillRoundRect(0, 0, w, L.pillH, L.pillH / 2, border);
  sprPill.fillRoundRect(2, 2, w - 4, L.pillH - 4, L.pillH / 2 - 2, fill);
}

// label is hidden in portrait (not enough width, the icon explains it)
void pillText(int w, const char* label, const char* value, uint16_t labelC, uint16_t valueC) {
  LGFX_Sprite& s = sprPill;
  if (!L.portrait) {
    s.setTextDatum(textdatum_t::middle_left);
    s.setFont(&fonts::DejaVu12);
    s.setTextColor(labelC);
    s.drawString(label, 30, L.pillH / 2);
  }
  s.setTextDatum(textdatum_t::middle_right);
  s.setFont(&fonts::FreeSansBold9pt7b);
  s.setTextColor(valueC);
  s.drawString(value, w - 10, L.pillH / 2);
}

void drawLedPill() {
  LGFX_Sprite& s = sprPill;
  int w = L.pillW[0];
  bool on = ledIsOn;
  pillBase(w, on ? rgb(74, 58, 12) : C_PANEL, on ? C_GOLD : C_GOLD_DIM);
  uint16_t c = on ? rgb(255, 220, 90) : C_MUTED;
  s.fillCircle(16, 12, 6, c);  // bulb
  s.fillRect(13, 17, 7, 5, on ? C_GOLD_DIM : C_PANEL_HI);
  if (on) drawRays(s, 16, 12, 8, 10, c, true);
  pillText(w, "LED STRIP", on ? "ON" : "OFF", on ? C_GOLD_LIGHT : C_MUTED, on ? C_GOLD_LIGHT : C_MUTED);
  s.pushSprite(L.pillX[0], L.pillY);
}

void drawMistPill() {
  LGFX_Sprite& s = sprPill;
  int w = L.pillW[1];
  bool on = mistIsOn;
  pillBase(w, on ? rgb(10, 58, 100) : C_PANEL, on ? C_WATER : C_GOLD_DIM);
  uint16_t c = on ? C_WATER : C_MUTED;
  s.fillCircle(16, 16, 5, c);  // water drop
  s.fillTriangle(11, 14, 21, 14, 16, 5, c);
  if (on) {  // falling droplets
    for (int k = 0; k < 3; k++) {
      int y = 4 + (millis() / 25 + k * 7) % 20;
      s.fillCircle(24 + (k % 2) * 2, y, 1, C_CREAM);
    }
  }
  char val[12];
  if (on && !fireDetected && mistOffInMs > 0) snprintf(val, sizeof(val), "ON %lus", (unsigned long)(mistOffInMs + 999) / 1000);
  else snprintf(val, sizeof(val), "%s", on ? "ON" : "OFF");
  pillText(w, "WATER MIST", val, on ? C_CREAM : C_MUTED, on ? C_CREAM : C_MUTED);
  s.pushSprite(L.pillX[1], L.pillY);
}

// ---------------------------------------------------------- full screen ----
void drawStaticScreen() {
  lcd.fillScreen(C_BG);
  drawFrame(C_GOLD);
  drawDivider(39, true);
  drawDivider(L.div2Y, false);
  drawMosque(L.mosqueCx, L.mosqueBase, L.mosqueWide);
}

void createSprites() {
  sprClock.setPsram(true);
  sprNext.setPsram(true);
  sprCard.setPsram(true);
  sprPill.setPsram(true);
  makeSprite(sprClock, L.clockW, L.clockH);
  makeSprite(sprNext, L.nextW, L.nextH);
  makeSprite(sprCard, L.pW, L.pH);
  makeSprite(sprPill, L.pillW[1], L.pillH);  // widest pill
}

void initDisplay() {
  lcd.init();
  lcd.setRotation((PORTRAIT ? 0 : 1) + (FLIP_180 ? 2 : 0));
  setupLayout(PORTRAIT);
  createSprites();
  drawStaticScreen();
}

// call often from loop(): redraws only what changed + runs the animations
void updateDisplay(bool forceAll = false) {
  static int lastSec = -1, lastMin = -1, lastDay = -1;
  static int lastTopKey = -1, lastLed = -1, lastMist = -1;
  static uint32_t lastAnim = 0;
  uint32_t ms = millis();

  int bars = wifiRssi > -55 ? 4 : wifiRssi > -65 ? 3
                                : wifiRssi > -75 ? 2
                                                 : 1;
  int topKey = (wifiConnected ? 1 + bars * 2 : 0) + (wifiEverConnected ? 16 : 0) + timesSource * 32;
  if (forceAll || topKey != lastTopKey) {
    drawTopBar();
    lastTopKey = topKey;
  }

  int sec = timeValid ? nowTm.tm_sec : (int)(ms / 1000 % 60);
  if (forceAll || sec != lastSec) {
    drawClock();
    lastSec = sec;
  }

  int day = timeValid ? nowTm.tm_yday : -2;
  if (forceAll || day != lastDay || uiTimesChanged) {
    drawDates();
    lastDay = day;
  }

  int minute = timeValid ? nowMinuteOfDay() : -2;
  if (forceAll || minute != lastMin || uiTimesChanged) {
    drawAllPrayers();
    lastMin = minute;
  }
  uiTimesChanged = false;

  if (forceAll || uiWeatherChanged) {
    drawWeather();
    uiWeatherChanged = false;
  }

  if (forceAll || ms - lastAnim >= 60) {  // ~16 fps animations
    lastAnim = ms;
    drawNextBar();
    drawStars();
    int act = timeValid ? activePrayer() : -1;
    if (act >= 0) drawPrayer(act);  // pulsing prayer

    if (forceAll || (int)ledIsOn != lastLed) {
      drawLedPill();
      lastLed = ledIsOn;
    }
    if (forceAll || (int)mistIsOn != lastMist || mistIsOn) {
      drawMistPill();
      lastMist = mistIsOn;
    }
  }
}
