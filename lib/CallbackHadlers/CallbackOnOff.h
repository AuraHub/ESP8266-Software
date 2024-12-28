#pragma once
#include <Arduino.h>
#include "Callback.h"

class CallbackOnOff : public Callback
{
private:
  const char *topic;
  int relayPin;

public:
  CallbackOnOff(const char *topic, int relayPin)
      : topic(topic), relayPin(relayPin) {}

  void setup() override
  {
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, HIGH); // Default to off
  }

  void loop() override
  {
  }

  void handle(char *topic, byte *payload, unsigned int length) override
  {
    char payloadString[length + 1];
    for (unsigned int i = 0; i < length; i++)
    {
      payloadString[i] = (char)payload[i];
    }
    payloadString[length] = '\0';

    if (strcmp(payloadString, "true") == 0)
    {
      Serial.println("Turning on relay");
      digitalWrite(relayPin, LOW);
    }
    else
    {
      Serial.println("Turning off relay");
      digitalWrite(relayPin, HIGH);
    }
  }

  const char *getTopic() override
  {
    return topic;
  }
};