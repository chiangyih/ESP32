#include <Arduino.h>
#include <TM1637Display.h>

// 定義 CLK 和 DIO 接腳 (請依實際接線修改)
#define CLK_PIN 22
#define DIO_PIN 23

// 初始化物件
TM1637Display display(CLK_PIN, DIO_PIN);

void setup() {
    // 設定亮度 (0x00 最暗 ~ 0x0f 最亮)
    display.setBrightness(0x0f); // 最亮, 開啟顯示器，調暗請將 0x0f 改為較小的值
    
    // 初始化 Serial (選用，方便除錯)
    Serial.begin(115200);
}

void loop() {
    // 題目要求：從 0000 遞增到 1000
    for (int i = 0; i <= 1000; i++) {
        
        // showNumberDec 參數說明:
        // 1. 數值
        // 2. 是否補前導零 (true = 顯示 0001, false = 顯示 1)
        // 3. 顯示位數 (4位)
        // 4. 起始位置 (從第0位開始)
        display.showNumberDec(i, true, 4, 0);
        
        // 頻率 1000Hz = 週期 1ms
        delay(1); 
    }

    // 跑完一輪後的緩衝 (可自行決定是否需要)
    delay(1000);
}