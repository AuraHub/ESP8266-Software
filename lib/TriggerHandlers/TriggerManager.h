#pragma once
#include <vector>
#include "Trigger.h"

class TriggerManager
{
private:
  std::vector<Trigger *> triggers;

public:
  void registerTrigger(Trigger *trigger)
  {
    triggers.push_back(trigger);
  }

  void setupAll()
  {
    for (Trigger *trigger : triggers)
    {
      trigger->setup();
    }
  }

  void loopAll()
  {
    for (Trigger *trigger : triggers)
    {
      trigger->loop();
    }
  }
};