#include <Arduino.h>
#include <LittleFS.h>
#include <TurnoutManager.h>
#include <WifiManager.h>
#include <Elog.h>

bool wifiEnabled = false;

Elog logger;
TurnoutManager turnoutManager;
WifiManager wifiManager;

void initFileSystem();

void setup()
{
  Serial.begin(115200);
  logger.addSerialLogging(Serial, "Main", DEBUG);

  turnoutManager.init();
  initFileSystem();
  turnoutManager.loadTurnouts();
  turnoutManager.initTurnouts();
  wifiEnabled = turnoutManager.shouldWifiBeEnabled();
  if (wifiEnabled)
  {
    logger.log(INFO, "Starting with WiFi enabled");
    wifiManager.init();
  }
  else
  {
    logger.log(INFO, "Starting with DCC enabled");
    // Do DCC related things
  }
}

void initFileSystem()
{
  if (!LittleFS.begin())
  {
    logger.log(CRITICAL, "An Error has occurred while mounting LittleFS");
    while (true)
    {
      delay(1000);
    }
  }
}

void loop()
{
  if (wifiEnabled)
  {
    wifiManager.processNextRequest();
  }
  else
  {
    // Do DCC related things
  }
  turnoutManager.updateTurnoutPositions();
  turnoutManager.checkForReboot();
  vTaskDelay(1);
}
