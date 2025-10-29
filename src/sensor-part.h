#include <DHT.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Adafruit_SGP30.h>
#define DHTPIN 4
#define SDAPIN 16
#define SCLPIN 17
#define PHOTOCELL1 34
#define PHOTOCELL2 35
#define SOILSENS 32
DHT dht(DHTPIN, DHT22);
Adafruit_SGP30 sgp;
float temperature = 0;
float humidity = 0;
int co2 = 0;
int tvoc = 0;
int inBright = 0;
int outBright = 0;
int moist = 0;
uint64_t DHT22Ignore = 0;
uint64_t SGP30Ignore = 0;
uint64_t SOILIgnore = 0;
bool DHT_22 = false;
bool SGP30 = false;
bool SOIL = false;

int readSoilMoisture()
{
    int sensorValue = analogRead(SOILSENS);
    int airValue = 3200;
    int waterValue = 1500;
    int moisturePercent = map(sensorValue, airValue, waterValue, 0, 100);
    moisturePercent = constrain(moisturePercent, 0, 100);
    return moisturePercent;
}

void sensorSetup()
{
    pinMode(PHOTOCELL1, INPUT);
    pinMode(PHOTOCELL2, INPUT);
    pinMode(SOILSENS, INPUT);
    Wire.begin(SDAPIN, SCLPIN);
    dht.begin();
    sgp.begin();
}

void sensorLoop()
{
    if (!isnan(dht.readTemperature()) && !isnan(dht.readHumidity()))
    {
        if (dht.readTemperature() > 80 || dht.readTemperature() < -40 || dht.readHumidity() > 100 || dht.readHumidity() < 0) {
            if (DHT22Ignore == 0) {
                DHT22Ignore = timeStamp + eeprom.ignoreTime;
            }
            else if (timeStamp > DHT22Ignore) {
                DHT_22 = false;
            }
        }
        else {
            temperature = dht.readTemperature() + eeprom.tempOffset;
            humidity = dht.readHumidity() + eeprom.rhOffset;
            DHT_22 = true;
            DHT22Ignore = 0;
        }
    }
    else
    {
        if (DHT22Ignore == 0) {
            DHT22Ignore = timeStamp + eeprom.ignoreTime;
        }
        else if (timeStamp > DHT22Ignore) {
            DHT_22 = false;
        }
    }
    Wire.beginTransmission(0x58);
    if (Wire.endTransmission() == 0 && sgp.IAQmeasure())
    {
        co2 = sgp.eCO2;
        tvoc = sgp.TVOC;
        SGP30 = true;
        SGP30Ignore = 0;
    }
    else
    {
        if (SGP30Ignore == 0) {
            SGP30Ignore = timeStamp + eeprom.ignoreTime;
        }
        else if (timeStamp > SGP30Ignore) {
            SGP30 = false;
        }
    }
    int total = 0;
    for (int i = 0; i < 10; i++)
    {
        total += analogRead(PHOTOCELL1);
    }
    int avg = total / 10;
    inBright = map(avg, 0, 1023, 0, 100);
    inBright = constrain(inBright, 0, 100);
    total = 0;
    for (int i = 0; i < 10; i++)
    {
        total += analogRead(PHOTOCELL2);
    }
    avg = total / 10;
    outBright = map(avg, 0, 1023, 0, 100);
    outBright = constrain(outBright, 0, 100);
    moist = readSoilMoisture();
    if (moist != 100)
    {
        SOIL = true;
        SOILIgnore = 0;
    }
    else
    {
        if (SOILIgnore == 0) {
            SOILIgnore = timeStamp + eeprom.ignoreTime;
        }
        else if (timeStamp > SOILIgnore) {
            SOIL = false;
        }
    }
}