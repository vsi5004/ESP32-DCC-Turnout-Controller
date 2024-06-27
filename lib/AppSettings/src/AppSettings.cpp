#include "AppSettings.h"
#include <LittleFS.h>
#include <Elog.h>

Elog loggerAS;

String AppSettings::toJson() const
{
    StaticJsonDocument<200> doc;
    doc["wifiSSID"] = wifiSSID;
    doc["controllerName"] = controllerName;
    doc["darkMode"] = darkMode;
    doc["autoOpenOn"] = autoOpenOn;

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

String AppSettings::toNestedJson() const
{
    StaticJsonDocument<256> doc;
    doc["type"] = TYPE_APP_SETTINGS;

    JsonObject settings = doc.createNestedObject("settings");
    settings["wifiSSID"] = wifiSSID;
    settings["controllerName"] = controllerName;
    settings["darkMode"] = darkMode;
    settings["autoOpenOn"] = autoOpenOn;

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

AppSettings AppSettings::fromJson(const JsonObject &json)
{
    return AppSettings(
        json["wifiSSID"],
        json["controllerName"],
        json["darkMode"],
        json["autoOpenOn"]
    );
}

void AppSettings::init()
{
    loggerAS.addSerialLogging(Serial, "AppSettings", DEBUG);
    loadFromFile();
}

void AppSettings::loadFromFile()
{
    if (LittleFS.exists("/appSettings.txt"))
    {
        File file = LittleFS.open("/appSettings.txt", "r");
        if (file)
        {
            size_t size = file.size();
            std::unique_ptr<char[]> buf(new char[size]);
            file.readBytes(buf.get(), size);
            file.close();

            StaticJsonDocument<200> doc;
            DeserializationError error = deserializeJson(doc, buf.get());

            if (!error)
            {
                // Extract JsonObject from the document
                JsonObject json = doc.as<JsonObject>();
                *this = AppSettings::fromJson(json);
                loggerAS.log(INFO, "Successfully loaded appSettings from file");
            }
            else
            {
                loggerAS.log(ERROR, "Failed to parse appSettings file");
            }
        }
    }
    else
    {
        loggerAS.log(INFO, "Creating new appSettings file");
        saveToFile();
    }
}

void AppSettings::saveToFile() const
{
    File file = LittleFS.open("/appSettings.txt", "w");
    if (file)
    {
        String jsonString = toJson();
        file.print(jsonString);
        file.close();
        loggerAS.log(INFO, "Saved appSettings to file");
    }
}
