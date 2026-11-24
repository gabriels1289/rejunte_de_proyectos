#include "massert.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "drvSysTick.h"
#include "drvEnc28j60.h"
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "server.h"

#include "string.h"

//NOTE: MAC address in ENC28J60 is byte-backward !!
const uint8_t g_MAC[6] = {0x54,0x55,0x58,0x10,0x00,0x24};
const uint8_t g_IP[4]  = {192,168,1,25};

int main(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	t_enc28j60Ctrl ethCtrl;
	t_enc28j60Init ethInit;

	if (0 < drvSysTickInit(SYSTICKDIV_TB_1us)) {
		mASSERT(0);
	}

	/* GPIOD Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/* Configure PD12 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	ethInit.fcn_IfaceInit          = drvEnc28j60_init;
	ethInit.fcn_IfaceDeInit        = drvEnc28j60_deInit;
	ethInit.fcn_IfaceReadWriteByte = drvEnc28j60_readWriteByte;
	ethInit.fcn_IfaceCsSelect      = drvEnc28j60_select;
	ethInit.fcn_IfaceCsUnselect    = drvEnc28j60_unselect;
	ethInit.fcn_IfaceTimerUs       = drvSysTickDelay;
	memcpy(ethInit.macAddr, g_MAC, 6);

	enc28j60_init(&ethCtrl, &ethInit);
	init_ip_arp_udp_tcp(&ethCtrl, (uint8_t *)g_MAC, (uint8_t *)g_IP, 80);

	while (1)
	{
		switch (server(&ethCtrl))
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
	}

	return 0;
}

