// 使用 SHT31 溫溼度感測器
// ESP32 I2C 腳位: SDA = 21, SCL = 22

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_SHT31.h>
#include "wifi_credentials.h"

// 建立 SHT31 感測器物件
Adafruit_SHT31 sht31;

void connectWiFi()
{
  // 透過獨立函式取得 Wi-Fi 帳密
  WiFiCredentials credentials = getWiFiCredentials();

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(credentials.ssid.c_str(), credentials.password.c_str());

  Serial.print("Wi-Fi 連線中");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Wi-Fi 已連線，IP: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  // 啟動序列埠，供終端機顯示量測結果
  Serial.begin(9600);
  delay(1000);

  // 初始化 I2C，指定 ESP32 的 SDA 與 SCL 腳位
  Wire.begin(21, 22);

  // 啟動 SHT31，預設 I2C 位址為 0x44
  if (!sht31.begin(0x44))
  {
    Serial.println("SHT31 初始化失敗，請檢查接線與 I2C 位址。");

    // 若初始化失敗則停止執行，避免後續持續讀取錯誤資料
    while (true)
    {
      delay(1000);
    }
  }

  // 連線到 Wi-Fi
  connectWiFi();

  Serial.println("SHT31 初始化完成。");
  Serial.println("每 3 秒輸出一次溫度與濕度。");
}

void loop()
{
  // 若 Wi-Fi 斷線則自動重新連線
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Wi-Fi 已斷線，重新連線中...");
    WiFi.disconnect();
    connectWiFi();
  }

  // 讀取溫度與濕度
  float temperature = sht31.readTemperature();
  float humidity = sht31.readHumidity();

  // 若回傳不是數值，表示本次讀取失敗
  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("讀取 SHT31 失敗。");
  }
  else
  {
    // 將量測結果輸出到序列終端機
    Serial.print("Temperature: ");
    Serial.print(temperature, 2);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity, 2);
    Serial.println(" %");
  }

  // 每 3 秒量測一次
  delay(3000);
}
