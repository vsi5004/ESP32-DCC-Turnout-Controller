#include "DCCManager.h"
#include <Elog.h>
#include <bitset>

volatile unsigned long lastInterruptTime = 0;
volatile bool pinState = 0;
QueueHandle_t bitTimeQueue;
Elog loggerDCC;

static constexpr int DCC_PIN = 4;
static constexpr int DEBUG_PIN = 5;

// Bitmasks for specific bits (adjusted for MSB-first transmission)
const uint64_t START_BIT_MASK = (uint64_t)1 << 10;
const uint64_t SPACING_BIT1_MASK = (uint64_t)1 << 19;
const uint64_t SPACING_BIT2_MASK = (uint64_t)1 << 28;
const uint64_t END_BIT_MASK = (uint64_t)1 << 37;

void IRAM_ATTR handleInterrupt()
{
    unsigned long currentTime = micros();
    unsigned long elapsedTime = currentTime - lastInterruptTime;
    lastInterruptTime = currentTime;

    if(digitalRead(DCC_PIN) == LOW)
    {
        pinState = !pinState;
        digitalWrite(DEBUG_PIN, pinState);
        xQueueSendFromISR(bitTimeQueue, &elapsedTime, NULL);
    }
    
}

DCCManager::DCCManager()
{
    bitTimeQueue = xQueueCreate(60, sizeof(unsigned long));
}

void DCCManager::init()
{
    loggerDCC.addSerialLogging(Serial, "DCCManager", INFO);
    pinMode(DCC_PIN, INPUT);
    pinMode(DEBUG_PIN, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(DCC_PIN), handleInterrupt, CHANGE);
}

void DCCManager::processDCC()
{
    unsigned long receivedTime;

    for (;;)
    {
        if (xQueueReceive(bitTimeQueue, &receivedTime, portMAX_DELAY))
        {
            int bit = -1;
            if (receivedTime > DCCManager::BIT_1_MIN && receivedTime < DCCManager::BIT_1_MAX)
            {
                bit = 1;
            }
            else if (receivedTime > DCCManager::BIT_0_MIN && receivedTime < DCCManager::BIT_0_MAX)
            {
                bit = 0;
            }

            if (bit != -1)
            {
                addBit(bit);
                //loggerDCC.log(DEBUG, "Received bit: %d", bit);
                Serial.print(bit);

                //if (_bitCount >= DCC_MIN_PACKET_LEN && checkForValidPacket())
                //{
                //    loggerDCC.log(INFO, "Valid packet detected.");
                    //resetBuffer();
                //}
            }
            else{
                loggerDCC.log(INFO, "Invalid bit received, time: %d", receivedTime);
            }
        }
        //vTaskDelay(1);
    }
}

void DCCManager::addBit(int bit)
{
    _bitBuffer = (_bitBuffer >> 1) | ((uint64_t)bit << 63);
    if (_bitCount < 64)
    {
        _bitCount++;
    }
}

bool DCCManager::checkForValidPacket()
{
    if (isValidPacket(_bitBuffer))
    {
        //loggerDCC.log(INFO, "Packet is valid.");
        DCCPacket packet = extractPacket(_bitBuffer);

        if(packet.address != 0 && packet.address != 255)
        {
            loggerDCC.log(INFO, "Extracted packet: %s, %d", std::bitset<64>(packet.raw).to_string().c_str(), packet.address);
        }
        

        
        
        

        

        if (checkPacketIntegrity(packet))
        {
            //loggerDCC.log(INFO, "Packet integrity check passed.");
            
            return true;
        }
        else
        {
            //loggerDCC.log(ERROR, "Packet integrity check failed.");
        }
    }
    return false;
}

bool DCCManager::isValidPacket(uint64_t bits)
{

    for (int i = 0; i < 10; i++)
    {
        if (((bits >> i) & 1) != 1)
        {
            loggerDCC.log(DEBUG, "Preamble check failed at bit position %d", i);
            return false;
        }
    }

    if ((bits & START_BIT_MASK) != 0)
    {
        loggerDCC.log(DEBUG, "Start bit check failed at bit position 53.");
        return false;
    }

    

    if ((bits & SPACING_BIT1_MASK) != 0)
    {
        loggerDCC.log(DEBUG, "First spacing bit check failed at bit position 44.");
        return false;
    }

    loggerDCC.log(INFO, "Bits: %s", std::bitset<64>(bits).to_string().c_str());

    if ((bits & SPACING_BIT2_MASK) != 0)
    {
        loggerDCC.log(DEBUG, "Second spacing bit check failed at bit position 35.");
        return false;
    }

    if ((bits & END_BIT_MASK) != END_BIT_MASK)
    {
        loggerDCC.log(DEBUG, "End bit check failed at bit position 26.");
        return false;
    }
    return true;
}

DCCManager::DCCPacket DCCManager::extractPacket(uint64_t bits)
{
    DCCPacket packet;
    packet.raw = bits;
    return packet;
}

bool DCCManager::checkPacketIntegrity(DCCPacket &packet)
{
    uint8_t calculatedXOR = calculateXOR(packet.address, packet.instruction);
    loggerDCC.log(DEBUG, "Calculated XOR: %d", calculatedXOR);
    loggerDCC.log(DEBUG, "Error Detection byte: %d", packet.errorDetection);
    return calculatedXOR == packet.errorDetection;
}

uint8_t DCCManager::calculateXOR(uint8_t address, uint8_t instruction)
{
    return address ^ instruction;
}

void DCCManager::resetBuffer()
{
    Serial.println("Resetting buffer...");
    _bitBuffer = 0;
    _bitCount = 0;
}
