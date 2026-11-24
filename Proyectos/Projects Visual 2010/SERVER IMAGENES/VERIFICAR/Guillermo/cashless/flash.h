// Dirección inicial de cada bloque de memoria de programa
#define OFFSET_FLASH    0x1000
#define OFFSET_CODE     0x1100
#define OFFSET_CODE2    0x0FA00
#define OFFSET_CODE3    0x0FC00
#define OFFSET_INTVECT  0x0FE00

//------------------------------------------------------------------------------
// Escribir una palabra en la flash
//------------------------------------------------------------------------------
void flash_ww( unsigned int *Data_ptr, unsigned int word );

//------------------------------------------------------------------------------
// Borra un segmento en la flash
//------------------------------------------------------------------------------
void flash_clr( unsigned int *Data_ptr );

//------------------------------------------------------------------------------
// Inicializa el controlador de la flash para la escritura
//------------------------------------------------------------------------------
void InitFlashCtl(void);

//------------------------------------------------------------------------------
// Reprograma la flash de código con una nueva versión descargada por la red
//------------------------------------------------------------------------------
void flashPgm(void);

