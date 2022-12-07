#include <ESP32Servo.h>
Servo s1;
int pos=0;
void setup() 
{
  s1.setPeriodHertz(50);
  s1.attach(2,600,2400);
}

void loop() 
{
  s1.write(0);
  delay(2000);
  //s1.write(90);
  //delay(2000);
  s1.write(180);
  delay(2000);
}
