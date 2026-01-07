#include <Arduino.h>

const byte touchPin=15;
const int threshold=28; //觸控臨界值
int touchvalue;   //觸控值

void setup() 
{
  pinMode(LED_BUILTIN,OUTPUT);
  
}

void loop() 
{
  touchvalue=touchRead(touchPin);   //讀取觸控值
  //printf("觸控值=%u\n",touchvalue);
  //vTaskDelay(1000);
  if (touchvalue < threshold)
  {
    digitalWrite(BUILTIN_LED,HIGH); //使用內建LED，當碰觸時點亮
  }
  else
  {
    digitalWrite(BUILTIN_LED,LOW);
  }
  vTaskDelay(500);
  
}