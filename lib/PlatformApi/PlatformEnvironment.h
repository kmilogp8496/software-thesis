#include <../WiFiCredentials.h>
#include <PlatformSensor.h>

#ifndef _WIFI_CREDENTIALS_H_

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#endif

#ifndef _APP_ENVIRONMENT_H_
#define _APP_ENVIRONMENT_H_

#ifdef CA_CERTIFICATE
#define API_URL "kmilo.dev"
#define API_PORT 443
#else
#define API_URL "192.168.1.138"
#define API_PORT 3000

#endif
#define AUTH_TOKEN_NAME "nuxt-session="

#endif
