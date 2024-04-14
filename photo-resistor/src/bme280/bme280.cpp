#include "Environment.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include "PlatformApi.h"
#include <Adafruit_BME280.h>

#define REQUEST_INTERVAL 300000

Adafruit_BME280 bme; // I2C

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

  Serial.println(String("Platform Sensor ") + String(SENSOR_NAME) + String(" started"));
  Serial.println(String("Connected to the WiFi network: ") + String(WIFI_SSID));
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP());

  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);
  if (!status)
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    Serial.println("Restarting in 10 seconds");
    delay(10000);
    ESP.restart();
  }

  platformLogin(SENSOR_ID, SENSOR_NAME, SENSOR_PASSWORD);
}

void loop()
{
  JsonDocument doc;
  doc[SENSOR_TEMPERATURA_TEMPERATURA_SALON] = bme.readTemperature();
  doc[SENSOR_HUMEDAD_HUMEDAD_SALON] = bme.readHumidity();
  doc[SENSOR_PRESION_PRESION_SALON] = bme.readPressure() / 100.0F;

  platformPushData(doc);

  delay(REQUEST_INTERVAL);
}
