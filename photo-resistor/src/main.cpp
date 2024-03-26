#include <Arduino.h>
#include "Api/Api.h"

enum PINS {
  DELAY_LED = 13,
  TIMER_LED = 12,
  WEB_SOCKET_LED = 14,
  PHOTO_RESISTOR = 34,
};

hw_timer_t *Timer0_Cfg = NULL;

void IRAM_ATTR Timer0_ISR()
{
    digitalWrite(PINS::TIMER_LED, !digitalRead(PINS::TIMER_LED));
}

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

  Timer0_Cfg = timerBegin(0, 80, true);
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
  timerAlarmWrite(Timer0_Cfg, 1000000, true);
  timerAlarmEnable(Timer0_Cfg);
  
  platformLogin();
  platformWebsocketsConnect();
}

void loop()
{
  delay(2000);

  uint16_t value = analogRead(PINS::PHOTO_RESISTOR);
  int output = 256 - ((value + 1) / 16);

  analogWrite(PINS::DELAY_LED, output);

  Serial.println(String("Valor leido: ") + String(value));
  Serial.println(String("Valor escrito: ") + String(output));
}
