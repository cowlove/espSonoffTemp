#ifndef UBUNTU
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <NTPClient.h>
#include <Wire.h>
#include <SPI.h>
#include <Pinger.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "OneWireNg_CurrentPlatform.h"
#include "RTClib.h"

#else
#include "ESP32sim_ubuntu.h"
#endif

#include "jimlib.h"


RTC_DS3231 rtc;
OneWireNg_CurrentPlatform ow(14, true);
WiFiClient espClient;
PubSubClient client(espClient);

const int ledPin = 13;
const int relayPin = 12;
const int buttonPin = 0;

DigitalButton button(buttonPin);
LongShortFilter butFilt(1000,500);

WiFiUDP udp;
//WiFiManager wman;

JStuff j;

CLI_VARIABLE_FLOAT(setTemp, 8); 
CLI_VARIABLE_FLOAT(setHist, 0.2);

TempSensor tempSense(&ow);

void setup() {
  j.begin();
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, 0);

	pinMode(relayPin, OUTPUT);
	pinMode(buttonPin, INPUT);  
}

EggTimer sec(1000 * 60);

int power = 0, minutesOff = 0;
void loop() {
  j.run();

  if (sec.tick())  {
    float temp = tempSense.readTemp(10);

    if (temp > (setTemp + setHist)) { 
      power = 1;
    }
    if (temp > -100 && temp < setTemp) { 
      power = 0;
    }
    //if (power == 0 && ++minutesOff > 20) { 
    //  power = 1;
    //}
    if (power == 1) {
      minutesOff = 0;
    }

    OUT("setTemp %.2f temp %.2f power %d minutesOff %d", (float)setTemp, temp, power, minutesOff);
    digitalWrite(relayPin, power);
    digitalWrite(ledPin, !power);
	}


	ArduinoOTA.handle();
	delay(10);
}



#ifdef UBUNTU
class ESP32sim_setup : public ESP32sim_Module {
  void setup() override {
      bm.addPress(buttonPin, 10, 1, true);
      Serial.scheduleInput(2000, "set setTemp=5.0\n"); 
  };
} csim; 
#endif
