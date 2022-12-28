
#include "Wire.h"
#include "SHT31.h"
#include <WiFi.h>
#include <HTTPClient.h>
char ssid[] = "填無線網路的SSID";       //wifi SSID
char passwd[] = "填無線網路密碼"; //wifi passwd

int moisture_pin=36;  //gpio36接土壤溼度感測器
int relay_pin=17;  //gpio21 接relay

String googlesheet_url="https://docs.google.com/forms/d/1RkAfATfK69qApBVhij3kjxMtO9rslxrsaJifx01-2YY/formResponse?" ; //寫入google sheet的網址

#define SHT31_ADDRESS   0x44
 ;SHT31 sht ;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  sht.begin(SHT31_ADDRESS);
  Wire.setClock(100000);
  pinMode(moisture_pin,INPUT);  //設定讀取土壤sensor值
  pinMode(relay_pin,OUTPUT);    //設定觸發relay動作
  digitalWrite(relay_pin,0);    //設定開機時relay不動作
  WifiConnecte();                //wifi開始連線

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
  delay(1800000);   //每30分鐘讀取一次
  

  if (Tempe >= 25)
  {
    digitalWrite(relay_pin,1);   //如果溫度大於等於25，則relay動作
  }
  else
  {
    digitalWrite(relay_pin,0);
  }

  Serial.println("傳送資料到Google Sheet");
  HTTPClient http;
  String url1=googlesheet_url + "entry.2049498581=" + Tempe + "&entry.2068971505=" + Humi + "&entry.1248109765=" + moisture_humi ; //組合要填入google form的欄位(共3個)
  Serial.println(url1);
  http.begin(url1);
  int httpCode=http.GET();  //執行GET命令，順道取得網頁狀態值

}

 void WifiConnecte()
 {
   int connect_count=0; //宣告嘗試連線次數=0
   WiFi.mode(WIFI_STA);   //將 esp32 wifi設為工作站模式
   WiFi.begin(ssid,passwd);
   Serial.print("Start connect to:");
   Serial.println(ssid);
   Serial.print("等待連線");
   while (WiFi.status() != WL_CONNECTED)
   {
     delay(200);
     Serial.print("..");  //等待連線中 
   }
   Serial.println("Wifi連線成功");
   Serial.print("已連線至:");
   Serial.println(ssid);
   Serial.print("IP Address:");
   Serial.println(WiFi.localIP());

 } 