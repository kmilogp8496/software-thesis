#include <ArduinoJson.h>
#include <CACertificate.h>
#include <HTTPClient.h>

#ifndef _APP_ENVIRONMENT_H_
#define _APP_ENVIRONMENT_H_

#define API_URL "iot.kmilo.dev"
#define API_PORT 443

#define AUTH_TOKEN_NAME "nuxt-session="
#endif

typedef std::function<void(JsonDocument)> HandleMessageCallback;

void platformLogin(int id, const char* username, const char* password);

void platformPushData(JsonDocument& doc);

String platformGetAuthToken();
