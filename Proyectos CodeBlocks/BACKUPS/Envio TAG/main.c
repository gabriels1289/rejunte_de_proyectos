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

//Since the chip is fused to set the clock prescaler to a division of 8 (8MHz/8=1MHz) as factory default,
//i simply just change the prescaler back to "1" in code. This is accomplished by this little function:
void clockprescale(void)
{
	CLKPR = 0b10000000; //Prepare the chip for a change of clock prescale (CLKPCE=1 and the rest zeros)
	CLKPR = 0b00000000;	//Wanted clock prescale (CLKPCE=0 and the four first bits CLKPS0-3 sets division factor = 1)
	//See page 38 in datasheet
}

//declaracion de variables globales
volatile tarjeta varTarjeta;			// variable de tipo tarjeta para la lectura y escritura de una sola tarjeta
volatile tarjeta auxvarTarjeta;			//variable de tipo tarjeta para la lectura y escritura de una sola tarjeta
volatile unsigned int delayTimer;    // Timer para retardos
volatile unsigned int dir;			// variable global para manejar la direccion de acceso a la eeprom 24FC512
uint8_t  tag[5];
uint8_t rebote = 0;
uint8_t auxiliar[16];
uint8_t i;
Flags flags;
int posicion = 0;                   // contabiliza el orden de los datos de la usart

int main(void)
{
	clockprescale();
  usart_init();
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
	CopyStringtoLCD(LCDversion, 0, 0);
	USART_PutString("VERSION: 1.0");
	delay(_2seg);

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
                sprintf(auxiliar,"\x02R%.2X%.2X%.2X%.2X%.2X\x04", tag[0], tag[1], tag[2], tag[3], tag[4]);
                LCD_puts(0, "LEYO TARJETA");
                LCD_puts(1, auxiliar);
                delay(_2seg);
                rebote++;
            }
            else
                rebote = 0;
		}
		else
		{
		  LCDclr();
			LCD_puts(0, "PASE TARJETA");
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

// Funciones asociadas a las tarjetas
// Escribe el contenido de una tarjeta en la direccion pasada como parametro
void EscribirTarjeta(unsigned int direccion, tarjeta tar)
{
  i2c_writebyte(direccion,EEPROM_ID,EEPROM_ADDR,tar.ID[0]);
  i2c_writebyte(direccion + 1,EEPROM_ID,EEPROM_ADDR,tar.ID[1]);
  i2c_writebyte(direccion + 2,EEPROM_ID,EEPROM_ADDR,tar.ID[2]);
  i2c_writebyte(direccion + 3,EEPROM_ID,EEPROM_ADDR,tar.ID[3]);
  i2c_writebyte(direccion + 4,EEPROM_ID,EEPROM_ADDR,tar.ID[4]);
  i2c_writebyte(direccion + 5,EEPROM_ID,EEPROM_ADDR,tar.tipo);
  i2c_writebyte(direccion + 6,EEPROM_ID,EEPROM_ADDR,tar.duracion);
  i2c_writebyte(direccion + 7,EEPROM_ID,EEPROM_ADDR,tar.disponible);
}

// Lee el contenido de una tarjeta en la direccion pasada como parametro
void LeerTarjeta(unsigned int direccion, tarjeta *tar)
{
  i2c_readbyte(direccion,EEPROM_ID,EEPROM_ADDR,&tar->ID[0]);
  i2c_readbyte(direccion + 1,EEPROM_ID,EEPROM_ADDR,&tar->ID[1]);
  i2c_readbyte(direccion + 2,EEPROM_ID,EEPROM_ADDR,&tar->ID[2]);
  i2c_readbyte(direccion + 3,EEPROM_ID,EEPROM_ADDR,&tar->ID[3]);
  i2c_readbyte(direccion + 4,EEPROM_ID,EEPROM_ADDR,&tar->ID[4]);
  i2c_readbyte(direccion + 5,EEPROM_ID,EEPROM_ADDR,&tar->tipo);
  i2c_readbyte(direccion + 6,EEPROM_ID,EEPROM_ADDR,&tar->duracion);
  i2c_readbyte(direccion + 7,EEPROM_ID,EEPROM_ADDR,&tar->disponible);
}

// Verifica si el tag de 2 tarjetas es igual
int ComparaID(tarjeta uno, tarjeta dos)
{
    if((uno.ID[0] == dos.ID[0]) && (uno.ID[1] == dos.ID[1]) && (uno.ID[2] == dos.ID[2]) && (uno.ID[3] == dos.ID[3]) && (uno.ID[4] == dos.ID[4]) )
        return 1;
    else
        return 0;
}

//And the USART is initialized by calling this function
void usart_init(void)
{
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

// To send a byte to the computer, use this function:
void USART_Transmit(uint8_t data)
{
  /* Wait for empty transmit buffer */
  while(!(UCSR0A & (1<<UDRE0)));
  /*Put data into buffer, sends the data */
  UDR0 = data;
}

// And to receive a byte from the computer:
uint8_t USART_Receive(void)
{
  /* Wait for data to be receive */
  while(!(UCSR0A & (1<<RXC0)));	//This loop is only needed if you not use the interrupt...

  /* Get and return received data from buffer */
  return UDR0;	//Return the received byte
}

//Instead of constantly calling the USART_Receive-function, I strongly recommend you to setup an interrupt, that automatically triggers when the computer sends something to your chip. The interrupt is initialized like this:
void USART_interrupt_init(void)
{
  UCSR0B |= (1<<RXCIE0);	//Enable interrupt that triggers on USART-data is receive
}

void USART_interrupt_disable(void)
{
  UCSR0B &= ~(1<<RXCIE0);	//Enable interrupt that triggers on USART-data is receive
}

//And put the function that gets triggered by the USART receive interrupt at the very bottom of your code:
ISR(USART_RX_vect)	//Vector that triggers when computer sends something to the Atmega88
{
  if(posicion==0)
  {
    varTarjeta.ID[0]=USART_Receive();
     posicion++;
  }
  else if(posicion==1)
  {
    varTarjeta.ID[1]=USART_Receive();
    posicion++;
  }
  else if(posicion==2)
  {
    varTarjeta.ID[2]=USART_Receive();
    posicion++;
  }

  else if(posicion==3)
  {
    varTarjeta.ID[3]=USART_Receive();
    posicion++;
  }
  else if(posicion==4)
  {
    varTarjeta.ID[4]=USART_Receive();
    posicion++;
  }
  else if(posicion==5)
  {
    varTarjeta.tipo=USART_Receive();
    posicion++;
  }
  else if(posicion==6)
  {
    varTarjeta.duracion=USART_Receive();
    posicion++;
  }
  else if(posicion==7)
  {
    varTarjeta.disponible=USART_Receive();
    posicion=0;
    flags.listaParaGrabar = 1;
  }
}

void USART_Flush(void)
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
  USART_Flush();
}
