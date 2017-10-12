#include <predict/predict.h>
#include "display.h"
#include "main.h"
#include <avr/dtostrf.h>

Display::Display(SSD1306_Mini *oledref)
{
  oled = oledref;
}

void Display::setQTH(QTH *qth)
{

}

void Display::setTime(unsigned int time)
{
  Serial.print("Now is ");
  Serial.println(time);
}

void Display::setOrbit(orbit *o)
{
  char lat[50];
  char lon[50];
  char alt[50];
  char outbuff[50];
  dtostrf(TO_DEGREES(o->latitude), 3, 6, lat);
  dtostrf(TO_DEGREES(o->longitude), 3, 6, lon);
  dtostrf(o->altitude, 3, 6, alt);

	sprintf(outbuff, "ISS @ lat=%s, lon=%s, alt=%s", lat, lon, alt);
  Serial.println(outbuff);
}

void Display::setObserervation(predict_observation *obs)
{
  char azi[50];
  char ele[50];
  char outbuff[50];
  dtostrf(TO_DEGREES(obs->azimuth), 3, 6, azi);
  dtostrf(TO_DEGREES(obs->elevation), 3, 6, ele);
  sprintf(outbuff, "ISS: azi=%s, ele=%s", azi, ele);
  Serial.println(outbuff);
}
