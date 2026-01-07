
#define bled 15   //藍LED接GPIO15
#define gled1 16   //綠LED1接GPIO16
#define gled2 17   //綠LED2接GPIO17

void task1(void *param)  //宣告任務1,綠led1間隔500ms閃爍
{
  pinMode(gled1,OUTPUT);
  while (1)     //綠led1間隔500ms閃爍
  {
      digitalWrite(gled1, !digitalRead(gled1));  
      vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
void task2(void *param)  //宣告任務2,綠led2間隔1s閃爍
{
  pinMode(gled2,OUTPUT);
  while (1)     //綠led2間隔1s閃爍
  {
      digitalWrite(gled2, !digitalRead(gled2));  
      vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup() 
{
        //xTaskCreat
        //(  //建立任務；xTaskCreat有下列6項參數(使用xTaskCreat建立任務都是運行在核心0)      
        //  task1,      //任務函式的名稱
        //  "task1",    //自訂的任務名稱
        //  1000,       //配置給任務的記憶大小,esp32大小單位為byte，通常設為1000byte
        //  NULL,       //傳給任務函式的參數值
        //  1,          //優先權，數字越小優先等級越，最低為0，最大為FreeRTOSConfig.h定義的configMAX_PRIORITIES的常數值-1
        //  NULL        //任務的參照，沒有的話設為NULL
        //);
   xTaskCreate( task1, "task1", 1000, NULL, 1, NULL);
   xTaskCreate( task2, "task2", 1000, NULL, 1, NULL);

     
   //上述建立的兩項任務，皆為持續閃，由核心0負責執行

   pinMode(bled,OUTPUT);
}

void loop()   // 由核心1負責執行
{
   digitalWrite(bled,0);
   delay(1000);
   digitalWrite(bled,1);
   delay(1000);
}
