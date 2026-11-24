/* Define to prevent recursive inclusion -------------------------------------*/
//#pragma once
#ifndef __DELAY_H
#define __DELAY_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"
#include "misc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_syscfg.h"

 volatile unsigned long SysTickCnt;
 void SysTick_Setup(void);
 void SysTick_Handler(void);
 void Delay_ms(unsigned long tick);
 void drvSysTickDelay(unsigned long tick);
 volatile long UdpCntTime;
 //volatile long CntTimerImagen;
 //volatile uint32_t CntTimerImagen;

#define _5min 300000000
 ////////////////////////////////////////////////////////////////


 #ifdef __cplusplus
 }
 #endif

 #endif /*__DELAY_H */
