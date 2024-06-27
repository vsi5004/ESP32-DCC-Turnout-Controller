#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <ArduinoJson.h>

class AppSettings
{
    public:
        char wifiSSID[32];
        char controllerName[32];
        bool darkMode;
        bool autoOpenOn;

        static constexpr const char *TYPE_APP_SETTINGS = "appSettings";
        static constexpr const char *TYPE_GET_APP_SETTINGS = "getAppSettings";

        AppSettings(const char *wifiSSID, const char *controllerName, bool darkMode, bool autoOpenOn)
            : darkMode(darkMode), autoOpenOn(autoOpenOn)
        {
            strncpy(this->wifiSSID, wifiSSID, sizeof(this->wifiSSID));
            strncpy(this->controllerName, controllerName, sizeof(this->controllerName));
        };
        void init();
        void saveToFile() const;
        String toJson() const;
        String toNestedJson() const;
        static AppSettings fromJson(const JsonObject &json);

    private:
        void loadFromFile();
};

#endif // APP_SETTINGS_H
