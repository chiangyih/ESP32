#include <Arduino.h>

int in1=12;  //DIO12
int in2=13;  //DIO13
int in3=14;  //DIO14
int in4=15;  //DIO15
int en=27;   //DIO27;pwm用

const int freq=50000; //pwm頻率
const int pwmchannel=0;  //pwm通道
const int resolution=10;  //解析度(位元)
int dutycyle=150;  //初始工作週期(解析度)

void setup() 
{
  pinMode(in1,OUTPUT);  
  pinMode(in2,OUTPUT);  
  pinMode(in3,OUTPUT);  
  pinMode(in4,OUTPUT);  
  ledcSetup(pwmchannel,freq,resolution);  //設定pwm的通道、頻率與解析度；esp32有16個通道可用
  ledcAttachPin(en,pwmchannel);   //設定pwm輸出腳位
  Serial.begin(9600);
  
  stop();  //====全數馬達停止

}

void loop() 
{
  int a;
  
  forward(a);
  delay(2000);

  revers(a);
  delay(2000);

  stop();
  delay(2000);
 
}

void forward(int speed) 
{
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
  ledcWrite(pwmchannel,speed);
}

void revers(int speed)
{
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  ledcWrite(pwmchannel,speed);
}

void stop()
{
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  ledcWrite(pwmchannel,0);
}