#include "massert.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "drvEnc28j60.h"
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "server.h"
#include "TFTssd1963.h"
#include "stm32_ub_led.h"
//#include "stm32f4xx.h"
#include "net.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>

int reiniciarENC;
static void TFT_show(int op);

#define MiPuertoUDP 1200	//Puerto UDP de la STM32F4 para el refrezco de imagenes
#define SERVERUDPPORT 1500	//Puerto UDP del servidor

//#define BUFFER_SIZE 1400	//Defino el tamaño del buffer

//NOTE: MAC address in ENC28J60 is byte-backward !!
const uint8_t g_MAC[6] = {0x54,0x55,0x58,0x10,0x00,0x24}; //Direccion MAC de la STM32F4
const uint8_t g_IP[4]  = {192,168,2,25};	//Direccion IP de la STM32F4
uint8_t server_IP[4]  = {192,168,2,107};	//Direccion IP del servidor

//const char msgKeepAlive[] __attribute__((section(".ARM.__at_0x08004000"))) = "LIVE";	//Mensaje para el keep alive del servidor
const char msgKeepAlive[] = "LIVE";	//Mensaje para el keep alive del servidor

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
	//static uint8_t miBuffer[BUFFER_SIZE+1];
	static uint8_t miBuffer[256];
	t_enc28j60Ctrl ethCtrl;
	t_enc28j60Init ethInit;

	reiniciarENC = 1;

	int muestroPagina=0;

	SystemInit();
	UB_Led_Init();
	TFT_init_pins();	//inicializacion de los pines de la TFT
	UB_Led_On(LED_ORANGE);
	SysTick_Setup();
	UB_Led_Off(LED_ORANGE);
	Delay_ms (500);

	ethInit.fcn_IfaceInit          = drvEnc28j60_init;
	ethInit.fcn_IfaceDeInit        = drvEnc28j60_deInit;
	ethInit.fcn_IfaceReadWriteByte = drvEnc28j60_readWriteByte;
	ethInit.fcn_IfaceCsSelect      = drvEnc28j60_select;
	ethInit.fcn_IfaceCsUnselect    = drvEnc28j60_unselect;
	ethInit.fcn_IfaceTimerUs       = drvSysTickDelay;
	memcpy(ethInit.macAddr, g_MAC, 6);

	UB_Led_On(LED_RED);

	enc28j60_init(&ethCtrl, &ethInit);
	init_ip_arp_udp_tcp(&ethCtrl, (uint8_t *)g_MAC, (uint8_t *)g_IP, 80);

	UB_Led_Off(LED_RED);

	TFT_initMio();	//inicializo la TFT
	//TFT_Init();
	UB_Led_On(LED_BLUE);
	//pinto de 3 colores distintos cada pagina
	TFT_WindowSet(0,479,0,271);
	TFT_WriteArea(RED, 130560); //480*272
	TFT_WindowSet(0,479,272,543);
	TFT_WriteArea(GREEN, 130560); //480*272
	TFT_WindowSet(0,479,544,815);
	TFT_WriteArea(BLUE, 130560); //480*272
	UB_Led_Off(LED_BLUE);

	//TFT_Write_Command(SET_SCROLL_START);
	//TFT_Write_Data(272>>8);
	//TFT_Write_Data(272&0xff);

	client_arp_whohas(miBuffer, server_IP);
	send_udp(miBuffer, msgKeepAlive, strlen(msgKeepAlive), MiPuertoUDP, server_IP, SERVERUDPPORT);

	UdpCntTime = 0;	//Reinicio la variable de conteo para el envio del Keep Alive
	//CntTimerImagen = 0; //Reinicio el contador para el cambio de imagen
	//TFT_printLine(127, PURPLE);

	while(1)
	{
		switch(server(&ethCtrl))
		{
			default:
			case eServerIdle:
				break;
			case eServerLedOn:
				GPIO_SetBits(GPIOD, GPIO_Pin_12);
				break;
			case eServerLedOff:
				GPIO_ResetBits(GPIOD, GPIO_Pin_12);
				break;
		}
		if (UdpCntTime >= 10000000)//cada 10 segundos envio un paquete udp
		{
			UdpCntTime = 0;
			client_arp_whohas(miBuffer, server_IP);
			send_udp(miBuffer, msgKeepAlive, strlen(msgKeepAlive), MiPuertoUDP, server_IP, SERVERUDPPORT);
			//mostrarPagina = (mostrarPagina+1)%3;
			//TFT_show(mostrarPagina);
		}
		if (reiniciarENC == 0)//cada 10 segundos
		{
			reiniciarENC = 1;
			//TFT_save(2);
			enc28j60_init(&ethCtrl, &ethInit);
			init_ip_arp_udp_tcp(&ethCtrl, (uint8_t *)g_MAC, (uint8_t *)g_IP, 80);

		}
		/*if(CntTimerImagen >= _5min) //90000000 minuto y medio
		{
			CntTimerImagen = 0;
			muestroPagina = (muestroPagina+1)%3;
			TFT_show(muestroPagina);
		}*/
	}

	return 0;
}
//---------------------------------------------
// Para mostrar una pantalla u otra
//---------------------------------------------
static void TFT_show(int op)
{
	//UB_Led_Toggle(LED_GREEN);
	switch(op)
	{
	case 0:
		// muestra la primer pantalla (0-271)
		TFT_Write_Command(SET_SCROLL_START);
		TFT_Write_Data(0>>8);
		TFT_Write_Data(0&0xff);
		break;
	case 1:
		// muestra la segunda pantalla (272-543)
		TFT_Write_Command(SET_SCROLL_START);
		TFT_Write_Data(272>>8);
		TFT_Write_Data(272&0xff);
		break;
	case 2:
		// muestra la segunda pantalla (544-815)
		TFT_Write_Command(SET_SCROLL_START);
		TFT_Write_Data(544>>8);
		TFT_Write_Data(544&0xff);
		break;
	}
}

#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif
