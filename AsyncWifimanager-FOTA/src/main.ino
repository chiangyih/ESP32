#include <AsyncElegantOTA.h>
const char *ssid = "........";
const char *password = "........";
 
/****************************************************************************************************************************
  Async_AutoConnect_ESP32_minimal.ino
  For ESP8266 / ESP32 boards
  Built by Khoi Hoang https://github.com/khoih-prog/ESPAsync_WiFiManager
  Licensed under MIT license
 *****************************************************************************************************************************/
#if !(defined(ESP32))
#error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
#endif
#include <ESPAsync_WiFiManager.h>
AsyncWebServer webServer(80);
 
DNSServer dnsServer;
 
 
void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    while (!Serial)
        ;
    delay(200);
    Serial.print("\nStarting Async_AutoConnect_ESP32_minimal on " + String(ARDUINO_BOARD));
    Serial.println(ESP_ASYNC_WIFIMANAGER_VERSION);
 
    ESPAsync_WiFiManager ESPAsync_wifiManager(&webServer, &dnsServer, "Async_AutoConnect");
 
    //ESPAsync_wifiManager.resetSettings();   //reset saved settings
    //ESPAsync_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 132, 1), IPAddress(192, 168, 132, 1), IPAddress(255, 255, 255, 0));
    ESPAsync_wifiManager.autoConnect("MyESP32S");
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.print(F("Connected. Local IP: "));
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println(ESPAsync_wifiManager.getStatus(WiFi.status()));
    }
 
    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(200, "text/plain", "Hi! I am MyESP32S."); });
 
    AsyncElegantOTA.begin(&webServer); // Start ElegantOTA
    webServer.begin();
    Serial.println("HTTP server started");
}
 
void loop() 
{
  
}