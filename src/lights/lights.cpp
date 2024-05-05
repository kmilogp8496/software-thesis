#include "IoTPlatformApiWebsockets.h"
#include "WiFiCredentials.h"
#include "CACertificate.h"
#include "Environment.h"

enum PINS {
  LIGHTS = 13,
};

void setup()
{

  Serial.begin(921600);
  Serial.println("Serial connected");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  pinMode(PINS::LIGHTS, OUTPUT);

  platformSetCaCertificate(CA_CERTIFICATE);  
  platformLogin(SENSOR_ID, SENSOR_NAME, SENSOR_PASSWORD, API_URL, API_PORT);

  platformWebsocketsConnect([](JsonDocument doc) {
    int lightStatus = doc[ACTUATOR_CONFIGURATION_LUMINARIAS_FOR_VARIABLE_MOVIMIENTO_AT_LOCATION_SALON].as<int>();
    digitalWrite(PINS::LIGHTS, lightStatus);
  });
}

void loop()
{
}
