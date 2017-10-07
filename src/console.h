#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <Adafruit_FRAM_I2C.h>
#include <RTClib.h>
#include "main.h"
#include "storage.h"
#include "qth.h"

int command_mode(Adafruit_FRAM_I2C *fram, QTH *qth);

class Console
{
public:
  // TODO: Need to work on removing dependency on RTC and maybe QTH.
  Console(QTH *qth, RTC_PCF8523 *rtc);
  bool enterCommandMode();
private:
  QTH *qth;
  RTC_PCF8523 *rtc;
  void swallowIncoming();
  int getYear();
  int getUTCOffset();  
  int getDateTimePart(char *prompt);
  bool configureDateTime();
  bool configureCallsign();
};

#endif
