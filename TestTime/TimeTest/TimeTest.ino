#include <Time.h>
#include <TimeLib.h>

long int CurrentTime = millis();;

int Sec = 0;
int Min = 0;
int Hr = 0;

void setup() {
  Serial.begin(9600);
  //Serial.println("Now, year, month, day, hour, minute, second");
}

void loop() {
  //Time Stamping in year, month, day, hour, minute, second
  /*********************************************/

//  setTime(18,37,0,18,11,2016);

while
  Sec = CurrentTime/1000;
  
  if((Sec%60)== 0 && CurrentTime > 1000){
    Sec = 0;
    CurrentTime = CurrentTime - 60000;
    Min++;
  }

  if(Min%60 == 0 && CurrentTime > 1000){
    Min = 0;
    Hr++;
  }

  Serial.print("Current Time: \t");
  Serial.print((CurrentTime));

  Serial.print("\tSeconds: \t");
  Serial.print(Sec);

  Serial.print("\tMinutes: \t");
  Serial.println(Min);
  
  //Serial.println(Hr);
  //delay(500);
}
