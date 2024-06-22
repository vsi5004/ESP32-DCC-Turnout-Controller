#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include "wsEventHandler.h"
#include "Turnout.h"
#include "TurnoutManager.h"

// allocate memory for received json data
#define BUFFER_SIZE 512
StaticJsonDocument<BUFFER_SIZE> receivedJson;
// initial device state
char dataBuffer[BUFFER_SIZE] = "{\"type\":\"message\",\"LED\":false}";
AsyncWebSocketClient *clients[16] = {nullptr};

// External reference to TurnoutManager instance
extern TurnoutManager turnoutManager;

void wsEventHandler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_DATA)
  {
    if (len >= BUFFER_SIZE)
    {
      Serial.println("Data too large for buffer");
      return;
    }

    // save the response as newest device state
    strncpy(dataBuffer, (char *)data, len);
    dataBuffer[len] = '\0';
    Serial.print("Received data: ");
    Serial.println(dataBuffer);

    // parse the received json data
    DeserializationError error = deserializeJson(receivedJson, dataBuffer);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    const char *msgType = receivedJson["type"];
    if (strcmp(msgType, "message") == 0)
    {
      // get the target LED state
      bool led = receivedJson["LED"];
      digitalWrite(2, led);
      // send ACK
      client->text(dataBuffer);
      // alert all other clients
      for (AsyncWebSocketClient *c : clients)
        if (c != nullptr && c != client)
          c->text(dataBuffer);
    }
    else if (strcmp(msgType, TurnoutManager::TYPE_GET_TURNOUTS) == 0)
    {
      String jsonString = turnoutManager.turnoutsToJson();
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
      receivedJson["type"] = TurnoutManager::TYPE_TURNOUT_TEST_COMPLETE;
      String jsonString;
      serializeJson(receivedJson, jsonString);
      jsonString.toCharArray(dataBuffer, BUFFER_SIZE);
      client->text(dataBuffer);
      Serial.print("Sent data: ");
      Serial.println(dataBuffer);
    }
    else
    {
      Serial.println("Unknown message type");
    }
  }
  else if (type == WS_EVT_CONNECT)
  {
    Serial.println("Websocket client connection received");
    // ACK with current state
    client->text(dataBuffer);
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
    Serial.println("Client disconnected");
    // remove client from storage
    for (int i = 0; i < 16; ++i)
      if (clients[i] == client)
      {
        clients[i] = nullptr;
        break;
      }
  }
}
