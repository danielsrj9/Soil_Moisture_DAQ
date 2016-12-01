//Ryan J. Daniels
//ECE 555 Term Project
//Watering Soil Prediction Project
//Fall 2016

// Libraries for ethernet, SD, DHT11 sensors, and SPI
#include <Wire.h>
#include <SparkFunDS1307RTC.h>

#include <Time.h>
#include <TimeLib.h>

#include <Adafruit_Sensor.h>
#include <SparkFunVernier.h>

#include <DHT.h>
#include <DHT_U.h>
#include <SPI.h>
#include <SD.h>

#define SAMPLENUMBER 50        // Sample number for averaged SM data
#define AirValue 520    // Sensor upper limit
#define dryBoundary 430 // Boundary from wet to dry
#define wetBoundary 350 // Boundary from very wet to wet
#define WaterValue 260  // Sensor lower limi

int intervals = (AirValue - WaterValue) / 3;
int WetBoundaryTime;
int DryBoundaryTime;

#define PRINT_USA_DATE
#define SQW_INPUT_PIN 45   // Input pin to read SQW
#define SQW_OUTPUT_PIN 13 // LED to indicate SQW's state#define SQW_INPUT_PIN 45   // Input pin to read SQW

// DHT11 Humidity & Temperature
#define DHTPIN 22
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Reporting Values

String dataString;
File Evapotranspiration; //Creating file to write data in

const int chipSelect = 53; //for SD card

int MeasurementCycle = 1;
int i;
long int y1[SAMPLENUMBER];
float y1Sum;
float y1Avg;

int LEDDAQ = 44;
//int LEDSD = 44;

void setup() {
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  pinMode(A0, INPUT);
  //Serial.println("Time Stamp, Avg. Soil Moisture, Soil Condition");

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
  rtc.setTime(0, 45, 9, 2, 21, 11, 16);  // Uncomment to manually set time
  //rtc.set12Hour(); // Use rtc.set12Hour to set to 12-hour mode


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

  // LED Outputs
  pinMode(LEDDAQ, OUTPUT);// Indicator for DAQ cycle
  //pinMode(LEDSD, OUTPUT);// Indicator for successful SD card access
}

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


  //Sensor reading
  Y1Reading();

  // open the file. *Note that only one file can be open at a time,
  // so you have to close this one before opening another.
  Evapotranspiration = SD.open("ECEData.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (Evapotranspiration) {
    Evapotranspiration.println(dataString);
    Evapotranspiration.close();
    // print to the serial port too:
    //Serial.println(dataString);

    digitalWrite(LEDDAQ, HIGH);
    delay(10);
    digitalWrite(LEDDAQ, LOW);
    delay(10);
  }

  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening ECEData.txt");
  }
  delay(50000);
}

// Time printing function
/*********************************************/
void printTime(){
  
  Serial.print(String(rtc.hour()) + ": "); // Print hour
  dataString += String(rtc.hour());
  dataString += String(": ");
  if (rtc.minute() < 10)
    Serial.print('0'); // Print leading '0' for minute
  dataString += String('0');
  Serial.print(String(rtc.minute()) + ": "); // Print minute
  dataString += String(rtc.minute());
  dataString += String(":");
  if (rtc.second() < 10)
    Serial.print('0'); // Print leading '0' for second
  dataString += String('0');
  Serial.print(String(rtc.second())); // Print second
  dataString += String(rtc.second());

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


// Soil Moisture Measurement and Soil Condition
/*********************************************/
void Y1Reading() {
  for (i = 0; i <= SAMPLENUMBER; i++) { //take N samples and append to y1
    y1[i] = analogRead(A8);
  }

  for (i = 0; i < SAMPLENUMBER; i++) { //sum the values in y1
    y1Sum += y1[i]; 
  }

  y1Avg = y1Sum / SAMPLENUMBER; // average SM value for y1
  //Serial.print("Avg y1 value: ");
  
  Serial.print(y1Avg);
  dataString += String(y1Avg);

  Serial.print(", ");
  Serial.print(MeasurementCycle);
  dataString += String(MeasurementCycle);
  MeasurementCycle++;


  if (y1Avg > WaterValue && y1Avg < (WaterValue + intervals))
  {
    Serial.print(", ");
    dataString += String(",");
    Serial.println("Very Wet");
    dataString += String("Very Wet");
  }
  else if (y1Avg > (WaterValue + intervals) && y1Avg < (AirValue - intervals))
  {
    Serial.print(", ");
    dataString += String(",");
    Serial.println("Wet");
    dataString += String("Wet");
  }
  else if (y1Avg < AirValue || y1Avg > AirValue && y1Avg > (AirValue - intervals))
  {
    Serial.print(", ");
    dataString += String(",");
    Serial.println("Dry");
    dataString += String("Dry");
  }

  y1Sum = 0;
  y1Avg = 0;
  return;
}
