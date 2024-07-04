#include "DCCManager.h"

volatile unsigned long lastInterruptTime = 0;
QueueHandle_t bitTimeQueue;

void IRAM_ATTR handleInterrupt()
{
    unsigned long currentTime = micros();
    unsigned long elapsedTime = currentTime - lastInterruptTime;
    lastInterruptTime = currentTime;
    xQueueSendFromISR(bitTimeQueue, &elapsedTime, NULL);
}

DCCManager::DCCManager(uint8_t dccPin) : _dccPin(dccPin)
{
    bitTimeQueue = xQueueCreate(60, sizeof(unsigned long));
}

void DCCManager::init()
{
    pinMode(_dccPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(_dccPin), handleInterrupt, RISING);
}

void DCCManager::processDCC()
{
    unsigned long receivedTime;

    if (xQueueReceive(bitTimeQueue, &receivedTime, portMAX_DELAY))
    {
        int bit = receivedTime;
        if (receivedTime > DCCManager::BIT_1_MIN && receivedTime < DCCManager::BIT_1_MAX)
        {
            bit = 1;
        }
        else if (receivedTime > DCCManager::BIT_0_MIN && receivedTime < DCCManager::BIT_0_MAX)
        {
            bit = 0;
        }
        Serial.print(bit);
        Serial.print(" : ");

        // add bit to circular buffer

    }
}

bool DCCManager::checkPacketIntegrity(DCCPacket &packet)
{
    uint8_t calculatedXOR = calculateXOR(packet.address, packet.instruction);
    return calculatedXOR == packet.errorDetection;
}

uint8_t DCCManager::calculateXOR(uint8_t address, uint8_t instruction)
{
    return address ^ instruction;
}

bool DCCManager::isValidPacket(int arr[], int length)
{
    if (length < DCC_MIN_PACKET_LEN)
        return false;

    // Check preamble
    for (int i = 0; i < 10; i++)
    {
        if (arr[i] != 1)
            return false;
    }
    // Check start bit
    if (arr[10] != 0)
        return false;
    // Check first spacing bit
    if (arr[19] != 0)
        return false;
    // Check second spacing bit
    if (arr[28] != 0)
        return false;
    // Check end bit
    if (arr[37] != 1)
        return false;

    return true;
}

DCCManager::DCCPacket DCCManager::extractPacket(int arr[])
{
    DCCPacket packet;
    memset(packet.bits, 0, sizeof(packet.bits)); // Initialize to zero

    for (int i = 0; i < 64; i++)
    {
        int wordIndex = i / 32;
        int bitIndex = i % 32;
        if (arr[i] == 1)
        {
            packet.bits[wordIndex] |= (1 << bitIndex);
        }
    }

    return packet;
}
