#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <DNSServer.h>
#include <PubSubClient.h>
#include "Credentials.h"

ESP8266WebServer server(80);
DNSServer dnsServer;

WiFiClient espClient;
PubSubClient client(espClient);

const char *id = ID;
const char *mqtt_server = BROKER_IP;
const int mqtt_port = BROKER_PORT;
const char *attributes = ATTRIBUTES;

bool setupIsRunning = false;
bool accesPointIsRunning = false;

IPAddress apIP(192, 168, 1, 1);
const byte DNS_PORT = 53;

String formResponseHTML = "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title> <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center}</style> </head> <body><main class='form-signin'> <form action='/post' method='post'> <h1 class=''>Wifi Setup</h1><br/><div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid'> </div><div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password'></div><br/><br/><button type='submit'>Save</button><p style='text-align: right'></p></form></main> </body></html>";

struct settings
{
    char ssid[30];
    char password[30];

} user_wifi = {};

void setup_wifi()
{
    setupIsRunning = true;
    accesPointIsRunning = false;
    delay(100);

    // Turn on wifi and try to connect with credentials from EEPROM
    Serial.println();
    Serial.print("Trying to connect to ");
    Serial.println(user_wifi.ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(user_wifi.ssid, user_wifi.password);

    // Try to connect to wifi (with data from EEPROM)
    byte tries = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(BUILTIN_LED, LOW);
        Serial.print(".");
        delay(600);

        // If not connected after 15 seconds, run access point, turn on LED and break from while loop
        if (tries++ > 15)
        {
            accesPointIsRunning = true;
            Serial.println("");
            Serial.println("Starting Access Point Mode");
            digitalWrite(BUILTIN_LED, LOW);
            WiFi.mode(WIFI_AP);
            WiFi.softAP(id);

            dnsServer.start(DNS_PORT, "*", apIP);

            // Turn on web server and wait
            server.on("/", handleForm);
            server.on("/post", handlePost);

            server.onNotFound([]()
                              { server.send(200, "text/html", formResponseHTML); });
            server.begin();

            break;
        }
        digitalWrite(BUILTIN_LED, HIGH);
        delay(400);
    }

    // If connect print IP and turn off LED
    if (tries < 15)
    {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        digitalWrite(BUILTIN_LED, HIGH);

        // Set basic mqtt settings
        client.setServer(mqtt_server, mqtt_port);
        client.setCallback(mqttCallback);

        // Run mqtt setup
        setup_mqtt();
    }

    setupIsRunning = false;
}

void setup_mqtt()
{
  // Loop until we're connected
  while (!client.connected())
  {
    Serial.println("Attempting MQTT connection...");

    // Attempt to connect
    if (client.connect(id))
    {
      Serial.println("Connected to broker");
      client.subscribe(id);
      client.subscribe("ping");

      Serial.println("Sending setup");
      String message = "{\"deviceId\": \"";
      message += id;
      message += "\", \"attributes\":";
      message += attributes;
      message += "}";
      client.publish("setup", message.c_str());

      digitalWrite(BUILTIN_LED, LOW);
      delay(10);
      digitalWrite(BUILTIN_LED, HIGH);
      delay(70);
      digitalWrite(BUILTIN_LED, LOW);
      delay(10);
      digitalWrite(BUILTIN_LED, HIGH);
      delay(70);
      digitalWrite(BUILTIN_LED, LOW);
      delay(10);
      digitalWrite(BUILTIN_LED, HIGH);
    }
    else
    {
      Serial.print("Connection failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");
      // Wait 3 seconds before retrying
      for (int i = 0; i < 6; i++)
      {
        digitalWrite(BUILTIN_LED, HIGH);
        delay(450);
        digitalWrite(BUILTIN_LED, LOW);
        delay(50);
      }
    }
  }
}

void setup()
{
    // Initializing BUILTIN_LED and serial
    pinMode(BUILTIN_LED, OUTPUT);
    Serial.begin(115200);

    // Start EEPROM and retrive data
    EEPROM.begin(sizeof(struct settings));
    EEPROM.get(0, user_wifi);

    // Setup access point config
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

    setup_wifi();
}

void loop()
{
    // Handle web server and DNS request
    server.handleClient();
    dnsServer.processNextRequest();

    // If not connected to wifi run wifi setup sequence
    if (WiFi.status() != WL_CONNECTED)
    {
        if (setupIsRunning == false && accesPointIsRunning == false)
        {
            setup_wifi();
        }
    }

    // If not connected to mqtt broker run setup sequence
    if (!client.connected())
    {
      setup_mqtt();
    }

    client.loop();
}

void handlePost()
{

        // Parse data from HTTP POST request
        strncpy(user_wifi.ssid, server.arg("ssid").c_str(), sizeof(user_wifi.ssid));
        strncpy(user_wifi.password, server.arg("password").c_str(), sizeof(user_wifi.password));
        user_wifi.ssid[server.arg("ssid").length()] = user_wifi.password[server.arg("password").length()] = '\0';

        server.send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title><style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> </head> <body><main class='form-signin'> <h1>Wifi Setup</h1> <br/> <p>Your settings have been saved successfully!</main></body></html>");

        // Put data to EEPROM memory
        EEPROM.put(0, user_wifi);
        EEPROM.commit();

        // Rerun wifi setup sequence after getting new data
        setup_wifi();
}

void handleForm()
{
    server.send(200, "text/html", formResponseHTML);
}

void handleSuccess()
{
    server.send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup Success</title><style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> </head> <body><main class='form-signin'> <h1>Wifi Setup Success</h1> <br/> <p>Your settings have been saved successfully!</main></body></html>");
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  payload[length] = '\0';
  String strPayload = String((char *)payload);
  Serial.print(strPayload);
  Serial.println();

  if (strcmp(topic, "ping") == 0){
    Serial.println("Returning ping");
    String message = "{\"deviceId\": \"";
    message += id;
    message += "\"}";
    client.publish("returnPing", message.c_str());
  }
  
}