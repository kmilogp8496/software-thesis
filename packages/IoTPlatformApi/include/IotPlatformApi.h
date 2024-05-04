#include <ArduinoJson.h>
#include <HTTPClient.h>

#define AUTH_TOKEN_NAME "nuxt-session="

void platformLogin(int id, const char *username, const char *password, const char *url, int port);

void platformPushData(JsonDocument &doc);

String platformGetAuthToken();

const char *platformGetApiUrl();

int platformGetApiPort();

void platformSetCaCertificate(const char *caCertificate);

const char *platformGetCaCertificate();