#include <Arduino.h>
#include "Environment.h"
#include "IoTPlatformSensor.h"
#include <ArduinoJson.h>
#include "api/api.h"
#include <Adafruit_BME280.h>

// #define REQUEST_INTERVAL 300000
#define REQUEST_INTERVAL 3000

Adafruit_BME280 bme; // I2C

void setup()
{
  Serial.begin(230400);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(String("Connected to the WiFi network: ") + String(WIFI_SSID));
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP());

  Serial.println("Serial connected");

  Serial.println(F("BME280 test"));

  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  Serial.println("-- Default Test --");

  Serial.println();
}

void loop()
{
  JsonDocument doc;
  doc[SENSOR_TEMPERATURA_TEMPERATURA_HABITACION] = bme.readTemperature();
  doc[SENSOR_HUMEDAD_HUMEDAD_HABITACION] = bme.readHumidity();
  doc[SENSOR_PRESION_PRESION_HABITACION] = bme.readPressure() / 100.0F;
  
  platformPushData(doc);

  delay(REQUEST_INTERVAL);
}
