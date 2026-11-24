#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>	//para el lcd
#include <string.h>
#include "em4095.h"
#include "lcd_lib.h"
#include "main.h"
#include "i2c.h"

#define F_CPU 8000000UL	// 8 MHz
#define dataLen	50	// Tamaño para el buffer de recepción
#define LF 0x0A
#define CR 0x0D

/*
Since the chip is fused to set the clock prescaler to a division of 8 (8MHz/8=1MHz) as factory default,
i simply just change the prescaler back to "1" in code. This is accomplished by this little function:
*/
void clockprescale(void)
{
	CLKPR = 0b10000000; //Prepare the chip for a change of clock prescale (CLKPCE=1 and the rest zeros)
	CLKPR = 0b00000000;	//Wanted clock prescale (CLKPCE=0 and the four first bits CLKPS0-3 sets division factor = 1)
	//See page 38 in datasheet
}

/*
And the USART is initialized by calling this function
*/
void usart_init(void)
{
	//DDRD |= (0<<0)|(1<<1);	//Set TXD as output for USART

	unsigned int USART_BAUDRATE = 9600;
	unsigned int ubrr = (((F_CPU / (USART_BAUDRATE * 16UL))) - 1);	//baud prescale calculated according to F_CPU-define at top

	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;

	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);

	/* Set frame format: 8data, 1stopbit, The two stop-bits does not seem to make any difference in my case!?*/
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

/* To send a byte to the computer, use this function: */
void USART_Transmit(uint8_t data)
{
	/* Wait for empty transmit buffer */
	while(!(UCSR0A & (1<<UDRE0)));
	/*Put data into buffer, sends the data */
	UDR0 = data;
}

/* And to receive a byte from the computer: */
uint8_t USART_Receive(void)
{
	/* Wait for data to be receive */
	while(!(UCSR0A & (1<<RXC0)));	//This loop is only needed if you not use the interrupt...

	/* Get and return received data from buffer */
	return UDR0;	//Return the received byte
}

/*
Instead of constantly calling the USART_Receive-function, I strongly recommend you to setup an interrupt, that automatically triggers when the computer sends something to your chip. The interrupt is initialized like this:
*/
void USART_interrupt_init(void)
{
	UCSR0B |= (1<<RXCIE0);	//Enable interrupt that triggers on USART-data is receive
}

/*
And put the function that gets triggered by the USART receive interrupt at the very bottom of your code:
*/
ISR(USART_RX_vect)	//Vector that triggers when computer sends something to the Atmega88
{
	uint8_t W_buffer[dataLen];	//Creates a buffer to receive data with specified lenght (ex. dataLen = 5 bytes)

	int i;
	for(i=0; i<dataLen; i++)
	{
		W_buffer[i]=USART_Receive();	//receive the USART
		USART_Transmit(W_buffer[i]);	//Transmit the Data back to the computer
		//This probably should wait untill all the bytes is received, but works fine in to send and receive at the same time... =)
		LCDGotoXY(i, 1);
		LCDsendChar(W_buffer[i]);
	}

	//do something like send the data to nRF24L01 with "transmit_payload(W_buffer);"
}

void USART_Flush( void )
{
	unsigned char dummy;
	while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
}


void USART_PutString(uint8_t *cadena)
{
    while(*cadena)
	{
		USART_Transmit(*cadena++);
	}
}

void USART_PutTargeta(tarjeta tar)
{
    USART_Transmit(tar.ID[0]);
    USART_Transmit(tar.ID[1]);
    USART_Transmit(tar.ID[2]);
    USART_Transmit(tar.ID[3]);
    USART_Transmit(tar.ID[4]);
    USART_Transmit(tar.tipo);
    USART_Transmit(tar.duracion);
    USART_Transmit(tar.disponible);
	USART_Transmit(LF);
	USART_Transmit(CR);
}
//Strings stored in AVR Flash memory
const uint8_t LCDwelcomeln1[] PROGMEM="AVR LCD DEMO\0";

//delay 1s
void delay1s(void)
{
	uint8_t i;
	for(i=0;i<100;i++)
	{
		_delay_ms(10);
	}
}

int EscrituraCorrecta(tarjeta uno, tarjeta dos);

uint8_t auxiliar[16];
uint8_t i;
//declaracion de variables externas y globales
volatile unsigned int delayTimer;		// Timer para retardos
volatile tarjeta varTarjeta;			// variable de tipo tarjeta para la lectura y escritura de una sola tarjeta
volatile tarjeta auxvarTarjeta;			//variable de tipo tarjeta para la lectura y escritura de una sola tarjeta
volatile unsigned int dir;			// variable global para manejar la direccion de acceso a la eeprom 24FC512
/* Now here is the main function, that just waits for a USART command from the computer:*/
int main(void)
{
	clockprescale();
	usart_init();
	//USART_interrupt_init();

	// Configuro los pines del puerto C como salida
	DDRC |= HIDROLAVADO+ESPUMA+ASPIRADORA+BUZZER;
	// Los pongo en 0
	PORTC &= ~(HIDROLAVADO+ESPUMA+ASPIRADORA+BUZZER);

	DDRD = 1<<MOD | 1<<SHD; //Mod, Shd

	TCCR0B = 1<<CS01 | 1<<CS00; //Timer/Counter0 prescaler 64
	EICRA |= 1<<ISC10; //Any logical change on INT0 generates an interrupt request
	EIMSK |= 1<<INT1; //External interrupt request 0 enable (demod_out from RFID chip)

    // initialize timer
    timer1_init();

	sei();// Enable the Global Interrupt Enable flag so that interrupts can be processed
	LCDinit();//init LCD bit, dual line, cursor right
	LCDclr();//clears LCD
	CopyStringtoLCD(LCDwelcomeln1, 4, 0);

    uint8_t tag[5];

	volatile uint32_t rebote = 0;

	while(1)
	{
    //waiting for the usart-interrupt to trigger
	if (em4095_read_tag(tag))
		{
		    if(rebote == 0)
            {
                BEEP( _100ms );
                delay( _100ms );
                BEEP( _100ms );
                //printf("\x02R%.2X%.2X%.2X%.2X%.2X\x04", tag[0], tag[1], tag[2], tag[3], tag[4]);
                sprintf(auxiliar,"\x02R%.2X%.2X%.2X%.2X%.2X\x04", tag[0], tag[1], tag[2], tag[3], tag[4]);
                LCDGotoXY(0,0);
                LCDstring("LEYO TARJETA    ",16);
                LCDGotoXY(0,1);
                LCDstring(auxiliar,16);
                for(i=0; i<5; i++)
                {
                    USART_Transmit(tag[i]);
                    tag[i]=0;
                }
                USART_Transmit('\n');
                //USART_Transmit(CR);
                USART_Flush();
                delay(_3seg);
                rebote++;
            }
            else
                rebote = 0;
		}
		else
		{
			LCDGotoXY(0,0);
			LCDstring("PASE TARJETA    ",16);
			LCDGotoXY(0,1);
			LCDstring("                ",16);
		}
	}
	return 0;
}


// initialize timer, interrupt and variable
// Timer programado para que interrumpa cada 10 mseg.
void timer1_init(void)
{
	TCCR1B |= (1<<WGM12);	//Modo CTC

	OCR1A = 0x04E1;			//Valor de comparación

	// set up timer with prescaler = 64
	TCCR1B |= (1 << CS11)|(1 << CS10);

	TIMSK1 |= (1 << OCIE1A);	//Habilita las interrupciones CTC
}

// TIMER1 overflow interrupt service routine
// called whenever OCR1A es alcanzado
ISR(TIMER1_COMPA_vect)
{
        delayTimer++;
        // no timer reset required here as the timer
        // is reset every time it overflows
}

//--------------------------------------------------------
// Lazo de retardo. El tiempo se indica en decenas de ms
//--------------------------------------------------------
void delay( const unsigned int n_x_10ms )
{
	delayTimer = 0;
	while( delayTimer < n_x_10ms );
}

//estas funciones se usan con la libreria i2c.h
//Lee la informacion de una tarjeta de la eeprom y la almacena en una estructura

void ReadTarjetaEeprom(tarjeta *tar, unsigned int dev_address)
{
	char data;
	unsigned i;
	// Read the EEPROM
	for (i=0; i<8; i++)
	{
		i2c_readbyte(dev_address + i,EEPROM_ID,EEPROM_ADDR,&data);
		if(i < 5)
             tar->ID[i]=data;
		else if(i == 5)
            tar->tipo=data;
		else if(i == 6)
            tar->duracion=data;
		else
         	tar->disponible=data;
	}
}

//Escribe la informacion de una tarjeta almacenada en una estructura, en la eeprom
void WriteTarjetaEeprom(tarjeta tar, unsigned int dev_address)
{
	unsigned i;
	// Write the EEPROM
	for (i=0; i<8; i++)
	{
		if(i < 5)
			i2c_writebyte(dev_address + i,EEPROM_ID,EEPROM_ADDR,tar.ID[i]);
		else if(i == 5)
			i2c_writebyte(dev_address + i,EEPROM_ID,EEPROM_ADDR,tar.tipo);
		else if(i == 6)
		i2c_writebyte(dev_address + i,EEPROM_ID,EEPROM_ADDR,tar.duracion);
		else
		i2c_writebyte(dev_address + i,EEPROM_ID,EEPROM_ADDR,tar.disponible);
	}
}

int EscrituraCorrecta(tarjeta uno, tarjeta dos)
{
    if((uno.ID[0] == dos.ID[0]) && (uno.ID[1] == dos.ID[1]) && (uno.ID[2] == dos.ID[2]) && (uno.ID[3] == dos.ID[3]) && (uno.ID[4] == dos.ID[4]) )
        return 1;
    else
        return 0;
}
