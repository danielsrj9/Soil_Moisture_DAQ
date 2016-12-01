float ADCVoltage1;
float ADCVoltage2;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT); //Reference side yellow wire
  pinMode(A1, INPUT); //Thermistor side blue wire
  pinMode(A2, INPUT); //Amplifier output
  pinMode(13, OUTPUT);
}

void loop() {

  digitalWrite(13, LOW);

  ADCVoltage1 = analogRead(A0);
  ADCVoltage2 = analogRead(A1);

  Serial.print("V1: ");
  Serial.print(ADCVoltage1);
  Serial.print("\tV1 in: ");
  Serial.print(5 / (1024 / ADCVoltage1));

  Serial.print("\tV2: ");
  Serial.print(ADCVoltage2);
  Serial.print("\tV2 in: ");
  Serial.print(5 / (1024 / ADCVoltage2));

  Serial.print("\tDelta Voltage: ");
  Serial.print(ADCVoltage2 - ADCVoltage1);
  Serial.print("\tDelta In: ");
  Serial.println(5 / (1024 / (ADCVoltage2 - ADCVoltage1)));

  digitalWrite(13, HIGH);
  delay(1000);
}
