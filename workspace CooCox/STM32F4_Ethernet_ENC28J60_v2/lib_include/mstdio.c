/*******************************************************************************
 * @brief   Standard input/output interface.
 *
 *
 * @file    mstdio.c
 * @author  masu (masu@noemail.non)
 * @date    2013-02-25
 * @version 1.0
 *
 * @note    This is a note.
 *
 * @since   2013-02-13 Initial version.
 ******************************************************************************/

#include <stdio.h>
#include "drvUart.h"
#include "mstdio.h"

static t_comPort s_defaultIoPort;

void mstdioInit(void) {
	s_defaultIoPort = eComPort1;

	/* Turn off buffers, so IO occurs immediately */
	setvbuf(stdin,  NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	drvUartInit(s_defaultIoPort);
}

void mstdioDeInit(void) {
	drvUartDeInit(s_defaultIoPort);
}

char mstdioIn(void) {
	return drvUartRxByteBlck(s_defaultIoPort);
}

void mstdioOut(unsigned char aChar) {
	drvUartTxByteBlck(s_defaultIoPort, aChar);
}

void mstdioErr(unsigned char aChar) {
	drvUartTxByteBlck(s_defaultIoPort, aChar);
}

