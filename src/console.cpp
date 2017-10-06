#include <Adafruit_FRAM_I2C.h>
#include <RTClib.h>
#include "console.h"
#include "main.h"

Console::Console(QTH *qthref, RTC_PCF8523 *rtcref)
{
  qth = qthref;
  rtc = rtcref;
}

void Console::swallowIncoming() {
  while (Serial.available()) {
    Serial.read();
  }
}

// Get 4 digit year.
// -1 means failure
int Console::getYear() {
  Serial.println("Enter Year (YYYY)...");
  Serial.print(">>> ");
  char input[4];
  int success = Serial.readBytes(input, 4);
  if (success == 0) {
    return -1;
  }
  swallowIncoming();
  return atoi(input);
}

// Get 2 digit part of date.
// -1 means failure
int Console::getDateTimePart(char *prompt) {
  Serial.println(prompt);
  Serial.print(">>> ");
  char input[2];
  int success = Serial.readBytes(input, 2);
  if (success == 0) {
    return -1;
  }
  swallowIncoming();
  return atoi(input);
}

bool Console::configureDateTime() {
  Serial.println("Follow the prompts to enter date and time.");
  int year = getYear();
  Serial.println(year);
  if (year < 0) {
    Serial.println("Timed out during time/date setup. Resetting tracking mode...");
    return false;
  }

  int month = getDateTimePart("Enter month (MM)...");
  Serial.println(month);
  if (month < 0) {
    Serial.println("Timed out during time/date setup. Resetting tracking mode...");
    return false;
  }

  int day = getDateTimePart("Enter day (DD)...");
  Serial.println(day);
  if (day < 0) {
    Serial.println("Timed out during time/date setup. Resetting tracking mode...");
    return false;
  }

  int hour = getDateTimePart("Enter hours (HH / )...");
  Serial.println(hour);
  if (hour < 0) {
    Serial.println("Timed out during time/date setup. Resetting tracking mode...");
    return false;
  }

  int min = getDateTimePart("Enter minutes (MM)...");
  Serial.println(min);
  if (min < 0) {
    Serial.println("Timed out during time/date setup. Resetting tracking mode...");
    return false;
  }

  int sec = getDateTimePart("Enter seconds (SS)...");
  if (sec < 0) {
    Serial.println("Timed out during time/date setup. Resetting tracking mode...");
    return false;
  }
  Serial.println(sec);
  DateTime dt = DateTime(year, month, day, hour, min, sec);
  rtc->adjust(dt);
  Serial.println("RTC time updated. " + String(dt.unixtime()));
  return true;
}


bool Console::configureCallsign() {
  Serial.println("Enter your callsign...");
  Serial.print(">>> ");
  char input[CALLSIGN_LIMIT];
  int success = Serial.readBytesUntil('\n', input, CALLSIGN_LIMIT);
  if (success == 0) {
    Serial.println("Timed out during time/date setup. Resetting tracking mode...");
    return false;
  }
  Serial.println(input);
  for (int i = 0; i < CALLSIGN_LIMIT; i++) {
    qth->callsign[i] = input[i];
    Serial.print(qth->callsign[i]);
  }

  return true;
}

bool Console::enterCommandMode() {
  Serial.setTimeout(10000);
  Serial.println("Enter number...");
  Serial.println("0) Set date/time");
  Serial.println("1) Set callsign    (Coming soon!)");
  Serial.println("2) Add satellite   (Coming soon!)");
  Serial.println("3) Set QTH locator (Coming soon!)");
  Serial.print(">>> ");
  char input;
  Serial.readBytes(&input, 1);
  if (input == '0') {
    return configureDateTime();
  }  else if(input == '1') {
    return configureCallsign();
  } else {
    Serial.println("Unsupported option. Resetting tracking mode...");
  }
  Serial.setTimeout(1000);
}
