#include <Arduino.h>
#include<BluetoothSerial.h>

BluetoothSerial bt;
char* pin = "1111"; //設定BT配對碼
//String val; //儲存接收資料的變數
char val;

int in1=12;  //DIO12
int in2=13;  //DIO13
int in3=14;  //DIO14
int in4=15;  //DIO15
int en=27;   //DIO27;pwm用

const int freq=50000; //pwm頻率
const int pwmchannel=0;  //pwm通道
const int resolution=10;  //解析度(位元)
int dutycyle=150;  //初始工作週期(解析度)


void setup() 
{
  Serial.begin(9600);
  bt.begin(9600);
  pinMode(LED_BUILTIN,OUTPUT);
  bt.setPin(pin);
  bt.begin("Speed Control"); //藍芽顯示名稱

  byte macBT[6]; //宣告儲存mac位址的陣列
  esp_read_mac(macBT,ESP_MAC_BT); //讀取 BT mac address
  Serial.printf("藍芽MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",macBT[0],macBT[1],macBT[2],macBT[3],macBT[4],macBT[5]);

  // servo motor pwm
  pinMode(in1,OUTPUT);  
  pinMode(in2,OUTPUT);  
  pinMode(in3,OUTPUT);  
  pinMode(in4,OUTPUT);  
  ledcSetup(pwmchannel,freq,resolution);  //設定pwm的通道、頻率與解析度；esp32有16個通道可用
  ledcAttachPin(en,pwmchannel);   //設定pwm輸出腳位
  
  
  stop();  //====全數馬達停止

 
}

void loop() 
{
  if (bt.available())
  {
    //val=bt.readString();
    val=bt.read();
    switch (val)
    {
    case '1':
      stop();
      forward(900);
      break;
    
    case '2':
      stop();
      revers(900);
      break;

    case '4':
      stop();
      rightmotor_f(800);
      break;
    case '3':
      stop();
      leftmotor_f(800);
      break;

    default:
      stop();
      break;
    }
  }

  
}

void leftmotor_f(int speed)
{
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  ledcWrite(pwmchannel,speed);
}
void leftmotor_r(int speed)
{
  digitalWrite(in2,HIGH);
  digitalWrite(in1,LOW);
  ledcWrite(pwmchannel,speed);
}

void rightmotor_f(int speed)
{
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
  ledcWrite(pwmchannel,speed);
}
void rightmotor_r(int speed)
{
  digitalWrite(in4,HIGH);
  digitalWrite(in3,LOW);
  ledcWrite(pwmchannel,speed);
}



void forward(int speed) 
{
  leftmotor_f(speed);
  rightmotor_f(speed);
}

void revers(int speed)
{
  leftmotor_r(speed);
  rightmotor_r(speed);

}

void stop()
{
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  ledcWrite(pwmchannel,0);
}

