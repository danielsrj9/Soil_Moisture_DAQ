// the value of the 'other' resistor
#define SERIESRESISTOR 4700    
 
// What pin to connect the sensor to
float THERMISTORPIN = A0;
 
void setup(void) {
  Serial.begin(9600);
}
 
void loop(void) {
  float reading;
 
  reading = analogRead(THERMISTORPIN);
 
  Serial.print("Analog reading "); 
  Serial.println(reading);
 
  // convert the value to resistance
  reading = (1023 / reading)  - 1;
  
  Serial.print("reading two ");
  Serial.println(reading);
  
  reading = SERIESRESISTOR / reading;
  Serial.print("Thermistor resistance "); 
  Serial.println(reading);
  Serial.println("");
 
  delay(1000);
}
