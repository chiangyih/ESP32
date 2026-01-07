#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <MD_Parola.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW // Example hardware type
#define MAX_DEVICES 1                    // Number of connected devices
#define DATA_PIN   23
#define CLK_PIN    18
#define CS_PIN     5

// 使用硬體 SPI 初始設定 (如果要使用軟體 SPI，可改建構子參數順序)
MD_Parola display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

const char message[] = "Hello, World! "; // 訊息，尾端加空白讓捲動較順
const uint16_t SCROLL_SPEED = 60; // 捲動速度 (越大越慢)
const uint16_t SCROLL_PAUSE = 0;  // 停頓時間 (ms)

void setup() {
  display.begin();
  display.setIntensity(1); // Set brightness level (0 is min, 15 is max)
  display.displayClear(); // Clear display

  // 設定顯示為向左捲動
  // 參數: (文字, 對齊, 速度, 停頓, 進入特效, 離開特效)
  display.displayText(message, PA_LEFT, SCROLL_SPEED, SCROLL_PAUSE, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
}

void loop() {
  // 驅動動畫；回傳 true 表示該訊息已完成顯示
  if (display.displayAnimate()) {
    // 當動畫完成時，重新設定顯示文字讓它持續循環
    display.displayReset();
  }
}
