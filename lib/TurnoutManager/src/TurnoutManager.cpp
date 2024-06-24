#include "TurnoutManager.h"
#include "WSEventHandler.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <Adafruit_PWMServoDriver.h>

static constexpr int SERVO_OFF_CYCLE = 4096;
static constexpr int SERVO_FREQUENCY = 50;
static constexpr int SERVO_MAX_POSITION = 600;
static constexpr int SERVO_MIN_POSITION = 150;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

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
        obj["testInProgress"] = turnouts[i]->testInProgress;
        obj["throwSpeed"] = turnouts[i]->throwSpeed;
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
        turnoutObj["testInProgress"] = turnouts[i]->testInProgress;
        turnoutObj["throwSpeed"] = turnouts[i]->throwSpeed;
    }

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

void TurnoutManager::initServos()
{
    pwm.begin();
    pwm.setPWMFreq(SERVO_FREQUENCY);
    delay(10);
}

void TurnoutManager::updateServoPositions()
{
    unsigned long currentMillis = millis();
    for (int i = 0; i < turnoutCount; i++)
    {
        if (turnouts[i]->lastMoveTime + turnouts[i]->throwSpeed < currentMillis)
        {

            int channel = turnouts[i]->id;
            int position = turnouts[i]->targetPosition;
            if (turnouts[i]->currentPosition > position)
            {
                turnouts[i]->currentPosition--;
                pwm.setPWM(channel, 0, map(turnouts[i]->currentPosition, 0, 180, SERVO_MIN_POSITION, SERVO_MAX_POSITION));
                Serial.print("Moving turnout ");
                Serial.print(turnouts[i]->id);
                Serial.print(" to position ");
                Serial.print(turnouts[i]->currentPosition);
                Serial.print(" with target position ");
                Serial.println(turnouts[i]->targetPosition);
            }
            else if (turnouts[i]->currentPosition < position)
            {
                turnouts[i]->currentPosition++;
                pwm.setPWM(channel, 0, map(turnouts[i]->currentPosition, 0, 180, SERVO_MIN_POSITION, SERVO_MAX_POSITION));
                Serial.print("Moving turnout ");
                Serial.print(turnouts[i]->id);
                Serial.print(" to position ");
                Serial.print(turnouts[i]->currentPosition);
                Serial.print(" with target position ");
                Serial.println(turnouts[i]->targetPosition);
            }
            else
            {
                pwm.setPWM(channel, 0, SERVO_OFF_CYCLE);
                if (turnouts[i]->testInProgress)
                {
                    turnouts[i]->testInProgress = false;
                    SendTestComplete(turnouts[i]->id);
                    Serial.print("Turnout ");
                    Serial.print(turnouts[i]->id);
                    Serial.print(" arrived at position ");
                    Serial.println(turnouts[i]->currentPosition);
                }
            }
            turnouts[i]->lastMoveTime = currentMillis;
        }
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
