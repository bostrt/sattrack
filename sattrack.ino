#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <sgp4.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <avr/dtostrf.h>
#include <time.h>
#include <sys/time.h>

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  // Required for Serial on Zero based boards
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif

#define TO_RADIANS(degrees) (degrees * (M_PI / 180.0))
#define TO_DEGREES(radians) (radians * (180.0 / M_PI))

/* Application Configurations */
#define DEBUG 1
static char *QTH_CALLSIGN = "KN4CUV"; // Optional
static double QTH_LATITUDE = 35.751489; // In degrees
static double QTH_LONGITUDE = -78.775148; // In degrees
static double QTH_ELEVATION = 152; // In meters

#define I2C_ADDRESS 0x78

RTC_PCF8523 rtc;
Adafruit_SSD1306 display(-1);

void setup() {
  if (DEBUG) {
    Serial.begin(9600);
    while (!Serial); // TODO: Only wait in DEBUG mode.
    Serial.print("setup() running...");
    Serial.setTimeout(2000);
  }
  if(!rtc.begin() && DEBUG) {
    Serial.println("Trouble starting up RTC");
  }
  if (!rtc.initialized() && DEBUG) {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS, false);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.drawPixel(10, 10, WHITE);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  if (DEBUG) {
    Serial.print("loop()...\n");
  }

  const char *tle_line_1 = "1 25544U 98067A   17273.88039200  .00006192  00000-0  10105-3 0  9998";
	const char *tle_line_2 = "2 25544  51.6421 231.4274 0004085 329.1875  58.5254 15.54032580 78193";

	// Create orbit object
	predict_orbital_elements_t *iss = predict_parse_tle(tle_line_1, tle_line_2);
  if (DEBUG) {
    Serial.println("predict_parse_tle()...");
  }

	// Create observer object
	predict_observer_t *obs = predict_create_observer(QTH_CALLSIGN, TO_RADIANS(QTH_LATITUDE), TO_RADIANS(QTH_LONGITUDE), QTH_ELEVATION);
  if (DEBUG) {
    Serial.println("predict_create_observer()...");
  }

  predict_julian_date_t curr_time = predict_to_julian(rtc.now().unixtime());
  if (DEBUG) {
    Serial.println("predict_to_julian()...");
  }


	// Predict ISS
	struct predict_orbit iss_orbit;
	predict_orbit(iss, &iss_orbit, curr_time);
  if (DEBUG) {
    Serial.println("predict_orbit()...");
  }

  char lat[50];
  char lon[50];
  char alt[50];
  char outbuff[50];
  dtostrf(TO_DEGREES(iss_orbit.latitude), 3, 6, lat);
  dtostrf(TO_DEGREES(iss_orbit.longitude), 3, 6, lon);
  dtostrf(iss_orbit.altitude, 3, 6, alt);

	sprintf(outbuff, "ISS @ %d: lat=%s, lon=%s, alt=%s\n", rtc.now().unixtime(), lat, lon, alt);
  Serial.write(outbuff);

  struct predict_observation iss_obs;
  predict_observe_orbit(obs, &iss_orbit, &iss_obs);
  char azi[50];
  char ele[50];
  dtostrf(TO_DEGREES(iss_obs.azimuth), 3, 6, azi);
  dtostrf(TO_DEGREES(iss_obs.elevation), 3, 6, ele);
  sprintf(outbuff, "ISS: azi=%s, ele=%s\n", azi, ele);
  Serial.write(outbuff);

  predict_destroy_orbital_elements(iss);
  predict_destroy_observer(obs);
}

void serialEventRun() {
  if (Serial.available()) {
    String interrupt = Serial.readString();
    if (interrupt != NULL) {
      interrupt.trim();
    }
    if (DEBUG) {
      Serial.println("Received: " + interrupt);
    }
    if (interrupt.equalsIgnoreCase("break")) {
      command_mode();
    }
  }
}

void command_mode() {
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
    configureDateTime();
  }  else if(input == '1') {
    configureCallsign();
  } else {
    Serial.println("Unsupported option. Resetting tracking mode...");
  }
  Serial.setTimeout(1000);
}

bool configureDateTime() {
  Serial.println("Follow the prompts to enter date and time.");
  int year = getYear("Enter Year (YYYY)...");
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
  rtc.adjust(dt);
  Serial.println("RTC time updated. " + String(dt.unixtime()));
  return true;
}

// Get 4 digit year.
// -1 means failure
int getYear(char *prompt) {
  Serial.println(prompt);
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
int getDateTimePart(char *prompt) {
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

void swallowIncoming() {
  while (Serial.available()) {
    Serial.read();
  }
}

bool configureCallsign() {
  Serial.println("Enter your callsign...");
  Serial.print(">>> ");
  char *input;
  int success = Serial.readBytesUntil('\n', input, 6);
  if (success == 0) {
    Serial.println("Timed out during time/date setup. Resetting tracking mode...");
    return false;
  }
  Serial.println(input);
}
