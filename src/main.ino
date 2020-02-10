//Compiler instructions
#define relay_board_size 4
#define RELAY_ON LOW
#define RELAY_OFF HIGH

#include <LiquidCrystal_I2C.h>
#include <UIPEthernet.h> // Used for Ethernet

// **** ETHERNET SETTING ****
byte mac[] = {0x00, 0xA2, 0xDA, 0x0D, 0x78, 0xEE};

String readString;
String string = "";

int relay[relay_board_size] = {2, 3, 4, 5};

LiquidCrystal_I2C lcd(0x27, 20, 4);
EthernetServer server(80);

void setup()
{

    //Reset relays
    for (int i = 0; i < relay_board_size; i++)
    {
        pinMode(relay[i], OUTPUT);
        digitalWrite(relay[i], RELAY_OFF);
    }

    Serial.begin(9600);
    Serial.println("hello Wordld");
    // start the Ethernet connection and the server:
    Ethernet.begin(mac);
    server.begin();

    Ethernet.begin(mac);
    server.begin();

    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("IP: ");
    lcd.print(Ethernet.localIP());
    lcd.setCursor(0, 1);
    lcd.print("Mask: ");
    lcd.print(Ethernet.subnetMask());
    lcd.setCursor(0, 2);
    lcd.print("GwIP:");
    lcd.print(Ethernet.gatewayIP());
    lcd.setCursor(0, 3);
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
                    if (readString.indexOf("?opengate") > -1)
                    {
                        string = "opengate";
                    }
                    else if (readString.indexOf("?opendoor") > -1)
                    {
                        string = "opendoor";
                    }
                    else if (readString.indexOf("?open3") > -1)
                    {
                        pulse_relay(relay[2]);
                    }
                    else if (readString.indexOf("?open4") > -1)
                    {
                        pulse_relay(relay[3]);
                    }
                    else if (readString.indexOf("?key") > -1)
                    {
                        readString = readString.substring(readString.indexOf("?key")+5,readString.indexOf("end"));
                        char caratteri[20];
                        readString.toCharArray(caratteri,20);
                        lcd.print(pad(XORENC(caratteri,"supercazzola"),20));
                    }


                    lcd.setCursor(0, 3);

                    readString = pad(readString, 20);

                    Serial.println(readString);
                    //clearing string for next read
                    readString = "";
                }
            }
        }
    }

    delay(100);
}

String pad(String str, int size)
{
    int strsize = str.length();
    size = size - strsize;
    for (int i = 0; i < size; i++)
    {
        str += " ";
    }
    return str;
}

char *XORENC(char *in, char *key)
{
    // Brad @ pingturtle.com
    int insize = strlen(in);
    int keysize = strlen(key);
    for (int x = 0; x < insize; x++)
    {
        for (int i = 0; i < keysize; i++)
        {
            in[x] = (in[x] ^ key[i]) ^ (x * i);
        }
    }
    return in;
}

void pulse_relay(int relay)
{
    digitalWrite(relay, RELAY_ON);
    delay(500);
    digitalWrite(relay, RELAY_OFF);
}