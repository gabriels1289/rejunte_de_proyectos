#include "delay.h"

// SysTick Setup
void SysTick_Setup(void)
{
	RCC_ClocksTypeDef RCC_Clocks;

	/* SysTick end of count event each 1ms */ //1us
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000000);	//modificado 1000 000

//	SysTick->LOAD = 16000;								// 1ms: 168000 ticks @ 168MHz
//	SysTick->CTRL = 0x0007;								// Habilita Timer y Tick Interrupt
}

// SysTick Interrupt Handler (tick cada 1ms) 1us
void SysTick_Handler(void)
{
	SysTickCnt++;
	UdpCntTime++;
	//CntTimerImagen++;
}

void drvSysTickDelay(unsigned long tick)
{
	unsigned long systickcnt;

	systickcnt = SysTickCnt;
	while ((SysTickCnt - systickcnt) < tick);
}

void Delay_ms(unsigned long tick)
{
	unsigned long systickcnt;

	systickcnt = SysTickCnt;
	while ((SysTickCnt - systickcnt) < (tick * 1000));
}
