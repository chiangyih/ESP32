#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_SHT31.h>
#include <PubSubClient.h>
#include <NodeRemote.h>
#include "wifi_credentials.h"

// NodeRemote 裝置識別（必須與 NodeAnywhere 配對一致）
const char *CLAIM_TOKEN = "68w3iF8goqvLJzvOmclSAQt69mg7Wss4";
const char *DEVICE_UID = "PFTC_XD67_QG6K";
NodeRemote node(CLAIM_TOKEN, DEVICE_UID);

// const char *MQTT_BROKER = "mqttgo.io";
const char *MQTT_BROKER = "broker.emqx.io"; // 這是另一個公開 MQTT 伺服器
const uint16_t MQTT_PORT = 1883;
const char *TOPIC_TEMP = "/hhvs/cc/temp";
const char *TOPIC_HUMI = "/hhvs/cc/humi";

Adafruit_SHT31 sht31;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

WiFiCredentials wifiCredentials;
String mqttClientId;
bool shtReady = false;

// 非阻塞週期（毫秒）
const unsigned long WIFI_RETRY_MS = 10000;
const unsigned long MQTT_RETRY_MS = 3000;
const unsigned long SHT_RETRY_MS = 5000;
const unsigned long PUBLISH_INTERVAL_MS = 20000; 
const unsigned long NODE_ERR_LOG_MS = 30000;

unsigned long lastWiFiAttemptMs = 0;
unsigned long lastMqttAttemptMs = 0;
unsigned long lastShtAttemptMs = 0;
unsigned long lastPublishMs = 0;
unsigned long lastNodeErrLogMs = 0;

void connectWiFiIfNeeded(unsigned long now) // 這裡不直接呼叫 WiFi.begin()，而是透過 NodeRemote 的 wifiManaged 功能來維護連線，避免阻塞
{
  if (WiFi.status() == WL_CONNECTED || now - lastWiFiAttemptMs < WIFI_RETRY_MS)
  {
    return;
  }

  lastWiFiAttemptMs = now;
  WiFi.begin(wifiCredentials.ssid.c_str(), wifiCredentials.password.c_str());
  // Serial.println("Wi-Fi reconnect requested");
}

void connectMqttIfNeeded(unsigned long now) // 這裡不直接呼叫 mqttClient.connect()，而是透過 PubSubClient 的 loop() 來維護連線，避免阻塞
{
  if (WiFi.status() != WL_CONNECTED || mqttClient.connected() || now - lastMqttAttemptMs < MQTT_RETRY_MS)
  {
    return;
  }

  lastMqttAttemptMs = now;

  if (mqttClientId.length() == 0)
  {
    uint64_t mac = ESP.getEfuseMac();
    mqttClientId = "esp32-" + String((uint32_t)(mac >> 32), HEX) + String((uint32_t)mac, HEX);
  }

  if (mqttClient.connect(mqttClientId.c_str()))
  {
    // Serial.println("MQTT connected");
  }
  else
  {
    // Serial.print("MQTT connect failed, state=");
    // Serial.println(mqttClient.state());
  }
}

void initShtIfNeeded(unsigned long now) // SHT31 初始化可能會失敗，這裡做簡單的節流重試，避免不斷印出錯誤訊息
{
  if (shtReady || now - lastShtAttemptMs < SHT_RETRY_MS)
  {
    return;
  }

  lastShtAttemptMs = now;
  shtReady = sht31.begin(0x44);

  if (shtReady)
  {
    // Serial.println("SHT31 ready");
  }
  else
  {
    // Serial.println("SHT31 init failed, will retry");
  }
}

// 針對 NodeRemote 錯誤做節流提示，避免訊息淹沒
void logNodeRemoteErrorIfNeeded(unsigned long now)
{
  if (now - lastNodeErrLogMs < NODE_ERR_LOG_MS)
  {
    return;
  }

  const String err = node.lastError();
  if (err.length() == 0)
  {
    return;
  }

  lastNodeErrLogMs = now;

  if (err == "claim_permanently_failed")
  {
    // Serial.println("NodeRemote claim permanently failed. Create NEW CLAIM_TOKEN + DEVICE_UID in NodeAnywhere and reflash.");
    return;
  }

  // Serial.print("NodeRemote status: ");
  // Serial.println(err);
}

void publishSensorData(unsigned long now) // 定期發佈感測器數據，前提是 SHT31 已經準備好且 MQTT 連線正常
{
  if (!shtReady || !mqttClient.connected() || now - lastPublishMs < PUBLISH_INTERVAL_MS)
  {
    return;
  }

  lastPublishMs = now;

  float temperature = sht31.readTemperature();
  float humidity = sht31.readHumidity();
  if (isnan(temperature) || isnan(humidity)) // 讀取失敗，可能是 SHT31 短暫異常，這裡不發佈數據，等待下次重試
  {
    Serial.println("SHT31 read failed");
    return;
  }

  char tempPayload[16]; 
  char humiPayload[16];
  dtostrf(temperature, 0, 2, tempPayload); // 將浮點數轉為字串，保留兩位小數
  dtostrf(humidity, 0, 2, humiPayload);

  bool tempOk = mqttClient.publish(TOPIC_TEMP, tempPayload); // 發佈溫度數據
  bool humiOk = mqttClient.publish(TOPIC_HUMI, humiPayload);  // 發佈濕度數據

  // Serial.print("Temp ");
  // Serial.print(tempPayload);
  // Serial.print(" -> ");
  // Serial.print(TOPIC_TEMP);
  // Serial.print(tempOk ? " [OK]" : " [FAIL]");
  // Serial.print(" | Humi ");
  // Serial.print(humiPayload);
  // Serial.print(" -> ");
  // Serial.print(TOPIC_HUMI);
  // Serial.println(humiOk ? " [OK]" : " [FAIL]");
}

void setup()
{
  Serial.begin(9600);

  wifiCredentials = getWiFiCredentials(); // 從 wifi_credentials.h 取得 Wi-Fi 資訊

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false); // 這個專案主要是定期發佈感測器數據，沒有頻繁的用戶互動，因此關閉 Wi-Fi 省電模式，確保連線穩定

  // 保留 NodeRemote 的 Wi-Fi 管理設定
  node.setWifiManaged(true);
  node.wifiAdd(wifiCredentials.ssid.c_str(), wifiCredentials.password.c_str(), 1);

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT); // 設定 PubSubClient 的 MQTT 伺服器資訊
  Wire.begin(21, 22); // ESP32 的 I2C 預設腳位是 21 (SDA) 和 22 (SCL)，這裡顯式呼叫以確保正確初始化

  // 只呼叫一次 begin，後續由 node.loop() 持續維護
  if (!node.begin()) // NodeRemote 初始化失敗，可能是憑證問題或其他狀況，這裡不阻塞重試，而是透過 loop 中的 logNodeRemoteErrorIfNeeded 來定期印出錯誤訊息
  {
    // Serial.print("NodeRemote startup: ");
    // Serial.println(node.lastError());
  }
  else
  {
    // Serial.println("NodeRemote connected");
  }
}

void loop()
{
  const unsigned long now = millis();

  node.loop(); // NodeRemote 的 loop 會處理 MQTT 連線維護、後台任務等，這裡確保它被定期呼叫
  logNodeRemoteErrorIfNeeded(now); // 定期檢查 NodeRemote 狀態並印出錯誤訊息（如果有的話）

  connectWiFiIfNeeded(now); // 透過 NodeRemote 的 wifiManaged 功能來維護 Wi-Fi 連線，避免直接呼叫 WiFi.begin() 導致阻塞
  connectMqttIfNeeded(now);
  initShtIfNeeded(now);

  mqttClient.loop(); // PubSubClient 的 loop 會處理 MQTT 連線維護等，這裡確保它被定期呼叫
  publishSensorData(now); // 定期讀取 SHT31 感測器數據並發佈到 MQTT，前提是 SHT31 已經準備好且 MQTT 連線正常
}
