#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "wsEventHandler.h"
#include "Turnout.h"
#include "TurnoutManager.h"

#define SSID "ESP32 DCC Turnout Controller"
#define DNS_PORT 53

const IPAddress apIP(192, 168, 2, 1);
const IPAddress gateway(255, 255, 255, 0);

DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebSocket websocket("/ws");

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
  pinMode(2, OUTPUT);
  Serial.begin(115200);

  initWifi();
  configWifi();
  startWifi();
  initFileSystem();
  turnoutManager.loadTurnouts();
  startWebsocketServer();
}

void startWebsocketServer()
{
  websocket.onEvent(wsEventHandler);
  server.addHandler(&websocket);
  server.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");
  server.onNotFound(redirectToIndex);
  server.begin();
  Serial.println("Server Started");
}

void startWifi()
{
  dnsServer.start(DNS_PORT, "*", apIP);
  Serial.println("\nWiFi AP is now running\nIP address: ");
  Serial.println(WiFi.softAPIP());
}

void initFileSystem()
{
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
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
    Serial.println("AP Config Failed");
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
    Serial.println("AP Start Failed");
    while (true)
    {
      delay(1000);
    }
  }
}

void loop()
{
  dnsServer.processNextRequest();
  vTaskDelay(1);
}
