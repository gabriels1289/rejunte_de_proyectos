//--------------------------------------------------------------
// File     : main.c
// Datum    : 07.02.2014
// Version  : 1.0
// Autor    : MEPaz
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.0
//--------------------------------------------------------------

#include "stm32f4xx.h"
//#include "stm32_ub_led.h"
#include "SSD1963.h"

int main(void)
{
	SystemInit();

	//UB_Led_Init();
	TFT_Init_Pins();							//inicializacion de los pines del TFT
	SysTick_Setup();
    Delay_ms (500);

	TFT_Init();
    TFT_Clear(BLACK);
    Delay_ms (1000);

    while(1)
    {
        TFT_Draw(&Apple_iris);
        Delay_ms (5000);
        TFT_Draw(&Avril_Lavigne);
    	Delay_ms (5000);
    }
}
