// Tseng 2024/11/13
// 使用SN-3001-TR-*-N01模組讀取土壤濕度感測器的數值, 並透過nodemcu32-s的Serial2 port讀取 
// 使用nodemcu32-s的Serial port輸出數值到終端

#include <HardwareSerial.h>
#include <Arduino.h>

#define rxa 16
#define txa 17
#define BAUD_RATE 9600

// SN-3001-TR-*-N01模組的modbus協定 地址:1byte, 功能碼:1byte, 起始位址:2bytes, 資料長度:2bytes, CRC低位元:1bytes CRC高位元:1bytes
const byte soilSensorRequest[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x04, 0x44, 0x09};   // 請求土壤濕度資料, 8 bytes, 01 03 00 00 00 04 44 09(依據sensor的modbus協定，參考datasheet)

//回應資料格式 地址:1byte, 功能碼:1byte, 有效字節(資料長度):1byte, 第一數據區(濕度值):2bytes, 第二數據區(溫度值):2bytes, 第三數據區(電導率):2byte,第四數據區(PH值):2byte，CRC(低)):1bytes,CRC(高)):1bytes
byte soilSensorResponse[13];   // 回應土壤溫濕度資料,共13 bytes, 例如:01 03 04 02 92 FF 9B 03 E8  00 38 5A 3D, 
                               // 濕度值為292hex=658dec(index[3]與[4]),溫度值為FF9Bhex=-101dec(index[5]與[6]) ，電導率值為03E8hex=1000dec(index[7]與[8])，PH值為0038hex=56dec(index[9]與[10])


void setup() 
{  
  Serial.begin(BAUD_RATE);  //宣告預設的Serial port初始化
  
  Serial2.begin(4800, SERIAL_8N1, rxa, txa);  //宣告Serial2 port初始化，SN-3001-TR-*-N01模組的預設baud rate為4800
}

void loop() {
  //傳送request(要求)給土壤濕度感測器, 並等待回覆
  Serial2.write(soilSensorRequest, sizeof(soilSensorRequest));

  //檢查回覆的資料是否有13 bytes(sensor回應的資料))
  while (Serial2.available() < 13)  //若回應的資料長度小於13bytes,則再等待1秒
  {
    delay(1000);
  }

  //如果Serial2 port有等待讀取的資料且資料長度大於等於13 bytes
  if (Serial2.available() >= 13)
  {
   byte index = 0; // 宣告變數index(soilSensorResponse陣列用)

  //===========依index順序將讀取到的資料全數印出(index[0]~index[9])=============
   while (Serial2.available() && index < 13)  //當Serial2 port有資料且index小於13時
   {
     soilSensorResponse[index] = Serial2.read(); // 從Serial2 port讀取資料，依序存放到soilSensorResponse陣列    
    //  Serial.print(soilSensorResponse[index], HEX); // 印出剛存放的陣列資料(以16進位表示)
    //  Serial.print(" "); // print a space     
     index++; // 索引遞增
   }
   Serial.println(); // print a new line
  }
  //==================================================

  //=====只列印濕度原始讀值與正確百分比============
  int moisture= soilSensorResponse[3] << 8 | (soilSensorResponse[4] ); // 水分值為index[3]與[4] , soilSensorResponse[3] << 8 代表左移8位元,再加上soilSensorResponse[4]
  // Serial.print("Moisture:oringinal value="); //列印原始濕度值
  // Serial.println(moisture); //列印原始濕度值
  Serial.print("Moisture:");
  Serial.print(moisture/10);  //依據datasheet, 水分百分比為10倍,所以除以10
  Serial.println("%");
  //============================

  //=====列印正確溫度值============
  int temperature= soilSensorResponse[5] << 8 | (soilSensorResponse[6] ); // 溫度值為index[5]與[6] , soilSensorResponse[5] << 8 代表左移8位元,再加上soilSensorResponse[6]
  float temperature_real= temperature / 10.0 ; //溫度值為10倍,所以除以10
  //溫度值為2的補數,所以需轉換為正確的溫度值
  if (temperature > 0x7FFF) //如果溫度值大於0x7FFF,代表為負數(MSB為1)
  {
    temperature_real = 0xFFFF - temperature + 0x0001 ; //用0x7FFF減去原始溫度值為1的補數後再+1(改為2的補數),當msb為1時,代表為負數,msb為0時,代表為正數
    temperature_real = -temperature_real ; //轉換正負值；溫度值為10倍,所以除以10
  }
  Serial.print("Temperature:");
  Serial.print(temperature_real); //列印正確溫度值
  Serial.println(" C");
  //============================

  //=====列印電導率值============
  int conductivity= soilSensorResponse[7] << 8 | (soilSensorResponse[8] ); // 電導率值為index[7]與[8] , soilSensorResponse[7] << 8 代表左移8位元,再加上soilSensorResponse[8]
  Serial.print("Conductivity:");
  Serial.print(conductivity); //列印電導率值
  Serial.println(" uS/cm"); //單位為uS/cm
  //============================



  //=====列印PH值============
  int ph= soilSensorResponse[9] << 8 | (soilSensorResponse[10] ); // PH值為index[9]與[10] , soilSensorResponse[9] << 8 代表左移8位元,再加上soilSensorResponse[10]
  Serial.print("PH:");
  Serial.print(ph/10); //列印PH值, PH值為10倍,所以除以10  
  Serial.println(); // print a new line

  //============================

  delay(3000);

}
