// Funciones que implementan la transmisión y recepción
// serial asincrónica por software.
// Transmite por P6.7
// Recibe por P1.4 a 9600-8-N-1

#include <stdio.h>
#include <string.h>
#include "msp430x14x.h"
#include "cashless.h"
#include "support.h"
#include "serial.h"
#include "display.h"
#include "ks0108.h"

#define KBD_BACKLIGHT_ON()  (P4OUT |= 0x01)
#define KBD_BACKLIGHT_OFF() (P4OUT &= 0xfe)

//--------------------------------------------------
// Espera el ingreso de un caracter desde el teclado
// conectado via serie a 9600-8-N-1
//--------------------------------------------------
char Getchar()
{
  int i;
  char b = 0;
  
  delayTimer = 0;   // Usa este timer para un timeout entre caracteres
  
  while( delayTimer < _10seg && (P1IN & 0x10) )
  {
    // Mientras espera la llegada de un caracter, sigue enviando 0x80
    // para que la VGM SAS no se desincronice
    if( (vgmProto==SAS || vgmProto==EIBE) && pollingTimer > pollingPeriod )
    {
      pollingTimer = 0;
      UART0_tx(0x80,1);
    }
  }

  if( delayTimer >= _10seg )
    return 13;                // Sale por timeout --> envía CR

  DelayCycles(416);  // Supone que es el bit de start --> espera el tiempo de medio bit
  
  if( !(P1IN & 0x10) )
  {
    // Deshabilita las interrupciones durante la recepción de un byte, porque la
    // atención de una interrupción puede alterar el temporizado entre bits.
    __disable_interrupt();
    
    // bit de start --> demora el tiempo de un bit para ir a sensar el primer bit
    DelayCycles(832);

    for(i=0; i<8; ++i)
    {
      if( P1IN & 0x10 )
        b |= (1 << i);    // si hay un "1", setea el bit correspondiente

      DelayCycles(832);   // demora hasta el siguiente bit
    }

    // Después del 8vo bit espera el bit de stop
    // Si la línea está en alto --> bit de stop: OK --> retorna byte recibido
    // Si no, error de entramado, retorna 0
    if( !(P1IN & 0x10) )
      b = 0;

    __enable_interrupt();
  }
  return b;
}

//---------------------------------------------------------
// Ingresa por teclado serie una cadena de caracteres,
// de longitud máxima <n>. Lo muestra en el display en
// el renglón <y>
// Si pwd es distinto de cero, no muestra los caracteres
// ingresados, sino '*'
//---------------------------------------------------------
int Gets(char * s, int n, int y, int pwd)
{
  int i=0;
  char spwd[9];
  unsigned int wdtEnabled = watchdogEnabled();

  if( wdtEnabled )
    stopWatchdog();

  KBD_BACKLIGHT_ON();
    
  s[0] = 0;
  spwd[0] = 0;

  sprintf(s,"_%*s",n,"");  // Borra el espacio de edición
  LCD_puts(y,s);
  
  while(i < n)
  {
    s[i] = Getchar();

    if( s[i] == 27 )
    {
      i = 0;
      sprintf(s,"_%*s",n,"");
      LCD_puts(y,s);
      s[0] = 0;
      spwd[0] = 0;
    }else if( s[i] == 13 ) {
      s[i] = 0;
      spwd[0] = 0;
      if( !pwd )
        LCD_puts(y,s);
      else
        LCD_puts(y,spwd);
      break;
    }else if( s[i] >= '0' && s[i] <= '9' ) {
      spwd[i++] = '*';
      s[i] = 0;
      spwd[i] = 0;
      strcat(s,"_");
      if( !pwd )
        LCD_puts(y,s);
      else
        LCD_puts(y,spwd);
    }
  }

  KBD_BACKLIGHT_OFF();
    
  if( wdtEnabled )
  {
    WDTimer = 0;
    startWatchdog();
  }  
  return i;
}
