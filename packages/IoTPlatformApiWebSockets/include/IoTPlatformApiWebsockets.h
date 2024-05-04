#include <IotPlatformApi.h>
#include <WebSocketsClient.h>

typedef std::function<void(JsonDocument)> HandleMessageCallback;

void platformWebsocketsConnect(HandleMessageCallback);

void platformPushMessage(JsonDocument doc);