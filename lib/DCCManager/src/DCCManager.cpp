#include "DCCManager.h"

static constexpr int DCC_PIN = 4;
int onebittime = 150; // works nce..using average

byte timeRiseFall = 0; // 0 = Rising 1 = Falling
byte timingSync = 0;   // 1 means in sync
int timingSyncCounter; // Counter that works through a number of interrupts to check they are correct

volatile byte ISRRISING;
volatile unsigned long ISRRISETime;
unsigned long ISRLastRISETime;
byte Preamble = 0;
byte PreambleCounter = 0;
byte PacketStart = 0;
byte bitcounter = 0;
byte packetdata[28]; // array to get all the data ready for processing
int counter = 0;

// This is the interrupt function checking when pin state change
void IRAM_ATTR pinstate()
{
    ISRRISING = 1;
    ISRRISETime = micros();
}

DCCManager::DCCManager()
{
}

void DCCManager::init()
{
    pinMode(DCC_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(DCC_PIN), pinstate, RISING);
}

void DCCManager::processDCC()
{
    byte thisByte;
    int interruptTime;
    if (timingSync < 1)
    { // check if the interrupt is working correctly does it need reversing?
        if (timingSyncCounter < 200)
        { // check 200 bits...won't take long
            //Serial.println(timingSyncCounter);
            if (ISRRISING > 0)
            {
                if (timingSyncCounter > 9)
                {                                                  // allow a few cycles for everything to settle down
                    interruptTime = ISRRISETime - ISRLastRISETime; // time between interrupts
                    if (interruptTime < 125 || interruptTime > 200)
                    { // check the interrupt is within DCC parameters

                        // Serial.println(interruptTime);
                    }
                    else
                    {
                        Serial.print("F: ");
                        Serial.println(interruptTime);
                        timingSyncCounter = 0; // ready to start again
                        detachInterrupt(digitalPinToInterrupt(DCC_PIN));
                        if (timeRiseFall < 1)
                        {
                            timeRiseFall = 1;
                            attachInterrupt(digitalPinToInterrupt(DCC_PIN), pinstate, FALLING);
                            Serial.println("Switch to FALLING");
                        }
                        else
                        {
                            timeRiseFall = 0;
                            attachInterrupt(digitalPinToInterrupt(DCC_PIN), pinstate, RISING);
                            Serial.println("Switch to RISING");
                        }
                    }
                }
                timingSyncCounter++;
                ISRLastRISETime = ISRRISETime;
                ISRRISING = 0;
            }
        }
        else
        { // greater than 200 succesful so get the main system running
            timingSync = 1;
            timingSyncCounter = 0;
            Serial.println("Sync Complete...time for work");
        }
    }
    else
    {
        if (ISRRISING > 0)
        {
            if ((ISRRISETime - ISRLastRISETime) > onebittime)
            {
                thisByte = 0;
            }
            else
            {
                thisByte = 1;
            }
            ISRRISING = 0;
            ISRLastRISETime = ISRRISETime;
            if (Preamble < 1)
            { // hasn't finished preamble
                if (thisByte == 1)
                {
                    PreambleCounter++;
                }
                else
                {
                    PreambleCounter = 0;
                }
                if (PreambleCounter > 9)
                {                 // NMRA say a decoder must receive aminimum of 10 "1" bits before accepting a packet
                    Preamble = 1; // pramble condition met
                    PacketStart = 0;
                    PreambleCounter = 0;
                }
            }
            else
            { // preamble has been completed, wait for packet start
                if (PacketStart < 1)
                {
                    if (thisByte == 0)
                    {
                        PacketStart = 1;
                        bitcounter = 1;
                    }
                }
                else
                { // we have a preamble and a packet start so now get the rest of the data ready for processing
                    packetdata[bitcounter] = thisByte;
                    bitcounter++;
                    if (bitcounter > 27)
                    { // should now have the whole packet in array
                        Preamble = 0;
                        PreambleCounter = 0;
                        processDCCPacket();
                    }
                }
            }
        }
    }
}

void DCCManager::processDCCPacket()
{
    byte AddressByte = 0;
    byte InstructionByte = 0;
    byte ErrorByte = 0;
    byte errorTest;
    for (int q = 0; q < 8; q++)
    {
        if (packetdata[1 + q] > 0)
        {
            bitSet(AddressByte, 7 - q);
        }
        else
        {
            bitClear(AddressByte, 7 - q);
        }

        if (packetdata[10 + q] > 0)
        {
            bitSet(InstructionByte, 7 - q);
        }
        else
        {
            bitClear(InstructionByte, 7 - q);
        }

        if (packetdata[19 + q] > 0)
        {
            bitSet(ErrorByte, 7 - q);
        }
        else
        {
            bitClear(ErrorByte, 7 - q);
        }
    }
    // certain Addressess are reserved for the system so not shown to reduce load
    if (AddressByte > 0 && AddressByte != 0b11111110 && AddressByte != 0b11111111 && bitRead(AddressByte, 7) == 1 && bitRead(AddressByte, 6) == 0)
    {
        errorTest = AddressByte ^ InstructionByte;
        if (errorTest == ErrorByte)
        { // only send command if error byte checks out
            AccDecoder(AddressByte, InstructionByte, ErrorByte);
        }
    }
}

void DCCManager::AccDecoder(byte AddressByte, byte InstructionByte, byte ErrorByte)
{
    byte index;
    byte dir;
    int AccAddr;
    int BoardAddr;

    if (packetdata[27] == 1)
    { // basic packet
        dir = bitRead(InstructionByte, 0);
        index = bitRead(InstructionByte, 1);
        if (bitRead(InstructionByte, 2) > 0)
        {
            bitSet(index, 1);
        }

        BoardAddr = AddressByte - 0b10000000;
        // now get the weird address system from instruction byte
        if (bitRead(InstructionByte, 4) < 1)
        {
            bitSet(BoardAddr, 6);
        }
        if (bitRead(InstructionByte, 5) < 1)
        {
            bitSet(BoardAddr, 7);
        }
        if (bitRead(InstructionByte, 6) < 1)
        {
            bitSet(BoardAddr, 8);
        }
        AccAddr = ((BoardAddr - 1) * 4) + index + 1;
        ControlAccDecoder(index, dir, AccAddr, BoardAddr);
    }
    else
    {
        Serial.print("Ext pckt format not supported yet");
    }
}

void DCCManager::ControlAccDecoder(byte index, byte dir, int AccAddr, int BoardAddr)
{
    // NOTE: Sometimes the same function will be triggered multiple times. This is not a bug. The same packets are sent multiple times very quickly
    // in case the decoder was busy and missed a transmission.

    Serial.print(" BoardAddr: ");
    Serial.println(BoardAddr);

    Serial.print("index: ");
    Serial.println(index);

    Serial.println("two values to use in switch statements");
    Serial.print(" AccAddr: ");
    Serial.println(AccAddr);
    Serial.print("dir: ");
    Serial.println(dir);

}
