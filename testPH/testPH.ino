/*
  28/8/2015  Michael Ratcliffe  Mike@MichaelRatcliffe.com


    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.


    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 11 //Data wire For Temp Probe is plugged into pin 11 on the Arduino

OneWire oneWire(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.


//*********************** User defined variables ****************************//
int PHPin = A0;                               //pH meter Analog output to Arduino Analog Input 0

float K_PH = 3.64;                            //The calibration constant for the PH probe
//used for min/max logs
float MinPH = 10;
float MaxPH = 0;


//************** Some values for working out the ph & temp *****************//
                      
const int TempProbePossitive = 12;            //Temp Probe power connected to pin 12
const int TempProbeNegative = 13;             //Temp Probe Negative connected to pin 13

const int NumReadings = 10;                    // number of reading for LM35
int Index = 0;                                 // index
int TempReadings[NumReadings];                 // array for store LM35 readings
int TempTotal = 0;                             // LM35 running total
int TempAverage = 20;                           // LM35 average reading
double TempValue = 20;                          // LM35 Temperature Data in Human Reading Format after calculation
float MinT = 100;
float MaxT = 0;
float Temperature = 0.0;

int PhReadings[NumReadings];                   // array for store PH readings
int PhTotal = 0;                               // PH running total
int PhAverage = 0;                             // PH average reading

double Ph7Buffer = 7.60;                       // For PH7 buffer solution's PH value , 7 or 6.86
double Ph4Buffer = 4.01;                       // For PH4 buffer solution's PH value , 4 or 4.01

double Ph7Reading = 550;                       // PH7 Buffer Solution Reading.
double Ph4Reading = 655;                       // PH4 Buffer Solution Reading.

double PhRatio = 0;                            // PH Step
double PhValue = 0;                            // Ph Value in Human Reading Format after calculation


//********************** End Of Recomended User Variables ******************//


//************************** Just Some basic Definitions used for the Up Time LOgger ************//
long Day = 0;
int Hour = 0;
int Minute = 0;
int Second = 0;
int HighMillis = 0;
int Rollover = 0;


//************************************** Setup Loop Runs Once ****************//
void setup()
{
  Serial.begin(9600);
  pinMode(TempProbeNegative , OUTPUT ); //seting ground pin as output for tmp probe
  digitalWrite(TempProbeNegative , LOW );//Seting it to ground so it can sink current
  pinMode(TempProbePossitive , OUTPUT );//ditto but for positive
  digitalWrite(TempProbePossitive , HIGH );
  read_Temp();// getting rid of the first bad reading
 
  PhRatio = (Ph4Reading - Ph7Reading) / (Ph7Buffer - Ph4Buffer);
}
//******************************** End Of Setup **********************************//


//******************** Main Loops runs Forver ************************************//
void loop()
{
  //All these functions are put below the main loop, keeps the loop logic easy to see
  read_Temp();
  Log_Min_MaxTemp();
  ReadPH();
  Log_Min_MaxPH();
  uptime();
  PrintReadings();
  delay(100);
};


//************************** End Of Main Loop ***********************************//


//************** Function to read the external (d18b20) temperature **************//
void read_Temp() 
{
  sensors.requestTemperatures();// Send the command to get temperatures
  Temperature = sensors.getTempCByIndex(0); //Stores Value in Variable
}


//*************************Take Ten Readings And Average ****************************//
void ReadPH() 
{
  // Samplin LM35 and PH Value
  TempTotal = TempTotal - TempReadings[Index];   // subtract the last reading:
  PhTotal = PhTotal - PhReadings[Index];         // subtract the last reading:
  TempReadings[Index] = analogRead(A3);           // read from the sensor : LM35
  PhReadings[Index] = analogRead(A0);             // read from the sensor : PH
  TempTotal = TempTotal + TempReadings[Index];   // add the reading to the temperature total:
  PhTotal = PhTotal + PhReadings[Index];         // add the reading to the ph total:
  Index = Index + 1;                             // advance to the next position in the array:

  if (Index >= NumReadings)                     // if we're at the end of the array...
  {                    
    Index = 0;                                     // ...wrap around to the beginning:
    TempAverage = TempTotal / NumReadings;         // calculate the average:
    PhAverage = PhTotal / NumReadings;             // calculate the average:
  }

  TempValue = (double) TempAverage / 3.4  * (5 / 10.24); // LM35 connect to CA3140 for amplify 3 time
  PhValue = (Ph7Reading - PhAverage) / PhRatio + Ph7Buffer;    // Calculate PH

  //************Use this to debug and adjust calibration values for the probe***********//
  //Serial.print("rawValue: ");
  //Serial.println(analogRead(A0));

  delay(1000);
}


//************************ Uptime Code - Makes a count of the total up time since last start ****************//
void uptime() 
{
  //** Making Note of an expected rollover *****//
  if (millis() >= 3000000000) 
  {
    HighMillis = 1;
  }
  //** Making note of actual rollover **//
  if (millis() <= 100000 && HighMillis == 1) 
  {
    Rollover++;
    HighMillis = 0;
  }

  long secsUp = millis() / 1000;

  Second = secsUp % 60;
  Minute = (secsUp / 60) % 60;
  Hour = (secsUp / (60 * 60)) % 24;
  Day = (Rollover * 50) + (secsUp / (60 * 60 * 24)); //First portion takes care of a rollover [around 50 days]
};


//******************************* LOGS Min/MAX Values*******************************//
void Log_Min_MaxTemp() 
{
  if (Temperature >= MaxT) MaxT = Temperature;
  if (Temperature <= MinT) MinT = Temperature;
};


//******************************* LOGS Min/MAX Values*******************************//
void Log_Min_MaxPH() 
{
  if (PhValue >= MaxPH) MaxPH = PhValue;
  if (PhValue <= MinPH) MinPH = PhValue;
};


//******************************* Prints stuff *******************************//
void PrintReadings() 
{
  Serial.print("pH: ");
  Serial.println(PhValue);
  Serial.print("Ext: ");
  Serial.print(Temperature);
  Serial.println(" *C  ");
  Serial.print("Int: ");
  Serial.print(TempValue);
  Serial.println(" *C  ");
  Serial.println("-------");
  delay(2000);
};
