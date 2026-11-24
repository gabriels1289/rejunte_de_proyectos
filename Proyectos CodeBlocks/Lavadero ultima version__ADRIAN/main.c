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
Flags flags = {0,0,0,0,0,0,0,0,0,0,0,0};         // Estructura con los flags del programa;
int posicion = 0;                   // contabiliza el orden de los datos de la usart
uint8_t portbhistory;			  // default is low (0x00) because the pull-down
uint8_t selMenu;             // indice para llevar el menu actual
uint8_t cancelCont;          // indice que indica si se cancela o continua con la desicion de borrar la EEPROM
uint8_t contTipo;				    // lleva el registro del submenu con el tipo de tarjeta
volatile uint8_t contDuracion;			  // lleva la cantidad de minutos de la tarjeta
uint8_t estadoMenu;			    // indica el tipo de menu en el que estoy
// Contadores de tarjetas utilizadas
// El uso de los contadores diarios hace referencia al conteo efectuado entre tarjetas Master
// Los contadores se guardan a partir de la direccion 5000 de la memoria EEPROM
unsigned int historicoMaster;            // Cantidad historica de tarjetas pasadas tipo master
unsigned int historicoHidrolavadora;     // Cantidad historica de tarjetas pasadas tipo hidrolavadora
unsigned int historicoAspiradora;        // Cantidad historica de tarjetas pasadas tipo aspiradora
unsigned int historicoEspuma;            // Cantidad historica de tarjetas pasadas tipo espuma
unsigned int diarioHidrolavadora;        // Cantidad diaria de tarjetas pasadas tipo hidrolavadora
unsigned int diarioAspiradora;           // Cantidad diaria de tarjetas pasadas tipo aspiradora
unsigned int diarioEspuma;               // Cantidad diaria de taretas pasadas de tipo espuma
unsigned int timeDiarioHidrolavadora;    // Tiempo de uso diario de la hidrolavadora en minutos
unsigned int timeDiarioAspiradora;       // Tiempo de uso diario de la aspiradora en minutos
unsigned int timeDiarioEspuma;           // Tiempo de uso diario de espuma en minutos
unsigned int timeHistoricoHidrolavadora; // Tiempo total de uso de la hidrolavadora en minutos
unsigned int timeHistoricoAspiradora;    // Tiempo total de uso de la aspiradora en minutos
unsigned int timeHistoricoEspuma;        // Tiempo total de uso de espuma en minutos

uint8_t *pMenuPpal[5];		            // Arreglo de punteros para manejar como buffer circular los mensajes del menu principal del Display
uint8_t *pMenuCancel[2];		          // Arreglo de punteros para manejar como buffer circular los mensajes del menu cancelar del Display
uint8_t *pSubmenuTipo[5];	            // Arreglo de punteros para manejar como un buffer circular los mensajes del submenu tipo de tarjetas


int main(void)
{
	clockprescale();
  usart_init();
	initPuertos();
	Init_TWI();
	int0_enter_init();
  initVariables();
  // Configura el RFID
	TCCR0B = 1<<CS01 | 1<<CS00; //Timer/Counter0 prescaler 64
	EICRA |= 1<<ISC10; //Any logical change on INT0 generates an interrupt request
	EIMSK |= 1<<INT1; //External interrupt request 0 enable (demod_out from RFID chip)

  // Habilito las interrupciones por cambio, de los pines SUBIR y BAJAR
	PCICR |= (1 << PCIE2);     // set PCIE2 to enable PCMSK2 scan
  PCMSK2 |= (1 << PCINT22) | (1 << PCINT23);   // set PCINT22 and PCINT23 to trigger an interrupt on state change

  // initialize timer
  timer1_init();

	sei();// Enable the Global Interrupt Enable flag so that interrupts can be processed
	delay(_1seg);
	LCDinit();//init LCD bit, dual line, cursor right
	LCDclr();//clears LCD
	CopyStringtoLCD(LCDversion, 0, 0);
	USART_PutString("VERSION: 1.0");
	delay(_2seg);

  // Obtiene la cantidad de tarjetas en la memoria EEPROM
	cantidadTarjetas = 0;
	dir = 0;
  do{
    LeerTarjeta(dir, &auxvarTarjeta);
    dir+=8;
    if(verificaFIN(auxvarTarjeta)==0) // Devuelve 1 si encontro la palabra "TARJETAS"
      cantidadTarjetas++;
    }while(verificaFIN(auxvarTarjeta)==0 && dir<4000); // Mientras no encuentre el final

  if(cantidadTarjetas == 0 || cantidadTarjetas == 500) //if(dir >= 4000)
  {
    LCDclr();
    LCD_puts(0, "NO HAY TARJETAS");
    LCD_puts(1, "  SIN SERVICIO");
    delay(_2seg);
    sprintf(auxiliar,"N. TARJETAS: %u",cantidadTarjetas);
    LCD_puts(0, auxiliar);
    USART_PutString(auxiliar);
    delay(_2seg);
    cantidadTarjetas = 0;
    dir = 0;
    EscribeFIN(dir);
    loopProgramacion();
  }
  else{
    LCDclr();
    memset(auxiliar, ' ', 16);
    sprintf(auxiliar,"N. TARJETAS: %u",cantidadTarjetas);
    LCD_puts(0, auxiliar);
    USART_PutString(auxiliar);
    delay(_2seg);
    LCDclr();
    LCDcursorOFF();
    LCD_puts(0,"  \"BIENVENIDO\"");
    LCD_puts(1, "  PASE TARJETA");
  }

  // Verifico si tengo que entrar al menu de programación
	if( ENTER_PRESSED() )
	{
		BEEP( _100ms );
		delay( _500ms );
		if( ENTER_PRESSED() )
		{
			BEEP( _100ms );
			delay( _100ms );
			BEEP( _100ms );

      loopProgramacion();
		}
  }

	LCDcursorOFF();
	LCDclr();
  LCD_puts(0,"  \"BIENVENIDO\"");
  LCD_puts(1, "  PASE TARJETA");

	while(1)
	{
    //waiting for the usart-interrupt to trigger
    if (em4095_read_tag(tag))
		{
		    if(rebote == 0)
        {
          BEEP( _100ms );
          sprintf(auxiliar,"\x02R%.2X%.2X%.2X%.2X%.2X\x04", tag[0], tag[1], tag[2], tag[3], tag[4]);
          //LCD_puts(0, "LEYO TARJETA");  //Comentar esta linea en la version final
          //LCD_puts(1, auxiliar);        //Comentar esta linea en la version final
          USART_PutString(auxiliar);
          //delay(_2seg);

          varTarjeta.ID[0]=tag[0];
          varTarjeta.ID[1]=tag[1];
          varTarjeta.ID[2]=tag[2];
          varTarjeta.ID[3]=tag[3];
          varTarjeta.ID[4]=tag[4];
          varTarjeta.tipo = 0;
          varTarjeta.duracion =0;
          varTarjeta.disponible = 0;

          tag[0] = 0;
          tag[1] = 0;
          tag[2] = 0;
          tag[3] = 0;
          tag[4] = 0;

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
                LCDclr();
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
                delay(_2seg);
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
                    mostrarContadores();
                    delay(_3seg);
                    historicoMaster++;
                    timeDiarioAspiradora = 0;
                    timeDiarioEspuma = 0;
                    timeDiarioHidrolavadora = 0;
                    diarioAspiradora = 0;
                    diarioEspuma = 0;
                    diarioHidrolavadora = 0;
                    salvarContadores();
                    cargarContadores();
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
                      LCD_puts(0," HIDROLAVADORA");
                      // actualizo los contadores
                      historicoHidrolavadora++;
                      diarioHidrolavadora++;
                      timeHistoricoHidrolavadora += auxvarTarjeta.duracion;
                      timeDiarioHidrolavadora += auxvarTarjeta.duracion;

                      HIDRO_ON();
                      tiempoRestante();
                      HIDRO_OFF();
                      // salvo los contadores
                      salvarContadores();
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
                      historicoEspuma++;
                      diarioEspuma++;
                      timeHistoricoEspuma += auxvarTarjeta.duracion;
                      timeDiarioEspuma += auxvarTarjeta.duracion;

                      ESPUMA_ON();
                      tiempoRestante();
                      ESPUMA_OFF();
                      // salvo los contadores
                      salvarContadores();
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
                      historicoAspiradora++;
                      diarioAspiradora++;
                      timeHistoricoAspiradora += auxvarTarjeta.duracion;
                      timeDiarioAspiradora += auxvarTarjeta.duracion;

                      ASPIRADORA_ON();
                      tiempoRestante();
                      ASPIRADORA_OFF();
                      // salvo los contadores
                      salvarContadores();
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

void loopProgramacion(void)
{
	unsigned int direccion = 0;
	unsigned int i = 0;
	LCDclr();	// Borra el LCD
	//Menu por defecto:
	// MENÚ  PROGRAMACIÓN
	//CARGAR TARJETA
	menuDefault();
	delay(_4seg);
	while(flags.programacionMode == 1)
	{
		// verifico si tengo que entrar al menu
		if (flags.aceptarProgramacion == 1)
		{
			switch(selMenu)
			{
				case CARGAR:
				  flags.aceptarProgramacion = 0;
					MenuCargarTarjeta();
					menuDefault();
					break;
				case LEER:
				  // mostrar progreso en el lcd tambien
					flags.aceptarProgramacion = 0;	// clareo el flag de aceptar programacion
					dir = 0;
					LCD_puts(1,"LEYENDO MEMORIA");
					do
					{
						LeerTarjeta(dir, &auxvarTarjeta);
						if(verificaFIN(auxvarTarjeta)==0)
            {
              sprintf(auxiliar,"\x02R%.2X%.2X%.2X%.2X%.2X\x04", auxvarTarjeta.ID[0],
                                                                auxvarTarjeta.ID[1],
                                                                auxvarTarjeta.ID[2],
                                                                auxvarTarjeta.ID[3],
                                                                auxvarTarjeta.ID[4]);
              USART_PutString(auxiliar);
              dir += 8;
            }
            else
            {
              sprintf(auxiliar,"%c%c%c%c%c%c%c%c",auxvarTarjeta.ID[0],
                                                  auxvarTarjeta.ID[1],
                                                  auxvarTarjeta.ID[2],
                                                  auxvarTarjeta.ID[3],
                                                  auxvarTarjeta.ID[4],
                                                  auxvarTarjeta.tipo,
                                                  auxvarTarjeta.duracion,
                                                  auxvarTarjeta.disponible);
              USART_PutString(auxiliar);
            }

					}while((dir < 6000) && (verificaFIN(auxvarTarjeta) == 0));
					menuDefault();
          break;
				case ESCRIBIR:
					flags.aceptarProgramacion = 0;	// clareo el flag de aceptar programacion
					LCDclr();
					CopyStringtoLCD(LCDaviso, 0, 0);
					CopyStringtoLCD(LCDaviso2, 0, 1);
					delay(_3seg);
					subMenuConfirmacion();
					if(flags.continuaCancela == 1)
					{
            LCDGotoXY(0, 1);						//Posiciono el cursor en el 1º caracter del mensaje
            LCDcursorOnBlink();						//Underline blinking cursor ON
            CopyStringtoLCD(LCDerasingMemory, 0, 0);
            BorrarMemoria();
            initContadores();
            salvarContadores();
            dir=0;
            EscribeFIN(dir);
            CopyStringtoLCD(LCDmemoryClear, 0, 1);
            delay(_2seg);
            LCDclr();
            LCDGotoXY(0,0);
            CopyStringtoLCD(LCDcargueTarjetas, 0, 0);
            dir = 0;
            cantidadTarjetas = 0;
            USART_interrupt_init();
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
              if(flags.listaParaGrabar == 1)
              {
                sprintf(auxiliar,"\x02R%.2X%.2X%.2X%.2X%.2X\x04", varTarjeta.ID[0],
                                                                  varTarjeta.ID[1],
                                                                  varTarjeta.ID[2],
                                                                  varTarjeta.ID[3],
                                                                  varTarjeta.ID[4]);
                LCD_puts(0, auxiliar);
                if(!repetida())
                {
                  EscribirTarjeta(dir, varTarjeta);
                  LeerTarjeta(dir, &auxvarTarjeta);
                  if(ComparaID(varTarjeta, auxvarTarjeta))
                  {
                    dir+=8;
                    cantidadTarjetas++;
                    LCD_puts(1,"      ACK");
                    USART_PutString("ACK");
                  }
                  else
                  {
                    LCD_puts(1,"     ERROR");
                    USART_PutString("ERROR");
                  }
                  flags.listaParaGrabar = 0;
                }
                flags.listaParaGrabar = 0;
              }
            }
            EscribeFIN(dir);
            USART_PutString("FIN");
            flags.tarjetaAgregada = 1;
            USART_interrupt_disable();
            LCDclr();
            LCD_puts(0,"      FIN");
            delay(_2seg);
					}
					menuDefault();
					break;
        case CONTADOR:
          flags.aceptarProgramacion = 0;
          cargarContadores();
          mostrarContadores();
          menuDefault();
          break;
				case SALIR:
				  flags.aceptarProgramacion = 0;  // clareo el flag de aceptar programacion
					flags.programacionMode = 0;
					break;
				default:
					flags.aceptarProgramacion = 0;	// clareo el flag de aceptar programacion
					break;
			}
		}
		// verifico si tengo que cambiar el menu
		else if (flags.actualizarMenu == 1)
		{
			refrescarMenu();	//refresco el menu
			flags.actualizarMenu = 0;	// clareo el flag para no volver a cambiar el menu
		}
	}
}

void MenuCargarTarjeta(void)
{
	LCDclr();	// Borra el LCD
	// Menu por default:
	// TIPO DE TARJETA:
	// MASTER
	LCD_puts(0,"  PASE TARJETA");
	LCDGotoXY(0, 1);						//Posiciono el cursor en el 1º caracter del mensaje
	LCDcursorOnBlink();						//Underline blinking cursor ON
	contTipo = 1;
	flags.aceptarProgramacion = 0;
	flags.actualizarMenu = 0;
  flags.programacionMode2 = 1;
  clock_descartarTarjetas = 0;
  while( (!em4095_read_tag(tag)) && (clock_descartarTarjetas < _10seg) );  // me quedo a la espera que depositen la tarjeta
  if(clock_descartarTarjetas >= _10seg)
  {
    LCDclr();
    LCD_puts(0, "TIEMPO DE ESPERA");
    LCD_puts(1, "    AGOTADO");
    delay(_2seg);
    flags.programacionMode2 = 0;
    flags.aceptarProgramacion = 0;	// clareo el flag de aceptar programación
  }
  else
  {
    if(rebote == 0)
    {
      DDRD &= ~(1<<SHD); //Mod, Shd
      varTarjeta.ID[0]=tag[0];
      varTarjeta.ID[1]=tag[1];
      varTarjeta.ID[2]=tag[2];
      varTarjeta.ID[3]=tag[3];
      varTarjeta.ID[4]=tag[4];
      varTarjeta.tipo = 0;
      varTarjeta.duracion =0;
      varTarjeta.disponible = 0;
      sprintf(auxiliar,"\x02R%.2X%.2X%.2X%.2X%.2X\x04", varTarjeta.ID[0],
                                                      varTarjeta.ID[1],
                                                      varTarjeta.ID[2],
                                                      varTarjeta.ID[3],
                                                      varTarjeta.ID[4]);
      LCD_puts(1, auxiliar);
      BEEP(_100ms);
      delay(_2seg);

      tag[0] = 0;
      tag[1] = 0;
      tag[2] = 5;
      tag[3] = 0;
      tag[4] = 0;

      CopyStringtoLCD(LCDtipo, 0, 0);
      CopyStringtoLCD(pSubmenuTipo[1], 0, 1);
      LCDGotoXY(0, 1);						//Posiciono el cursor en el 1º caracter del mensaje
      flags.programacionMode2 = 1;
      flags.aceptarProgramacion = 0;
      contTipo = 1;
      rebote++;
      DDRD |= (1<<SHD);
    }
    else
      rebote = 0;
  }
  while(flags.programacionMode2 == 1)
	{
		// verifico si tengo que entrar al menu
		if (flags.aceptarProgramacion == 1)
		{
			switch(contTipo)
			{
				case T_MASTER:
					flags.aceptarProgramacion = 0;	// clareo el flag de aceptar programacion
					varTarjeta.tipo = T_MASTER;	// le asigno a la tarjeta el tipo Master
					flags.setTime = 1;				// seteo el flag para entrar al menu de asignacion de tiempo de vida de la tarjeta
					break;
				case T_HIDRO:
					flags.aceptarProgramacion = 0;	// clareo el flag de aceptar programacion
					varTarjeta.tipo = T_HIDRO;	// le asigno a la tarjeta el tipo Hidrolavado
					flags.setTime = 1;				// seteo el flag para entrar al menu de asignacion de tiempo de vida de la tarjeta
					break;
				case T_ESPUMA:
					flags.aceptarProgramacion = 0;	// clareo el flag de aceptar programacion
					varTarjeta.tipo = T_ESPUMA;	// le asigno a la tarjeta el tipo Espuma
					flags.setTime = 1;				// seteo el flag para entrar al menu de asignacion de tiempo de vida de la tarjeta
					break;
				case T_ASPIRA:
					flags.aceptarProgramacion = 0;	// clareo el flag de aceptar programacion
					varTarjeta.tipo = T_ASPIRA;	// le asigno a la tarjeta el tipo Aspiradora
					flags.setTime = 1;				// seteo el flag para entrar al menu de asignacion de tiempo de vida de la tarjeta
					break;
				case T_SALIR:
				  flags.aceptarProgramacion = 0;
					flags.programacionMode2 = 0;		//salgo del menu
					break;
				default:
					flags.aceptarProgramacion = 0;	// clareo el flag de aceptar programacion
					break;
			}
			if(flags.setTime == 1)
				SetearDuracion();
		}
		// verifico si tengo que cambiar el menu
		else if (flags.actualizarMenu == 1)
		{
			refrescarSubMenu();	//refresco el menu
			flags.actualizarMenu = 0;	// clareo el flag para no volver a cambiar el menu
		}
	}
}

void SetearDuracion(void)
{
	LCDclr();	// Borra el LCD
	// Menu por default:
	// TIEMPO DE VIDA (MIN)
	// 5
	CopyStringtoLCD(LCDvida, 0, 0);
	LCD_puts(1,"     00:10");
	LCDGotoXY(8, 1);						//Posiciono el cursor en el 1º caracter del mensaje
	LCDcursorOnBlink();						//Underline blinking cursor ON
	contDuracion = 1;
	flags.aceptarProgramacion = 0;
	flags.actualizarMenu = 0;

	while(flags.programacionMode2 == 1)
	{
		if (flags.aceptarProgramacion == 1)
		{
			varTarjeta.duracion = contDuracion;
			varTarjeta.disponible = 0;
			flags.aceptarProgramacion = 0;
			flags.programacionMode2 = 0;		//salgo del menu
			//grabar varTarjeta en la eeprom
			//if((tag[0]!= 0) && (tag[1]!= 0) && (tag[2]!= 0) && (tag[3]!= 0) && (tag[4]!= 0))
			if(!repetida())
      {
        dir = cantidadTarjetas * 8;
        EscribirTarjeta(dir, varTarjeta);
        dir+=8; //ACA TENGO QEU TENER CUIDADO CON LA DIRECCION; ASI ESTARIA PISANDO OTRAS TARJETAS
        EscribeFIN(dir);
        dir+=8;
        cantidadTarjetas++;
        sprintf(auxiliar,"N. TARJETAS: %u",cantidadTarjetas);
        USART_PutString(auxiliar);
        LCD_puts(1,auxiliar);
        delay(_2seg);
      }
		}
		// verifico si tengo que cambiar el menu
		else if (flags.actualizarMenu == 1)
		{
			memset(auxiliar, ' ', 16);
			sprintf(auxiliar, "     %02d:%02d", (contDuracion*10)/60,(contDuracion*10)%60);
			LCD_puts(1, auxiliar);
			LCDGotoXY(9, 1);
			flags.actualizarMenu = 0;	// clareo el flag para no volver a cambiar el menu
		}
	}
}

//Funcion para actualizar el menu
void refrescarMenu(void)
{
	//Si todavia no apreto entrar al menu, cambia el menu mostrado
	CopyStringtoLCD(pMenuPpal[selMenu], 0, 1);	//muestro en la 2º línea del lcd el menu actual
	LCDGotoXY(0,1);	//posiciono el cursor en el 1º espacio de la 2º línea
	LCDcursorOnBlink();	//Hago destellar el cursor
}

void initContadores(void)
{
  historicoMaster = 0;
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
}

//Funcion para actualizar el submenu de cargar tarjeta
void refrescarSubMenu(void)
{
	//Si todavia no apreto entrar al menu, cambia el menu mostrado
	CopyStringtoLCD(pSubmenuTipo[contTipo], 0, 1);	//muestro en la 2º línea del lcd el menu actual
	LCDGotoXY(0,1);	//posiciono el cursor en el 1º espacio de la 2º línea
	LCDcursorOnBlink();	//Hago destellar el cursor
}

void subMenuConfirmacion(void)
{
	LCDclr();
	CopyStringtoLCD(LCDdesea, 0, 0);    //desea continuar?
	CopyStringtoLCD(LCDcancelar, 0, 1); //cancelar

	LCDGotoXY(0, 1);						//Posiciono el cursor en el 1º caracter del mensaje
	LCDcursorOnBlink();						//Underline blinking cursor ON

	cancelCont = 0;
	flags.aceptarProgramacion = 0;
	flags.actualizarMenu = 0;
  flags.programacionMode2 = 1;

	while(flags.programacionMode2 == 1)
	{
		// verifico si tengo que entrar al menu
		if (flags.aceptarProgramacion == 1)
		{
			switch(cancelCont)
			{
				case 0: // CANCELAR
					flags.continuaCancela = 0;      // cancelo el borrado
          flags.aceptarProgramacion = 0;  // clareo el flag de aceptar programacion
					flags.programacionMode2 = 0;    // salgo del menu
					break;
				case 1: // CONTINUAR
					flags.continuaCancela = 1;			// seteo el flag para comenzar a cargar la EEPROM
					flags.aceptarProgramacion = 0;	// clareo el flag de aceptar programacion
					flags.programacionMode2 = 0;
					break;
				default:
          flags.continuaCancela = 0;      // cancelo el borrado
					flags.aceptarProgramacion = 0;	// clareo el flag de aceptar programacion
					break;
			}
		}
		// verifico si tengo que cambiar el menu
		else if (flags.actualizarMenu == 1)
		{
			refrescarConfirmacion();	//refresco el menu
			flags.actualizarMenu = 0;	// clareo el flag para no volver a cambiar el menu
		}
	}
}

//Funcion para actualizar el menu
void refrescarConfirmacion(void)
{
	//Si todavia no apreto entrar al menu, cambia el menu mostrado
	CopyStringtoLCD(pMenuCancel[cancelCont], 0, 1);	//muestro en la 2º línea del lcd el menu actual
	LCDGotoXY(0,1);	//posiciono el cursor en el 1º espacio de la 2º línea
	LCDcursorOnBlink();	//Hago destellar el cursor
}

// menu por default
void menuDefault(void)
{
  selMenu = 0;
  CopyStringtoLCD(LCDmenu, 0, 0);			//Muestra en la 1º linea del display un mensaje de Menu
	CopyStringtoLCD(pMenuPpal[0], 0, 1);	//Muestra en la 2º linea del display un mensaje de cargar tarjeta
	LCDGotoXY(0, 1);						//Posiciono el cursor en el 1º caracter del mensaje
	LCDcursorOnBlink();						//Underline blinking cursor ON
	flags.aceptarProgramacion = 0;
	flags.programacionMode = 1;
}

//-------------------------------------
// inicializa todas las variables
//-------------------------------------
void initVariables(void)
{
	// Cargo en los arreglos de punteros los mensajes del menu para el lcd
	//Menu principal
	pMenuPpal[0]=&LCDcargar;	  //Cargar tarjeta
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

	selMenu = 0;
	portbhistory = 0x00;
	estadoMenu = 0;
	contTipo = 0;
	contDuracion = 0;
	cantidadTarjetas = 0;
	posicion = 0;
	cancelCont = 0;
	dir = 0;

  tag[0] = 0;
  tag[1] = 0;
  tag[2] = 0;
  tag[3] = 0;
  tag[4] = 0;

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
	historicoMaster = 0;
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
  clock_descartarTarjetas = 0;
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

// ISR PCINT2 para el manejo de los cambios en los pines SUBIR y BAJAR
ISR (PCINT2_vect)
{
	/*
	if( (PINB & (1 << PINB0)) == 1 )
	{
		// LOW to HIGH pin change
	}
	else
	{
		// HIGH to LOW pin change
	}
	*/
	uint8_t changedbits;

	changedbits = PIND ^ portbhistory;
	portbhistory = PIND;

	if(changedbits & BOTON_BAJAR)
	{
		/* PCINT22 changed */
		_delay_ms(100);
		if(portbhistory & BOTON_BAJAR)
		{
			flags.actualizarMenu = 1;
			if(selMenu == 0)
        selMenu = 4;
      else
        selMenu = (--selMenu)%5;	    //decremento el indice del buffer circular para los mensajes del menu ppal (el %mod indica la cant de menus)
			if(cancelCont == 0)
        cancelCont = 1;
      else
        cancelCont = 0;
			if(contTipo == 0)			//decremento el indice del buffer circular para los mensajes del submenu tipo
				contTipo = 4;
			else
				contTipo = (--contTipo)%5;
			if (contDuracion > 0)		//decremento el tiempo de vida de la tarjeta
				contDuracion--;
		}
	}

	if(changedbits & BOTON_SUBIR)		//corroboro si fue el boton que provoco la interrupcion
	{
		/* PCINT23 changed */
		_delay_ms(100);					//estas 2 lineas se usan para antirrebote
		if(portbhistory & BOTON_SUBIR)
		{
			flags.actualizarMenu = 1;
			selMenu = (++selMenu)%5;	    //incremento el indice del buffer circular para los mensajes del menu ppal
			contTipo = (++contTipo)%5;	  //incremento el indice del buffer circular para los mensajes del submenu tipo
			contDuracion++;				        //incremento el tiempo de vida de la tarjeta
			cancelCont =(++cancelCont)%2; //incremento el indice del buffer circular para los mensajes del menu cancelar
		}
	}

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

//borra una pagina de la memoria 24fc512
void BorrarPagina(unsigned int dev_address)
{
	unsigned int i;
	for (i=0; i<128; i++)
	{
		i2c_writebyte(dev_address + i,EEPROM_ID,EEPROM_ADDR, 0xFF);
	}
}

//borra toda la memoria 24fc512
void BorrarMemoria(void)
{
	unsigned int i, direccion = 0;
	for(i=0; i<60; i++)
	{
		BorrarPagina(direccion);
		direccion+=128;
    memset(auxiliar, ' ', 16);
    sprintf(auxiliar,"ERASE PAG.: %u", i);
    LCD_puts(1,auxiliar);
	}
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
  clock_minutos = (auxvarTarjeta.duracion*10)/60;
  clock_segundos = (auxvarTarjeta.duracion*10)%60;
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
  /*uint8_t i;
  for(i=0;i<255;i++)
		{
			LCDGotoXY(0, 1);
			LCDprogressBar(i, 255, 16);
		}*/
  LCDGotoXY(0, 1);
  LCDprogressBar2(16);
}

void cargarContadores(void)
{
  uint8_t dataLow;
  uint8_t dataHigh;
  i2c_readbyte(5000,EEPROM_ID,EEPROM_ADDR,&dataHigh);
  i2c_readbyte(5001,EEPROM_ID,EEPROM_ADDR,&dataLow);
  historicoMaster = (unsigned int)(dataHigh&0x00ff);
  historicoMaster = (historicoMaster<<8) | (dataLow);
  i2c_readbyte(5002,EEPROM_ID,EEPROM_ADDR,&dataHigh);
  i2c_readbyte(5003,EEPROM_ID,EEPROM_ADDR,&dataLow);
  //historicoHidrolavadora = (unsigned int)(dataHigh<<8) | (dataLow);
  historicoHidrolavadora = (unsigned int)(dataHigh&0x00ff);
  historicoHidrolavadora = (historicoHidrolavadora<<8) | (dataLow);
  i2c_readbyte(5004,EEPROM_ID,EEPROM_ADDR,&dataHigh);
  i2c_readbyte(5005,EEPROM_ID,EEPROM_ADDR,&dataLow);
  historicoAspiradora = (unsigned int)(dataHigh&0x00ff);
  historicoAspiradora = (historicoAspiradora<<8) | (dataLow);
  i2c_readbyte(5006,EEPROM_ID,EEPROM_ADDR,&dataHigh);
  i2c_readbyte(5007,EEPROM_ID,EEPROM_ADDR,&dataLow);
  historicoEspuma = (unsigned int)(dataHigh&0x00ff);
  historicoEspuma = (historicoEspuma<<8) | (dataLow);
  i2c_readbyte(5008,EEPROM_ID,EEPROM_ADDR,&dataHigh);
  i2c_readbyte(5009,EEPROM_ID,EEPROM_ADDR,&dataLow);
  diarioHidrolavadora = (unsigned int)(dataHigh&0x00ff);
  diarioHidrolavadora = (diarioHidrolavadora<<8) | (dataLow);
  i2c_readbyte(5010,EEPROM_ID,EEPROM_ADDR,&dataHigh);
  i2c_readbyte(5011,EEPROM_ID,EEPROM_ADDR,&dataLow);
  diarioAspiradora = (unsigned int)(dataHigh&0x00ff);
  diarioAspiradora = (diarioAspiradora<<8) | (dataLow);
  i2c_readbyte(5012,EEPROM_ID,EEPROM_ADDR,&dataHigh);
  i2c_readbyte(5013,EEPROM_ID,EEPROM_ADDR,&dataLow);
  diarioEspuma = (unsigned int)(dataHigh&0x00ff);
  diarioEspuma = (diarioEspuma) | (dataLow);
  i2c_readbyte(5014,EEPROM_ID,EEPROM_ADDR,&dataHigh);
  i2c_readbyte(5015,EEPROM_ID,EEPROM_ADDR,&dataLow);
  timeDiarioHidrolavadora = (unsigned int)(dataHigh&0x00ff);
  timeDiarioHidrolavadora = (diarioHidrolavadora) | (dataLow);
  i2c_readbyte(5016,EEPROM_ID,EEPROM_ADDR,&dataHigh);
  i2c_readbyte(5017,EEPROM_ID,EEPROM_ADDR,&dataLow);
  timeDiarioAspiradora = (unsigned int)(dataHigh&0x00ff);
  timeDiarioAspiradora = (timeDiarioAspiradora<<8) | (dataLow);
  i2c_readbyte(5018,EEPROM_ID,EEPROM_ADDR,&dataHigh);
  i2c_readbyte(5019,EEPROM_ID,EEPROM_ADDR,&dataLow);
  timeDiarioEspuma = (unsigned int)(dataHigh&0x00ff);
  timeDiarioEspuma = (timeHistoricoEspuma<<8) | (dataLow);
  i2c_readbyte(5020,EEPROM_ID,EEPROM_ADDR,&dataHigh);
  i2c_readbyte(5021,EEPROM_ID,EEPROM_ADDR,&dataLow);
  timeHistoricoHidrolavadora = (unsigned int)(dataHigh&0xff);
  timeHistoricoHidrolavadora = (timeDiarioHidrolavadora<<8) | (dataLow);
  i2c_readbyte(5022,EEPROM_ID,EEPROM_ADDR,&dataHigh);
  i2c_readbyte(5023,EEPROM_ID,EEPROM_ADDR,&dataLow);
  timeHistoricoAspiradora = (unsigned int)(dataHigh&0x00ff);
  timeHistoricoAspiradora = (timeHistoricoAspiradora<<8) | (dataLow);
  i2c_readbyte(5024,EEPROM_ID,EEPROM_ADDR,&dataHigh);
  i2c_readbyte(5025,EEPROM_ID,EEPROM_ADDR,&dataLow);
  timeHistoricoEspuma = (unsigned int)(dataHigh&0x00ff);
  timeHistoricoEspuma = (timeHistoricoEspuma<<8) | (dataLow);
}

void salvarContadores(void)
{
  uint8_t dataLow;
  uint8_t dataHigh;
  dataHigh = (unsigned char)(historicoMaster>>8);
  dataLow = (unsigned char)historicoMaster;
  i2c_writebyte(5000,EEPROM_ID,EEPROM_ADDR,dataHigh);
  i2c_writebyte(5001,EEPROM_ID,EEPROM_ADDR,dataLow);
  dataHigh = (unsigned char)(historicoHidrolavadora>>8);
  dataLow = (unsigned char)historicoHidrolavadora;
  i2c_writebyte(5002,EEPROM_ID,EEPROM_ADDR,dataHigh);
  i2c_writebyte(5003,EEPROM_ID,EEPROM_ADDR,dataLow);
  dataHigh = (unsigned char)(historicoAspiradora>>8);
  dataLow = (unsigned char)historicoAspiradora;
  i2c_writebyte(5004,EEPROM_ID,EEPROM_ADDR,dataHigh);
  i2c_writebyte(5005,EEPROM_ID,EEPROM_ADDR,dataLow);
  dataHigh = (unsigned char)(historicoEspuma>>8);
  dataLow = (unsigned char)historicoEspuma;
  i2c_writebyte(5006,EEPROM_ID,EEPROM_ADDR,dataHigh);
  i2c_writebyte(5007,EEPROM_ID,EEPROM_ADDR,dataLow);
  dataHigh = (unsigned char)(diarioHidrolavadora>>8);
  dataLow = (unsigned char)diarioHidrolavadora;
  i2c_writebyte(5008,EEPROM_ID,EEPROM_ADDR,dataHigh);
  i2c_writebyte(5009,EEPROM_ID,EEPROM_ADDR,dataLow);
  dataHigh = (unsigned char)(diarioAspiradora>>8);
  dataLow = (unsigned char)diarioAspiradora;
  i2c_writebyte(5010,EEPROM_ID,EEPROM_ADDR,dataHigh);
  i2c_writebyte(5011,EEPROM_ID,EEPROM_ADDR,dataLow);
  dataHigh = (unsigned char)(diarioEspuma>>8);
  dataLow = (unsigned char)diarioEspuma;
  i2c_writebyte(5012,EEPROM_ID,EEPROM_ADDR,dataHigh);
  i2c_writebyte(5013,EEPROM_ID,EEPROM_ADDR,dataLow);
  dataHigh = (unsigned char)(timeDiarioHidrolavadora>>8);
  dataLow = (unsigned char)timeDiarioHidrolavadora;
  i2c_writebyte(5014,EEPROM_ID,EEPROM_ADDR,dataHigh);
  i2c_writebyte(5015,EEPROM_ID,EEPROM_ADDR,dataLow);
  dataHigh = (unsigned char)(timeDiarioAspiradora>>8);
  dataLow = (unsigned char)timeDiarioAspiradora;
  i2c_writebyte(5016,EEPROM_ID,EEPROM_ADDR,dataHigh);
  i2c_writebyte(5017,EEPROM_ID,EEPROM_ADDR,dataLow);
  dataHigh = (unsigned char)(timeDiarioEspuma>>8);
  dataLow = (unsigned char)timeDiarioEspuma;
  i2c_writebyte(5018,EEPROM_ID,EEPROM_ADDR,dataHigh);
  i2c_writebyte(5019,EEPROM_ID,EEPROM_ADDR,dataLow);
  dataHigh = (unsigned char)(timeHistoricoHidrolavadora>>8);
  dataLow = (unsigned char)timeHistoricoHidrolavadora;
  i2c_writebyte(5020,EEPROM_ID,EEPROM_ADDR,dataHigh);
  i2c_writebyte(5021,EEPROM_ID,EEPROM_ADDR,dataLow);
  dataHigh = (unsigned char)(timeHistoricoAspiradora>>8);
  dataLow = (unsigned char)timeHistoricoAspiradora;
  i2c_writebyte(5022,EEPROM_ID,EEPROM_ADDR,dataHigh);
  i2c_writebyte(5023,EEPROM_ID,EEPROM_ADDR,dataLow);
  dataHigh = (unsigned char)(timeHistoricoEspuma>>8);
  dataLow = (unsigned char)timeHistoricoEspuma;
  i2c_writebyte(5024,EEPROM_ID,EEPROM_ADDR,dataHigh);
  i2c_writebyte(5025,EEPROM_ID,EEPROM_ADDR,dataLow);
}

void mostrarContadores(void)
{
  //mostrar todos los historicos y los tiempos por la usart, en el lcd solo cantidad de tarjetas en el dia
  LCDclr();
  USART_PutString("Historico cantidad de tarjetas usadas");
  sprintf(auxiliar,"MASTER: %u",historicoMaster);
  USART_PutString(auxiliar);

  sprintf(auxiliar,"HIDRO.: %u",historicoHidrolavadora);
  USART_PutString(auxiliar);

  sprintf(auxiliar,"ASPI.: %u",historicoAspiradora);
  USART_PutString(auxiliar);

  sprintf(auxiliar,"ESPUMA: %u",historicoEspuma);
  USART_PutString(auxiliar);

  USART_PutString("Diario cantidad de tarjetas usadas");
  LCD_puts(0, "Cant. Tarjetas:");
  sprintf(auxiliar,"HIDRO.: %u",diarioHidrolavadora);
  LCD_puts(1,auxiliar);
  USART_PutString(auxiliar);
  delay(_2seg);

  sprintf(auxiliar,"ASPI.: %u",diarioAspiradora);
  LCD_puts(1,auxiliar);
  USART_PutString(auxiliar);
  delay(_2seg);

  sprintf(auxiliar,"ESPUMA: %u",diarioEspuma);
  LCD_puts(1,auxiliar);
  USART_PutString(auxiliar);
  delay(_2seg);

  USART_PutString("Uso diario de los equipos (multiplicar por 10 segundos)");
  sprintf(auxiliar,"HIDRO.: %u",timeDiarioHidrolavadora);
  USART_PutString(auxiliar);

  sprintf(auxiliar,"ASPI.: %u",timeDiarioAspiradora);
  USART_PutString(auxiliar);

  sprintf(auxiliar,"ESPUMA: %u",timeDiarioEspuma);
  USART_PutString(auxiliar);

  USART_PutString("Uso historico de los equipos (multiplicar por 10 segundos)");
  sprintf(auxiliar,"HIDRO.: %u",timeHistoricoHidrolavadora);
  USART_PutString(auxiliar);

  sprintf(auxiliar,"ASPI.: %u",timeHistoricoAspiradora);
  USART_PutString(auxiliar);

  sprintf(auxiliar,"HIDRO.: %u",timeHistoricoEspuma);
  USART_PutString(auxiliar);
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
  USART_Transmit(0X0D);
  USART_Flush();
}

int repetida(void)
{
  unsigned int direccion = 0;
  do
  {
    LeerTarjeta(direccion, &auxvarTarjeta);
    direccion+=8;
  } while( (!ComparaID(varTarjeta, auxvarTarjeta)) && (direccion < 4000) && (!verificaFIN(auxvarTarjeta)) );
  if(ComparaID(varTarjeta, auxvarTarjeta))
  {
    memset(auxiliar, ' ', 16);
    sprintf(auxiliar, "repet. dir: %u", direccion/8);
    LCD_puts(1,auxiliar);
    USART_PutString(auxiliar);
    delay(_2seg);
    return 1; // La tarjeta esta repetida
  }
  else return 0;
}
