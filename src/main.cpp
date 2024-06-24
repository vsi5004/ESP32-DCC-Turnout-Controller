#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WSEventHandler.h>
#include <Turnout.h>
#include <TurnoutManager.h>
#include <Elog.h>

#define SSID "ESP32 DCC Turnout Controller"
#define DNS_PORT 53

const IPAddress apIP(192, 168, 2, 1);
const IPAddress gateway(255, 255, 255, 0);

DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebSocket websocket("/ws");
Elog logger;
TurnoutManager turnoutManager;

void startWifi();
void initFileSystem();
void configWifi();
void initWifi();
void startWebsocketServer();

void redirectToIndex(AsyncWebServerRequest *request)
{
  request->redirect("http://" + apIP.toString());
}

void setup()
{
  Serial.begin(115200);
  logger.addSerialLogging(Serial, "Main", DEBUG);

  turnoutManager.init();
  initWSEventHandler();
  initFileSystem();
  turnoutManager.loadTurnouts();
  turnoutManager.initTurnouts();
  initWifi();
  configWifi();
  startWifi();
  startWebsocketServer();
}

void startWebsocketServer()
{
  websocket.onEvent(WSEventHandler);
  server.addHandler(&websocket);
  server.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");
  server.onNotFound(redirectToIndex);
  server.begin();
  logger.log(INFO, "Web Server Started");
}

void startWifi()
{
  dnsServer.start(DNS_PORT, "*", apIP);
  logger.log(INFO, "WiFi AP is now running at IP address: %s", WiFi.softAPIP().toString());
}

void initFileSystem()
{
  if (!LittleFS.begin())
  {
    logger.log(CRITICAL, "An Error has occurred while mounting LittleFS");
    while (true)
    {
      delay(1000);
    }
  }
}

void configWifi()
{
  if (!WiFi.softAPConfig(apIP, apIP, gateway))
  {
    logger.log(CRITICAL, "AP Config Failed");
    while (true)
    {
      delay(1000);
    }
  }
}

void initWifi()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(SSID))
  {
    logger.log(CRITICAL, "AP Start Failed");
    while (true)
    {
      delay(1000);
    }
  }
}

void loop()
{
  dnsServer.processNextRequest();
  turnoutManager.updateTurnoutPositions();
  vTaskDelay(1);
}
