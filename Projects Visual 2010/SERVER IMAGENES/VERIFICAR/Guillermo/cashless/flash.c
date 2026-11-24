// flash.c

#include "msp430x14x.h"
#include "fram.h"
#include "flash.h"

//----------------------------------------------------------
// Funciones para reprogramar la flash
// Obtiene la nueva imagen de programa de la EEPROM externa
// y la copia en CODE
//----------------------------------------------------------
#pragma location="CODE3"

void flashPgm()
{
  unsigned int *p, w;
  void (*f)(void);
  
  // Deshabilita interrupciones y WDT
  asm("dint");

  // Inicializa el controlador de flash
  FCTL2 = 0x0A500 | FSSEL0 | 4;     // ACLK (2 MHz) / 5 = 400 KHz
  
  // Borra todos los segmentos de CODE
  for(p = (unsigned int *)OFFSET_CODE; p<(unsigned int *)OFFSET_CODE2; p+=256)
  {
    FCTL3 = 0x0A500;   // Lock = 0
    FCTL1 = 0x0A502;   // ERASE = 1
    *p = 0;
    FCTL1 = 0x0A500;   // ERASE = 0
    FCTL3 = 0x0A510;   // Lock = 1
  }

  // Copia de EEPROM externa a CODE
  for(p = (unsigned int *)OFFSET_CODE; p<(unsigned int *)OFFSET_CODE2; p++)
  {
    FCTL3 = 0x0A500;   // Lock = 0
    FCTL1 = 0x0A540;   // WRT = 1
    framRead((unsigned int)p, (unsigned char *)&w, 2);
    *p = w;
    FCTL1 = 0x0A500;   // WRT = 0
    FCTL3 = 0x0A510;   // Lock = 1
  }
  
  // Borra el segmento del vector de interrupción
  for(p = (unsigned int *)OFFSET_INTVECT; p>=(unsigned int *)OFFSET_INTVECT; p+=256)
  {
    FCTL3 = 0x0A500;   // Lock = 0
    FCTL1 = 0x0A502;   // ERASE = 1
    *p = 0;
    FCTL1 = 0x0A500;   // ERASE = 0
    FCTL3 = 0x0A510;   // Lock = 1
  }

  // Copia el segmento que contiene el vector de interrupción
  for(p = (unsigned int *)OFFSET_INTVECT; p >= (unsigned int *)OFFSET_INTVECT; p++)
  {
    FCTL3 = 0x0A500;   // Lock = 0
    FCTL1 = 0x0A540;   // WRT = 1
    framRead((unsigned int)p, (unsigned char *)&w, 2);
    *p = w;
    FCTL1 = 0x0A500;   // WRT = 0
    FCTL3 = 0x0A510;   // Lock = 1
  }

  f=(void (*)(void))(*(unsigned int *)0x0FFFE);
  f();                        // salta adonde apunta el vector de reset
}

//------------------------------------------------------------------------------
// Escribe una palabra en la flash
//------------------------------------------------------------------------------
void flash_ww( unsigned int *Data_ptr, unsigned int word )
{
  asm("DINT");
  FCTL3 = 0x0A500;  // Lock = 0
  FCTL1 = 0x0A540;  // WRT = 1
  *Data_ptr=word;   // program Flash word
  FCTL1 = 0x0A500;  // WRT = 0
  FCTL3 = 0x0A510;  // Lock = 1
  asm("EINT");
}

//------------------------------------------------------------------------------
// Borra un segmento en la flash
//------------------------------------------------------------------------------
void flash_clr( unsigned int *Data_ptr )
{
  asm("DINT");
  FCTL3 = 0x0A500;  // Lock = 0
  FCTL1 = 0x0A502;  // WRT = 1
  *Data_ptr=0;      // dummy write
  FCTL1 = 0x0A500;  // WRT = 0
  FCTL3 = 0x0A510;  // Lock = 1
  asm("EINT");
}

//------------------------------------------------------------------------------
// Inicializa el controlador de la flash para la escritura
//------------------------------------------------------------------------------
void InitFlashCtl()
{
  FCTL2 = 0x0A500 | FSSEL0 | 4;     // ACLK (2 MHz) / 5 = 400 KHz
}

