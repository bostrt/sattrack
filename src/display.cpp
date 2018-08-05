#include <predict/predict.h>
#include <avr/dtostrf.h>
#include "display.h"
#include "main.h"

// Data coordinates
#define TIME_COL 0
#define TIME_ROW 0
#define QTH_COL 0
#define QTH_ROW 64
#define LAT_COL 1
#define LAT_ROW 0
#define LON_COL 2
#define LON_ROW 0
#define AZI_COL 3
#define AZI_ROW 0
#define ELE_COL 4
#define ELE_ROW 0

Display::Display(Adafruit_SSD1306 *oledref)
{
  oled = oledref;
}

void Display::setQTH(QTH *qth)
{
  oled->setCursor(0, 0);
  char buff[8];
  sprintf(buff, "\xb0%s\xb0", qth);
  oled->println(buff);
}

// TODO: Fix TZ of hour.
// TODO: Cache hour and minute.
void Display::setTime(unsigned int hour, unsigned int minute, unsigned int seconds)
{
  char buff[10];
  sprintf(buff, "%02d:%02d:%02d", hour, minute, seconds);
  oled->setCursor(64, 0);
  oled->print(buff);
}

void Display::setOrbit(orbit *o)
{
  char lat[10];
  char lon[10];
  char alt[6];
  char buff[17];
  dtostrf(TO_DEGREES(o->latitude), 3, 6, lat);
  dtostrf(TO_DEGREES(o->longitude), 3, 6, lon);
  dtostrf(o->altitude, 3, 2, alt);

	sprintf(buff, "LAT: %s", lat);
//  oled->cursorTo(LAT_ROW, LAT_COL);
//  oled->printString(buff);
  Serial.println(buff);
  sprintf(buff, "LON: %s", lon);
  Serial.println(buff);  
//  oled->cursorTo(LON_ROW, LON_COL);
//  oled->printString(buff);
}

void Display::setObserervation(predict_observation *obs)
{
  char azi[10];
  char polarx[10];
  char polary[10];
  char ele[10];
  char buff[17];
  dtostrf(TO_DEGREES(obs->azimuth), 3, 6, azi);
  dtostrf(TO_DEGREES(obs->elevation), 3, 6, ele);

  sprintf(buff, "AZI: %s", azi);
//  oled->cursorTo(AZI_ROW, AZI_COL);
//  oled->printString(buff);
  Serial.println(buff);
  
  double degrees = (-(int)TO_DEGREES(obs->azimuth) + 90) % 360;
  double x = cos(TO_RADIANS(degrees)) * 30 + oled->width()/2 + 10;
  double y = abs(sin(TO_RADIANS(degrees)) * 30) + oled->height()/2 + 10;
  dtostrf(x, 3, 6, polarx);
  dtostrf(y, 3, 6, polary);  
  sprintf(buff, "X: %s, Y: %s", polarx, polary);
  Serial.println(buff);

  int elevationPixels = TO_DEGREES(obs->elevation) / (90 / 30); // radius = 30
  sprintf(buff, "ELE: %d", elevationPixels);
//  oled->cursorTo(ELE_ROW, ELE_COL);
//  oled->printString(buff);
  Serial.println(buff);
  
  oled->drawPixel(x, y, WHITE);  
}
