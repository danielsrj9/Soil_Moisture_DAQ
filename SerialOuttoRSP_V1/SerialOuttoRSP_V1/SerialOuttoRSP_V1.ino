// Libraries for ethernet, SD, DHT11 sensors, and SPI

#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>

#include <Adafruit_Sensor.h>
#include <SparkFunVernier.h>
#include <DHT.h>
#include <DHT_U.h>
#include <SPI.h>
#include <SD.h>

//Common variables for processing analog and digital signals

#define NUMSAMPLES 10

float samples[NUMSAMPLES]; // Array for appending values to
int ArrayIndex;   // Index used to append and sum values from data acquisition
float VAReading;  // Analog voltage reading from ADC
float Reading;    // Sum of the readings in the ArrayIndex
float AvgReading; // Average value of the collected readings
float AvgVoltage; // Converted ADC voltage to Average input voltage from sensor

//Constants for the Thermistor temperature calculations

// resistance at 298.15 K
#define THERMISTORNOMINAL 10000
// temp. for nominal resistance (almost always 298.15 C)
#define TEMPERATURENOMINAL 298.15
// how many samples to take and average, more takes longer
// but is more 'smooth'
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 9990

int Light = 0;
int BP = 0;
int SM = 0;
int Temp1 =0 ;
int Temp2 = 0;
int Humidity = 0;

int count = 0;
int MeasurementCycles = 0;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A2, INPUT);
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);
  pinMode(A10, INPUT);
  pinMode(A11, INPUT);

  Serial.print("Lux,");
  Serial.print("BP,");
  Serial.print("T1,");
  Serial.print("RSM,");
  Serial.print("RH,");
  Serial.print("T2,");
  Serial.println("Measurement Cycles");
  Serial.flush();
}

void loop() {

  if (count == 0) {
    //Serial.print("Entering Light Read: ");
    VernierLux();
    Serial.print(Light);
    Serial.print(",");
    Serial.flush();
  }

  if (count == 1) {
    //Serial.print("Entering Barometric Pressure Read: ");
    VernierBP();
    Serial.print(BP);
    Serial.print(",");
    Serial.flush();
  }

  if (count == 2) {
    //Serial.print("Entering Soil Mositure Read: ");
    DFRobotSM();
    Serial.print(SM);
    Serial.print(",");
    Serial.flush();
  }
  
  if (count == 3) {
    //Serial.print("Entering Humidity Read: ");
    DHT11Humdity();
    Serial.print(Humidity);
    Serial.print(",");
    Serial.flush();
  }
  
  if (count == 4) {
    //Serial.print("Entering Thermistor Read: ");
    TempThermistor();
    Serial.print(Temp1);
    Serial.print(",");
    Serial.flush();
  }

  if (count == 5) {
    //Serial.print("Entering DHT11 Temp Read: ");
    DHT11Temp();
    Serial.print(Temp2);
    Serial.print(",");
    Serial.flush();
  }
  count = 0;
  MeasurementCycles++;

  Serial.print(MeasurementCycles);
  Serial.println("");
  Serial.flush();
    
  delay(2000);
}


int VernierLux() {
  Light = analogRead(A0);
  count++;
  return Light;
}

int VernierBP() {
  BP = analogRead(A2);
  count++;
  return Light;
}

int DFRobotSM() {
  SM = analogRead(A8);
  count++;
  return SM;
}

int TempThermistor() {
  Temp1 = analogRead(A9); // Read temperature from the analog pin 0 connected to the thermistor
  count++;
  return Temp1;
}

int DHT11Temp() {
  Temp2 = analogRead(A11);
  count++;
  return Temp2;
}

int DHT11Humdity() {
  Humidity = analogRead(A10);
  count++;
  return Humidity;
}
