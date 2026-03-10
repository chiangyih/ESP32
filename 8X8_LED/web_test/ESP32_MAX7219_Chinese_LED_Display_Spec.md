
# ESP32 + MAX7219 8×8 LED（4 模組）中文字顯示系統
## 開發技術規格書

版本：v1.1  
狀態：Draft（開發規劃用）  
輸入方式：Serial 文字輸入  
MCU：ESP32  
顯示硬體：MAX7219 8×8 LED Matrix ×4  
顯示解析度：32 × 8 pixels  

---

# 1. 專案目標

本專案使用 **ESP32** 搭配 **MAX7219 LED Matrix（4 模組）** 建立一套低解析度文字顯示系統。

系統透過 **Serial（UART）接收文字輸入**，並將文字內容轉換為點矩陣後顯示在 LED Matrix 上。

系統主要功能：

- 接收 Serial 文字輸入
- 顯示英文、數字與指定中文字
- 超出畫面寬度時進行跑馬燈顯示
- 接收新文字時即時更新顯示內容

---

# 2. 系統架構

## 2.1 系統方塊圖

PC / Terminal  
│  
│ Serial (USB UART)  
▼  
ESP32  
│  
│ SPI  
▼  
MAX7219 LED Matrix ×4

---

## 2.2 資料流程

1. PC 或終端機輸入文字
2. ESP32 Serial 接收文字
3. 文字解析
4. 字元轉換為點矩陣字模
5. 將點陣寫入顯示 buffer
6. MAX7219 顯示

---

# 3. 硬體規格

## 3.1 ESP32

需求：

- SPI 介面
- UART（Serial）
- 足夠 Flash 存放字模資料

建議開發板：

- ESP32 DevKit
- ESP32-WROOM

---

## 3.2 MAX7219 LED Matrix

模組數量：4  
單模組解析度：8×8  

總解析度：

32 × 8 pixels

通訊方式：SPI

---

## 3.3 GPIO 建議配置

| 功能 | MAX7219 | ESP32 GPIO |
|-----|---------|-----------|
| DIN | Data In | GPIO23 |
| CLK | Clock | GPIO18 |
| CS | Chip Select | GPIO5 |
| VCC | 電源 | 5V |
| GND | 地 | GND |

注意：ESP32 與 MAX7219 必須共地。

---

# 4. Serial 通訊規格

| 項目 | 規格 |
|-----|-----|
| Baud Rate | 115200 |
| Data Bits | 8 |
| Stop Bits | 1 |
| Parity | None |
| 編碼 | UTF-8 |

---

# 5. 文字輸入格式

系統接收 **一行文字** 作為顯示內容。

結束符號：

\n 或 \r\n

範例輸入：

HELLO  
中文測試  
HHVS IT  

---

# 6. 顯示模式

## 6.1 固定顯示

若文字寬度 ≤ 32 pixels  
直接顯示

---

## 6.2 跑馬燈顯示

若文字寬度 > 32 pixels  

採用 **右 → 左 捲動顯示**

---

## 6.3 新文字優先

收到新 Serial 文字時：

1. 停止舊顯示
2. 清除畫面
3. 顯示新文字

---

# 7. 字模規格

## 7.1 中文字模

採用 **8×8 點矩陣**

每個字使用：

8 bytes

---

## 7.2 字元寬度

| 字元 | 寬度 |
|----|----|
| 中文 | 8 pixels |
| 英文 | 5~8 pixels |
| 間距 | 1 pixel |

---

# 8. 顯示緩衝區

系統維護兩種 buffer

實體 buffer：32×8  
虛擬畫布：儲存完整字串點陣

---

# 9. 捲動控制

建議參數：

位移：1 pixel  
速度：80ms / step

---

# 10. 中文字支援範圍

第一版建議支援字：

中 文 資 訊 科 學 系 統 測 試 開 發 成 功

---

# 11. Serial 除錯輸出

範例：

[Serial] RX: 中文測試  
[Parser] UTF8 parse ok  
[Display] Mode: Scroll  

---

# 12. 系統模組

1. Serial 接收模組  
2. 字串解析模組  
3. 字模引擎  
4. 顯示 buffer 管理  
5. MAX7219 驅動  

---

# 13. 開發階段

Phase 1：MAX7219 驅動  
Phase 2：Serial 接收  
Phase 3：ASCII 顯示  
Phase 4：中文字顯示  
Phase 5：跑馬燈與最佳化

---

# 14. 限制

解析度僅 32×8  
中文字會簡化  
適合跑馬燈與短句顯示

