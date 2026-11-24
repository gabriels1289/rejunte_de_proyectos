/*******************************************************************************
 * @brief   Standard input/output interface.
 *
 *
 * @file    mstdio.h
 * @author  masu (masu@noemail.non)
 * @date    2013-02-25
 * @version 1.0
 *
 * @note    This is a note.
 *
 * @since   2013-02-13 Initial version.
 ******************************************************************************/

#ifndef __MSTDIO_H__
#define __MSTDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

void mstdioInit  (void);
void mstdioDeInit(void);
char mstdioIn(void);
void mstdioOut(unsigned char aChar);
void mstdioErr(unsigned char aChar);

#ifdef __cplusplus
}
#endif

#endif /* __MSTDIO_H__ */
