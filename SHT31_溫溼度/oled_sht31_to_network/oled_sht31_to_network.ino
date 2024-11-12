// 使用 SHT31 by Rob Tillaart(rob.tillarrt@gmail.com)的library
#include "Wire.h"
#include "U8g2lib.h"
#include "SHT31.h"
#include "WiFi.h"
char ssid[] = "hhvs-iot";       //請填入欲連線的SSID名稱
char passwd[] = "Hhvs@54Iot"; //請填連線密碼

#define SHT31_ADDRESS   0x44
SHT31 sht;


U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


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

  WifiConnect();

}

void loop()
{
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
