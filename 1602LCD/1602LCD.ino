/*
本程式為LCD 1602的範例
需安裝LiquidCrystal_PCF8574的函式庫
*/

// esp32 GPIO21=> SDA
// esp32 GPIO22=> SCL

#include <Wire.h>  //變更預設I2C接角用
#include <LiquidCrystal_PCF8574.h>




LiquidCrystal_PCF8574 lcd(0x27);  // 設定i2c位址，一般情況就是0x27和0x3F兩種

void setup()
{
 


  lcd.begin(16, 2); // 初始化LCD
  //lcd.begin(20, 4); // 如果是20x4的LCD就是用這行
  
  lcd.setBacklight(255);
  lcd.clear();
  lcd.setCursor(0, 0);  //設定游標位置 (字,行)
  lcd.print("*~ first line.");
  lcd.setCursor(0, 1);
  lcd.print("~* second line.");
} 

void loop()
{
 
} // loop()
