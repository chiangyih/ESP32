//ESP32內建HAll元件接於GPIO36、39，使用時此兩腳勿接其它裝置

#include <Arduino.h>

void setup()
{
  
}

void loop() 
{
  int hall=hallRead(); //讀取內建hall感測值，放到hall變數
  printf("%d\n",hall);
  vTaskDelay(1000);

}