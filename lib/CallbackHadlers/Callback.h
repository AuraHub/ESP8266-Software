#pragma once
#include <Arduino.h>
#include <functional>

class Callback
{
public:
  virtual void setup() = 0;
  virtual void loop() = 0;
  virtual void handle(char *topic, byte *payload, unsigned int length) = 0;
  virtual const char *getTopic() = 0;
};