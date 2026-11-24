/*
Title:
	C header file for EM4095 RFID reader IC

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

#define F_RFID 125000UL	//RFID carrier frequency
#define TIMER0_PRESCALER 64 //Needed for manchester decoding

extern uint8_t em4095_read_tag(uint8_t *d);
