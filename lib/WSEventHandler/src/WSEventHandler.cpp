#include <Arduino.h>
#include <ArduinoJson.h>
#include <Elog.h>
#include "WSEventHandler.h"
#include "Turnout.h"
#include "TurnoutManager.h"
#include "AppSettings.h"

// Allocate memory for received json data, assuming list of 12 turnouts and their settings is around
// 2048 bytes long with some extra for future growth
#define BUFFER_SIZE 2176
StaticJsonDocument<BUFFER_SIZE> receivedJson;
char dataBuffer[BUFFER_SIZE] = "{}";
AsyncWebSocketClient *clients[10] = {nullptr};

// External references to TurnoutManager and AppSettings instances in main.cpp
extern TurnoutManager turnoutManager;
extern AppSettings appSettings;

Elog loggerWSE;

void initWSEventHandler()
{
  loggerWSE.addSerialLogging(Serial, "WSEventHandler", DEBUG);
}

void WSEventHandler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_DATA)
  {
    if (len >= BUFFER_SIZE)
    {
      loggerWSE.log(ERROR, "Data too large for buffer");
      return;
    }

    // save the response as newest device state
    strncpy(dataBuffer, (char *)data, len);
    dataBuffer[len] = '\0';
    loggerWSE.log(DEBUG, "Received data: %s", dataBuffer);

    // parse the received json data
    DeserializationError error = deserializeJson(receivedJson, dataBuffer);
    if (error)
    {
      loggerWSE.log(ERROR, "deserializeJson() failed: %s", error.f_str());
      return;
    }

    const char *msgType = receivedJson["type"];
    if (strcmp(msgType, TurnoutManager::TYPE_GET_TURNOUTS) == 0)
    {
      String jsonString = turnoutManager.turnoutsToJson();
      jsonString.toCharArray(dataBuffer, BUFFER_SIZE);
      client->text(dataBuffer);
    }
    else if (strcmp(msgType, AppSettings::TYPE_APP_SETTINGS) == 0)
    {
      if (receivedJson.containsKey("settings"))
      {
        JsonObject settings = receivedJson["settings"].as<JsonObject>();
        AppSettings newSettings = AppSettings::fromJson(settings);
        bool rebootRequired = appSettings.wifiSSID != newSettings.wifiSSID;
        appSettings = newSettings;
        appSettings.saveToFile();

        String jsonString = appSettings.toNestedJson();
        jsonString.toCharArray(dataBuffer, BUFFER_SIZE);
        // Send updated settings to all clients
        for (AsyncWebSocketClient *c : clients)
        {
          if (c != nullptr)
          {
            c->text(dataBuffer);
          }
        }
        if (rebootRequired)
        {
          loggerWSE.log(INFO, "Rebooting to apply new wifi settings");
          delay(100);
          ESP.restart();
        }
      }
    }
    else if (strcmp(msgType, AppSettings::TYPE_GET_APP_SETTINGS) == 0)
    {
      String jsonString = appSettings.toNestedJson();
      jsonString.toCharArray(dataBuffer, BUFFER_SIZE);
      client->text(dataBuffer);
    }
    else if (strcmp(msgType, TurnoutManager::TYPE_TURNOUT_SETTINGS) == 0)
    {
      if (receivedJson.containsKey("settings"))
      {
        JsonObject settings = receivedJson["settings"].as<JsonObject>();
        Turnout newTurnout = Turnout::fromJson(settings);
        turnoutManager.updateTurnout(newTurnout);

        // Send updated turnouts list to all clients
        String jsonString = turnoutManager.turnoutsToJson();
        jsonString.toCharArray(dataBuffer, BUFFER_SIZE);
        for (AsyncWebSocketClient *c : clients)
          if (c != nullptr)
            c->text(dataBuffer);
      }
    }
    else if (strcmp(msgType, TurnoutManager::TYPE_TURNOUT_TEST) == 0)
    {
      if (receivedJson.containsKey("settings"))
      {
        int targetPosition = receivedJson["targetPosition"];
        JsonObject settings = receivedJson["settings"].as<JsonObject>();
        Turnout turnout = Turnout::fromJson(settings);
        bool setFrogClosed = turnoutManager.calculateFrogPolarityClosed(turnout, targetPosition);
        turnoutManager.setTurnoutPosition(turnout.id, targetPosition, setFrogClosed, turnout.throwSpeed);
      }
    }
    else
    {
      loggerWSE.log(ERROR, "Unknown message type");
    }
  }
  else if (type == WS_EVT_CONNECT)
  {
    loggerWSE.log(INFO, "Websocket client connection received");
    // store connected client
    for (int i = 0; i < 16; ++i)
      if (clients[i] == nullptr)
      {
        clients[i] = client;
        break;
      }
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    loggerWSE.log(INFO, "Client disconnected");
    // remove client from storage
    for (int i = 0; i < 16; ++i)
      if (clients[i] == client)
      {
        clients[i] = nullptr;
        break;
      }
  }
}

void SendTestComplete(const int turnoutId)
{
  receivedJson.clear();
  receivedJson["type"] = TurnoutManager::TYPE_TURNOUT_TEST_COMPLETE;
  receivedJson["turnoutId"] = turnoutId;
  String jsonString;
  serializeJson(receivedJson, jsonString);
  jsonString.toCharArray(dataBuffer, BUFFER_SIZE);
  for (AsyncWebSocketClient *c : clients)
  {
    if (c != nullptr)
    {
      c->text(dataBuffer);
    }
  }
  loggerWSE.log(DEBUG, "Sent data: %s", dataBuffer);
}
