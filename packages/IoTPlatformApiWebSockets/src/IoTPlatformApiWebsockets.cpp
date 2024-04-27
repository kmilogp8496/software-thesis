#include <IoTPlatformApiWebsockets.h>

#define USE_SERIAL Serial

// callback function for handling messages as JSON
void handleMessage(JsonDocument doc)
{
}

HandleMessageCallback handleMessageCallback = NULL;

WebSocketsClient webSocket;

void pushMessage(JsonDocument doc)
{
    String message;
    serializeJson(doc, message);
    webSocket.sendTXT(message);
}

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
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
        break;
    }
}

void openWebsocketsConnection()
{
    webSocket.setReconnectInterval(2000);

    String cookieHeader = "Cookie: " + String(AUTH_TOKEN_NAME) + platformGetAuthToken();
    webSocket.setExtraHeaders(cookieHeader.c_str());

    webSocket.onEvent(webSocketEvent);

#ifdef CA_CERTIFICATE
    webSocket.beginSslWithCA(API_URL, API_PORT, "/_ws", CA_CERTIFICATE);
#else
    webSocket.begin(API_URL, API_PORT, "/_ws");
#endif
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