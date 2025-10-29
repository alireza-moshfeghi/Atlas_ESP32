#define BUZZER 15
bool status = false;
String errorTitle = "none";
String errorText = "none";

void alarmSetup() {
    pinMode(BUZZER, OUTPUT);
    if (eeprom.buzzer) {
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
    if (!DS3231) {
        errorTitle = "Sensor Connection Error";
        errorText = "Failed to communicate with DS3231 (Time/Date) Please check the connections.";
        if (eeprom.buzzer) {
            digitalWrite(BUZZER, !status);
            status = !status;
        }
        else {
            digitalWrite(BUZZER, LOW);
            status = false;
        }
    }
    else if (!SGP30) {
        errorTitle = "Sensor Connection Error";
        errorText = "Failed to communicate with SGP30 (CO2/TVOC) Please check the connections.";
        if (eeprom.buzzer) {
            digitalWrite(BUZZER, !status);
            status = !status;
        }
        else {
            digitalWrite(BUZZER, LOW);
            status = false;
        }
    }
    else if (!DHT_22) {
        errorTitle = "Sensor Connection Error";
        errorText = "Failed to communicate with DHT22 (Temperature/Humidity) Please check the connections.";
        if (eeprom.buzzer) {
            digitalWrite(BUZZER, !status);
            status = !status;
        }
        else {
            digitalWrite(BUZZER, LOW);
            status = false;
        }
    }
    else if (!SOIL) {
        errorTitle = "Sensor Connection Error";
        errorText = "Failed to communicate with Soil moisture sensor Please check the connections.";
        if (eeprom.buzzer) {
            digitalWrite(BUZZER, !status);
            status = !status;
        }
        else {
            digitalWrite(BUZZER, LOW);
            status = false;
        }
    }
    else if (eeprom.errorTime < (timeStamp - heaterTime) && heaterTime != 0 && eeprom.heaterMode == "automat") {
        errorTitle = "Heater Not Working";
        errorText = "The heater is not responding. Please check the power supply and device settings.";
        if (eeprom.buzzer) {
            digitalWrite(BUZZER, !status);
            status = !status;
        }
        else {
            digitalWrite(BUZZER, LOW);
            status = false;
        }
    }
    else if (eeprom.errorTime < (timeStamp - coolerTime) && coolerTime != 0 && eeprom.coolerMode == "automat") {
        errorTitle = "Cooler Not Working";
        errorText = "The cooler is not responding. Please check the power supply and device settings.";
        if (eeprom.buzzer) {
            digitalWrite(BUZZER, !status);
            status = !status;
        }
        else {
            digitalWrite(BUZZER, LOW);
            status = false;
        }
    }
    else if (eeprom.errorTime < (timeStamp - foggerTime) && foggerTime != 0 && eeprom.foggerMode == "automat") {
        errorTitle = "Fogger Not Working";
        errorText = "The fogger is not responding. Please check the power supply and device settings.";
        if (eeprom.buzzer) {
            digitalWrite(BUZZER, !status);
            status = !status;
        }
        else {
            digitalWrite(BUZZER, LOW);
            status = false;
        }
    }
    else if (eeprom.errorTime < (timeStamp - exfanTime) && exfanTime != 0 && eeprom.exfanMode == "automat") {
        errorTitle = "Exfan Not Working";
        errorText = "The exfan is not responding. Please check the power supply and device settings.";
        if (eeprom.buzzer) {
            digitalWrite(BUZZER, !status);
            status = !status;
        }
        else {
            digitalWrite(BUZZER, LOW);
            status = false;
        }
    }
    else if (eeprom.errorTime < (timeStamp - pumpTime) && pumpTime != 0 && eeprom.pumpMode == "automat") {
        errorTitle = "Pump Not Working";
        errorText = "The pump is not responding. Please check the power supply and device settings.";
        if (eeprom.buzzer) {
            digitalWrite(BUZZER, !status);
            status = !status;
        }
        else {
            digitalWrite(BUZZER, LOW);
            status = false;
        }
    }
    else if (eeprom.errorTime < (timeStamp - co2genTime) && co2genTime != 0 && eeprom.co2genMode == "automat") {
        errorTitle = "CO₂ generator Not Working";
        errorText = "The co₂ generator is not responding. Please check the power supply and device settings.";
        if (eeprom.buzzer) {
            digitalWrite(BUZZER, !status);
            status = !status;
        }
        else {
            digitalWrite(BUZZER, LOW);
            status = false;
        }
    }
    else {
        errorTitle = "none";
        errorText = "none";
        digitalWrite(BUZZER, LOW);
        status = false;
    }
}