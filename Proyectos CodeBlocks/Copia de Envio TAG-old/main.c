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
volatile unsigned int cantidadTarjetas;	// lleva la cuenta de las tarjetas almacenadas en EEPROM (cuando alcanza los 65536 datos, memoria eeprom 24FC512 completa,
volatile int clock_milisegundos;      // Para el manejo del countdown (en realidad son decenas de milisegundos)
volatile int clock_segundos;          // Para el manejo del countdown
volatile int clock_minutos;           // Para el manejo del countdown
volatile int clock_descartarTarjetas; // Cuando se vence este contador se sale de la espera del deposito de la tarjeta
volatile uint8_t  tag[5];
uint8_t rebote = 0;
uint8_t auxiliar[16];
uint8_t i;
Flags flags;
int posicion = 0;                   // contabiliza el orden de los datos de la usart

int main(void)
{
	clockprescale();
  usart_init();
	initPuertos();
	Init_TWI();
	int0_enter_init();

  // Configura el RFID
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

	// Obtiene la cantidad de tarjetas en la memoria EEPROM
	cantidadTarjetas = 0;
  do{
    LeerTarjeta(dir, &auxvarTarjeta);
    dir+=8;
    cantidadTarjetas++;
    }while(verificaFIN(auxvarTarjeta)==0 && dir<4000); // Mientras no encuentre el final
  if(dir >= 4000)
  {
    LCDclr();
    LCD_puts(0, "NO HAY TARJETAS");
    LCD_puts(1, "  SIN SERVICIO");
    while(1); // No lo dejo hacer nada, ya que no se validaron las tarjetas en la EEPROM
  }
  else{
    LCDclr();
    cantidadTarjetas-=1;
    memset(auxiliar, ' ', 16);
    sprintf(auxiliar,"N. TARJETAS: %u",cantidadTarjetas);
    LCD_puts(0, auxiliar);
    delay(_2seg);
    LCDclr();
    LCD_puts(0,"  \"BIENVENIDO\"");
    LCD_puts(1, "  PASE TARJETA");
  }

	while(1)
	{
    //waiting for the usart-interrupt to trigger
    if (em4095_read_tag(tag))
		{
		    if(rebote == 0)
        {
          BEEP( _100ms );
          sprintf(auxiliar,"\x02R%.2X%.2X%.2X%.2X%.2X\x04", tag[0], tag[1], tag[2], tag[3], tag[4]);
          LCD_puts(0, "LEYO TARJETA");  //Comentar esta linea en la version final
          LCD_puts(1, auxiliar);        //Comentar esta linea en la version final
          USART_Transmit(auxiliar);
          delay(_2seg);

          varTarjeta.ID[0]=tag[0];
          varTarjeta.ID[1]=tag[1];
          varTarjeta.ID[2]=tag[2];
          varTarjeta.ID[3]=tag[3];
          varTarjeta.ID[4]=tag[4];
          varTarjeta.tipo = 0;
          varTarjeta.duracion =0;
          varTarjeta.disponible = 0;
          flags.tarjetaDepositada = 0;

          flags.salirBucle = 0;
          dir = 0;
          flags.tarjetaHallada = 0;
          unsigned int i = 0;
          for(i=0; (i<cantidadTarjetas) && (flags.salirBucle == 0); i++)
          {
            LeerTarjeta(dir, &auxvarTarjeta);

            if(ComparaID(varTarjeta, auxvarTarjeta)==1)
            {
              BEEP( _100ms );
              delay( _100ms );
              BEEP( _100ms );

              flags.salirBucle = 1;
              flags.tarjetaHallada = 1;
              if(auxvarTarjeta.disponible == 1) // la tarjeta no esta disponible
              {
                LCD_puts(0, " NO DISPONIBLE");
                BEEP( _100ms );
                delay( _100ms );
                BEEP( _100ms );
                delay( _100ms );
                BEEP( _100ms );
                delay( _100ms );
                BEEP( _100ms );
                delay( _100ms );
                BEEP( _100ms );
              }
              else//(auxvarTarjeta.disponible == 0x00) //si la tarjeta esta disponible
              {
                LCDclr();
                flags.actualizarHora = 0;
                switch(auxvarTarjeta.tipo)
                {
                  case T_MASTER:
                    LCD_puts(0,"     MASTER");
                    HabilitarTarjetas();
                    LCD_puts(0,"TAR. HABILITADAS");
                    //mostrarContadores();
                    //delay(_3seg);
                    //historicoMaster++;
                    //timeDiarioAspiradora = 0;
                    //timeDiarioEspuma = 0;
                    //timeDiarioHidrolavadora = 0;
                    //diarioAspiradora = 0;
                    //diarioEspuma = 0;
                    //diarioHidrolavadora = 0;
                    //salvarContadores();
                    //cargarContadores();
                    break;
                  case T_HIDRO:
                    LCD_puts(0,"DEPOSITE TARJETA");
                    clock_descartarTarjetas = 0;
                    while( (flags.tarjetaDepositada == 0) && (clock_descartarTarjetas < _10seg) )
                    {
                      progress();
                    };  // me quedo a la espera que depositen la tarjeta
                    if(flags.tarjetaDepositada == 1)  // se vencio el tiempo de espera asique salgo del case
                    {
                      auxvarTarjeta.disponible = 1;
                      EscribirTarjeta(dir, auxvarTarjeta);  // actualiza en la EEPROM que la tarjeta ya fue usada
                      LCD_puts(" HIDROLAVADORA");
                      // actualizo los contadores
                      //historicoHidrolavadora++;
                      //diarioHidrolavadora++;
                      //timeHistoricoHidrolavadora += auxvarTarjeta.duracion;
                      //timeDiarioHidrolavadora += auxvarTarjeta.duracion;

                      HIDRO_ON();
                      tiempoRestante();
                      HIDRO_OFF();
                      // salvo los contadores
                      //salvarContadores();
                      //Muestro mensaje de despedida
                      LCDclr();
                      LCD_puts(0,"  GRACIAS POR");
                      LCD_puts(1,"   ELEGIRNOS");
                      delay(_5seg);
                    }
                    break;
              case T_ESPUMA:
                    LCD_puts(0,"DEPOSITE TARJETA");
                    clock_descartarTarjetas = 0;
                    while( (flags.tarjetaDepositada == 0) && (clock_descartarTarjetas < _10seg) )
                    {
                      progress();
                    };  // me quedo a la espera que depositen la tarjeta
                    if(flags.tarjetaDepositada == 1)  // se vencio el tiempo de espera asique salgo del case
                    {
                      auxvarTarjeta.disponible = 1;
                      EscribirTarjeta(dir, auxvarTarjeta);  // actualiza en la EEPROM que la tarjeta ya fue usada
                      LCD_puts(0,"     ESPUMA");
                      // actualizo los contadores
                      //historicoEspuma++;
                      //diarioEspuma++;
                      //timeHistoricoEspuma += auxvarTarjeta.duracion;
                      //timeDiarioEspuma += auxvarTarjeta.duracion;

                      ESPUMA_ON();
                      tiempoRestante();
                      ESPUMA_OFF();
                      // salvo los contadores
                      //salvarContadores();
                      LCDclr();
                      LCD_puts(0,"  GRACIAS POR");
                      LCD_puts(1,"   ELEGIRNOS");
                      delay(_5seg);
                    }
                    break;
              case T_ASPIRA:
                    LCD_puts(0,"DEPOSITE TARJETA");
                    clock_descartarTarjetas = 0;
                    while( (flags.tarjetaDepositada == 0) && (clock_descartarTarjetas < _10seg) )
                    {
                      progress();
                    };  // me quedo a la espera que depositen la tarjeta
                    if(flags.tarjetaDepositada == 1)  // se vencio el tiempo de espera asique salgo del case
                    {
                      auxvarTarjeta.disponible = 1;
                      EscribirTarjeta(dir, auxvarTarjeta);  // actualiza en la EEPROM que la tarjeta ya fue usada
                      LCD_puts(0,"   ASPIRADORA");
                      // actualizo los contadores
                      //historicoAspiradora++;
                      //diarioAspiradora++;
                      //timeHistoricoAspiradora += auxvarTarjeta.duracion;
                      //timeDiarioAspiradora += auxvarTarjeta.duracion;

                      ASPIRADORA_ON();
                      tiempoRestante();
                      ASPIRADORA_OFF();
                      // salvo los contadores
                      //salvarContadores();
                      LCDclr();
                      LCD_puts(0,"  GRACIAS POR");
                      LCD_puts(1,"   ELEGIRNOS");
                      delay(_5seg);
                    }
                    break;
              default:
                  break;
              }
            }
          }
          else
          {
            dir+=8;
          }
        }

          if(flags.tarjetaHallada == 0)
          {
            LCDclr();
            LCD_puts(0,"   NO HALLADA");
            delay(_2seg);
          }

          rebote++;
          LCDclr();
          LCD_puts(0,"  \"BIENVENIDO\"");
          LCD_puts(1, "  PASE TARJETA");
        }
        else
          rebote = 0;
		}
	}
	return 0;
}

//-------------------------------------
// inicializa todas las variables
//-------------------------------------
void initVariables(void)
{
	// Cargo en los arreglos de punteros los mensajes del menu para el lcd
	//Menu principal
	/*pMenuPpal[0]=&LCDcargar;	  //Cargar tarjeta
	pMenuPpal[1]=&LCDleer;		  //Leer memoria
	pMenuPpal[2]=&LCDescribir;	//Escribir memoria
	pMenuPpal[3]=&LCDcontadores;//Mostrar contadores
	pMenuPpal[4]=&LCDsalir;		  //Salir del menu
	//submenu tipo de tarjeta
	pSubmenuTipo[0]=&LCDmaster;	//Tarjeta maestra
	pSubmenuTipo[1]=&LCDhidro;	//Tarjeta hidrolavado
	pSubmenuTipo[2]=&LCDespuma;	//Tarjeta espuma
	pSubmenuTipo[3]=&LCDaspiradora;	//Tarjeta aspiradora
	pSubmenuTipo[4]=&LCDsalir;  //Salir del menu

	pMenuCancel[0]=&LCDcancelar;  //cancelar
	pMenuCancel[1]=&LCDcontinuar; //continuar
*/
	//selMenu = 0;
//	portbhistory = 0x00;
	//estadoMenu = 0;
	//contTipo = 0;
	//contDuracion = 0;
	cantidadTarjetas = 0;
	posicion = 0;
	//cancelCont = 0;
	dir = 0;

	auxvarTarjeta.ID[0] = 0;
	auxvarTarjeta.ID[1] = 0;
	auxvarTarjeta.ID[2] = 0;
	auxvarTarjeta.ID[3] = 0;
	auxvarTarjeta.ID[4] = 0;
	auxvarTarjeta.tipo = 0;
	auxvarTarjeta.duracion = 0;
	auxvarTarjeta.disponible = 0;

	varTarjeta.ID[0] = 0;
	varTarjeta.ID[1] = 0;
	varTarjeta.ID[2] = 0;
	varTarjeta.ID[3] = 0;
	varTarjeta.ID[4] = 0;
	varTarjeta.tipo = 0;
	varTarjeta.duracion = 0;
	varTarjeta.disponible = 0;

	// inicializo los contadores
	/*historicoMaster = 0;
  historicoHidrolavadora = 0;
  historicoAspiradora = 0;
  historicoEspuma = 0;
  diarioHidrolavadora = 0;
  diarioAspiradora = 0;
  diarioEspuma = 0;
  timeDiarioHidrolavadora = 0;
  timeDiarioAspiradora = 0;
  timeDiarioEspuma = 0;
  timeHistoricoHidrolavadora = 0;
  timeHistoricoAspiradora = 0;
  timeHistoricoEspuma = 0;
  clock_descartarTarjetas = 0;*/
}

// initialize timer, interrupt and variable
void int0_enter_init(void)
{
	EICRA |= (1 << ISC01) | (1 << ISC00); // That is, the rising edge of INT0 generate an interrupt request.
	EIMSK |= (1 << INT0); // Enables INT0 interrupts.
}

ISR(INT0_vect)
{
  flags.aceptarProgramacion = 1;	//el boton fue presionado entro al menu
	flags.tarjetaDepositada = 1;  // depositaron la tarjeta
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
  // no timer reset required here as the timer
  // is reset every time it overflows
  delayTimer++;
  clock_descartarTarjetas++;
  clock_milisegundos++;
  if(clock_milisegundos == _1seg) // 100 cuentas por 10mseg cada una
  {
    flags.actualizarHora = 1;
    clock_segundos--;
    clock_milisegundos = 0;
    if(clock_segundos == -1)
    {
      clock_minutos--;
      clock_segundos = 59;
    }
  }
}

//--------------------------------------------------------
// Lazo de retardo. El tiempo se indica en decenas de ms
//--------------------------------------------------------
void delay( const unsigned int n_x_10ms )
{
	delayTimer = 0;
	while( delayTimer < n_x_10ms );
}

// Inicializa los puertos
void initPuertos(void)
{
  // Configuro los pines del puerto C como salida
	DDRC |= HIDROLAVADO+ESPUMA+ASPIRADORA+BUZZER;
	// Los pongo en 0
	PORTC &= ~(HIDROLAVADO+ESPUMA+ASPIRADORA+BUZZER);

  // Configuro los pines del puerto D como entrada
	DDRD |= BOTON_ENTER+BOTON_BAJAR+BOTON_SUBIR;
	// Los pongo en 0
	PORTD &= ~(BOTON_ENTER+BOTON_BAJAR+BOTON_SUBIR);

	DDRD = 1<<MOD | 1<<SHD; //Mod, Shd
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

//pone disponible a todas las tajetas
void HabilitarTarjetas(void)
{
	unsigned int i,j;
	for (i=7, j=0; j<cantidadTarjetas; j++)
	{
		i2c_writebyte(i,EEPROM_ID,EEPROM_ADDR, 0x00);
		i+=8;
		_delay_ms(2);
	}
}

void tiempoRestante(void)
{
  clock_minutos = auxvarTarjeta.duracion;
  clock_segundos = 0;
  clock_milisegundos = 0;
  while((clock_minutos >= 0) && (clock_segundos >= 0) && (clock_milisegundos <= 99))
  {
    if(flags.actualizarHora == 1)
    {
      memset(auxiliar, ' ', 16);
      sprintf(auxiliar, " Restan: %02d:%02d",clock_minutos,clock_segundos);
      LCD_puts(1, auxiliar);
      flags.actualizarHora = 0;
    }
  }
}

void progress(void)
{
  uint8_t i;
  for(i=0;i<255;i++)
		{
			//_delay_ms(1);
			LCDGotoXY(0, 1);
			LCDprogressBar(i, 255, 16);
		}
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
