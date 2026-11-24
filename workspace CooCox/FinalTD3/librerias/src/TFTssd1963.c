#include "TFTssd1963.h"
#include "delay.h"

void TFT_Write_Data(uint8_t byte)
{
	GPIO_Write(DataPort, byte&0xFF);						// data out
	LCD_WRL;
	LCD_CSL;
	LCD_CSH;
	LCD_WRH;
}

void TFT_WriteCmd(uint8_t cmd)
{
	LCD_RSL;			// Lower RS (Cmd)
	TFT_Write_Data(cmd);
	LCD_RSH;			// Rise RS (Data)
}

void TFT_Write_Command_Arg(uint8_t cmd, uint8_t arg)
{
	TFT_WriteCmd(cmd);
	TFT_Write_Data(arg);
}

void TFT_WindowSet( uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1 )
{
	TFT_WriteCmd(SET_COLUMN_ADDRESS);					// SET column address

	TFT_Write_Data(((unsigned)x0>>8)&0xFF);	// HI byte
	TFT_Write_Data(x0&0xFF);					// LO byte

	TFT_Write_Data(((unsigned)x1>>8)&0xFF);	// HI byte
	TFT_Write_Data(x1&0xFF);					// LO byte

	TFT_WriteCmd(SET_PAGE_ADDRESS);					// SET page address

	TFT_Write_Data(((unsigned)y0>>8)&0xFF);	// HI byte
	TFT_Write_Data(y0&0xFF);					// LO byte

	TFT_Write_Data(((unsigned)y1>>8)&0xFF);	// HI byte
	TFT_Write_Data(y1&0xFF);					// LO byte
}

void TFT_WriteArea(uint32_t rgb,uint32_t area)
{
	TFT_WriteCmd(WRITE_MEMORY_START);
	while(area--)
	{
		TFT_Write_Data(((uint32_t)rgb>>16)&0xFF);
		TFT_Write_Data(((uint32_t)rgb>>8)&0xFF);
		TFT_Write_Data(((uint32_t)rgb>>0)&0xFF);
	}
}

void TFT_clear(void)
{
	TFT_WindowSet(0,479,0,815);
	TFT_WriteArea(BLACK, 391680); //480*816
}

void TFT_init_pins(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* GPIOB Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* Configure PB11, PB12, PB13, PB14 and PB15 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* GPIOA Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	/* Configure Pin_All in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);								//modificado GPIOA
}

//--------------------------------------------------------
// Inicializa el display y el controlador TFT
//--------------------------------------------------------
void TFT_Init(void)
{
	Delay_ms(1000); // Para dar tiempo a que se inicie el display (el micro puede arrancar antes)
	LCD_CSL;
	TFT_WriteCmd(SET_DISPLAY_OFF);
	LCD_RDH;
	LCD_WRL;
	LCD_RSTH;//RST_ON();
	Delay_ms(100);
	LCD_RSTL;//RST_OFF();
	Delay_ms(100);
	LCD_CSH;
	LCD_WRH;

	TFT_WriteCmd(SOFT_RESET);
	Delay_ms(100);
	TFT_WriteCmd(SOFT_RESET);
	Delay_ms(100);

	TFT_Write_Command_Arg(SET_PLL,0x01); //START PLL
	TFT_Write_Command_Arg(SET_PLL,0x03); //LOCK PLL

	TFT_WriteCmd(SET_LCD_MODE); //SET LCD MODE SET TFT 18Bits MODE
	TFT_Write_Data(0x08); //SET TFT MODE - 0x0c
	TFT_Write_Data(0x80); //SET TFT MODE & hsync+Vsync+DEN MODE
	TFT_Write_Data(479>>8); //SET horizontal size=480-1 HightByte
	TFT_Write_Data(479&0xff); //SET horizontal size=480-1 LowByte
	TFT_Write_Data(271>>8); //SET vertical size=272-1 HightByte
	TFT_Write_Data(271&0xff); //SET vertical size=272-1 LowByte
	TFT_Write_Data(0x00); //SET even/odd line RGB seq.=RGB

	TFT_Write_Command_Arg(SET_PIXEL_DATA_INTERFACE,0x00); //SET pixel data I/F format=8bit
	TFT_Write_Command_Arg(SET_PIXEL_FORMAT,0x60); //SET R G B format = 6 6 6  (Cuanto vale set_pixel_format

	TFT_WriteCmd(SET_PIXEL_CLK_FRQ); //Pixel CLK Freq=9 MHz ; pixel clock frequency  (SET_LSHIFT_FREQ??)
	TFT_Write_Data(0x01); //02
	TFT_Write_Data(0x45); //ff
	TFT_Write_Data(0x47); //ff

	TFT_WriteCmd(SET_HORIZ_PERIOD);
	TFT_Write_Data(525>>8); //SET HSYNC Total=525
	TFT_Write_Data(525&0xff);
	TFT_Write_Data(43>>8); //SET HBP=43
	TFT_Write_Data(43&0xff);
	TFT_Write_Data(40); //SET VBP 41=40+1
	TFT_Write_Data(0x00); //SET Hsync pulse start position
	TFT_Write_Data(0x00);
	TFT_Write_Data(0x00); //SET Hsync pulse subpixel start position

	TFT_WriteCmd(SET_VERT_PERIOD);
	TFT_Write_Data(285>>8); //SET Vsync total=286=285+1
	TFT_Write_Data(285&0xff);
	TFT_Write_Data(12>>8); //SET VBP=12
	TFT_Write_Data(12&0xff);
	TFT_Write_Data(9); //SET Vsync pulse 10=9+1
	TFT_Write_Data(0x00); //SET Vsync pulse start position
	TFT_Write_Data(0x00);

	TFT_WriteCmd(SET_DISPLAY_OFF);//DISP_OFF();

	TFT_WindowSet(0,479,0,815);
	TFT_WriteCmd(ENTER_NORMAL_MODE);
	TFT_WriteCmd(EXIT_IDLE_MODE);
	TFT_WriteCmd(SET_DISPLAY_ON);
	TFT_WriteCmd(WRITE_MEMORY_START);

	TFT_WriteCmd(SET_DISPLAY_ON);//DISP_ON();

	// Define que el área de scroll ocupa la totalidad del display.
	// Luego, con el comando SET_SCROLL_START se especifica la línea inicial
	// para cada pantalla.
	TFT_WriteCmd(SET_SCROLL_AREA);
	TFT_Write_Data(0); // Top fixed area number of lines from the top of frame buffer
	TFT_Write_Data(0);
	TFT_Write_Data(272>>8); // Vertical scrolling area in number of bytes
	TFT_Write_Data(272&0xff);
	TFT_Write_Data(816>>8); // Bottom fixed area number of lines from the top of frame buffer
	TFT_Write_Data(816&0xff);

	TFT_clear();

	// Inicialmente muestra la primera pantalla (líneas 0 a 271)
	TFT_WriteCmd(SET_SCROLL_START);
	TFT_Write_Data(0>>8);
	TFT_Write_Data(0&0xff);
}

void TFT_WriteStrCmd(const uint8_t *cmd,unsigned int len)
{
	TFT_WriteCmd(*cmd++);
	while(len--)
	{
		TFT_Write_Data(*cmd++);
	}
}

void TFT_initMio(void)
{
	const static uint8_t init_string1[]={
			SET_PLL,
			0x01								// START PLL
	};

	const static uint8_t init_string2[]={
			SET_PLL,
			0x03								// LOCK PLL
	};

	const static uint8_t init_string3[]={
			SET_LCD_MODE,
			0x07,									//SET LCD MODE  SET TFT 18Bits MODE
			0x80,									//SET TFT MODE & hsync+Vsync+DEN MODE
			0x01,0xDF,								//SET horizontal size=480-1
			0x01,0x0F,								//SET vertical size=272-1
			0x00									//SET even/odd line RGB seq.=RGB
	};

	const static uint8_t init_string4[]={
			SET_PIXEL_DATA_INTERFACE,
			0x00								//SET pixel data I/F format=8bit
	};

	const static uint8_t init_string5[]={
			SET_ADDRESS_MODE,
			0x88								// SET address mode		B-G-R
	};

	const static uint8_t init_string6[]={
			SET_PIXEL_FORMAT, //no existe
			0x70								// SET R G B format = 6 6 6			//modificado RGB = 8 8 8
	};

	const static uint8_t init_string7[]={
			SET_PLL_MN,
			0x23,0x02,
			0x04						//SET PLL freq=113.33MHz
	};

	const static uint8_t init_string8[]={
			SET_PIXEL_CLK_FRQ,
			0x03,0xFF,
			0xFF						//SET PCLK freq=4.94MHz  ; pixel clock frequency
	};

	const static uint8_t init_string9[]={
			SET_HORIZ_PERIOD,									//SET HBP,
			0x02,0xDF,								//SET HSYNC Total = 760
			0x00,0xD2,								//SET HBP = 68
			0x01,									//SET VBP 16=15+1
			0x00,0x00,								//SET Hsync pulse start position
			0x00									//SET Hsync pulse subpixel start position
	};

	const static uint8_t init_string10[]={
			SET_VERT_PERIOD,									//SET VBP,
			0x01,0x3C,								//SET Vsync total = 504
			0x00,0x22,								//SET VBP = 19
			0x01,									//SET Vsync pulse 8=7+1
			0x00,0x00								//SET Vsync pulse start position
	};

	Delay_ms(1000);
	LCD_CSL;

	LCD_RSTH;
	Delay_ms(5);
	LCD_RSTL;
	Delay_ms(5);
	LCD_RSTH;

	LCD_CSH;
	LCD_RDH;
	LCD_WRH;

	Delay_ms(10);

	LCD_CSH;

	TFT_WriteCmd(0x01);	// Software reset
	TFT_WriteCmd(0x01);	// Software reset
	TFT_WriteCmd(0x01);	// Software reset
	Delay_ms(10);

	TFT_WriteStrCmd(init_string7,sizeof(init_string7));
	TFT_WriteStrCmd(init_string1,sizeof(init_string1));
	Delay_ms(10);

	TFT_WriteStrCmd(init_string2,sizeof(init_string2));
	Delay_ms(10);
	TFT_WriteCmd(0x01);	// Software reset

	TFT_WriteStrCmd(init_string8,sizeof(init_string8));
	TFT_WriteStrCmd(init_string3,sizeof(init_string3));
	TFT_WriteStrCmd(init_string9,sizeof(init_string9));
	TFT_WriteStrCmd(init_string10,sizeof(init_string10));
	TFT_WriteStrCmd(init_string4,sizeof(init_string4));
	TFT_WriteStrCmd(init_string5,sizeof(init_string5));
	TFT_WriteStrCmd(init_string6,sizeof(init_string6));
	//TFT_WindowSet(0,479,0,271);
	TFT_WriteCmd(0x29);	// Display ON

	// Define que el área de scroll ocupa la totalidad del display.
	// Luego, con el comando SET_SCROLL_START se especifica la línea inicial
	// para cada pantalla.
	TFT_WriteCmd(SET_SCROLL_AREA);
	TFT_Write_Data(0); // Top fixed area number of lines from the top of frame buffer
	TFT_Write_Data(0);
	TFT_Write_Data(272>>8); // Vertical scrolling area in number of bytes
	TFT_Write_Data(272&0xff);
	TFT_Write_Data(816>>8); // Bottom fixed area number of lines from the top of frame buffer
	TFT_Write_Data(816&0xff);

	TFT_clear();

	// Inicialmente muestra la primera pantalla (líneas 0 a 271)
	TFT_WriteCmd(SET_SCROLL_START);
	TFT_Write_Data(0>>8);
	TFT_Write_Data(0&0xff);


}
