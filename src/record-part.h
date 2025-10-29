#include <Jalali.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <stdlib.h>

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
bool isInit = false;
uint64_t timeStampOld = 0;
int lastDay = -1;

void record() {
    String filePath = "/history/" + String(cuDay) + "-" + String(cuMonth) + "-" + String(cuYear) + ".greg";
    File gregorian = LittleFS.open(filePath, "w");
    Jalali jal;
    int jYear, jMonth, jDay;
    jal.convert_date(atoi(cuYear), atoi(cuMonth), atoi(cuDay), jYear, jMonth, jDay, GREGORIAN);
    char dateStr[11];
    sprintf(dateStr, "%04d-%02d-%02d", jYear, jMonth, jDay);
    File jalali = LittleFS.open("/history/" + String(dateStr) + ".jal", "w");
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
    jalali.println(jsonString);
    gregorian.close();
    jalali.close();
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
    LittleFS.begin();
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
        if (heater) {
            heaterToday++;
            record();
        }
        if (cooler) {
            coolerToday++;
            record();
        }
        if (fogger) {
            foggerToday++;
            record();
        }
        if (exfan) {
            exfanToday++;
            record();
        }
        if (light) {
            lightToday++;
            record();
        }
        if (pump) {
            pumpToday++;
            record();
        }
        if (co2gen) {
            co2genToday++;
            record();
        }
        timeStampOld = timeStamp;
    }
}