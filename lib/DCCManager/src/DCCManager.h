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
            uint32_t unused : 26;       // Padding to fill 64 bits
        };
        uint64_t raw; // Raw 64-bit representation
    };
#pragma pop // Restore default packing

    DCCManager();
    void init();
    void processDCC();
    static constexpr int BIT_1_MIN = 95;
    static constexpr int BIT_1_MAX = 130;
    static constexpr int BIT_0_MIN = 45;
    static constexpr int BIT_0_MAX = 75;
    static constexpr int DCC_MIN_PACKET_LEN = 38;

private:
    uint8_t _dccPin;
    uint64_t _bitBuffer = 0;
    int _bitCount = 0;

    void addBit(int bit);
    bool checkForValidPacket();
    void resetBuffer();
    static bool checkPacketIntegrity(DCCPacket &packet);
    static uint8_t calculateXOR(uint8_t address, uint8_t instruction);
    static bool isValidPacket(uint64_t bits);
    static DCCPacket extractPacket(uint64_t bits);
};

#endif // DCCMANAGER_H
