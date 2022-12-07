#include "WiFi.h"
char ssid1[]="IE-office";  //設定你要連線的SSID
char password1[]="20632063"; //設定連線密碼
void setup() 
{
   Serial.begin(115200);  
   delay(1000);
   Serial.print("Start connect to WIFI...");
   Serial.print("SSID:");
   Serial.println(ssid1);
   WiFi.mode(WIFI_STA);  //將esp32 wifi設定為工作站模式
   WiFi.begin(ssid1,password1);  //開始連線
   Serial.print("Please wait");
   while(WiFi.status() != WL_CONNECTED)  //偵測wifi是否連線
   {
      Serial.print(".");
      delay(1000);
   }
   Serial.println("");
   Serial.print("Connected to ");
   Serial.print(ssid1);
   Serial.println(" Connect to WiFi Successful!");
}
void loop() 
{
 
}
