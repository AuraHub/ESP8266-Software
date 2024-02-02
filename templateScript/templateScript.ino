#include <ESP8266WiFi.h>
#include "arduino_secrets.h"

const char ssid[] = SECRET_SSID;
const char password[] = SECRET_PASS;
const char id[] = SECRET_ID;

void setup_wifi()
{
    delay(10);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(BUILTIN_LED, LOW);
        delay(500);
        Serial.print(".");
        digitalWrite(BUILTIN_LED, HIGH);
        delay(250);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(BUILTIN_LED, LOW);
}

void setup()
{
    pinMode(BUILTIN_LED, OUTPUT);
    Serial.begin(115200);

    setup_wifi();

    digitalWrite(BUILTIN_LED, HIGH);
}

void loop()
{
}