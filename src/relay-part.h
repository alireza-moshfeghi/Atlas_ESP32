#define HEATER 5
#define COOLER 18
#define FOGGER 19
#define EXFAN 21
#define LIGHT 27
#define PUMP 26
#define CO2GEN 25
bool heater = false;
bool cooler = false;
bool fogger = false;
bool exfan = false;
bool light = false;
bool pump = false;
bool co2gen = false;
uint64_t heaterTime = 0;
uint64_t coolerTime = 0;
uint64_t foggerTime = 0;
uint64_t exfanTime = 0;
uint64_t lightTime = 0;
uint64_t pumpTime = 0;
uint64_t co2genTime = 0;
uint64_t customTime = 0;
uint64_t heaterStart = 0;
uint64_t coolerStart = 0;
uint64_t foggerStart = 0;
uint64_t exfanStart = 0;
uint64_t lightStart = 0;
uint64_t pumpStart = 0;
uint64_t co2genStart = 0;

void risingControl(
    String mode,
    bool &device,
    float sensorValue,
    float threshold,
    int delayTime,
    uint64_t &start,
    uint64_t &devTime,
    uint64_t &now) {
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

void fallingControl(
    String mode,
    bool &device,
    float sensorValue,
    float threshold,
    int delayTime,
    uint64_t &start,
    uint64_t &devTime,
    uint64_t &now) {
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

void relayLoop();

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
    if (DHT_22 && timeStamp != 0) {
        risingControl(
            eeprom.heaterMode,
            heater,
            temperature,
            eeprom.heaterThresh,
            eeprom.delayTime,
            heaterStart,
            heaterTime,
            timeStamp
        );
        fallingControl(
            eeprom.coolerMode,
            cooler,
            temperature,
            eeprom.coolerThresh,
            eeprom.delayTime,
            coolerStart,
            coolerTime,
            timeStamp
        );
        risingControl(
            eeprom.foggerMode,
            fogger,
            humidity,
            eeprom.foggerThresh,
            eeprom.delayTime,
            foggerStart,
            foggerTime,
            timeStamp
        );
        fallingControl(
            eeprom.exfanMode,
            exfan,
            humidity,
            eeprom.exfanThresh,
            eeprom.delayTime,
            exfanStart,
            exfanTime,
            timeStamp
        );
    }
    else {
        if (eeprom.heaterMode == "automat") 
            heater = false;
        if (eeprom.coolerMode == "automat")
            cooler = false;
        if (eeprom.foggerMode == "automat")
            fogger = false;
        if (eeprom.exfanMode == "automat")
            exfan = false;
    }
    if (moist != 100 && timeStamp != 0) {
        risingControl(
            eeprom.pumpMode,
            pump,
            moist,
            eeprom.pumpThresh,
            eeprom.delayTime,
            pumpStart,
            pumpTime,
            timeStamp
        );
    }
    else {
        if (eeprom.pumpMode == "automat")
            pump = false;
    }
    if (SGP30 && timeStamp != 0) {
        risingControl(
            eeprom.co2genMode,
            co2gen,
            co2,
            eeprom.co2genThresh,
            eeprom.delayTime,
            co2genStart,
            co2genTime,
            timeStamp
        );
    }
    else {
        if (eeprom.co2genMode == "automat")
            co2gen = false;
    }
    if (timeStamp != 0) {
        risingControl(
            eeprom.lightMode,
            light,
            outBright,
            eeprom.lightThresh,
            eeprom.delayTime,
            lightStart,
            lightTime,
            timeStamp
        );
    }
    digitalWrite(HEATER, !heater);
    digitalWrite(COOLER, !cooler);
    digitalWrite(FOGGER, !fogger);
    digitalWrite(EXFAN, !exfan);
    digitalWrite(LIGHT, !light);
    digitalWrite(PUMP, !pump);
    digitalWrite(CO2GEN, !co2gen);
}