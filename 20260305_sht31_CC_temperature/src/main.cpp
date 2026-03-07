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

// 額外 MQTT 發佈到 mqttgo.io
const char *MQTT_BROKER = "mqttgo.io";
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
const unsigned long PUBLISH_INTERVAL_MS = 10000;
const unsigned long NODE_ERR_LOG_MS = 30000;

unsigned long lastWiFiAttemptMs = 0;
unsigned long lastMqttAttemptMs = 0;
unsigned long lastShtAttemptMs = 0;
unsigned long lastPublishMs = 0;
unsigned long lastNodeErrLogMs = 0;

void connectWiFiIfNeeded(unsigned long now)
{
  if (WiFi.status() == WL_CONNECTED || now - lastWiFiAttemptMs < WIFI_RETRY_MS)
  {
    return;
  }

  lastWiFiAttemptMs = now;
  WiFi.begin(wifiCredentials.ssid.c_str(), wifiCredentials.password.c_str());
  Serial.println("Wi-Fi reconnect requested");
}

void connectMqttIfNeeded(unsigned long now)
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
    Serial.println("MQTT connected");
  }
  else
  {
    Serial.print("MQTT connect failed, state=");
    Serial.println(mqttClient.state());
  }
}

void initShtIfNeeded(unsigned long now)
{
  if (shtReady || now - lastShtAttemptMs < SHT_RETRY_MS)
  {
    return;
  }

  lastShtAttemptMs = now;
  shtReady = sht31.begin(0x44);

  if (shtReady)
  {
    Serial.println("SHT31 ready");
  }
  else
  {
    Serial.println("SHT31 init failed, will retry");
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
    Serial.println("NodeRemote claim permanently failed. Create NEW CLAIM_TOKEN + DEVICE_UID in NodeAnywhere and reflash.");
    return;
  }

  Serial.print("NodeRemote status: ");
  Serial.println(err);
}

void publishSensorData(unsigned long now)
{
  if (!shtReady || !mqttClient.connected() || now - lastPublishMs < PUBLISH_INTERVAL_MS)
  {
    return;
  }

  lastPublishMs = now;

  float temperature = sht31.readTemperature();
  float humidity = sht31.readHumidity();
  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("SHT31 read failed");
    return;
  }

  char tempPayload[16];
  char humiPayload[16];
  dtostrf(temperature, 0, 2, tempPayload);
  dtostrf(humidity, 0, 2, humiPayload);

  bool tempOk = mqttClient.publish(TOPIC_TEMP, tempPayload);
  bool humiOk = mqttClient.publish(TOPIC_HUMI, humiPayload);

  Serial.print("Temp ");
  Serial.print(tempPayload);
  Serial.print(" -> ");
  Serial.print(TOPIC_TEMP);
  Serial.print(tempOk ? " [OK]" : " [FAIL]");
  Serial.print(" | Humi ");
  Serial.print(humiPayload);
  Serial.print(" -> ");
  Serial.print(TOPIC_HUMI);
  Serial.println(humiOk ? " [OK]" : " [FAIL]");
}

void setup()
{
  Serial.begin(9600);

  wifiCredentials = getWiFiCredentials();

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);

  // 保留 NodeRemote 的 Wi-Fi 管理設定
  node.setWifiManaged(true);
  node.wifiAdd(wifiCredentials.ssid.c_str(), wifiCredentials.password.c_str(), 1);

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  Wire.begin(21, 22);

  // 只呼叫一次 begin，後續由 node.loop() 持續維護
  if (!node.begin())
  {
    Serial.print("NodeRemote startup: ");
    Serial.println(node.lastError());
  }
  else
  {
    Serial.println("NodeRemote connected");
  }
}

void loop()
{
  const unsigned long now = millis();

  node.loop();
  logNodeRemoteErrorIfNeeded(now);

  connectWiFiIfNeeded(now);
  connectMqttIfNeeded(now);
  initShtIfNeeded(now);

  mqttClient.loop();
  publishSensorData(now);
}
