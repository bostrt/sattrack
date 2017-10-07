#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_FRAM_I2C.h>

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

RTC_PCF8523 rtc;
Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();
Storage storage = Storage(&fram);
Adafruit_SSD1306 display(-1);
QTH qth;
Console console = Console(&qth, &rtc);

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
  if (fram.begin()) {
    Serial.println("FRAM I2C Found!");
  }

  bool valid = storage.verify();
  if (valid) {
    Serial.println("FRAM Check passed and storage valid. Reading QTH state...");
    storage.load(&qth);
    char buffer[50];
    sprintf(buffer, "Hello %s, [lat: %3.6f, long: %3.6f, elev: %3.6f]", qth.callsign, qth.latitude, qth.longitude, qth.elevation);
    Serial.println(buffer);
  } else {
    Serial.println("FRAM Check passed and storage invalid. Using default QTH.");
    storage.save(&qth);
  }


  display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS, false);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.drawPixel(10, 10, WHITE);

  const char *tle_line_1 = "1 27607U 02058C   17277.94906682 +.00000114 +00000-0 +37005-4 0  9990";
	const char *tle_line_2 = "2 27607 064.5549 150.1407 0034654 042.8809 317.4986 14.75371499795175";

	// Create orbit object
  iss = predict_parse_tle(tle_line_1, tle_line_2);
  if (DEBUG) {
    Serial.println("predict_parse_tle()...");
  }

	// Create observer object
	obs = predict_create_observer(qth.callsign, TO_RADIANS(qth.latitude), TO_RADIANS(qth.longitude), qth.elevation);
  if (DEBUG) {
    Serial.println("predict_create_observer()...");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  if (DEBUG) {
    Serial.print("loop()...\n");
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
      bool result = console.enterCommandMode();
      if (result) {
        storage.save(&qth);
      }
    }
  }
}
