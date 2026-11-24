/*******************************************************************************
 * @brief   stm32f4xx UART interface.
 *
 *
 * @file    drvUart.h
 * @author  masu (masu@noemail.non)
 * @date    2013-02-25
 * @version 1.0
 *
 * @note    This is a note.
 *
 * @since   2013-02-13 Initial version.
 ******************************************************************************/

#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	eComPort1 = 0,
	eComPortMax
} t_comPort;
#define IS_COM_PORT(a)	((a) == eComPort1)

void 		  drvUartInit      (t_comPort aComPort);
void 		  drvUartDeInit	   (t_comPort aComPort);
void 		  drvUartTxByteBlck(t_comPort aComPort, unsigned char aChar);
unsigned char drvUartRxByteBlck(t_comPort aComPort);

/**
 * @brief Definition for COM port1, connected to USART2
 */
#define DISCOVERY_COM1                        USART2
#define DISCOVERY_COM1_RCC					  RCC_APB1PeriphClockCmd
#define DISCOVERY_COM1_CLK                    RCC_APB1Periph_USART2
#define DISCOVERY_COM1_TX_PIN                 GPIO_Pin_2
#define DISCOVERY_COM1_TX_GPIO_PORT           GPIOA
#define DISCOVERY_COM1_TX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define DISCOVERY_COM1_TX_SOURCE              GPIO_PinSource2
#define DISCOVERY_COM1_TX_AF                  GPIO_AF_USART2
#define DISCOVERY_COM1_RX_PIN                 GPIO_Pin_3
#define DISCOVERY_COM1_RX_GPIO_PORT           GPIOA
#define DISCOVERY_COM1_RX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define DISCOVERY_COM1_RX_SOURCE              GPIO_PinSource3
#define DISCOVERY_COM1_RX_AF                  GPIO_AF_USART2
#define DISCOVERY_COM1_IRQn                   USART2_IRQn

#ifdef __cplusplus
}
#endif

#endif /*__DRV_UART_H__ */
