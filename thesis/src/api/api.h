#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "CACertificate.h"
#include "Environment.h"

void sendGetRequest(String path);

void sendPostRequest(String path, JsonDocument& doc);