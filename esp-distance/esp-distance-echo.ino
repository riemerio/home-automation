

const int pinTrigger = D7;
const int pinEcho = D6;

long dauer = 0;
long entfernungCM = 0;

void setup()
{
    Serial.begin(9600);
    pinMode(pinTrigger, OUTPUT);
    pinMode(pinEcho, INPUT);
}

void loop()
{
    digitalWrite(pinTrigger, LOW);
    delay(5);
    digitalWrite(pinTrigger, HIGH);
    delay(10);
    digitalWrite(pinTrigger, LOW);

    dauer = pulseIn(pinEcho, HIGH);
    entfernungCM = (dauer / 2) * 0.03432;
    if (entfernungCM >= 500 || entfernungCM <= 0)
    {
        Serial.println("Kein Messwert");
    }
    else
    {
        Serial.print(entfernungCM);
        Serial.println(" cm");
    }
    delay(1000);
}