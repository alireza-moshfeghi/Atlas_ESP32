void statusAPI(AsyncWebServer &server) {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->redirect("/status");
    });
    server.on("/getStatus", HTTP_POST, [](AsyncWebServerRequest *request){
        DynamicJsonDocument doc(4096);
        doc["login"] = isValidSession(request->arg("sessionID"));
        doc["forceLogin"] = eeprom.forceLogin;
        if (!eeprom.forceLogin || (eeprom.forceLogin && isValidSession(request->arg("sessionID")))) {
            if (DS3231)
                doc["time"] = String(cuHour) + ":" + String(cuMinute) + ":" + String(cuSecond);
            else
                doc["time"] = "--:--:--";
            if (eeprom.jalali) {
                if (DS3231) {
                    Jalali jal;
                    int jYear, jMonth, jDay;
                    jal.convert_date(atoi(cuYear), atoi(cuMonth), atoi(cuDay), jYear, jMonth, jDay, GREGORIAN);
                    char dateStr[11];
                    sprintf(dateStr, "%02d/%02d/%04d", jDay, jMonth, jYear);
                    doc["date"] = dateStr;
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
                if (eeprom.unit == "centigrade")
                    doc["temperature"] = String(temperature);
                else if (eeprom.unit == "fahrenheit")
                    doc["temperature"] = String((temperature * 1.8 + 32));
                doc["humidity"] = String(humidity);
            }
            else {
                doc["temperature"] = "--";
                doc["humidity"] = "--";
            }
            doc["unit"] = eeprom.unit;
            if (SOIL)
                doc["moist"] = moist;
            else
                doc["moist"] = "--";
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
            doc["jalali"] = eeprom.jalali;
            doc["errorTitle"] = errorTitle;
            doc["errorText"] = errorText;
        }
        String resp;
        serializeJson(doc, resp);
        request->send(200, "application/json", resp);
    });
}