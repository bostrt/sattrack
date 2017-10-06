#include "config.h"

void writeVerify(Adafruit_FRAM_I2C *fram) {
  fram->write8(VERIFY_LOC, VERIFY);
}

void reset(Adafruit_FRAM_I2C *fram) {
  fram->write8(VERIFY_LOC, 0x00);
}

boolean verify(Adafruit_FRAM_I2C *fram) {
  uint8_t result = fram->read8(VERIFY_LOC);
  if (result == VERIFY) {
    return true;
  }
  return false;
}

void save(QTH *qth, Adafruit_FRAM_I2C *fram) {
  writeString(qth->callsign, CALLSIGN_LOC, fram);
  writeDouble(qth->latitude, LATITUDE_LOC, fram);
  writeDouble(qth->longitude, LONGITUDE_LOC, fram);
  writeDouble(qth->elevation, ELEVATION_LOC, fram);
}
void load(QTH *qth, Adafruit_FRAM_I2C *fram) {
  readString(qth->callsign, CALLSIGN_LOC, fram);
  qth->latitude = readDouble(LATITUDE_LOC, fram);
  qth->longitude = readDouble(LONGITUDE_LOC, fram);
  qth->elevation = readDouble(ELEVATION_LOC, fram);
}

void writeString(char *value, uint16_t addr, Adafruit_FRAM_I2C *fram) {
  if (strlen(value) > CALLSIGN_LIMIT) {
    // unspported callsign length
  } else {
    for (int i = 0; i < CALLSIGN_LIMIT; i++) {
      fram->write8(addr+i, value[i]);
    }
  }
}

void readString(char *value, uint16_t addr, Adafruit_FRAM_I2C *fram) {
    value[0] = (char) fram->read8(addr);
//  for (int i = 0; i < CALLSIGN_LIMIT; i++) {
//  }
}

void writeDouble(double value, uint16_t addr, Adafruit_FRAM_I2C *fram) {
  char buffer[DOUBLE_LEN]; // [-]nnn.nnnnnn0x00
  sprintf(buffer, DOUBLE_FMT, value);
  for (int i = 0; i < DOUBLE_LEN; i++) {
    fram->write8(addr+i, buffer[i]);
  }

  writeVerify(fram);
}

double readDouble(uint16_t addr, Adafruit_FRAM_I2C *fram) {
  char buffer[DOUBLE_LEN]; // [-]nnn.nnnnnn0x00
  for (int i = 0; i < DOUBLE_LEN; i++) {
    buffer[i] = (char) fram->read8(addr+i);
  }

  return atof(buffer);
}
