#ifndef __PMS5003ST_H__
#define __PMS5003ST_H__

#include "usart2.h"

#pragma pack(1)
typedef struct {
    uint8_t start1;             //起始位0x42
    uint8_t start2;             //起始位0x4d
    uint16_t frameLen;          //帧长度
    uint16_t CF_pm1;            //pm1.0标准颗粒物(ug/m3)
    uint16_t CF_pm25;           //pm2.5标准颗粒物(ug/m3)
    uint16_t CF_pm10;           //pm10标准颗粒物(ug/m3)
    uint16_t data_pm1;          //pm1.0大气环境(ug/m3)
    uint16_t data_pm25;         //pm2.5大气环境(ug/m3)
    uint16_t data_pm10;         //pm10大气环境(ug/m3)
    uint16_t data_0_3um;        //在0.1L空气中直径0.3um以上的颗粒物数量
    uint16_t data_0_5um;        //在0.1L空气中直径0.5um以上的颗粒物数量
    uint16_t data_1um;          //在0.1L空气中直径1um以上的颗粒物数量
    uint16_t data_2_5um;        //在0.1L空气中直径2.5um以上的颗粒物数量
    uint16_t data_5_0um;        //在0.1L空气中直径5.0um以上的颗粒物数量
    uint16_t data_10um;         //在0.1L空气中直径10um以上的颗粒物数量
    uint16_t data_hcho;         //甲醛浓度 真实值=本数值/1000 mg/m3
    uint16_t temperature;       //温度  真实值=本数值/10 C
    uint16_t humidty;           //湿度  真实值=本数值/10 %
    uint16_t RSV;               //保留
    uint8_t version;            //版本号
    uint8_t err;                //错误代码
    uint16_t check;             //校验码
}PMS5003ST_t;
#pragma pack()

/**************************************************************************
函 数 名:PMS5003ST_Init
功能描述:传感器初始化。
输入参数:
@pvParameters:系统传参
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void PMS5003ST_Init(uint32_t baud);




#endif



