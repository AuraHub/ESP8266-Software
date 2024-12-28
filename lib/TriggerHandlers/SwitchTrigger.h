#pragma once
#include <Arduino.h>
#include "Trigger.h"
#include <PubSubClient.h>

// Pass the MQTT client reference to publish messages
extern PubSubClient client;
extern const char *id;

class SwitchTrigger : public Trigger
{
private:
  int pin; // GPIO pin for the Switch

  void publishTrigger(const String &triggerType)
  {
    String message = "{\"deviceId\": \"" + String(id) + "\", \"trigger\": \"" + triggerType + "\"}";
    client.publish("trigger", message.c_str());
  }

public:
  SwitchTrigger(int pin)
      : pin(pin) {}

  void setup() override
  {
    pinMode(pin, INPUT_PULLUP);
  }

  void loop() override
  {
    static int lastState = HIGH;         // Store the last state of the switch
    int currentState = digitalRead(pin); // Read the current state of the switch

    // Check if the state has changed from the last state
    if (currentState != lastState)
    {
      // Update the last state
      lastState = currentState;

      // Publish "ON" or "OFF" based on the current state
      if (currentState == LOW)
      {
        publishTrigger("SwitchOn");
      }
      else
      {
        publishTrigger("SwitchOff");
      }
    }
  }
};