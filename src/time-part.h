#include "RTClib.h"
RTC_DS3231 rtc;
unsigned long lastMillis = 0;
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
uint64_t DS3231Ignore = 0;
unsigned int secondsSinceMidnight = 0;
bool DS3231 = false;

void timeSetup()
{
    rtc.begin();
}

void timeLoop()
{
    Wire.beginTransmission(0x68);
    if (Wire.endTransmission() == 0)
    {
        DateTime now = rtc.now();
        sprintf(cuHour, "%02d", now.hour());
        sprintf(cuMinute, "%02d", now.minute());
        sprintf(cuSecond, "%02d", now.second());
        sprintf(cuYear, "%04d", now.year());
        sprintf(cuMonth, "%02d", now.month());
        sprintf(cuDay, "%02d", now.day());
        int dayIndex = now.dayOfTheWeek();
        const char *daysOfWeek[7] = {
            "Sunday", "Monday", "Tuesday", "Wednesday",
            "Thursday", "Friday", "Saturday"};
        strcpy(cuDayOfWeek, daysOfWeek[dayIndex]);
        secondsSinceMidnight = now.hour() * 3600 + now.minute() * 60 + now.second();
        DS3231 = true;
        DS3231Ignore = 0;
    }
    else
    {
        if (DS3231Ignore == 0)
        {
            DS3231Ignore = timeStamp + eeprom.ignoreTime;
        }
        else if (timeStamp > DS3231Ignore)
        {
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