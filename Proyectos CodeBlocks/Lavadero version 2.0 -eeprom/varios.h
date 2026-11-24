#ifndef VARIOS_H_INCLUDED
#define VARIOS_H_INCLUDED

// Estructura para el manejo de la tarjeta y sus datos asociados
typedef struct
{
	uint8_t ID[5];
	uint8_t tipo;
	uint8_t duracion;
	uint8_t disponible;
}tarjeta;

extern volatile tarjeta	varTarjeta;			//variable de tipo tarjeta para la lectura y escritura de una sola tarjeta
extern volatile tarjeta	auxvarTarjeta;			//variable de tipo tarjeta para la lectura y escritura de una sola tarjeta
extern volatile tarjeta arrayTarjetas[16];	//arreglo de tarjetas para el manejo de una página completa de la eeprom


#endif // VARIOS_H_INCLUDED
