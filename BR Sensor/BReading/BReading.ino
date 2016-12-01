float BReading;

void setup() {
  Serial.begin(9600);
  pinMode(A0,INPUT);

}

void loop() {
  BReading = analogRead(A0);
  Serial.println(BReading);
}
