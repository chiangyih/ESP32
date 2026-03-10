/**
 * ESP32 + MAX7219 8x8 LED 矩陣顯示控制
 *
 * 功能摘要：
 *   - 透過 Serial 接收文字，顯示於 4 個串接的 8x8 LED 模組
 *   - 支援固定顯示（文字短）與捲動顯示（文字長）兩種模式
 *   - 支援上下/左右翻轉、模組鏈順序反轉
 *   - 方向鍵控制捲動速度與方向，Enter 送出文字指令
 *
 * 硬體接線（FC-16 模組，SPI 預設腳位）：
 *   CS   → GPIO 5
 *   MOSI → GPIO 23 (VSPI MOSI)
 *   CLK  → GPIO 18 (VSPI CLK)
 */
#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <MD_MAX72xx.h>
#include <vector>

namespace {

// ── 硬體設定 ──────────────────────────────────────────────────────────────────
constexpr MD_MAX72XX::moduleType_t kHardwareType   = MD_MAX72XX::FC16_HW; // 模組型號
constexpr uint8_t  kNumModules        = 4;               // 串接的模組數量
constexpr uint8_t  kDisplayWidth      = kNumModules * 8; // 總顯示寬度（像素欄數）
constexpr uint8_t  kPinCs             = 5;               // SPI CS 腳位
constexpr uint32_t kBaudRate          = 115200;          // Serial 鮑率
constexpr uint8_t  kGlyphBufferSize   = 8;               // 單字元字型緩衝區大小
constexpr char     kWifiSsid[]        = "hhvs-iot";      // Wi-Fi SSID
constexpr uint8_t  kWifiXorKey        = 0x5A;            // Wi-Fi 密碼解密金鑰
constexpr uint8_t  kWifiPasswordEnc[] = {18, 50, 44, 41, 26, 111, 110, 19, 53, 46}; // XOR 加密後密碼
constexpr uint16_t kWifiConnectTimeoutSeconds = 20;      // 開機首次連線逾時秒數
constexpr uint32_t kWifiReconnectIntervalMs   = 10000;   // 掉線後重試間隔
constexpr char     kMqttHost[]        = "mqb.hhvs.tn.edu.tw"; // MQTT broker 位址
constexpr uint16_t kMqttPort          = 1883;                 // MQTT broker port
constexpr char     kMqttTopic[]       = "/hhvs/ie/8x8led";   // 訂閱的 MQTT topic
constexpr uint32_t kMqttReconnectIntervalMs = 5000;           // MQTT 掉線重試間隔

// ── 捲動速度設定（單位：毫秒/步）────────────────────────────────────────────
constexpr uint16_t kDefaultScrollStep = 80;  // 預設捲動間隔
constexpr uint16_t kMinScrollStep     = 20;  // 最快捲動間隔
constexpr uint16_t kMaxScrollStep     = 400; // 最慢捲動間隔
constexpr uint16_t kScrollAdjustDelta = 10;  // 每次加減速的調整量
constexpr size_t   kMaxInputLength    = 128; // Serial 輸入緩衝區最大長度（防止記憶體耗盡）

// ── 列舉型別 ─────────────────────────────────────────────────────────────────
enum class DisplayMode    : uint8_t { Auto, Fixed, Scroll };   // 顯示模式：自動/固定/捲動
enum class ScrollDirection : int8_t { Left = 1, Right = -1 }; // 捲動方向：向左或向右
enum class EscapeState    : uint8_t { None, Esc, Bracket };   // ANSI 跳脫序列解析狀態

// ── 全域物件與狀態變數 ─────────────────────────────────────────────────────────
MD_MAX72XX           gMatrix(kHardwareType, kPinCs, kNumModules); // LED 矩陣驅動物件
std::vector<uint8_t> gVirtualColumns; // 虛擬畫布：儲存整段文字所有欄的像素資料
String               gInputLine;      // Serial 輸入緩衝區（尚未送出的一行）
String               gCurrentText        = "";                            // 目前顯示文字（於 setup 中按連線結果設置）
DisplayMode          gDisplayMode        = DisplayMode::Auto;            // 目前顯示模式
ScrollDirection      gScrollDirection    = ScrollDirection::Left;        // 目前捲動方向
EscapeState          gEscapeState        = EscapeState::None;            // ANSI 跳脫解析狀態
bool                 gReverseModuleChain = false; // 是否反轉模組鏈順序（調整實體接線方向）
bool                 gFlipRows           = true;  // 是否上下翻轉每欄像素
bool                 gFlipCols           = false; // 是否左右翻轉整個畫面
bool                 gIsScrolling        = false; // 目前是否處於捲動模式
int                  gOffset             = 0;     // 捲動偏移量（虛擬畫布的起始欄索引）
unsigned long        gLastScrollMs       = 0;     // 上次捲動的時間戳（millis）
uint16_t             gScrollStepMs       = kDefaultScrollStep; // 目前捲動間隔
unsigned long        gLastWifiRetryMs    = 0;                  // 上次 Wi-Fi 重連檢查時間
unsigned long        gLastMqttRetryMs    = 0;                  // 上次 MQTT 重連檢查時間
String               gWifiPassword;                            // 執行時解密後密碼
WiFiClient           gNetClient;                               // TCP client
PubSubClient         gMqttClient(gNetClient);                  // MQTT client
bool                 gMqttSubscribed      = false;                           // 是否已訂閱 topic

void adjustScrollSpeed(bool faster);

void printInputPrompt() {
  Serial.print(F("請輸入文字: "));
  Serial.print(gInputLine);
}

void startWifiConnection() {
  WiFi.begin(kWifiSsid, gWifiPassword.c_str());
}

// 將加密後密碼解密為可供 WiFi.begin 使用的字串
String decodeWifiPassword() {
  String plain;
  plain.reserve(sizeof(kWifiPasswordEnc));
  for (size_t i = 0; i < sizeof(kWifiPasswordEnc); ++i) {
    plain += static_cast<char>(kWifiPasswordEnc[i] ^ kWifiXorKey);
  }
  return plain;
}

// 啟動 Wi-Fi 並在指定秒數內等待連線完成
bool connectWifiBlocking(uint16_t timeoutSeconds) {
  WiFi.mode(WIFI_STA);
  startWifiConnection();

  Serial.print(F("[WiFi] 連線中"));
  uint16_t elapsedSeconds = 0;
  while (WiFi.status() != WL_CONNECTED && elapsedSeconds < timeoutSeconds) {
    delay(1000);
    ++elapsedSeconds;
    Serial.print('.');
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print(F("[WiFi] 已連線，IP: "));
    Serial.println(WiFi.localIP());
    return true;
  }

  Serial.println(F("[WiFi] 連線逾時，稍後將自動重試"));
  return false;
}

// 定期檢查 Wi-Fi 狀態；若掉線則重試
void maintainWifiConnection() {
  if (WiFi.status() == WL_CONNECTED) return;
  if (millis() - gLastWifiRetryMs < kWifiReconnectIntervalMs) return;

  gLastWifiRetryMs = millis();
  Serial.println(F("[WiFi] 偵測到未連線，重新嘗試連線..."));
  WiFi.disconnect();
  startWifiConnection();
}

// 嘗試建立 MQTT 連線（需先有 Wi-Fi）
void maintainMqttConnection() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  if (gMqttClient.connected()) {
    // 已連線，確保已訂閱 topic
    if (!gMqttSubscribed) {
      gMqttClient.subscribe(kMqttTopic);
      gMqttSubscribed = true;
      Serial.print(F("[MQTT] 已訂閱 topic: "));
      Serial.println(kMqttTopic);
    }
    return;
  }
  
  if (millis() - gLastMqttRetryMs < kMqttReconnectIntervalMs) return;

  gLastMqttRetryMs = millis();
  String clientId = String("esp32-led-") + String(static_cast<uint32_t>(ESP.getEfuseMac() & 0xFFFFFFFF), HEX);
  Serial.print(F("[MQTT] 嘗試連線 broker: "));
  Serial.print(kMqttHost);
  Serial.print(':');
  Serial.println(kMqttPort);

  if (gMqttClient.connect(clientId.c_str())) {
    Serial.println(F("[MQTT] 連線成功"));
    gMqttSubscribed = false;  // 重置訂閱狀態，下次迴圈會訂閱
    return;
  }

  Serial.print(F("[MQTT] 連線失敗，rc="));
  Serial.println(gMqttClient.state());
}

// 將 8 位元位元組的位元順序完全反轉（用於上下翻轉每欄像素）
// 實作方式：先交換高低各 4 位元，再交換各 2 位元，最後交換各 1 位元
uint8_t reverseBits(uint8_t v) {
  v = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4);
  v = ((v & 0xCC) >> 2) | ((v & 0x33) << 2);
  v = ((v & 0xAA) >> 1) | ((v & 0x55) << 1);
  return v;
}

// 將虛擬畫布的 x 欄索引映射到實際 LED 的欄位址
// 依序套用左右翻轉（gFlipCols）與模組鏈反轉（gReverseModuleChain）兩種補正
uint8_t mapColumn(uint8_t x) {
  uint8_t col = gFlipCols ? static_cast<uint8_t>(kDisplayWidth - 1 - x) : x; // 左右翻轉補正
  if (!gReverseModuleChain) return col;
  // 模組鏈反轉：將模組編號由末端往前重新排列，欄內偏移不變
  return static_cast<uint8_t>((kNumModules - 1 - col / 8) * 8 + col % 8);
}

// 將虛擬畫布從 offset 起算的 kDisplayWidth 欄資料寫入實體 LED
// 超出虛擬畫布範圍的欄填 0（熄滅）；寫入前關閉自動更新以避免閃爍
void drawViewport(int offset) {
  gMatrix.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF); // 暫停自動刷新
  for (uint8_t x = 0; x < kDisplayWidth; ++x) {
    int src = offset + x;
    uint8_t col = (src >= 0 && src < static_cast<int>(gVirtualColumns.size())) ? gVirtualColumns[src] : 0;
    gMatrix.setColumn(mapColumn(x), gFlipRows ? reverseBits(col) : col); // 套用上下翻轉後寫入
  }
  // control(UPDATE, ON) 內部已觸發一次 flush，無需再呼叫 update()
  gMatrix.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

// 將字串轉換為虛擬畫布（gVirtualColumns）
// 使用 MD_MAX72xx 內建 ASCII 字型取得每個字元的欄像素資料
// 字元間插入 1 欄空白（像素值 0）作為間距；非可印字元以 '?' 替代
void buildVirtualText(const String& text) {
  gVirtualColumns.clear();
  uint8_t glyph[kGlyphBufferSize];
  for (size_t i = 0; i < text.length(); ++i) {
    uint8_t ch = static_cast<uint8_t>(text[i]);
    if (ch < 32 || ch > 126) ch = '?'; // 過濾不可印字元
    uint8_t w = gMatrix.getChar(ch, sizeof(glyph), glyph); // 取得字元字型寬度與資料
    for (uint8_t k = 0; k < w; ++k) gVirtualColumns.push_back(glyph[k]);
    if (i + 1 < text.length()) gVirtualColumns.push_back(0); // 字元間距（1 欄空白）
  }
  if (gVirtualColumns.empty()) gVirtualColumns.push_back(0); // 防止空字串導致畫布為空
}

// 設定並立即顯示文字，同時決定顯示模式（固定或捲動）
// Auto 模式下：文字寬度 <= 畫面寬度時固定顯示，否則捲動顯示
// 捲動模式的起始偏移為 -kDisplayWidth（從右側外緣開始進入）
void setDisplayText(const String& text) {
  gCurrentText = text;
  buildVirtualText(text); // 重建虛擬畫布
  bool fixed = (gDisplayMode == DisplayMode::Fixed) ||
               (gDisplayMode == DisplayMode::Auto && gVirtualColumns.size() <= kDisplayWidth);
  gIsScrolling  = !fixed;
  gOffset       = gIsScrolling ? -static_cast<int>(kDisplayWidth) : 0; // 捲動從右側進入
  gLastScrollMs = millis();
  drawViewport(gOffset);
  Serial.println(gIsScrolling ? F("[顯示] 模式：捲動") : F("[顯示] 模式：固定"));
}

// 解析並執行 Serial 輸入的指令或文字
// 支援指令：fixed / scroll / auto / fliprow on|off / flipcol on|off / reverse on|off
// 若不符合任何指令，則直接將輸入的文字顯示在 LED 上
void handleCommand(String cmd) {
  cmd.trim();
  if (cmd.isEmpty()) return;

  auto handleMode = [&](const char* token, DisplayMode mode, const __FlashStringHelper* msg) {
    if (!cmd.equalsIgnoreCase(token)) return false;
    gDisplayMode = mode;
    Serial.println(msg);
    setDisplayText(gCurrentText);
    return true;
  };

  auto handleToggle = [&](const char* onToken,
                          const char* offToken,
                          bool& flag,
                          const __FlashStringHelper* onMsg,
                          const __FlashStringHelper* offMsg) {
    if (cmd.equalsIgnoreCase(onToken)) {
      flag = true;
      Serial.println(onMsg);
      setDisplayText(gCurrentText);
      return true;
    }
    if (cmd.equalsIgnoreCase(offToken)) {
      flag = false;
      Serial.println(offMsg);
      setDisplayText(gCurrentText);
      return true;
    }
    return false;
  };

  // 顯示模式切換
  if (handleMode("fixed", DisplayMode::Fixed, F("[輸入] 已切換為固定顯示"))) return;
  if (handleMode("scroll", DisplayMode::Scroll, F("[輸入] 已切換為捲動顯示"))) return;
  if (handleMode("auto", DisplayMode::Auto, F("[輸入] 已切換為自動模式"))) return;

  // 上下翻轉控制
  if (handleToggle("fliprow on", "fliprow off", gFlipRows,
                   F("[輸入] 上下翻轉：開啟"), F("[輸入] 上下翻轉：關閉"))) return;

  // 左右翻轉控制
  if (handleToggle("flipcol on", "flipcol off", gFlipCols,
                   F("[輸入] 左右翻轉：開啟"), F("[輸入] 左右翻轉：關閉"))) return;

  // 模組鏈順序反轉控制（調整實體接線與顯示方向不一致的情況）
  if (handleToggle("reverse on", "reverse off", gReverseModuleChain,
                   F("[輸入] 模組鏈反轉：開啟"), F("[輸入] 模組鏈反轉：關閉"))) return;

  // 方向與速度控制（供 MQTT/文字指令使用）
  if (cmd.equalsIgnoreCase("left")) {
    gScrollDirection = ScrollDirection::Left;
    Serial.println(F("[輸入] 已設定為往左捲動"));
    return;
  }
  if (cmd.equalsIgnoreCase("right")) {
    gScrollDirection = ScrollDirection::Right;
    Serial.println(F("[輸入] 已設定為往右捲動"));
    return;
  }
  if (cmd.equalsIgnoreCase("faster")) {
    adjustScrollSpeed(true);
    return;
  }
  if (cmd.equalsIgnoreCase("slower")) {
    adjustScrollSpeed(false);
    return;
  }

  // 非指令：直接顯示輸入的文字
  setDisplayText(cmd);
}

// 調整捲動速度：faster=true 為加速（減少間隔），faster=false 為減速（增加間隔）
// 速度有上下限保護，不會超出 kMinScrollStep / kMaxScrollStep 範圍
void adjustScrollSpeed(bool faster) {
  if (faster) {
    gScrollStepMs = static_cast<uint16_t>(
      (gScrollStepMs > kMinScrollStep + kScrollAdjustDelta)
      ? gScrollStepMs - kScrollAdjustDelta : kMinScrollStep);
    Serial.print(F("[輸入] 捲動加速，間隔(ms)："));
  } else {
    gScrollStepMs = static_cast<uint16_t>(
      (gScrollStepMs + kScrollAdjustDelta < kMaxScrollStep)
      ? gScrollStepMs + kScrollAdjustDelta : kMaxScrollStep);
    Serial.print(F("[輸入] 捲動減速，間隔(ms)："));
  }
  Serial.println(gScrollStepMs);
}

// 解析 ANSI 跳脫序列（終端機方向鍵）
// 序列格式：ESC(0x1B) → '[' → 方向字元（A/B/C/D）
//   ↑(A) 加速、↓(B) 減速、→(C) 往右捲動、←(D) 往左捲動
// 回傳 true 表示已消耗此字元（不需再做其他處理）
bool handleEscapeSequence(char ch) {
  if (gEscapeState == EscapeState::Esc) {
    // 收到 ESC 後，等待 '[' 進入 Bracket 狀態，否則重置
    gEscapeState = (ch == '[') ? EscapeState::Bracket : EscapeState::None;
    return true;
  }
  if (gEscapeState == EscapeState::Bracket) {
    // 收到方向字元，執行對應動作
    if      (ch == 'A') adjustScrollSpeed(true);   // ↑ 加速
    else if (ch == 'B') adjustScrollSpeed(false);  // ↓ 減速
    else if (ch == 'C') { gScrollDirection = ScrollDirection::Right; Serial.println(F("[輸入] 已設定為往右捲動")); } // →
    else if (ch == 'D') { gScrollDirection = ScrollDirection::Left;  Serial.println(F("[輸入] 已設定為往左捲動")); } // ←
    gEscapeState = EscapeState::None;
    printInputPrompt();  // 重新顯示提示符與已輸入內容
    return true;
  }
  if (static_cast<uint8_t>(ch) == 0x1B) { gEscapeState = EscapeState::Esc; return true; } // 收到 ESC，進入等待狀態
  return false;
}

// 輪詢 Serial 並處理每個收到的字元
// 處理邏輯優先順序：ANSI 跳脫序列 > CR（忽略）> LF（送出）> Backspace > 一般文字
void pollSerial() {
  while (Serial.available() > 0) {
    char ch = static_cast<char>(Serial.read());
    if (handleEscapeSequence(ch)) continue;  // 跳脫序列（方向鍵）
    if (ch == '\r') continue;                // 忽略 CR（Windows 換行的前半）
    if (ch == '\n') {                        // LF：送出目前輸入行
      Serial.println();
      handleCommand(gInputLine);
      gInputLine = "";
      printInputPrompt();
      continue;
    }
    if (ch == '\b' || static_cast<uint8_t>(ch) == 127) { // Backspace（\b 或 DEL）
      if (gInputLine.length() > 0) { gInputLine.remove(gInputLine.length() - 1); Serial.print(F("\b \b")); }
      continue;
    }
    // 一般可印字元：加入輸入緩衝區並回顯（超過上限時忽略，防止記憶體耗盡）
    if (gInputLine.length() < kMaxInputLength) {
      gInputLine += ch;
      Serial.print(ch);
    }
  }
}

// 依照捲動間隔推進捲動偏移，並重繪畫面（在 loop() 中持續呼叫）
// 偏移超過虛擬畫布末端或前端時循環重置，形成無縫循環捲動
void updateScroll() {
  if (!gIsScrolling || millis() - gLastScrollMs < gScrollStepMs) return;
  gLastScrollMs = millis();
  gOffset += static_cast<int>(gScrollDirection); // 依方向推進一步
  // 超出範圍時循環：向左捲動超過末端 → 從右側重新進入，反之亦然
  // 使用 >= 避免 offset == size 時出現一格全黑畫面
  if      (gOffset >= static_cast<int>(gVirtualColumns.size())) gOffset = -static_cast<int>(kDisplayWidth);
  else if (gOffset < -static_cast<int>(kDisplayWidth))          gOffset =  static_cast<int>(gVirtualColumns.size()) - 1;
  drawViewport(gOffset);
}

// MQTT 消息回調函式：處理接收到的 topic 消息
void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  String msg;
  msg.reserve(length);
  for (unsigned int i = 0; i < length; ++i) {
    msg += static_cast<char>(payload[i]);
  }
  
  Serial.print(F("[MQTT] 收到消息 [topic: "));
  Serial.print(topic);
  Serial.print(F("] "));
  Serial.println(msg);

  // 空字串代表清除顯示；其餘先當作指令解析，非指令才當作一般文字顯示
  if (msg.isEmpty()) {
    setDisplayText("");
    return;
  }
  handleCommand(msg);
}

void setupMqttCallback() {
  // 在 namespace 內部進行 callback 設定，避免名稱解析問題
  gMqttClient.setCallback(onMqttMessage);
}

}  // namespace

void setup() {
  Serial.begin(kBaudRate);
  delay(200);

  gWifiPassword = decodeWifiPassword();
  gMqttClient.setServer(kMqttHost, kMqttPort);

  bool wifiConnected = connectWifiBlocking(kWifiConnectTimeoutSeconds);

  gMatrix.begin();
  gMatrix.control(MD_MAX72XX::INTENSITY, 4);

  gVirtualColumns.assign(kDisplayWidth, 0xFF);
  drawViewport(0);
  delay(200);
  gVirtualColumns.assign(kDisplayWidth, 0x00);
  drawViewport(0);

  // 根據 Wi-Fi 連線結果設置初始顯示
  if (wifiConnected) {
    gCurrentText = String("wifi connect success  ") + WiFi.localIP().toString() + "  waiting for input:";
  } else {
    gCurrentText = "waiting for wifi connection...";
  }

  maintainMqttConnection();
setupMqttCallback();  // 在 namespace 內設定 callback
  
  Serial.println(F("[系統] LED 顯示系統已啟動"));
  Serial.println(F("[輸入] 輸入文字後按 Enter，會顯示在 LED 上"));
  Serial.println(F("[輸入] 指令：fixed 固定顯示，scroll 捲動顯示，auto 自動判斷"));
  Serial.println(F("[輸入] 方向：fliprow on/off、flipcol on/off、reverse on/off"));
  Serial.println(F("[輸入] 方向鍵：↑加速、↓減速、→往右捲動、←往左捲動"));
  Serial.print(F("[輸入] 目前捲動間隔(ms)：")); Serial.println(gScrollStepMs);
  Serial.print(F("[MQTT] 訂閱 topic: ")); Serial.println(kMqttTopic);
  printInputPrompt();
  setDisplayText(gCurrentText);
}

void loop() {
  maintainWifiConnection();
  maintainMqttConnection();
  gMqttClient.loop();
  pollSerial();
  updateScroll();
}
