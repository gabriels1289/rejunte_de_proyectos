#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include "varios.h"

// máscaras para lectura de pulsadores y manejo de las salidas
#define HIDROLAVADO 0x01	//PC0
#define ESPUMA		0x02	//PC1
#define	ASPIRADORA	0x04	//PC2
#define	BUZZER		0x08	//PC3
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
#define HIDRO_ON()		(PORTC |= HIDROLAVADO)		// enciende la hidrolavadora
#define HIDRO_OFF()		(PORTC &= ~HIDROLAVADO)		// apaga la hidrolavadora
#define ESPUMA_ON()		(PORTC |= ESPUMA)			// enciende la espuma
#define ESPUMA_OFF()	(PORTC |= ~ESPUMA)			// apaga la espuma
#define ASPIRADORA_ON()	(PORTC |= ASPIRADORA)		// enciende la aspiradora
#define ASPIRADORA_OFF() (PORTC &= ~ASPIRADORA)		// enciende la aspiradora

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
void timer1_init(void);												// configuración del timer para interrupcion de 10mseg
void delay( const unsigned int n_x_10ms);							// Lazo de retardo. El tiempo se indica en decenas de ms
void ReadTarjetaEeprom(tarjeta *tar, unsigned int dev_address);		// Lee la información de una tarjeta de la eeprom y la almacena en una estructura
void WriteTarjetaEeprom(tarjeta tar, unsigned int dev_address);		// Escribe la información de una tarjeta almacenada en una estructura, en la eeprom externa

// variables externas
extern volatile unsigned int delayTimer;    // Timer para retardos
extern volatile unsigned int dir;			// variable global para manejar la direccion de acceso a la eeprom 24FC512

#endif // MAIN_H_INCLUDED
