//--------------------------------------------------------------
// File     : SSD1963.c
//--------------------------------------------------------------

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "SSD1963.h"

TFT_PIN_t TFT_PIN[] = {
// Name ,PORT 	, PIN        	, CLOCK              	, Init
  {RS 	,GPIOD	,GPIO_Pin_3  	,RCC_AHB1Periph_GPIOD	,TFT_PIN_OFF},
  {WR	,GPIOD	,GPIO_Pin_4  	,RCC_AHB1Periph_GPIOD	,TFT_PIN_OFF},
  {RD   ,GPIOD	,GPIO_Pin_5 	,RCC_AHB1Periph_GPIOD	,TFT_PIN_OFF},
  {CS  	,GPIOD	,GPIO_Pin_6 	,RCC_AHB1Periph_GPIOD	,TFT_PIN_OFF},
  {RST  ,GPIOD	,GPIO_Pin_7 	,RCC_AHB1Periph_GPIOD	,TFT_PIN_OFF},
  {D_P  ,GPIOC	,GPIO_Pin_All	,RCC_AHB1Periph_GPIOC	,TFT_PIN_OFF}, 		// data port PC8 ~ PC15
};

void TFT_Init_Pins(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TFT_PIN_NAME_t tft_pin_name;

	for(tft_pin_name = 0 ; tft_pin_name < TFT_PIN_ANZ ; tft_pin_name++)
	{
		RCC_AHB1PeriphClockCmd(TFT_PIN[tft_pin_name].TFT_PIN_CLK, ENABLE);

		GPIO_InitStructure.GPIO_Pin = TFT_PIN[tft_pin_name].TFT_PIN_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(TFT_PIN[tft_pin_name].TFT_PIN_PORT, &GPIO_InitStructure);

		if		(TFT_PIN[tft_pin_name].TFT_PIN_INIT == TFT_PIN_OFF)
			TFT_OFF(tft_pin_name);

		else if (TFT_PIN[tft_pin_name].TFT_PIN_INIT == TFT_PIN_ON)
			TFT_ON(tft_pin_name);
	}
}

void TFT_Init (void)
{
	const static uint8_t init_string1[]={
	0xE0,
	0x01								// START PLL
	};

	const static uint8_t init_string2[]={
	0xE0,
	0x03								// LOCK PLL
	};

	const static uint8_t init_string3[]={
	0xB0,
	0x07,									//SET LCD MODE  SET TFT 18Bits MODE
	0x80,									//SET TFT MODE & hsync+Vsync+DEN MODE
	0x01,0xDF,								//SET horizontal size=480-1
	0x01,0x0F,								//SET vertical size=272-1
	0x00									//SET even/odd line RGB seq.=RGB
	};

	const static uint8_t init_string4[]={
	0xF0,
	0x00								//SET pixel data I/F format=8bit
	};

	const static uint8_t init_string5[]={
	0x36,
	0x88								// SET address mode		B-G-R
	};

	const static uint8_t init_string6[]={
	0x3A,
	0x70								// SET R G B format = 6 6 6			//modificado RGB = 8 8 8
	};

	const static uint8_t init_string7[]={
	0xE2,
	0x23,0x02,
	0x04						//SET PLL freq=113.33MHz
	};

	const static uint8_t init_string8[]={
	0xE6,
	0x03,0xFF,
	0xFF						//SET PCLK freq=4.94MHz  ; pixel clock frequency
	};

	const static uint8_t init_string9[]={
	0xB4,									//SET HBP,
	0x02,0xDF,								//SET HSYNC Total = 760
	0x00,0xD2,								//SET HBP = 68
	0x01,									//SET VBP 16=15+1
	0x00,0x00,								//SET Hsync pulse start position
	0x00									//SET Hsync pulse subpixel start position
	};

	const static uint8_t init_string10[]={
	0xB6,									//SET VBP,
	0x01,0x3C,								//SET Vsync total = 504
	0x00,0x22,								//SET VBP = 19
	0x01,									//SET Vsync pulse 8=7+1
	0x00,0x00								//SET Vsync pulse start position
	};

	LCD_CSH;

	LCD_RSTH;
	Delay_ms(5);
	LCD_RSTL;
	Delay_ms(5);
	LCD_RSTH;

	LCD_CSH;
	LCD_RDH;
	LCD_WRH;

	Delay_ms(10);

	LCD_CSL;

	TFT_WriteCmd ( 0x01 );	// Software reset
	TFT_WriteCmd ( 0x01 );	// Software reset
	TFT_WriteCmd ( 0x01 );	// Software reset
	Delay_ms(10);

	TFT_WriteStrCmd ( init_string7,sizeof(init_string7) );
	TFT_WriteStrCmd ( init_string1,sizeof(init_string1) );
	Delay_ms(10);

	TFT_WriteStrCmd ( init_string2,sizeof(init_string2) );
	Delay_ms(10);
	TFT_WriteCmd ( 0x01 );	// Software reset

	TFT_WriteStrCmd ( init_string8,sizeof(init_string8) );
	TFT_WriteStrCmd ( init_string3,sizeof(init_string3) );
	TFT_WriteStrCmd ( init_string9,sizeof(init_string9) );
	TFT_WriteStrCmd ( init_string10,sizeof(init_string10) );
	TFT_WriteStrCmd ( init_string4,sizeof(init_string4) );
	TFT_WriteStrCmd ( init_string5,sizeof(init_string5) );
	TFT_WriteStrCmd ( init_string6,sizeof(init_string6) );
	TFT_SetWindow ( 0, 479, 0, 271);

	TFT_WriteCmd ( 0x29 );	// Display ON
}

void TFT_OFF(TFT_PIN_NAME_t tft_pin_name)
{
	TFT_PIN[tft_pin_name].TFT_PIN_PORT->BSRRH = TFT_PIN[tft_pin_name].TFT_PIN_PIN;
}

void TFT_ON(TFT_PIN_NAME_t tft_pin_name)
{
	TFT_PIN[tft_pin_name].TFT_PIN_PORT->BSRRL = TFT_PIN[tft_pin_name].TFT_PIN_PIN;
}

void TFT_Write(uint16_t byte)
{
	GPIO_Write(DataPort, byte<<0x08);						// data out
	LCD_WRL;
	LCD_WRH;
}

void TFT_WriteCmd(uint8_t cmd)
{
	LCD_RSL;			// Lower RS (Cmd)
	TFT_Write(cmd);
	LCD_RSH;			// Rise RS (Data)
}

void TFT_WriteStrCmd(const uint8_t *cmd,unsigned int len)
{
	TFT_WriteCmd(*cmd++);
	while(len--)
	{
		TFT_Write(*cmd++);
	}
}

void TFT_WriteArea(uint32_t rgb,uint32_t area)
{
	TFT_WriteCmd(0x2C);
	while(area--)
	{
		TFT_Write(((uint32_t)rgb>>16)&0xFF);	// LCD_WritePixel(rgb)
		TFT_Write(((uint32_t)rgb>>8)&0xFF);
		TFT_Write(((uint32_t)rgb>>0)&0xFF);
	}
}

void TFT_DumpArea(const uint8_t *img,uint32_t area)
{
	TFT_WriteCmd(0x2C);
	while(area--)
	{
		TFT_Write(*img++);
		TFT_Write(*img++);
		TFT_Write(*img++);
	}
}

void TFT_SetWindow ( uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1 )
{
	TFT_WriteCmd(0x2A);					// SET column address

	TFT_Write(((unsigned)x0>>8)&0xFF);	// HI byte
	TFT_Write(x0&0xFF);					// LO byte

	TFT_Write(((unsigned)x1>>8)&0xFF);	// HI byte
	TFT_Write(x1&0xFF);					// LO byte

	TFT_WriteCmd(0x2B);					// SET page address

	TFT_Write(((unsigned)y0>>8)&0xFF);	// HI byte
	TFT_Write(y0&0xFF);					// LO byte

	TFT_Write(((unsigned)y1>>8)&0xFF);	// HI byte
	TFT_Write(y1&0xFF);					// LO byte
}

//Para iluminar un punto de pantalla, definimos un Area de 1x1:
void TFT_Plot(uint16_t x,uint16_t y,uint32_t pcolor)
{
	TFT_SetWindow (x,x,y,y);
	TFT_WriteArea(pcolor,(uint32_t)x * y);
}

//Para colocar un color en un area del display (o borrarlo):
void TFT_Area(uint16_t x, uint16_t y, uint16_t wx, uint16_t wy, uint32_t fcolor)
{
	TFT_SetWindow ( x, wx+x-1, y, wy+y-1);
	TFT_WriteArea(fcolor,(uint32_t)wx * wy);
}

//Para mostrar un Icono:
void TFT_Icon(uint16_t x, uint16_t y, uint16_t wx, uint16_t wy, uint8_t *img)
{
	TFT_SetWindow ( x, wx+x-1, y, wy+y-1);
	TFT_DumpArea(img,(uint32_t)wx * wy);
}
//Para mostrar una Imagen centrada en pantalla:
void TFT_Draw (SSD_imag *imagen )
{
	uint16_t ancho 	= imagen->imag_ancho;
	uint16_t alto 	= imagen->imag_alto;

	TFT_SetWindow ( 240-ancho/2, 239+ancho/2, 136-alto/2, 135+alto/2);
    TFT_DumpArea(imagen->imag_arreglo, ancho*alto);
}

/*****************************************************************/

// SysTick Setup
void SysTick_Setup(void)
{
	RCC_ClocksTypeDef RCC_Clocks;

	/* SysTick end of count event each 1ms */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	//modificado 1000
}

// SysTick Interrupt Handler (tick cada 1ms)
void SysTick_Handler(void)
{
	SysTickCnt++;
}

void Delay_ms(unsigned long tick)
{
	unsigned long systickcnt;

	systickcnt = SysTickCnt;
	while ((SysTickCnt - systickcnt) < tick);
}
