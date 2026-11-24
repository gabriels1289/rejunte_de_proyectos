
//--------------------------------------------------------------
// File     : SSD1963.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __SSD1963_H
#define __SSD1963_H

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "misc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_syscfg.h"

typedef struct SSD_imag_T {
	const uint8_t 	*imag_arreglo;
	uint16_t 		imag_ancho;
	uint16_t 		imag_alto;
	uint32_t 		imag_tamanio;
}SSD_imag;

//extern SSD_imag bad_bird;
//extern SSD_imag TMMI;
extern SSD_imag Apple_iris;
//extern SSD_imag Apple_Galaxy;
extern SSD_imag Avril_Lavigne;
//extern SSD_imag tiburon;

typedef enum {
	RS 	= 0,
	WR 	= 1,
	RD 	= 2,
	CS 	= 3,
	RST	= 4,
	D_P = 5
}TFT_PIN_NAME_t;

#define  TFT_PIN_ANZ   6

typedef enum {
	TFT_PIN_OFF = 0,
	TFT_PIN_ON  = 1,
}TFT_PIN_STATUS_t;

typedef struct {
	TFT_PIN_NAME_t 		TFT_PIN_NAME;	// Name
	GPIO_TypeDef*  		TFT_PIN_PORT; 	// Port
	const uint16_t 		TFT_PIN_PIN; 	// Pin
	const uint32_t 		TFT_PIN_CLK; 	// Clock
	TFT_PIN_STATUS_t   	TFT_PIN_INIT;  	// Init
}TFT_PIN_t;

#define DataPort	TFT_PIN[D_P].TFT_PIN_PORT

#define LCD_RSTH	TFT_ON(RST)
#define LCD_RDH		TFT_ON(RD)
#define LCD_RSH 	TFT_ON(RS)
#define LCD_WRH 	TFT_ON(WR)
#define LCD_CSH 	TFT_ON(CS)

#define LCD_RSTL	TFT_OFF(RST)
#define LCD_RDL 	TFT_OFF(RD)
#define LCD_RSL 	TFT_OFF(RS)
#define LCD_WRL 	TFT_OFF(WR)
#define LCD_CSL 	TFT_OFF(CS)

void TFT_OFF(TFT_PIN_NAME_t tft_pin_name);
void TFT_ON (TFT_PIN_NAME_t tft_pin_name);

uint32_t fclear;
#define TFT_Clear(fclear)	TFT_Area(0,0,480,272,fclear)

void TFT_Init_Pins(void);
void TFT_Init (void);

void TFT_Write(uint16_t byte);
void TFT_WriteCmd(uint8_t cmd);
void TFT_WriteStrCmd(const uint8_t *cmd,unsigned int len);
void TFT_WriteArea(uint32_t rgb,uint32_t area);
void TFT_DumpArea(const uint8_t *img,uint32_t area);
void TFT_SetWindow(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1);
void TFT_Plot(uint16_t x, uint16_t y, uint32_t pcolor);
void TFT_Area(uint16_t x, uint16_t y, uint16_t wx, uint16_t wy, uint32_t fcolor);		// x - y = posicion, wx - wy = ancho y alto del area
void TFT_Icon(uint16_t x, uint16_t y, uint16_t wx, uint16_t wy, uint8_t *img);			// x - y = posicion, wx - wy = ancho y alto del icono
void TFT_Draw (SSD_imag *imagen );

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
