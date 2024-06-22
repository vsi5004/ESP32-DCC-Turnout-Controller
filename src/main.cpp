#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "wsEventHandler.h"

#define SSID "ESP32 DCC Turnout Controller" // This is the SSID that ESP32 will broadcast
#define DNS_PORT 53

const IPAddress apIP(192, 168, 2, 1);
const IPAddress gateway(255, 255, 255, 0);

DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebSocket websocket("/ws");

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
  startWebsocketServer();
}

void startWebsocketServer()
{
  // bind websocket to async web server
  websocket.onEvent(wsEventHandler);
  server.addHandler(&websocket);
  // setup static web server
  server.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");
  // Captive portal to keep the client
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
      delay(1000); // Stop execution in case of FS error
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
      delay(1000); // Stop execution if AP config fails
    }
  }
}

void initWifi()
{
  WiFi.disconnect();   // added to start with the wifi off, avoid crashing
  WiFi.mode(WIFI_OFF); // added to start with the wifi off, avoid crashing
  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(SSID))
  {
    Serial.println("AP Start Failed");
    while (true)
    {
      delay(1000); // Stop execution if AP start fails
    }
  }
}

void loop()
{
  // serve DNS request for captive portal
  dnsServer.processNextRequest();
  vTaskDelay(1);
}