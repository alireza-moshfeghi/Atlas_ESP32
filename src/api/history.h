String yearsList = "";

void historyAPI(AsyncWebServer &server) {
    server.on("/getHistory", HTTP_POST, [](AsyncWebServerRequest *request){
        if (!eeprom.jalali) {
            File root = LittleFS.open("/history");
            if (!root || !root.isDirectory())
            {
                return;
            }
            std::vector<String> years;
            File file = root.openNextFile();
            while (file)
            {
                String fname = file.name();
                if (fname.endsWith(".greg"))
                {
                    fname = fname.substring(0, fname.length() - 5);
                    int lastDash = fname.lastIndexOf('-');
                    if (lastDash != -1 && lastDash + 1 < fname.length())
                    {
                        String year = fname.substring(lastDash + 1);
                        bool exists = false;
                        for (auto &y : years)
                        {
                            if (y == year)
                            {
                                exists = true;
                                break;
                            }
                        }
                        if (!exists)
                        {
                            years.push_back(year);
                        }
                    }
                }
                file = root.openNextFile();
            }
            yearsList = "";
            for (auto &y : years)
            {
                yearsList += "<option value='" +  y + "'>" + y + "</option>";
            }
        }
        else {
            File root = LittleFS.open("/history");
            if (!root || !root.isDirectory())
            {
                return;
            }
            std::vector<String> years;
            File file = root.openNextFile();
            while (file)
            {
                String fname = file.name();
                if (fname.endsWith(".jal"))
                {
                    fname = fname.substring(0, fname.length() - 4);
                    int firstDash = fname.indexOf('-');
                    if (firstDash != -1)
                    {
                        String year = fname.substring(0, firstDash);

                        bool exists = false;
                        for (auto &y : years)
                        {
                            if (y == year)
                            {
                                exists = true;
                                break;
                            }
                        }
                        if (!exists)
                        {
                            years.push_back(year);
                        }
                    }
                }
                file = root.openNextFile();
            }
            yearsList = "";
            for (auto &y : years)
            {
                yearsList += "<option value='" + y + "'>" + y + "</option>";
            }
        }
        DynamicJsonDocument doc(4096);
        doc["login"] = isValidSession(request->arg("sessionID"));
        doc["forceLogin"] = eeprom.forceLogin;
        if (!eeprom.forceLogin || (eeprom.forceLogin && isValidSession(request->arg("sessionID")))) {
            doc["year"] = yearsList;
            if (!eeprom.jalali)
                doc["month"] = "<option value='01'>January</option>"
                                "<option value='02'>February</option>"
                                "<option value='03'>March</option>"
                                "<option value='04'>April</option>"
                                "<option value='05'>May</option>"
                                "<option value='06'>June</option>"
                                "<option value='07'>July</option>"
                                "<option value='08'>August</option>"
                                "<option value='09'>September</option>"
                                "<option value='10'>October</option>"
                                "<option value='11'>November</option>"
                                "<option value='12'>December</option>";
            else
                doc["month"] = "<option value='01'>Farvardin</option>"
                                "<option value='02'>Ordibehesht</option>"
                                "<option value='03'>Khordad</option>"
                                "<option value='04'>Tir</option>"
                                "<option value='05'>Mordad</option>"
                                "<option value='06'>Shahrivar</option>"
                                "<option value='07'>Mehr</option>"
                                "<option value='08'>Aban</option>"
                                "<option value='09'>Azar</option>"
                                "<option value='10'>Dey</option>"
                                "<option value='11'>Bahman</option>"
                                "<option value='12'>Esfand</option>";
            doc["errorTitle"] = errorTitle;
            doc["errorText"] = errorText;
        }
        String resp;
        serializeJson(doc, resp);
        request->send(200, "application/json", resp);
    });
    server.on("/showHistory", HTTP_POST, [](AsyncWebServerRequest *request){
        if (eeprom.jalali) {
            String find = request->arg("year") + "-" + request->arg("month") + "-";
            String resultString = "";
            File root = LittleFS.open("/history");
            if (!root || !root.isDirectory()) {
                return;
            }
            File file = root.openNextFile();
            while (file) {
                String fileName = file.name();
                if (fileName.endsWith(".jal") && fileName.startsWith(find)) {
                    DynamicJsonDocument doc(4096);
                    DeserializationError error = deserializeJson(doc, file);
                    if (error) {
                        file = root.openNextFile();
                        continue;
                    }
                    if (request->arg("value") == "temp") {
                        float minTemp = doc["minTemp"].as<float>();
                        String minTempTime = doc["minTempTime"].as<String>();
                        float maxTemp = doc["maxTemp"].as<float>();
                        String maxTempTime = doc["maxTempTime"].as<String>();
                        fileName = fileName.substring(0, fileName.length() - 4);
                        fileName.replace("-", "/");
                        resultString += "<tr>";
                        resultString += "<td>" + fileName + "</td>";
                        if (eeprom.unit == "centigrade")
                            resultString += "<td>" + String(minTemp) + " °C</td>";
                        else if (eeprom.unit == "fahrenheit")
                            resultString += "<td>" + String((minTemp * 1.8 + 32)) + " °F</td>";
                        resultString += "<td>" + minTempTime + "</td>";
                        if (eeprom.unit == "centigrade")
                            resultString += "<td>" + String(maxTemp) + " °C</td>";
                        else if (eeprom.unit == "fahrenheit")
                            resultString += "<td>" + String((maxTemp * 1.8 + 32)) + " °F</td>";
                        resultString += "<td>" + maxTempTime + "</td>";
                        resultString += "</tr>";
                    }
                    else if (request->arg("value") == "rh") {
                        int minRH = doc["minRH"].as<int>();
                        String minRHTime = doc["minRHTime"].as<String>();
                        int maxRH = doc["maxRH"].as<int>();
                        String maxRHTime = doc["maxRHTime"].as<String>();
                        fileName = fileName.substring(0, fileName.length() - 4);
                        fileName.replace("-", "/");
                        resultString += "<tr>";
                        resultString += "<td>" + fileName + "</td>";
                        resultString += "<td>" + String(minRH) + " %</td>";
                        resultString += "<td>" + minRHTime + "</td>";
                        resultString += "<td>" + String(maxRH) + " %</td>";
                        resultString += "<td>" + maxRHTime + "</td>";
                        resultString += "</tr>";
                    }
                    else if (request->arg("value") == "bright") {
                        int minBright = doc["minBright"].as<int>();
                        String minBrightTime = doc["minBrightTime"].as<String>();
                        int maxBright = doc["maxBright"].as<int>();
                        String maxBrightTime = doc["maxBrightTime"].as<String>();
                        fileName = fileName.substring(0, fileName.length() - 4);
                        fileName.replace("-", "/");
                        resultString += "<tr>";
                        resultString += "<td>" + fileName + "</td>";
                        resultString += "<td>" + String(minBright) + " %</td>";
                        resultString += "<td>" + minBrightTime + "</td>";
                        resultString += "<td>" + String(maxBright) + " %</td>";
                        resultString += "<td>" + maxBrightTime + "</td>";
                        resultString += "</tr>";
                    }
                    else if (request->arg("value") == "co2") {
                        int minCo2 = doc["minCo2"].as<int>();
                        String minCo2Time = doc["minCo2Time"].as<String>();
                        int maxCo2 = doc["maxCo2"].as<int>();
                        String maxCo2Time = doc["maxCo2Time"].as<String>();
                        fileName = fileName.substring(0, fileName.length() - 4);
                        fileName.replace("-", "/");
                        resultString += "<tr>";
                        resultString += "<td>" + fileName + "</td>";
                        resultString += "<td>" + String(minCo2) + " ppm</td>";
                        resultString += "<td>" + minCo2Time + "</td>";
                        resultString += "<td>" + String(maxCo2) + " ppm</td>";
                        resultString += "<td>" + maxCo2Time + "</td>";
                        resultString += "</tr>";
                    }
                    else if (request->arg("value") == "moist") {
                        int minMoist = doc["minMoist"].as<int>();
                        String minMoistTime = doc["minMoistTime"].as<String>();
                        int maxMoist = doc["maxMoist"].as<int>();
                        String maxMoistTime = doc["maxMoistTime"].as<String>();
                        fileName = fileName.substring(0, fileName.length() - 4);
                        fileName.replace("-", "/");
                        resultString += "<tr>";
                        resultString += "<td>" + fileName + "</td>";
                        resultString += "<td>" + String(minMoist) + " %</td>";
                        resultString += "<td>" + minMoistTime + "</td>";
                        resultString += "<td>" + String(maxMoist) + " %</td>";
                        resultString += "<td>" + maxMoistTime + "</td>";
                        resultString += "</tr>";
                    }
                    else if (request->arg("value") == "tvoc") {
                        int minTvoc = doc["minTvoc"].as<int>();
                        String minTvocTime = doc["minTvocTime"].as<String>();
                        int maxTvoc = doc["maxTvoc"].as<int>();
                        String maxTvocTime = doc["maxTvocTime"].as<String>();
                        fileName = fileName.substring(0, fileName.length() - 4);
                        fileName.replace("-", "/");
                        resultString += "<tr>";
                        resultString += "<td>" + fileName + "</td>";
                        resultString += "<td>" + String(minTvoc) + " ppb</td>";
                        resultString += "<td>" + minTvocTime + "</td>";
                        resultString += "<td>" + String(maxTvoc) + " ppb</td>";
                        resultString += "<td>" + maxTvocTime + "</td>";
                        resultString += "</tr>";
                    }
                    else if (request->arg("value") == "uptime") {
                        int heaterToday = doc["heaterToday"].as<int>();
                        int coolerToday = doc["coolerToday"].as<int>();
                        int foggerToday = doc["foggerToday"].as<int>();
                        int exfanToday = doc["exfanToday"].as<int>();
                        int lightToday = doc["lightToday"].as<int>();
                        int pumpToday = doc["pumpToday"].as<int>();
                        int co2genToday = doc["co2genToday"].as<int>();
                        fileName = fileName.substring(0, fileName.length() - 4);
                        fileName.replace("-", "/");
                        resultString += "<tr>";
                        resultString += "<td>" + fileName + "</td>";
                        resultString += "<td>" + secondsToTimeString(heaterToday) + "</td>";
                        resultString += "<td>" + secondsToTimeString(coolerToday) + "</td>";
                        resultString += "<td>" + secondsToTimeString(foggerToday) + "</td>";
                        resultString += "<td>" + secondsToTimeString(exfanToday) + "</td>";
                        resultString += "<td>" + secondsToTimeString(lightToday) + "</td>";
                        resultString += "<td>" + secondsToTimeString(pumpToday) + "</td>";
                        resultString += "<td>" + secondsToTimeString(co2genToday) + "</td>";
                        resultString += "</tr>";
                    }
                }
                file = root.openNextFile();
            }
            DynamicJsonDocument doc(4096);
            doc["login"] = isValidSession(request->arg("sessionID"));
            doc["table"] = resultString;
            String resp;
            serializeJson(doc, resp);
            request->send(200, "application/json", resp);
        }
        else {
            String find = "-" + request->arg("month") + "-" + request->arg("year") + ".greg";
            String resultString = "";
            File root = LittleFS.open("/history");
            if (!root || !root.isDirectory()) {
                return;
            }
            File file = root.openNextFile();
            while (file) {
                String fileName = file.name();
                if (fileName.endsWith(find)) {
                    DynamicJsonDocument doc(4096);
                    DeserializationError error = deserializeJson(doc, file);
                    if (error) {
                        file = root.openNextFile();
                        continue;
                    }
                    if (request->arg("value") == "temp") {
                        float minTemp = doc["minTemp"].as<float>();
                        String minTempTime = doc["minTempTime"].as<String>();
                        float maxTemp = doc["maxTemp"].as<float>();
                        String maxTempTime = doc["maxTempTime"].as<String>();
                        fileName = fileName.substring(0, fileName.length() - 5);
                        fileName.replace("-", "/");
                        resultString += "<tr>";
                        resultString += "<td>" + fileName + "</td>";
                        if (eeprom.unit == "centigrade")
                            resultString += "<td>" + String(minTemp) + " °C</td>";
                        else if (eeprom.unit == "fahrenheit")
                            resultString += "<td>" + String((minTemp * 1.8 + 32)) + " °F</td>";
                        resultString += "<td>" + minTempTime + "</td>";
                        if (eeprom.unit == "centigrade")
                            resultString += "<td>" + String(maxTemp) + " °C</td>";
                        else if (eeprom.unit == "fahrenheit")
                            resultString += "<td>" + String((maxTemp * 1.8 + 32)) + " °F</td>";
                        resultString += "<td>" + maxTempTime + "</td>";
                        resultString += "</tr>";
                    }
                    else if (request->arg("value") == "rh") {
                        int minRH = doc["minRH"].as<int>();
                        String minRHTime = doc["minRHTime"].as<String>();
                        int maxRH = doc["maxRH"].as<int>();
                        String maxRHTime = doc["maxRHTime"].as<String>();
                        fileName = fileName.substring(0, fileName.length() - 5);
                        fileName.replace("-", "/");
                        resultString += "<tr>";
                        resultString += "<td>" + fileName + "</td>";
                        resultString += "<td>" + String(minRH) + " %</td>";
                        resultString += "<td>" + minRHTime + "</td>";
                        resultString += "<td>" + String(maxRH) + " %</td>";
                        resultString += "<td>" + maxRHTime + "</td>";
                        resultString += "</tr>";
                    }
                    else if (request->arg("value") == "bright") {
                        int minBright = doc["minBright"].as<int>();
                        String minBrightTime = doc["minBrightTime"].as<String>();
                        int maxBright = doc["maxBright"].as<int>();
                        String maxBrightTime = doc["maxBrightTime"].as<String>();
                        fileName = fileName.substring(0, fileName.length() - 5);
                        fileName.replace("-", "/");
                        resultString += "<tr>";
                        resultString += "<td>" + fileName + "</td>";
                        resultString += "<td>" + String(minBright) + " %</td>";
                        resultString += "<td>" + minBrightTime + "</td>";
                        resultString += "<td>" + String(maxBright) + " %</td>";
                        resultString += "<td>" + maxBrightTime + "</td>";
                        resultString += "</tr>";
                    }
                    else if (request->arg("value") == "co2") {
                        int minCo2 = doc["minCo2"].as<int>();
                        String minCo2Time = doc["minCo2Time"].as<String>();
                        int maxCo2 = doc["maxCo2"].as<int>();
                        String maxCo2Time = doc["maxCo2Time"].as<String>();
                        fileName = fileName.substring(0, fileName.length() - 5);
                        fileName.replace("-", "/");
                        resultString += "<tr>";
                        resultString += "<td>" + fileName + "</td>";
                        resultString += "<td>" + String(minCo2) + " ppm</td>";
                        resultString += "<td>" + minCo2Time + "</td>";
                        resultString += "<td>" + String(maxCo2) + " ppm</td>";
                        resultString += "<td>" + maxCo2Time + "</td>";
                        resultString += "</tr>";
                    }
                    else if (request->arg("value") == "moist") {
                        int minMoist = doc["minMoist"].as<int>();
                        String minMoistTime = doc["minMoistTime"].as<String>();
                        int maxMoist = doc["maxMoist"].as<int>();
                        String maxMoistTime = doc["maxMoistTime"].as<String>();
                        fileName = fileName.substring(0, fileName.length() - 5);
                        fileName.replace("-", "/");
                        resultString += "<tr>";
                        resultString += "<td>" + fileName + "</td>";
                        resultString += "<td>" + String(minMoist) + " %</td>";
                        resultString += "<td>" + minMoistTime + "</td>";
                        resultString += "<td>" + String(maxMoist) + " %</td>";
                        resultString += "<td>" + maxMoistTime + "</td>";
                        resultString += "</tr>";
                    }
                    else if (request->arg("value") == "tvoc") {
                        int minTvoc = doc["minTvoc"].as<int>();
                        String minTvocTime = doc["minTvocTime"].as<String>();
                        int maxTvoc = doc["maxTvoc"].as<int>();
                        String maxTvocTime = doc["maxTvocTime"].as<String>();
                        fileName = fileName.substring(0, fileName.length() - 5);
                        fileName.replace("-", "/");
                        resultString += "<tr>";
                        resultString += "<td>" + fileName + "</td>";
                        resultString += "<td>" + String(minTvoc) + " ppb</td>";
                        resultString += "<td>" + minTvocTime + "</td>";
                        resultString += "<td>" + String(maxTvoc) + " ppb</td>";
                        resultString += "<td>" + maxTvocTime + "</td>";
                        resultString += "</tr>";
                    }
                    else if (request->arg("value") == "uptime") {
                        int heaterToday = doc["heaterToday"].as<int>();
                        int coolerToday = doc["coolerToday"].as<int>();
                        int foggerToday = doc["foggerToday"].as<int>();
                        int exfanToday = doc["exfanToday"].as<int>();
                        int lightToday = doc["lightToday"].as<int>();
                        int pumpToday = doc["pumpToday"].as<int>();
                        int co2genToday = doc["co2genToday"].as<int>();
                        fileName = fileName.substring(0, fileName.length() - 5);
                        fileName.replace("-", "/");
                        resultString += "<tr>";
                        resultString += "<td>" + fileName + "</td>";
                        resultString += "<td>" + secondsToTimeString(heaterToday) + "</td>";
                        resultString += "<td>" + secondsToTimeString(coolerToday) + "</td>";
                        resultString += "<td>" + secondsToTimeString(foggerToday) + "</td>";
                        resultString += "<td>" + secondsToTimeString(exfanToday) + "</td>";
                        resultString += "<td>" + secondsToTimeString(lightToday) + "</td>";
                        resultString += "<td>" + secondsToTimeString(pumpToday) + "</td>";
                        resultString += "<td>" + secondsToTimeString(co2genToday) + "</td>";
                        resultString += "</tr>";
                    }
                }
                file = root.openNextFile();
            }
            DynamicJsonDocument doc(4096);
            doc["login"] = isValidSession(request->arg("sessionID"));
            doc["table"] = resultString;
            String resp;
            serializeJson(doc, resp);
            request->send(200, "application/json", resp);
        }
    });
    server.on("/deleteAll", HTTP_POST, [](AsyncWebServerRequest *request){
        if (isValidSession(request->arg("sessionID"))) {
            Jalali jal;
            int jYear, jMonth, jDay;
            jal.convert_date(atoi(cuYear), atoi(cuMonth), atoi(cuDay), jYear, jMonth, jDay, GREGORIAN);
            char dateStr[11];
            sprintf(dateStr, "%04d-%02d-%02d", jYear, jMonth, jDay);
            File dir = LittleFS.open("/history");
            File file = dir.openNextFile();
            while (file) {
                String fileName = file.name();
                if (fileName.endsWith(".greg") || fileName.endsWith(".jal")) {
                    if (fileName != String(cuDay) + "-" + String(cuMonth) + "-" + String(cuYear) + ".greg" && fileName != String(dateStr) + ".jal") {
                        file.close();
                        LittleFS.remove("/history/" + fileName);
                    }
                }
                file = dir.openNextFile();
            }
            request->send(200, "application/json", "{\"response\":\"OK\"}");
        }
        else {
            request->send(200, "application/json", "{\"response\":\"Login\"}");
        }
    });
}