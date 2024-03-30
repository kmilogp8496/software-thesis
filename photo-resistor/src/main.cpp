#include <Arduino.h>
#include "Api/Api.h"

enum PINS {
  DELAY_LED = 13,
  TIMER_LED = 12,
  WEB_SOCKET_LED = 14,
  PHOTO_RESISTOR = 34,
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

  Serial.println(String("Connected to the WiFi network: ") + String(WIFI_SSID));
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP());

  pinMode(PINS::DELAY_LED, OUTPUT);
  pinMode(PINS::TIMER_LED, OUTPUT);
  pinMode(PINS::WEB_SOCKET_LED, OUTPUT);

  
  platformLogin();
  platformWebsocketsConnect([](JsonDocument doc) {
    float ledIllumination = doc["7"].as<float>();
    analogWrite(PINS::WEB_SOCKET_LED, ledIllumination);
  });
}

void loop()
{
}
