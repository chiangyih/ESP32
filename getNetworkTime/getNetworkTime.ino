#include "soc/soc.h"  //解決 brownout detector問題
#include "soc/rtc_cntl_reg.h"  //解決 brownout detector問題

#include<WiFi.h>
#include<time.h>

char ssid1[]="IE-office";
char password1[]="20632063";
char* ntpServer="pool.ntp.org";
uint16_t utcOffset=28800;  //UTC+8偏移量
uint8_t daylightOffset=0; //夏令時間

void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector解決 brownout detector問題
  Serial.begin(115200);
  Serial.print("Start connect to WIFI...");
   Serial.print("SSID:");
   Serial.println(ssid1);
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid1,password1);  //wifi 連線
   Serial.print("Please wait");
   while(WiFi.status() != WL_CONNECTED)
   {
      Serial.print(".");
      delay(1000);
   }
   Serial.println("");
   Serial.print("Connected to ");
   Serial.print(ssid1);
   Serial.println(" Done!");

  //=====取得網路時間======
   configTime(utcOffset,daylightOffset,ntpServer);
   delay(1000); 
  //=====================
}

void loop()
{
  struct tm now;  //宣告分解時間結構變數
  if(!getLocalTime(&now))  //取得本地時間
  {
    Serial.println("無法取得網路時間...");
    return;
   }

   Serial.println(&now, "%Y/%m/%d %H:%M:%S");
   delay(1000);
}
