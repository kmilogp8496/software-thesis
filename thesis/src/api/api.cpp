#include "api.h"

CookieJar cookieJar;
String authToken;

HTTPClient http;

#ifdef CA_CERTIFICATE
    WiFiClientSecure client;
    client.setCACert(CA_CERTIFICATE);
    bool https = true;
#else
    WiFiClient client;
    bool https = false;
#endif

void login() {
    JsonDocument doc;
    doc["id"] = SENSOR_ID;
    doc["username"] = SENSOR_NAME;
    doc["password"] = SENSOR_PASSWORD;

    const char* headers[] = {"Set-Cookie"};
    http.collectHeaders(headers, sizeof(headers)/ sizeof(headers[0]));
    
    http.begin(client, API_URL, API_PORT, "/api/things/login", https);
    String payload;

    serializeJson(doc, payload);
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode <= 0) {
        Serial.print("Error: ");
        Serial.println(httpResponseCode);
    }
    http.end();

    String cookie = http.header("Set-Cookie");

    authToken = cookie.substring(cookie.indexOf("=") + 1, cookie.indexOf(";")) + ";";
    Serial.println(authToken);
}


void handleResponse(HTTPClient* http, int httpResponseCode) {
    Serial.println(httpResponseCode);
    if (httpResponseCode > 0) {
        if (httpResponseCode == HTTP_CODE_UNAUTHORIZED) {
            Serial.println("Unauthorized");
            http->end();
            login();
            return;
        }

        String response = http->getString();
        // Successful response
        Serial.println(response);
    } else {
        // Error occurred
        Serial.print("Error: ");
        Serial.println(httpResponseCode);
    }

    // Close the connection
    http->end();
}

void sendGetRequest(String path) {
    http.begin(client, API_URL, API_PORT, path, https);

    http.addHeader("Cookie", AUTH_TOKEN_NAME + authToken);

    handleResponse(&http, http.GET());
}

void sendPostRequest(String path, JsonDocument& doc) {
    String payload;

    // Specify the target URL
    http.begin(client, API_URL, API_PORT, path, https);
    http.addHeader("Cookie", AUTH_TOKEN_NAME + authToken);

    serializeJson(doc, payload);

    handleResponse(&http, http.POST(payload));
}
