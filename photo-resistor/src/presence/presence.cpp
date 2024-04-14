#include "Environment.h"
#include <Arduino.h>
#include <PlatformApi.h>

enum PINS {
  SR_602 = 36,
  SR_501 = 39,
  SR_505 = 34,
};

void setup()
{

  Serial.begin(921600);
  Serial.println("Serial connected");

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

  Serial.println(String("Presence Sensor started") + String(SENSOR_NAME));
  Serial.println(String("Connected to the WiFi network: ") + String(WIFI_SSID));
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP());

  pinMode(PINS::SR_602, INPUT);
  pinMode(PINS::SR_505, INPUT);
  pinMode(PINS::SR_501, INPUT);

  
  platformLogin(SENSOR_ID, SENSOR_NAME, SENSOR_PASSWORD);
  platformWebsocketsConnect(NULL);
}

void loop()
{
  delay(5000);
  Serial.println("SR_602: " + String(digitalRead(PINS::SR_602)));
  Serial.println("SR_501: " + String(digitalRead(PINS::SR_501)));
  Serial.println("SR_505: " + String(digitalRead(PINS::SR_505)));

  JsonDocument doc;
  doc[SENSOR_CONFIGURATION_SR_602_FOR_VARIABLE_MOVIMIENTO_AT_LOCATION_ALGUN_LUGAR] = digitalRead(PINS::SR_602);
  doc[SENSOR_CONFIGURATION_SR_501_FOR_VARIABLE_MOVIMIENTO_AT_LOCATION_ALGUN_LUGAR] = digitalRead(PINS::SR_501);
  doc[SENSOR_CONFIGURATION_SR_505_FOR_VARIABLE_MOVIMIENTO_AT_LOCATION_ALGUN_LUGAR] = digitalRead(PINS::SR_505);

  pushMessage(doc);
}
