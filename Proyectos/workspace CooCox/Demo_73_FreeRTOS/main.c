//--------------------------------------------------------------
// File     : main.c
// Datum    : 11.01.2013
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.4
// GCC      : 4.7 2012q4
// Module   : CMSIS_BOOT, M4_CMSIS_CORE
// Funktion : Demo der FreeRTOS-Library
// Hinweis  : Diese zwei Files muessen auf 8MHz stehen
//              "cmsis_boot/stm32f4xx.h"
//              "cmsis_boot/system_stm32f4xx.c"
//--------------------------------------------------------------

#include "main.h"
#include "stm32_ub_freertos.h"
#include "stm32_ub_led.h"


//--------------------------------------------------------------
// Prototypen der benutzten FreeRTOS-Tasks
//--------------------------------------------------------------
void BlinkyTask( void *pvParameters );



//--------------------------------------------------------------
// init der komplette Hardare für das Beispiel
//--------------------------------------------------------------
void hw_init(void)
{
  UB_Led_Init();
}


//--------------------------------------------------------------
int main(void)
{
  SystemInit(); // Quarz Einstellungen aktivieren

  // init der Hardware
  hw_init();

  // einen einzelnen Task erzeugen "Blinky"
  xTaskCreate( BlinkyTask, ( signed char * ) "Blinky", configMINIMAL_STACK_SIZE, NULL, 1, NULL );

  // FreeRTOS starten
  vTaskStartScheduler();

  // hier kommt die Software nicht hin,
  // wenn FreeRTOS richtig funktioniert
  UB_Led_On(LED_RED);

  while(1)
  {

  }
}


//--------------------------------------------------------------
// FreeRTOS-Task "Blinky"
//--------------------------------------------------------------
void BlinkyTask( void *pvParameters )
{
  while(1)
  {
    // grüne LED toggeln
    UB_Led_Toggle(LED_GREEN);
    // Task für 500ms auf "Blocked"
    vTaskDelay(500/portTICK_RATE_MS);
  }
}
