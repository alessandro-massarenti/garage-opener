#include <UIPEthernet.h>
#include <PubSubClient.h>

//Compiler instructions

#define RELAY_ON LOW
#define RELAY_OFF HIGH

#define RELAY_BOARD_SIZE 4

#define IN_TOPIC "/home/garage/g_controller/commands"
#define OUT_TOPIC "/home/garage/g_controller/confirmations"
#define ID_CONTROLLER "controller_1"
#define USERNAME "username"
#define PASSWORD "password"

// Update these with values suitable for your network.
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED};
IPAddress server(192, 168, 51, 117);

int relay[RELAY_BOARD_SIZE] = {2, 3, 4, 5};

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

void blinkRelay(int pin)
{
  digitalWrite(pin, RELAY_ON);
  delay(250);
  digitalWrite(pin, RELAY_OFF);
  delay(50);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  int data = (int)payload[0] - 48;

  if (data >= 0 && data < RELAY_BOARD_SIZE)
  {
    blinkRelay(relay[data]);
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(ID_CONTROLLER, USERNAME, PASSWORD))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      mqttClient.publish(OUT_TOPIC, "online");
      // ... and resubscribe
      mqttClient.subscribe(IN_TOPIC);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);

  for (int i = 0; i < RELAY_BOARD_SIZE; i++)
  {
    pinMode(relay[i], OUTPUT);
    digitalWrite(relay[i], RELAY_OFF);
  }

  mqttClient.setServer(server, 1883);
  mqttClient.setCallback(callback);

  Ethernet.begin(mac);
  // Allow the hardware to sort itself out
  delay(500);
}

void loop()
{
  if (!mqttClient.connected())
  {
    reconnect();
  }
  mqttClient.loop();
  delay(20);
}
