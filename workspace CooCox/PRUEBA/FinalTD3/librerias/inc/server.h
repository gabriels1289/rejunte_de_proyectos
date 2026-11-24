/*******************************************************************************
 * @brief   Single packet http server.
 *
 *
 * @file    server.h
 * @author  masu (masu@noemail.non)
 * @date    2013-02-25
 * @version 1.0
 *
 * @note    This is a note.
 *
 * @since   2013-02-13 Initial version.
 ******************************************************************************/

#ifndef __SERVER_H__
#define __SERVER_H__

typedef enum {
	eServerIdle,
	eServerLedOn,
	eServerLedOff
} t_eServerState;

t_eServerState server(t_enc28j60Ctrl * apCtrl);

#endif /* __SERVER_H__ */
