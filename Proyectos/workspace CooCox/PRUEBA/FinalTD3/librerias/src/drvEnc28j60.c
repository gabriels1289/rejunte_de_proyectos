/*******************************************************************************
 * @brief   stm32f4xx SPI interface for ENC28J60 Ethernet controller.
 *
 *
 * @file    drvEnc28j60.c
 * @author  masu (masu@noemail.non)
 * @date    2013-02-25
 * @version 1.0
 *
 * @note    This is a note.
 *
 * @since   2013-02-13 Initial version.
 ******************************************************************************/

#include "massert.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_spi.h"
#include "drvEnc28j60.h"

/**
  * @fn      void drvEnc28j60_Init(void)
  * @brief   This function initialize SPIx interface.
  *
  *          This function initialize SPIx interface.
  *
  */
void drvEnc28j60_init(void) {
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/*!< Enable the SPI clock */
	SPIx_CLK_INIT(SPIx_CLK, ENABLE);

	/*!< Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(SPIx_SCK_GPIO_CLK | \
			SPIx_MISO_GPIO_CLK | \
			SPIx_MOSI_GPIO_CLK | \
			SPIx_CS_GPIO_CLK, ENABLE);

	/*!< SPI GPIO Configuration */
	/*!< Connect SPI pins to AF */
	GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT,  SPIx_SCK_SOURCE,  SPIx_SCK_AF);
	GPIO_PinAFConfig(SPIx_MISO_GPIO_PORT, SPIx_MISO_SOURCE, SPIx_MISO_AF);
	GPIO_PinAFConfig(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_SOURCE, SPIx_MOSI_AF);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	/*!< SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = SPIx_SCK_PIN;
	GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);

	/*!< SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPIx_MISO_PIN;
	GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStructure);

	/*!< SPI MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPIx_MOSI_PIN;
	GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure SPI CS pin in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin   = SPIx_CS_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(SPIx_CS_GPIO_PORT, &GPIO_InitStructure);

	/*!< SPI configuration */
	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode      		= SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize  		= SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL      		= SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA      		= SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; /* Max 20MHz */
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial 	= 7;
	SPI_Init(SPIx, &SPI_InitStructure);

	/*!< Enable SPI */
	SPI_Cmd(SPIx, ENABLE);
}

/**
  * @fn      void drvEnc28j60_DeInit(void)
  * @brief   This function deinitialize SPIx interface.
  *
  *          This function deinitialize SPIx interface.
  *
  */
void drvEnc28j60_deInit(void) {
	/*!< Enable SPI */
	SPI_Cmd(SPIx, DISABLE);

	SPI_I2S_DeInit(SPIx);
}

/**
  * @fn      unsigned char drvEnc28j60_ReadWriteByte(unsigned char aSendData)
  * @brief   This function send \a aSendData byte over SPIx bus and read back input byte.
  *
  *          This function send \a aSendData byte over SPIx bus and read back input byte.
  *
  * @param[in] aSendData Send byte.
  * @return  Returns calculated read byte.
  *
  */
unsigned char drvEnc28j60_readWriteByte(unsigned char aSendData) {
	unsigned char recvData;

	/* TX must be empty */
	mASSERT(SET == SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE));

	/* Send byte through the SPIx peripheral */
	SPI_I2S_SendData(SPIx, aSendData);

	/* Wait to receive a byte */
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET) {} ;
	recvData = SPI_I2S_ReceiveData(SPIx);

	return recvData;
}

void drvEnc28j60_select(void) {
	GPIO_ResetBits(SPIx_CS_GPIO_PORT, SPIx_CS_PIN);
}

void drvEnc28j60_unselect(void) {
	GPIO_SetBits(SPIx_CS_GPIO_PORT, SPIx_CS_PIN);
}
