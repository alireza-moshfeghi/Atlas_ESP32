#include <Arduino.h>
#include "eeprom-part.h"
#include "time-part.h"
#include "sensor-part.h"
#include "relay-part.h"
#include "alarm-part.h"
#include "record-part.h"
#include "schedule-part.h"
#include "wifi-part.h"
#include "web-part.h"
#include "dns-part.h"

void setup() {
    eepromSetup(); 
    wifiSetup(); 
    sensorSetup(); 
    recordSetup();
    webSetup();
    dnsSetup();
    timeSetup();
    alarmSetup();
    relaySetup();
}

void loop() {
    eepromLoop();
    timeLoop();
    sensorLoop();
    recordLoop();
    alarmLoop();
    dnsLoop();
    relayLoop();
    scheduleLoop();
    wifiLoop();
}