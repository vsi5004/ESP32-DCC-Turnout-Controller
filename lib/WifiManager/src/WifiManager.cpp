#include "WifiManager.h"
#include <Elog.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <WSEventHandler.h>
#include <AppSettings.h>
#define DNS_PORT 53

const IPAddress apIP(192, 168, 2, 1);
const IPAddress gateway(255, 255, 255, 0);
const char *defaultName = "DCC Turnout Controller";

Elog loggerWM;
DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebSocket websocket("/ws");
AppSettings appSettings(defaultName, defaultName, false, true);

WifiManager::WifiManager()
{
}

void WifiManager::init()
{
    loggerWM.addSerialLogging(Serial, "WFManager", DEBUG);
    appSettings.init();
    initWSEventHandler();
    initWifi();
    configWifi();
    startWifi();
    startWebsocketServer();
}

void WifiManager::processNextRequest()
{
    dnsServer.processNextRequest();
}

void redirectToIndex(AsyncWebServerRequest *request)
{
    request->redirect("http://" + apIP.toString());
}

void WifiManager::configWifi()
{
    if (!WiFi.softAPConfig(apIP, apIP, gateway))
    {
        loggerWM.log(CRITICAL, "AP Config Failed");
        while (true)
        {
            delay(1000);
        }
    }
}

void WifiManager::initWifi()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_AP);
    if (!WiFi.softAP(appSettings.wifiSSID))
    {
        loggerWM.log(CRITICAL, "AP Start Failed");
        while (true)
        {
            delay(1000);
        }
    }
}

void WifiManager::startWebsocketServer()
{
    websocket.onEvent(WSEventHandler);
    server.addHandler(&websocket);
    server.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");
    server.onNotFound(redirectToIndex);
    server.begin();
    loggerWM.log(INFO, "Web Server Started");
}

void WifiManager::startWifi()
{
    dnsServer.start(DNS_PORT, "*", apIP);
    loggerWM.log(INFO, "WiFi AP is now running at IP address: %s", WiFi.softAPIP().toString());
}
