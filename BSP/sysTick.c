#include "sysTick.h"


/**************************************************************************
�� �� ��:SysTick_Init
��������:��ʼ��systick��
�������:None
�������:None
�� �� ֵ:None
����˵��:
**************************************************************************/
void SysTick_Init(void)
{
	uint32_t prioritygroup = 0x00U;
	
    SysTick_Config(SystemCoreClock/1000UL);    
    prioritygroup = NVIC_GetPriorityGrouping();
	
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(prioritygroup, 15, 0));	//��ʼ��sysTick�жϣ����ȼ�15
}

/**************************************************************************
�� �� ��:Clock_Init
��������:��ʼ��ʱ�ӡ�
�������:None
�������:None
�� �� ֵ:None
����˵��:
**************************************************************************/
void Clock_Init(void)
{
#if 1
	RCC_DeInit();													//������ RCC�Ĵ�������Ϊȱʡֵ
	RCC_HSICmd(ENABLE);												//ʹ��HSI
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);				//�ȴ�HSIʹ�ܳɹ�
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div2);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);			//���� PLL ʱ��Դ����Ƶϵ��
	RCC_PLLCmd(ENABLE);												//���PLL������ϵͳʱ��,��ô�����ܱ�ʧ��											
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);				//�ȴ�ָ���� RCC ��־λ���óɹ� �ȴ�PLL��ʼ���ɹ�
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);						//ѡ����Ҫ��ϵͳʱ��
	//�ȴ�PLL�ɹ�������ϵͳʱ�ӵ�ʱ��Դ
	// 0x00��HSI ��Ϊϵͳʱ��
	// 0x04��HSE��Ϊϵͳʱ��
	// 0x08��PLL��Ϊϵͳʱ��
	while(RCC_GetSYSCLKSource() != 0x08);							//���뱻ѡ���ϵͳʱ�Ӷ�Ӧ������RCC_SYSCLKSource_PLL
#else
	RCC_DeInit();    												//�ȸ�λRCC�Ĵ���
	RCC_HSEConfig( RCC_HSE_ON );    								//ʹ��HSE
    if ( SUCCESS == RCC_WaitForHSEStartUp() )
    {
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);		//ʹ��Ԥȡָ������FLASH�̼��еĺ���
        FLASH_SetLatency(FLASH_Latency_2);  						//����FLASH�ȴ����ڡ�  ��Ϊ��Ƶ��72M ���Եȴ��������ڡ�
        //�����������ߵı�Ƶ����
        RCC_HCLKConfig(RCC_SYSCLK_Div1);							//HCLK --> AHB ���Ϊ72M������ֻ��Ҫ1��Ƶ
		RCC_PCLK1Config(RCC_HCLK_Div2);								//PCLK1 --> APB1 ���Ϊ36M������Ҫ2��Ƶ
		RCC_PCLK2Config(RCC_HCLK_Div1);								//PCLK2 --> APB2 ���Ϊ72M������ֻ��Ҫ1��Ƶ
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);		//���������໷ PLLCLK = HSE * ��Ƶ����
        RCC_PLLCmd(ENABLE);        									//ʹ��PLL
        while ( RESET == RCC_GetFlagStatus(RCC_FLAG_PLLRDY) );  	//�ȴ�PLL�ȶ�
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);					//ѡ��ϵͳʱ�ӣ�ѡ�����໷�����
        while ( 0x08 != RCC_GetSYSCLKSource() );					//�ȴ�ѡ���ȶ�
    }
#endif
}


