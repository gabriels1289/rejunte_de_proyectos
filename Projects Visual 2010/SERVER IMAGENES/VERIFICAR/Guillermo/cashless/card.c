//-------------------------------------------------
// card.c
// Implementa el manejo de las tarjetas chip
//-------------------------------------------------

#include "msp430x14x.h"
#include "support.h"
#include "card.h"

// Máscaras para leer y escribir bits para el manejo de la tarjeta

#define CARD_IN  0x02    // Para testear el puerto P1.1 (en 0 indica tarjeta presente)
#define CARD_DI  0x04    // Para leer dato entrante en el puerto P1.2
#define CARD_DO  0x08    // Para escribir dato saliente en el puerto P4.3
#define CARD_CLK 0x10    // Para manipular el clock en el puerto P4.4
#define CARD_RST 0x20    // Para manipular el reset en el puerto P4.5

// Macros para manejar las operaciones con la tarjeta

#define RESET_H()   (P4OUT |= CARD_RST)
#define RESET_L()   (P4OUT &= ~CARD_RST)
#define CLK_H()     (P4OUT |= CARD_CLK)
#define CLK_L()     (P4OUT &= ~CARD_CLK)
#define READ()      ((P1IN & CARD_DI)?0:1)
#define WRITE0()    (P4OUT |= CARD_DO)
#define WRITE1()    (P4OUT &= ~CARD_DO)

//-----------------------------------------------------------
// Funciones para manejar el lector de tarjetas
//-----------------------------------------------------------

//------------------------------------------------------
// Retorna el estado del lector de tarjetas chip
// 0 --> no hay tarjeta
// 1 --> hay una tarjeta colocada en el lector
//------------------------------------------------------
unsigned char getCardStatus()
{
  // CARD_IN es normalmente abierto,
  // se pone a uno cuando hay una tarjeta presente
  if(P1IN & CARD_IN)
    return 1;         // Tarjeta presente
  else
    return 0;         // No hay tarjeta presente en el lector
}

//------------------------------------------------------
// Efectúa el reset y answer-to-reset de la tarjeta
// Deposita los 32 bits leidos en cardID
// Retorna 1 si tuvo éxito, 0 si falló la operación
//------------------------------------------------------
unsigned char cardAnswerToReset(unsigned char * cardID)
{
  int i,j;
  
  if( getCardStatus() )
  {
    RESET_H();
    DelayCycles(50);    // 6.25 us
    CLK_H();
    DelayCycles(100);   // 12.5 us
    CLK_L();
    DelayCycles(50);    // 6.25 us
    RESET_L();
    DelayCycles(50);    // 6.25 us
    for(i=0;i<4;++i)
    {
      cardID[i]=0;
      
      for(j=0; j<8; ++j)
      {
        if( READ() )
          cardID[i] |= (1 << j);
        CLK_H();
        DelayCycles(100);
        CLK_L();
        DelayCycles(100);
      }
    }
  }
  return getCardStatus();
}

//-----------------------------------------------------------
// Hace un break para interrumpir una secuencia
//-----------------------------------------------------------
void cardBreak()
{
  CLK_L();
  DelayCycles(50);    // 6.25 us
  RESET_H();
  DelayCycles(50);    // 6.25 us
  RESET_L();
  DelayCycles(50);    // 6.25 us
}

//-----------------------------------------------------------
// Leer "nBytes" de la memoria indicada, a partir de "from"
//-----------------------------------------------------------

unsigned char cardRead(unsigned char cmd, unsigned char *buf, unsigned char from, unsigned char nBytes )
{
  int i, j;
  
  if( getCardStatus() )
  {
    WRITE1();
    CLK_H();
    DelayCycles(100);   // 12.5 us
    WRITE0();
    DelayCycles(100);   // 12.5 us
    CLK_L();
    
    // Envía comando
    for(i=0; i<8; ++i)
    {
      if( cmd & (1<<i) )
        WRITE1();
      else
        WRITE0();
      DelayCycles(100);   // 12.5 us
      CLK_H();
      DelayCycles(100);   // 12.5 us
      CLK_L();
    }

    // Envía dirección inicial
    for(i=0; i<8; ++i)
    {
      if( from & (1<<i) )
        WRITE1();
      else
        WRITE0();
      DelayCycles(100);   // 12.5 us
      CLK_H();
      DelayCycles(100);   // 12.5 us
      CLK_L();
    }
    
    // Envía 3er byte (no importa)
    for(i=0; i<8; ++i)
    {
      WRITE0();
      DelayCycles(100);   // 12.5 us
      CLK_H();
      DelayCycles(100);   // 12.5 us
      CLK_L();
    }
    
    DelayCycles(100);   // 12.5 us
    CLK_H();
    DelayCycles(100);   // 12.5 us
    WRITE1();
    DelayCycles(200);   // 12.5 us
    CLK_L();
    DelayCycles(100);   // 12.5 us

    for(i=0; i<nBytes; ++i)
    {
      buf[i]=0;
      
      for(j=0; j<8; ++j)
      {
        if( READ() )
          buf[i] |= (1 << j);
        CLK_H();
        DelayCycles(100);
        CLK_L();
        DelayCycles(100);
      }
    }
  }
  
  cardBreak();
  
  return getCardStatus();
}

//-------------------------------------------------------------------
// Escribir un byte en la dirección indicada de la memoria principal
//-------------------------------------------------------------------

unsigned char cardWriteByte( unsigned char cmd, unsigned char data, unsigned char addr )
{
  int i;
  
  if( getCardStatus() )
  {
    WRITE1();
    DelayCycles(100);   // 12.5 us
    CLK_H();
    DelayCycles(100);   // 12.5 us
    WRITE0();
    DelayCycles(100);   // 12.5 us
    CLK_L();
    
    // Envía comando
    for(i=0; i<8; ++i)
    {
      if( cmd & (1<<i) )
        WRITE1();
      else
        WRITE0();
      DelayCycles(100);   // 12.5 us
      CLK_H();
      DelayCycles(100);   // 12.5 us
      CLK_L();
    }

    // Envía dirección
    for(i=0; i<8; ++i)
    {
      if( addr & (1<<i) )
        WRITE1();
      else
        WRITE0();
      DelayCycles(100);   // 12.5 us
      CLK_H();
      DelayCycles(100);   // 12.5 us
      CLK_L();
    }
    
    // Envía dato
    for(i=0; i<8; ++i)
    {
      if( data & (1<<i) )
        WRITE1();
      else
        WRITE0();
      DelayCycles(100);   // 12.5 us
      CLK_H();
      DelayCycles(100);   // 12.5 us
      CLK_L();
    }
    
    DelayCycles(100);   // 12.5 us
    CLK_H();
    DelayCycles(100);   // 12.5 us
    WRITE1();
    DelayCycles(200);   // 12.5 us
    CLK_L();
    DelayCycles(100);   // 12.5 us
    WRITE0();
    DelayCycles(100);   // 12.5 us

    for(i=0; i<255 && !READ(); ++i)
    {
      CLK_H();
      DelayCycles(100);
      CLK_L();
      DelayCycles(100);
    }
  }
  return getCardStatus();
}

//----------------------------------------------------------------
// Escribir "nBytes" en la memoria principal, a partir de "from"
//----------------------------------------------------------------

unsigned char cardWrite( unsigned char *buf, unsigned char from, unsigned char nBytes )
{
  int i;
  unsigned char sec[4], mask = 1;
  
  cardRead(CMD_READ_SECU, sec, 0, 4);
  if( sec[0] & 7 )
  {
    for (i=0; i<3; ++i)
    {
      if(sec[0] & mask) break;
      mask <<= 1;
    }
    sec[0] ^= mask; 
    cardWriteByte( CMD_WRITE_SECU, sec[0], 0 );
    cardWriteByte( CMD_COMP_VERIF, 0xFF, 1 );
    cardWriteByte( CMD_COMP_VERIF, 0xFF, 2 );
    cardWriteByte( CMD_COMP_VERIF, 0xFF, 3 );
    cardWriteByte( CMD_WRITE_SECU, 0xFF, 0 );
    cardRead(CMD_READ_SECU, sec, 0, 4);
    
    for(i=0; i<nBytes; ++i)
      cardWriteByte( CMD_WRITE_MAIN, buf[i], from+i );
  }
  return getCardStatus();
}
