#ifndef __USART1_H
#define __USART1_H

#include <stdio.h>
#include "stm32f10x.h"

/**************************************************************************
函 数 名:Usart1_Init
功能描述:串口1初始化。
输入参数:
    bound:波特率
输出参数:None
返 回 值:None。
其他说明:
        使用串口1前，必须调用该函数初始化。
**************************************************************************/
void Usart1_Init(const uint32_t bound);

/**************************************************************************
函 数 名:Usart1_SendData
功能描述:串口1发送数据。
输入参数:
    pData:发送的数据缓存
    dataLen:发送的数据长度
输出参数:None
返 回 值:None
其他说明:
        使用串口1前，必须调用该函数初始化。
**************************************************************************/
void Usart1_SendData(const uint8_t *pData, const uint16_t dataLen);

/**
name:USART1_Read
function:串口1读数据
input:None
output:
@rData:读取的数据指针,实际地址空间在该函数内部申请,所以在外部使用后必须要释放
@rDataLen:读取的数据长度指针
@timeout:等待超时时间
return:错误
other:None
*/
int USART1_Read(uint8_t **rData, uint32_t *rDataLen, uint32_t timeout);



#endif
