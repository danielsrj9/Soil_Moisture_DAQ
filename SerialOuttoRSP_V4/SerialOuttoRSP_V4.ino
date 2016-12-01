#include <Wire.h>
#include <SparkFunDS1307RTC.h>

#include <Time.h>
#include <TimeLib.h>

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

#define NUMSAMPLES 50
#define PRINT_USA_DATE

#define SQW_INPUT_PIN 45   // Input pin to read SQW
#define SQW_OUTPUT_PIN 13 // LED to indicate SQW's state#define SQW_INPUT_PIN 45   // Input pin to read SQW

int samples[NUMSAMPLES]; // Array for appending values to
int ArrayIndex;   // Index used to append and sum values from data acquisition
float VAReading;  // Analog voltage reading from ADC
float Reading;    // Sum of the readings in the ArrayIndex
float AvgReading; // Average value of the collected readings
float AvgSensorVoltage; // Converted ADC voltage to Average input voltage from sensor

// Light Sensor Constants
const float Luxk0 = 0; // intercept 0-6000 lux
const float Luxk1 = 1692; // slope lux/V


/*// Barometric Pressure Constants
  //From Original Manual
  const float BRk0 = 0.809; //intercept of BR sensor atmospheres
  const float BRk1 = 0.077; //slope of BR sensor atmospheres

  //float BRk0 = 24.215; // intercept mercury
  //float BRk1 = 2.292; // slope mercury
*/

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

String dataString;

float Lux = 0; // Vernier BP sensor with 5V Vcc
//float BP = 0; // Vernier BP sensor with 5V Vcc
float SM = 0; // DFRobot corrosion resistant SM sesnor with 5V Vcc
float Humidity = 0; // DHT11 sensor with 5V Vcc
float Temp1 = 0 ; // Thermistor with 3.3V Vcc
float Temp2 = 0; // DHT11 sensor with 5V Vcc

int count = 0;
int MeasurementCycles = 0;

int LEDDAQ = 13;
int LEDSD = 44;

// Time Variables
float CurrentTime;

File Evapotranspiration; //Creating file to write data in

const int chipSelect = 53; //for SD card

void setup() {
  Serial.begin(9600);

  //DHT11 sensor
  dht.begin();

  //DS1307 RTC
  pinMode(SQW_INPUT_PIN, INPUT_PULLUP);
  pinMode(SQW_OUTPUT_PIN, OUTPUT);
  digitalWrite(SQW_OUTPUT_PIN, digitalRead(SQW_INPUT_PIN));

  rtc.begin(); // Call rtc.begin() to initialize the library
  // (Optional) Sets the SQW output to a 1Hz square wave.
  // (Pull-up resistor is required to use the SQW pin.)
  rtc.writeSQW(SQW_SQUARE_1);

  // You can use the autoTime() function to set the RTC's clock and
  // date to the compiliers predefined time. (It'll be a few seconds
  // behind, but close!)
  //rtc.autoTime();
  // Or you can use the rtc.setTime(s, m, h, day, date, month, year)
  // function to explicitly set the time:
  // e.g. 7:32:16 | Monday October 31, 2016:
  rtc.setTime(15, 26, 20, 7, 19, 11, 16);  // Uncomment to manually set time
  // rtc.set12Hour(); // Use rtc.set12Hour to set to 12-hour mode

  //Sensor Inputs
  pinMode(A0, INPUT); // Analog Lux
  //  pinMode(A2, INPUT); // Analog BP
  pinMode(A8, INPUT); // Analog SM
  pinMode(DHTPIN, INPUT);  // Digital Humidity
  pinMode(A9, INPUT);// Analog Temp
  pinMode(DHTPIN, INPUT);  // Digital Temp

  // LED Outputs
  pinMode(LEDDAQ, OUTPUT);// Indicator for DAQ cycle
  pinMode(LEDSD, OUTPUT);// Indicator for successful SD card access

  //Header
  //Serial.print("Time,");
  //Serial.print("Lux,");
  //  Serial.print("BP,");
  //Serial.print("RSM,");
  //Serial.print("RH,");
  //  Serial.print("T1,");
  //Serial.print("T2,");
  //Serial.println("Measurement Cycles");
  //Serial.flush();

  //SD Card setup
  //Waiting for the usb port to connect for the SD card
  // Open serial communications and wait for port to open:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    //Serial.println("initialization failed!");
    return;
  }
  //Serial.println("initialization done.");

  if (SD.exists("ECEData.txt")) {
    //Serial.println("ECEData.txt exists.");
  }
  else {
    //Serial.println("ECEData.txt doesn't exist.");
  }

  // open a new file and immediately close it:
  //Serial.println("Creating ECEData.txt...");
  Evapotranspiration = SD.open("ECEData.txt", FILE_WRITE);
  Evapotranspiration.close();

  // Check to see if the file exists:
  if (SD.exists("ECEData.txt")) {
    //Serial.println("ECEData.txt exists.");
  } else {
    //Serial.println("ECEData.txt doesn't exist.");
  }
}


//Main loop that calls DAQ and sensor functions
/*********************************************/
void loop() {

  dataString = "";

  //Time Stamping using DS1307 RTC
  /*********************************************/
  static int8_t lastSecond = -1;

  // Call rtc.update() to update all rtc.seconds(), rtc.minutes(),
  // etc. return functions.
  rtc.update();

  if (rtc.second() != lastSecond) // If the second has changed
  {
    printTime(); // Print the new time

    lastSecond = rtc.second(); // Update lastSecond value
  }

  // Read the state of the SQW pin and show it on the
  // pin 13 LED. (It should blink at 1Hz.)
  digitalWrite(SQW_OUTPUT_PIN, digitalRead(SQW_INPUT_PIN));

  // Sensor Data Aquisition
  /*********************************************/

  //if (count == 0) {
    //Serial.print("Entering Soil Mositure Read: ");
    DAQ();
    //DFRobotSM();
    dataString += String(AvgSensorVoltage);
    dataString += ",";
    Serial.print(AvgSensorVoltage);
    Serial.print(", ");
    Serial.flush();
  //}

/*
  if (count == 1) { // DHT11 is a digital sensor and requires different handling
    //Serial.print("Entering Humidity Read: ");
    DHT11Humdity();
    dataString += String(Humidity);
    dataString += ",";
    Serial.print(Humidity);
    Serial.print(", ");
    Serial.flush();
  }
*/
/*
  if (count == 2) { // DHT11 is a digital sensor and requires different handling
    //Serial.print("Entering DHT11 Temp Read: ");
    DHT11Temp();
    dataString += String(Temp2);
    dataString += ",";
    Serial.print(Temp2);
    Serial.print(", ");
    Serial.flush();
  }
*/
  //count = 0;
  MeasurementCycles++;
  dataString += String(MeasurementCycles);

  digitalWrite(LEDDAQ, HIGH);
  delay(1);
  digitalWrite(LEDDAQ, LOW);
  delay(1);

  Serial.print(MeasurementCycles);
  Serial.print(", ");
  Serial.flush();

  // open the file. *Note that only one file can be open at a time,
  // so you have to close this one before opening another.
  Evapotranspiration = SD.open("ECEData.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (Evapotranspiration) {
    Evapotranspiration.println(dataString);
    Evapotranspiration.close();
    // print to the serial port too:
    //Serial.println(dataString);

    analogWrite(LEDSD, 75);
    delay(10);
    analogWrite(LEDSD, 0);
    delay(10);
  }

  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening ECEData.txt");
  }

  delay(50000);
} // end of void loop


// Time printing function
/*********************************************/
void printTime()
{
  Serial.print(String(rtc.hour()) + ":"); // Print hour
  dataString += String(rtc.hour());
  dataString += String(":");
  if (rtc.minute() < 10)
    Serial.print('0'); // Print leading '0' for minute
  dataString += String('0');
  Serial.print(String(rtc.minute()) + ":"); // Print minute
  dataString += String(rtc.minute());
  dataString += String(":");
  if (rtc.second() < 10)
    Serial.print('0'); // Print leading '0' for second
  dataString += String('0');
  Serial.print(String(rtc.second())); // Print second
  dataString += String(rtc.second());
  dataString += String(", ");

  if (rtc.is12Hour()) // If we're in 12-hour mode
  {
    // Use rtc.pm() to read the AM/PM state of the hour
    if (rtc.pm()) Serial.print(" PM"); // Returns true if PM
    else Serial.print(" AM");
  }

  Serial.print(", ");

  // Few options for printing the day, pick one:
  //Serial.print(rtc.dayStr()); // Print day string
  //dataString += String(rtc.dayStr());
  //Serial.print(rtc.dayC()); // Print day character
  //Serial.print(rtc.day()); // Print day integer (1-7, Sun-Sat)
  //Serial.print(" - ");
  //dataString += String(" - ");
#ifdef PRINT_USA_DATE
  Serial.print(String(rtc.month()) + "/" +   // Print month
               String(rtc.date()) + "/");  // Print date
  dataString += String(rtc.month());
  dataString += String("/");
  dataString += String(rtc.date());
  dataString += String("/");

#else
  Serial.print(String(rtc.date()) + "/" +    // (or) print date
               String(rtc.month()) + "/"); // Print month
#endif
  Serial.print(String(rtc.year()));        // Print year
  dataString += String(rtc.year());

  Serial.print(String(", "));        // Print year
  dataString += String(", ");
}


// Generic data collection for sensors
/*********************************************/

int DAQ() {
  // take N samples in a row, with a slight delay
  AvgReading = 0;
  Reading = 0;
  int i;
  AvgSensorVoltage = 0;

  for (i = 0; i < NUMSAMPLES; i++) {
    samples[i] = analogRead(A8);
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
  AvgSensorVoltage = 5 / (1024 / AvgReading);

  //Serial.print("Average Voltage: ");
  //Serial.println(AvgSensorVoltage);

  return AvgSensorVoltage;
}

//Sensor Functions
/*********************************************/

/*
int VernierLux() {
  Lux = Luxk1 * AvgSensorVoltage + Luxk0;
  count++;
  return Lux;
}
*/
/*
  int VernierBP() {
  BP = AvgSensorVoltage * BRk1 + BRk0;
  count++;
  return BP;
  }
*/

/*
int DFRobotSM() {
  SM = analogRead(A8);
  count++;
  return SM;
}
*/
/*
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
*/
/*
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
*/
/*
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
*/
