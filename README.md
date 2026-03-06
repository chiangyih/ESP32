# ESP32 專題實作教學

> **更新日期：** 2026年1月7日  
> **Author:** Tseng



> 📖 **線上教學文件**：[https://hackmd.io/@johnt/esp32](https://hackmd.io/@johnt/esp32)

---

## 📋 目錄

- [硬體需求](#-硬體需求)
- [軟體環境設定](#️-開發工具與環境)
- [學習路徑](#-目錄架構與學習路徑)
- [常用函式庫清單](#-常用函式庫清單)
- [完整專案列表](#-全資料夾功能清單-按字母排序)
- [快速開始](#-快速開始指南)
- [除錯技巧](#-除錯技巧)
- [參考資源](#-參考資源)

---

## 💡 硬體需求

### 必備硬體
- **ESP32 開發板** (如 ESP32-DevKitC、ESP32-WROOM、NodeMCU-32S 等)
- **USB 傳輸線** (Micro-USB 或 Type-C，依開發板而定)
- **麵包板與杜邦線** (用於電路連接)

### 常用感測器與模組
| 類型 | 推薦型號 | 用途 |
| :--- | :--- | :--- |
| 溫溼度感測器 | DHT11、DHT22、SHT31 | 環境監測 |
| 顯示器 | SSD1306 OLED、SSH1106、LCD1602 | 數據呈現 |
| 馬達驅動 | L298N | 直流馬達控制 |
| 距離感測 | HC-SR04 | 超音波測距 |
| 空氣品質 | GP2Y1014AU、PMS5003 | PM2.5 偵測 |
| 土壤感測 | RS485 Modbus | 農業自動化 |
| 伺服馬達 | SG90、MG996R | 角度控制 |
| RGB 燈條 | WS2812B | 彩燈效果 |

---

## 📁 目錄架構與學習路徑

### 1. 基礎入門篇 (Level 1: Basic)
從基本的 GPIO 控制與內建功能開始，熟悉開發環境。
*   [blink](blink/blink.ino): 控制內建或外接 LED 閃爍，確認環境設定正確。
*   [touch_pin-test](touch_pin-test/): 學習使用 ESP32 內建的電容式觸控腳位。
*   [vscode-Builtin_Hall](vscode-Builtin_Hall/): 讀取 ESP32 內建的霍爾感測器（磁力感應）。
*   [multiTask](multiTask/): **進階必學**。學習 FreeRTOS 任務分配，讓 ESP32 同時處理多個任務（如同時讀取感測器與處理網路請求）。

### 2. 顯示技術應用 (Level 2: Display)
學習各類顯示模組的驅動與排版。
*   **OLED 螢幕**: [oled](oled/)、[oled_sht31](oled_sht31/)、[WeMos 0.96寸OLED](WeMos%200.96寸OLED%20ESP32/)。使用 I2C 介面。
*   **LCD 1602**: [1602LCD](1602LCD/1602LCD.ino)。經典的文字型顯示器。
*   **LED 點矩陣**: [8X8_LED](8X8_LED/)。使用 MAX7219 驅動，適合製作跑馬燈或簡單動畫。
*   **七段顯示器**: [esp32-TM1637_7seg](esp32-TM1637_7seg/)。常見於定時器或數值顯示。

### 3. 多樣化感測器實務 (Level 3: Sensors)
掌握環境數據的擷取方法。
*   **溫溼度感應**: [DHT22_mqtt](DHT22_mqtt/)、[SHT31_溫溼度](SHT31_溫溼度/)。
*   **土壤與農業偵測 (進階 RS485 介面)**:
    *   [土壤偵測-Soil_sn-3001-tr-ecthph](土壤偵測-Soil_sn-3001-tr-ecthph/)
    *   [土壤溫溼度sensor(485介面)](土壤溫溼度sensor(485介面)JXBS-3001-TR-RS/)
    *   學習 Modbus RTU 協議與 485 訊號轉型，這是工業級感測器的標準。
*   **空氣品質**: [灰塵感測器](灰塵感測器/GP2Y1014AU/)、[getPM25](getPM25/)。
*   **距離偵測**: [HCSR04](HCSR04/) 超音波感測器。

### 4. 無線通訊與雲端串接 (Level 4: Connectivity)
這是 ESP32 的強項，學習如何將數據上傳與遠端控制。
*   **WiFi 連線**: [WiFi_connect](WiFi_connect/)、[scanWifi](scanWifi/)、[AsyncWifimanager-FOTA](AsyncWifimanager-FOTA/)。
*   **MQTT 協定**: [vscode-mqtt](vscode-mqtt/)。IoT 最核心的通訊協議，用於設備間的溝通。
*   **藍牙應用**: [vscode-bt_esp32](vscode-bt_esp32/)、[esp32-bt_test20251117](esp32-bt_test20251117/)。包含經典藍牙與 BLE 控制。
*   **遠端更新 (OTA)**: [OTA](OTA/)、[AsyncWifimanager-FOTA](AsyncWifimanager-FOTA/)。學習無需連接傳輸線即可更新程式碼。

### 5. 動力控制與機器人 (Level 5: Actuators)
*   **馬達驅動**: [vscode-L298N](vscode-L298N/)、[vscode-L298N-speed-control](vscode-L298N-speed-control/)。
*   **藍牙自走車**: [vscode-bt-simpleCar](vscode-bt-simpleCar/)。整合馬達、藍牙與 ESP32 的綜合應用。
*   **伺服馬達**: [servo-test](servo-test/)。

### 6. 綜合整合專案 (Level 6: Integrated Projects)
*   **[自動化農業](自動化農業/)**: 整合溫溼度、土壤水分監測、抽水幫浦控制，並結合 Node-RED 儀表板進行遠端監控。
*   **[資訊科辦/電腦中心專案](資訊科辦用/)**: 針對特定場域開發的自動化系統，整合了 RTC 時間、OTA 更新、MQTT 回傳與 LCD 狀態顯示。

---

## 🛠️ 開發工具與環境

### 方案一：Arduino IDE (初學者推薦)
1. 下載並安裝 [Arduino IDE](https://www.arduino.cc/en/software)
2. 新增 ESP32 開發板支援：
   - 開啟「檔案 → 偏好設定」
   - 在「額外的開發板管理員網址」貼上：
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
   - 開啟「工具 → 開發板 → 開發板管理員」，搜尋 `ESP32` 並安裝
3. 選擇對應的開發板型號與連接埠
4. 直接開啟 `.ino` 檔案即可編譯上傳

### 方案二：VS Code + PlatformIO (進階推薦)
1. 安裝 [Visual Studio Code](https://code.visualstudio.com/)
2. 在擴充套件中搜尋並安裝 `PlatformIO IDE`
3. 開啟含有 `platformio.ini` 的專案資料夾
4. 優勢：
   - 更智慧的程式碼補全與語法檢查
   - 自動管理函式庫相依性
   - 支援多平台開發與測試
   - 更強大的除錯功能

### 驅動程式安裝
若電腦無法辨識 ESP32，請安裝對應的 USB 轉序列晶片驅動：
- **CP2102**: [Silicon Labs 官網](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
- **CH340**: [WCH 官網](http://www.wch-ic.com/downloads/CH341SER_ZIP.html)

## ⚡ 快速開始指南

### Step 1: 環境驗證 (5 分鐘)
從最簡單的 LED 控制開始：
```bash
1. 開啟 blink/blink.ino
2. 選擇開發板型號與連接埠
3. 按下上傳按鈕
4. 觀察板上 LED 是否閃爍
```
✅ **成功標準**：LED 每秒閃爍一次

### Step 2: 感測器讀取 (15 分鐘)
學習讀取環境數據：
```bash
1. 連接 DHT22 感測器 (VCC→3.3V, GND→GND, Data→GPIO4)
2. 開啟 DHT22_mqtt/ 專案
3. 開啟序列埠監控視窗 (鮑率 115200)
4. 觀察溫溼度數值變化
```
✅ **成功標準**：每 2 秒顯示一次溫度與溼度數值

### Step 3: 顯示器整合 (20 分鐘)
將數據顯示在螢幕上：
```bash
1. 連接 SSD1306 OLED (SDA→GPIO21, SCL→GPIO22)
2. 開啟 oled_sht31/ 專案
3. 上傳並觀察螢幕顯示
```
✅ **成功標準**：OLED 螢幕顯示感測器即時數值

### Step 4: 無線通訊 (30 分鐘)
讓 ESP32 連上網路：
```bash
1. 開啟 WiFi_connect/ 專案
2. 修改 WiFi SSID 與密碼
3. 上傳並觀察序列埠的 IP 位址
4. 進階：嘗試 vscode-mqtt/ 將數據發送至 MQTT Broker
```
✅ **成功標準**：成功取得區域網路 IP 並能 Ping 通

### Step 5: 綜合應用專案
參考以下成品專案，開發屬於你的系統：
- **[自動化農業](自動化農業/)**: 整合感測、控制與遠端監控
- **[藍牙自走車](vscode-bt-simpleCar/)**: 馬達控制與無線通訊
- **[智慧監測系統](資訊科辦用/)**: 包含 LCD、RTC、OTA 等完整功能

---

## 📂 全資料夾功能清單 (按字母排序)

| # | 資料夾名稱 | 功能說明 |
| :--- | :--- | :--- |
| 01 | [1602LCD](1602LCD/) | 驅動經典 16x2 LCD 文字型顯示器。 |
| 02 | [8X8_LED](8X8_LED/) | MAX7219 點矩陣控制，包含捲動文字與圖形。 |
| 03 | [AsyncWifimanager-FOTA](AsyncWifimanager-FOTA/) | 非同步 WiFi 管理員與 FOTA 網路更新整合。 |
| 04 | [blink](blink/) | 基礎 LED 閃爍，硬體測試起點。 |
| 05 | [Computer_center](Computer_center/) | 電腦中心監測系統，含 DHT22 與 RTC 應用。 |
| 06 | [DHT11-u8g2-ssh1106](DHT11-u8g2-ssh1106/) | 使用 u8g2 函式庫在 SSH1106 OLED 顯示 DHT11 數據。 |
| 07 | [dht11_mqtt_wifi](dht11_mqtt_wifi/) | DHT11 基本感測與 WiFi MQTT 上傳實作。 |
| 08 | [DHT22_mqtt](DHT22_mqtt/) | 溫溼度數據透過 MQTT 協定發送至伺服器。 |
| 09 | [DHT22_mqtt_computer-center-RTC](DHT22_mqtt_computer-center-RTC/) | 整合 RTC 實時時鐘的機房監測系統。 |
| 10 | [esp32-TM1637_7seg](esp32-TM1637_7seg/) | 控制 TM1637 四位數七段顯示器。 |
| 11 | [getNetworkTime](getNetworkTime/) | 透過 NTP 伺服器獲取並同步網路精準時間。 |
| 12 | [getPM25](getPM25/) | PM2.5 空氣品質感測數據讀取。 |
| 13 | [HCSR04](HCSR04/) | 超音波感測器測距實作。 |
| 14 | [multiTask](multiTask/) | FreeRTOS 多任務處理範例，示範平行處理。 |
| 15 | [oled](oled/) | 基本 SSD1306 OLED 螢幕驅動與繪圖教學。 |
| 16 | [oled_sht31](oled_sht31/) | I2C 介面 OLED 顯示器與 SHT31 感測器結合應用。 |
| 17 | [OTA](OTA/) | Over-the-Air 無線程式燒錄更新實作。 |
| 18 | [scanWifi](scanWifi/) | 掃描周遭 WiFi 熱點訊號強度 (RSSI)。 |
| 19 | [servo-test](servo-test/) | 伺服馬達 (Servo) 角度控制基礎。 |
| 20 | [SHT31_溫溼度](SHT31_溫溼度/) | 高精度 SHT31 溫溼度感應器讀取。 |
| 21 | [touch_pin-test](touch_pin-test/) | ESP32 電容觸控腳位讀值測試。 |
| 22 | [vscode-bt-simpleCar](vscode-bt-simpleCar/) | 藍牙遙控自走車成品專案。 |
| 23 | [vscode-bt_esp32](vscode-bt_esp32/) | ESP32 經典藍牙與電腦/手機通訊。 |
| 24 | [vscode-Builtin_Hall](vscode-Builtin_Hall/) | 讀取 ESP32 晶片內建的霍爾磁力感測器。 |
| 25 | [vscode-L298N](vscode-L298N/) | L298N 馬達驅動板基礎控制。 |
| 26 | [vscode-mqtt](vscode-mqtt/) | MQTT 基本發布 (Publish) 與訂閱 (Subscribe) 教學。 |
| 27 | [vscode-touchControl](vscode-touchControl/) | 利用觸控腳位控制 LED 或其他元件輸出。 |
| 28 | [WeMos 0.96寸OLED ESP32](WeMos%200.96寸OLED%20ESP32/) | 針對帶有內建螢幕的開發板之顯示設定。 |
| 29 | [WiFi_connect](WiFi_connect/) | 基本 WiFi STA 模式連線設定範例。 |
| 30 | [win_speed_direction](win_speed_direction/) | 風速與風向計感測實作。 |
| 31 | [ws2812](ws2812/) | WS2812B RGB 彩燈控制。 |
| 32 | [土壤偵測 (RS485型)](土壤偵測-Soil_sn-3001-tr-ecthph/) | 工業級五針土壤多參數變送器 (Modbus RTU)。 |
| 33 | [土壤溫溼度 (RS485)](土壤溫溼度sensor(485介面)JXBS-3001-TR-RS/) | 使用 485 介面的土壤溫溼度感測器。 |
| 34 | [自動化農業](自動化農業/) | 包含抽水幫浦、感測器與 MQTT 整合的完整系統。 |
| 35 | [資訊科辦用](資訊科辦用/) | 結合 Line Notify、LCD、RTC 的綜合自動化專案。 |
| 36 | [灰塵感測器](灰塵感測器/) | GP2Y1014AU PM2.5 細懸浮微粒偵測。 |

---

## 📚 常用函式庫清單

以下是專案中常用的 Arduino 函式庫，可在「草稿碼 → 匯入函式庫 → 管理函式庫」中搜尋安裝：

| 函式庫名稱 | 用途 | 安裝指令 (PlatformIO) |
| :--- | :--- | :--- |
| `DHT sensor library` | DHT11/DHT22 溫溼度感測器 | `lib_deps = adafruit/DHT sensor library` |
| `Adafruit SSD1306` | OLED 顯示器驅動 | `lib_deps = adafruit/Adafruit SSD1306` |
| `PubSubClient` | MQTT 協定通訊 | `lib_deps = knolleary/PubSubClient` |
| `WiFiManager` | 智慧 WiFi 配置 | `lib_deps = tzapu/WiFiManager` |
| `ArduinoJson` | JSON 資料解析 | `lib_deps = bblanchon/ArduinoJson` |
| `U8g2` | 進階 OLED 圖形函式庫 | `lib_deps = olikraus/U8g2` |
| `TM1637` | 七段顯示器驅動 | `lib_deps = avishorp/TM1637` |
| `FastLED` | WS2812B RGB 燈條 | `lib_deps = fastled/FastLED` |
| `ModbusMaster` | Modbus RTU 通訊協定 | `lib_deps = 4-20ma/ModbusMaster` |
| `NTPClient` | 網路時間同步 | `lib_deps = arduino-libraries/NTPClient` |
| `RTClib` | 即時時鐘 (RTC) 模組 | `lib_deps = adafruit/RTClib` |

---

## 🔧 除錯技巧

### 常見問題排解

#### 問題 1：無法上傳程式 (連接埠錯誤)
**症狀**：顯示「找不到連接埠」或「序列埠錯誤」  
**解決方法**：
1. 確認 USB 線材具有資料傳輸功能（非純充電線）
2. 安裝對應的驅動程式 (CP2102 或 CH340)
3. 在上傳時按住 `BOOT` 鈕，直到開始上傳
4. 檢查裝置管理員中是否出現 COM 埠

#### 問題 2：WiFi 連線失敗
**症狀**：序列埠顯示「WiFi connection failed」  
**解決方法**：
1. 確認 SSID 與密碼正確（注意大小寫）
2. ESP32 僅支援 **2.4GHz** WiFi（不支援 5GHz）
3. 檢查路由器是否開啟 AP 隔離模式
4. 嘗試將手機熱點作為測試網路

#### 問題 3：感測器數值異常
**症狀**：讀取到 `-999` 或 `NaN`  
**解決方法**：
1. 檢查接線是否正確（特別是 VCC 與 GND）
2. 確認感測器工作電壓（3.3V 或 5V）
3. 在數據腳位與 VCC 之間加上 4.7kΩ 上拉電阻（DHT 系列）
4. 使用 `Serial.println()` 逐步檢查程式邏輯

#### 問題 4：程式編譯錯誤
**症狀**：出現 `'XXX' was not declared in this scope`  
**解決方法**：
1. 確認已安裝所需的函式庫
2. 檢查函式庫版本是否相容
3. 清除編譯快取後重新編譯
4. 查看函式庫範例程式碼的標頭檔引用

### 除錯工具
- **序列埠監控視窗**：即時查看 `Serial.print()` 輸出
- **WiFi 掃描工具**：使用 [scanWifi](scanWifi/) 確認訊號強度
- **MQTT 測試工具**：[MQTT Explorer](http://mqtt-explorer.com/) 監聽訊息
- **示波器/邏輯分析儀**：檢查訊號波形（進階）

---

## 🌐 參考資源

### 官方文件
- [ESP32 官方技術文件](https://docs.espressif.com/projects/esp-idf/)
- [Arduino ESP32 支援套件](https://github.com/espressif/arduino-esp32)
- [PlatformIO ESP32 平台](https://docs.platformio.org/en/latest/platforms/espressif32.html)

### 學習資源
- [RandomNerdTutorials - ESP32 教學](https://randomnerdtutorials.com/projects-esp32/)
- [ESP32.net 社群論壇](https://www.esp32.net/)
- [DroneBot Workshop - ESP32 影片教學](https://dronebotworkshop.com/)

### 工具與元件資料表
- [ESP32 腳位參考圖](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
- [MQTT Broker 測試伺服器](http://www.mqtt-dashboard.com/)
- [Fritzing 電路圖繪製工具](https://fritzing.org/)

---

## 📝 授權與貢獻

本倉庫內容僅供教學與學習使用。如有任何問題或建議，歡迎提出 Issue 或 Pull Request。

**持續更新中... 🚀**
