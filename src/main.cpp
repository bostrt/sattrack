#include <Wire.h>
#include <RTClib.h>
#include <SSD1306_minimal.h>

#include <predict/predict.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <avr/dtostrf.h>
#include <time.h>
#include <sys/time.h>

#include "main.h"
#include "storage.h"
#include "console.h"


static predict_orbital_elements_t *iss;
static predict_observer_t *obs;

#define I2C_ADDRESS 0x78

#ifdef DEBUG
#define MSG(...) Serial.println(__VA_ARGS__)
#else
#define MSG(...)
#endif

SSD1306_Mini oled;
RTC_PCF8523 rtc;
Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();
Storage storage = Storage(&fram);
QTH qth;
Console console = Console(&qth, &rtc);

void setup() {
  Serial.begin(9600);

#if DEBUG
  while (!Serial); // TODO: Only wait in DEBUG mode.
  MSG("setup() running...");
#endif

  oled.init(I2C_ADDRESS);
  oled.clear();
  bool success = rtc.begin();
  if (!success) {
    MSG("There was an issue configuring RTC. Exiting.");
    exit(-1);
  }

  success = rtc.initialized();
  if (!success) {
    MSG("RTC not initialized. Adjusting time to __DATE__, __TIME__.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  success = fram.begin();
  if (!success) {
    MSG("There was an issue configuring Storage. Continuing.");
  }

  bool valid = storage.verify();
  if (valid) {
    MSG("FRAM Check passed and storage valid. Reading QTH state.");
    storage.load(&qth);
    char buffer[50];
    sprintf(buffer, "Hello %s, [lat: %3.6f, long: %3.6f, elev: %3.6f]", qth.callsign, qth.latitude, qth.longitude, qth.elevation);
    MSG(buffer);
  } else {
    MSG("FRAM Check failed and storage invalid. Using default QTH.");
    storage.save(&qth);
  }

  const char *tle_line_1 = "1 27607U 02058C   17279.44029422  .00000122  00000-0  38164-4 0  9991";
	const char *tle_line_2 = "2 27607  64.5548 145.5835 0034805  42.2572 318.1202 14.75372406795523";

	// Create orbit object
  iss = predict_parse_tle(tle_line_1, tle_line_2);
  MSG("predict_parse_tle()...");

	// Create observer object
	obs = predict_create_observer(qth.callsign, TO_RADIANS(qth.latitude), TO_RADIANS(qth.longitude), qth.elevation);
  MSG("predict_create_observer()...");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  oled.printString("OKAY!");
  MSG("loop()...\n");

  predict_julian_date_t curr_time = predict_to_julian(rtc.now().unixtime());
  MSG("predict_to_julian()...");


	// Predict ISS
	struct predict_orbit iss_orbit;
	predict_orbit(iss, &iss_orbit, curr_time);
  MSG("predict_orbit()...");

  char lat[50];
  char lon[50];
  char alt[50];
  char outbuff[50];
  dtostrf(TO_DEGREES(iss_orbit.latitude), 3, 6, lat);
  dtostrf(TO_DEGREES(iss_orbit.longitude), 3, 6, lon);
  dtostrf(iss_orbit.altitude, 3, 6, alt);

	sprintf(outbuff, "ISS @ %d: lat=%s, lon=%s, alt=%s\n", rtc.now().unixtime(), lat, lon, alt);
  MSG(outbuff);

  struct predict_observation iss_obs;
  predict_observe_orbit(obs, &iss_orbit, &iss_obs);
  char azi[50];
  char ele[50];
  dtostrf(TO_DEGREES(iss_obs.azimuth), 3, 6, azi);
  dtostrf(TO_DEGREES(iss_obs.elevation), 3, 6, ele);
  sprintf(outbuff, "ISS: azi=%s, ele=%s\n", azi, ele);
  Serial.write(outbuff);

}

void serialEventRun() {
  if (Serial.available()) {
    String interrupt = Serial.readString();
    if (interrupt != NULL) {
      interrupt.trim();
    }
    MSG("Received: " + interrupt);

    if (interrupt.equalsIgnoreCase("break")) {
      bool result = console.enterCommandMode();
      if (result) {
        storage.save(&qth);
      }
    }
  }
}
