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

int samples[NUMSAMPLES]; // Array for appending values to
int ArrayIndex;   // Index used to append and sum values from data acquisition
float VAReading;  // Analog voltage reading from ADC
float Reading;    // Sum of the readings in the ArrayIndex
float AvgReading; // Average value of the collected readings
float AvgSensorVoltage; // Converted ADC voltage to Average input voltage from sensor

// Light Sensor Constants
const float Luxk0 = 0; // intercept 0-6000 lux
const float Luxk1 = 1692; // slope lux/V


// Barometric Pressure Constants
//From Original Manual
const float BRk0 = 0.809; //intercept of BR sensor atmospheres
const float BRk1 = 0.077; //slope of BR sensor atmospheres

//float BRk0 = 24.215; // intercept mercury
//float BRk1 = 2.292; // slope mercury


// DHT11 Humidity & Temperature
#define DHTPIN 22
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


//Constants for the Thermistor temperature calculations
// resistance at 298.15 K
const int THERMISTORNOMINAL = 10000;
// temp. for nominal resistance (almost always 298.15 C)
const float TEMPERATURENOMINAL = 298.15;
// how many samples to take and average, more takes longer
// but is more 'smooth'
// The beta coefficient of the thermistor (usually 3000-4000)
const int BCOEFFICIENT = 3950;
// the value of the 'other' resistor
const int SERIESRESISTOR = 9990;

float Resistance;
float InvTemp;
float TempA;
float TempB;
float TempC;


// Reporting Values
float Lux = 0; // Vernier BP sensor with 5V Vcc
float BP = 0; // Vernier BP sensor with 5V Vcc
float SM = 0; // DFRobot corrosion resistant SM sesnor with 5V Vcc
float Humidity = 0; // DHT11 sensor with 5V Vcc
float Temp1 = 0 ; // Thermistor with 3.3V Vcc
float Temp2 = 0; // DHT11 sensor with 5V Vcc

int count = 0;
int MeasurementCycles = 0;

int LEDDAQ = 13;
int LEDSD = 44;

File Evapotranspiration;
const int chipSelect = 53;

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(A0, INPUT); // Analog Lux
  pinMode(A2, INPUT); // Analog BP
  pinMode(A8, INPUT); // Analog SM
  pinMode(DHTPIN, INPUT);  // Digital Humidity
  pinMode(A9, INPUT);// Analog Temp
  pinMode(DHTPIN, INPUT);  // Digital Temp

  pinMode(LEDDAQ, OUTPUT);// Indicator for DAQ cycle
  pinMode(LEDSD, OUTPUT);// Indicator for successful SD card access

  Serial.print("Lux,");
  Serial.print("BP,");
  Serial.print("RSM,");
  Serial.print("RH,");
  Serial.print("T1,");
  Serial.print("T2,");
  Serial.println("Measurement Cycles");
  Serial.flush();

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

}

//Main loop that calls DAQ and sensor functions
/*********************************************/

void loop() {

  String dataString = "";

  if (count == 0) {
    //Serial.print("Entering Light Read: ");
    DAQ();
    VernierLux();
    dataString += String(Lux);
    dataString += ",";
    Serial.print(Lux);
    Serial.print(",");
    Serial.flush();
  }

  if (count == 1) {
    //Serial.print("Entering Barometric Pressure Read: ");
    DAQ();
    VernierBP();
    dataString += String(BP);
    dataString += ",";
    Serial.print(BP);
    Serial.print(",");
    Serial.flush();
  }

  if (count == 2) {
    //Serial.print("Entering Soil Mositure Read: ");
    DAQ();
    DFRobotSM();
    dataString += String(SM);
    dataString += ",";
    Serial.print(SM);
    Serial.print(",");
    Serial.flush();
  }

  if (count == 3) { // DHT11 is a digital sensor and requires different handling
    //Serial.print("Entering Humidity Read: ");
    DHT11Humdity();
    dataString += String(Humidity);
    dataString += ",";
    Serial.print(Humidity);
    Serial.print(",");
    Serial.flush();
  }

  if (count == 4) {
    //Serial.print("Entering Thermistor Read: ");
    DAQ();
    TempThermistor();
    dataString += String(Temp1);
    dataString += ",";
    Serial.print(Temp1);
    Serial.print(",");
    Serial.flush();
  }

  if (count == 5) { // DHT11 is a digital sensor and requires different handling
    //Serial.print("Entering DHT11 Temp Read: ");
    DHT11Temp();
    dataString += String(Temp2);
    dataString += ",";
    Serial.print(Temp2);
    Serial.print(",");
    Serial.flush();
  }

  /*if (count > 5) {
    Serial.println("Fatal Error!");
    return;
    } // end of else statements
  */
  count = 0;
  MeasurementCycles++;
  dataString += String(MeasurementCycles);
  dataString += "\n";

  analogWrite(LEDDAQ, 15);
  delay(1);
  analogWrite(LEDDAQ, 0);
  delay(1);

  Serial.print(MeasurementCycles);
  Serial.println("");
  Serial.flush();

// open the file. *Note that only one file can be open at a time,
  // so you have to close this one before opening another.
  Evapotranspiration = SD.open("Test1.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (Evapotranspiration) {
    Evapotranspiration.println(dataString);
    Evapotranspiration.close();
    // print to the serial port too:
    Serial.println(dataString);

    digitalWrite(LEDSD, HIGH);
    delay(10);
    digitalWrite(LEDSD, LOW);
    delay(10);
    
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening Test1.txt");
  }

  delay(2000);
} // end of void loop

// Generic data collection for sensors
/*********************************************/

int DAQ() {
  // take N samples in a row, with a slight delay
  AvgReading = 0;
  Reading = 0;
  int i;
  AvgSensorVoltage = 0;

  for (i = 0; i < NUMSAMPLES; i++) {
    samples[i] = analogRead(A0);
    delay(10);
  }

  // average all the samples out
  for (i = 0; i < NUMSAMPLES; i++) {
    Reading += samples[i];
  }

  AvgReading = Reading / NUMSAMPLES;
  //Serial.print("\nAverage analog reading: ");
  //Serial.println(AvgReading);

  // convert the value to input voltage
  if (count != 4) {
    AvgSensorVoltage = 5 / (1024 / AvgReading);
  }

  else if (count == 4) {
    AvgSensorVoltage = 3.3 / (1024 / AvgReading);
  }
  //Serial.print("Average Voltage: ");
  //Serial.println(AvgSensorVoltage);

  return AvgSensorVoltage;
}

//Sensor Functions
/*********************************************/

int VernierLux() {
  Lux = Luxk1 * AvgSensorVoltage + Luxk0;
  count++;
  return Lux;
}

int VernierBP() {
  BP = AvgSensorVoltage * BRk1 + BRk0;
  count++;
  return BP;
}

int DFRobotSM() {
  SM = analogRead(A8);
  count++;
  return SM;
}

int DHT11Humdity() { // Sensor is slow and may take up to 250 ms to read humidity
  Humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(Humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    DHT11Humdity();
  }

  count++;
  return Humidity;
}

int TempThermistor() {

  // convert to resistance
  Resistance = SERIESRESISTOR * (AvgSensorVoltage / (3.3 - AvgSensorVoltage));
  //Serial.print("Thermistor resistance: ");
  //Serial.println(Resistance);

  TempA = 1 / (TEMPERATURENOMINAL * 0.001);
  TempB = 1 / (BCOEFFICIENT * 0.001);
  TempC = log(Resistance / SERIESRESISTOR);

  InvTemp = (TempA) + (TempB) * TempC;

  Temp1 = (1 / InvTemp) * 1000;

  count++;
  return Temp1;
}

int DHT11Temp() {
  Temp2 = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(Temp2)) {
    Serial.println("Failed to read from DHT sensor!");
    DHT11Temp();
  }

  count++;
  return Temp2;
}
