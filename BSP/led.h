#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
/**************************************************************************
�� �� ��:Led_Init
��������:��ʼ��LEDʹ�õ�IO�ڡ�
�������:None
�������:None
�� �� ֵ:None��
����˵��:
        ʹ��LEDǰ��������øú�����ʼ����
**************************************************************************/
void Led_Init(void);

void Led1_On(void);
void Led2_On(void);
void Led3_On(void);
void Led4_On(void);
void Led5_On(void);
void Led6_On(void);
void Led_Off(void);

void Led_Mode_On(uint8_t brightness);
void Led_Mode_Off(void);
void Led_Mode_LongOff(uint32_t duration_ms);
void Led_Mode_Breath(uint8_t brightness, uint16_t breath_time_ms);
void Led_Mode_Blink(uint8_t brightness, uint16_t single_blink_time_ms, uint16_t blink_count, uint32_t sustain_blink_time_ms);
void Led_Update_10ms(void);
void Led_Update_1ms(void);
void Led_Select(uint8_t led_id);

#endif
