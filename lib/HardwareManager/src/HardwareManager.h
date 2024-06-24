#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include "Turnout.h"

class HardwareManager
{
  public:
    HardwareManager();
    void initServos();
    void updateServoPosition(Turnout *turnout);
};

#endif // HARDWARE_MANAGER_H