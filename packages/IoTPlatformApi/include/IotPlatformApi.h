#include <ArduinoJson.h>
#include <HTTPClient.h>

#ifndef _APP_ENVIRONMENT_H_
#define _APP_ENVIRONMENT_H_

#define AUTH_TOKEN_NAME "nuxt-session="
#endif

typedef std::function<void(JsonDocument)> HandleMessageCallback;

void platformLogin(int id, const char *username, const char *password, const char *url, int port);

void platformPushData(JsonDocument &doc);

String platformGetAuthToken();

const char *platformGetApiUrl();

int platformGetApiPort();

void platformSetCaCertificate(const char *caCertificate);

const char *platformGetCaCertificate();