#include "ConnectionsSetup.h"
#include "CallbackHandler.h"
#include "Credentials.h"
#include "CallbackOnOff.h"
#include "CallbackPing.h"

CallbackHandler callbackHandler;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  callbackHandler.registerCallback(new CallbackOnOff("AuraHub_bc4937f37619|OnOff", RELAY_PIN));
  callbackHandler.registerCallback(new CallbackPing("Ping"));

  // Set global MQTT callback
  client.setCallback([](char *topic, byte *payload, unsigned int length)
                     { callbackHandler.handleCallback(topic, payload, length); });

  // Set up all devices
  callbackHandler.setupAll();

  // Initialize EEPROM
  EEPROM.begin(sizeof(struct settings));
  EEPROM.get(0, user_wifi);

  // Setup AP Config
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  setup_wifi();
}

void loop()
{
  server.handleClient();
  dnsServer.processNextRequest();

  if (WiFi.status() != WL_CONNECTED)
  {
    if (!setupIsRunning && !accesPointIsRunning)
    {
      setup_wifi();
    }
  }

  if (!client.connected())
  {
    setup_mqtt();
  }

  client.loop();
  callbackHandler.loopAll();
}