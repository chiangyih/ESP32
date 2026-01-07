// 使用 SHT31 by Rob Tillaart(rob.tillarrt@gmail.com)的library

#include "Wire.h"
#include "SHT31.h"

#define SHT31_ADDRESS   0x44
SHT31 sht;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  sht.begin(SHT31_ADDRESS);
  Wire.setClock(100000);

}


void loop()
{
  sht.read();         // default = true/fast       slow = false
  delay(1000);
  Serial.print("Temperature:");
  Serial.println(sht.getTemperature(), 1);
 
  Serial.print("Humidity:");
  Serial.println(sht.getHumidity(), 1);
  Serial.println("");
  delay(1500);
}

