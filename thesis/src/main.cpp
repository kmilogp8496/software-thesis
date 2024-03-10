#include <Arduino.h>
#include "Environment.h"
#include <ArduinoJson.h>
#include "api/api.h"

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
}

void loop()
{
  delay(10000);
  sendGetRequest("/api/things/" + String(SENSOR_ID));
}
