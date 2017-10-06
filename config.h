#ifndef _CONFIG_H
#define _CONFIG_H

#include <Adafruit_FRAM_I2C.h>

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

typedef struct QTH {
  char callsign[7] = "KN4CUV";
  double latitude = 35.751489; // In degrees
  double longitude = -78.775148; // In degrees
  double elevation = 152; // In meters
  // TODO: Store TLE
};

/**
 * Verify that storage has been initialized before using simple aglorithm.
 */
boolean verify(Adafruit_FRAM_I2C *fram);

/**
 * Persist state of QTH.
 */
void save(QTH *qth, Adafruit_FRAM_I2C *fram);

/**
 * Read state of QTH from persistent storage.
 */
void load(QTH *qth, Adafruit_FRAM_I2C *fram);

/**
 * USE WITH CAUTION.
 */
void writeString(char *value, uint16_t addr, Adafruit_FRAM_I2C *fram);

/**
 * USE WITH CAUTION.
 */
void readString(char *value, uint16_t addr, Adafruit_FRAM_I2C *fram);

/**
 * USE WITH CAUTION.
 */
void writeDouble(double value, uint16_t addr, Adafruit_FRAM_I2C *fram);

/**
 * USE WITH CAUTION.
 */
double readDouble(uint16_t addr, Adafruit_FRAM_I2C *fram);

/**
 * USE WITH CAUTION.
 */
void reset(Adafruit_FRAM_I2C *fram);

#endif
