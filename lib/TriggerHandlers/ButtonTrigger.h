#pragma once
#include <Arduino.h>
#include <OneButton.h>
#include "Trigger.h"
#include <PubSubClient.h>

// Pass the MQTT client reference to publish messages
extern PubSubClient client;
extern const char *id;

class ButtonTrigger : public Trigger
{
private:
  int pin;                        // GPIO pin for the button
  OneButton button;               // OneButton instance
  static ButtonTrigger *instance; // Static instance for forwarding

  void handleClick() { publishTrigger("Click"); }
  void handleLongPress() { publishTrigger("Hold"); }

  void publishTrigger(const String &triggerType)
  {
    String message = "{\"deviceId\": \"" + String(id) + "\", \"trigger\": \"" + triggerType + "\"}";
    client.publish("trigger", message.c_str());
  }

  // Static forwarding methods
  static void onClick() { instance->handleClick(); }
  static void onLongPress() { instance->handleLongPress(); }

public:
  ButtonTrigger(int pin)
      : pin(pin), button(pin, true, true) {}

  void setup() override
  {
    pinMode(pin, INPUT_PULLUP);
    instance = this;
    button.setLongPressIntervalMs(1000);
    button.attachClick(onClick);
    button.attachLongPressStart(onLongPress);
  }

  void loop() override
  {
    button.tick();
  }
};

// Define static member
ButtonTrigger *ButtonTrigger::instance = nullptr;