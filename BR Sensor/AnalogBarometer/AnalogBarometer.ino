#define Count 10
float BRVout = A0;
// int LedPin = 13;

int i;
int BRAnalogVoltage[Count] = {};
float BRSum;
float BRAverage;
float BRVoltageIn;
float BR;

//From Original Manual
float BRk0 = 0.809; //intercept of BR sensor atmospheres
float BRk1 = 0.077; //slope of BR sensor atmospheres

//float BRk0 = 24.215; // intercept mercury
//float BRk1 = 2.292; // slope mercury

void setup() {
  Serial.begin(9600);
  pinMode(BRVout, INPUT);
  
  // pinMode(LedPin, OUTPUT);
}

void loop() {
  // digitalWrite(LedPin, HIGH);
  // delay(1000);

  BRSum = 0;
  for (i = 0; i < Count; i++) {
    BRAnalogVoltage[i] = analogRead(BRVout);
    BRSum += BRAnalogVoltage[i];
   //Serial.print(BRAnalogVoltage[i]);
   // Serial.print("\t");
   // Serial.println(BRSum);
  }

  BRAverage = BRSum / Count;
  BRVoltageIn = 5/(1024/BRAverage); // Use this format becuase 5/1024 * ADC value chops least sig. bits off
  BR = BRVoltageIn*BRk1 + BRk0;
  
  //Serial.print("BRSum = ");
  //Serial.print(BRSum);
  
  //Serial.print("\t BR Average: ");
  //Serial.print(BRAverage);

  //Serial.print("\t Voltage input from BR sensor: ");
  //Serial.print(BRAverage);

  //Serial.print("\t BR = "); //converted analog voltage to atmospheres
  Serial.println(BR);
  //Serial.println(" ATM");

  // digitalWrite(LedPin, LOW);
  delay(100);
}
