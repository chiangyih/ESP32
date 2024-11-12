#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h> 

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


// put function declarations here:
// int myFunction(int, int);

void setup() 
{
  // put your setup code here, to run once:
  // int result = myFunction(2, 3);
  Serial.begin(115200);

  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenR10_tf);
  u8g2.setFontRefHeightExtendedText(); //設定字體高度, 以便可以使用u8g2.getDisplayHeight()取得顯示器高度
  u8g2.clearBuffer();
  u8g2.setDrawColor(1); //1的顏色為白色
  u8g2.setFontDirection(0); //0為正常方向
  u8g2.setFontPosTop(); //設定字體位置為上方

}

void loop() 
{
  String data;
  data=Serial.readString(); //讀取序列埠資料, 並存入str字串

  u8g2.setCursor(0, 0); //設定游標位置, 0,0為左上角
  u8g2.print(String("abc").c_str()); //顯示字串, 並轉換為c字串

  u8g2.setCursor(0, 15); //設定游標位置
  u8g2.print(data); //顯示字串, 並轉換為c字串

 

  u8g2.sendBuffer(); //將緩衝區內容送到顯示器
  u8g2.clearBuffer(); //清除緩衝區內容


  
}

// put function definitions here:
// int myFunction(int x, int y) {
//   return x + y;
// }