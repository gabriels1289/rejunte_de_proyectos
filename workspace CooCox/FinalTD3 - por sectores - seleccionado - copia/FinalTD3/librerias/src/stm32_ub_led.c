//--------------------------------------------------------------
// File     : stm32_ub_led.c
//--------------------------------------------------------------

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_led.h"

//--------------------------------------------------------------
LED_t LED[] = {
  // Name    ,PORT , PIN       , CLOCK              , Init
  {LED_GREEN ,GPIOD,GPIO_Pin_12,RCC_AHB1Periph_GPIOD,LED_OFF},   // PD12=Gruene LED auf dem Discovery-Board
  {LED_ORANGE,GPIOD,GPIO_Pin_13,RCC_AHB1Periph_GPIOD,LED_OFF},   // PD13=Orange LED auf dem Discovery-Board
  {LED_RED   ,GPIOD,GPIO_Pin_14,RCC_AHB1Periph_GPIOD,LED_OFF},   // PD14=Rote LED auf dem Discovery-Board
  {LED_BLUE  ,GPIOD,GPIO_Pin_15,RCC_AHB1Periph_GPIOD,LED_OFF},   // PD15=Blaue LED auf dem Discovery-Board
};

//--------------------------------------------------------------
void UB_Led_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	LED_NAME_t led_name;
  
	for(led_name=0;led_name<LED_ANZ;led_name++)
	{
		RCC_AHB1PeriphClockCmd(LED[led_name].LED_CLK, ENABLE);

		GPIO_InitStructure.GPIO_Pin = LED[led_name].LED_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(LED[led_name].LED_PORT, &GPIO_InitStructure);

		if(LED[led_name].LED_INIT==LED_OFF)
			UB_Led_Off(led_name);
		else
			UB_Led_On(led_name);
	}
}

//--------------------------------------------------------------
void UB_Led_Off(LED_NAME_t led_name)
{
	LED[led_name].LED_PORT->BSRRH = LED[led_name].LED_PIN;
}

//--------------------------------------------------------------
void UB_Led_On(LED_NAME_t led_name)
{
	LED[led_name].LED_PORT->BSRRL = LED[led_name].LED_PIN;
} 

//--------------------------------------------------------------
void UB_Led_Toggle(LED_NAME_t led_name)
{
	LED[led_name].LED_PORT->ODR ^= LED[led_name].LED_PIN;
}

