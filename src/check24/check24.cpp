#include "IoTPlatformApiWebsockets.h"
#include "WiFiCredentials.h"
#include <DHT.h>
#include "Environment.h"

#define REQUEST_INTERVAL 5000
#define PRESENCE_THRESHOLD 5

enum PINS
{
    PRESENCE_SENSOR = 0,
    DHT_SENSOR = 4,
    PHOTO_DIODE = 34
};

DHT dht(PINS::DHT_SENSOR, DHT11);

TaskHandle_t presenceTaskHandle;

void presenceTask(void *pvParameters)
{
    uint8_t current, last = LOW, secondsOff = 0;
    JsonDocument doc;
    for (;;)
    {
        delay(1000);
        current = digitalRead(PINS::PRESENCE_SENSOR);

        Serial.println("Presence: " + String(current));
        Serial.println("Seconds off: " + String(secondsOff));

        if (current == HIGH && last == LOW)
        {
            if (secondsOff >= PRESENCE_THRESHOLD - 1)
            {
                doc[SENSOR_CONFIGURATION_BACKOFFICE_PRESENCIA_FOR_VARIABLE_PRESENCIA_AT_LOCATION_BACKOFFICE] = current;
                pushMessage(doc);
            }
            secondsOff = 0;
        }
        else if (current == LOW && last == LOW)
        {
            if (secondsOff == PRESENCE_THRESHOLD)
            {
                continue;
            }
            else if (secondsOff == (PRESENCE_THRESHOLD - 1))
            {
                secondsOff++;
                doc[SENSOR_CONFIGURATION_BACKOFFICE_PRESENCIA_FOR_VARIABLE_PRESENCIA_AT_LOCATION_BACKOFFICE] = current;
                pushMessage(doc);
            }
            else
            {
                secondsOff++;
            }
        }

        last = current;
    }
}

void setup()
{
    Serial.begin(921600);
    pinMode(PINS::PRESENCE_SENSOR, INPUT);
    pinMode(PINS::PHOTO_DIODE, INPUT);
    Serial.println("Serial initialized");
    dht.begin();


    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    for (size_t i = 0; i < 60; i++)
    {
        Serial.println("Connecting to WiFi..");
        if (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
        }
        else
        {
            break;
        }

        if (i == 59)
        {
            ESP.restart();
        }
    }

    Serial.println(String("Connected to the WiFi network: ") + String(WIFI_SSID));
    Serial.println();
    Serial.println("WiFi connected");
    Serial.println("IP address set: ");
    Serial.println(WiFi.localIP());

    platformLogin(SENSOR_ID, SENSOR_NAME, SENSOR_PASSWORD);

    platformWebsocketsConnect([](JsonDocument doc)
                              {
        String payload;
        serializeJson(doc, payload);
        Serial.println(payload); });

    delay(5000);

    xTaskCreate(
        presenceTask,   /* Function to implement the task */
        "presenceTask", /* Name of the task */
        10000,                 /* Stack size in words */
        NULL,                  /* Task input parameter */
        tskIDLE_PRIORITY,      /* Priority of the task */
        &presenceTaskHandle);  /* Task handle. */
}

void loop()
{
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println("Error obteniendo los datos del sensor DHT11");
        return;
    }

    JsonDocument doc;
    doc[SENSOR_CONFIGURATION_BACKOFFICE_TEMPERATURA_FOR_VARIABLE_TEMPERATURA_AT_LOCATION_BACKOFFICE] = temperature;
    doc[SENSOR_CONFIGURATION_BACKOFFICE_HUMEDAD_FOR_VARIABLE_HUMEDAD_AT_LOCATION_BACKOFFICE] = humidity;
    doc[SENSOR_CONFIGURATION_BACKOFFICE_ILUMINACION_FOR_VARIABLE_ILUMINACION_AT_LOCATION_BACKOFFICE] = analogRead(PINS::PHOTO_DIODE);
    pushMessage(doc);

    Serial.println("Temperatura: " + String(temperature) + "°C");
    Serial.println("Humedad: " + String(humidity) + "%");
    Serial.println("Iluminación: " + String(analogRead(PINS::PHOTO_DIODE)));

    delay(REQUEST_INTERVAL);
}
