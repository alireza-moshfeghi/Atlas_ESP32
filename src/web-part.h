#include <vector>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "api/login.h"
#include "api/logout.h"
#include "api/status.h"
#include "api/devices.h"
#include "api/reports.h"
#include "api/history.h"
#include "api/settings.h"
#include "api/network.h"
#define WEBPORT 80
AsyncWebServer server(WEBPORT);
char cuJYear[5] = "0000";
char cuJMonth[3] = "00";
char cuJDay[3] = "00";

void addFileRoute(const char* uri, const char* path, const char* contentType) { 
    server.on(uri, HTTP_GET, [path, contentType](AsyncWebServerRequest *request) { 
        request->send(LittleFS, path, contentType); 
    }); 
}

void webSetup() {
    
    // API
    statusAPI(server);
    devicesAPI(server);
    loginAPI(server);
    logoutAPI(server);
    reportsAPI(server);
    historyAPI(server);
    settingsAPI(server);
    networkAPI(server);
    server.begin();

    // Captive Portal
    server.onNotFound([](AsyncWebServerRequest *request){
        request->redirect("/status");
    });

    // HTML
    addFileRoute("/status", "/status.html", "text/html");
    addFileRoute("/devices", "/devices.html", "text/html");
    addFileRoute("/reports", "/reports.html", "text/html");
    addFileRoute("/history", "/history.html", "text/html");
    addFileRoute("/settings", "/settings.html", "text/html");
    addFileRoute("/network", "/network.html", "text/html");
    addFileRoute("/about", "/about.html", "text/html");
    addFileRoute("/login", "/login.html", "text/html");
    
    // CSS
    addFileRoute("/assets/css/bootstrap.min.css", "/assets/css/bootstrap.min.css", "text/css");
    addFileRoute("/assets/css/style.css", "/assets/css/style.css", "text/css");

    // JS
    addFileRoute("/assets/js/jquery-3.7.1.min.js", "/assets/js/jquery-3.7.1.min.js", "application/javascript");
    addFileRoute("/assets/js/bootstrap.min.js", "/assets/js/bootstrap.min.js", "application/javascript");
    addFileRoute("/assets/js/script.js", "/assets/js/script.js", "application/javascript");
    addFileRoute("/assets/js/chart.js", "/assets/js/chart.js", "application/javascript");
    
    // ICON
    addFileRoute("/assets/icon/flower.png", "/assets/icon/flower.png", "image/png");

}
