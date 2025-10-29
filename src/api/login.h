std::vector<String> sessions;

String generateSession(int length = 32) {
  String s = "";
  for (int i = 0; i < length; i++) 
    s += String(random(0, 16), HEX);
  return s;
}

bool isValidSession(String sessionID){
  for(auto &s : sessions) {
    if(s == sessionID) return true;
  }
  return false;
}

void loginAPI(AsyncWebServer &server) {
    server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request){
        String username = request->arg("username");
        String password = request->arg("password");
        if(username == eeprom.username && password == eeprom.password){
            String sessionID = generateSession();
            sessions.push_back(sessionID);
            request->send(200, "application/json", "{\"response\":\"OK\", \"sessionID\":\"" + sessionID + "\"}");
        } 
        else {
            request->send(200, "application/json", "{\"response\":\"Invalid\"}");
        }
    });
}