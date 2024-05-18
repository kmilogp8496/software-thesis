#include <IoTPlatformApiWebsockets.h>

#define USE_SERIAL Serial

// callback function for handling messages as JSON
void handleMessage(JsonDocument doc)
{
}

HandleMessageCallback handleMessageCallback = NULL;

WebSocketsClient webSocket;

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16)
{
    const uint8_t *src = (const uint8_t *)mem;
    USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
    for (uint32_t i = 0; i < len; i++)
    {
        if (i % cols == 0)
        {
            USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
        }
        USE_SERIAL.printf("%02X ", *src);
        src++;
    }
    USE_SERIAL.printf("\n");
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    JsonDocument doc;

    switch (type)
    {
    case WStype_DISCONNECTED:
        USE_SERIAL.printf("[WSc] Disconnected!\n");
        break;
    case WStype_CONNECTED:
        USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
        break;
    case WStype_TEXT:
        USE_SERIAL.printf("[WSc] get text: %s\n", payload);
        deserializeJson(doc, payload);

        if (handleMessageCallback != NULL)
        {
            handleMessageCallback(doc);
        }

        break;
    case WStype_BIN:
        USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
        hexdump(payload, length);

        break;
    case WStype_ERROR:
        hexdump(payload, length);
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
        break;
    }
}

void openWebsocketsConnection()
{
    webSocket.setReconnectInterval(1000);
    webSocket.enableHeartbeat(15000, 3000, 2);

    String cookieHeader = "Cookie: " + String(AUTH_TOKEN_NAME) + platformGetAuthToken();
    webSocket.setExtraHeaders(cookieHeader.c_str());

    webSocket.onEvent(webSocketEvent);

    const char *caCertificate = platformGetCaCertificate();

    if (caCertificate != "")
    {
        webSocket.beginSslWithCA(platformGetApiUrl(), platformGetApiPort(), "/_ws", caCertificate);
    }
    else
    {
        webSocket.begin(platformGetApiUrl(), platformGetApiPort(), "/_ws");
    }
}

TaskHandle_t websocketLoopTaskHandler;

void websocketsLoop(void *pvParameters)
{
    for (;;)
    {
        webSocket.loop();

        delay(200);
    }
}

void platformWebsocketsConnect(HandleMessageCallback handleMessageCb)
{
    handleMessageCallback = handleMessageCb;

    openWebsocketsConnection();

    xTaskCreatePinnedToCore(
        websocketsLoop,             /* Task function. */
        "websocketLoopTaskHandler", /* name of task. */
        10000,                      /* Stack size of task */
        NULL,                       /* parameter of the task */
        tskIDLE_PRIORITY,           /* priority of the task */
        &websocketLoopTaskHandler,  /* Task handle to keep track of created task */
        0);                         /* pin task to core 0 */
}


void platformPushMessage(JsonDocument doc)
{
    String message;
    serializeJson(doc, message);

    int disconnected = 0;

    while (webSocket.isConnected() == false)
    {
        USE_SERIAL.println("Waiting for connection for sending messages...");
        delay(500);
        disconnected++;

        if (disconnected > 10)
        {
            webSocket.disconnect();
            openWebsocketsConnection();
            disconnected = 0;
        }
    }

    webSocket.sendTXT(message);
}