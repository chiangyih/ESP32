#include <Arduino.h>

int in1=12;  //DIO12
int in2=13;  //DIO13
int in3=14;  //DIO14
int in4=15;  //DIO15


void setup() 
{
  //pinMode(in1,OUTPUT);  
  //pinMode(in2,OUTPUT);  
  //pinMode(in3,OUTPUT);  
  //pinMode(in4,OUTPUT);  
  ledcSetup(0,1000,10);
  ledcSetup(1,1000,10);
  ledcSetup(2,1000,10);
  ledcSetup(3,1000,10);
  ledcAttachPin(in1,0);
  ledcAttachPin(in2,0);
  ledcAttachPin(in3,0);
  ledcAttachPin(in4,0);
}

void loop() 
{
  ledcWrite(0,300);
  digitalWrite(in2,LOW);
  ledcWrite(2,300);
  digitalWrite(in4,LOW);
  delay(2000);
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  delay(2000);
  digitalWrite(in1,LOW);
  ledcWrite(1,HIGH);
  digitalWrite(in3,LOW);
  ledcWrite(3,HIGH);  
  delay(2000);
}

