#include "IotPlatformApi.h"

String authToken = "";

#ifdef CA_CERTIFICATE
bool https = true;
#else
bool https = false;
#endif

int SENSOR_ID = 0;
const char *SENSOR_NAME = "";
const char *SENSOR_PASSWORD = "";
const char *API_URL = "";
int API_PORT = 443;
const char *caCertificate = "";

const char *platformGetApiUrl()
{
    return API_URL;
}

int platformGetApiPort()
{
    return API_PORT;
}

void platformSetCaCertificate(const char *certificate)
{
    caCertificate = certificate;
}

const char *platformGetCaCertificate()
{
    return caCertificate;
}

void platformLogin(int id, const char *username, const char *password, const char *url, int port)
{
    SENSOR_ID = id;
    SENSOR_NAME = username;
    SENSOR_PASSWORD = password;
    API_URL = url;
    API_PORT = port;

    WiFiClient *client = new WiFiClient;
    bool https = false;

    if (caCertificate != "")
    {
        client = new WiFiClientSecure;
    }

    if (client)
    {

        if (caCertificate != "")
        {
            ((WiFiClientSecure *)client)->setCACert(caCertificate);
            https = true;
        }

        {
            // Add a scoping block for HTTPClient http to make sure it is destroyed before WiFiClientSecure *client is
            HTTPClient http;

            if (http.begin(*client, platformGetApiUrl(), platformGetApiPort(), "/api/things/login", https))
            { // HTTPS
                JsonDocument doc;
                doc["id"] = SENSOR_ID;
                doc["username"] = SENSOR_NAME;
                doc["password"] = SENSOR_PASSWORD;
                const char *headers[] = {"Set-Cookie"};
                http.collectHeaders(headers, sizeof(headers) / sizeof(headers[0]));

                String payload;

                serializeJson(doc, payload);
                int httpResponseCode = http.POST(payload);

                Serial.println("Login response code: " + String(httpResponseCode));

                if (httpResponseCode > 0)
                {
                    String cookie = http.header("Set-Cookie");

                    authToken = cookie.substring(cookie.indexOf("=") + 1, cookie.indexOf(";")) + ";";
                }
                else
                {
                    Serial.print("Error: ");
                    Serial.println(httpResponseCode);
                }
                http.end();
            }
            else
            {
                Serial.printf("[HTTPS] Unable to connect\n");
            }
        }

        delete client;
    }

    else
    {
        Serial.println("Unable to create client");
    }
}

void handleResponse(HTTPClient *http, int httpResponseCode)
{
    Serial.println(httpResponseCode);
    if (httpResponseCode > 0)
    {
        if (httpResponseCode == HTTP_CODE_UNAUTHORIZED || httpResponseCode == HTTP_CODE_FORBIDDEN)
        {
            Serial.println("Unauthorized");
            http->end();
            platformLogin(SENSOR_ID, SENSOR_NAME, SENSOR_PASSWORD, platformGetApiUrl(), platformGetApiPort());
            return;
        }

        String response = http->getString();
        // Successful response
        Serial.println("Push response: " + response);
    }
    else
    {
        // Error occurred
        Serial.print("Error: ");
        Serial.println(httpResponseCode);
    }

    // Close the connection
    http->end();
}

void sendGetRequest(String path)
{
    WiFiClient *client = new WiFiClient;
    bool https = false;

    if (caCertificate != "")
    {
        client = new WiFiClientSecure;
    }

    if (client)
    {

        if (caCertificate != "")
        {
            ((WiFiClientSecure *)client)->setCACert(caCertificate);
            https = true;
        }

        {
            HTTPClient http;

            http.begin(*client, API_URL, API_PORT, path, https);

            http.addHeader("Cookie", AUTH_TOKEN_NAME + authToken);

            handleResponse(&http, http.GET());
        }

        delete client;
    }
    else
    {
        Serial.println("Unable to create client");
    }
}

void sendPostRequest(String path, JsonDocument &doc)
{

    WiFiClient *client = new WiFiClient;
    bool https = false;

    if (caCertificate != "")
    {
        client = new WiFiClientSecure;
    }
    if (client)
    {

        if (caCertificate != "")
        {
            ((WiFiClientSecure *)client)->setCACert(caCertificate);
            https = true;
        }

        {
            HTTPClient http;
            String payload;

            // Specify the target URL
            http.begin(*client, API_URL, API_PORT, path, https);
            http.addHeader("Cookie", AUTH_TOKEN_NAME + authToken);

            serializeJson(doc, payload);

            handleResponse(&http, http.POST(payload));
        }

        delete client;
    }
    else
    {
        Serial.println("Unable to create client");
    }
}

void platformPushData(JsonDocument &doc)
{
    sendPostRequest("/api/things", doc);
}

String platformGetAuthToken()
{
    return authToken;
}