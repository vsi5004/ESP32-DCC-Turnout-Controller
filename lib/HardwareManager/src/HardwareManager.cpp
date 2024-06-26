#include "HardwareManager.h"
#include "WSEventHandler.h"
#include "TurnoutManager.h"
#include <Adafruit_PWMServoDriver.h>
#include <Elog.h>

static constexpr int SERVO_OFF_CYCLE = 4096;
static constexpr int SERVO_FREQUENCY = 50;
static constexpr int SERVO_MAX_POSITION = 600;
static constexpr int SERVO_MIN_POSITION = 150;
// Note: pins 34 and 35 are input only and will cause an error if used with digitalWrite
static constexpr int RELAY_PINS[TurnoutManager::MAX_TURNOUTS] = {32,33,25,26,27,14,12,13,19,18,5,17};
static constexpr int RELAY_INIT_STATE = HIGH;
static constexpr int BUTTON_PIN = 2;

Elog loggerHWM;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
volatile bool bRebootPending;

HardwareManager::HardwareManager()
{
}

void IRAM_ATTR handleButtonChange()
{
    bRebootPending = true;
}

void HardwareManager::init()
{
    bRebootPending = false;
    loggerHWM.addSerialLogging(Serial, "HWManager", DEBUG);
    pwm.begin();
    pwm.setPWMFreq(SERVO_FREQUENCY);
    for (int i = 0; i < TurnoutManager::MAX_TURNOUTS; i++)
    {
        pinMode(RELAY_PINS[i], OUTPUT);
    }
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonChange, CHANGE);
    m_bButtonPressed = digitalRead(BUTTON_PIN);
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
            setServoPosition(channel, turnout);
        }
        else if (turnout->currentPosition < position)
        {
            turnout->currentPosition++;
            setServoPosition(channel, turnout);
        }
        else
        {
            disableServo(channel);
            if (turnout->moveInProgress)
            {
                turnout->moveInProgress = false;
                SendTestComplete(turnout->id);
                loggerHWM.log(INFO, "Turnout %d arrived at position %d", turnout->id, turnout->currentPosition);
            }
        }
        turnout->lastMoveTime = currentMillis;
    }
}

void HardwareManager::disableServo(int channel)
{
    pwm.setPWM(channel, 0, SERVO_OFF_CYCLE);
}

void HardwareManager::setServoPosition(const int channel, const Turnout *turnout)
{
    pwm.setPWM(channel, 0, map(turnout->currentPosition, 0, 180, SERVO_MIN_POSITION, SERVO_MAX_POSITION));
    loggerHWM.log(DEBUG, "Moving turnout %d to position %d with target position %d", turnout->id, turnout->currentPosition, turnout->targetPosition);
}

void HardwareManager::setRelayPosition(const int channel, const bool state)
{
    digitalWrite(RELAY_PINS[channel], state);
    loggerHWM.log(DEBUG, "Setting relay %d to %s", channel, state ? "ON" : "OFF");
}

bool HardwareManager::isRebootPending()
{
    return bRebootPending;
}
