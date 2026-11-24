/*******************************************************************************
 * @brief   stm32f4xx SysTick timer.
 *
 *
 * @file    drvSysTick.h
 * @author  masu (masu@noemail.non)
 * @date    2013-02-25
 * @version 1.0
 *
 * @note    This is a note.
 *
 * @since   2013-02-13 Initial version.
 ******************************************************************************/

#ifndef __DRV_SYS_TICK_H__
#define __DRV_SYS_TICK_H__

#define SYSTICKDIV_TB_1s	1u
#define SYSTICKDIV_TB_1ms	1000u
#define SYSTICKDIV_TB_1us	1000000u

int  drvSysTickInit     (unsigned int aTimeBaseDiv);
void drvSysTickDelay    (unsigned long aTime);
void drvSysTickDecrement(void);

#endif /* __DRV_SYS_TICK_H__ */
