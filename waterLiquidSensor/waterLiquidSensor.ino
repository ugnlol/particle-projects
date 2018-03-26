
int WaterSensor1Input = D4;
int WaterSensor2Input = D5;

int WaterSensor1 = 0;
int WaterSensor2 = 0;

int RELAY1 = D0;
int RELAY2 = D1;
int RELAY3 = D2;
int RELAY4 = D3;

void setup()
{
    pinMode(WaterSensor1, INPUT_PULLUP);
    pinMode(WaterSensor2, INPUT_PULLUP);

    pinMode(RELAY1, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    pinMode(RELAY3, OUTPUT);
    pinMode(RELAY4, OUTPUT);
    // Initialize all relays to an OFF state
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, LOW);
    digitalWrite(RELAY3, LOW);
    digitalWrite(RELAY4, LOW);
    //Serial.begin(9600);
}

void loop()
{
    WaterSensor1 = digitalRead(WaterSensor1Input);
    WaterSensor2 = digitalRead(WaterSensor2Input);
    if(WaterSensor1 == HIGH)
    {
      digitalWrite(RELAY2, HIGH);
      Particle.publish("rele", "WaterSensor1, HIGH - Turning on Rele2");
    }
    if(WaterSensor2 == HIGH)
    {
      digitalWrite(RELAY4, HIGH);
      Particle.publish("rele", "WaterSensor2, HIGH - Turning on Rele4");
    }
    delay(100);
    digitalWrite(RELAY2, LOW);
    digitalWrite(RELAY4, LOW);
}
