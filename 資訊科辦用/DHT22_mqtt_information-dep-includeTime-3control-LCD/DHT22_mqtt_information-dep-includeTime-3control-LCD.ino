// esp32 GPIO21=> SDA
// esp32 GPIO22=> SCL
#include <LiquidCrystal_PCF8574.h> //LCD1602用
#include "soc/soc.h"  //解決 brownout detector問題
#include "soc/rtc_cntl_reg.h"  //解決 brownout detector問題

#include "WiFi.h"
#include <PubSubClient.h> //請先安裝PubSubClient程式庫
#include <SimpleDHT.h>
#include<time.h>

LiquidCrystal_PCF8574 lcd(0x27);  // 設定i2c位址，一般情況就是0x27和0x3F兩種

//---抓取網路時間---
char* ntpServer="pool.ntp.org";
uint16_t utcOffset=28800;  //UTC+8偏移量
uint8_t daylightOffset=0; //夏令時間

// ------ 以下修改成你自己的WiFi帳號密碼 ------
//char ssid[] = "IE-office";
//char password[] = "20632063";
char ssid[] = "HHVS_wireless";
char password[] = "";

//------ 以下修改成你DHT22腳位 ------
int pinDHT22 = 16;
SimpleDHT22 dht22(pinDHT22);
//-------SR-501 PIR 腳位----------
int pinSR501=17;

//----LED腳位--------
int led21=12;
int led17=14;
int led22=27;
 
// ------ 以下修改成你MQTT設定 ------
//char* MQTTServer = "mqtt.eclipseprojects.io";//免註冊MQTT伺服器
//int MQTTPort = 1883;//MQTT Port
char* MQTTServer = "210.60.50.184";
int MQTTPort = 1883;//School MQTT Port
char* MQTTUser = "";//不須帳密
char* MQTTPassword = "";//不須帳密
char* MQTTPubTopic1 = "hhvs/ie/DHT22/temp";//推播主題1:推播溫度(記得改Topic)
char* MQTTPubTopic2 = "hhvs/ie/DHT22/humi";//推播主題2:推播濕度(記得改Topic)
char* MQTTPubTopic3 = "hhvs/ie/SR-501";//推播主題3:推播是否有人進入
char* MQTTPubTopic4 = "hhvs/time";//推播主題4:推播現在時間

char* MQTTSubTopic1 = "hhvs/ie/led21";//訂閱主題1:改變led21燈號(記得改Topic)
char* MQTTSubTopic2 = "hhvs/ie/led17";//訂閱主題2:改變led17燈號(記得改Topic)
char* MQTTSubTopic3 = "hhvs/ie/led22";//訂閱主題3:改變led22燈號(記得改Topic)
long MQTTLastPublishTime;//此變數用來記錄推播時間
long MQTTPublishInterval = 1000;//每1秒推撥一次
WiFiClient WifiClient;
PubSubClient MQTTClient(WifiClient);

void setup() 
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector解決 brownout detector問題
  Serial.begin(115200);
  pinMode(led21, OUTPUT);//紅色led21燈
  pinMode(led17, OUTPUT);//黃色led17燈
  pinMode(led22, OUTPUT);//綠色led22燈

  //開始WiFi連線
  WifiConnecte();

  //開始MQTT連線
  MQTTConnecte();

  pinMode(pinSR501,INPUT); //讀取SR-501腳位
  //=====取得網路時間======
   configTime(utcOffset,daylightOffset,ntpServer);
   delay(1000); 
  //=====================  
  
  //=====初始化LCD1602=====
  lcd.begin(16, 2); // 初始化LCD
  //lcd.begin(20, 4); // 如果是20x4的LCD就是用這行
  lcd.setBacklight(150); //背光
  lcd.clear();//清除
  lcd.setCursor(0, 0);  //設定游標位置 (字,行)
  
  //=================================  
}

void loop() 
{
  struct tm now;  //宣告分解時間結構變數
  
  //如果WiFi連線中斷，則重啟WiFi連線
  if (WiFi.status() != WL_CONNECTED) { WifiConnecte(); }

  //如果MQTT連線中斷，則重啟MQTT連線
  if (!MQTTClient.connected()) {  MQTTConnecte(); }

  //如果距離上次傳輸已經超過10秒，則Publish溫溼度
  if ((millis() - MQTTLastPublishTime) >= MQTTPublishInterval ) 
  {
    //讀取溫濕度
    byte temperature = 0;
    byte humidity = 0;
    ReadDHT(&temperature, &humidity);
    //讀取SR-501偵測資料
    int sr501Value=digitalRead(pinSR501);
    //====抓取網路時間===
    if(!getLocalTime(&now))  //取得本地時間
    {
      Serial.println("無法取得網路時間...");
      return;
    }
    String a=asctime(&now);
    lcd.setCursor(0, 0);  //設定游標位置 (字,行)
    lcd.print(a);   //將網路日期時間輸出到LCD第一行
      
   //================   

    
    // ------ 將DHT22溫度送到MQTT主題 ------
    MQTTClient.publish(MQTTPubTopic1, String(temperature).c_str());
    MQTTClient.publish(MQTTPubTopic2, String(humidity).c_str());
    MQTTClient.publish(MQTTPubTopic3, String(sr501Value).c_str());
    MQTTClient.publish(MQTTPubTopic4, a.c_str());
    Serial.println("溫溼度已推播到MQTT Broker");
    MQTTLastPublishTime = millis(); //更新最後傳輸時間


  }
  
  MQTTClient.loop();//更新訂閱狀態
  delay(50);

 
}

//讀取DHT22溫濕度
void ReadDHT(byte *temperature, byte *humidity) {
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read(temperature, humidity, NULL)) !=
      SimpleDHTErrSuccess) {
    Serial.print("讀取失敗,錯誤訊息="); 
    Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); 
    Serial.println(SimpleDHTErrDuration(err)); 
    delay(1000);
    return;
  }
  Serial.print("DHT讀取成功：");
  Serial.print((int)*temperature); 
  Serial.print(" *C, ");
  Serial.print((int)*humidity); 
  Serial.println(" H");
  lcd.setCursor(0, 1);  //設定游標位置 (字,行)
  lcd.print("T:");
  lcd.print((int)*temperature);   //第2行第1字顯示溫度
  lcd.print(".C");
  lcd.setCursor(8, 1);  //設定游標位置 (第8字,第1行)
  lcd.print("H:");
  lcd.print((int)*humidity);     //第2行第9字顯示濕度
  lcd.print("%");
}

//WiFi連線副程式
void WifiConnecte() {
  //開始WiFi連線
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi連線成功");
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());
}

//開始MQTT連線
void MQTTConnecte() {
  MQTTClient.setServer(MQTTServer, MQTTPort);
  MQTTClient.setCallback(MQTTCallback);
  while (!MQTTClient.connected()) {
    //以亂數為ClietID
    String  MQTTClientid = "esp32-" + String(random(1000000, 9999999));
    if (MQTTClient.connect(MQTTClientid.c_str(), MQTTUser, MQTTPassword)) 
    {
      //連結成功，顯示「已連線」。
      Serial.println("MQTT已連線");
      //訂閱SubTopic1主題
      MQTTClient.subscribe(MQTTSubTopic1);  //訂閱subtopic1
      MQTTClient.subscribe(MQTTSubTopic2);  //訂閱subtopic2
      MQTTClient.subscribe(MQTTSubTopic3);  //訂閱subtopic3
    
    } else {
      //若連線不成功，則顯示錯誤訊息，並重新連線
      Serial.print("MQTT連線失敗,狀態碼=");
      Serial.println(MQTTClient.state());
      Serial.println("五秒後重新連線");
      delay(5000);
    }
  }
}

//接收到訂閱時
void MQTTCallback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print(topic); Serial.print("訂閱通知:");
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
    Serial.println("改變燈號：" + payloadString);
    if (payloadString == "R") 
  {
      digitalWrite(led21, HIGH);      
    }
    if (payloadString == "r") 
  {
      digitalWrite(led21, LOW);      
    }
  }

  //比對主題是否為訂閱主題2
  if (strcmp(topic, MQTTSubTopic2) == 0) 
  {
    Serial.println("改變燈號：" + payloadString);
    if (payloadString == "Y") 
  {
      digitalWrite(led17, HIGH);      
    }
    if (payloadString == "y") 
  {
      digitalWrite(led17, LOW);      
    }
  }  
  
  //比對主題是否為訂閱主題3
  if (strcmp(topic, MQTTSubTopic3) == 0) 
  {
    Serial.println("改變燈號：" + payloadString);
    if (payloadString == "G") 
  {
      digitalWrite(led22, HIGH);      
    }
    if (payloadString == "g") 
  {
      digitalWrite(led22, LOW);      
    }
  }  
  

}
