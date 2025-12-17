#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

void pwm_init(void);
// channel: 1=PA11, 2=PA8
void pwm_set_duty(uint8_t channel, uint8_t duty);
uint8_t pwm_get_duty(uint8_t channel);

#endif /* __PWM_H */
