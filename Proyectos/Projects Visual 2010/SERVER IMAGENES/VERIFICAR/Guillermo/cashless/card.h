// card.h

// archivo de encabezado de card.c

// Prototipos de funciones, variables externas y constantes para el manejo del lector de tarjetas

unsigned char getCardStatus(void);  // Retorna estado del lector de tarjetas (0:no hay tarjeta - 1:si)

unsigned char cardAnswerToReset(unsigned char * cardID); // Efectúa el reset y answer-to-reset de la tarjeta

                                    // Lee "nBytes" de la memoria indicada, a partir de "from"
unsigned char cardRead( unsigned char cmd, unsigned char *buf, unsigned char from, unsigned char nBytes );

                                    // Escribe un byte en la dirección de memoria indicada
unsigned char cardWriteByte( unsigned char cmd, unsigned char data, unsigned char addr );

                                    // Escribe "nBytes" en memoria, a partir de "from"
unsigned char cardWrite( unsigned char *buf, unsigned char from, unsigned char nBytes );

void cardBreak(void);   // Hace un break para interrumpir una secuencia

// Comandos para la tarjeta

#define CMD_READ_MAIN  0x30     // Leer memoria ppal
#define CMD_WRITE_MAIN 0x38     // Escribir en memoria ppal
#define CMD_READ_SECU  0x31     // Leer memoria de seguridad
#define CMD_WRITE_SECU 0x39     // Escribir en memoria de seguridad
#define CMD_COMP_VERIF 0x33     // Comparar datos de verificación
