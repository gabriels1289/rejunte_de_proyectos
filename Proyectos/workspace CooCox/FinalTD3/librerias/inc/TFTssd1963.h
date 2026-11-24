/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TFTSSD1963_H
#define __TFTSSD1963_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include "misc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_syscfg.h"

//------------------------------------------
// Comandos del controlador SSD1963
//------------------------------------------
#define SOFT_RESET            0x01
#define ENTER_PARTIAL_MODE    0x12
#define ENTER_NORMAL_MODE     0x13
#define SET_DISPLAY_OFF       0x28
#define SET_DISPLAY_ON        0x29
#define SET_COLUMN_ADDRESS    0x2A
#define SET_PAGE_ADDRESS      0x2B
#define WRITE_MEMORY_START    0x2C
#define READ_MEMORY_START     0x2E
#define SET_PARTIAL_AREA      0x30
#define SET_SCROLL_AREA       0x33
#define SET_SCROLL_START      0x37
#define EXIT_IDLE_MODE        0x38
#define SET_PIXEL_FORMAT      0x3A
#define WRITE_MEMORY_CONTINUE 0x3C
#define READ_MEMORY_CONTINUE  0x3E
#define SET_LCD_MODE          0xB0
#define SET_HORIZ_PERIOD      0xB4
#define SET_VERT_PERIOD       0xB6
#define SET_PLL               0xE0
#define SET_PIXEL_CLK_FRQ     0xE6
#define SET_PIXEL_DATA_INTERFACE 0xF0

#define SET_LCD_MODE 0xB0
#define SET_ADDRESS_MODE 0x36
#define SET_PLL_MN 0xE2

/* LCD control signals*/							//modificado
#define RSTb 		GPIO_Pin_11
#define RDb			GPIO_Pin_12
#define RSb			GPIO_Pin_13
#define WRb			GPIO_Pin_14
#define CSb			GPIO_Pin_15

#define DataPort	GPIOE									//modificado GPIOA

#define LCD_RSTH	GPIO_SetBits(GPIOB, RSTb)
#define LCD_RDH		GPIO_SetBits(GPIOB, RDb)
#define LCD_RSH 	GPIO_SetBits(GPIOB, RSb)
#define LCD_WRH 	GPIO_SetBits(GPIOB, WRb)
#define LCD_CSH 	GPIO_SetBits(GPIOB, CSb)

#define LCD_RSTL	GPIO_ResetBits(GPIOB, RSTb)
#define LCD_RDL 	GPIO_ResetBits(GPIOB, RDb)
#define LCD_RSL 	GPIO_ResetBits(GPIOB, RSb)
#define LCD_WRL 	GPIO_ResetBits(GPIOB, WRb)
#define LCD_CSL 	GPIO_ResetBits(GPIOB, CSb)

void TFT_Write_Data(uint8_t byte);
void TFT_WriteCmd(uint8_t cmd);
void TFT_init_pins(void);
void TFT_Write_Command_Arg(uint8_t cmd, uint8_t arg);
void TFT_WindowSet( uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1 );
void TFT_WriteArea(uint32_t rgb,uint32_t area);
void TFT_clear(void);
void TFT_Init(void);
void TFT_WriteStrCmd(const uint8_t *cmd,unsigned int len);
void TFT_initMio(void);

//definicion de colores
#define BLACK	0xffffff
#define RED		0x0000ff
#define GREEN	0x00ff00
#define BLUE	0xff0000

#ifdef __cplusplus
}
#endif

#endif /*__TFTSSD1963_H */
