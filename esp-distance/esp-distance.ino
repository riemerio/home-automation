#include "Adafruit_VL53L0X.h"
#include <ESP8266WiFi.h>

/**
 * Measuring distances with th VL53LOX sensor
 * offering measurement via GET request
 * 
**/

const char *ssid = "WIFI_NAME";
const char *password = "WIFI_PW";

WiFiServer server(80);
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup()
{
    Serial.begin(115200);

    // Mit dem WiFi-Netzwerk verbinden
    Serial.print("Connecting to WiFi");
    WiFi.hostname("esp-distance2");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    // Lokale IP-Adresse im Seriellen Monitor ausgeben und Server starten
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();

    Serial.println("Adafruit VL53L0X test");
    if (!lox.begin())
    {
        Serial.println(F("Failed to boot VL53L0X"));
        while (1)
            ;
    }
}

void loop()
{
    VL53L0X_RangingMeasurementData_t measure;
    WiFiClient client = server.available();
    // wait for a client (web browser) to connect
    if (client)
    {
        Serial.println("\n[Client connected]");
        while (client.connected())
        {
            // read line by line what the client (web browser) is requesting
            if (client.available())
            {
                String line = client.readStringUntil('\r');
                Serial.print(line);
                // wait for end of client's request, that is marked with an empty line
                if (line.length() == 1 && line[0] == '\n')
                {

                    Serial.print("Reading a measurement... ");
                    lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println();

                    if (measure.RangeStatus != 4)
                    {
                        client.println(measure.RangeMilliMeter);
                    }
                    else
                    {
                        client.println("-");
                    }

                    break;
                }
            }
        }

        while (client.available())
        {
            // but first, let client finish its request
            // that's diplomatic compliance to protocols
            // (and otherwise some clients may complain, like curl)
            // (that is an example, prefer using a proper webserver library)
            client.read();
        }

        // close the connection:
        client.stop();
        Serial.println("[Client disconnected]");
    }

    //delay(1000);
}