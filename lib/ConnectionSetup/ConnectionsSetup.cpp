#include "ConnectionsSetup.h"

// Global variables
ESP8266WebServer server(80);
DNSServer dnsServer;

WiFiClient espClient;
PubSubClient client(espClient);

const char *id = ID;
const char *mqtt_server = BROKER_IP;
const int mqtt_port = BROKER_PORT;
const char *attributes[] = ATTRIBUTES;
const char *trigers[] = TRIGERS;

bool setupIsRunning = false;
bool accesPointIsRunning = false;

IPAddress apIP(192, 168, 1, 1);
const byte DNS_PORT = 53;

String formResponseHTML = "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title> <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center}</style> </head> <body><main class='form-signin'> <form action='/post' method='post'> <h1 class=''>Wifi Setup</h1><br/><div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid'> </div><div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password'></div><br/><br/><button type='submit'>Save</button><p style='text-align: right'></p></form></main> </body></html>";

settings user_wifi = {}; // Initialize the user_wifi structure

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
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print(".");
    delay(600);

    // If not connected after 15 seconds, run access point, turn on LED and break from while loop
    if (tries++ > 15)
    {
      accesPointIsRunning = true;
      Serial.println("");
      Serial.println("Starting Access Point Mode");
      digitalWrite(LED_BUILTIN, LOW);
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
    digitalWrite(LED_BUILTIN, HIGH);
    delay(400);
  }

  // If connect print IP and turn off LED
  if (tries < 15)
  {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_BUILTIN, HIGH);

    // Set basic mqtt settings
    client.setServer(mqtt_server, mqtt_port);

    // Run mqtt setup
    setup_mqtt();
  }

  setupIsRunning = false;
}

void setup_mqtt()
{
  while (!client.connected())
  {
    Serial.println("Attempting MQTT connection...");
    if (client.connect(id))
    {
      Serial.println("Connected to broker");
      client.subscribe("ping");
      for (size_t i = 0; i < sizeof(attributes) / sizeof(attributes[0]); ++i)
      {
        client.subscribe((String(id) + "|" + String(attributes[i])).c_str());
      }
      client.subscribe(id);

      Serial.println("Sending setupDevice message");
      String message = "{\"deviceId\": \"";
      message += id;
      message += "\", \"attributes\":[";
      for (size_t i = 0; i < sizeof(attributes) / sizeof(attributes[0]); ++i)
      {
        message += "\"" + String(attributes[i]) + "\"";
        if (i < (sizeof(attributes) / sizeof(attributes[0]) - 1))
        {
          message += ",";
        }
      }
      message += "]}";
      client.publish("setupDevice", message.c_str());

      Serial.println("Sending setupDeviceTrigger message");
      message = "{\"deviceId\": \"";
      message += id;
      message += "\", \"triggers\":[";
      for (size_t i = 0; i < sizeof(trigers) / sizeof(trigers[0]); ++i)
      {
        message += "\"" + String(trigers[i]) + "\"";
        if (i < (sizeof(trigers) / sizeof(trigers[0]) - 1))
        {
          message += ",";
        }
      }
      message += "]}";
      client.publish("setupDeviceTrigger", message.c_str());

      // Flash LED to indicate success
      for (int i = 0; i < 3; i++)
      {
        digitalWrite(LED_BUILTIN, LOW);
        delay(10);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(70);
      }
    }
    else
    {
      Serial.print("Connection failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");

      for (int i = 0; i < 6; i++)
      {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(450);
        digitalWrite(LED_BUILTIN, LOW);
        delay(50);
      }
    }
  }
}

void handlePost()
{
  strncpy(user_wifi.ssid, server.arg("ssid").c_str(), sizeof(user_wifi.ssid));
  strncpy(user_wifi.password, server.arg("password").c_str(), sizeof(user_wifi.password));
  user_wifi.ssid[server.arg("ssid").length()] = user_wifi.password[server.arg("password").length()] = '\0';

  server.send(200, "text/html", "<p>Your settings have been saved successfully!</p>");
  EEPROM.put(0, user_wifi);
  EEPROM.commit();

  setup_wifi();
}

void handleForm()
{
  server.send(200, "text/html", formResponseHTML);
}

void handleSuccess()
{
  server.send(200, "text/html", "<p>Your settings have been saved successfully!</p>");
}