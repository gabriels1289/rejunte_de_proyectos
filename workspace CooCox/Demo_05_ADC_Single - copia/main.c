//--------------------------------------------------------------
// File     : main.c
// Datum    : 17.02.2013
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.0
// Module   : CMSIS_BOOT, M4_CMSIS_CORE
// Funktion : Demo der ADC-Single-Library
// Hinweis  : Diese zwei Files muessen auf 8MHz stehen
//              "cmsis_boot/stm32f4xx.h"
//              "cmsis_boot/system_stm32f4xx.c"
//--------------------------------------------------------------

#include "main.h"
#include "stm32_ub_adc1_single.h"
#include "stm32_ub_led.h"
#include "arm_math.h"

// Prototipos de funciones
void pinInit(void);

#define FFT_SIZE 1024

float32_t adcSamples[2048];
float32_t Output[FFT_SIZE];

int main(void)
{
  uint16_t adc_wert;
  int i=0;

  arm_cfft_radix4_instance_f32 S;    /* ARM CFFT module */
  float32_t maxValue;                /* Max FFT value is stored here */
  uint32_t maxIndex;                /* Index in Output array where max value is */


  SystemInit(); // Quarz Einstellungen aktivieren

  UB_ADC1_SINGLE_Init(); // Init vom ADC1
  UB_Led_Init();         // Init der LEDs

  pinInit();

  // ADC-Kanal an PA3 messen und 3 LEDs entsprechend schalten
  adc_wert=UB_ADC1_SINGLE_Read_MW(ADC_PA3);
  if(adc_wert>1024) UB_Led_On(LED_GREEN); else UB_Led_Off(LED_GREEN);
  if(adc_wert>2048) UB_Led_On(LED_ORANGE); else UB_Led_Off(LED_ORANGE);
  if(adc_wert>3072) UB_Led_On(LED_RED); else UB_Led_Off(LED_RED);

  // ADC-Kanal an PC4 messen und die Blaue LED entsprechend schalten
  adc_wert=UB_ADC1_SINGLE_Read_MW(ADC_PC4);
  if(adc_wert>2048) UB_Led_On(LED_BLUE); else UB_Led_Off(LED_BLUE);

  while(1)
  {
	  for(i=0; i<2048; i++)
	  {
		  // Messkanal einrichten
		  ADC_RegularChannelConfig(ADC1,ADC_Channel_15,1,ADC_SampleTime_3Cycles);
		  // Messung starten
		  ADC_SoftwareStartConv(ADC1);
		  // warte bis Messung fertig ist
		  while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);
		  // Messwert auslesen

		  adcSamples[i] = (float32_t)((float32_t)ADC_GetConversionValue(ADC1) - (float32_t)1024.0) / (float32_t)1024.0;
		  //adcSamples[i]=ADC_GetConversionValue(ADC1);
		  GPIOB->ODR ^= GPIO_Pin_14;	//Tooglea el GPIOB_14 para indicar dato almacenado del adc
	  }

	  /* Initialize the CFFT/CIFFT module, intFlag = 0, doBitReverse = 1 */
	    arm_cfft_radix4_init_f32(&S, FFT_SIZE, 0, 1);

	    /* Process the data through the CFFT/CIFFT module */
	    arm_cfft_radix4_f32(&S, adcSamples);

	    /* Process the data through the Complex Magniture Module for calculating the magnitude at each bin */
	    arm_cmplx_mag_f32(adcSamples, Output, FFT_SIZE);

	    /* Calculates maxValue and returns corresponding value */
	    arm_max_f32(Output, FFT_SIZE, &maxValue, &maxIndex);

	    UB_Led_Toggle(LED_GREEN);
  }
}

void pinInit(void)
{
	// Configura como salida el GPIOB_14
	GPIO_InitTypeDef  GPIO_InitStructure;

	// Clock Enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	// Config als Digital-Ausgang
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Coloca el pin en "0"
	GPIOB->BSRRH = GPIO_Pin_14;
}
