

/*
  This a simple example of the aREST Library for the ESP8266 WiFi chip.
  See the README file for more details.

  Written in 2015 by Marco Schwartz under a GPL license.
*/

// Import required libraries
#include <ESP8266WiFi.h>
#include <aREST.h>
#include <DHT.h>

#define DHT_TYPE DHT22
const int DHT_PIN = D1;
DHT dht(DHT_PIN, DHT_TYPE);

// Create aREST instance
aREST rest = aREST();

// WiFi parameters
const char *ssid = "WIFI_NAME";
const char *password = "WIFI_PW";

// The port to listen for incoming TCP connections
#define LISTEN_PORT 80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Variables to be exposed to the API
int temperature;
int humidity;
float t;

// Declare functions to be exposed to the API
int ledControl(String command);

void setup(void)
{
    // Start Serial
    Serial.begin(9600);

    // Init variables and expose them to REST API
    temperature = 24;
    humidity = 40;

    //t = dht.readTemperature();

    rest.variable("temperature", &t);
    rest.variable("humidity", &humidity);

    // Function to be exposed
    rest.function("led", ledControl);
    rest.function("temp", getTemp);

    // Give name & ID to the device (ID should be 6 characters long)
    rest.set_id("1");
    rest.set_name("esp8266");

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    // Start the server
    server.begin();
    Serial.println("Server started");

    // Print the IP address
    Serial.println(WiFi.localIP());
}

void loop()
{

    // Handle REST calls
    WiFiClient client = server.available();
    if (!client)
    {
        return;
    }
    while (!client.available())
    {
        delay(1);
    }
    rest.handle(client);
}

int getTemp(String x)
{
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    Serial.print("Temperatur: ");
    Serial.print(t);
    Serial.print("°C, Luftfeuchtigkeit: ");
    Serial.print(h);
    Serial.println("%");
    return (int)t;
}

// Custom function accessible by the API
int ledControl(String command)
{

    // Get state from command
    int state = command.toInt();

    digitalWrite(D0, state);
    return 1;
}