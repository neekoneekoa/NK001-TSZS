#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "buzzer.h"
#include "test.h"
#include "oled.h"

uint8_t test_LED(void){
    Led1_On();
    vTaskDelay(200);
    Led2_On();
    vTaskDelay(200);
    Led3_On();
    vTaskDelay(200);
    Led4_On();
    vTaskDelay(200);
    Led5_On();
    vTaskDelay(200);
    Led6_On();
    vTaskDelay(200);
    Led_Off();
    vTaskDelay(200);
    return 1;
}
//  1000-1800 测试蜂鸣器频率
uint8_t buzzer_test(void){
    // 对于9042一体无源蜂鸣器，先确保使用推荐的频率
    buzzer_on();    
    vTaskDelay(200);  // 1秒，比原来的300ms更长，更容易听到
    // 关闭蜂鸣器
    buzzer_off();
	vTaskDelay(500);  // 2秒的间隔，避免频率过高导致听不清
    return 1;
}

uint8_t oled_test(void){
    
    return 1;
}