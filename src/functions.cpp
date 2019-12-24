#define RELAY_ON LOW
#define RELAY_OFF HIGH

#include "functions.h"

void pulse_relay(int relay)
{
    digitalWrite(relay, RELAY_ON);
    delay(500);
    digitalWrite(relay, RELAY_OFF);
}
