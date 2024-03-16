#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "CACertificate.h"
#include "Environment.h"
#include "IoTPlatformSensor.h"

void platformLogin();

void platformPushData(JsonDocument& doc);