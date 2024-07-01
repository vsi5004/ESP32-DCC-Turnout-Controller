#ifndef DCC_MANAGER_H
#define DCC_MANAGER_H

#include <Arduino.h>

class DCCManager
{
    public:
        DCCManager();
        void init();
        void processDCC();
        void processDCCPacket();
        void AccDecoder(byte AddressByte, byte InstructionByte, byte ErrorByte);
        void ControlAccDecoder(byte index, byte dir, int AccAddr, int BoardAddr);
};

#endif // DCC_MANAGER_H