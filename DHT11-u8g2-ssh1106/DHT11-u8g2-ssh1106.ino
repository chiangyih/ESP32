
#include <Arduino.h>
#include <U8g2lib.h>
#include "DHT.h"
#include <TimeLib.h>

//==定義 U8g2==
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
//=============

//==使用SSH1106 oled===
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//====================

// LED & DHT11
#define DHTPIN  23                        
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);




void setup(void) {
  u8g2.begin();
  u8g2.enableUTF8Print(); //enable UTF-8中文
  Serial.begin(115200);  // Set software serial baud to 115200;

  u8g2.clear();
  u8g2.clearBuffer();
  u8g2.setCursor(5,30);
  
  u8g2.setFont(u8g2_font_unifont_t_chinese3);
  
  u8g2.print(" 讀取");
  u8g2.sendBuffer();
  delay(3000);
  u8g2.clear();

}

void loop(void) {
  //u8g2.clearBuffer();					// clear the internal memory
  //u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  //u8g2.drawStr(0,10,"Hello World!");	// write something to the internal memory
  //u8g2.sendBuffer();					// transfer internal memory to the display
  //delay(1000);  
  //u8g2.clear();
  
  delay(2000); // Wait a few seconds between measurements.
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Read temperature as Celsius (the default)
  float f = dht.readTemperature(true); // Read temperature as Fahrenheit (isFahrenheit = true)

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) 
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  if ( second()%10 ==0)
  {
    //client.publish(pub_topic,String(t).c_str());
    Serial.print("Temperature:");
    Serial.println(t);
    
    u8g2.clear();
    u8g2.clearBuffer();
    u8g2.setCursor(5,20);
    u8g2.setFont(u8g2_font_unifont_t_chinese1);     
    u8g2.print("Temp:");
    u8g2.print(t);
    u8g2.print("°C");

    u8g2.setCursor(5,50);
    u8g2.setFont(u8g2_font_unifont_t_chinese1);     
    u8g2.print("Humi:");
    u8g2.print(h);
    u8g2.print("%");
    u8g2.sendBuffer();     
  }
  

  
}
