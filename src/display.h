#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <SSD1306_minimal.h>
#include <Button.h>
#include <predict/predict.h>
#include "qth.h"
#include "main.h"

class Display
{
public:
  Display(SSD1306_Mini *oled);
  void setQTH(QTH *qth);
  void setTime(unsigned int time);
  void setOrbit(orbit *orbit);
  void setObserervation(predict_observation *observation);
private:
  SSD1306_Mini *oled;
};


#endif
