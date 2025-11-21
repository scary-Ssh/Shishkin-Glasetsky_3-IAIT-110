#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 8

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
XPT2046_Touchscreen ts(TFT_CS);

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  ts.begin();
  
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("ILI9341 2.8 TFT");
}

void loop() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    int pixelX = map(p.x, 200, 3700, 0, tft.width());
    int pixelY = map(p.y, 250, 3800, 0, tft.height());
    tft.fillCircle(pixelX, pixelY, 5, ILI9341_RED);
  }
  delay(10);
}
