/*
Title:
	C source code file for EM4095 RFID reader IC

Datasheet:
	http://www.emmicroelectronic.com/webfiles/Product/RFID/DS/EM4095_DS.pdf

Application note:
	http://www.emmicroelectronic.com/webfiles/product/rfid/an/an404.pdf

Compiler:
	avrgcc 3.4.5 with AVR Studio 4.12 Service Pack 3 Build 490

Author:
	Stefan Seegel / dahamm@gmx.net

Date:
	30.08.2006

Version:
	1.0

Free for noncommercial use
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "em4095.h"

#define MANCHESTER_LONG (F_CPU / F_RFID * 64 / TIMER0_PRESCALER)
#define MANCHESTER_SHORT (F_CPU / F_RFID * 64 / TIMER0_PRESCALER / 2)
#define MANCHESTER_MIDDLE (MANCHESTER_LONG + MANCHESTER_SHORT) / 2

static unsigned char data[8];
static volatile uint8_t flag;

//SIGNAL (SIG_INTERRUPT1)
ISR(INT1_vect)
{
	static uint16_t sync;
	static uint8_t bitcount;
	static uint8_t insync = 0;
	static uint8_t old_cnt;
  PORTC |= 1<<PC1;
	if ((uint8_t)(TCNT0 - old_cnt) > MANCHESTER_MIDDLE)
	{
		old_cnt = TCNT0;

		sync <<= 1;
		bitcount++;

		if (bit_is_clear(PIND, PD3))
			sync |= 1;

		if ((sync & 0x3FF) == 0x1FF)
		{
			data[0] = 0xFF;
			bitcount = 1;
			insync = 1;
		}

		if (insync)
		{
			if ((bitcount % 8) == 0)
			{
				data[bitcount / 8] = sync & 0xFF;

				if (bitcount == 56)
				{
					flag = 1;
					insync = 0;
					EIMSK &= ~(1<<INT1);
				}
			}
		}
	}
}

static uint8_t check_parity(uint16_t param)
{
	uint8_t bit;
	uint8_t par = 0;

	for (bit=0; bit<16; bit++)
		par ^= (param >> bit) & 0x01;

	if (par)
		return 0;
	else
		return 1;
}


uint8_t em4095_read_tag(uint8_t *d)
{
	uint8_t timeout = 50;

	do
	{
		if (bit_is_set(TIFR0, TOV0))
		{
			timeout--;
			TIFR0 |= 1<<TOV0; //Clear Overflow Flag
		}

	} while ((!flag) && (timeout > 0));

	if (!flag)
		return 0;

	flag = 0;
	uint8_t temp[8];
	memcpy(temp, data, 8);
	memset(data, 0, 8);
	EIMSK |= 1<<INT1;

	if (((temp[0] << 8 | temp[1]) & 0xFF80) != 0xFF80) //Check startbits
		return 0;

	if ((temp[7] & 0x01)) //Check stopbit
		return 0;


	uint8_t i, p, lp, ibit;
	//calc line parities
	i=0;
	p=0;
	lp=0;
	for (ibit=9; ibit<59; ibit++)
	{
		lp ^= (temp[ibit / 8] >> (7 - (ibit % 8))) & 0x01;

		i++;
		if (i==5)
		{
			i=0;
			p |= lp;
		}
	}

	//calc line parities
	for (i=0; i<4; i++) //Check 4 columns
	{
		lp = 0;
		for (ibit=9+i; ibit<9+55+i; ibit+=5)
			lp ^= (temp[ibit / 8] >> (7 - (ibit % 8))) & 0x01;
		p |= lp;
	}

	if (p)
		return 0;

	d[0] = 0; d[1] = 0; d[2] = 0; d[3] = 0; d[4] = 0;

	//Remove parity bits
	ibit = 9;
	uint8_t obit;
	for (obit=0; obit<40; obit++)
	{
		if ((temp[ibit / 8] >> (7 - (ibit % 8))) & 0x01)
			d[obit / 8] |= 1 << (7 - (obit % 8));

		ibit++;
		if (((obit + 1) % 4) == 0)
			ibit++;
	}

	return 1;
}
