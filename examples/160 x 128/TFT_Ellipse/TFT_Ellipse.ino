/*
  Ellipse drawing example
  
  This sketch does not use any fonts.
*/

#include <SPI.h>
#include "../../../ESP32_SPIDisplay.h" // Hardware-specific library

Screen tft = Screen();       // Invoke custom library

void setup(void) {
  tft.init();

  tft.setRotation(1);

}

void loop() {

  tft.fillScreen(TFT_BLACK);

  // Draw some random filled elipses
  for (int i = 0; i < 20; i++)
  {
    int rx = random(40);
    int ry = random(40);
    int x = rx + random(160 - rx - rx);
    int y = ry + random(128 - ry - ry);
    tft.fillEllipse(x, y, rx, ry, random(0xFFFF));
  }

  delay(2000);
  tft.fillScreen(TFT_BLACK);

  // Draw some random outline elipses
  for (int i = 0; i < 20; i++)
  {
    int rx = random(40);
    int ry = random(40);
    int x = rx + random(160 - rx - rx);
    int y = ry + random(128 - ry - ry);
    tft.drawEllipse(x, y, rx, ry, random(0xFFFF));
  }

  delay(2000);
}



