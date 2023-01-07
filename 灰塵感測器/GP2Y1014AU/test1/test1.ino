int dustPin=36;
float dustVal=0;
 
int ledPower=12;
int delayTime=280;
int delayTime2=40;
float offTime=9680;
void setup()
{
  Serial.begin(9600);
  pinMode(ledPower,OUTPUT);
  pinMode(dustPin, INPUT);
}
 
void loop()
{
  // ledPower is any digital pin on the arduino connected to Pin 3 on the sensor
  digitalWrite(ledPower,LOW); 
  delayMicroseconds(delayTime);
  dustVal=analogRead(dustPin); 
  delayMicroseconds(delayTime2);
  digitalWrite(ledPower,HIGH); 
  delayMicroseconds(offTime);
  
  delay(1000);

  if (dustVal>36.455)
    Serial.println((float(dustVal/1024)-0.0356)*120000*0.035);
}