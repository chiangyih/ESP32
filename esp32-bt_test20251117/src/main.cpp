#include <Arduino.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;        // ESP32 經典藍牙序列埠物件，負責 SPP 資料收發
const char* pin_code = "1234";   // 預設藍牙配對碼（可依需求變更）
String btBuffer;                 // 暫存藍牙字串，直到收到換行符為止

void setup() {
  Serial.begin(9600);            // 啟動 USB 序列埠，便於在監控視窗查看除錯訊息
  SerialBT.begin(9600);          // 預設波特率，實際藍牙傳輸速率由 SPP 連線決定
  delay(1000);                   // 稍作等待，讓藍牙堆疊完成初始化
  
  Serial.println("\n=== ESP32 經典藍牙應用啟動 ===");
  
  // 初始化藍牙並設定顯示名稱；若 begin 回傳 false 代表驅動啟動失敗
  if (!SerialBT.begin("ESP32_BT")) {
    Serial.println("藍牙初始化失敗");
  } else {
    Serial.println("經典藍牙已就緒");
    Serial.println("裝置名稱: ESP32_BT");
    Serial.println("配對碼: 1234");
  }
  
  Serial.println("=================================\n");
}

void flushBtBuffer() {
  btBuffer.trim();                // 去除前後空白，避免純換行或空白被視為有效指令
  if (btBuffer.length() == 0) {   // 過濾掉空字串，以免印出多餘訊息
    btBuffer = "";
    return;
  }

  Serial.print("收到藍牙字串: "); // 將完整字串顯示在序列監視器上
  Serial.println(btBuffer);
  SerialBT.print("respond: ");    // 原樣回傳給藍牙端，方便確認互通性
  SerialBT.println(btBuffer);
  btBuffer = "";                  // 清空緩衝區以便接收下一筆資料
}

void loop() {
  // 逐字讀取藍牙接收緩衝區；一次讀完所有累積字元避免延遲
  while (SerialBT.available()) {
    char data = SerialBT.read();  // 從 SPP 取出單一字元

    if (data == '\r' || data == '\n') {  // 偵測 CR/LF，視為一筆完整指令結束
      flushBtBuffer();
    } else {
      btBuffer += data;                      // 尚未結束則持續把字元加到緩衝區
    }
  }
  
  // // 若需要由 USB 序列端發指令給藍牙，可取消註解以下程式碼
  // if (Serial.available()) {
  //   char data = Serial.read();
  //   SerialBT.write(data);         // 將鍵盤輸入轉送到藍牙設備
  // }
  
  delay(10);                      // 短暫延遲，避免 loop 過於頻繁占用核心
}