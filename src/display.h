#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <Adafruit_SSD1306.h>
#include <Button.h>
#include <predict/predict.h>
#include "qth.h"
#include "main.h"

class Display
{
public:
  Display(Adafruit_SSD1306 *oled);
  void setQTH(QTH *qth);
  void setTime(unsigned int hour, unsigned int minute, unsigned int seconds);
  void setOrbit(orbit *orbit);
  void setObserervation(predict_observation *observation);
private:
  Adafruit_SSD1306 *oled;
};

#endif
