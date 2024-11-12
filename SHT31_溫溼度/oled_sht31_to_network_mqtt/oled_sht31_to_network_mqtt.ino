// 使用 SHT31 by Rob Tillaart(rob.tillarrt@gmail.com)的library
#include "Wire.h"
#include "U8g2lib.h"
#include "SHT31.h"
#include "WiFi.h"
char ssid[] = "hhvs-iot";       //請填入欲連線的SSID名稱
char passwd[] = "Hhvs@54Iot"; //請填連線密碼
//char ssid[] = "HHVS_wireless";       //請填入欲連線的SSID名稱
//char passwd[] = ""; //請填連線密碼

#define SHT31_ADDRESS   0x44
SHT31 sht;

#include <PubSubClient.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//---抓取網路時間---
char* ntpServer="pool.ntp.org";
uint16_t utcOffset=28800;  //UTC+8偏移量
uint8_t daylightOffset=0; //夏令時間
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
char* MQTTSubTopic1 = "DHT22/library/led";
long MQTTLastPublishTime;//此變數用來記錄推播時間
long MQTTPublishInterval = 10000;//每10秒推撥一次
WiFiClient WifiClient;
PubSubClient MQTTClient(WifiClient);

void setup()
{
  Serial.begin(115200);
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);

  Wire.begin();
  sht.begin(SHT31_ADDRESS);
  Wire.setClock(100000);

  WifiConnect(); //wifi連線
  MQTTConnecte(); //mqtt broker連線

  //=====取得網路時間======
  configTime(utcOffset,daylightOffset,ntpServer);
  delay(1000); 
  //===================== 

}

void loop()
{
  struct tm now;  //宣告分解時間結構變數

  //如果WiFi連線中斷，則重啟WiFi連線
  if (WiFi.status() != WL_CONNECTED) { WifiConnect(); }

  //如果MQTT連線中斷，則重啟MQTT連線
  if (!MQTTClient.connected()) {  MQTTConnecte(); }

  float temp;
  float humi;
  sht.read();
  temp=sht.getTemperature();
  humi=sht.getHumidity();

  u8g2.clearBuffer();
  u8g2.setCursor(0, 0);
  u8g2.print(String("Temperature: ").c_str());
  u8g2.print(String(temp).c_str());
  u8g2.print(String("*C").c_str());

  u8g2.setCursor(0, 20);
  u8g2.print(String("Humidity: ").c_str());
  u8g2.print(String(humi).c_str());
  u8g2.print(String("%").c_str());
  
  u8g2.sendBuffer();
  delay(500);

  //如果距離上次傳輸已經超過MQTTPublishInterval設定值，則Publish溫溼度
  if ((millis() - MQTTLastPublishTime) >= MQTTPublishInterval ) 
  {
    //讀取溫濕度
   //temp=sht.getTemperature();
   //humi=sht.getHumidity();


    //====抓取網路時間===
    if(!getLocalTime(&now))  //取得本地時間
    {
      Serial.println("無法取得網路時間...");
      return;
    }
    String a=asctime(&now);
   //===================   
    
    // ------ 將sht31溫度送到MQTT主題 ------
    MQTTClient.publish(MQTTPubTopic1, String(temp).c_str());
    MQTTClient.publish(MQTTPubTopic2, String(humi).c_str());
    
    MQTTClient.publish(MQTTPubTopic4, a.c_str());  //傳送時間值
    //Serial.println("溫溼度已推播到MQTT Broker");
    MQTTLastPublishTime = millis(); //更新最後傳輸時間
  }
  MQTTClient.loop();//更新訂閱狀態
  delay(50);

}

void WifiConnect()
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

//===============開始MQTT連線=============
void MQTTConnecte() 
{
  MQTTClient.setServer(MQTTServer, MQTTPort);
  MQTTClient.setCallback(MQTTCallback);
  while (!MQTTClient.connected()) {
    //以亂數為ClietID
    String  MQTTClientid = "esp32-" + String(random(1000000, 9999999));
    if (MQTTClient.connect(MQTTClientid.c_str(), MQTTUser, MQTTPassword)) {
      //連結成功，顯示「已連線」。
      Serial.println("MQTT已連線");
      //訂閱SubTopic1主題
      MQTTClient.subscribe(MQTTSubTopic1);
    } else {
      //若連線不成功，則顯示錯誤訊息，並重新連線
      Serial.print("MQTT連線失敗,狀態碼=");
      Serial.println(MQTTClient.state());
      Serial.println("五秒後重新連線");
      delay(5000);
    }
  }
}

//==========接收到訂閱時==============
void MQTTCallback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print(topic); Serial.print("訂閱通知:");
  String payloadString;//將接收的payload轉成字串
  //顯示訂閱內容
  for (int i = 0; i < length; i++) {
    payloadString = payloadString + (char)payload[i];
  }
  Serial.println(payloadString);
  //比對主題是否為訂閱主題1
  if (strcmp(topic, MQTTSubTopic1) == 0) {
    Serial.println("改變燈號：" + payloadString);
    if (payloadString == "ON") {
      digitalWrite(2, HIGH);
      //MatrixInt(6,1); //實驗版上綠燈亮
    }
    if (payloadString == "OFF") {
      digitalWrite(2, LOW);
      //MatrixInt(6,0); //實驗版上綠燈暗
    }
  }
}
