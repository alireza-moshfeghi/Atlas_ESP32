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

void settingsAPI(AsyncWebServer &server)
{
    server.on("/getSettings", HTTP_POST, [](AsyncWebServerRequest *request) {
        DynamicJsonDocument doc(4096);
        doc["login"] = isValidSession(request->arg("sessionID"));
        doc["forceLogin"] = eeprom.forceLogin;
        if (!eeprom.forceLogin || (eeprom.forceLogin && isValidSession(request->arg("sessionID")))) {
            doc["jalali"] = eeprom.jalali;
            doc["hours"] = String(cuHour);
            doc["minutes"] = String(cuMinute);
            doc["seconds"] = String(cuSecond);
            if (eeprom.jalali) {
                Jalali jal;
                int jYear, jMonth, jDay;
                jal.convert_date(atoi(cuYear), atoi(cuMonth), atoi(cuDay), jYear, jMonth, jDay, GREGORIAN);
                doc["year"] = String(jYear);
                doc["month"] = String(jMonth);
                doc["day"] = String(jDay);
            }
            else {
                doc["year"] = String(cuYear);
                doc["month"] = String(cuMonth);
                doc["day"] = String(cuDay);
            }
            doc["unit"] = eeprom.unit;
            doc["buzzer"] = eeprom.buzzer;
            doc["forceLogin"] = eeprom.forceLogin;
            doc["delay"] = eeprom.delayTime;
            doc["error"] = eeprom.errorTime;
            doc["ignore"] = eeprom.ignoreTime;
            doc["tempOffset"] = String(eeprom.tempOffset);
            doc["rhOffset"] = String(eeprom.rhOffset);
            doc["heaterMode"] = eeprom.heaterMode;
            if (eeprom.unit == "centigrade")
                doc["heaterThresh"] = String(eeprom.heaterThresh);
            else if (eeprom.unit == "fahrenheit")
                doc["heaterThresh"] = String((eeprom.heaterThresh * 1.8 + 32));
            doc["heaterList"] = buildScheduleOptions("heater");
            doc["coolerMode"] = eeprom.coolerMode;
            if (eeprom.unit == "centigrade")
                doc["coolerThresh"] = String(eeprom.coolerThresh);
            else if (eeprom.unit == "fahrenheit")
                doc["coolerThresh"] = String((eeprom.coolerThresh * 1.8 + 32));
            doc["coolerList"] = buildScheduleOptions("cooler");
            doc["foggerMode"] = eeprom.foggerMode;
            doc["foggerThresh"] = String(eeprom.foggerThresh);
            doc["foggerList"] = buildScheduleOptions("fogger");
            doc["exfanMode"] = eeprom.exfanMode;
            doc["exfanThresh"] = String(eeprom.exfanThresh);
            doc["exfanList"] = buildScheduleOptions("exfan");
            doc["lightMode"] = eeprom.lightMode;
            doc["lightThresh"] = String(eeprom.lightThresh);
            doc["lightList"] = buildScheduleOptions("light");
            doc["pumpMode"] = eeprom.pumpMode;
            doc["pumpThresh"] = String(eeprom.pumpThresh);
            doc["pumpList"] = buildScheduleOptions("pump");
            doc["co2genMode"] = eeprom.co2genMode;
            doc["co2genThresh"] = String(eeprom.co2genThresh);
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
            if (eeprom.jalali) {
                Jalali jal;
                int jYear, jMonth, jDay;
                jal.convert_date(request->arg("year").toInt(), request->arg("month").toInt(), request->arg("day").toInt(), jYear, jMonth, jDay, JALALI);
                rtc.adjust(DateTime(jYear, jMonth, jDay, request->arg("hours").toInt(), request->arg("minutes").toInt(), request->arg("seconds").toInt()));
            }
            else {
                rtc.adjust(DateTime(request->arg("year").toInt(), request->arg("month").toInt(), request->arg("day").toInt(), request->arg("hours").toInt(), request->arg("minutes").toInt(), request->arg("seconds").toInt()));
            }
            if (request->arg("unit") == "centigrade" || request->arg("unit") == "fahrenheit")
                eeprom.unit = request->arg("unit");
            if (request->arg("calc") == "gregorian")
                eeprom.jalali = false;
            else if (request->arg("calc") == "jalali")
                eeprom.jalali = true;
            if (request->arg("buzzer") == "enable")
                eeprom.buzzer = true;
            else if (request->arg("buzzer") == "disable")
                eeprom.buzzer = false;
            if (request->arg("forceLogin") == "enable")
                eeprom.forceLogin = true;
            else if (request->arg("forceLogin") == "disable")
                eeprom.forceLogin = false;
            eeprom.delayTime = request->arg("delay").toInt();
            eeprom.errorTime = request->arg("error").toInt();
            eeprom.ignoreTime = request->arg("ignore").toInt();
            eeprom.tempOffset = request->arg("tempOffset").toInt();
            eeprom.rhOffset = request->arg("rhOffset").toInt();
            if (request->arg("heater") == "manual" || request->arg("heater") == "automat" || request->arg("heater") == "schedule") {
                if (eeprom.heaterMode != "manual" && request->arg("heater") == "manual") {
                    heater = false;
                    heaterTime = 0;
                }
                eeprom.heaterMode = request->arg("heater");
            }
            if (eeprom.unit == "centigrade")
                eeprom.heaterThresh = request->arg("heaterThresh").toFloat();
            else if (eeprom.unit == "fahrenheit")
                eeprom.heaterThresh = (request->arg("heaterThresh").toFloat() - 32) / 1.8;
            if (request->arg("cooler") == "manual" || request->arg("cooler") == "automat" || request->arg("cooler") == "schedule") {
                if (eeprom.coolerMode != "manual" && request->arg("cooler") == "manual") {
                    cooler = false;
                    coolerTime = 0;
                }
                eeprom.coolerMode = request->arg("cooler");
            }
            if (eeprom.unit == "centigrade")
                eeprom.coolerThresh = request->arg("coolerThresh").toFloat();
            else if (eeprom.unit == "fahrenheit")
                eeprom.coolerThresh = (request->arg("coolerThresh").toFloat() - 32) / 1.8;
            if (request->arg("fogger") == "manual" || request->arg("fogger") == "automat" || request->arg("fogger") == "schedule") {
                if (eeprom.foggerMode != "manual" && request->arg("fogger") == "manual") {
                    fogger = false;
                    foggerTime = 0;
                }
                eeprom.foggerMode = request->arg("fogger");
            }
            eeprom.foggerThresh = request->arg("foggerThresh").toFloat();
            if (request->arg("exfan") == "manual" || request->arg("exfan") == "automat" || request->arg("exfan") == "schedule") {
                if (eeprom.exfanMode != "manual" && request->arg("exfan") == "manual") {
                    exfan = false;
                    exfanTime = 0;
                }
                eeprom.exfanMode = request->arg("exfan");
            }
            eeprom.exfanThresh = request->arg("exfanThresh").toFloat();
            if (request->arg("light") == "manual" || request->arg("light") == "automat" || request->arg("light") == "schedule") {
                if (eeprom.lightMode != "manual" && request->arg("light") == "manual") {
                    light = false;
                    lightTime = 0;
                }
                eeprom.lightMode = request->arg("light");
            }
            eeprom.lightThresh = request->arg("lightThresh").toInt();
            if (request->arg("pump") == "manual" || request->arg("pump") == "automat" || request->arg("pump") == "schedule") {
                if (eeprom.pumpMode != "manual" && request->arg("pump") == "manual") {
                    pump = false;
                    pumpTime = 0;
                }
                eeprom.pumpMode = request->arg("pump");
            }
            eeprom.pumpThresh = request->arg("pumpThresh").toInt();
            if (request->arg("co2gen") == "manual" || request->arg("co2gen") == "automat" || request->arg("co2gen") == "schedule") {
                if (eeprom.co2genMode != "manual" && request->arg("co2gen") == "manual") {
                    co2gen = false;
                    co2genTime = 0;
                }
                eeprom.co2genMode = request->arg("co2gen");
            }
            eeprom.co2genThresh = request->arg("co2genThresh").toInt();
            eepromSave();
            request->send(200, "application/json", "{\"response\":\"OK\"}");
        }
        else {
            request->send(200, "application/json", "{\"response\":\"Login\"}");
        }
    });
    server.on("/setSchedule", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (isValidSession(request->arg("sessionID"))) {
            if (request->arg("begin").toInt() < request->arg("end").toInt()) {
                File file = LittleFS.open("/schedule/" + request->arg("device") + "/" + request->arg("day") + ".txt", "a");
                file.println(request->arg("begin") + "-" + request->arg("end"));
                file.close();
                request->send(200, "application/json", "{\"response\":\"OK\"}");
            }
            else {
                request->send(200, "application/json", "{\"response\":\"Invalid\"}");
            }
        }
        else {
            request->send(200, "application/json", "{\"response\":\"Login\"}");
        } 
    });
    server.on("/removeSchedule", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (isValidSession(request->arg("sessionID"))) {
            File file = LittleFS.open("/schedule/" + request->arg("device") + "/" + request->arg("day") + ".txt", "r");
            if (!file) {
                return;
            }
            String newContent = "";
            while (file.available()) {
                String line = file.readStringUntil('\n');
                line.trim();
                if (line != request->arg("value")) {
                    newContent += line + "\n";
                }
            }
            file.close();
            file = LittleFS.open("/schedule/" + request->arg("device") + "/" + request->arg("day") + ".txt", "w");
            if (!file) {
                return;
            }
            file.print(newContent);
            file.close();
            request->send(200, "application/json", "{\"response\":\"OK\"}");

        }
        else {
            request->send(200, "application/json", "{\"response\":\"Login\"}");
        } 
    });
}