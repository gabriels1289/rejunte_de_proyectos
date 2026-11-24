//-------------------------------------------------------------
// fram.c
// Implementa el manejo de la memoria EEPROM serial 24LC512
//-------------------------------------------------------------

#include "msp430x14x.h"
#include "support.h"
#include "fram.h"

#define FRAM_DATA 0x40   // Para leer / escribir dato serial en el puerto P4.6
#define FRAM_CLK  0x80   // Para manipular el clock de la FRAM en el puerto P4.7

#define FRAM_DIN()  (P4DIR &= ~FRAM_DATA)    // Configura P4.6 como entrada
#define FRAM_DOUT() (P4DIR |= FRAM_DATA)     // Configura P4.6 como salida
#define CLK_H()     (P4OUT |= FRAM_CLK)
#define CLK_L()     (P4OUT &= ~FRAM_CLK)
#define WRITE1()    (P4OUT |= FRAM_DATA)
#define WRITE0()    (P4OUT &= ~FRAM_DATA)

#define READ()      (P4IN & FRAM_DATA)

#define delay125ns()  asm("nop")
#define delay250ns()  {delay125ns();delay125ns();}
#define delay500ns()  {delay250ns();delay250ns();}

#define FRAM_SLAVE_READ  0xA1   // Slave ID + Slave Addr + Read
#define FRAM_SLAVE_WRITE 0xA0   // Slave ID + Slave Addr + Write

#define START()  { WRITE1();      \
                   CLK_H();       \
                   WRITE0();      \
                   CLK_L();       \
                   delay500ns(); \
                 }

#define STOP()   { WRITE0();      \
                   CLK_L();       \
                   delay500ns(); \
                   CLK_H();       \
                   WRITE1();      \
                 }

//-----------------------------------------------------------
// Funciones y variables para manejar la FRAM (o serial RAM
// o serial EEPROM)
//-----------------------------------------------------------

//-----------------------------------------------------------
// Leer "nBytes" de la FRAM, a partir de "from"
//-----------------------------------------------------------
#pragma location="CODE2"

unsigned char framRead( unsigned int addr, unsigned char *buf, unsigned int nBytes )
{
  unsigned int i, j;
  unsigned char cmd[4], b;

  cmd[0]=FRAM_SLAVE_WRITE;
  cmd[1]=(addr >> 8);
  cmd[2]=(addr & 0x0FF);
  cmd[3]=FRAM_SLAVE_READ;
  
  FRAM_DOUT();
    
  for(i=0; i<4; ++i)
  {
    if(i==0 || i==3)
      START();
    
    // Envía comando
    for(j=0x100, b=cmd[i]; j=j>>1;)
    {
      if( b & j ) WRITE1(); else  WRITE0();
      CLK_H();
      delay125ns();
      CLK_L();
    }
    FRAM_DIN();

    // Espera ACK  
    CLK_H();
    if( READ() )
    {
      // No ACK --> Aborta la lectura
      // Envía stop y deja las líneas de clock y data en alto
      FRAM_DOUT();
      STOP();      
      return 0;
    }
    CLK_L();
    delay500ns();
    FRAM_DOUT();
  }
  
  // Obtiene los nBytes
  for(i=0; i<nBytes;)
  {
    FRAM_DIN();
    
    for(j=0x100, b=0; j=j>>1;)
    {
      CLK_H();
      if( READ() )
        b |= j;
      CLK_L();
      delay125ns();
    }
    buf[i] = b;
    
    FRAM_DOUT();

    if( ++i < nBytes )
    {
      // Envía ACK
      WRITE0();
    }else{
      // Último byte --> envía NAK
      WRITE1();
    }
    CLK_H();
    delay125ns();
    CLK_L();
    delay500ns();
  }    
  STOP();

  return 1;
}

//-------------------------------------------------------------
// Escribir "nBytes" de "data" en la FRAM, a partir de "addr".
// Obs.: nBytes puede estar entre 1 y 128, siempre que todo
// el rango de direcciones corresponda a la misma página.
//-------------------------------------------------------------
unsigned char framWrite( unsigned int addr, unsigned char *data, unsigned char nBytes )
{
  unsigned int i, j, n=nBytes+3;
  static unsigned char cmd[3], b;
  
  cmd[0]=FRAM_SLAVE_WRITE;
  cmd[1]=(addr >> 8);
  cmd[2]=(addr & 0x0FF);
  
  FRAM_DOUT();
    
  START();
  
  for(i=0; i<n; ++i)
  {
    // Envía comando (y dato)
    if(i<3)
      b = cmd[i];
    else
      b = data[i-3];

    for(j=0x100; j = j>>1; )
    {
      if( b & j ) WRITE1(); else  WRITE0();
      CLK_H();
      delay125ns();
      CLK_L();
    }
    FRAM_DIN();

    // Espera ACK  
    CLK_H();
    if( READ() )
    {
      // No ACK --> Aborta la lectura
      // Envía stop y deja las líneas de clock y data en alto
      FRAM_DOUT();
      STOP();      
      return 0;
    }
    CLK_L();
    FRAM_DOUT();
  }
  STOP();

  DelayCycles(40000);
  
  return 1;
}

