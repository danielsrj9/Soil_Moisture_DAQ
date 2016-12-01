/***************************************************
  This example reads Capacitive Soil Moisture Sensor.

  Created 2015-10-21
  By berinie Chen <bernie.chen@dfrobot.com>

  GNU Lesser General Public License.
  See <http://www.gnu.org/licenses/> for details.
  All above must be included in any redistribution
 ****************************************************/

/***********Notice and Trouble shooting***************
  1.Connection and Diagram can be found here: https://www.dfrobot.com/wiki/index.php?title=Capacitive_Soil_Moisture_Sensor_SKU:SEN0193
  2.This code is tested on Arduino Uno.
  3.Sensor is connect to Analog 0 port.
 ****************************************************/
#define count 10        // Sample number for averaged SM data
#define AirValue 520    // Sensor upper limit
#define dryBoundary 430 // Boundary from wet to dry
#define wetBoundary 350 // Boundary from very wet to wet
#define WaterValue 260  // Sensor lower limit

int intervals = (AirValue - WaterValue) / 3;
int soilMoistureValue = 0;
int WetBoundaryTime;
int DryBoundaryTime;
int i;

int y1[count];
float y1Sum;
float y1Avg;
long int y1Time;

int y2[count];
float y2Sum;
float y2Avg;
long int y2Time;

float SMSlope;

int StartTime;
long int RunTime;

void setup() {
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  pinMode(A0, INPUT);
}

void loop() {

  // StartTime = millis();

  // while (RunTime < 5200) {
  //   RunTime = millis() - StartTime;

  //   if (RunTime == 500 && y1Avg == 0) {
  Serial.println("Entering y1");
  Y1Reading();
  //   }

  /*   else if (RunTime == 5000 && y2Avg == 0) {
       Serial.println("Entering y2");
       Y2Reading();
     }

     else if (RunTime == 5100) {
       Serial.println("Entering summary");
       summary();
     }
    }*/
}

void Y1Reading() {
  for (i = 0; i <= count; i++) { //take N samples and append to y1
    y1[i] = analogRead(A0);
  }
  y1Time = millis();

  for (i = 0; i < count; i++) { //sum the values in y1
    y1Sum += y1[i];
    Serial.print(y1[i]);
    Serial.print(", ");
  }

  Serial.println("");
  Serial.println(y1Sum);

  y1Avg = y1Sum / count; // average SM value for y1
  Serial.print("Avg y1 value: ");
  Serial.println(y1Avg);

  if (y1Avg > WaterValue && y1Avg < (WaterValue + intervals))
  {
    Serial.print("Very Wet: ");
    Serial.println(y1Avg);
    Serial.println("");
  }
  else if (y1Avg > (WaterValue + intervals) && y1Avg < (AirValue - intervals))
  {
    Serial.print("Wet: ");
    Serial.println(y1Avg);
    Serial.println("");
  }
  else if (y1Avg < AirValue && y1Avg > (AirValue - intervals))
  {
    Serial.print("Dry: ");
    Serial.println(y1Avg);
    Serial.println("");
  }

  y1Sum = 0;
  y1Avg = 0;
  delay(2000);
  //RunTime = millis() - StartTime;
  return;
}

/*
  void Y2Reading() {
  for (i = 0; i <= count; i++) {
    y2[i] = analogRead(A0);
  }
  y2Time = millis();

  for (i = 0; i < count; i++) { //sum the values in y2
    y2Sum += y2[i];
    Serial.print(y2[i]);
    Serial.print(", ");
  }

  Serial.println("");
  Serial.println(y2Sum);

  y2Avg = y2Sum / count; // average SM value for y2
  Serial.print("Avg y2 value: ");
  Serial.println(y2Avg);
  Serial.println("");

  //RunTime = millis() - StartTime;
  return;
  }
*/
void summary() {
  soilMoistureValue = analogRead(A0);  //Insert sensor into soil

  if (soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
  {
    Serial.print("Very Wet: ");
    Serial.println(soilMoistureValue);
  }
  else if (soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
  {
    Serial.print("Wet: ");
    Serial.println(soilMoistureValue);
  }
  else if (soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
  {
    Serial.print("Dry: ");
    Serial.println(soilMoistureValue);
  }

  // Watering Prediction
  /*
    SMSlope = (y2Avg - y1Avg) / (y2Time - y1Time); // Rate change of soil moisture content

    if (y2Avg < wetBoundary) { // Time prediction for SM to reach wet and dry boundary if very wet
      WetBoundaryTime = (y2Avg - (soilMoistureValue - wetBoundary)) / SMSlope;
      Serial.print("Hours until soil is not over watered: ");
      Serial.print(WetBoundaryTime / 1000);

      DryBoundaryTime = (y2Avg - (soilMoistureValue - dryBoundary)) / SMSlope;
      Serial.print("\tTime until soil needs to be watered: ");
      Serial.println(DryBoundaryTime / 1000);
      Serial.println("");
    }

    else if (y2Avg < dryBoundary) { // Time prediction for Sm to reach dry boundary if wet
      DryBoundaryTime = (y2Avg - (soilMoistureValue - dryBoundary)) / SMSlope;
      Serial.print("\tHours until soil needs to be watered: ");
      Serial.println(DryBoundaryTime / 1000);
      Serial.println("");
    }

    else if (y2Avg > dryBoundary) {
      Serial.println("Soil needs to be watered!");
      Serial.println("");
    }
  */
  // StartTime = millis();
  // RunTime = 0;
  y1Sum = 0;
  // y2Sum = 0;
  y1Avg = 0;
  // y2Avg = 0;
  delay(5000);
  return;
}

