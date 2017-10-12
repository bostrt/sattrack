#ifndef _MAIN_H
#define _MAIN_H

#include <Arduino.h>

/*#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  // Required for Serial on Zero based boards
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif*/

#define TO_RADIANS(degrees) (degrees * (M_PI / 180.0))
#define TO_DEGREES(radians) (radians * (180.0 / M_PI))

typedef struct predict_orbit orbit;
typedef struct predict_observation observation;

#define DEBUG 1

#endif
