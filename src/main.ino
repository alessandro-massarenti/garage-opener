//Compiler instructions
#define relay_board_size = 4
#define RELAY_ON LOW
#define RELAY_OFF HIGH

#include <UIPEthernet.h> // Used for Ethernet

// **** ETHERNET SETTING ****
byte mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0x78, 0xEE};
IPAddress ip(192, 168, 71, 55);

String readString;

int relay[relay_board_size] = {2, 3, 4, 5};
EthernetServer server(80);

void setup()
{
    //Reset relays
    for (int i = 0; i < relay_board_size; i++)
    {
        pinMode(relay[i], OUTPUT);
        digitalWrite(ledPin, RELAY_OFF);
    }

    // start the Ethernet connection and the server:
    Ethernet.begin(mac, ip);
    server.begin();
}

void loop()
{
    // Create a client connection
    EthernetClient client = server.available();
    if (client)
    {
        while (client.connected())
        {
            if (client.available())
            {
                char c = client.read();

                //read char by char HTTP request
                if (readString.length() < 100)
                {
                    //store characters to string
                    readString += c;
                }

                //if HTTP request has ended– 0x0D is Carriage Return \n ASCII
                if (c == 0x0D)
                {
                    client.println("HTTP/1.1 200 OK"); //send new page
                    delay(10);
                    //stopping client
                    client.stop();

                    // control arduino pin
                    if (readString.indexOf("?open") > -1)
                    {
                       pulse_relay();
                    }
                    //clearing string for next read
                    readString = "";
                }
            }
        }
    }
}