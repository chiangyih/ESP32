// Tseng 2024/11/12
// 使用JXBS-3001-TR-RS模組讀取土壤濕度感測器的數值, 並透過nodemcu32-s的Serial2 port讀取 
// 使用nodemcu32-s的Serial port輸出數值到終端

#include <HardwareSerial.h>
#include <Arduino.h>

#define rxa 16
#define txa 17
#define BAUD_RATE 9600

// JXBS-3001-TR-RS模組的modbus協定 地址:1byte, 功能碼:1byte, 起始位址:2bytes, 資料長度:2bytes, CRC低位元:1bytes CRC高位元:1bytes
const byte soilSensorRequest[] = {0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x65, 0xCB};   // 請求土壤濕度資料, 8 bytes, 01 03 00 02 00 02 65 CB(依據sensor的modbus協定，參考datasheet)

//回應資料格式 地址:1byte, 功能碼:1byte, 有效字節(資料長度):1byte, 第一數據區(濕度值):2bytes, 第二數據區(溫度值):2bytes, CRC:2bytes
byte soilSensorResponse[9];   // 回應土壤溫濕度資料,共9 bytes, 例如:01 03 04 02 92 FF 9B 5A 3D, 濕度值為292hex=658dec(index[3]與[4]),溫度值為FF9Bhex=-101dec(index[5]與[6]) 


void setup() 
{
  //宣告預設的Serial port初始化
  Serial.begin(BAUD_RATE);
  //宣告Serial2 port初始化
  Serial2.begin(BAUD_RATE, SERIAL_8N1, rxa, txa); 
}

void loop() {
  //傳送request給土壤濕度感測器
  Serial2.write(soilSensorRequest, sizeof(soilSensorRequest));

  //檢查回覆的資料是否有9 bytes
  while (Serial2.available() < 9)
  {
    delay(1000);
  }

  //如果Serial2 port有等待讀取的資料，資料長度大於等於9 bytes
  if (Serial2.available() >= 9)
  {
    //讀取Serial2 port的資料到soilSensorResponse
   byte index = 0; // index of soilSensorResponse

  //===========依index順序將讀取到的資料印出(index[0]~index[9])=============
   while (Serial2.available() && index < 9)
   {
     soilSensorResponse[index] = Serial2.read(); // read the byte from Serial2 port     
     Serial.print(soilSensorResponse[index], HEX); // print the byte in HEX
     Serial.print(" "); // print a space     
     index++; // increment the index
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

  delay(3000);

}
