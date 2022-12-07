#include "WiFi.h"
void setup() 
{
   Serial.begin(115200);
   WiFi.mode(WIFI_STA); //#setting network mode to STA workstation mode
   WiFi.disconnect();
   delay(100);
   Serial.println("Setup Working Mode Done!")  ;
}

void loop() 
{
   Serial.println("Scan Wifi Start...");
   int n=WiFi.scanNetworks();
   Serial.println("Scan done!");
   if(n==0)
   {
       Serial.println("no wifi found");
   }
    else
    {
       Serial.print(n);
       Serial.println("network found!");
       for (int i=0;i<n;i++)
       {
           Serial.print(i+1);
           Serial.print(":");
           Serial.print(WiFi.SSID(i));
           Serial.print("(");
           Serial.print(WiFi.RSSI(i));
           Serial.print(")");
           Serial.println((WiFi.encryptionType(i)==WIFI_AUTH_OPEN)?" ":"*");
           delay(20);
        }     
     }
     Serial.println("");
     delay(5000);     

}
