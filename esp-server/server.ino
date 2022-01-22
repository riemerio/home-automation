#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <uri/UriBraces.h>

#ifndef STASSID
#define STASSID "WIFI_NAME"
#define STAPSK "WIFI_PW"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);

const int led = 13;

void handleRoot()
{
    digitalWrite(led, 1);
    server.send(200, "text/plain", "hello from garden controller!");
    digitalWrite(led, 0);
}

void ledOn()
{
    int pin = D0;
    setMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    server.send(200, "text/plain", pin + " on");
}

void ledOff()
{
    int pin = D0;
    setMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    server.send(200, "text/plain", pin + " off");
}

void handleNotFound()
{
    digitalWrite(led, 1);
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
    digitalWrite(led, 0);
}

void setup(void)
{

    pinMode(led, OUTPUT);
    pinMode(D0, OUTPUT);
    digitalWrite(led, LOW);
    digitalWrite(D0, LOW);
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // prepare data pins
    int pins[] = {D0, D1, D2, D3, D4, D5, D6, D7, D8};

    for (int i = 0; i < 9; i++)
    {
        Serial.println("setting pin modes for pins to >output<");
        Serial.println(pins[i]);
        pinMode(pins[i], OUTPUT);
        digitalWrite(pins[i], 0);
    }

    Serial.println("setup");

    if (MDNS.begin("esp8266"))
    {
        Serial.println("MDNS responder started");
    }

    server.on("/", handleRoot);

    server.on("/inline", []()
              { server.send(200, "text/plain", "this works as well"); });

    server.on(UriBraces("/pins/{}"), []()
              {
                  String pin = server.pathArg(0);
                  server.send(200, "text/plain", "Pin: '" + pin + "'");
              });

    server.on("/led-on", ledOn);

    server.on("/led-off", ledOff)

        server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");
}

void loop(void)
{
    server.handleClient();
    MDNS.update();
}