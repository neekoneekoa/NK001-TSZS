#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define LED_ON    ENABLE
#define LED_OFF   DISABLE

#define LED0(X)   do{if(X == LED_ON) GPIO_ResetBits(GPIOC, GPIO_Pin_13); else GPIO_SetBits(GPIOC, GPIO_Pin_13);}while(0)
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

/**************************************************************************
�� �� ��:GetPB13State
��������:��ȡPB13Ӳ���״̬
�������:None
�������:None
�� �� ֵ:uint8_t - ��PB13�Ĵ��ƽ״̬
����˵��:
        ��1��ʾ��ƽ��0��ʾ��ƽ
**************************************************************************/
uint8_t GetPB13State(void);

#endif
