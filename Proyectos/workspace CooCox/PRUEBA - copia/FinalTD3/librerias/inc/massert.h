/*******************************************************************************
 * @brief   ASSERT() functions.
 *
 *
 * @file    massert.h
 * @author  masu (masu@noemail.non)
 * @date    2013-02-25
 * @version 1.0
 *
 * @since   2013-02-25 Initial version.
 ******************************************************************************/

#ifndef __MASSERT_H__
#define __MASSERT_H__

#define USE_MASSERT

#ifdef  USE_MASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *   which reports the name of the source file and the source
  *   line number of the call that failed.
  *   If expr is true, it returns no value.
  * @retval None
  */
  #define mASSERT(expr) ((expr) ? (void)0 : massert((unsigned char *) __FILE__, __LINE__))

  void massert(unsigned char * apFile, unsigned long line);
#else
  #define mASSERT(expr) ((void)0)
#endif /* USE_MASSERT */

#endif /* __MASSERT_H__ */
