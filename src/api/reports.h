String labelsJson = "[]";
String minTempJson = "[]";
String maxTempJson = "[]";
String minRHJson = "[]";
String maxRHJson = "[]";
String minBrightJson = "[]";
String maxBrightJson = "[]";
String minCo2Json = "[]";
String maxCo2Json = "[]";
String minMoistJson = "[]";
String maxMoistJson = "[]";
String minTvocJson = "[]";
String maxTvocJson = "[]";

void reportsAPI(AsyncWebServer &server)
{
    server.on("/getReports", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(4096);
        doc["login"] = isValidSession(request->arg("sessionID"));
        doc["forceLogin"] = eeprom.forceLogin;
        if (!eeprom.forceLogin || (eeprom.forceLogin && isValidSession(request->arg("sessionID")))) {
            if (eeprom.unit == "centigrade")
                doc["minTemp"] = String(minTemp);
            else if (eeprom.unit == "fahrenheit")
                doc["minTemp"] = String((minTemp * 9.0 / 5.0 + 32));
            doc["minTempTime"] = minTempTime;
            if (eeprom.unit == "centigrade")
                doc["maxTemp"] = String(maxTemp);
            else if (eeprom.unit == "fahrenheit")
                doc["maxTemp"] = String((maxTemp * 9.0 / 5.0 + 32));
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
            doc["unit"] = eeprom.unit;
            if (!eeprom.jalali) {
                File root = LittleFS.open("/history");
                if (!root || !root.isDirectory())
                {
                    String resp;
                    serializeJson(doc, resp);
                    request->send(200, "application/json", resp);
                    return;
                }
                File file = root.openNextFile();
                std::vector<String> allFiles;
                while (file)
                {
                    String fname = file.name();
                    if (fname.endsWith(".greg"))
                    {
                        allFiles.push_back("/history/" + fname);
                    }
                    file = root.openNextFile();
                }
                int totalCount = allFiles.size();
                if (totalCount == 0)
                {
                    String resp;
                    serializeJson(doc, resp);
                    request->send(200, "application/json", resp);
                    return;
                }
                int startIndex = (totalCount > 7) ? totalCount - 7 : 0;
                labelsJson = "[";
                minTempJson = "[";
                maxTempJson = "[";
                minRHJson = "[";
                maxRHJson = "[";
                minBrightJson = "[";
                maxBrightJson = "[";
                minCo2Json = "[";
                maxCo2Json = "[";
                minMoistJson = "[";
                maxMoistJson = "[";
                minTvocJson = "[";
                maxTvocJson = "[";
                for (int i = startIndex; i < totalCount; i++)
                {
                    String fullPath = allFiles[i];
                    String name = fullPath;
                    if (name.startsWith("/history/"))
                        name = name.substring(9);
                    if (name.endsWith(".greg"))
                        name = name.substring(0, name.length() - 5);
                    name.replace("-", "/");
                    File file = LittleFS.open(fullPath, "r");
                    if (file)
                    {
                        DynamicJsonDocument doc2(4096);
                        deserializeJson(doc2, file.readString());
                        file.close();
                        int minT = doc2["minTemp"].as<float>();
                        int maxT = doc2["maxTemp"].as<float>();
                        int minR = doc2["minRH"].as<float>();
                        int maxR = doc2["maxRH"].as<float>();
                        int minB = doc2["minBright"].as<int>();
                        int maxB = doc2["maxBright"].as<int>();
                        int minC = doc2["minCo2"].as<int>();
                        int maxC = doc2["maxCo2"].as<int>();
                        int minM = doc2["minMoist"].as<int>();
                        int maxM = doc2["maxMoist"].as<int>();
                        int minV = doc2["minTvoc"].as<int>();
                        int maxV = doc2["maxTvoc"].as<int>();
                        labelsJson += "\"" + name + "\"";
                        if (eeprom.unit == "centigrade") {
                            minTempJson += "\"" + String(minT) + "\"";
                            maxTempJson += "\"" + String(maxT) + "\"";
                        }
                        else if (eeprom.unit == "fahrenheit") {
                            minTempJson += "\"" + String((minT * 1.8 + 32)) + "\"";
                            maxTempJson += "\"" + String((maxT * 1.8 + 32)) + "\"";
                        }
                        minRHJson += "\"" + String(minR) + "\"";
                        maxRHJson += "\"" + String(maxR) + "\"";
                        minBrightJson += "\"" + String(minB) + "\"";
                        maxBrightJson += "\"" + String(maxB) + "\"";
                        minCo2Json += "\"" + String(minC) + "\"";
                        maxCo2Json += "\"" + String(maxC) + "\"";
                        minMoistJson += "\"" + String(minM) + "\"";
                        maxMoistJson += "\"" + String(maxM) + "\"";
                        minTvocJson += "\"" + String(minV) + "\"";
                        maxTvocJson += "\"" + String(maxV) + "\"";
                        if (i < totalCount - 1)
                        {
                            labelsJson += ",";
                            minTempJson += ",";
                            maxTempJson += ",";
                            minRHJson += ",";
                            maxRHJson += ",";
                            minBrightJson += ",";
                            maxBrightJson += ",";
                            minCo2Json += ",";
                            maxCo2Json += ",";
                            minMoistJson += ",";
                            maxMoistJson += ",";
                            minTvocJson += ",";
                            maxTvocJson += ",";
                        }
                    }
                }
                labelsJson += "]";
                minTempJson += "]";
                maxTempJson += "]";
                minRHJson += "]";
                maxRHJson += "]";
                minBrightJson += "]";
                maxBrightJson += "]";
                minCo2Json += "]";
                maxCo2Json += "]";
                minMoistJson += "]";
                maxMoistJson += "]";
                minTvocJson += "]";
                maxTvocJson += "]";
                doc["labels"] = labelsJson;
                doc["minTempChart"] = minTempJson;
                doc["maxTempChart"] = maxTempJson;
                doc["minRHChart"] = minRHJson;
                doc["maxRHChart"] = maxRHJson;
                doc["minBrightChart"] = minBrightJson;
                doc["maxBrightChart"] = maxBrightJson;
                doc["minCo2Chart"] = minCo2Json;
                doc["maxCo2Chart"] = maxCo2Json;
                doc["minMoistChart"] = minMoistJson;
                doc["maxMoistChart"] = maxMoistJson;
                doc["minTvocChart"] = minTvocJson;
                doc["maxTvocChart"] = maxTvocJson;
            }
            else {
                File root = LittleFS.open("/history");
                if (!root || !root.isDirectory())
                {
                    String resp;
                    serializeJson(doc, resp);
                    request->send(200, "application/json", resp);
                    return;
                }
                File file = root.openNextFile();
                std::vector<String> allFiles;
                while (file)
                {
                    String fname = file.name();
                    if (fname.endsWith(".jal"))
                    {
                        allFiles.push_back("/history/" + fname);
                    }
                    file = root.openNextFile();
                }
                int totalCount = allFiles.size();
                if (totalCount == 0)
                {
                    String resp;
                    serializeJson(doc, resp);
                    request->send(200, "application/json", resp);
                    return;
                }
                int startIndex = (totalCount > 7) ? totalCount - 7 : 0;
                labelsJson = "[";
                minTempJson = "[";
                maxTempJson = "[";
                minRHJson = "[";
                maxRHJson = "[";
                minBrightJson = "[";
                maxBrightJson = "[";
                minCo2Json = "[";
                maxCo2Json = "[";
                minMoistJson = "[";
                maxMoistJson = "[";
                minTvocJson = "[";
                maxTvocJson = "[";
                for (int i = startIndex; i < totalCount; i++)
                {
                    String fullPath = allFiles[i];
                    String name = fullPath;
                    if (name.startsWith("/history/"))
                        name = name.substring(9);
                    if (name.endsWith(".jal"))
                        name = name.substring(0, name.length() - 4);
                    name.replace("-", "/");
                    File file = LittleFS.open(fullPath, "r");
                    if (file)
                    {
                        DynamicJsonDocument doc2(4096);
                        deserializeJson(doc2, file.readString());
                        file.close();
                        int minT = doc2["minTemp"].as<float>();
                        int maxT = doc2["maxTemp"].as<float>();
                        int minR = doc2["minRH"].as<float>();
                        int maxR = doc2["maxRH"].as<float>();
                        int minB = doc2["minBright"].as<int>();
                        int maxB = doc2["maxBright"].as<int>();
                        int minC = doc2["minCo2"].as<int>();
                        int maxC = doc2["maxCo2"].as<int>();
                        int minM = doc2["minMoist"].as<int>();
                        int maxM = doc2["maxMoist"].as<int>();
                        int minV = doc2["minTvoc"].as<int>();
                        int maxV = doc2["maxTvoc"].as<int>();
                        labelsJson += "\"" + name + "\"";
                        if (eeprom.unit == "centigrade") {
                            minTempJson += "\"" + String(minT) + "\"";
                            maxTempJson += "\"" + String(maxT) + "\"";
                        }
                        else if (eeprom.unit == "fahrenheit") {
                            minTempJson += "\"" + String((minT * 1.8 + 32)) + "\"";
                            maxTempJson += "\"" + String((maxT * 1.8 + 32)) + "\"";
                        }
                        minRHJson += "\"" + String(minR) + "\"";
                        maxRHJson += "\"" + String(maxR) + "\"";
                        minBrightJson += "\"" + String(minB) + "\"";
                        maxBrightJson += "\"" + String(maxB) + "\"";
                        minCo2Json += "\"" + String(minC) + "\"";
                        maxCo2Json += "\"" + String(maxC) + "\"";
                        minMoistJson += "\"" + String(minM) + "\"";
                        maxMoistJson += "\"" + String(maxM) + "\"";
                        minTvocJson += "\"" + String(minV) + "\"";
                        maxTvocJson += "\"" + String(maxV) + "\"";
                        if (i < totalCount - 1)
                        {
                            labelsJson += ",";
                            minTempJson += ",";
                            maxTempJson += ",";
                            minRHJson += ",";
                            maxRHJson += ",";
                            minBrightJson += ",";
                            maxBrightJson += ",";
                            minCo2Json += ",";
                            maxCo2Json += ",";
                            minMoistJson += ",";
                            maxMoistJson += ",";
                            minTvocJson += ",";
                            maxTvocJson += ",";
                        }
                    }
                }
                labelsJson += "]";
                minTempJson += "]";
                maxTempJson += "]";
                minRHJson += "]";
                maxRHJson += "]";
                minBrightJson += "]";
                maxBrightJson += "]";
                minCo2Json += "]";
                maxCo2Json += "]";
                minMoistJson += "]";
                maxMoistJson += "]";
                minTvocJson += "]";
                maxTvocJson += "]";
                doc["labels"] = labelsJson;
                doc["minTempChart"] = minTempJson;
                doc["maxTempChart"] = maxTempJson;
                doc["minRHChart"] = minRHJson;
                doc["maxRHChart"] = maxRHJson;
                doc["minBrightChart"] = minBrightJson;
                doc["maxBrightChart"] = maxBrightJson;
                doc["minCo2Chart"] = minCo2Json;
                doc["maxCo2Chart"] = maxCo2Json;
                doc["minMoistChart"] = minMoistJson;
                doc["maxMoistChart"] = maxMoistJson;
                doc["minTvocChart"] = minTvocJson;
                doc["maxTvocChart"] = maxTvocJson;
            }
            doc["errorTitle"] = errorTitle;
            doc["errorText"] = errorText;
        }
        String resp;
        serializeJson(doc, resp);
        request->send(200, "application/json", resp); });
}