#pragma once
#include <Arduino.h>

class Trigger
{
public:
  virtual void setup() = 0; // Called during setup to configure the trigger
  virtual void loop() = 0;  // Called during the main loop to process the trigger
};