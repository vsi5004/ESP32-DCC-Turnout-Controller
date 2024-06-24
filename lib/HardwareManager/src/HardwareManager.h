#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include "Turnout.h"

class HardwareManager
{
  public:
    HardwareManager();
    void init();
    void updateServoPosition(Turnout *turnout);
    void disableServo(int channel);
    void setServoPosition(const int channel, const Turnout *turnout);
    void setRelayPostion(const int channel, const bool state);
};

#endif // HARDWARE_MANAGER_H