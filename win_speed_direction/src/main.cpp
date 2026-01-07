// Tseng 2024/11/13
// 使用KM53B88模組讀取風速與風向, 並透過nodemcu32-s的Serial2 port讀取 
// 感測器機體上有白點，需面向北方
// 使用nodemcu32-s的Serial port輸出數值到終端

#include <HardwareSerial.h>
#include <Arduino.h>

#define rxa 16
#define txa 17
#define BAUD_RATE 9600

// JXBS-3001-TR-RS模組的modbus協定 地址:1byte, 功能碼:1byte, 起始位址:2bytes, 資料長度:2bytes, CRC低位元:1bytes CRC高位元:1bytes
const byte WindSensorRequest[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};   // 請求土壤濕度資料, 8 bytes, 01 03 00 00 00 02 C4 0B(依據sensor的modbus協定，參考datasheet)

//回應資料格式 地址:1byte, 功能碼:1byte, 有效字節(資料長度):1byte, 第一數據區(風速):2bytes, 第二數據區(風向):2bytes, CRC:2bytes
byte WindSensorResponse[9];   // 回應資料,共9 bytes, 例如:01 03 04 02 92 FF 9B 5A 3D


void setup() 
{  
  Serial.begin(BAUD_RATE);  //宣告預設的Serial port初始化
  
  Serial2.begin(BAUD_RATE, SERIAL_8N1, rxa, txa);  //宣告Serial2 port初始化
}

void loop() {
  //傳送request(要求)給感測器, 並等待回覆
  Serial2.write(WindSensorRequest, sizeof(WindSensorRequest));

  //檢查回覆的資料是否有9 bytes(sensor回應的資料))
  // while (Serial2.available() < 8)  //若回應的資料長度小於9 bytes,則再等待1秒
  // {
  //   delay(1000);
  // }

  //如果Serial2 port有等待讀取的資料且資料長度大於等於9 bytes
  if (Serial2.available() >= 8)
  {
   byte index = 0; // 宣告變數index(WindSensorResponse陣列用)

  //===========依index順序將讀取到的資料全數印出(index[0]~index[9])=============
   while (Serial2.available() )  //當Serial2 port有資料且index小於9時
   {
     WindSensorResponse[index] = Serial2.read(); // 從Serial2 port讀取資料，依序存放到WindSensorResponse陣列    
     Serial.print(WindSensorResponse[index], HEX); // 印出剛存放的陣列資料(以16進位表示)
     Serial.print(" "); // print a space     
     index++; // 索引遞增
   }
   Serial.println(); // print a new line
  }
  //==================================================

  // //=====列印風速讀值============
  // int speed= WindSensorResponse[3] << 8 | (WindSensorResponse[4] ); // 值為index[3]與[4] , WindSensorResponse[3] << 8 代表左移8位元,再加上WindSensorResponse[4]
  // // Serial.print("Moisture:oringinal value="); //列印原始濕度值
  // // Serial.println(moisture); //列印原始濕度值
  // Serial.print("Speed:");
  // Serial.println(speed/10);  //依據datasheet, 水分百分比為10倍,所以除以10
  // // Serial.println("%");
  // //============================

  // //=====列印風向讀值(角度)============
  // int direction= WindSensorResponse[5] << 8 | (WindSensorResponse[6] ); // 值為index[3]與[4] , WindSensorResponse[3] << 8 代表左移8位元,再加上WindSensorResponse[4]
  // // Serial.print("Diretion="); //列印原始濕度值
  // // Serial.println(direction); //列印原始濕度值
  // Serial.print("Direction:");
  // Serial.println(direction/100);  //依據datasheet, 水分百分比為10倍,所以除以10
  // // Serial.println("%");
  // //============================


  delay(3000);

}