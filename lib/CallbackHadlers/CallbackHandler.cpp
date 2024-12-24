#include "CallbackHandler.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "../../include/Credentials.h"

// Array of devices and their associated callbacks
DeviceHandler deviceHandlers[10];
int deviceCount = 0;

// Register a device and its callback
void registerDevice(const char *topic, CallbackFunction callback)
{
  if (deviceCount < 10)
  {
    deviceHandlers[deviceCount].topic = topic;
    deviceHandlers[deviceCount].callback = callback;
    deviceCount++;
  }
}

// Callback function for handling messages
void handleCallback(char *topic, byte *payload, unsigned int length)
{
  for (int i = 0; i < deviceCount; i++)
  {
    // Check if the device ID matches the topic
    if (strcmp(topic, deviceHandlers[i].topic) == 0)
    {
      // Call the corresponding callback function
      deviceHandlers[i].callback(topic, payload, length);
      break;
    }
  }
}
