#ifndef DCCMANAGER_H
#define DCCMANAGER_H

#include <Arduino.h>

class DCCManager
{
public:
#pragma pack(1)
    union DCCPacket
    {
        struct
        {
            uint16_t preamble : 10;     // 10 bits
            uint8_t startBit : 1;       // 1 bit
            uint8_t address : 8;        // 8 bits
            uint8_t spacingBit1 : 1;    // 1 bit
            uint8_t instruction : 8;    // 8 bits
            uint8_t spacingBit2 : 1;    // 1 bit
            uint8_t errorDetection : 8; // 8 bits (XOR of address and instruction)
            uint8_t endBit : 1;         // 1 bit
            uint32_t unused : 26;        // Padding to fill 64 bits
        };
        struct
        {
            uint32_t bits[2]; // Array of 64 bits packed into 2 uint32_t
        };
    };
#pragma pop // Restore default packing

    DCCManager(uint8_t dccPin);
    void init();
    void processDCC();
    static constexpr int BIT_1_MIN = 150;
    static constexpr int BIT_1_MAX = 240;
    static constexpr int BIT_0_MIN = 90;
    static constexpr int BIT_0_MAX = 130;
    static constexpr int DCC_MIN_PACKET_LEN = 38;

private:
    uint8_t _dccPin;

    static const int BUFFER_SIZE = 512; // Size of the circular buffer
    static bool checkPacketIntegrity(DCCPacket &packet);
    static uint8_t calculateXOR(uint8_t address, uint8_t instruction);
    static bool isValidPacket(int arr[], int length);
    static DCCPacket extractPacket(int arr[]);
};

#endif // DCCMANAGER_H
