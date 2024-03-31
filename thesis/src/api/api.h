#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "CACertificate.h"
#include "Environment.h"
#include "IoTPlatformSensor.h"

#ifndef _APP_ENVIRONMENT_H_
#define _APP_ENVIRONMENT_H_

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#define API_URL "easy-iot.vercel.app"
#define API_PORT 443

#define AUTH_TOKEN_NAME "nuxt-session="

#endif

void platformLogin();

void platformPushData(JsonDocument& doc);