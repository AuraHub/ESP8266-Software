#pragma once
#include "CallbackHandler.h"
#include <Arduino.h>
#include "Callback.h"

class CallbackPing : public Callback
{
private:
  const char *topic;

public:
  CallbackPing(const char *topic)
      : topic(topic) {}

  void handle(char *topic, byte *payload, unsigned int length) override
  {
    Serial.println("Returning ping");
    String message = "{\"deviceId\": \"";
    message += id;
    message += "\"}";
    client.publish("returnPing", message.c_str());
  }

  const char *getTopic() override
  {
    return topic;
  }
};