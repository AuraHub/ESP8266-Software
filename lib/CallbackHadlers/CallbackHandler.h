#pragma once
#include <vector>
#include <PubSubClient.h>
#include "Callback.h"
extern PubSubClient client;
extern const char *id;
class CallbackHandler
{
private:
  std::vector<Callback *> callbacks;

public:
  void registerCallback(Callback *callback)
  {
    callbacks.push_back(callback);
  }

  void setupAll()
  {
    for (Callback *callback : callbacks)
    {
      callback->setup();
    }
  }

  void loopAll()
  {
    for (Callback *callback : callbacks)
    {
      callback->loop();
    }
  }

  void handleCallback(char *topic, byte *payload, unsigned int length)
  {
    for (Callback *callback : callbacks)
    {
      if (strcmp(topic, callback->getTopic()) == 0)
      {
        callback->handle(topic, payload, length);
        return;
      }
    }
    Serial.println("Unhandled topic: " + String(topic));
  }
};