#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

int in1=12;  //DIO12
int in2=13;  //DIO13
int in3=14;  //DIO14
int in4=15;  //DIO15


void setup() 
{
  // put your setup code here, to run once:
  int result = myFunction(2, 3);


  pinMode(in1,OUTPUT);  
  pinMode(in2,OUTPUT);  
  pinMode(in3,OUTPUT);  
  pinMode(in4,OUTPUT);  
}

void loop() 
{
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
  delay(2000);
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  delay(2000);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);  
  delay(2000);
}





// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}