#include functions.h

void pulse_relay(int relay)
{
    digitalWrite(relay, RELAY_ON);
    delay(500);
    digitalWrite(relay, RELAY_OFF);
}
