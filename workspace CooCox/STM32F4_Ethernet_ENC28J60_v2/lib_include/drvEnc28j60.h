/*******************************************************************************
 * @brief   stm32f4xx SPI interface for ENC28J60 Ethernet controller.
 *
 *
 * @file    drvEnc28j60.h
 * @author  masu (masu@noemail.non)
 * @date    2013-02-25
 * @version 1.0
 *
 * @note    This is a note.
 *
 * @since   2013-02-13 Initial version.
 ******************************************************************************/

#ifndef __DRV_ENC28J60_H__
#define __DRV_ENC28J60_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_gpio.h"

/* SPIx Communication Interface */
/*
 * SPI1 SCK  PA5 -> ENC28J60(SCK)
 * SPI1 MISO PA6 -> ENC28J60(SO)
 * SPI1 MOSI PA7 -> ENC28J60(SI)
 * SPI1 NSS  PA4 -> ENC28J60(CS)
 */
#define SPIx                           SPI1
#define SPIx_CLK                       RCC_APB2Periph_SPI1
#define SPIx_CLK_INIT                  RCC_APB2PeriphClockCmd
#define SPIx_IRQn                      SPI1_IRQn
#define SPIx_IRQHANDLER                SPI1_IRQHandler

#define SPIx_SCK_PIN                   GPIO_Pin_5
#define SPIx_SCK_GPIO_PORT             GPIOA
#define SPIx_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define SPIx_SCK_SOURCE                GPIO_PinSource5
#define SPIx_SCK_AF                    GPIO_AF_SPI1

#define SPIx_MISO_PIN                  GPIO_Pin_6
#define SPIx_MISO_GPIO_PORT            GPIOA
#define SPIx_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define SPIx_MISO_SOURCE               GPIO_PinSource6
#define SPIx_MISO_AF                   GPIO_AF_SPI1

#define SPIx_MOSI_PIN                  GPIO_Pin_7
#define SPIx_MOSI_GPIO_PORT            GPIOA
#define SPIx_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define SPIx_MOSI_SOURCE               GPIO_PinSource7
#define SPIx_MOSI_AF                   GPIO_AF_SPI1

#define SPIx_CS_PIN                    GPIO_Pin_4
#define SPIx_CS_GPIO_PORT              GPIOA
#define SPIx_CS_GPIO_CLK               RCC_AHB1Periph_GPIOA

void          drvEnc28j60_init     	   (void);
void 		  drvEnc28j60_deInit	   (void);
unsigned char drvEnc28j60_readWriteByte(unsigned char aSendData);
void          drvEnc28j60_select	   (void);
void          drvEnc28j60_unselect	   (void);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_ENC28J60_H__ */
