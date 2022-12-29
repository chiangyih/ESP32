
#include "Wire.h"
#include "SHT31.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
char ssid[] = "    ";       //wifi SSID
char passwd[] = "    "; //wifi passwd

int moisture_pin=36;  //gpio36接土壤溼度感測器
int relay_pin=17;  //gpio21 接relay

String googlesheet_url="https://docs.google.com/forms/d/1RkAfATfK69qApBVhij3kjxMtO9rslxrsaJifx01-2YY/formResponse?" ; //這裡請改為自己設定的"寫入google sheet的網址"

#define SHT31_ADDRESS   0x44
 SHT31 sht ;

//===========MQTT Broker=========
char* MQTTServer="broker.emqx.io";
int   MQTTPort=1883;
char* MQTTUser="";
char* MQTTPassword="";
char* subTopic1="hhvs/library/plant/pump";      //改變pump狀態
char* pubTopic1="hhvs/library/plant/temp";    //推播氣溫
char* pubTopic2="hhvs/library/plant/humi";    //推播空氣濕度
char* pubTopic3="hhvs/library/plant/moisture";//推播土壤濕度
long MQTTLastPublishTime;//此變數用來記錄推播時間
long MQTTPublishInterval = 10000;//每10秒推撥一次
WiFiClient WifiClient;
PubSubClient MQTTClient(WifiClient);
//===============================



TaskHandle_t Task1 , Task2 ;                  //多工宣告(Task2未使用)
void SendToGoogleSheet(void * pvParameters)   //工作一:每30分鐘傳送資料到google sheet(實際傳送時間會比30分鐘長一些)
{
  while(1)
  {
      float Tempe=sht.getTemperature();  //讀取sht31溫度值
      float Humi=sht.getHumidity();      //讀取sht31濕度值
      int moisture_humi=analogRead(moisture_pin); //讀取土壤濕度sensor值
      HTTPClient http;
      String url1=googlesheet_url + "entry.2049498581=" + Tempe + "&entry.2068971505=" + Humi + "&entry.1248109765=" + moisture_humi ;
      //Serial.println(url1);
      http.begin(url1);
      int httpCode=http.GET();  //執行GET命令，順道取得網頁狀態值
      delay(1800000);   //延遲30分鐘
  }
}

//void PublishDatatoMqtt(void * pvParameters)  //工作二:發送資料與訂閱mqtt broker #此項測試後wifi無法連線，將功能改回到loop()執行
//{
//  if(!MQTTClient.connected())  //若mqtt連線中斷，則重新連線
//  {
//    MQTTConnecte();
//  }
//  if((millis() - MQTTLastPublishTime) >= MQTTPublishInterval)
//  {
//    //string te="22";
//    //string hu="50";
//    //string mo="100";
//    MQTTClient.publish(pubTopic1,"te");  //推播氣溫
//    MQTTClient.publish(pubTopic2,"hu");  //推播天氣濕度
//    MQTTClient.publish(pubTopic3,"mo");  //推播土壤濕度
//    MQTTLastPublishTime = millis(); //更新最後傳輸時間
//  }
//  MQTTClient.loop(); //更新訂閱狀態
//  delay(50);
//}


void setup()
{
  
  Serial.begin(115200);
  Wire.begin();
  sht.begin(SHT31_ADDRESS);
  Wire.setClock(100000);
  pinMode(moisture_pin,INPUT);  //設定讀取土壤sensor值
  pinMode(relay_pin,OUTPUT);    //設定觸發relay動作
  //digitalWrite(relay_pin,0);    //設定開機時relay不動作
  WifiConnecte();                //wifi開始連線
  MQTTConnecte();               //mqtt連線

  xTaskCreatePinnedToCore(SendToGoogleSheet,"Task1",10000,NULL,1,&Task1,0);  //呼叫工作一
  //xTaskCreatePinnedToCore(PublishDatatoMqtt,"Task2",10000,NULL,1,&Task2,0);  //呼叫工作二

}


void loop()
{
  if (WiFi.status() != WL_CONNECTED)  //若wifi斷線，重新連線
  {
    WifiConnecte();
  }

  if(!MQTTClient.connected())  //若mqtt連線中斷，則重新連線
  {
    MQTTConnecte();
  }  

  sht.read();         // default = true/fast       slow = false
  delay(500);
  float Tempe=sht.getTemperature();  //讀取sht31溫度值
  float Humi=sht.getHumidity();      //讀取sht31濕度值
  int moisture_humi=analogRead(moisture_pin); //讀取土壤濕度sensor值
  
  //Serial.print("Temp:"); 
  //Serial.println(Tempe,1);  //小數點取1位
  //Serial.print("Humi:");
  //Serial.println(Humi,1);
  //Serial.print("Moisture_Humi=");
  //Serial.println(moisture_humi);
  //Serial.println("");

  if((millis() - MQTTLastPublishTime) >= MQTTPublishInterval)
  {
    MQTTClient.publish(pubTopic1,String(Tempe).c_str());  //推播氣溫
    MQTTClient.publish(pubTopic2,String(Humi).c_str());  //推播天氣濕度
    MQTTClient.publish(pubTopic3,String(moisture_humi).c_str());  //推播土壤濕度
    MQTTLastPublishTime = millis(); //更新最後傳輸時間
  }
  MQTTClient.loop(); //更新訂閱狀態
  delay(50);




  delay(1000);        //每秒讀取一次
  

  if (Tempe >= 25)
  {
    digitalWrite(relay_pin,1);   //如果溫度大於等於25，則relay動作
  }
  else
  {
    digitalWrite(relay_pin,0);
  }

}

 void WifiConnecte()  //wifi連線副程式
 {
   //int connect_count=0; //宣告嘗試連線次數=0
   //WiFi.mode(WIFI_STA);   //將 esp32 wifi設為工作站模式
   WiFi.begin(ssid,passwd);
   Serial.print("Start connect to:");
   Serial.println(ssid);
   Serial.print("等待連線");
   while (WiFi.status() != WL_CONNECTED)
   {
     delay(500);
     Serial.print("...");  //等待連線中 
   }
   Serial.println("Wifi連線成功");
   Serial.print("已連線至:");
   Serial.println(ssid);
   Serial.print("IP Address:");
   Serial.println(WiFi.localIP());

 } 

void MQTTConnecte()  //MQTT連線副程式
{
  MQTTClient.setServer(MQTTServer, MQTTPort);
  MQTTClient.setCallback(MQTTCallback);
  while (!MQTTClient.connected()) 
  {    
    String  MQTTClientid = "esp32-" + String(random(1000000, 9999999));    //以亂數為ClietID
    if (MQTTClient.connect(MQTTClientid.c_str(), MQTTUser, MQTTPassword))
    {      
      Serial.println("MQTT已連線");  //連結成功，顯示「已連線」      
      MQTTClient.subscribe(subTopic1);  //訂閱SubTopic1主題
    } else 
    {      
      Serial.print("MQTT連線失敗,狀態碼=");  //若連線不成功，則顯示錯誤訊息，並重新連線
      Serial.println(MQTTClient.state());
      Serial.println("五秒後重新連線");
      delay(5000);
    }
  }
}

void MQTTCallback(char* topic, byte* payload, unsigned int length)  //當subscribe到資料時的動作
{
  Serial.print(topic); Serial.print("訂閱通知:");
  String payloadString;//將接收的payload轉成字串
  
  for (int i = 0; i < length; i++)   //顯示訂閱內容
  {
    payloadString = payloadString + (char)payload[i];
  }
  Serial.println(payloadString);
  
  if (strcmp(topic, subTopic1) == 0)  //比對主題是否為訂閱主題1
  {
    Serial.println("改變燈號：" + payloadString);
    if (payloadString == "ON") 
    {
      digitalWrite(17, HIGH);
      
    }
    if (payloadString == "OFF") 
    {
      digitalWrite(17, LOW);
      
    }
  }
}

