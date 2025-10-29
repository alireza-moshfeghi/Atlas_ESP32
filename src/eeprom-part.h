#include <EEPROM.h>
#define RESET 14
uint64_t resetTime = 0;
uint64_t timeStamp = 0;
struct Eeprom {
  bool active = true;
  String apName = "Atlas GH";
  String apPass = "";
  String stName = "";
  String stPass = "";
  String username = "admin";
  String password = "admin";
  String unit = "centigrade";
  String heaterMode = "manual";
  String coolerMode = "manual";
  String foggerMode = "manual";
  String exfanMode = "manual";
  String lightMode = "manual";
  String pumpMode = "manual";
  String co2genMode = "manual";
  float heaterThresh = 0;
  float coolerThresh = 0;
  float foggerThresh = 0;
  float exfanThresh = 0;
  int lightThresh = 0;
  int pumpThresh = 0;
  int co2genThresh = 0;
  int delayTime = 10;
  int errorTime = 3600;
  int ignoreTime = 3;
  byte jalali = false;
  bool buzzer = true;
  bool dhcpMode = true;
  bool forceLogin = true;
  float tempOffset = 0.0;
  float rhOffset = 0.0;
  IPAddress ip;
  IPAddress subnet;
  IPAddress gateway;
  IPAddress dns;
};
Eeprom eeprom;

void eepromSetup() {
  pinMode(RESET, INPUT_PULLDOWN);
  EEPROM.begin(4096);
  if (EEPROM.read(0) == 1 && EEPROM.read(1) == 3 && EEPROM.read(2) == 7 && EEPROM.read(3) == 9) {
    EEPROM.get(4, eeprom);
  }
  else {
    EEPROM.put(4, eeprom);
    EEPROM.write(0, 1);
    EEPROM.write(1, 3);
    EEPROM.write(2, 7);
    EEPROM.write(3, 9);
    EEPROM.commit();
  }
}

void eepromLoop() {
  if (digitalRead(RESET) == HIGH) {
    if (resetTime == 0) {
      resetTime = timeStamp + 5;
    }
    else if (resetTime < timeStamp) {
      EEPROM.write(0, 0);
      EEPROM.write(1, 0);
      EEPROM.write(2, 0);
      EEPROM.write(3, 0);
      EEPROM.commit();
      ESP.restart();
    }
  }
  else {
    resetTime = 0;
  }
}

void eepromSave() {
  EEPROM.put(4, eeprom);
  EEPROM.commit();
}