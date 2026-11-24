#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include "varios.h"

// máscaras para lectura de pulsadores y manejo de las salidas
#define HIDROLAVADO 0x01	//PC0
#define ESPUMA		  0x02	//PC1
#define	ASPIRADORA	0x04	//PC2
#define	BUZZER		  0x08	//PC3
#define SENSOR_OPTO	0x04	//PD2	//defino un alias para el PD2 donde se usa como SENSOR en modo normal
#define BOTON_ENTER	0x04	//PD2	//defino un alias para el PD2 donde se usa como ENTER al entrar en modo programación
#define BOTON_BAJAR	0x40	//PD6
#define BOTON_SUBIR	0x80	//PD7

#define MOD PD4
#define SHD PD5

// macros para activar y desactivar la emisión de un tono en el buzzer
#define BUZZER_ON()  (PORTC |= BUZZER)    // emite sonido
#define BUZZER_OFF() (PORTC &= ~BUZZER)     // silencio
#define BEEP(time)   {BUZZER_ON();delay(time);BUZZER_OFF();}

// macros para activar y desactivar las salidas de la hidrolavadora, la espuma y la aspiradora
#define HIDRO_ON()		    (PORTC |= HIDROLAVADO)		// enciende la hidrolavadora
#define HIDRO_OFF()		    (PORTC &= ~HIDROLAVADO)		// apaga la hidrolavadora
#define ESPUMA_ON()		    (PORTC |= ESPUMA)			// enciende la espuma
#define ESPUMA_OFF()	    (PORTC &= ~ESPUMA)			// apaga la espuma
#define ASPIRADORA_ON()	  (PORTC |= ASPIRADORA)		// enciende la aspiradora
#define ASPIRADORA_OFF()  (PORTC &= ~ASPIRADORA)		// enciende la aspiradora

// macros para testear si los pulsadores de bajar y subir están presionados
#define BAJAR_PRESSED()  (PIND & BOTON_BAJAR)   // retorna true si está presionado
#define SUBIR_PRESSED()  (PIND & BOTON_SUBIR)   // retorna true si está presionado
#define ENTER_PRESSED()  (PIND & BOTON_ENTER)   // retorna true si está presionado

// Constantes que definen tiempos (en base a los 10 ms del timer)
#define _10ms   1
#define _20ms   2
#define _30ms   3
#define _40ms   4
#define _50ms   5
#define _100ms  10
#define _120ms  12
#define _150ms  15
#define _200ms  20
#define _250ms  25
#define _500ms  50
#define _1seg   100
#define _1500ms 150
#define _2seg   200
#define _3seg   300
#define _4seg   400
#define _5seg   500
#define _10seg  1000
#define _15seg  1500
#define _20seg  2000
#define _30seg  3000
#define _1min   6000
#define _2min   12000

// Prototipos de funciones externas, accesibles desde otros módulos
void timer1_init(void);												                // configuración del timer para interrupcion de 10mseg
void delay( const unsigned int n_x_10ms);							        // Lazo de retardo. El tiempo se indica en decenas de ms
void LeerTarjeta(unsigned int direccion, tarjeta *tar);       // Lee la información de una tarjeta de la eeprom y la almacena en una estructura
void EscribirTarjeta(unsigned int direccion, tarjeta tar);		// Escribe la información de una tarjeta almacenada en una estructura, en la eeprom externa
void usart_init(void);
void USART_Transmit(uint8_t data);
uint8_t USART_Receive(void);
void USART_interrupt_init(void);
void USART_interrupt_disable(void);
ISR(USART_RX_vect);
void USART_Flush(void);
void USART_PutString(uint8_t *cadena);

//---------------------------------------
// Estructura para los flags de control
//---------------------------------------
typedef struct
{
	unsigned int listaParaGrabar     : 1;  // En 1 indica que tengo una tarjeta lista para grabar
	unsigned int actualizarHora      : 1;  // En 1 indica que tengo que actualizar la hora en el display (cada 1 segundo)
	unsigned int tarjetaHallada      : 1;  // En 1 indica que se halló la tarjeta en la memoria
	unsigned int actualizarMenu      : 1;  // En 1 indica que tengo que actualizar el menu
	unsigned int programacionMode    : 1;  // En 1 indica que tengo que permanecer en el menu principal del modo programacion
	unsigned int programacionMode2   : 1;  // En 1 indica que tengo que permanecer en el submenu del modo programacion
	unsigned int aceptarProgramacion : 1;  // En 1 indica confirmación para aceptar la programacion seleccionada
  unsigned int tarjetaDepositada   : 1;  // En 1 indica que se deposito la trajeta
  unsigned int setTime             : 1;  // En 1 indica que tengo que ir a setear el tiempo de vida de la tarjeta
  unsigned int tarjetaAgregada     : 1;  // En 1 indica que se agrego una tarjeta y es necesario recorrer nuevamente la memoria
  unsigned int salirBucle          : 1;  // en 1 indica que tengo que salir del bucle de recorrido de la EEPROM cuando busco una tarjeta
  unsigned int continuaCancela     : 1;  // En 1 indica que el usuario decidio continuar con el borrado de la EEPROM
}Flags;

//Strings stored in AVR Flash memory
const uint8_t LCDwelcomeln1[] PROGMEM="AVR LCD DEMO\0";
const uint8_t LCDversion[]    PROGMEM="  VERSION: 1.0  \0";

#endif // MAIN_H_INCLUDED
