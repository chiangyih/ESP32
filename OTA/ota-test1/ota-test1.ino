#include <WiFi.h>
#include <ArduinoOTA.h>

const char* ssid="hhvs-iot";
const char* password="";

void setup() 
{
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED ) 
  {
    Serial.print(".");
    delay(500);  
  } 
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.setHostname("ESP32-OTA-test");
  ArduinoOTA.setPassword("1111");
  ArduinoOTA.begin();

}

void loop() 
{
  ArduinoOTA.handle(); 

}
