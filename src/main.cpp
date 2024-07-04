#include <Arduino.h>
#include <LittleFS.h>
#include <TurnoutManager.h>
#include <WifiManager.h>
#include <DCCManager.h>
#include <Elog.h>

bool wifiEnabled = false;

Elog logger;
TurnoutManager turnoutManager;
DCCManager dccManager(4);
WifiManager wifiManager;

void initFileSystem();

void turnoutManagerTask(void *pvParameters)
{
  for (;;)
  {
    turnoutManager.updateTurnoutPositions();
    turnoutManager.checkForReboot();
    vTaskDelay(1);
  }
}

void dccManagerTask(void *pvParameters)
{
  for (;;)
  {
    dccManager.processDCC();
    vTaskDelay(1);
  }
}

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
    dccManager.init();
    xTaskCreatePinnedToCore(
        dccManagerTask,     // Function to implement the task
        "DCC Manager Task", // Name of the task
        10000,                // Stack size in words
        NULL,                 // Task input parameter
        2,                    // Priority of the task
        NULL,                 // Task handle
        0                     // Core where the task should run
    );
  }
  xTaskCreatePinnedToCore(
    turnoutManagerTask,       // Function to implement the task
    "Turnout Manager Task",   // Name of the task
    10000,                 // Stack size in words
    NULL,                  // Task input parameter
    1,                     // Priority of the task
    NULL,                  // Task handle
    1                      // Core where the task should run
  );
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
  
  vTaskDelay(1);
}
