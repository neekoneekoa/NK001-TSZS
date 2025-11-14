#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "buzzer.h"
#include "test.h"
#include "oled.h"
#include "key.h"
#include "queue.h"
static uint8_t key1;
extern QueueHandle_t xKeyQueue;   /* 告诉编译器实体在别处 */
void test_init(void){
    
}
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
    return 1;
}

uint8_t oled_test(void){
    OLED_ShowNum(0, 16, 0, 1, 16);
    return 1;
}

uint8_t test_KEY(void){
        uint16_t pb0_mv, pb1_mv;
        // 按键按下处理
        if (xQueueReceive(xKeyQueue, &key1, 0) == pdTRUE)
        {
            OLED_ShowNum(0, 48, key1, 1, 16);
            // 按键按下时蜂鸣器响一下
            buzzer_on();
            vTaskDelay(100 / portTICK_PERIOD_MS);
            buzzer_off();
        }
            // switch(key_current)
            // {
            //     case KEY_K1:
            //         OLED_ShowNum(0, 48, 1, 6, 16);
            //         break;
            //     case KEY_K2:
            //         OLED_ShowNum(0, 48, 2, 6, 16);   
            //         break;
            //     case KEY_K3:
            //         OLED_ShowNum(0, 48, 3, 6, 16);
            //         break;
            //     case KEY_K4:
            //         OLED_ShowNum(0, 48, 4, 6, 16);
            //         break;
            //     case KEY_K5:
            //         OLED_ShowNum(0, 48, 5, 6, 16);
            //         break;
            //     case KEY_K6:
            //         OLED_ShowNum(0, 48, 6, 6, 16);
            //         break;
            //     default:
            //         break;
            // }
        key_get_adc_mv(&pb0_mv, &pb1_mv);
        // 在不同位置显示PB0和PB1的ADC值，避免重叠
        OLED_ShowString(0, 32, "PB0:", 16);
        OLED_ShowNum(32, 32, pb0_mv, 4, 16);
        OLED_ShowString(0, 48, "PB1:", 16);
        OLED_ShowNum(32, 48, pb1_mv, 4, 16);
        return 1;
}