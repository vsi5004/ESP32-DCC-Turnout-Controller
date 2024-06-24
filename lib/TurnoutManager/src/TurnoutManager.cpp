#include "TurnoutManager.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "HardwareManager.h"


HardwareManager hardwareManager;

TurnoutManager::TurnoutManager()
{
    turnoutCount = 0;
    for (int i = 0; i < MAX_TURNOUTS; i++)
    {
        turnouts[i] = nullptr;
    }
}

void TurnoutManager::loadTurnouts()
{
    Serial.println("Loading turnouts");
    if (LittleFS.exists("/turnouts.txt"))
    {
        File file = LittleFS.open("/turnouts.txt", "r");
        if (file)
        {
            size_t size = file.size();
            std::unique_ptr<char[]> buf(new char[size]);
            file.readBytes(buf.get(), size);
            file.close();

            StaticJsonDocument<2000> doc;
            DeserializationError error = deserializeJson(doc, buf.get());

            if (!error)
            {
                turnoutCount = doc.size();
                for (int i = 0; i < turnoutCount && i < MAX_TURNOUTS; i++)
                {
                    turnouts[i] = new Turnout(Turnout::fromJson(doc[i]));
                }
                Serial.println("Turnouts loaded from file");
            }
            else
            {
                Serial.println("Failed to parse turnouts file");
            }
        }
    }
    else
    {
        Serial.println("Creating new turnouts file");
        saveTurnouts(); // Create an empty turnouts file
    }
}

void TurnoutManager::saveTurnouts()
{
    StaticJsonDocument<2000> doc;
    for (int i = 0; i < turnoutCount; i++)
    {
        JsonObject obj = doc.createNestedObject();
        obj["id"] = turnouts[i]->id;
        obj["address"] = turnouts[i]->address;
        obj["closedEndpoint"] = turnouts[i]->closedEndpoint;
        obj["openEndpoint"] = turnouts[i]->openEndpoint;
        obj["reversed"] = turnouts[i]->reversed;
        obj["startClosed"] = turnouts[i]->startClosed;
        obj["testInProgress"] = turnouts[i]->testInProgress;
        obj["throwSpeed"] = turnouts[i]->throwSpeed;
        obj["poweredFrog"] = turnouts[i]->poweredFrog;
        obj["reverseFrogPolarity"] = turnouts[i]->reverseFrogPolarity;
    }

    File file = LittleFS.open("/turnouts.txt", "w");
    if (file)
    {
        serializeJson(doc, file);
        file.close();
        Serial.println("Latest turnout settings saved in file");
    }
    else
    {
        Serial.println("Failed to create turnouts file");
    }
}

void TurnoutManager::updateTurnout(const Turnout &turnout)
{
    for (int i = 0; i < turnoutCount; i++)
    {
        if (turnouts[i]->id == turnout.id)
        {
            *turnouts[i] = turnout;
            saveTurnouts();
            return;
        }
    }

    if (turnoutCount < MAX_TURNOUTS)
    {
        turnouts[turnoutCount++] = new Turnout(turnout);
        saveTurnouts();
    }
    else
    {
        Serial.println("Max turnouts limit reached");
    }
}

String TurnoutManager::turnoutsToJson() const
{
    StaticJsonDocument<2000> doc;
    doc["type"] = TYPE_TURNOUTS_LIST;
    JsonArray turnoutsArray = doc.createNestedArray("turnouts");
    for (int i = 0; i < turnoutCount; i++)
    {
        JsonObject turnoutObj = turnoutsArray.createNestedObject();
        turnoutObj["id"] = turnouts[i]->id;
        turnoutObj["address"] = turnouts[i]->address;
        turnoutObj["closedEndpoint"] = turnouts[i]->closedEndpoint;
        turnoutObj["openEndpoint"] = turnouts[i]->openEndpoint;
        turnoutObj["reversed"] = turnouts[i]->reversed;
        turnoutObj["startClosed"] = turnouts[i]->startClosed;
        turnoutObj["testInProgress"] = turnouts[i]->testInProgress;
        turnoutObj["throwSpeed"] = turnouts[i]->throwSpeed;
        turnoutObj["poweredFrog"] = turnouts[i]->poweredFrog;
        turnoutObj["reverseFrogPolarity"] = turnouts[i]->reverseFrogPolarity;
    }

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

void TurnoutManager::initHardwareManager()
{
    hardwareManager.initServos();
}

void TurnoutManager::updateTurnoutPositions()
{
    for (int i = 0; i < turnoutCount; i++)
    {
        hardwareManager.updateServoPosition(turnouts[i]);
    }
}

void TurnoutManager::setTurnoutPosition(int turnoutId, int targetPosition, int throwSpeed = 0)
{
    for (int i = 0; i < turnoutCount; i++)
    {
        if (turnouts[i]->id == turnoutId)
        {
            turnouts[i]->targetPosition = targetPosition;
            turnouts[i]->lastMoveTime = millis();
            turnouts[i]->testInProgress = true;

            // Set throw speed if provided for endpoint testing, otherwise use the existing speed
            // Note that 0 is not a valid option in the UI, so it is safe to use as a default value
            if (throwSpeed > 0)
            {
                turnouts[i]->throwSpeed = throwSpeed;
            }

            Serial.print("Setting turnout ");
            Serial.print(turnoutId);
            Serial.print(" to position ");
            Serial.println(targetPosition);

            return;
        }
    }
}
