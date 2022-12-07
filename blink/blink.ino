
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  #include <stdio.h>
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  #include "driver/gpio.h"
  #include "sdkconfig.h"
  pinMode(2, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  vTaskDelay(500);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  vTaskDelay(500);                       // wait for a second
}
