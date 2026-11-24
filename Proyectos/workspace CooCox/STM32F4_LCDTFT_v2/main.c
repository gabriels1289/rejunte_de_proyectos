//--------------------------------------------------------------
// File     : main.c
// Datum    : 07.02.2014
// Version  : 1.0
// Autor    : MEPaz
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.0
//--------------------------------------------------------------

#include "stm32f4xx.h"
#include "stm32_ub_led.h"

#include "SSD1963.h"

int main(void)
{
	SystemInit();

	UB_Led_Init();
	TFT_init_pins();							//inicializacion de los pines del TFT
	SysTick_Setup();
    Delay_ms (500);

	TFT_init();
    TFT_clear(WHITE);

    Delay_ms (2000);

    //TFT_draw(&TMMI);
    //TFT_draw(&Apple_iris);
    TFT_draw(&tiburon);

    //TFT_setwindow ( 240-ancho/2, 239+ancho/2, 136-alto/2, 135+alto/2);
    //TFT_DumpArea(&bad_bird->arreglo, tamanio);

    while(1)
    {
    }
}
