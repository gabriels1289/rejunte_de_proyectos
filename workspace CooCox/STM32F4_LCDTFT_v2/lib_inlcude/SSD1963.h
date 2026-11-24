/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SSD1963_H
#define __SSD1963_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"
#include "misc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_syscfg.h"

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

//extern uint8_t 	arreglo;
//extern uint16_t ancho, alto;
//extern uint32_t tamanio;

typedef struct SSD_imag_T
{
  const uint8_t *imag_arreglo; // Tabelle mit den Daten
  uint16_t imag_ancho;
  uint16_t imag_alto;
  uint32_t imag_tamanio;        // Anzahl der Bytes
}SSD_imag;

//extern SSD_imag bad_bird;
//extern SSD_imag TMMI;
//extern SSD_imag Apple_iris;
extern SSD_imag tiburon;

uint32_t fclear;
#define TFT_clear(fclear)	TFT_setwindow(0,479,0,271);TFT_fill(480,272,fclear)

void TFT_Write(uint8_t byte);
void TFT_WriteFast(uint8_t byte);
void TFT_WritePixel(uint32_t rgb);
void TFT_WriteCmd(uint8_t cmd);
void TFT_WriteStrCmd(const uint8_t *cmd,unsigned int len);
void TFT_WriteArea(uint32_t rgb,uint32_t area);
void TFT_DumpArea(const uint8_t *img,uint32_t area);
void TFT_setwindow ( uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1 );
void TFT_init_pins(void);
void TFT_init (void);
void TFT_fill(uint16_t x, uint16_t y, uint32_t fcolor);

void TFT_icon(uint16_t x, uint16_t y, uint16_t wx, uint16_t wy, uint8_t *img);
void TFT_plot(uint16_t x,uint16_t y,uint32_t pcolor);

void TFT_draw (SSD_imag *imagen );

//	R G B
#define RED		0xff0000
#define GREEN  	0x00ff00
#define BLUE	0x0000ff
#define BLACK  	0x000000
#define WHITE  	0xffffff
#define GRAY	0x808080
#define YELLOW	0xffff00
#define OTRO	0x000000

////////////////////////////////////////////////////////////////
volatile unsigned long SysTickCnt;
void SysTick_Setup(void);
void SysTick_Handler(void);
void Delay_ms(unsigned long tick);
////////////////////////////////////////////////////////////////


#ifdef __cplusplus
}
#endif

#endif /*__SSD1963_H */
