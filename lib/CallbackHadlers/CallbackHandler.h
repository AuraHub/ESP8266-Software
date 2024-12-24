#ifndef CALLBACKHANDLER_H
#define CALLBACKHANDLER_H

#include <PubSubClient.h>
extern PubSubClient client;
extern const char *id;

// Define the callback types for different handlers
typedef void (*CallbackFunction)(char *topic, byte *payload, unsigned int length);

// Structure to map devices to their callback functions
struct DeviceHandler
{
  const char *topic;
  CallbackFunction callback;
};

// Function to register a device and its callback
void registerDevice(const char *topic, CallbackFunction callback);

// Function to handle the callback for a specific topic
void handleCallback(char *topic, byte *payload, unsigned int length);
void callbackPing(char *topic, byte *payload, unsigned int length);
void callbackOnOff(char *topic, byte *payload, unsigned int length);

#endif