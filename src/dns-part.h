#include <DNSServer.h>
#define DNSPORT 53
DNSServer dnsServer;

void dnsSetup() {
    dnsServer.start(DNSPORT, "*", WiFi.softAPIP());
}

void dnsLoop() {
    dnsServer.processNextRequest();
}