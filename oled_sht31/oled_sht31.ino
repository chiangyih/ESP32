// 使用 SHT31 by Rob Tillaart(rob.tillarrt@gmail.com)的library
#include "Wire.h"
#include "U8g2lib.h"
#include "SHT31.h"

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
