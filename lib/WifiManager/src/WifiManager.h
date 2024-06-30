#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

class WifiManager
{
    public:
        WifiManager();
        void init();
        void processNextRequest();
        
    private:
        void configWifi();
        void initWifi();
        void startWebsocketServer();
        void startWifi();
};

#endif // WIFI_MANAGER_H