#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN  6 

// 定義使用多少顆LED
#define NUMPIXELS 8 
#define LED_COUNT  8

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define BRIGHTNESS 200
#define DELAYVAL 100 // 每顆燈的延遲時間
Adafruit_NeoPixel strip(LED_COUNT, PIN, NEO_GRBW + NEO_KHZ800);
void setup() 
{
  //pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  //pixels.clear(); // Set all pixel colors to 'off'
  strip.begin();
  strip.show();
  strip.setBrightness(10);
}
// ==============rainbow副程式=================
void rainbow(int wait) {
   for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) 
    { 
       int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
       strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); 
    delay(wait); 
  }
}
// ======================================
// ===========colorWipe副程式============
void colorWipe(uint32_t color, int wait)
{
  for(int i=0; i<strip.numPixels(); i++) 
  { 
    strip.setPixelColor(i, color);         
    strip.show();                          
    delay(wait);                           
  }
}
// ======================================
// =========TheaterChase副程式===========
void theaterChase(uint32_t color, int wait) 
{
  for(int a=0; a<10; a++)  // Repeat 10 times...
  {  
    for(int b=0; b<3; b++)  
    { 
      strip.clear();         
      for(int c=b; c<strip.numPixels(); c += 3)  
      {
        strip.setPixelColor(c, color);           // Set pixel 'c' to value 'color'
      }
      strip.show(); 
      delay(wait);  
    }
  }
}
// ======================================
// =========TheaterChaseRainbow副程式=========
void theaterChaseRainbow(int wait) 
{
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++)    // Repeat 30 times...
  {  
    for(int b=0; b<3; b++)   
    { 
      strip.clear();         
      for(int c=b; c<strip.numPixels(); c += 3) 
      {
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue));              // hue -> RGB
        strip.setPixelColor(c, color);                                    // Set pixel 'c' to value 'color'
      }
      strip.show();                
      delay(wait);                 
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
// ===========================================


// ======主程式==============
void loop() 
{
    rainbow(2);                                     //rainbow效果;設置執行時間(sec)
    colorWipe(strip.Color(0,   0,   255), 500);     //colorWipe效果(顏色,時間)
    theaterChase(strip.Color(200, 200, 0), 100);   //theaterChase效果(顏色,時間)
    theaterChaseRainbow(100);                      //theaterChaseRainbow效果(時間)
   // ======來回流動效果=================== 
   for(int i=0; i<NUMPIXELS; i++) 
  { 
    pixels.setPixelColor(i, pixels.Color(200, 0, 0));  // setup color
    pixels.show();   
    delay(DELAYVAL); // set delay time
  }
  
  for(int i=NUMPIXELS; i>=0; i--)
  {
   pixels.setPixelColor(i, pixels.Color(0, 200, 0));   //setup color
   pixels.show();   
   delay(DELAYVAL); 
  }
  // ========================================
  
}
