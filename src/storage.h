#ifndef _CONFIG_H
#define _CONFIG_H

#include <Adafruit_FRAM_I2C.h>
#include "qth.h"

#define VERIFY_LOC 0
#define VERIFY 0xAB
#define STORAGE_START 0x9
#define CALLSIGN_LIMIT 6
#define DOUBLE_FMT "%3.6f"
#define DOUBLE_LEN 12  // [-]nnn.nnnnnn0x00
#define LATITUDE_LOC STORAGE_START
#define LONGITUDE_LOC LATITUDE_LOC+DOUBLE_LEN
#define ELEVATION_LOC LONGITUDE_LOC+DOUBLE_LEN
#define CALLSIGN_LOC ELEVATION_LOC+DOUBLE_LEN

class Storage {
public:
  Storage(Adafruit_FRAM_I2C *fram);
  /**
   * Verify that storage has been initialized before using simple aglorithm.
   */
  boolean verify();
  /**
   * Persist state of QTH.
   */
  void save(QTH *qth);
  /**
   * Read state of QTH from persistent storage.
   */
  void load(QTH *qth);
  void writeString(char *value, uint16_t addr);
  void readString(char *value, uint16_t addr);
  void writeDouble(double value, uint16_t addr);
  double readDouble(uint16_t addr);
  void reset();
private:
  Adafruit_FRAM_I2C *fram;
  void writeVerify();
};

#endif
