#include "Wire.h"
#include "U8g2lib.h"

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup()
{
  u8g2.begin();
  u8g2.setFont(u8g2_font_8x13O_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);


}

void loop()
{
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 0);
    u8g2.print(String("HELLO_World!").c_str());
    u8g2.drawLine(0, 11, 60, 11);

    u8g2.sendBuffer();
  } while (u8g2.nextPage());
}
