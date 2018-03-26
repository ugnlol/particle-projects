// This #include statement was automatically added by the Particle IDE.
#include "OneWire/OneWire.h"
// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature/spark-dallas-temperature.h"

#define ONE_WIRE_BUS D4

int RELAY1 = D0;
int RELAY2 = D1;
int RELAY3 = D2;
int RELAY4 = D3;

int dosePmdd(String doseTime);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

double tempC = 0.0;

/*
These are the ROM codes of my 3 sensors:
    Sensor# 1 = 28FFF76380160361
    Sensor# 2 = 28FF8F4964160300
*/    

void setup()
{
   sensors.begin();

   //Initilize the relay control pins as output
   pinMode(RELAY1, OUTPUT); //KNO3
   pinMode(RELAY2, OUTPUT); //KH2PO4
   pinMode(RELAY3, OUTPUT); //K2SO4 + Micro
   pinMode(RELAY4, OUTPUT); //Heat element
   // Initialize all relays to an OFF state
   digitalWrite(RELAY1, LOW);
   digitalWrite(RELAY2, LOW);
   digitalWrite(RELAY3, LOW);
   digitalWrite(RELAY4, LOW);
   //digitalWrite(RELAY3, (state) ? LOW : HIGH); //if low, switch to high
   //digitalWrite(RELAY4, (state) ? HIGH : LOW); //if high, switch to low
   pinMode(7, OUTPUT);

   Particle.variable("tempc", tempC);
   Particle.subscribe("hook-response/temperature", myHandler, MY_DEVICES);

   Particle.function("motor", toggle);
   Particle.function("pmdd", dosePmdd);
}

void loop()
{
  sensors.requestTemperatures();
  double tempCheck = sensors.getTempCByIndex(0);

  if ( tempCheck > -1 && tempCheck < 100) // might want to guard against a spurious high value as well
  {
    tempC = tempCheck;
    Particle.publish("temperature", String(tempC), PRIVATE);
  }
  //600000 = 10 min
  delay(5000);
}

int toggle(String command)
{
  if (command=="on") {
      digitalWrite(RELAY1, HIGH);
      return 1;
  }
  else if (command=="off") {
      digitalWrite(RELAY1,LOW);
      return 0;
  }
  else {
      return -1;
  }
}

void myHandler(const char *event, const char *data)
{
  // Handle the integration response
}

int dosePmdd(String doseTime)
{
  Particle.publish("pmdd", "dosing started");

  int doseTimeInt = atoi(doseTime);
  //check if any relays are on
  if(digitalRead(RELAY1) == LOW && digitalRead(RELAY2) == LOW && digitalRead(RELAY3) == LOW)
  {
    if(doseTimeInt>1000&&doseTimeInt<20000)
    {
      //turn on relays
      Particle.publish("dosing", "All relays are off. Starting to dose now!");
      digitalWrite(RELAY1, HIGH);
      digitalWrite(RELAY2, HIGH);
      digitalWrite(RELAY3, HIGH);
      //use variable passed on to the function
      delay(doseTimeInt);
      //turn off relay
      digitalWrite(RELAY1, LOW);
      digitalWrite(RELAY2, LOW);
      digitalWrite(RELAY3, LOW);
      Particle.publish("pmdd", "dosing finised");
    }
    else
    {
      Particle.publish("dosing", "I'm too cool to dose for that short period of time.");
    }

  }
  else
  {
      Particle.publish("dosing", "Can't starte before i'm finished!");
  }
}
