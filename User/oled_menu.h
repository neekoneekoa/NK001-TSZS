#ifndef __OLED_MENU_H
#define __OLED_MENU_H

#include "FreeRTOS.h"
#include "task.h"
#include "oled.h"
#include "key.h"

// 定义界面类型
typedef enum {
    MENU_MAIN,
    MENU_PWM,
    MENU_ADC,
    MENU_GPIO_INPUT,
    MENU_GPIO_OUTPUT,
    MENU_UART_RECEIVE,
    MENU_FREE_DEBUG,
    MENU_COUNT  // 界面总数
} MENU_TYPE;

// 函数声明
void menu_init(void);                  // 菜单系统初始化
void menu_switch(MENU_TYPE menu);      // 切换到指定界面
void menu_update(void);                // 更新当前界面显示
void menu_handle_key(KEY_ID key);      // 处理按键输入

// 各界面的回调函数类型
typedef void (*MenuInitFunc)(void);    // 进入界面的初始化函数
typedef void (*MenuLoopFunc)(void);    // 运行界面的循环函数
typedef void (*MenuExitFunc)(void);    // 退出界面的函数

#endif /* __OLED_MENU_H */