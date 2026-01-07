#include "WiFi.h"
#include "HTTPClient.h"
#include "WiFiMulti.h";
#include "ArduinoJson.h";

char ssid1[]="IE-office";
char password1[]="20632063";
char ssid2[]="hhvs-iot";
char password2[]="Hhvs@54iot";
char ssid3[]="lib-n66u";
char password3[]="library161";
char url[]="http://opendata2.epa.gov.tw/AQI.json";

void setup() 
{
   Serial.begin(115200);
   delay(1000);
   Serial.print("Start connect to WIFI...");
   Serial.print("SSID:");
   Serial.println(ssid3);
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid3,password3);  //wifi 連線
   Serial.print("Please wait");
   while(WiFi.status() != WL_CONNECTED)
   {
      Serial.print(".");
      delay(1000);
   }
   Serial.println("");
   Serial.print("Connected to ");
   Serial.print(ssid3);
   Serial.println(" Done!");

}

void loop() 
{
   Serial.println("Starting Connect to PM2.5 Webpage...");
   HTTPClient http;
   http.begin(url);
   int httpCode=http.GET();
   Serial.print("httpCode=");
   Serial.println(httpCode);
   if (httpCode==HTTP_CODE_OK)
   {
      String payload=http.getString();
      // Serial.print("payload=");
      // Serial.println(payload); 
      DynamicJsonDocument AQJarray(payload.length()*2);
      deserializeJson(AQJarray,payload);
      for(int i=0;i<=AQJarray.size();i++)
      {
         if(AQJarray[i]["SiteId"]=="43") //新營的SiteID=43
         {
            String Datetime=AQJarray[i]["PublishTime"];
            Serial.println("PublishTime=" + Datetime);
         }         
         if(AQJarray[i]["SiteId"]=="43") //新營的SiteID=43
         {
            String HsinYinAQI=AQJarray[i]["AQI"];
            Serial.println("新營AQI=" + HsinYinAQI);
         }
         if(AQJarray[i]["SiteId"]=="44") //善化的SiteID=44
         {
            String SanHuaAQI=AQJarray[i]["AQI"];
            Serial.println("善化AQI=" + SanHuaAQI);
         }  
         if(AQJarray[i]["SiteId"]=="45") //安南的SiteID=45
         {
            String AnNanAQI=AQJarray[i]["AQI"];
            Serial.println("安南AQI=" + AnNanAQI);
         }
         if(AQJarray[i]["SiteId"]=="46") //臺南市的SiteID=46
         {
            String AnNanAQI=AQJarray[i]["AQI"];
            Serial.println("臺南市AQI=" + AnNanAQI);
         }                                                     
      }     
   }
   http.end();
   delay(100000);

}
