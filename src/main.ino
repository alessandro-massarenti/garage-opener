//Compiler instructions
#define relay_board_size 4
#define RELAY_ON LOW
#define RELAY_OFF HIGH
#define CHIAVE 97

#include <LiquidCrystal_I2C.h>
#include <UIPEthernet.h> // Used for Ethernet

// **** ETHERNET SETTING ****
byte mac[] = {0x00, 0xA2, 0xDA, 0x0D, 0x78, 0xEE};
EthernetServer server(80);
//lcd
int relay[relay_board_size] = {2, 3, 4, 5};
LiquidCrystal_I2C lcd(0x27, 20, 4);

//variable
String readString;
int prime = 0;
int base = 0;

void setup()
{

    reset_relay();

    Serial.begin(9600);

    // start the Ethernet connection and the server:
    Ethernet.begin(mac);
    server.begin();

    lcd.init();
    lcd.backlight();

    show_info();
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

                    if (readString.indexOf("prime=") > -1 && readString.indexOf("base=") > -1)
                    {
                        prime = readString.substring(readString.indexOf("prime=") + 5, readString.indexOf("&")).toInt();
                        base = readString.substring(readString.indexOf("base=") + 5, readString.indexOf("&", readString.indexOf("base="))).toInt();

                        Serial.println(prime);
                        Serial.println(base);
                        Serial.println(pubkey(prime, base));
                    }

                    lcd.setCursor(0, 3);
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

int pubkey(int prime, int base)
{
    return (CHIAVE * base) % prime;
}

int sharekey(int prime, int base, int chiave)
{
    return (chiave * CHIAVE) % prime;
}

void reset_relay()
{
    //Reset relays
    for (int i = 0; i < relay_board_size; i++)
    {
        pinMode(relay[i], OUTPUT);
        digitalWrite(relay[i], RELAY_OFF);
    }
    for (int i = 0; i < relay_board_size; i++)
    {
        pulse_relay(relay[i]);
    }
}

void show_info()
{
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

/*

// control arduino pin
                    if (readString.indexOf("?opengate") > -1)
                    {
                        pulse_relay(relay[0]);
                    }
                    else if (readString.indexOf("?opendoor") > -1)
                    {
                        pulse_relay(relay[1]);
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
                        readString = readString.substring(readString.indexOf("?key") + 5, readString.indexOf("end"));
                        char caratteri[20];
                        readString.toCharArray(caratteri, 20);

                        char *roba = XORENC(caratteri, "52546");

                        lcd.print(pad(roba, 20));
                        Serial.println(roba);
                    }

                    */