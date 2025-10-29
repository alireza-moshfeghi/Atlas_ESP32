#include <WiFi.h>
#define LED 2
bool stConnect = false;
bool restart = false;

void wifiSetup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  WiFi.setHostname("Atlas GH");
  WiFi.softAP(eeprom.apName, eeprom.apPass);
  if (!eeprom.dhcpMode) {
    WiFi.config(eeprom.ip, eeprom.gateway, eeprom.subnet, eeprom.dns);
  }
  WiFi.setAutoReconnect(true);
  WiFi.begin(eeprom.stName, eeprom.stPass);
}

void apUpdate() {
  WiFi.softAP(eeprom.apName, eeprom.apPass);
}

void stUpdate() {
  WiFi.begin(eeprom.stName, eeprom.stPass);
}

void wifiLoop() {
  if (WiFi.status() == WL_CONNECTED) {
    stConnect = true;
    digitalWrite(LED, HIGH);
    delay(500);
  }
  else {
    stConnect = false;
    digitalWrite(LED, HIGH);
    delay(250);
    digitalWrite(LED, LOW);
    delay(250);
  }
  if (restart) {
    delay(1000);
    ESP.restart();
  }
}