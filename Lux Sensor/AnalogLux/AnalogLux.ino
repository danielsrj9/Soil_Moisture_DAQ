#define Count 10

int i;
//int LedPin = 13;
float ADCVoltage[Count] = {};
float ADCAvgVoltage;
float ADCSum;
float LuxVoltage;
float Lux;

//From Original Manual
float Luxk0 = 0; //intercept of Lux sensor 0-600 lux
float Luxk1 = 154; //slope of Lux sensor lux/V

//float Luxk0 = 0; // intercept 0-6000 lux
//float Luxk1 = 1692; // slope lux/V

//float Luxk0 = 0; // intercept 0-150000 lux
//float Luxk1 = 38424; // slope lux/V

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  //pinMode(LedPin, OUTPUT);
}

void loop() {
  //digitalWrite(LedPin, HIGH);
  //delay(100);

  ADCSum = 0;
  for (i = 0; i < Count; i++) {
    ADCVoltage[i] = analogRead(A0);
    ADCSum += ADCVoltage[i];
    //Serial.print(ADCVoltage[i]);
    //Serial.print("\t");
    //Serial.println(ADCSum);
  }

  ADCAvgVoltage = ADCSum / Count;
  LuxVoltage = 5 / (1024 / ADCAvgVoltage);
  Lux = Luxk1 * LuxVoltage + Luxk0;

  // Light Sensor

  Serial.print("ADC AVG. Voltage = "); //analog voltage output of Vernier Lux Sensor
  Serial.print(ADCAvgVoltage);

  Serial.print("\t Lux sensor input = "); //converted analog voltage to Lux
  Serial.print(LuxVoltage);
  
  Serial.print("\t Lux = ");
  Serial.println(Lux);
  Serial.println("");

  // digitalWrite(LedPin, LOW);

  delay(500);
}
