#include <Arduino.h>

int in1=12;  //DIO12
int in2=13;  //DIO13
int in3=14;  //DIO14
int in4=15;  //DIO15
int en=27;   //DIO27

const int freq=50000;
const int pwmchannel=0;
const int resolution=10; 
int dutycyle=150;

void setup() 
{
  pinMode(in1,OUTPUT);  
  pinMode(in2,OUTPUT);  
  pinMode(in3,OUTPUT);  
  pinMode(in4,OUTPUT);  
  ledcSetup(pwmchannel,freq,resolution);
  ledcAttachPin(en,pwmchannel);
  Serial.begin(9600);

  Serial.println("motor test...");

}

void loop() 
{
  digitalWrite(in1,LOW); 
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  delay(100);

  Serial.println("move forward wiht increasing speed...");
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);

  while (dutycyle<1000)
  {
    ledcWrite(pwmchannel,dutycyle);
    Serial.print("Speed with dutycyle=");
    Serial.println(dutycyle);
    dutycyle=dutycyle+10;
    delay(100);
  }
  dutycyle=150;


}

