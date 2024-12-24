#include "CallbackHandler.h"
#include "../../include/Credentials.h"

extern PubSubClient client;

void callbackPing(char *topic, byte *payload, unsigned int length)
{
  Serial.println("Returning ping");
  String message = "{\"deviceId\": \"";
  message += id;
  message += "\"}";
  client.publish("returnPing", message.c_str());
}