#ifndef _CONSOLE_H
#define _CONSOLE_H

#include "main.h"
#include "storage.h"
#include "qth.h"
#include <Adafruit_FRAM_I2C.h>

int command_mode(Adafruit_FRAM_I2C *fram, QTH *qth);

#endif
