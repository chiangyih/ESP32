void setup() 
{
  Serial.begin(115200);  
}

void loop() 
{
  int a=touchRead(T3);  //使用touch3 (gpio15)
  Serial.printf("觸控感測值:%u\n",a);
  delay(2000);
}
