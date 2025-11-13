#include "pms5003st.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "oled.h"
#include <stdlib.h>

/**************************************************************************
函 数 名:GetWordFromBuff
功能描述:读取uint16_t。
输入参数:
@pbuff:数据缓存
@isBigEndian: 1:小端  0:大端
输出参数:
@pWord:读取的数据
返 回 值:None
其他说明:
**************************************************************************/
 int GetWordFromBuff(uint8_t *pbuff, uint16_t *pWord, uint8_t isBigEndian)
 {
	 if(isBigEndian){
		 *pWord   =pbuff[0] << 8;
		 *pWord  |=pbuff[1];
		 return 2;
	 }else{
		 *pWord   =pbuff[1] << 8;
		 *pWord  |=pbuff[0];
		 return 2;
	 }
 }

/**************************************************************************
函 数 名:PMS5003ST_TaskFunc
功能描述:传感器任务。
输入参数:
@pvParameters:系统传参
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
static void PMS5003ST_TaskFunc( void *pvParameters )
{
    uint8_t *recvData = NULL;
    uint32_t recvLen = 0;
    PMS5003ST_t pmsData;
    uint16_t sum = 0, temp = 0;
    uint32_t offset = 0, i;
    char str[20] = {0};

    while (1)
    {
        sum = 0;
        offset = 0;
       if( !USART2_GetData(&recvData, &recvLen, portMAX_DELAY) ){
           for( i = 0; i < recvLen - 2; i++ )
                sum += recvData[i];

           GetWordFromBuff(recvData + i, &temp, 1);
           if( (recvData[0] == 0x42) && (recvData[1] == 0x4d) && (sum == temp)){
                offset += 2;
                offset += GetWordFromBuff(recvData + offset, &pmsData.frameLen, 1);
                offset += GetWordFromBuff(recvData + offset, &pmsData.CF_pm1, 1);
                offset += GetWordFromBuff(recvData + offset, &pmsData.CF_pm25, 1);
                offset += GetWordFromBuff(recvData + offset, &pmsData.CF_pm10, 1);
                offset += GetWordFromBuff(recvData + offset, &pmsData.data_pm1, 1);
                offset += GetWordFromBuff(recvData + offset, &pmsData.data_pm25, 1);
                offset += GetWordFromBuff(recvData + offset, &pmsData.data_pm10, 1);
                offset += GetWordFromBuff(recvData + offset, &pmsData.data_0_3um, 1);
                offset += GetWordFromBuff(recvData + offset, &pmsData.data_0_5um, 1);
                offset += GetWordFromBuff(recvData + offset, &pmsData.data_1um, 1);
                offset += GetWordFromBuff(recvData + offset, &pmsData.data_2_5um, 1);
                offset += GetWordFromBuff(recvData + offset, &pmsData.data_5_0um, 1);
                offset += GetWordFromBuff(recvData + offset, &pmsData.data_10um, 1);
                offset += GetWordFromBuff(recvData + offset, &pmsData.data_hcho, 1);
                offset += GetWordFromBuff(recvData + offset, &pmsData.temperature, 1);
                offset += GetWordFromBuff(recvData + offset, &pmsData.humidty, 1);

                printf("pm2.5 = %dug/m3, HCHO = %fug/m3, temperature = %fC, humidty = %f%%\r\n",
                        pmsData.data_pm25, pmsData.data_hcho/1000.0f, pmsData.temperature/10.0f, pmsData.humidty/10.0f);
                sprintf(str,"pm2.5: %4d   (ug/m3)", pmsData.data_pm25);
                OLED_ShowString(1, 0, str, 12);
                sprintf(str,"HCHO : %2d.%d   (ug/m3)", pmsData.data_hcho/1000,  abs(pmsData.data_hcho%1000));
                OLED_ShowString(1, 16, str, 12);
                sprintf(str,"temp : %2d.%d   (C)",pmsData.temperature/10, abs(pmsData.temperature%10));
                OLED_ShowString(1, 32, str, 12);
                sprintf(str,"humi : %2d.%d   (%%)",pmsData.humidty/10,  abs(pmsData.humidty%10));
                OLED_ShowString(1, 48, str, 12);


           }
           
            vPortFree(recvData);
       }
    }
    
}

/**************************************************************************
函 数 名:PMS5003ST_Init
功能描述:传感器初始化。
输入参数:
@pvParameters:系统传参
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void PMS5003ST_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    Usart2_Init(baud);

	  /* 初始化IO口的时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;   //设置IO口推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;   //设置IO口速度
	GPIO_Init(GPIOA, &GPIO_InitStructure);              //初始化GPIO
	GPIO_SetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_4);                    //设置IO口输出高

    xTaskCreate( PMS5003ST_TaskFunc, "pms5003ST", 128, NULL, 3, NULL );
}




