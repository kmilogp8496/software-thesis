#include "api.h"

String handleResponse(HTTPClient* http, int httpResponseCode) {
    if (httpResponseCode > 0) {
        // Successful response
        String response = http->getString();
        Serial.println(response);
        return response;
    } else {
        // Error occurred
        Serial.print("Error: ");
        Serial.println(httpResponseCode);
        return "";
    }

    // Close the connection
    http->end();
}

void sendGetRequest(String path) {
    HTTPClient http;

    #ifdef CA_CERTIFICATE
        WiFiClientSecure client;
        client.setCACert(CA_CERTIFICATE);
        bool https = true;
    #else
        WiFiClient client;
        bool https = false;
    #endif


    // Specify the target URL
    http.begin(client, API_URL, API_PORT, path, https);

    handleResponse(&http, http.GET());
}

void sendPostRequest(String path, JsonDocument& doc) {
    HTTPClient http;

    #ifdef CA_CERTIFICATE
        WiFiClientSecure client;
        client.setCACert(CA_CERTIFICATE);
        bool https = true;
    #else
        WiFiClient client;
        bool https = false;
    #endif


    // Specify the target URL
    http.begin(client, API_URL, API_PORT, path, https);
    String payload;

    serializeJson(doc, payload);

    handleResponse(&http, http.POST(payload));
}