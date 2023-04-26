#include "Wire.h"
#include "U8g2lib.h"
#include <DHT.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define DHTPIN 23 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);

  dht.begin();
}

void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  u8g2.clearBuffer();
  u8g2.setCursor(0, 0);
  u8g2.print(String("Temperature: ").c_str());
  u8g2.print(String(t).c_str());
  u8g2.print(String("*C").c_str());

  u8g2.setCursor(0, 20);
  u8g2.print(String("Humidity: ").c_str());
  u8g2.print(String(h).c_str());
  u8g2.print(String("%").c_str());
  
  u8g2.sendBuffer();

}
