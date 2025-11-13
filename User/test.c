#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "test.h"

uint8_t test_LED(){
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
