//#include "Adafruit_VL53L0X.h"
#include <ESP8266WiFi.h>
#include <VL53L0X.h>
#include <Wire.h>
/**
 * Measuring distances with th VL53LOX sensor
 * offering measurement via GET request
 * 
**/


const char *ssid = "xxx";
const char *password = "xxx";

WiFiServer server(80);
VL53L0X sensor;
#define HIGH_ACCURACY

void setup()
{
    Serial.begin(115200);
    Wire.begin();

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

    sensor.init();
    sensor.setTimeout(500);

    #if defined LONG_RANGE
        // lower the return signal rate limit (default is 0.25 MCPS)
        sensor.setSignalRateLimit(0.1);
        // increase laser pulse periods (defaults are 14 and 10 PCLKs)
        sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
        sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
    #endif
    #if defined HIGH_SPEED
        // reduce timing budget to 20 ms (default is about 33 ms)
        sensor.setMeasurementTimingBudget(20000);
        #elif defined HIGH_ACCURACY
        // increase timing budget to 200 ms
        sensor.setMeasurementTimingBudget(200000);
    #endif
}

void loop()
{
    //VL53L0X_RangingMeasurementData_t measure;
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
    
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println();
                    
                    client.println(sensor.readRangeSingleMillimeters());
                    if (sensor.timeoutOccurred()) { 
                      Serial.print(" TIMEOUT"); 
                      // if ranges are to near, the value is 65535 -> then this value freezes -> restart
                      ESP.restart();
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
