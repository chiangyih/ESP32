#include "soc/soc.h"  //解決 brownout detector問題
#include "soc/rtc_cntl_reg.h"  //解決 brownout detector問題
#include<ArduinoOTA.h> //處理OTA的函式庫
#include "WiFi.h"

// ------ 以下修改成你自己的WiFi帳號密碼 ------
char ssid[] = "IE-office";
char password[] = "20632063";

void setup() {

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector解決 brownout detector問題
  Serial.begin(115200);
  //開始WiFi連線
  WifiConnecte();  

    //====處理OTA====
  ArduinoOTA.setHostname("HHVS_IE_Iot"); //定義主機名稱
  ArduinoOTA.setPassword("20632063"); //定義密碼
  ArduinoOTA.begin();  //啟用
  //==============

}

void loop() {
  ArduinoOTA.handle();  //處理OTA任務

}

//WiFi連線副程式
void WifiConnecte() {
  //開始WiFi連線
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi連線成功");
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());
}
