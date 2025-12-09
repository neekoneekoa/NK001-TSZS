#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

void pwm_init(void);
void pwm_set_duty(uint8_t duty);
uint8_t pwm_get_duty(void);

#endif /* __PWM_H */