#include "CallbackHandler.h"
#include "../../include/Credentials.h"

void callbackOnOff(char *topic, byte *payload, unsigned int length)
{
  Serial.println("Callback OnOff");
  char payloadString[length];
  for (unsigned int i = 0; i < length; i++)
  {
    payloadString[i] = (char)payload[i];
  }
  payloadString[length] = '\0';

  if (strcmp(payloadString, "true") == 0)
  {
    Serial.println("Turning on relay");
    digitalWrite(RELAY_PIN, LOW);
  }
  else
  {
    Serial.println("Turning off relay");
    digitalWrite(RELAY_PIN, HIGH);
  }
}