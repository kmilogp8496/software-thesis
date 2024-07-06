#include "Environment.h"
#include "IoTPlatformApiWebsockets.h"
#include "WiFiCredentials.h"
#include <DHT.h>
#include "CACertificate.h"

#define REQUEST_INTERVAL 5000
#define PRESENCE_THRESHOLD 600

enum PINS
{
    DHT_SENSOR = 25,
    PHOTO_DIODE = 34
};

DHT dht(PINS::DHT_SENSOR, DHT11);

TaskHandle_t presenceTaskHandle;

void setup()
{
    Serial.begin(921600);
    pinMode(PINS::PHOTO_DIODE, INPUT);
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

    platformSetCaCertificate(CA_CERTIFICATE);

    platformLogin(SENSOR_ID, SENSOR_NAME, SENSOR_PASSWORD, API_URL, API_PORT);

    platformWebsocketsConnect(NULL);
}

void loop()
{
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println("Error obteniendo los datos del sensor DHT11");
        delay(REQUEST_INTERVAL);
        return;
    }

    JsonDocument doc;
    doc[SENSOR_CONFIGURATION_ILUMINACION_FOR_VARIABLE_ILUMINACION_AT_LOCATION_SALON] = analogRead(PINS::PHOTO_DIODE);
    doc[SENSOR_CONFIGURATION_TEMPERATURA_FOR_VARIABLE_TEMPERATURA_AT_LOCATION_SALON] = temperature;
    doc[SENSOR_CONFIGURATION_HUMEDAD_FOR_VARIABLE_HUMEDAD_AT_LOCATION_SALON] = humidity;
    platformPushMessage(doc);

    delay(REQUEST_INTERVAL);
}
