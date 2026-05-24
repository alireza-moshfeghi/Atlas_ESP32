#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <RTClib.h>
#include <DHT.h>
#include <Adafruit_SGP30.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <vector>

#define HEATER 5
#define COOLER 18
#define FOGGER 19
#define EXFAN 21
#define LIGHT 27
#define PUMP 26
#define CO2GEN 25
#define DHTPIN 4
#define SDAPIN 16
#define SCLPIN 17
#define PHOTOCELL1 34
#define PHOTOCELL2 35
#define SOILSENS 32
#define BUZZER 15
#define RESET 14
#define LED 2
#define DNSPORT 53
#define WEBPORT 80

RTC_DS3231 rtc;
DHT dht(DHTPIN, DHT22);
Adafruit_SGP30 sgp;
DNSServer dnsServer;
AsyncWebServer server(WEBPORT);
std::vector<String> sessions;

bool heater = false;
bool cooler = false;
bool fogger = false;
bool exfan = false;
bool light = false;
bool pump = false;
bool co2gen = false;
bool DHT_22 = false;
bool SGP30 = false;
bool SOIL = false;
bool DS3231 = false;
bool stConnect = false;
bool restart = false;
bool isInit = false;

uint64_t heaterTime = 0;
uint64_t coolerTime = 0;
uint64_t foggerTime = 0;
uint64_t exfanTime = 0;
uint64_t lightTime = 0;
uint64_t pumpTime = 0;
uint64_t co2genTime = 0;
uint64_t heaterStart = 0;
uint64_t coolerStart = 0;
uint64_t foggerStart = 0;
uint64_t exfanStart = 0;
uint64_t lightStart = 0;
uint64_t pumpStart = 0;
uint64_t co2genStart = 0;
uint64_t timeStamp = 0;
uint64_t resetTime = 0;
uint64_t DHT22Ignore = 0;
uint64_t SGP30Ignore = 0;
uint64_t SOILIgnore = 0;
uint64_t DS3231Ignore = 0;
uint64_t timeStampOld = 0;

unsigned long lastMillis = 0;
unsigned int secondsSinceMidnight = 0;
int lastDay = -1;
int inBright = 0;
int outBright = 0;
int moist = 0;
int co2 = 0;
int tvoc = 0;
float temperature = 0;
float humidity = 0;

char cuHour[3] = "";
char cuMinute[3] = "";
char cuSecond[3] = "";
char cuYear[5] = "";
char cuMonth[3] = "";
char cuDay[3] = "";
char cuDayOfWeek[10] = "";
char upDay[3] = "";
char upHour[3] = "";
char upMinute[3] = "";
char upSecond[3] = "";

float minTemp = 0;
float maxTemp = 0;
float minRH = 0;
float maxRH = 0;
int minBright = 0;
int maxBright = 0;
int minCo2 = 0;
int maxCo2 = 0;
int minMoist = 0;
int maxMoist = 0;
int minTvoc = 0;
int maxTvoc = 0;
String minTempTime = "";
String maxTempTime = "";
String minRHTime = "";
String maxRHTime = "";
String minBrightTime = "";
String maxBrightTime = "";
String minCo2Time = "";
String maxCo2Time = "";
String minMoistTime = "";
String maxMoistTime = "";
String minTvocTime = "";
String maxTvocTime = "";
int heaterToday = 0;
int coolerToday = 0;
int foggerToday = 0;
int exfanToday = 0;
int lightToday = 0;
int pumpToday = 0;
int co2genToday = 0;
String errorTitle = "none";
String errorText = "none";
String yearsList = "";
String labelsJson = "[]";
String minTempJson = "[]";
String maxTempJson = "[]";
String minRHJson = "[]";
String maxRHJson = "[]";
String minBrightJson = "[]";
String maxBrightJson = "[]";
String minCo2Json = "[]";
String maxCo2Json = "[]";
String minMoistJson = "[]";
String maxMoistJson = "[]";
String minTvocJson = "[]";
String maxTvocJson = "[]";

String readFile(String path) {
  File file = LittleFS.open(path, "r");
  if (!file) return "";
  String content = file.readString();
  content.trim();
  file.close();
  return content;
}

void writeFile(String path, String value) {
  File file = LittleFS.open(path, "w");
  if (file) {
    file.println(value);
    file.close();
  }
}

void configSetup() {
  LittleFS.begin();
  if (!LittleFS.exists("/configs")) {
    LittleFS.mkdir("/configs");
  }
  
  writeFile("/configs/apName.txt", "Atlas GH");
  writeFile("/configs/apPass.txt", "");
  writeFile("/configs/stName.txt", "");
  writeFile("/configs/stPass.txt", "");
  writeFile("/configs/username.txt", "admin");
  writeFile("/configs/password.txt", "admin");
  writeFile("/configs/unit.txt", "centigrade");
  writeFile("/configs/heaterMode.txt", "manual");
  writeFile("/configs/coolerMode.txt", "manual");
  writeFile("/configs/foggerMode.txt", "manual");
  writeFile("/configs/exfanMode.txt", "manual");
  writeFile("/configs/lightMode.txt", "manual");
  writeFile("/configs/pumpMode.txt", "manual");
  writeFile("/configs/co2genMode.txt", "manual");
  writeFile("/configs/heaterThresh.txt", "25");
  writeFile("/configs/coolerThresh.txt", "30");
  writeFile("/configs/foggerThresh.txt", "60");
  writeFile("/configs/exfanThresh.txt", "70");
  writeFile("/configs/lightThresh.txt", "50");
  writeFile("/configs/pumpThresh.txt", "30");
  writeFile("/configs/co2genThresh.txt", "800");
  writeFile("/configs/delayTime.txt", "10");
  writeFile("/configs/errorTime.txt", "3600");
  writeFile("/configs/ignoreTime.txt", "3");
  writeFile("/configs/jalali.txt", "0");
  writeFile("/configs/buzzer.txt", "1");
  writeFile("/configs/dhcpMode.txt", "1");
  writeFile("/configs/forceLogin.txt", "1");
  writeFile("/configs/tempOffset.txt", "0");
  writeFile("/configs/rhOffset.txt", "0");
  writeFile("/configs/ip.txt", "192.168.1.100");
  writeFile("/configs/gateway.txt", "192.168.1.1");
  writeFile("/configs/subnet.txt", "255.255.255.0");
  writeFile("/configs/dns.txt", "8.8.8.8");
}

float readFloat(String path) {
  return readFile(path).toFloat();
}

int readInt(String path) {
  return readFile(path).toInt();
}

bool readBool(String path) {
  String s = readFile(path);
  return s == "1" || s == "true";
}

void eepromLoop() {
  if (digitalRead(RESET) == HIGH) {
    if (resetTime == 0) {
      resetTime = timeStamp + 5;
    }
    else if (resetTime < timeStamp) {
      LittleFS.format();
      ESP.restart();
    }
  }
  else {
    resetTime = 0;
  }
}

void timeSetup() {
  rtc.begin();
}

void timeLoop() {
  Wire.beginTransmission(0x68);
  if (Wire.endTransmission() == 0) {
    DateTime now = rtc.now();
    sprintf(cuHour, "%02d", now.hour());
    sprintf(cuMinute, "%02d", now.minute());
    sprintf(cuSecond, "%02d", now.second());
    sprintf(cuYear, "%04d", now.year());
    sprintf(cuMonth, "%02d", now.month());
    sprintf(cuDay, "%02d", now.day());
    int dayIndex = now.dayOfTheWeek();
    const char *daysOfWeek[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    strcpy(cuDayOfWeek, daysOfWeek[dayIndex]);
    secondsSinceMidnight = now.hour() * 3600 + now.minute() * 60 + now.second();
    DS3231 = true;
    DS3231Ignore = 0;
  }
  else {
    if (DS3231Ignore == 0) {
      DS3231Ignore = timeStamp + readInt("/configs/ignoreTime.txt");
    }
    else if (timeStamp > DS3231Ignore) {
      DS3231 = false;
    }
  }
  unsigned long currentMillis = millis();
  unsigned long delta;
  if (currentMillis >= lastMillis) {
    delta = currentMillis - lastMillis;
  }
  else {
    delta = (0xFFFFFFFFUL - lastMillis + 1) + currentMillis;
  }
  timeStamp += delta / 1000ULL;
  static unsigned long leftoverMs = 0;
  leftoverMs += delta % 1000;
  if (leftoverMs >= 1000) {
    timeStamp++;
    leftoverMs -= 1000;
  }
  lastMillis = currentMillis;
  uint64_t days = timeStamp / 86400ULL;
  unsigned int hours = (timeStamp % 86400ULL) / 3600UL;
  unsigned int minutes = (timeStamp % 3600UL) / 60UL;
  unsigned int seconds = timeStamp % 60UL;
  sprintf(upDay, "%02llu", days);
  sprintf(upHour, "%02u", hours);
  sprintf(upMinute, "%02u", minutes);
  sprintf(upSecond, "%02u", seconds);
}

int readSoilMoisture() {
  int sensorValue = analogRead(SOILSENS);
  int airValue = 3200;
  int waterValue = 1500;
  int moisturePercent = map(sensorValue, airValue, waterValue, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);
  return moisturePercent;
}

void sensorSetup() {
  pinMode(PHOTOCELL1, INPUT);
  pinMode(PHOTOCELL2, INPUT);
  pinMode(SOILSENS, INPUT);
  Wire.begin(SDAPIN, SCLPIN);
  dht.begin();
  sgp.begin();
}

void sensorLoop() {
  int ignore = readInt("/configs/ignoreTime.txt");
  float tempOff = readFloat("/configs/tempOffset.txt");
  float rhOff = readFloat("/configs/rhOffset.txt");
  
  if (!isnan(dht.readTemperature()) && !isnan(dht.readHumidity())) {
    if (dht.readTemperature() > 80 || dht.readTemperature() < -40 || dht.readHumidity() > 100 || dht.readHumidity() < 0) {
      if (DHT22Ignore == 0) {
        DHT22Ignore = timeStamp + ignore;
      }
      else if (timeStamp > DHT22Ignore) {
        DHT_22 = false;
      }
    }
    else {
      temperature = dht.readTemperature() + tempOff;
      humidity = dht.readHumidity() + rhOff;
      DHT_22 = true;
      DHT22Ignore = 0;
    }
  }
  else {
    if (DHT22Ignore == 0) {
      DHT22Ignore = timeStamp + ignore;
    }
    else if (timeStamp > DHT22Ignore) {
      DHT_22 = false;
    }
  }
  Wire.beginTransmission(0x58);
  if (Wire.endTransmission() == 0 && sgp.IAQmeasure()) {
    co2 = sgp.eCO2;
    tvoc = sgp.TVOC;
    SGP30 = true;
    SGP30Ignore = 0;
  }
  else {
    if (SGP30Ignore == 0) {
      SGP30Ignore = timeStamp + ignore;
    }
    else if (timeStamp > SGP30Ignore) {
      SGP30 = false;
    }
  }
  int total = 0;
  for (int i = 0; i < 10; i++) {
    total += analogRead(PHOTOCELL1);
  }
  int avg = total / 10;
  inBright = map(avg, 0, 1023, 0, 100);
  inBright = constrain(inBright, 0, 100);
  total = 0;
  for (int i = 0; i < 10; i++) {
    total += analogRead(PHOTOCELL2);
  }
  avg = total / 10;
  outBright = map(avg, 0, 1023, 0, 100);
  outBright = constrain(outBright, 0, 100);
  moist = readSoilMoisture();
  if (moist != 100) {
    SOIL = true;
    SOILIgnore = 0;
  }
  else {
    if (SOILIgnore == 0) {
      SOILIgnore = timeStamp + ignore;
    }
    else if (timeStamp > SOILIgnore) {
      SOIL = false;
    }
  }
}

void risingControl(String mode, bool &device, float sensorValue, float threshold, int delayTime, uint64_t &start, uint64_t &devTime, uint64_t &now) {
  if (mode == "automat") {
    if (!device) {
      if (sensorValue <= threshold) {
        if (start == 0) {
          start = now + delayTime;
        }
        else if (now >= start) {
          device = true;
          devTime = now;
          start = 0;
        }
      } else {
        start = 0;
      }
    }
    else {
      if (sensorValue > threshold) {
        if (start == 0) {
          start = now + delayTime;
        }
        else if (now >= start) {
          device = false;
          devTime = 0;
          start = 0;
        }
      } else {
        start = 0;
      }
    }
  } else {
    start = 0;
  }
}

void fallingControl(String mode, bool &device, float sensorValue, float threshold, int delayTime, uint64_t &start, uint64_t &devTime, uint64_t &now) {
  if (mode == "automat") {
    if (!device) {
      if (sensorValue >= threshold) {
        if (start == 0) {
          start = now + delayTime;
        }
        else if (now >= start) {
          device = true;
          devTime = now;
          start = 0;
        }
      } else {
        start = 0;
      }
    }
    else {
      if (sensorValue < threshold) {
        if (start == 0) {
          start = now + delayTime;
        }
        else if (now >= start) {
          device = false;
          devTime = 0;
          start = 0;
        }
      } else {
        start = 0;
      }
    }
  } else {
    start = 0;
  }
}

void relaySetup() {
  pinMode(HEATER, OUTPUT);
  digitalWrite(HEATER, !heater);
  pinMode(COOLER, OUTPUT);
  digitalWrite(COOLER, !cooler);
  pinMode(FOGGER, OUTPUT);
  digitalWrite(FOGGER, !fogger);
  pinMode(EXFAN, OUTPUT);
  digitalWrite(EXFAN, !exfan);
  pinMode(LIGHT, OUTPUT);
  digitalWrite(LIGHT, !light);
  pinMode(PUMP, OUTPUT);
  digitalWrite(PUMP, !pump);
  pinMode(CO2GEN, OUTPUT);
  digitalWrite(CO2GEN, !co2gen);
}

void relayLoop() {
  String hMode = readFile("/configs/heaterMode.txt");
  String cMode = readFile("/configs/coolerMode.txt");
  String fMode = readFile("/configs/foggerMode.txt");
  String eMode = readFile("/configs/exfanMode.txt");
  String pMode = readFile("/configs/pumpMode.txt");
  String gMode = readFile("/configs/co2genMode.txt");
  String lMode = readFile("/configs/lightMode.txt");
  int delayT = readInt("/configs/delayTime.txt");
  float hThresh = readFloat("/configs/heaterThresh.txt");
  float cThresh = readFloat("/configs/coolerThresh.txt");
  float fThresh = readFloat("/configs/foggerThresh.txt");
  float eThresh = readFloat("/configs/exfanThresh.txt");
  int pThresh = readInt("/configs/pumpThresh.txt");
  int gThresh = readInt("/configs/co2genThresh.txt");
  int lThresh = readInt("/configs/lightThresh.txt");
  
  if (DHT_22 && timeStamp != 0) {
    risingControl(hMode, heater, temperature, hThresh, delayT, heaterStart, heaterTime, timeStamp);
    fallingControl(cMode, cooler, temperature, cThresh, delayT, coolerStart, coolerTime, timeStamp);
    risingControl(fMode, fogger, humidity, fThresh, delayT, foggerStart, foggerTime, timeStamp);
    fallingControl(eMode, exfan, humidity, eThresh, delayT, exfanStart, exfanTime, timeStamp);
  }
  else {
    if (hMode == "automat") heater = false;
    if (cMode == "automat") cooler = false;
    if (fMode == "automat") fogger = false;
    if (eMode == "automat") exfan = false;
  }
  if (moist != 100 && timeStamp != 0) {
    risingControl(pMode, pump, moist, pThresh, delayT, pumpStart, pumpTime, timeStamp);
  }
  else {
    if (pMode == "automat") pump = false;
  }
  if (SGP30 && timeStamp != 0) {
    risingControl(gMode, co2gen, co2, gThresh, delayT, co2genStart, co2genTime, timeStamp);
  }
  else {
    if (gMode == "automat") co2gen = false;
  }
  if (timeStamp != 0) {
    risingControl(lMode, light, outBright, lThresh, delayT, lightStart, lightTime, timeStamp);
  }
  digitalWrite(HEATER, !heater);
  digitalWrite(COOLER, !cooler);
  digitalWrite(FOGGER, !fogger);
  digitalWrite(EXFAN, !exfan);
  digitalWrite(LIGHT, !light);
  digitalWrite(PUMP, !pump);
  digitalWrite(CO2GEN, !co2gen);
}

void alarmSetup() {
  pinMode(BUZZER, OUTPUT);
  if (readBool("/configs/buzzer.txt")) {
    delay(200);
    digitalWrite(BUZZER, HIGH);
    delay(150);
    digitalWrite(BUZZER, LOW);
    delay(200);
    digitalWrite(BUZZER, HIGH);
    delay(150);
    digitalWrite(BUZZER, LOW);
    delay(200);
  }
}

void alarmLoop() {
  bool buzzerEn = readBool("/configs/buzzer.txt");
  int errTime = readInt("/configs/errorTime.txt");
  String hMode = readFile("/configs/heaterMode.txt");
  String cMode = readFile("/configs/coolerMode.txt");
  String fMode = readFile("/configs/foggerMode.txt");
  String eMode = readFile("/configs/exfanMode.txt");
  String pMode = readFile("/configs/pumpMode.txt");
  String gMode = readFile("/configs/co2genMode.txt");
  
  if (!DS3231) {
    errorTitle = "Sensor Connection Error";
    errorText = "Failed to communicate with DS3231 (Time/Date) Please check the connections.";
    if (buzzerEn) {
      static bool status = false;
      digitalWrite(BUZZER, !status);
      status = !status;
    }
    else {
      digitalWrite(BUZZER, LOW);
    }
  }
  else if (!SGP30) {
    errorTitle = "Sensor Connection Error";
    errorText = "Failed to communicate with SGP30 (CO2/TVOC) Please check the connections.";
    if (buzzerEn) {
      static bool status = false;
      digitalWrite(BUZZER, !status);
      status = !status;
    }
    else {
      digitalWrite(BUZZER, LOW);
    }
  }
  else if (!DHT_22) {
    errorTitle = "Sensor Connection Error";
    errorText = "Failed to communicate with DHT22 (Temperature/Humidity) Please check the connections.";
    if (buzzerEn) {
      static bool status = false;
      digitalWrite(BUZZER, !status);
      status = !status;
    }
    else {
      digitalWrite(BUZZER, LOW);
    }
  }
  else if (!SOIL) {
    errorTitle = "Sensor Connection Error";
    errorText = "Failed to communicate with Soil moisture sensor Please check the connections.";
    if (buzzerEn) {
      static bool status = false;
      digitalWrite(BUZZER, !status);
      status = !status;
    }
    else {
      digitalWrite(BUZZER, LOW);
    }
  }
  else if (errTime < (timeStamp - heaterTime) && heaterTime != 0 && hMode == "automat") {
    errorTitle = "Heater Not Working";
    errorText = "The heater is not responding. Please check the power supply and device settings.";
    if (buzzerEn) {
      static bool status = false;
      digitalWrite(BUZZER, !status);
      status = !status;
    }
    else {
      digitalWrite(BUZZER, LOW);
    }
  }
  else if (errTime < (timeStamp - coolerTime) && coolerTime != 0 && cMode == "automat") {
    errorTitle = "Cooler Not Working";
    errorText = "The cooler is not responding. Please check the power supply and device settings.";
    if (buzzerEn) {
      static bool status = false;
      digitalWrite(BUZZER, !status);
      status = !status;
    }
    else {
      digitalWrite(BUZZER, LOW);
    }
  }
  else if (errTime < (timeStamp - foggerTime) && foggerTime != 0 && fMode == "automat") {
    errorTitle = "Fogger Not Working";
    errorText = "The fogger is not responding. Please check the power supply and device settings.";
    if (buzzerEn) {
      static bool status = false;
      digitalWrite(BUZZER, !status);
      status = !status;
    }
    else {
      digitalWrite(BUZZER, LOW);
    }
  }
  else if (errTime < (timeStamp - exfanTime) && exfanTime != 0 && eMode == "automat") {
    errorTitle = "Exfan Not Working";
    errorText = "The exfan is not responding. Please check the power supply and device settings.";
    if (buzzerEn) {
      static bool status = false;
      digitalWrite(BUZZER, !status);
      status = !status;
    }
    else {
      digitalWrite(BUZZER, LOW);
    }
  }
  else if (errTime < (timeStamp - pumpTime) && pumpTime != 0 && pMode == "automat") {
    errorTitle = "Pump Not Working";
    errorText = "The pump is not responding. Please check the power supply and device settings.";
    if (buzzerEn) {
      static bool status = false;
      digitalWrite(BUZZER, !status);
      status = !status;
    }
    else {
      digitalWrite(BUZZER, LOW);
    }
  }
  else if (errTime < (timeStamp - co2genTime) && co2genTime != 0 && gMode == "automat") {
    errorTitle = "CO₂ generator Not Working";
    errorText = "The co₂ generator is not responding. Please check the power supply and device settings.";
    if (buzzerEn) {
      static bool status = false;
      digitalWrite(BUZZER, !status);
      status = !status;
    }
    else {
      digitalWrite(BUZZER, LOW);
    }
  }
  else {
    errorTitle = "none";
    errorText = "none";
    digitalWrite(BUZZER, LOW);
  }
}

void record() {
  String filePath = "/history/" + String(cuDay) + "-" + String(cuMonth) + "-" + String(cuYear) + ".greg";
  File gregorian = LittleFS.open(filePath, "w");
  StaticJsonDocument<2048> doc;
  doc["minTemp"] = String(minTemp);
  doc["minTempTime"] = minTempTime;
  doc["maxTemp"] = String(maxTemp);
  doc["maxTempTime"] = maxTempTime;
  doc["minRH"] = String(minRH);
  doc["minRHTime"] = minRHTime;
  doc["maxRH"] = String(maxRH);
  doc["maxRHTime"] = maxRHTime;
  doc["minBright"] = String(minBright);
  doc["minBrightTime"] = minBrightTime;
  doc["maxBright"] = String(maxBright);
  doc["maxBrightTime"] = maxBrightTime;
  doc["minCo2"] = String(minCo2);
  doc["minCo2Time"] = minCo2Time;
  doc["maxCo2"] = String(maxCo2);
  doc["maxCo2Time"] = maxCo2Time;
  doc["minMoist"] = String(minMoist);
  doc["minMoistTime"] = minMoistTime;
  doc["maxMoist"] = String(maxMoist);
  doc["maxMoistTime"] = maxMoistTime;
  doc["minTvoc"] = String(minTvoc);
  doc["minTvocTime"] = minTvocTime;
  doc["maxTvoc"] = String(maxTvoc);
  doc["maxTvocTime"] = maxTvocTime;
  doc["heaterToday"] = String(heaterToday);
  doc["coolerToday"] = String(coolerToday);
  doc["foggerToday"] = String(foggerToday);
  doc["exfanToday"] = String(exfanToday);
  doc["lightToday"] = String(lightToday);
  doc["pumpToday"] = String(pumpToday);
  doc["co2genToday"] = String(co2genToday);
  String jsonString;
  serializeJson(doc, jsonString);
  gregorian.println(jsonString);
  gregorian.close();
}

void reload() {
  heaterToday = 0;
  coolerToday = 0;
  foggerToday = 0;
  exfanToday = 0;
  lightToday = 0;
  pumpToday = 0;
  co2genToday = 0;
  minTemp = temperature;
  maxTemp = temperature;
  minRH = humidity;
  maxRH = humidity;
  minBright = inBright;
  maxBright = inBright;
  minCo2 = co2;
  maxCo2 = co2;
  minMoist = moist;
  maxMoist = moist;
  minTvoc = tvoc;
  maxTvoc = tvoc;
  minTempTime = String(cuHour) + ":" + String(cuMinute);
  maxTempTime = String(cuHour) + ":" + String(cuMinute);
  minRHTime = String(cuHour) + ":" + String(cuMinute);
  maxRHTime = String(cuHour) + ":" + String(cuMinute);
  minBrightTime = String(cuHour) + ":" + String(cuMinute);
  maxBrightTime = String(cuHour) + ":" + String(cuMinute);
  minCo2Time = String(cuHour) + ":" + String(cuMinute);
  maxCo2Time = String(cuHour) + ":" + String(cuMinute);
  minMoistTime = String(cuHour) + ":" + String(cuMinute);
  maxMoistTime = String(cuHour) + ":" + String(cuMinute);
  minTvocTime = String(cuHour) + ":" + String(cuMinute);
  maxTvocTime = String(cuHour) + ":" + String(cuMinute);
}

void recordSetup() {
  if (!LittleFS.exists("/history")) {
    LittleFS.mkdir("/history");
  }
}

void recordLoop() {
  int currentDay = atoi(cuDay);
  if (!isInit) {
    String filePath = "/history/" + String(cuDay) + "-" + String(cuMonth) + "-" + String(cuYear) + ".greg";
    if (LittleFS.exists(filePath)) {
      File file = LittleFS.open(filePath, "r");
      if (file) {
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, file.readString());
        minTemp = doc["minTemp"].as<float>();
        minTempTime = doc["minTempTime"].as<String>();
        maxTemp = doc["maxTemp"].as<float>();
        maxTempTime = doc["maxTempTime"].as<String>();
        minRH = doc["minRH"].as<float>();
        minRHTime = doc["minRHTime"].as<String>();
        maxRH = doc["maxRH"].as<float>();
        maxRHTime = doc["maxRHTime"].as<String>();
        minBright = doc["minBright"].as<int>();
        minBrightTime = doc["minBrightTime"].as<String>();
        maxBright = doc["maxBright"].as<int>();
        maxBrightTime = doc["maxBrightTime"].as<String>();
        minCo2 = doc["minCo2"].as<int>();
        minCo2Time = doc["minCo2Time"].as<String>();
        maxCo2 = doc["maxCo2"].as<int>();
        maxCo2Time = doc["maxCo2Time"].as<String>();
        minMoist = doc["minMoist"].as<int>();
        minMoistTime = doc["minMoistTime"].as<String>();
        maxMoist = doc["maxMoist"].as<int>();
        maxMoistTime = doc["maxMoistTime"].as<String>();
        minTvoc = doc["minTvoc"].as<int>();
        minTvocTime = doc["minTvocTime"].as<String>();
        maxTvoc = doc["maxTvoc"].as<int>();
        maxTvocTime = doc["maxTvocTime"].as<String>();
        heaterToday = doc["heaterToday"].as<int>();
        coolerToday = doc["coolerToday"].as<int>();
        foggerToday = doc["foggerToday"].as<int>();
        exfanToday = doc["exfanToday"].as<int>();
        lightToday = doc["lightToday"].as<int>();
        pumpToday = doc["pumpToday"].as<int>();
        co2genToday = doc["co2genToday"].as<int>();
        file.close();
      }
    } else {
      reload();
      record();
    }
    lastDay = currentDay;
    isInit = true;
  }
  if (currentDay != lastDay) {
    reload();
    record();
    lastDay = currentDay;
    return;
  }
  if (DHT_22 && !(temperature == 0 && humidity == 0)) {
    if (minTemp > temperature) {
      minTemp = temperature;
      minTempTime = String(cuHour) + ":" + String(cuMinute);
      record();
    } else if (maxTemp < temperature) {
      maxTemp = temperature;
      maxTempTime = String(cuHour) + ":" + String(cuMinute);
      record();
    }
    if (minRH > humidity) {
      minRH = humidity;
      minRHTime = String(cuHour) + ":" + String(cuMinute);
      record();
    } else if (maxRH < humidity) {
      maxRH = humidity;
      maxRHTime = String(cuHour) + ":" + String(cuMinute);
      record();
    }
  }
  if (SGP30) {
    if (minCo2 > co2) {
      minCo2 = co2;
      minCo2Time = String(cuHour) + ":" + String(cuMinute);
      record();
    } else if (maxCo2 < co2) {
      maxCo2 = co2;
      maxCo2Time = String(cuHour) + ":" + String(cuMinute);
      record();
    }
    if (minTvoc > tvoc) {
      minTvoc = tvoc;
      minTvocTime = String(cuHour) + ":" + String(cuMinute);
      record();
    } else if (maxTvoc < tvoc) {
      maxTvoc = tvoc;
      maxTvocTime = String(cuHour) + ":" + String(cuMinute);
      record();
    }
  }
  if (moist != 100) {
    if (minMoist > moist) {
      minMoist = moist;
      minMoistTime = String(cuHour) + ":" + String(cuMinute);
      record();
    } else if (maxMoist < moist) {
      maxMoist = moist;
      maxMoistTime = String(cuHour) + ":" + String(cuMinute);
      record();
    }
  }
  if (minBright > inBright) {
    minBright = inBright;
    minBrightTime = String(cuHour) + ":" + String(cuMinute);
    record();
  } else if (maxBright < inBright) {
    maxBright = inBright;
    maxBrightTime = String(cuHour) + ":" + String(cuMinute);
    record();
  }
  if (timeStampOld != timeStamp) {
    if (heater) { heaterToday++; record(); }
    if (cooler) { coolerToday++; record(); }
    if (fogger) { foggerToday++; record(); }
    if (exfan) { exfanToday++; record(); }
    if (light) { lightToday++; record(); }
    if (pump) { pumpToday++; record(); }
    if (co2gen) { co2genToday++; record(); }
    timeStampOld = timeStamp;
  }
}

bool checkSchedule(const char* folder, bool *stat, const char* cuDayOfWeek, int secondsSinceMidnight, uint64_t timeStamp, uint64_t *outTime) {
  String path = String(folder) + String(cuDayOfWeek) + ".txt";
  if (!LittleFS.exists(path)) {
    *outTime = 0;
    return false;
  }
  File file = LittleFS.open(path, "r");
  if (!file) {
    *outTime = 0;
    return false;
  }
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;
    int sepIndex = line.indexOf('-');
    if (sepIndex == -1) continue;
    String first = line.substring(0, sepIndex);
    String second = line.substring(sepIndex + 1);
    first.trim();
    second.trim();
    int startSec = first.toInt();
    int endSec = second.toInt();
    if (startSec <= secondsSinceMidnight && secondsSinceMidnight <= endSec && timeStamp != 0) {
      file.close();
      if (!*stat) *outTime = timeStamp;
      return true;
    }
  }
  file.close();
  *outTime = 0;
  return false;
}

void scheduleLoop() {
  String hMode = readFile("/configs/heaterMode.txt");
  String cMode = readFile("/configs/coolerMode.txt");
  String fMode = readFile("/configs/foggerMode.txt");
  String eMode = readFile("/configs/exfanMode.txt");
  String lMode = readFile("/configs/lightMode.txt");
  String pMode = readFile("/configs/pumpMode.txt");
  String gMode = readFile("/configs/co2genMode.txt");
  
  if (hMode == "schedule") {
    heater = checkSchedule("/schedule/heater/", &heater, cuDayOfWeek, secondsSinceMidnight, timeStamp, &heaterTime);
  }
  if (cMode == "schedule") {
    cooler = checkSchedule("/schedule/cooler/", &cooler, cuDayOfWeek, secondsSinceMidnight, timeStamp, &coolerTime);
  }
  if (fMode == "schedule") {
    fogger = checkSchedule("/schedule/fogger/", &fogger, cuDayOfWeek, secondsSinceMidnight, timeStamp, &foggerTime);
  }
  if (eMode == "schedule") {
    exfan = checkSchedule("/schedule/exfan/", &exfan, cuDayOfWeek, secondsSinceMidnight, timeStamp, &exfanTime);
  }
  if (lMode == "schedule") {
    light = checkSchedule("/schedule/light/", &light, cuDayOfWeek, secondsSinceMidnight, timeStamp, &lightTime);
  }
  if (pMode == "schedule") {
    pump = checkSchedule("/schedule/pump/", &pump, cuDayOfWeek, secondsSinceMidnight, timeStamp, &pumpTime);
  }
  if (gMode == "schedule") {
    co2gen = checkSchedule("/schedule/co2gen/", &co2gen, cuDayOfWeek, secondsSinceMidnight, timeStamp, &co2genTime);
  }
}

void wifiSetup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  WiFi.setHostname("Atlas GH");
  
  String apName = readFile("/configs/apName.txt");
  String apPass = readFile("/configs/apPass.txt");
  bool dhcp = readBool("/configs/dhcpMode.txt");
  
  int channel = 6;
  bool ssid_hidden = 0;
  int max_connection = 4;
  WiFi.softAP(apName.c_str(), apPass.c_str(), channel, ssid_hidden, max_connection);
  // WiFi.setBandwidth(WIFI_IF_AP, WIFI_BW_HT20);  // <--- این خط را حذف کن یا کامنت کن
  
  if (!dhcp) {
    IPAddress ip, gateway, subnet, dns;
    ip.fromString(readFile("/configs/ip.txt"));
    gateway.fromString(readFile("/configs/gateway.txt"));
    subnet.fromString(readFile("/configs/subnet.txt"));
    dns.fromString(readFile("/configs/dns.txt"));
    WiFi.config(ip, gateway, subnet, dns);
  }
  WiFi.setAutoReconnect(true);
  
  String stName = readFile("/configs/stName.txt");
  String stPass = readFile("/configs/stPass.txt");
  WiFi.begin(stName.c_str(), stPass.c_str());
}

void apUpdate() {
  String apName = readFile("/configs/apName.txt");
  String apPass = readFile("/configs/apPass.txt");
  WiFi.softAP(apName.c_str(), apPass.c_str());
}

void stUpdate() {
  String stName = readFile("/configs/stName.txt");
  String stPass = readFile("/configs/stPass.txt");
  WiFi.begin(stName.c_str(), stPass.c_str());
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

void dnsSetup() {
  dnsServer.start(DNSPORT, "*", WiFi.softAPIP());
}

void dnsLoop() {
  dnsServer.processNextRequest();
}

String generateSession(int length = 32) {
  String s = "";
  for (int i = 0; i < length; i++)
    s += String(random(0, 16), HEX);
  return s;
}

bool isValidSession(String sessionID) {
  for (auto &s : sessions) {
    if (s == sessionID) return true;
  }
  return false;
}

void removeSession(String sessionID) {
  for (auto it = sessions.begin(); it != sessions.end(); ++it) {
    if (*it == sessionID) {
      sessions.erase(it);
      break;
    }
  }
}

void loginAPI(AsyncWebServer &server) {
  server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request) {
    String username = request->arg("username");
    String password = request->arg("password");
    String uname = readFile("/configs/username.txt");
    String pword = readFile("/configs/password.txt");
    if (username == uname && password == pword) {
      String sessionID = generateSession();
      sessions.push_back(sessionID);
      request->send(200, "application/json", "{\"response\":\"OK\", \"sessionID\":\"" + sessionID + "\"}");
    }
    else {
      request->send(200, "application/json", "{\"response\":\"Invalid\"}");
    }
  });
}

void logoutAPI(AsyncWebServer &server) {
  server.on("/logout", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool force = readBool("/configs/forceLogin.txt");
    if (isValidSession(request->arg("sessionID"))) {
      removeSession(request->arg("sessionID"));
      String response = "{\"response\":\"OK\",\"forceLogin\":";
      response += force ? "true" : "false";
      response += "}";
      request->send(200, "application/json", response);
    }
    else {
      String response = "{\"response\":\"Invalid\",\"forceLogin\":";
      response += force ? "true" : "false";
      response += "}";
      request->send(200, "application/json", response);
    }
  });
}

void handleDevice(AsyncWebServerRequest *request, const char* name, bool &device, String deviceMode, uint64_t &deviceTime, uint64_t timeStamp) {
  if (request->hasArg(name)) {
    String arg = request->arg(name);
    if (deviceMode == "manual") {
      if (arg == "true") {
        device = true;
        deviceTime = timeStamp;
      } else if (arg == "false") {
        device = false;
        deviceTime = 0;
      }
      request->send(200, "application/json", "{\"response\":\"OK\"}");
    }
    else if (deviceMode == "automat") {
      request->send(200, "application/json", "{\"response\":\"Automat\"}");
    }
    else if (deviceMode == "schedule") {
      request->send(200, "application/json", "{\"response\":\"Schedule\"}");
    }
  }
}

String diffTimeStamps(uint64_t ts1, uint64_t ts2) {
  if (ts2 != 0) {
    uint64_t diff = (ts1 > ts2) ? (ts1 - ts2) : (ts2 - ts1);
    unsigned int hours = diff / 3600;
    unsigned int minutes = (diff % 3600) / 60;
    unsigned int seconds = diff % 60;
    char buffer[9];
    snprintf(buffer, sizeof(buffer), "%02u:%02u:%02u", hours, minutes, seconds);
    return String(buffer);
  }
  else
    return "00:00:00";
}

String secondsToTimeString(unsigned long totalSeconds) {
  unsigned int hours = totalSeconds / 3600;
  unsigned int minutes = (totalSeconds % 3600) / 60;
  unsigned int seconds = totalSeconds % 60;
  char buffer[9];
  sprintf(buffer, "%02u:%02u:%02u", hours, minutes, seconds);
  return String(buffer);
}

String capitalizeFirst(const String& str) {
  if (str.length() == 0) return str;
  String result = str;
  result[0] = toupper(result[0]);
  return result;
}

void devicesAPI(AsyncWebServer &server) {
  server.on("/getDevices", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool force = readBool("/configs/forceLogin.txt");
    DynamicJsonDocument doc(4096);
    doc["login"] = isValidSession(request->arg("sessionID"));
    doc["forceLogin"] = force;
    if (!force || (force && isValidSession(request->arg("sessionID")))) {
      doc["heater"] = heater;
      doc["heaterMode"] = capitalizeFirst(readFile("/configs/heaterMode.txt"));
      doc["heaterTime"] = diffTimeStamps(timeStamp, heaterTime);
      doc["heaterToday"] = secondsToTimeString(heaterToday);
      doc["cooler"] = cooler;
      doc["coolerMode"] = capitalizeFirst(readFile("/configs/coolerMode.txt"));
      doc["coolerTime"] = diffTimeStamps(timeStamp, coolerTime);
      doc["coolerToday"] = secondsToTimeString(coolerToday);
      doc["fogger"] = fogger;
      doc["foggerMode"] = capitalizeFirst(readFile("/configs/foggerMode.txt"));
      doc["foggerTime"] = diffTimeStamps(timeStamp, foggerTime);
      doc["foggerToday"] = secondsToTimeString(foggerToday);
      doc["exfan"] = exfan;
      doc["exfanMode"] = capitalizeFirst(readFile("/configs/exfanMode.txt"));
      doc["exfanTime"] = diffTimeStamps(timeStamp, exfanTime);
      doc["exfanToday"] = secondsToTimeString(exfanToday);
      doc["light"] = light;
      doc["lightMode"] = capitalizeFirst(readFile("/configs/lightMode.txt"));
      doc["lightTime"] = diffTimeStamps(timeStamp, lightTime);
      doc["lightToday"] = secondsToTimeString(lightToday);
      doc["pump"] = pump;
      doc["pumpMode"] = capitalizeFirst(readFile("/configs/pumpMode.txt"));
      doc["pumpTime"] = diffTimeStamps(timeStamp, pumpTime);
      doc["pumpToday"] = secondsToTimeString(pumpToday);
      doc["co2gen"] = co2gen;
      doc["co2genMode"] = capitalizeFirst(readFile("/configs/co2genMode.txt"));
      doc["co2genTime"] = diffTimeStamps(timeStamp, co2genTime);
      doc["co2genToday"] = secondsToTimeString(co2genToday);
      doc["errorTitle"] = errorTitle;
      doc["errorText"] = errorText;
    }
    String resp;
    serializeJson(doc, resp);
    request->send(200, "application/json", resp);
  });
  server.on("/setDevices", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (isValidSession(request->arg("sessionID"))) {
      handleDevice(request, "heater", heater, readFile("/configs/heaterMode.txt"), heaterTime, timeStamp);
      handleDevice(request, "cooler", cooler, readFile("/configs/coolerMode.txt"), coolerTime, timeStamp);
      handleDevice(request, "fogger", fogger, readFile("/configs/foggerMode.txt"), foggerTime, timeStamp);
      handleDevice(request, "exfan", exfan, readFile("/configs/exfanMode.txt"), exfanTime, timeStamp);
      handleDevice(request, "light", light, readFile("/configs/lightMode.txt"), lightTime, timeStamp);
      handleDevice(request, "pump", pump, readFile("/configs/pumpMode.txt"), pumpTime, timeStamp);
      handleDevice(request, "co2gen", co2gen, readFile("/configs/co2genMode.txt"), co2genTime, timeStamp);
    }
    else {
      request->send(200, "application/json", "{\"response\":\"Login\"}");
    }
  });
}

bool isValidIP(String ip) {
  ip.trim();
  int parts[4] = {0, 0, 0, 0};
  int partIndex = 0;
  int length = ip.length();
  if (length < 7 || length > 15) return false;
  int start = 0;
  for (int i = 0; i <= length; i++) {
    if (i == length || ip.charAt(i) == '.') {
      if (partIndex > 3) return false;
      String partStr = ip.substring(start, i);
      partStr.trim();
      if (partStr.length() == 0) return false;
      if (partStr.length() > 3) return false;
      for (int j = 0; j < partStr.length(); j++) {
        if (!isDigit(partStr.charAt(j))) return false;
      }
      int partVal = 0;
      for (int j = 0; j < partStr.length(); j++) {
        partVal = partVal * 10 + (partStr.charAt(j) - '0');
      }
      if (partVal < 0 || partVal > 255) return false;
      if (partStr.length() > 1 && partStr.charAt(0) == '0') return false;
      parts[partIndex] = partVal;
      partIndex++;
      start = i + 1;
    }
  }
  if (partIndex != 4) return false;
  return true;
}

void networkAPI(AsyncWebServer &server) {
  server.on("/getNetwork", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool force = readBool("/configs/forceLogin.txt");
    DynamicJsonDocument doc(4096);
    if (isValidSession(request->arg("sessionID"))) {
      doc["login"] = true;
      doc["forceLogin"] = force;
      doc["apName"] = readFile("/configs/apName.txt");
      doc["apPass"] = readFile("/configs/apPass.txt");
      doc["stName"] = readFile("/configs/stName.txt");
      doc["stPass"] = readFile("/configs/stPass.txt");
      doc["ipAddress"] = WiFi.localIP().toString();
      doc["subnetMask"] = WiFi.subnetMask().toString();
      doc["gateway"] = WiFi.gatewayIP().toString();
      doc["dns"] = WiFi.dnsIP(0).toString();
      doc["username"] = readFile("/configs/username.txt");
      doc["dhcpMode"] = readBool("/configs/dhcpMode.txt");
      doc["errorTitle"] = errorTitle;
      doc["errorText"] = errorText;
    }
    else {
      doc["login"] = false;
      doc["forceLogin"] = force;
      doc["dhcpMode"] = true;
      doc["errorTitle"] = "Authentication Required";
      doc["errorText"] = "You must be logged in to view this information. Please login and try again.";
    }
    String resp;
    serializeJson(doc, resp);
    request->send(200, "application/json", resp);
  });
  server.on("/setNetwork", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (isValidSession(request->arg("sessionID"))) {
      bool apChange = false;
      bool stChange = false;
      if (request->arg("apName") == "") {
        request->send(200, "application/json", "{\"response\":\"apNameNull\"}");
        return;
      }
      else {
        if (readFile("/configs/apName.txt") != request->arg("apName")) apChange = true;
        writeFile("/configs/apName.txt", request->arg("apName"));
      }
      if (request->arg("apPass").length() < 8 && request->arg("apPass") != "") {
        request->send(200, "application/json", "{\"response\":\"apPassShort\"}");
        return;
      }
      else {
        if (readFile("/configs/apPass.txt") != request->arg("apPass")) apChange = true;
        writeFile("/configs/apPass.txt", request->arg("apPass"));
      }
      if (request->arg("stName") != readFile("/configs/stName.txt")) stChange = true;
      writeFile("/configs/stName.txt", request->arg("stName"));
      if (request->arg("stPass").length() < 8 && request->arg("stPass") != "") {
        request->send(200, "application/json", "{\"response\":\"stPassShort\"}");
        return;
      }
      else {
        if (readFile("/configs/stPass.txt") != request->arg("stPass")) stChange = true;
        writeFile("/configs/stPass.txt", request->arg("stPass"));
      }
      if (request->arg("newUsername") != "" && readFile("/configs/username.txt") != request->arg("newUsername")) {
        writeFile("/configs/username.txt", request->arg("newUsername"));
        sessions.clear();
      }
      if (request->arg("oldPassword") != "") {
        if (request->arg("oldPassword") != readFile("/configs/password.txt")) {
          request->send(200, "application/json", "{\"response\":\"wrongPass\"}");
          return;
        }
        else if (request->arg("newPassword") == "") {
          request->send(200, "application/json", "{\"response\":\"emptyPass\"}");
          return;
        }
        else if (request->arg("newPassword").length() < 8) {
          request->send(200, "application/json", "{\"response\":\"shortPass\"}");
          return;
        }
        if (readFile("/configs/password.txt") != request->arg("newPassword")) {
          writeFile("/configs/password.txt", request->arg("newPassword"));
          sessions.clear();
        }
      }
      if (request->arg("netMode") == "dhcp") {
        writeFile("/configs/dhcpMode.txt", "1");
      }
      else if (request->arg("netMode") == "static") {
        writeFile("/configs/dhcpMode.txt", "0");
        if (!isValidIP(request->arg("ip"))) {
          request->send(200, "application/json", "{\"response\":\"InvalidIP\"}");
          return;
        }
        else if (!isValidIP(request->arg("subnet"))) {
          request->send(200, "application/json", "{\"response\":\"InvalidIP\"}");
          return;
        }
        else if (!isValidIP(request->arg("gateway"))) {
          request->send(200, "application/json", "{\"response\":\"InvalidIP\"}");
          return;
        }
        else if (!isValidIP(request->arg("dns"))) {
          request->send(200, "application/json", "{\"response\":\"InvalidIP\"}");
          return;
        }
        writeFile("/configs/ip.txt", request->arg("ip"));
        writeFile("/configs/subnet.txt", request->arg("subnet"));
        writeFile("/configs/gateway.txt", request->arg("gateway"));
        writeFile("/configs/dns.txt", request->arg("dns"));
      }
      request->send(200, "application/json", "{\"response\":\"OK\"}");
      if (apChange) apUpdate();
      if (stChange) stUpdate();
    }
    else {
      request->send(200, "application/json", "{\"response\":\"Login\"}");
    }
  });
  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (isValidSession(request->arg("sessionID"))) {
      request->send(200, "application/json", "{\"response\":\"OK\"}");
      restart = true;
    }
    else {
      request->send(200, "application/json", "{\"response\":\"Login\"}");
    }
  });
}

void statusAPI(AsyncWebServer &server) {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/status");
  });
  server.on("/getStatus", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool force = readBool("/configs/forceLogin.txt");
    bool jalali = readBool("/configs/jalali.txt");
    String unit = readFile("/configs/unit.txt");
    DynamicJsonDocument doc(4096);
    doc["login"] = isValidSession(request->arg("sessionID"));
    doc["forceLogin"] = force;
    if (!force || (force && isValidSession(request->arg("sessionID")))) {
      if (DS3231) doc["time"] = String(cuHour) + ":" + String(cuMinute) + ":" + String(cuSecond);
      else doc["time"] = "--:--:--";
      if (jalali) {
        if (DS3231) {
          doc["date"] = String(cuDay) + "/" + String(cuMonth) + "/" + String(cuYear);
        }
        else {
          doc["date"] = "--/--/----";
        }
      }
      else {
        if (DS3231) {
          doc["date"] = String(cuDay) + "/" + String(cuMonth) + "/" + String(cuYear);
        }
        else {
          doc["date"] = "--/--/----";
        }
      }
      doc["dayOfWeek"] = cuDayOfWeek;
      if (DHT_22) {
        if (unit == "centigrade") doc["temperature"] = String(temperature);
        else if (unit == "fahrenheit") doc["temperature"] = String((temperature * 1.8 + 32));
        doc["humidity"] = String(humidity);
      }
      else {
        doc["temperature"] = "--";
        doc["humidity"] = "--";
      }
      doc["unit"] = unit;
      if (SOIL) doc["moist"] = moist;
      else doc["moist"] = "--";
      doc["inBright"] = inBright;
      doc["outBright"] = outBright;
      if (SGP30) {
        doc["co2"] = String(co2);
        doc["tvoc"] = String(tvoc);
      }
      else {
        doc["co2"] = "--";
        doc["tvoc"] = "--";
      }
      doc["upTime"] = String(upDay) + ":" + String(upHour) + ":" + String(upMinute) + ":" + String(upSecond);
      doc["jalali"] = jalali;
      doc["errorTitle"] = errorTitle;
      doc["errorText"] = errorText;
    }
    String resp;
    serializeJson(doc, resp);
    request->send(200, "application/json", resp);
  });
}

void historyAPI(AsyncWebServer &server) {
  server.on("/getHistory", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool jalali = readBool("/configs/jalali.txt");
    bool force = readBool("/configs/forceLogin.txt");
    if (!jalali) {
      File root = LittleFS.open("/history");
      if (!root || !root.isDirectory()) { return; }
      std::vector<String> years;
      File file = root.openNextFile();
      while (file) {
        String fname = file.name();
        if (fname.endsWith(".greg")) {
          fname = fname.substring(0, fname.length() - 5);
          int lastDash = fname.lastIndexOf('-');
          if (lastDash != -1 && lastDash + 1 < fname.length()) {
            String year = fname.substring(lastDash + 1);
            bool exists = false;
            for (auto &y : years) { if (y == year) { exists = true; break; } }
            if (!exists) years.push_back(year);
          }
        }
        file = root.openNextFile();
      }
      yearsList = "";
      for (auto &y : years) yearsList += "<option value='" + y + "'>" + y + "</option>";
    }
    else {
      File root = LittleFS.open("/history");
      if (!root || !root.isDirectory()) { return; }
      std::vector<String> years;
      File file = root.openNextFile();
      while (file) {
        String fname = file.name();
        if (fname.endsWith(".jal")) {
          fname = fname.substring(0, fname.length() - 4);
          int firstDash = fname.indexOf('-');
          if (firstDash != -1) {
            String year = fname.substring(0, firstDash);
            bool exists = false;
            for (auto &y : years) { if (y == year) { exists = true; break; } }
            if (!exists) years.push_back(year);
          }
        }
        file = root.openNextFile();
      }
      yearsList = "";
      for (auto &y : years) yearsList += "<option value='" + y + "'>" + y + "</option>";
    }
    DynamicJsonDocument doc(4096);
    doc["login"] = isValidSession(request->arg("sessionID"));
    doc["forceLogin"] = force;
    if (!force || (force && isValidSession(request->arg("sessionID")))) {
      doc["year"] = yearsList;
      if (!jalali)
        doc["month"] = "<option value='01'>January</option><option value='02'>February</option><option value='03'>March</option><option value='04'>April</option><option value='05'>May</option><option value='06'>June</option><option value='07'>July</option><option value='08'>August</option><option value='09'>September</option><option value='10'>October</option><option value='11'>November</option><option value='12'>December</option>";
      else
        doc["month"] = "<option value='01'>Farvardin</option><option value='02'>Ordibehesht</option><option value='03'>Khordad</option><option value='04'>Tir</option><option value='05'>Mordad</option><option value='06'>Shahrivar</option><option value='07'>Mehr</option><option value='08'>Aban</option><option value='09'>Azar</option><option value='10'>Dey</option><option value='11'>Bahman</option><option value='12'>Esfand</option>";
      doc["errorTitle"] = errorTitle;
      doc["errorText"] = errorText;
    }
    String resp;
    serializeJson(doc, resp);
    request->send(200, "application/json", resp);
  });
}

void reportsAPI(AsyncWebServer &server) {
  server.on("/getReports", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool force = readBool("/configs/forceLogin.txt");
    String unit = readFile("/configs/unit.txt");
    DynamicJsonDocument doc(4096);
    doc["login"] = isValidSession(request->arg("sessionID"));
    doc["forceLogin"] = force;
    if (!force || (force && isValidSession(request->arg("sessionID")))) {
      if (unit == "centigrade") doc["minTemp"] = String(minTemp);
      else if (unit == "fahrenheit") doc["minTemp"] = String((minTemp * 9.0 / 5.0 + 32));
      doc["minTempTime"] = minTempTime;
      if (unit == "centigrade") doc["maxTemp"] = String(maxTemp);
      else if (unit == "fahrenheit") doc["maxTemp"] = String((maxTemp * 9.0 / 5.0 + 32));
      doc["maxTempTime"] = maxTempTime;
      doc["minRH"] = String(minRH);
      doc["minRHTime"] = minRHTime;
      doc["maxRH"] = String(maxRH);
      doc["maxRHTime"] = maxRHTime;
      doc["minBright"] = String(minBright);
      doc["minBrightTime"] = minBrightTime;
      doc["maxBright"] = String(maxBright);
      doc["maxBrightTime"] = maxBrightTime;
      doc["minCo2"] = String(minCo2);
      doc["minCo2Time"] = minCo2Time;
      doc["maxCo2"] = String(maxCo2);
      doc["maxCo2Time"] = maxCo2Time;
      doc["minMoist"] = String(minMoist);
      doc["minMoistTime"] = minMoistTime;
      doc["maxMoist"] = String(maxMoist);
      doc["maxMoistTime"] = maxMoistTime;
      doc["minTvoc"] = String(minTvoc);
      doc["minTvocTime"] = minTvocTime;
      doc["maxTvoc"] = String(maxTvoc);
      doc["maxTvocTime"] = maxTvocTime;
      doc["unit"] = unit;
      doc["errorTitle"] = errorTitle;
      doc["errorText"] = errorText;
    }
    String resp;
    serializeJson(doc, resp);
    request->send(200, "application/json", resp);
  });
}

String buildScheduleOptions(const String& device) {
  const char *days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
  String result = "";
  for (int i = 0; i < 7; i++) {
    String day = days[i];
    String path = "/schedule/" + device + "/" + day + ".txt";
    if (!LittleFS.exists(path)) continue;
    File file = LittleFS.open(path, "r");
    if (!file) continue;
    while (file.available()) {
      String line = file.readStringUntil('\n');
      line.trim();
      if (line.length() == 0) continue;
      int sepIndex = line.indexOf('-');
      if (sepIndex == -1) continue;
      String first = line.substring(0, sepIndex);
      String second = line.substring(sepIndex + 1);
      first.trim();
      second.trim();
      result += "<option value='" + day + " (" + first + " - " + second + ")'>";
      result += day + " (" + secondsToTimeString(first.toInt()) + " - " + secondsToTimeString(second.toInt()) + ")</option>";
    }
    file.close();
  }
  return result;
}

void settingsAPI(AsyncWebServer &server) {
  server.on("/getSettings", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool force = readBool("/configs/forceLogin.txt");
    bool jalali = readBool("/configs/jalali.txt");
    String unit = readFile("/configs/unit.txt");
    DynamicJsonDocument doc(4096);
    doc["login"] = isValidSession(request->arg("sessionID"));
    doc["forceLogin"] = force;
    if (!force || (force && isValidSession(request->arg("sessionID")))) {
      doc["jalali"] = jalali;
      doc["hours"] = String(cuHour);
      doc["minutes"] = String(cuMinute);
      doc["seconds"] = String(cuSecond);
      doc["year"] = String(cuYear);
      doc["month"] = String(cuMonth);
      doc["day"] = String(cuDay);
      doc["unit"] = unit;
      doc["buzzer"] = readBool("/configs/buzzer.txt");
      doc["delay"] = readInt("/configs/delayTime.txt");
      doc["error"] = readInt("/configs/errorTime.txt");
      doc["ignore"] = readInt("/configs/ignoreTime.txt");
      doc["tempOffset"] = String(readFloat("/configs/tempOffset.txt"));
      doc["rhOffset"] = String(readFloat("/configs/rhOffset.txt"));
      doc["heaterMode"] = readFile("/configs/heaterMode.txt");
      if (unit == "centigrade") doc["heaterThresh"] = String(readFloat("/configs/heaterThresh.txt"));
      else if (unit == "fahrenheit") doc["heaterThresh"] = String((readFloat("/configs/heaterThresh.txt") * 1.8 + 32));
      doc["heaterList"] = buildScheduleOptions("heater");
      doc["coolerMode"] = readFile("/configs/coolerMode.txt");
      if (unit == "centigrade") doc["coolerThresh"] = String(readFloat("/configs/coolerThresh.txt"));
      else if (unit == "fahrenheit") doc["coolerThresh"] = String((readFloat("/configs/coolerThresh.txt") * 1.8 + 32));
      doc["coolerList"] = buildScheduleOptions("cooler");
      doc["foggerMode"] = readFile("/configs/foggerMode.txt");
      doc["foggerThresh"] = String(readFloat("/configs/foggerThresh.txt"));
      doc["foggerList"] = buildScheduleOptions("fogger");
      doc["exfanMode"] = readFile("/configs/exfanMode.txt");
      doc["exfanThresh"] = String(readFloat("/configs/exfanThresh.txt"));
      doc["exfanList"] = buildScheduleOptions("exfan");
      doc["lightMode"] = readFile("/configs/lightMode.txt");
      doc["lightThresh"] = String(readInt("/configs/lightThresh.txt"));
      doc["lightList"] = buildScheduleOptions("light");
      doc["pumpMode"] = readFile("/configs/pumpMode.txt");
      doc["pumpThresh"] = String(readInt("/configs/pumpThresh.txt"));
      doc["pumpList"] = buildScheduleOptions("pump");
      doc["co2genMode"] = readFile("/configs/co2genMode.txt");
      doc["co2genThresh"] = String(readInt("/configs/co2genThresh.txt"));
      doc["co2genList"] = buildScheduleOptions("co2gen");
      doc["errorTitle"] = errorTitle;
      doc["errorText"] = errorText;
    }
    String resp;
    serializeJson(doc, resp);
    request->send(200, "application/json", resp);
  });
  server.on("/setSettings", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (isValidSession(request->arg("sessionID"))) {
      if (readBool("/configs/jalali.txt")) {
        rtc.adjust(DateTime(request->arg("year").toInt(), request->arg("month").toInt(), request->arg("day").toInt(), request->arg("hours").toInt(), request->arg("minutes").toInt(), request->arg("seconds").toInt()));
      }
      else {
        rtc.adjust(DateTime(request->arg("year").toInt(), request->arg("month").toInt(), request->arg("day").toInt(), request->arg("hours").toInt(), request->arg("minutes").toInt(), request->arg("seconds").toInt()));
      }
      if (request->arg("unit") == "centigrade" || request->arg("unit") == "fahrenheit") writeFile("/configs/unit.txt", request->arg("unit"));
      if (request->arg("calc") == "gregorian") writeFile("/configs/jalali.txt", "0");
      else if (request->arg("calc") == "jalali") writeFile("/configs/jalali.txt", "1");
      if (request->arg("buzzer") == "enable") writeFile("/configs/buzzer.txt", "1");
      else if (request->arg("buzzer") == "disable") writeFile("/configs/buzzer.txt", "0");
      if (request->arg("forceLogin") == "enable") writeFile("/configs/forceLogin.txt", "1");
      else if (request->arg("forceLogin") == "disable") writeFile("/configs/forceLogin.txt", "0");
      writeFile("/configs/delayTime.txt", request->arg("delay"));
      writeFile("/configs/errorTime.txt", request->arg("error"));
      writeFile("/configs/ignoreTime.txt", request->arg("ignore"));
      writeFile("/configs/tempOffset.txt", request->arg("tempOffset"));
      writeFile("/configs/rhOffset.txt", request->arg("rhOffset"));
      if (request->arg("heater") == "manual" || request->arg("heater") == "automat" || request->arg("heater") == "schedule") {
        if (readFile("/configs/heaterMode.txt") != "manual" && request->arg("heater") == "manual") { heater = false; heaterTime = 0; }
        writeFile("/configs/heaterMode.txt", request->arg("heater"));
      }
      if (readFile("/configs/unit.txt") == "centigrade") writeFile("/configs/heaterThresh.txt", request->arg("heaterThresh"));
      else if (readFile("/configs/unit.txt") == "fahrenheit") writeFile("/configs/heaterThresh.txt", String((request->arg("heaterThresh").toFloat() - 32) / 1.8));
      if (request->arg("cooler") == "manual" || request->arg("cooler") == "automat" || request->arg("cooler") == "schedule") {
        if (readFile("/configs/coolerMode.txt") != "manual" && request->arg("cooler") == "manual") { cooler = false; coolerTime = 0; }
        writeFile("/configs/coolerMode.txt", request->arg("cooler"));
      }
      if (readFile("/configs/unit.txt") == "centigrade") writeFile("/configs/coolerThresh.txt", request->arg("coolerThresh"));
      else if (readFile("/configs/unit.txt") == "fahrenheit") writeFile("/configs/coolerThresh.txt", String((request->arg("coolerThresh").toFloat() - 32) / 1.8));
      if (request->arg("fogger") == "manual" || request->arg("fogger") == "automat" || request->arg("fogger") == "schedule") {
        if (readFile("/configs/foggerMode.txt") != "manual" && request->arg("fogger") == "manual") { fogger = false; foggerTime = 0; }
        writeFile("/configs/foggerMode.txt", request->arg("fogger"));
      }
      writeFile("/configs/foggerThresh.txt", request->arg("foggerThresh"));
      if (request->arg("exfan") == "manual" || request->arg("exfan") == "automat" || request->arg("exfan") == "schedule") {
        if (readFile("/configs/exfanMode.txt") != "manual" && request->arg("exfan") == "manual") { exfan = false; exfanTime = 0; }
        writeFile("/configs/exfanMode.txt", request->arg("exfan"));
      }
      writeFile("/configs/exfanThresh.txt", request->arg("exfanThresh"));
      if (request->arg("light") == "manual" || request->arg("light") == "automat" || request->arg("light") == "schedule") {
        if (readFile("/configs/lightMode.txt") != "manual" && request->arg("light") == "manual") { light = false; lightTime = 0; }
        writeFile("/configs/lightMode.txt", request->arg("light"));
      }
      writeFile("/configs/lightThresh.txt", request->arg("lightThresh"));
      if (request->arg("pump") == "manual" || request->arg("pump") == "automat" || request->arg("pump") == "schedule") {
        if (readFile("/configs/pumpMode.txt") != "manual" && request->arg("pump") == "manual") { pump = false; pumpTime = 0; }
        writeFile("/configs/pumpMode.txt", request->arg("pump"));
      }
      writeFile("/configs/pumpThresh.txt", request->arg("pumpThresh"));
      if (request->arg("co2gen") == "manual" || request->arg("co2gen") == "automat" || request->arg("co2gen") == "schedule") {
        if (readFile("/configs/co2genMode.txt") != "manual" && request->arg("co2gen") == "manual") { co2gen = false; co2genTime = 0; }
        writeFile("/configs/co2genMode.txt", request->arg("co2gen"));
      }
      writeFile("/configs/co2genThresh.txt", request->arg("co2genThresh"));
      request->send(200, "application/json", "{\"response\":\"OK\"}");
    }
    else {
      request->send(200, "application/json", "{\"response\":\"Login\"}");
    }
  });
}

void addFileRoute(const char* uri, const char* path, const char* contentType) {
  server.on(uri, HTTP_GET, [path, contentType](AsyncWebServerRequest *request) {
    request->send(LittleFS, path, contentType);
  });
}

void webSetup() {
  statusAPI(server);
  devicesAPI(server);
  loginAPI(server);
  logoutAPI(server);
  reportsAPI(server);
  historyAPI(server);
  settingsAPI(server);
  networkAPI(server);
  server.begin();
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->redirect("/status");
  });
  addFileRoute("/status", "/status.html", "text/html");
  addFileRoute("/devices", "/devices.html", "text/html");
  addFileRoute("/reports", "/reports.html", "text/html");
  addFileRoute("/history", "/history.html", "text/html");
  addFileRoute("/settings", "/settings.html", "text/html");
  addFileRoute("/network", "/network.html", "text/html");
  addFileRoute("/about", "/about.html", "text/html");
  addFileRoute("/login", "/login.html", "text/html");
  addFileRoute("/assets/css/bootstrap.min.css", "/assets/css/bootstrap.min.css", "text/css");
  addFileRoute("/assets/css/style.css", "/assets/css/style.css", "text/css");
  addFileRoute("/assets/js/jquery-3.7.1.min.js", "/assets/js/jquery-3.7.1.min.js", "application/javascript");
  addFileRoute("/assets/js/bootstrap.min.js", "/assets/js/bootstrap.min.js", "application/javascript");
  addFileRoute("/assets/js/script.js", "/assets/js/script.js", "application/javascript");
  addFileRoute("/assets/js/chart.js", "/assets/js/chart.js", "application/javascript");
}

void setup() {
  configSetup();
  wifiSetup();
  sensorSetup();
  recordSetup();
  webSetup();
  dnsSetup();
  timeSetup();
  alarmSetup();
  relaySetup();
  pinMode(RESET, INPUT_PULLDOWN);
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
