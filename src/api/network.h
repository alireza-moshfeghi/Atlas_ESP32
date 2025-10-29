bool isValidIP(String ip) {
  ip.trim();
  int parts[4] = {0, 0, 0, 0};
  int partIndex = 0;
  int length = ip.length();
  if (length < 7 || length > 15) return false;
  int start = 0;
  for (int i = 0; i <= length; i++) {
    if (i == length || ip.charAt(i) == '.') {
      if (partIndex > 3) return false;
      String partStr = ip.substring(start, i);
      partStr.trim();
      if (partStr.length() == 0) return false;
      if (partStr.length() > 3) return false;
      for (int j = 0; j < partStr.length(); j++) {
        if (!isDigit(partStr.charAt(j))) return false;
      }
      int partVal = 0;
      for (int j = 0; j < partStr.length(); j++) {
        partVal = partVal * 10 + (partStr.charAt(j) - '0');
      }
      if (partVal < 0 || partVal > 255) return false;
      if (partStr.length() > 1 && partStr.charAt(0) == '0') return false;
      parts[partIndex] = partVal;
      partIndex++;
      start = i + 1;
    }
  }
  if (partIndex != 4) return false;
  return true;
}


void networkAPI(AsyncWebServer &server) {
    server.on("/getNetwork", HTTP_POST, [](AsyncWebServerRequest *request) {
        DynamicJsonDocument doc(4096);
        if (isValidSession(request->arg("sessionID"))) {
          doc["login"] = true;
          doc["forceLogin"] = eeprom.forceLogin;
          doc["apName"] = eeprom.apName;
          doc["apPass"] = eeprom.apPass;
          doc["stName"] =  eeprom.stName;
          doc["stPass"] = eeprom.stPass;
          doc["ipAddress"] = WiFi.localIP().toString();
          doc["subnetMask"] = WiFi.subnetMask().toString();
          doc["gateway"] = WiFi.gatewayIP().toString();
          doc["dns"] = WiFi.dnsIP(0).toString();
          doc["username"] = eeprom.username;
          if (eeprom.dhcpMode)
            doc["dhcpMode"] = true;
          else
            doc["dhcpMode"] = false;
          doc["errorTitle"] = errorTitle;
          doc["errorText"] = errorText;
        }
        else {
          doc["login"] = false;
          doc["forceLogin"] = eeprom.forceLogin;
          doc["dhcpMode"] = true;
          doc["errorTitle"] = "Authentication Required";
          doc["errorText"] = "You must be logged in to view this information. Please login and try again.";
        }
        String resp;
        serializeJson(doc, resp);
        request->send(200, "application/json", resp);
    });
    server.on("/setNetwork", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (isValidSession(request->arg("sessionID"))) {
          bool apChange = false;
          bool stChange = false;
          if (request->arg("apName") == "") {
            request->send(200, "application/json", "{\"response\":\"apNameNull\"}");
            return;
          }
          else {
            if (eeprom.apName != request->arg("apName")) {
              apChange = true;
            }
            eeprom.apName = request->arg("apName");
          }
          if (request->arg("apPass").length() < 8 && request->arg("apPass") != "") {
            request->send(200, "application/json", "{\"response\":\"apPassShort\"}");
            return;
          }
          else {
            if (eeprom.apPass != request->arg("apPass")) {
              apChange = true;
            }
            eeprom.apPass = request->arg("apPass");
          }
          if (request->arg("stName") != eeprom.stName) {
            stChange = true;
          }
          eeprom.stName = request->arg("stName");
          if (request->arg("stPass").length() < 8 && request->arg("stPass") != "") {
            request->send(200, "application/json", "{\"response\":\"stPassShort\"}");
            return;
          }
          else {
            if (eeprom.stPass != request->arg("stPass")) {
              stChange = true;
            }
            eeprom.stPass = request->arg("stPass");
          }
          if (request->arg("newUsername") != "" && eeprom.username != request->arg("newUsername")) {
            eeprom.username = request->arg("newUsername");
            sessions.clear();
          }
          if (request->arg("oldPassword") != "") {
            if (request->arg("oldPassword") != eeprom.password) {
              request->send(200, "application/json", "{\"response\":\"wrongPass\"}");
              return;
            }
            else if (request->arg("newPassword") == "") {
              request->send(200, "application/json", "{\"response\":\"emptyPass\"}");
              return;
            }
            else if (request->arg("newPassword").length() < 8) {
              request->send(200, "application/json", "{\"response\":\"shortPass\"}");
              return;
            }
            if (eeprom.password != request->arg("newPassword")) {
              eeprom.password = request->arg("newPassword");
              sessions.clear();
            }
          }
          if (request->arg("netMode") == "dhcp") {
            eeprom.dhcpMode = true;
          }
          else if (request->arg("netMode") == "static") {
            eeprom.dhcpMode = false;
            if (!isValidIP(request->arg("ip"))) {
              request->send(200, "application/json", "{\"response\":\"InvalidIP\"}");
              return;
            }
            else if (!isValidIP(request->arg("subnet"))) {
              request->send(200, "application/json", "{\"response\":\"InvalidIP\"}");
              return;
            }
            else if (!isValidIP(request->arg("gateway"))) {
              request->send(200, "application/json", "{\"response\":\"InvalidIP\"}");
              return;
            }
            else if (!isValidIP(request->arg("dns"))) {
              request->send(200, "application/json", "{\"response\":\"InvalidIP\"}");
              return;
            }
            eeprom.ip.fromString(request->arg("ip"));
            eeprom.subnet.fromString(request->arg("subnet"));
            eeprom.gateway.fromString(request->arg("gateway"));
            eeprom.dns.fromString(request->arg("dns"));
          }
          request->send(200, "application/json", "{\"response\":\"OK\"}");
          eepromSave();
          if (apChange) apUpdate();
          if (stChange) stUpdate();
        }
        else {
            request->send(200, "application/json", "{\"response\":\"Login\"}");
        }
    });
    server.on("/restart", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (isValidSession(request->arg("sessionID"))) {
          request->send(200, "application/json", "{\"response\":\"OK\"}");
          restart = true;
        }
        else {
          request->send(200, "application/json", "{\"response\":\"Login\"}");
        }
    });
}