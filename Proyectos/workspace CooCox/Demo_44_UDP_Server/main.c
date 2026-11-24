//--------------------------------------------------------------
// File     : main.c
// Datum    : 31.05.2013
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.0
// Module   : CMSIS_BOOT, M4_CMSIS_CORE
// Funktion : Demo der UPD-Server-Library
// Hinweis  : Diese zwei Files muessen auf 8MHz stehen
//              "cmsis_boot/stm32f4xx.h"
//              "cmsis_boot/system_stm32f4xx.c"
//--------------------------------------------------------------

#include "main.h"
#include "stm32_ub_udp_server.h"
#include "stm32_ub_led.h"

int main(void)
{
  UDP_INIT_STATUS_t init_check;
  UDP_RECEIVE_t rec_check;
  char rx_buf[UDP_RX_BUFFER_SIZE];

  SystemInit(); // Quarz Einstellungen aktivieren

  // LEDs initialisieren
  UB_Led_Init();

  // UDP-Server initialisieren
  init_check=UB_UDP_Server_Init();
  if(init_check==UDP_INIT_OK) {
    // wenn initialisierung ok
    UB_Led_On(LED_GREEN);
    // Server starten
    if(UB_UDP_Server_Connect()==UDP_CONNECT_OK) {
      // wenn der Server läuft
      // einmal einen Text per UDP senden
      UB_UDP_Server_SendString("UDP Server ok");
    }
  }
  else {
    // bei einem Fehler
    UB_Led_On(LED_RED);
  }


  while(1)
  {
    // UDP-Server zyklisch aufrufen
    // und check ob Daten empfangen wurden
    rec_check=UB_UDP_Server_Do(rx_buf);
    if(rec_check==UDP_SERVER_OFFLINE) {
      // UDP-Server ist Offline
      UB_Led_Off(LED_BLUE);
    }
    else {
      // UDP-Server ist Online
      UB_Led_On(LED_BLUE);

      if(rec_check==UDP_RECEIVE_READY) {
        // wenn Daten per UDP empfangen wurde
        UB_Led_Toggle(LED_ORANGE);
        // als Echo zurücksenden
        UB_UDP_Server_SendString(rx_buf);
      }
    }
  }
}

