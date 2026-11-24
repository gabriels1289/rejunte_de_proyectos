//Libreria FLEURY
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>	//para el lcd
#include <string.h>
#include "lcd_lib.h"
#include "main.h"
#include "i2c.h"
//#include "i2cmaster.h"
#include "varios.h"

#define F_CPU 8000000UL	// 8 MHz
#define dataLen	50	// Tamaño para el buffer de recepción
#define LF 0x0A
#define CR 0x0D

// variables globales y externas
volatile tarjeta varTarjeta;  //variable de tipo tarjeta para la lectura y escritura de una sola tarjeta
int posicion = 0; //contabiliza el orden de los datos de la usart
int contador_tarjetas = 0;  //lleva el numero de tarjetas recibidas
int flag_grabar=0;  //bandera que me indica que tengo una tarjeta para grabar
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
	if(posicion==0)
  {
    varTarjeta.ID[0]=USART_Receive();
    USART_Transmit(varTarjeta.ID[0]);	//Transmit the Data back to the computer
    posicion++;
  }
  else if(posicion==1)
  {
    varTarjeta.ID[1]=USART_Receive();
    USART_Transmit(varTarjeta.ID[1]);	//Transmit the Data back to the computer
    posicion++;
  }
  else if(posicion==2)
  {
    varTarjeta.ID[2]=USART_Receive();
    USART_Transmit(varTarjeta.ID[2]);	//Transmit the Data back to the computer
    posicion++;
  }

  else if(posicion==3)
  {
    varTarjeta.ID[3]=USART_Receive();
    USART_Transmit(varTarjeta.ID[3]);	//Transmit the Data back to the computer
    posicion++;
  }
  else if(posicion==4)
  {
    varTarjeta.ID[4]=USART_Receive();
    USART_Transmit(varTarjeta.ID[4]);	//Transmit the Data back to the computer
    posicion++;
  }
  else if(posicion==5)
  {
    varTarjeta.tipo=USART_Receive();
    USART_Transmit(varTarjeta.tipo);	//Transmit the Data back to the computer
    posicion++;
  }
  else if(posicion==6)
  {
    varTarjeta.duracion=USART_Receive();
    USART_Transmit(varTarjeta.duracion);	//Transmit the Data back to the computer
    posicion++;
  }
  else if(posicion==7)
  {
    varTarjeta.disponible=USART_Receive();
    USART_Transmit(varTarjeta.disponible);	//Transmit the Data back to the computer
    posicion=0;
   // contador_tarjetas++;
    flag_grabar=1;
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
}
//Strings stored in AVR Flash memory
const uint8_t LCDwelcomeln1[] PROGMEM="CARGA EEPROM\0";
const uint8_t VERSION[] PROGMEM="VERSION 1.0\0";

//delay 1s
void delay1s(void)
{
	uint8_t i;
	for(i=0;i<100;i++)
	{
		_delay_ms(10);
	}
}

uint8_t auxiliar[16];
//declaracion de variables externas y globales
volatile unsigned int delayTimer;		// Timer para retardos
volatile tarjeta	auxvarTarjeta;			//variable de tipo tarjeta para la lectura y escritura de una sola tarjeta
volatile unsigned int dir;			// variable global para manejar la direccion de acceso a la eeprom 24FC512
/* Now here is the main function, that just waits for a USART command from the computer:*/

// Prototipos funciones
void EscribirTarjeta(unsigned int direccion, tarjeta tar);
void LeerTarjeta(unsigned int direccion, tarjeta *tar);
int EscrituraCorrecta(tarjeta uno, tarjeta dos);

//#define Dev24FC512  0xA2      // device address of EEPROM 24C512

int main(void)
{
	clockprescale();
	usart_init();
	//i2c_init(); // initialize I2C library
	Init_TWI();
	USART_interrupt_init();

	// Configuro los pines del puerto C como salida
	DDRC |= HIDROLAVADO+ESPUMA+ASPIRADORA+BUZZER;
	// Los pongo en 0
	PORTC &= ~(HIDROLAVADO+ESPUMA+ASPIRADORA+BUZZER);

	dir = 0;
  // initialize timer
  timer1_init();

	sei();// Enable the Global Interrupt Enable flag so that interrupts can be processed
	LCDinit();//init LCD bit, dual line, cursor right
	LCDclr();//clears LCD
	CopyStringtoLCD(LCDwelcomeln1, 0, 0);

//  USART_PutString("INI");
//  USART_Transmit(LF);
//  USART_Transmit(CR);

  varTarjeta.ID[0] = 0;
  varTarjeta.ID[1] = 0;
  varTarjeta.ID[2] = 0;
  varTarjeta.ID[3] = 0;
  varTarjeta.ID[4] = 0;
  varTarjeta.tipo = 0;
  varTarjeta.duracion = 0;
  varTarjeta.disponible = 0;

	while(verificaFIN(varTarjeta) == 0)
	{
    //waiting for the usart-interrupt to trigger
    if(flag_grabar==1)
    {
      sprintf(auxiliar,"\x02R%.2X%.2X%.2X%.2X%.2X\x04", varTarjeta.ID[0],
                                                        varTarjeta.ID[1],
                                                        varTarjeta.ID[2],
                                                        varTarjeta.ID[3],
                                                        varTarjeta.ID[4]);
      LCDGotoXY(0,1);
      LCDstring(auxiliar,16);
      EscribirTarjeta(dir, varTarjeta);
      //WriteTarjetaEeprom(varTarjeta, dir);
      delay(_20ms);
      //USART_PutTargeta(varTarjeta);
      LeerTarjeta(dir, &auxvarTarjeta);
      //LeerTarjetaEEPROM(dir, &auxvarTarjeta);
      //ReadTarjetaEeprom(&auxvarTarjeta, dir);
			//USART_PutTargeta(auxvarTarjeta);
			USART_PutString(" EEPROM: ");
			if(EscrituraCorrecta(varTarjeta, auxvarTarjeta))
      {
        dir+=8;
//        USART_Transmit('A');
//        USART_Transmit('C');
//        USART_Transmit('K');
        USART_PutTargeta(auxvarTarjeta);
        LCDGotoXY(0,0);
        LCDstring("ACK             ",16);
      }
      else
      {
//        USART_Transmit('N');
//        USART_Transmit('A');
//        USART_Transmit('C');
//        USART_Transmit('K');
        USART_PutTargeta(auxvarTarjeta);
        LCDGotoXY(0,0);
        LCDstring("ERROR           ",16);
      }
      flag_grabar=0;
    }
	}
	EscribeFIN(dir);
	LCDGotoXY(0,0);
  LCDstring("FIN   ", 6);
  USART_PutString("FIN");
//  USART_Transmit(LF);
//  USART_Transmit(CR);
	return 0;
}

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


//------------------------------------------------------------------------
//
//			Se trabaja con estas funciones si se define TWISTER
//
//------------------------------------------------------------------------

//Lee la informacion de una tarjeta de la eeprom y la almacena en una estructura
/*
void LeerTarjeta(unsigned int direccion, tarjeta *tar)
{
  unsigned char Hdir = direccion;
  unsigned char Ldir = direccion >> 8;
	//unsigned int aux=dev_address;
	// write 0x75 to eeprom address 0x05 (Byte Write)
	//if(ret = i2c_start(Dev24FC512+I2C_WRITE); )      // set device address and write mode
	//{
	//// failed to issue start condition, possibly no device found
	//i2c_stop();
	//}
	//else{
	// write ok, read value back from eeprom address dev_address (Sequencial Read),
	// wait until the device is no longer busy from the previous write operation
	i2c_start_wait(Dev24FC512+I2C_WRITE);	// set device address and write mode
	i2c_write(Hdir);                 // write address = dev_address
	i2c_write(Ldir);
	i2c_rep_start(Dev24FC512+I2C_READ);     // set device address and read mode
	tar->ID[0] = i2c_readAck();             // read one byte form address dev_address
	tar->ID[1] = i2c_readAck();             //  "    "    "    "     "    dev_address+1
	tar->ID[2] = i2c_readAck();             //  "    "    "    "     "    dev_address+2
	tar->ID[3] = i2c_readNak();             //  "    "    "    "     "    dev_address+3
	tar->ID[4] = i2c_readAck();             //  "    "    "    "     "    dev_address+4
	tar->tipo = i2c_readAck();              //  "    "    "    "     "    dev_address+5
	tar->duracion = i2c_readNak();          //  "    "    "    "     "    dev_address+6
	tar->disponible = i2c_readNak();        //  "    "    "    "     "    dev_address+7
	i2c_stop();								// set stop condition = release bus
	//}
	//return dev_address+8; Ó return aux+8;
}


//Escribe la informacion de una tarjeta almacenada en una estructura, en la eeprom
void EscribirTarjeta(unsigned int direccion, tarjeta tar)
{
  unsigned char Hdir = direccion; //direccion & 0x00ff
  unsigned char Ldir = direccion >> 8; // (direccion >> 8) & 0x00ff
	//unsigned int aux=dev_address;
	// write 0x75 to eeprom address 0x05 (Byte Write)
	//if(ret = i2c_start(Dev24FC512+I2C_WRITE); )      // set device address and write mode
	//{
	//// failed to issue start condition, possibly no device found
	//i2c_stop();
	//}
	//else{
	// write ok, read value back from eeprom address dev_address (Sequencial Read),
	// wait until the device is no longer busy from the previous write operation
	i2c_start_wait(Dev24FC512+I2C_WRITE);	// set device address and write mode
	i2c_write(Hdir);                 // write address = dev_address
	i2c_write(Ldir);
	i2c_write(tar.ID[0]);					// write data to address dev_address
	i2c_write(tar.ID[1]);					//    "    "   "    "    dev_address+1
	i2c_write(tar.ID[2]);					//    "    "   "    "    dev_address+2
	i2c_write(tar.ID[3]);					//    "    "   "    "    dev_address+3
	i2c_write(tar.ID[4]);					//    "    "   "    "    dev_address+4
	i2c_write(tar.tipo);					//    "    "   "    "    dev_address+5
	i2c_write(tar.duracion);				//    "    "   "    "    dev_address+6
	i2c_write(tar.disponible);				//    "    "   "    "    dev_address+7
	i2c_stop();								// set stop condition = release bus
	//}
	//return dev_address+8; Ó return aux+8;
}
*/

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

int EscrituraCorrecta(tarjeta uno, tarjeta dos)
{
    if((uno.ID[0] == dos.ID[0]) && (uno.ID[1] == dos.ID[1]) && (uno.ID[2] == dos.ID[2]) && (uno.ID[3] == dos.ID[3]) && (uno.ID[4] == dos.ID[4]) )
        return 1;
    else
        return 0;
}

int verificaFIN(tarjeta tar)
{
  if( (tar.ID[0] == 'T') &&
      (tar.ID[1] == 'A') &&
      (tar.ID[2] == 'R') &&
      (tar.ID[3] == 'J') &&
      (tar.ID[4] == 'E') &&
      (tar.tipo == 'T') &&
      (tar.duracion == 'A') &&
      (tar.disponible == 'S'))
  return 1;
  else
    return 0;
}

void EscribeFIN(unsigned int direccion)
{
  i2c_writebyte(direccion,EEPROM_ID,EEPROM_ADDR,'T');
  i2c_writebyte(direccion + 1,EEPROM_ID,EEPROM_ADDR,'A');
  i2c_writebyte(direccion + 2,EEPROM_ID,EEPROM_ADDR,'R');
  i2c_writebyte(direccion + 3,EEPROM_ID,EEPROM_ADDR,'J');
  i2c_writebyte(direccion + 4,EEPROM_ID,EEPROM_ADDR,'E');
  i2c_writebyte(direccion + 5,EEPROM_ID,EEPROM_ADDR,'T');
  i2c_writebyte(direccion + 6,EEPROM_ID,EEPROM_ADDR,'A');
  i2c_writebyte(direccion + 7,EEPROM_ID,EEPROM_ADDR,'S');
}

/*
void EscribeFIN(unsigned int direccion)
{
  unsigned char Hdir = direccion;
  unsigned char Ldir = direccion >> 8;
	//unsigned int aux=dev_address;
	// write 0x75 to eeprom address 0x05 (Byte Write)
	//if(ret = i2c_start(Dev24FC512+I2C_WRITE); )      // set device address and write mode
	//{
	//// failed to issue start condition, possibly no device found
	//i2c_stop();
	//}
	//else{
	// write ok, read value back from eeprom address dev_address (Sequencial Read),
	// wait until the device is no longer busy from the previous write operation
	i2c_start_wait(Dev24FC512+I2C_WRITE);	// set device address and write mode
	i2c_write(Hdir);                 // write address = dev_address
	i2c_write(Ldir);
	i2c_write('T');					// write data to address dev_address
	i2c_write('A');					//    "    "   "    "    dev_address+1
	i2c_write('R');					//    "    "   "    "    dev_address+2
	i2c_write('J');					//    "    "   "    "    dev_address+3
	i2c_write('E');					//    "    "   "    "    dev_address+4
	i2c_write('T');					//    "    "   "    "    dev_address+5
	i2c_write('A');				//    "    "   "    "    dev_address+6
	i2c_write('S');				//    "    "   "    "    dev_address+7
	i2c_stop();								// set stop condition = release bus
	//}
	//return dev_address+8; Ó return aux+8;
}*/
