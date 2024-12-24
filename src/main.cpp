#include "ConnectionsSetup.h"
#include "CallbackHandler.h"
#include "Credentials.h"

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  Serial.begin(115200);

  // Initialize EEPROM
  EEPROM.begin(sizeof(struct settings));
  EEPROM.get(0, user_wifi);

  // Setup AP Config
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  setup_wifi();

  registerDevice("ping", callbackPing);
  registerDevice("AuraHub_bc4937f37619|OnOff", callbackOnOff);

  // Set the global callback for the MQTT client
  client.setCallback(handleCallback);
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
}