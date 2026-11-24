/*******************************************************************************
 * @brief   stm32f4xx SysTick timer.
 *
 *
 * @file    drvSysTick.c
 * @author  masu (masu@noemail.non)
 * @date    2013-02-25
 * @version 1.0
 *
 * @note    This is a note.
 *
 * @since   2013-02-13 Initial version.
 ******************************************************************************/

#include "stm32f4xx.h"
#include "massert.h"
#include "drvSysTick.h"

extern unsigned long          SystemCoreClock;
static volatile unsigned long s_sysTickDelay;

int drvSysTickInit(unsigned int aTimeBaseDiv)
{
	int ret = 0;
	unsigned int reloadValue = SystemCoreClock/aTimeBaseDiv;

	mASSERT(0xFFFFFFu >= reloadValue);

	if (0 < SysTick_Config(reloadValue)) {
		ret = -1;
	}

	return ret;
}

void drvSysTickDelay(unsigned long aTime)
{
	s_sysTickDelay = aTime;

	while(s_sysTickDelay != 0);
}


void drvSysTickDecrement(void)
{
	if (s_sysTickDelay != 0x00)
	{
		s_sysTickDelay--;
	}
}
