#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_SSD1306.h>

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
#include "display.h"

static predict_orbital_elements_t *iss;
static predict_observer_t *observer;

#define I2C_ADDRESS 0x3C

#ifdef DEBUG
#define MSG(...) Serial.println(__VA_ARGS__)
#else
#define MSG(...)
#endif

Adafruit_SSD1306 oled = Adafruit_SSD1306();
RTC_PCF8523 rtc;
Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();
Storage storage = Storage(&fram);
QTH qth;
Console console = Console(&qth, &rtc);
Display display = Display(&oled);

void setup() {
  Serial.begin(9600);

#if DEBUG
  while (!Serial); // TODO: Only wait in DEBUG mode.
  MSG("setup() running...");
#endif

  oled.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS);
  oled.display();
  delay(1000);
  oled.clearDisplay();
  
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

  display.setQTH(&qth);
    oled.drawCircle(oled.width()/2, oled.height()/2, 30, WHITE);
    oled.drawCircle(oled.width()/2, oled.height()/2, 20, WHITE);
    oled.drawCircle(oled.width()/2, oled.height()/2, 9, WHITE);
    oled.drawFastHLine(oled.width()/2, oled.height()/2, 30, WHITE);
    oled.drawFastHLine(oled.width()/2-30, oled.height()/2, 30, WHITE);
    oled.drawFastVLine(oled.width()/2, oled.height()/2, 30, WHITE);
    oled.drawFastVLine(oled.width()/2, oled.height()/2-30, 30, WHITE);
    oled.setTextSize(1);
    oled.setTextColor(WHITE);    
    oled.setCursor(0, 0);
    oled.println("Test test");
    oled.display();
    delay(10);
//    oled.clearDisplay();

  const char *tle_line_1 = "1 22825U 93061C   17306.87224525 +.00000010 +00000-0 +21491-4 0  9992";
	const char *tle_line_2 = "2 22825 098.8314 274.2839 0007425 289.9051 070.1330 14.30011892256951";

	// Create orbit object
  iss = predict_parse_tle(tle_line_1, tle_line_2);
  MSG("predict_parse_tle()...");

	// Create observer object
	observer = predict_create_observer(qth.callsign, TO_RADIANS(qth.latitude), TO_RADIANS(qth.longitude), qth.elevation);
  MSG("predict_create_observer()...");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  MSG("loop()...\n");
  DateTime now = rtc.now();
  predict_julian_date_t curr_time = predict_to_julian(now.unixtime());

	// Predict
	orbit o;
  observation obs;
	predict_orbit(iss, &o, curr_time);
  MSG("predict_orbit()...");
  predict_observe_orbit(observer, &o, &obs);

  Serial.println(now.hour());
  Serial.println(now.minute());
  Serial.println(now.second());
  Serial.println(now.unixtime());
  
  display.setTime(now.hour(), now.minute(), now.second());
  display.setOrbit(&o);
  display.setObserervation(&obs);
  oled.display();
}

void serialEventRun() {
  if (Serial.available()) {
    String interrupt = Serial.readString();
    if (interrupt != NULL) {
      interrupt.trim();
    }
    MSG("Received: " + interrupt);

    if (interrupt.equalsIgnoreCase("break")) {
      MSG("Entering command mode.");
      bool result = console.enterCommandMode();
      if (result) {
        storage.save(&qth);
        display.setQTH(&qth);
      }
    }
  }
}
