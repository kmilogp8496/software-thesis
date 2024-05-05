#include "IotPlatformApi.h"
#include "Adafruit_BME280.h"
#include "Environment.h"
#include "WiFiCredentials.h"
#include "CACertificate.h"

#define REQUEST_INTERVAL 300000 // 5 minutes

Adafruit_BME280 bme;

void setup()
{
  Serial.begin(921600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  for (size_t i = 0; i < 60; i++)
  {
    Serial.println("Connecting to WiFi..");
    if (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
    } else {
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

  bool status;

  status = bme.begin(0x76);
  if (!status)
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    Serial.println("Restarting in 10 seconds");
    delay(10000);
    ESP.restart();
  }

  platformSetCaCertificate(CA_CERTIFICATE);
  platformLogin(SENSOR_ID, SENSOR_NAME, SENSOR_PASSWORD, API_URL, API_PORT);
}

void loop()
{
  JsonDocument doc;
  doc[SENSOR_CONFIGURATION_TEMPERATURA_FOR_VARIABLE_TEMPERATURA_AT_LOCATION_SALON] = bme.readTemperature();
  doc[SENSOR_CONFIGURATION_HUMEDAD_FOR_VARIABLE_HUMEDAD_AT_LOCATION_SALON] = bme.readHumidity();
  doc[SENSOR_CONFIGURATION_PRESION_FOR_VARIABLE_PRESION_AT_LOCATION_SALON] = bme.readPressure() / 100.0F;

  platformPushData(doc);

  delay(REQUEST_INTERVAL);
}
