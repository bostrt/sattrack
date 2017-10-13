#include <predict/predict.h>
#include "display.h"
#include "main.h"
#include <avr/dtostrf.h>

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

Display::Display(SSD1306_Mini *oledref)
{
  oled = oledref;
}

void Display::setQTH(QTH *qth)
{
  oled->cursorTo(QTH_ROW, QTH_COL);
  char buff[8];
  sprintf(buff, "\xb0%s\xb0", qth);
  oled->printString(buff);
}

// TODO: Fix TZ of hour.
// TODO: Cache hour and minute.
void Display::setTime(unsigned int hour, unsigned int minute, unsigned int seconds)
{
  char buff[10];
  sprintf(buff, "%02d:%02d:%02d", hour, minute, seconds);
  oled->cursorTo(TIME_ROW,  TIME_COL);
  oled->printString(buff);
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
  oled->cursorTo(LAT_ROW, LAT_COL);
  oled->printString(buff);

  sprintf(buff, "LON: %s", lon);
  oled->cursorTo(LON_ROW, LON_COL);
  oled->printString(buff);
}

void Display::setObserervation(predict_observation *obs)
{
  char azi[10];
  char ele[10];
  char buff[17];
  dtostrf(TO_DEGREES(obs->azimuth), 3, 6, azi);
  dtostrf(TO_DEGREES(obs->elevation), 3, 6, ele);

  sprintf(buff, "AZI: %s", azi);
  oled->cursorTo(AZI_ROW, AZI_COL);
  oled->printString(buff);

  sprintf(buff, "ELE: %s", ele);
  oled->cursorTo(ELE_ROW, ELE_COL);
  oled->printString(buff);
}
