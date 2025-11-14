#ifndef __TEST_H
#define __TEST_H

#include "stm32f10x.h"

uint8_t test_LED(void);
uint8_t buzzer_test(void);
uint8_t oled_test(void);
void OLED_ShowNum(uint8_t x, uint8_t y, int32_t num, uint8_t len, uint8_t size);
#endif
