#include <ESPAsyncWebServer.h>

void WSEventHandler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

void SendTestComplete(int turnoutId);
