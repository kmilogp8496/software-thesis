#include "Api.h"
#include <ArduinoWebsockets.h>

using namespace websockets;

WebsocketsClient client;
WebsocketsEvent status = WebsocketsEvent::ConnectionClosed;

void openWebsocketsConnection()
{
    client.addHeader("Cookie", AUTH_TOKEN_NAME + platformGetAuthToken());
#ifdef CA_CERTIFICATE
    client.setCACert(CA_CERTIFICATE);
    bool connected = client.connectSecure(API_URL, API_PORT, "/_ws");
#else
    bool connected = client.connect(API_URL, API_PORT, "/_ws");
#endif

    Serial.println("Connecting to Websockets...");

    if (connected)
    {
        Serial.println("Connected!");
        client.send("Hello Server");
    }
    else
    {
        Serial.println("Not Connected!");
    }
}

TaskHandle_t Task1;

void Task1code(void *pvParameters)
{

    for (;;)
    {
        platformWebsocketsLoop();
        delay(100);
    }
}

void platformWebsocketsConnect()
{
    xTaskCreatePinnedToCore(
        Task1code,        /* Task function. */
        "Task1",          /* name of task. */
        10000,            /* Stack size of task */
        NULL,             /* parameter of the task */
        tskIDLE_PRIORITY, /* priority of the task */
        &Task1,           /* Task handle to keep track of created task */
        0);               /* pin task to core 0 */
    openWebsocketsConnection();

    // run callback when messages are received
    client.onMessage(
        [&](WebsocketsMessage message)
        {
            Serial.print("Got Message: ");
            Serial.println(message.data());
            JsonDocument doc;
            deserializeJson(doc, message.data());
            Serial.println(doc["value"].as<int>());
            analogWrite(14, doc["value"].as<int>());
        });

    client.onEvent(
        [&](WebsocketsEvent event, String data)
        {
            switch (event)
            {
            case WebsocketsEvent::ConnectionOpened:
                Serial.println("Connnection Opened");
                status = WebsocketsEvent::ConnectionOpened;
                break;
            case WebsocketsEvent::ConnectionClosed:
                Serial.println("Connnection Closed");
                status = WebsocketsEvent::ConnectionClosed;
                break;
            case WebsocketsEvent::GotPing:
                Serial.println("Got a Ping!");
                client.send("Pong!");
                break;
            case WebsocketsEvent::GotPong:
                Serial.println("Got a Pong!");
                break;
            default:
                break;
            }
        });
}

void platformWebsocketsLoop()
{
    if (status == WebsocketsEvent::ConnectionClosed)
    {
        Serial.println("Reconnecting...");
        openWebsocketsConnection();
        delay(1000);
        return;
    }
    if (client.available())
    {
        client.poll();
    }
}
