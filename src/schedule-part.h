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
            if (!*stat)
                *outTime = timeStamp;
            return true;
        }
    }
    file.close();
    *outTime = 0;
    return false;
}


void scheduleLoop()
{
    if (eeprom.heaterMode == "schedule") {
        heater = checkSchedule("/schedule/heater/", &heater, cuDayOfWeek, secondsSinceMidnight, timeStamp, &heaterTime);
    }
    if (eeprom.coolerMode == "schedule") {
        cooler = checkSchedule("/schedule/cooler/", &cooler, cuDayOfWeek, secondsSinceMidnight, timeStamp, &coolerTime);
    }
    if (eeprom.foggerMode == "schedule") {
        fogger = checkSchedule("/schedule/fogger/", &fogger, cuDayOfWeek, secondsSinceMidnight, timeStamp, &foggerTime);
    }
    if (eeprom.exfanMode == "schedule") {
        exfan = checkSchedule("/schedule/exfan/", &exfan, cuDayOfWeek, secondsSinceMidnight, timeStamp, &exfanTime);
    }
    if (eeprom.lightMode == "schedule") {
        light = checkSchedule("/schedule/light/", &light, cuDayOfWeek, secondsSinceMidnight, timeStamp, &lightTime);
    }
    if (eeprom.pumpMode == "schedule") {
        pump = checkSchedule("/schedule/pump/", &pump, cuDayOfWeek, secondsSinceMidnight, timeStamp, &pumpTime);
    }
    if (eeprom.co2genMode == "schedule") {
        co2gen = checkSchedule("/schedule/co2gen/", &co2gen, cuDayOfWeek, secondsSinceMidnight, timeStamp, &co2genTime);
    }
    
}