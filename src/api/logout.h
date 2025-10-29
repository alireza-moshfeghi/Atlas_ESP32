void removeSession(String sessionID){
  for(auto it = sessions.begin(); it != sessions.end(); ++it){
    if(*it == sessionID){
      sessions.erase(it);
      break;
    }
  }
}

void logoutAPI(AsyncWebServer &server) {
    server.on("/logout", HTTP_POST, [](AsyncWebServerRequest *request){
        if (isValidSession(request->arg("sessionID"))) {
            removeSession(request->arg("sessionID"));
            String response = "{\"response\":\"OK\",\"forceLogin\":";
            response += eeprom.forceLogin ? "true" : "false";
            response += "}";
            request->send(200, "application/json", response);
        }
        else {
            String response = "{\"response\":\"Invalid\",\"forceLogin\":";
            response += eeprom.forceLogin ? "true" : "false";
            response += "}";
        }
    });
}