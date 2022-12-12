#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN        16
#define NUMPIXELS 8

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 100

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();
}

void loop() {
  pixels.clear();

  for(int i=0; i<NUMPIXELS; i++) 
  {
    pixels.setPixelColor(i, pixels.Color(150, 0, 0));
    pixels.show();
    delay(DELAYVAL);
  }
  for(int i=NUMPIXELS; i>=0; i--) 
  {
    pixels.setPixelColor(i, pixels.Color(0,150, 0));
    pixels.show();
    delay(DELAYVAL);
  }


  
  
}