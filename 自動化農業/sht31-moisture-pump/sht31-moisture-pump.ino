
#include "Wire.h"
#include "SHT31.h"
int moisture_pin=36;  //gpio36接土壤溼度感測器
int relay_pin=17;  //gpio21 接relay

#define SHT31_ADDRESS   0x44
SHT31 sht;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  sht.begin(SHT31_ADDRESS);
  Wire.setClock(100000);
  pinMode(moisture_pin,INPUT);  //設定讀取土壤sensor值
  pinMode(relay_pin,OUTPUT);    //設定觸發relay動作
  digitalWrite(relay_pin,0);    //開機時relay不動作

}


void loop()
{
  sht.read();         // default = true/fast       slow = false
  delay(500);
  float Tempe=sht.getTemperature();  //讀取sht31溫度值
  float Humi=sht.getHumidity();      //讀取sht31濕度值
  int moisture_humi=analogRead(moisture_pin); //讀取土壤濕度sensor值

  Serial.print("Temp:"); 
  Serial.println(Tempe,1);  //小數點取1位
  Serial.print("Humi:");
  Serial.println(Humi,1);
  Serial.print("Moisture_Humi=");
  Serial.println(moisture_humi);
  Serial.println("");
  delay(3000);

  if (Tempe >= 25)
  {
    digitalWrite(relay_pin,1);
  }
  else
  {
    digitalWrite(relay_pin,0);
  }

}