// resistance at 298.15 K
#define THERMISTORNOMINAL 10000
// temp. for nominal resistance (almost always 298.15 C)
#define TEMPERATURENOMINAL 298.15
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 10
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 9990

float samples[NUMSAMPLES];
int i;
float VAReading;
//float samples;
float Reading;
float AvgReading;
float AvgVoltage;
float Resistance;
float InvTemp;
float Temp;

void setup(void) {
  Serial.begin(9600);
  //analogReference(EXTERNAL);
  pinMode(A0, INPUT);
}

void loop(void) {

  // take N samples in a row, with a slight delay
  AvgReading = 0;
  Reading = 0;
  for (i = 0; i < NUMSAMPLES; i++) {
    samples[i] = analogRead(A0);
    delay(10);
  }

  // average all the samples out
  for (i = 0; i < NUMSAMPLES; i++) {
    Reading += samples[i];
  }
  AvgReading = Reading / NUMSAMPLES;
  Serial.print("\nAverage analog reading: ");
  Serial.println(AvgReading);

  // convert the value to input voltage
  AvgVoltage = 5 / (1024 / AvgReading);
  Serial.print("Average Voltage: ");
  Serial.println(AvgVoltage);

  // convert to resistance
  Resistance = SERIESRESISTOR * (AvgVoltage / (3.3 - AvgVoltage));
  Serial.print("Thermistor resistance: ");
  Serial.println(Resistance);

  float Temp1 = 1 / (TEMPERATURENOMINAL * 0.001);
  float Temp2 = 1 / (BCOEFFICIENT * 0.001);
  float Temp3 = log(Resistance / SERIESRESISTOR);

  //Serial.println(Temp1);
  //Serial.println(Temp2);
  //Serial.println(Temp3);

  InvTemp = (Temp1) + (Temp2) * Temp3;

  Temp = (1 / InvTemp)*1000;
  Serial.print("Inverse Temp: ");
  Serial.println(InvTemp);

  Serial.print("Temperature: ");
  Serial.print(Temp);
  Serial.print(" K\t");
  Serial.print(Temp - 273.15);
  Serial.println(" C");

  delay(5000);
}
