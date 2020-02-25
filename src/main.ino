/*
 Publishing in the callback
  - connects to an MQTT server
  - subscribes to the topic "inTopic"
  - when a message is received, republishes it to "outTopic"
  This example shows how to control pins using Ethernet shield via MQTT
*/

#include <SPI.h>
#include <UIPEthernet.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress server(192, 168, 51, 117);

const int lightPin = 2;

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
    
  //turn the LED ON if the payload is '1' and publish to the MQTT server a confirmation message
  if(payload[0] == '2'){
    pulserelay();
    client.publish("outTopic", "Light On"); }
    
   if(payload[0] == '1'){
    digitalWrite(lightPin, HIGH);
    client.publish("outTopic", "Light On"); }
    
  //turn the LED OFF if the payload is '0' and publish to the MQTT server a confirmation message
  if (payload[0] == '0'){
    digitalWrite(lightPin, LOW);
    client.publish("outTopic", "Light Off");
    }
} // void callback


void setup()
{
  pinMode(lightPin, OUTPUT);
  digitalWrite(lightPin, HIGH);
  Serial.begin(9600);
  Serial.println("prova");

  
  Ethernet.begin(mac);

  Serial.print(Ethernet.localIP());
  
  if (client.connect("alessandro")) {
    Serial.println("connesso");
    client.publish("outTopic","hello world");
    client.subscribe("/test/inTopic");
  }
}

void loop()
{
  client.loop();
}

void pulserelay(){
    digitalWrite(lightPin, LOW);
    delay(250);
    digitalWrite(lightPin, HIGH);
}