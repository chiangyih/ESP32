#include <Arduino.h>
#include<BluetoothSerial.h>

BluetoothSerial bt;
char* pin = "1111"; //設定BT配對碼
char val; //儲存接收資料的變數

void setup() 
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN,OUTPUT);
  bt.setPin(pin);
  bt.begin("bt classic");

  byte macBT[6]; //宣告儲存mac位址的陣列
  esp_read_mac(macBT,ESP_MAC_BT); //讀取 BT mac address
  Serial.printf("藍芽MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",macBT[0],macBT[1],macBT[2],macBT[3],macBT[4],macBT[5]);
 
}

void loop() 
{
  if (bt.available())
  {
    val=bt.read();
    switch (val)
    {
    case '0':
      digitalWrite(LED_BUILTIN,LOW);
      bt.println('turn off light');
      break;
    case '1':
      digitalWrite(LED_BUILTIN,HIGH);
      bt.println('a');
      break; 
    }
  }
  
}
