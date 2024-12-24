#ifndef CONNECTIONSSETUP_H
#define CONNECTIONSSETUP_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <DNSServer.h>
#include <PubSubClient.h>
#include "../../include/Credentials.h"

// Structure to hold WiFi credentials
struct settings
{
  char ssid[30];
  char password[30];
};

// Declare the global variables
extern ESP8266WebServer server;
extern DNSServer dnsServer;
extern WiFiClient espClient;
extern PubSubClient client;
extern const char *id;
extern const char *mqtt_server;
extern const int mqtt_port;
extern const char *attributes[];
extern bool setupIsRunning;
extern bool accesPointIsRunning;
extern IPAddress apIP;
extern const byte DNS_PORT;
extern String formResponseHTML;
extern settings user_wifi;

void setup_wifi();
void setup_mqtt();
void handlePost();
void handleForm();
void handleSuccess();

#endif