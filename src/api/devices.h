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
    if (!ts2 == 0) {
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
    server.on("/getDevices", HTTP_POST, [](AsyncWebServerRequest *request){
        DynamicJsonDocument doc(4096);
        doc["login"] = isValidSession(request->arg("sessionID"));
        doc["forceLogin"] = eeprom.forceLogin;
        if (!eeprom.forceLogin || (eeprom.forceLogin && isValidSession(request->arg("sessionID")))) {
            doc["heater"] = heater;
            doc["heaterMode"] = capitalizeFirst(eeprom.heaterMode);
            doc["heaterTime"] = diffTimeStamps(timeStamp, heaterTime);
            doc["heaterToday"] = secondsToTimeString(heaterToday);
            doc["cooler"] = cooler;
            doc["coolerMode"] = capitalizeFirst(eeprom.coolerMode);
            doc["coolerTime"] = diffTimeStamps(timeStamp, coolerTime);
            doc["coolerToday"] = secondsToTimeString(coolerToday);
            doc["fogger"] = fogger;
            doc["foggerMode"] = capitalizeFirst(eeprom.foggerMode);
            doc["foggerTime"] = diffTimeStamps(timeStamp, foggerTime);
            doc["foggerToday"] = secondsToTimeString(foggerToday);
            doc["exfan"] = exfan;
            doc["exfanMode"] = capitalizeFirst(eeprom.exfanMode);
            doc["exfanTime"] = diffTimeStamps(timeStamp, exfanTime);
            doc["exfanToday"] = secondsToTimeString(exfanToday);
            doc["light"] = light;
            doc["lightMode"] = capitalizeFirst(eeprom.lightMode);
            doc["lightTime"] = diffTimeStamps(timeStamp, lightTime);
            doc["lightToday"] = secondsToTimeString(lightToday);
            doc["pump"] = pump;
            doc["pumpMode"] = capitalizeFirst(eeprom.pumpMode);
            doc["pumpTime"] = diffTimeStamps(timeStamp, pumpTime);
            doc["pumpToday"] = secondsToTimeString(pumpToday);
            doc["co2gen"] = co2gen;
            doc["co2genMode"] = capitalizeFirst(eeprom.co2genMode);
            doc["co2genTime"] = diffTimeStamps(timeStamp, co2genTime);
            doc["co2genToday"] = secondsToTimeString(co2genToday);
            doc["errorTitle"] = errorTitle;
            doc["errorText"] = errorText;
        }
        String resp;
        serializeJson(doc, resp);
        request->send(200, "application/json", resp);
    });
    server.on("/setDevices", HTTP_POST, [](AsyncWebServerRequest *request){
        if (isValidSession(request->arg("sessionID"))) {
            handleDevice(request, "heater", heater, eeprom.heaterMode, heaterTime, timeStamp);
            handleDevice(request, "cooler", cooler, eeprom.coolerMode, coolerTime, timeStamp);
            handleDevice(request, "fogger", fogger, eeprom.foggerMode, foggerTime, timeStamp);
            handleDevice(request, "exfan", exfan, eeprom.exfanMode, exfanTime, timeStamp);
            handleDevice(request, "light", light, eeprom.lightMode, lightTime, timeStamp);
            handleDevice(request, "pump", pump, eeprom.pumpMode, pumpTime, timeStamp);
            handleDevice(request, "co2gen", co2gen, eeprom.co2genMode, co2genTime, timeStamp);
        }
        else {
            request->send(200, "application/json", "{\"response\":\"Login\"}");
        }
    });
}