
int moisture_pin=36;  //感測器接gpio36
void setup()
{
   Serial.begin(115200);
   
   pinMode(moisture_pin,INPUT);

}

void loop() 
{
   int humi=analogRead(moisture_pin);
   Serial.print("Humi=");
   Serial.println(humi);
   delay(1000);

}
