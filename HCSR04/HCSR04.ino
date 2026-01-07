int trig=12;
int echo=14;
void setup() 
{
  Serial.begin(115200);
  Serial2.begin(9600);
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
}

void loop() 
{
  digitalWrite(trig,LOW);  //關閉
  delayMicroseconds(5);
  digitalWrite(trig,HIGH);  //啟動超音波
  delayMicroseconds(10);
  digitalWrite(trig,LOW);  //關閉
  
  float echotime=pulseIn(echo,HIGH);  //計算回傳時間
  float cmValue=echotime/29.4/2;      //將時間轉成距離

  Serial.print("Distance=");
  Serial.println(cmValue);
  Serial2.println(cmValue);
  delay(500);
  
}
