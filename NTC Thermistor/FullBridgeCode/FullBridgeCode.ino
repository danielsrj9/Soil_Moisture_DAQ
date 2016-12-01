#define BridgeResistance 6000 //Resistance of fixed resistors
  float VBReading;
  float VAReading;

  float VADCDelta;
  float VABVoltage;

  float VoltageIn;

  float Thermistor;
  
void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
}

void loop() {


  VAReading = analogRead(A0);
  VBReading = analogRead(A1);

  // Voltage to Resistance conversion

  VADCDelta = VAReading - VBReading;
  VoltageIn = 5/(1024/VADCDelta);
  
  Thermistor = BridgeResistance / VBReading;

  Serial.print("Thermistor Resistance (Ohms)");
  Serial.println(Thermistor);

  delay(1000);
}
