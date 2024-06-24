#include "HardwareManager.h"
#include "WSEventHandler.h"
#include <Adafruit_PWMServoDriver.h>

static constexpr int SERVO_OFF_CYCLE = 4096;
static constexpr int SERVO_FREQUENCY = 50;
static constexpr int SERVO_MAX_POSITION = 600;
static constexpr int SERVO_MIN_POSITION = 150;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

HardwareManager::HardwareManager()
{
}

void HardwareManager::initServos()
{
    pwm.begin();
    pwm.setPWMFreq(SERVO_FREQUENCY);
    delay(10);
}

void HardwareManager::updateServoPosition(Turnout *turnout)
{
    unsigned long currentMillis = millis();
    if (turnout->lastMoveTime + turnout->throwSpeed < currentMillis)
    {

        int channel = turnout->id;
        int position = turnout->targetPosition;
        if (turnout->currentPosition > position)
        {
            turnout->currentPosition--;
            pwm.setPWM(channel, 0, map(turnout->currentPosition, 0, 180, SERVO_MIN_POSITION, SERVO_MAX_POSITION));
            Serial.print("Moving turnout ");
            Serial.print(turnout->id);
            Serial.print(" to position ");
            Serial.print(turnout->currentPosition);
            Serial.print(" with target position ");
            Serial.println(turnout->targetPosition);
        }
        else if (turnout->currentPosition < position)
        {
            turnout->currentPosition++;
            pwm.setPWM(channel, 0, map(turnout->currentPosition, 0, 180, SERVO_MIN_POSITION, SERVO_MAX_POSITION));
            Serial.print("Moving turnout ");
            Serial.print(turnout->id);
            Serial.print(" to position ");
            Serial.print(turnout->currentPosition);
            Serial.print(" with target position ");
            Serial.println(turnout->targetPosition);
        }
        else
        {
            pwm.setPWM(channel, 0, SERVO_OFF_CYCLE);
            if (turnout->testInProgress)
            {
                turnout->testInProgress = false;
                SendTestComplete(turnout->id);
                Serial.print("Turnout ");
                Serial.print(turnout->id);
                Serial.print(" arrived at position ");
                Serial.println(turnout->currentPosition);
            }
        }
        turnout->lastMoveTime = currentMillis;
    }
}
