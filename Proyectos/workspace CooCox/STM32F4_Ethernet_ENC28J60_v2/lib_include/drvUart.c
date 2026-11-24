/*******************************************************************************
 * @brief   stm32f4xx UART interface.
 *
 *
 * @file    drvUart.c
 * @author  masu (masu@noemail.non)
 * @date    2013-02-25
 * @version 1.0
 *
 * @note    This is a note.
 *
 * @since   2013-02-13 Initial version.
 ******************************************************************************/

#include "typedef.h"
#include "massert.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "drvUart.h"

const USART_TypeDef * COM_USART[eComPortMax] = {
		DISCOVERY_COM1};

const GPIO_TypeDef * COM_TX_PORT[eComPortMax] = {
		DISCOVERY_COM1_TX_GPIO_PORT};

const GPIO_TypeDef * COM_RX_PORT[eComPortMax] = {
		DISCOVERY_COM1_RX_GPIO_PORT};

const uint32_t COM_USART_CLK[eComPortMax] = {
		DISCOVERY_COM1_CLK};

const uint32_t COM_TX_PORT_CLK[eComPortMax] = {
		DISCOVERY_COM1_TX_GPIO_CLK};

const uint32_t COM_RX_PORT_CLK[eComPortMax] = {
		DISCOVERY_COM1_RX_GPIO_CLK};

const uint16_t COM_TX_PIN[eComPortMax] = {
		DISCOVERY_COM1_TX_PIN};

const uint16_t COM_RX_PIN[eComPortMax] = {
		DISCOVERY_COM1_RX_PIN};

const uint16_t COM_TX_PIN_SOURCE[eComPortMax] = {
		DISCOVERY_COM1_TX_SOURCE};

const uint16_t COM_RX_PIN_SOURCE[eComPortMax] = {
		DISCOVERY_COM1_RX_SOURCE};

const uint16_t COM_TX_AF[eComPortMax] = {
		DISCOVERY_COM1_TX_AF};

const uint16_t COM_RX_AF[eComPortMax] = {
		DISCOVERY_COM1_RX_AF};

static void drvUartInitHw(t_comPort aComPort, const USART_InitTypeDef * apUsartInitStruct) {
	GPIO_InitTypeDef gpioInitStruct;

	mASSERT(IS_COM_PORT(aComPort));
	mASSERT(NULL != apUsartInitStruct);

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(COM_TX_PORT_CLK[aComPort] | COM_RX_PORT_CLK[aComPort], ENABLE);

	/* Enable UART clock */
	if ((RCC_APB2Periph_USART1 == COM_USART_CLK[aComPort]) || \
			(RCC_APB2Periph_USART6 == COM_USART_CLK[aComPort])) {
		RCC_APB2PeriphClockCmd(COM_USART_CLK[aComPort], ENABLE);
	} else {
		RCC_APB1PeriphClockCmd(COM_USART_CLK[aComPort], ENABLE);
	}

	/* Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(COM_TX_PORT[aComPort], COM_TX_PIN_SOURCE[aComPort], COM_TX_AF[aComPort]);

	/* Connect PXx to USARTx_Rx*/
	GPIO_PinAFConfig(COM_RX_PORT[aComPort], COM_RX_PIN_SOURCE[aComPort], COM_RX_AF[aComPort]);

	/* Configure USART Tx as alternate function  */
	gpioInitStruct.GPIO_OType = GPIO_OType_PP;
	gpioInitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	gpioInitStruct.GPIO_Mode  = GPIO_Mode_AF;
	gpioInitStruct.GPIO_Pin   = COM_TX_PIN[aComPort];
	GPIO_Init(COM_TX_PORT[aComPort], &gpioInitStruct);

	/* Configure USART Rx as alternate function  */
	gpioInitStruct.GPIO_Mode  = GPIO_Mode_AF;
	gpioInitStruct.GPIO_Pin   = COM_RX_PIN[aComPort];
	GPIO_Init(COM_RX_PORT[aComPort], &gpioInitStruct);

	/* USART configuration */
	USART_Init(COM_USART[aComPort], apUsartInitStruct);

	/* Enable USART */
	USART_Cmd(COM_USART[aComPort], ENABLE);
}

void drvUartInit(t_comPort aComPort) {
	USART_InitTypeDef initStruct;

	initStruct.USART_BaudRate   = 115200;
	initStruct.USART_WordLength = USART_WordLength_8b;
	initStruct.USART_StopBits   = USART_StopBits_1;
	initStruct.USART_Parity     = USART_Parity_No;
	initStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	initStruct.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;

	drvUartInitHw(aComPort, &initStruct);
}

void drvUartDeInit(t_comPort aComPort) {
	mASSERT(IS_COM_PORT(aComPort));
}

void drvUartTxByteBlck(t_comPort aComPort, unsigned char aChar) {
	mASSERT(IS_COM_PORT(aComPort));

	/* Loop until the end of transmission */
	while (RESET == USART_GetFlagStatus(COM_USART[aComPort], USART_FLAG_TC)) {} ;

	USART_SendData(COM_USART[aComPort], aChar);
}

unsigned char drvUartRxByteBlck(t_comPort aComPort) {
	mASSERT(IS_COM_PORT(aComPort));

	/* Loop until the end of reception */
	while (RESET == USART_GetFlagStatus(COM_USART[aComPort], USART_FLAG_RXNE)) {} ;

	return (unsigned char) USART_ReceiveData(COM_USART[aComPort]);
}
