#include "soc/soc.h"  //解決 brownout detector問題
#include "soc/rtc_cntl_reg.h"  //解決 brownout detector問題
#include <WiFi.h>
#include <PubSubClient.h> //請先安裝PubSubClient程式庫
#include <time.h>

// ------ 以下修改成你自己的WiFi帳號密碼 ------
char* ssid = "hhvs-iot";
char* password = "Hhvs@54Iot";

// ------ 以下修改成你MQTT設定 ------
//char* MQTTServer = "mqtt.eclipseprojects.io";//免註冊MQTT伺服器
//int MQTTPort = 1883;//MQTT Port
char* MQTTServer = "210.60.50.184";
int MQTTPort = 1883;//School MQTT Port
char* MQTTUser = "";//不須帳密
char* MQTTPassword = "";//不須帳密
//推播主題1:推播溫度(記得改Topic)
char* MQTTPubTopic1 = "hhvs/cc/DHT22/temp";
//推播主題2:推播濕度(記得改Topic)
char* MQTTPubTopic2 = "hhvs/cc/DHT22/humi";
//推播主題3:推播是否有人進入
char* MQTTPubTopic3 = "hhvs/cc/SR-501";
char* MQTTPubTopic4 = "hhvs/cc/time";//推播主題4:推播現在時間

//訂閱主題1:改變LED燈號(記得改Topic)
char* MQTTSubTopic1 = "hhvs/lib/1F-door";
long MQTTLastPublishTime;//此變數用來記錄推播時間
long MQTTPublishInterval = 1000;//每1秒推撥一次
WiFiClient WifiClient;
PubSubClient MQTTClient(WifiClient);

#define LED 2

void setup() 
{
  Serial.begin(115200);
  WifiConnecte();   //開始WiFi連線
  MQTTConnecte();    //開始MQTT連線
  pinMode(LED,OUTPUT);

}

void loop() 
{
  if (WiFi.status() != WL_CONNECTED) { WifiConnecte(); }   //如果WiFi連線中斷，則重啟WiFi連線
  if (!MQTTClient.connected()) { MQTTConnecte(); }   //如果MQTT連線中斷，則重啟MQTT連線
  MQTTClient.loop();   //更新訂閱狀態

  
}


void WifiConnecte()   //開始WiFi連線
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    vTaskDelay(500);
    Serial.print(".");
  }
  Serial.println("WiFi連線成功!");
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());
}


void MQTTConnecte()  //開始MQTT連線
{
  MQTTClient.setServer(MQTTServer, MQTTPort);
  MQTTClient.setCallback(MQTTCallback);
  while (!MQTTClient.connected()) 
  {
    //以亂數為ClietID
    String  MQTTClientid = "esp32-" + String(random(1000000, 9999999));
    if (MQTTClient.connect(MQTTClientid.c_str(), MQTTUser, MQTTPassword)) 
    {
      //連結成功，顯示「已連線」。
      Serial.println("MQTT已連線");
      //訂閱SubTopic1主題
      MQTTClient.subscribe(MQTTSubTopic1);
    } else 
    {
      //若連線不成功，則顯示錯誤訊息，並重新連線
      Serial.print("MQTT連線失敗 , 狀態碼=");
      Serial.println(MQTTClient.state());
      Serial.println("5秒後重新連線...");
      vTaskDelay(5000);
    }
  }
}

//接收到訂閱時
void MQTTCallback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print(topic); Serial.print("Subscribe Notice:");
  String payloadString;//將接收的payload轉成字串

  //顯示訂閱內容
  for (int i = 0; i < length; i++) 
  {
    payloadString = payloadString + (char)payload[i];
  }
  Serial.println(payloadString);

  //比對主題是否為訂閱主題1
  if (strcmp(topic, MQTTSubTopic1) == 0) 
  {
    Serial.println("變更LED狀態：" + payloadString);
    if (payloadString == "on")
    {
      digitalWrite(LED, HIGH);    //實驗版上燈亮 
      Serial.println("Light ON");     
    }
    if (payloadString == "off") 
    {
      digitalWrite(LED, LOW);    //實驗版上燈暗 
      Serial.println("Light Off");     
    }
  }
}