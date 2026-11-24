//--------------------------------------------------
// ks0108.c
// Funciones para manejar el display gráfico 128x64
// basado en el controlador KS0108
//--------------------------------------------------

#include <stdio.h>
#include <string.h>

#include "cashless.h"
#include "tcpip.h"
#include "ks0108.h"
#include "support.h"
#include "msp430x14x.h"
#include "mgmt.h"
#include "fonts.h"
#include "bitmap.h"
#include "graficos.h"

#define toUpper(c)    ((c>='a' && c<='z')?c-'a'+'A':c)
#define underScore(c) ((c=='_')?'Z'+1:c);

// Tipos
typedef struct {
	unsigned char x;
	unsigned char y;
	unsigned char page;
}KS0108Coord;

// Variables estáticas
static KS0108Coord ks0108Coord;

// Macros
#define delay125ns()  asm("nop")
#define delay250ns()  {delay125ns();delay125ns();}
#define delay500ns()  {delay250ns();delay250ns();}

#define ks0108DrawVertLine(x, y, length, color) ks0108FillRect(x, y, 0, length, color)
#define ks0108DrawHoriLine(x, y, length, color) ks0108FillRect(x, y, length, 0, color)

// Puertos usados para el control del display

// bits del puerto P4 para controlar el display gráfico
#define R_W   0x01   // bit 0: R/W - Obs.: no se usa - está fijado a cero (write) por defecto
#define D_I   0x02   // bit 1: Data / Instruction
#define ENA   0x04   // bit 2: Enable

// bits del puerto P6 para controlar el display gráfico
#define CS1   0x10   // bit 4: CS1
#define CS2   0x20   // bit 5: CS2
#define RST   0x40   // bit 6: Reset

// bit 7 del puerto P1 para controlar la luz de fondo del LCD
#define BACKLIGHT 0x80

// Macros para encender y apagar el backlight
#define ks0108_backlight_off()  P1OUT &= ~BACKLIGHT
#define ks0108_backlight_on()   P1OUT |= BACKLIGHT

// Comandos para manejo del display
// Comandos
#define LCD_ON          0x3F
#define LCD_OFF         0x3E
#define LCD_SET_ADD     0x40
#define LCD_SET_PAGE    0xB8
#define LCD_DISP_START  0xC0

// Colores
#define BLACK           0xFF
#define WHITE           0x00

// Fonts
//static const unsigned char A[2][8]={{0xf8,0x1c,0x06,0x03,0x06,0x1c,0xf8,0x00},
//                                    {0x3f,0x03,0x03,0x03,0x03,0x03,0x3f,0x00}};

//----------------------------------------
// Inicializa el display gráfico
//----------------------------------------
void ks0108_init()
{
  P4OUT &= ~ENA;          // por defecto Enable deshabilitado
  
  P6OUT &= ~RST;          // pone reset en bajo
  delay125ns();
  P6OUT |= RST;           // pone reset en bajo
    
  ks0108Coord.x = 0;
  ks0108Coord.y = 0;
  ks0108Coord.page = 0;
  
  ks0108_backlight_on();

  ks0108WriteCommand(LCD_ON, 1);          // power on
  ks0108WriteCommand(LCD_ON, 2);
  
  ks0108WriteCommand(LCD_DISP_START, 1);  // display start line = 0
  ks0108WriteCommand(LCD_DISP_START, 2);
  
  ks0108Bitmap(LOGO);
  delay(_3seg);

  ks0108ClearScreen();

//ks0108Icono(40,32,graf_no_fumar);
//delay(_5seg);
}

//------------------------------------------------
// Escribe un comando en uno de los controladores
//------------------------------------------------
void ks0108WriteCommand(unsigned char cmd, unsigned char chip)
{
  if(chip == 1)
  {
    P6OUT &= ~CS2;            // deselecta chip 2
    P6OUT |= CS1;             // selecta chip 1
  }else{
    P6OUT &= ~CS1;            // deselecta chip 1
    P6OUT |= CS2;             // selecta chip 2
  }
  
  P4OUT &= ~D_I;              // D/I = 0 (instruction)
  P2OUT = cmd;                // escribe comando

  delay500ns();

  P4OUT |= ENA;
  delay500ns();
  delay500ns();
  P4OUT &= ~ENA;
  delay500ns();
  delay500ns();
}

//------------------------------------------------
// Escribe un dato
//------------------------------------------------
void ks0108WriteData(unsigned char data)
{
  if(ks0108Coord.x < 64)
  {
    P6OUT &= ~CS2;            // deselecta chip 2
    P6OUT |= CS1;             // selecta chip 1
  }else{
    P6OUT &= ~CS1;            // deselecta chip 1
    P6OUT |= CS2;             // selecta chip 2
  }

  P4OUT |= D_I;               // D/I = 1 (data)
  P2OUT = data;               // escribe dato
  
  delay500ns();

  P4OUT |= ENA;
  delay500ns();
  delay500ns();
  P4OUT &= ~ENA;
  delay500ns();
  delay500ns();

  if( ++ks0108Coord.x == 64 )
    ks0108GotoXY( ks0108Coord.x, ks0108Coord.y );
  else if( ks0108Coord.x >= 128 )
    ks0108GotoXY( 0, ks0108Coord.y + 8 );
}

//------------------------------------------------
//
//------------------------------------------------
void ks0108GotoXY(unsigned char x, unsigned char y)
{
  unsigned char chip = 1, cmd;
  
  x = x % 128;            // asegura que las coordenadas sean válidas
  y = y % 64;

  ks0108Coord.x = x;      // almacena las nuevas coordenadas
  ks0108Coord.y = y;
  ks0108Coord.page = y/8;
  
  if(x >= 64)
  {                       // selecciona el chip que corresponda
    x -= 64;
    chip = 2;
  }
  cmd = LCD_SET_ADD | x;

  ks0108WriteCommand(cmd, chip);               // set x address on active chip
  
  cmd = LCD_SET_PAGE | ks0108Coord.page;       // set y address on both chips

  ks0108WriteCommand(cmd, 1);
  ks0108WriteCommand(cmd, 2);
}

//------------------------------------------------
// Lee el estado del controlador
//------------------------------------------------
void ks0108ClearScreen()
{
  int i,j,k;
  for(k=0;k<128;k+=64)
  {
    for(j=0; j<64; j+=8)
    {
      ks0108GotoXY(k,j);
      for(i=0; i<64; ++i)
        ks0108WriteData(0x00);
    }
  }
  ks0108GotoXY(0,0);
}
        
//------------------------------------------------
// 
//------------------------------------------------
void ks0108Putchar(int x, int y, char c)
{
  static int i, j;
    
  // Como usa un subconjunto de ASCII, convierte a mayúsculas
  // y reubica el '_'
  c = toUpper(c);
  c = underScore(c);
  
  switch( Features.font )
  {
  case FONT_7x12:
    // Font 7x12
    j=14*(c-0x20);
    ks0108GotoXY(x,y);
    for(i=0; i<7; ++i)
      ks0108WriteData(Font7x12[j+i]);
    ks0108WriteData(0);
    ks0108GotoXY(x,y+8);
    for(i=7; i<14; ++i)
      ks0108WriteData(Font7x12[j+i]);
    ks0108WriteData(0);
    break;
  case FONT_8x12:
    // Font 8x12
    j=16*(c-0x20);
    ks0108GotoXY(x,y);
    for(i=0; i<8; ++i)
      ks0108WriteData(Font8x12[j+i]);
    ks0108GotoXY(x,y+8);
    for(i=8; i<16; ++i)
      ks0108WriteData(Font8x12[j+i]);
    break;
  default:
  case FONT_8x14:
    // Font 8x14
    j=16*(c-0x20);
    ks0108GotoXY(x,y);
    for(i=0; i<8; ++i)
      ks0108WriteData(Font8x14[j+i]);
    ks0108GotoXY(x,y+8);
    for(i=8; i<16; ++i)
      ks0108WriteData(Font8x14[j+i]);
    break;
  case FONT_8x14_BOLD:
    // Font 8x14
    j=16*(c-0x20);
    ks0108GotoXY(x,y);
    for(i=0; i<8; ++i)
      ks0108WriteData(Font8x14Bold[j+i]);
    ks0108GotoXY(x,y+8);
    for(i=8; i<16; ++i)
      ks0108WriteData(Font8x14Bold[j+i]);
    break;
  case FONT_10x15:
    // Font 10x15
    j=20*(c-0x20);
    ks0108GotoXY(x,y);
    for(i=0; i<10; ++i)
      ks0108WriteData(Font10x15[j+i]);
    ks0108GotoXY(x,y+8);
    for(i=10; i<20; ++i)
      ks0108WriteData(Font10x15[j+i]);
    break;
  case FONT_12x16:
    // Font 12x16
    j=24*(c-0x20);
    ks0108GotoXY(x,y);
    for(i=0; i<24; i+=2)
      ks0108WriteData(Font12x16[j+i]);
    ks0108GotoXY(x,y+8);
    for(i=1; i<24; i+=2)
      ks0108WriteData(Font12x16[j+i]);
    break;
  }
}

//------------------------------------------------
// 
//------------------------------------------------
int ks0108Puts(int line, char * s)
{
  int i, j, n, m, w;

  // Saltea los espacios iniciales
  while( *s == ' ' )
    ++s;
  
  switch(Features.font)
  {
  case FONT_12x16:
    w=10;
    m=12;
    break;
  case FONT_10x15:
    w=12;
    m=10;
    break;
  default:
    w=16;
    m=8;
    break;
  }
  
  n = strlen(s);
  if( n && n<=w )
  {
    // el texto cabe completo en un renglón
    for(i=0; s[i] && i<16; ++i)
    {
      switch(Features.font)
      {
      case FONT_10x15:
        ks0108Putchar(10*i, line*16, s[i]);
        break;
      case FONT_12x16:
        ks0108Putchar(12*i, line*16, s[i]);
        break;
      default:
        ks0108Putchar(8*i, line*16, s[i]);
        break;
      }
    }
  }else if( n>w ){
    // el texto debe ir en dos renglones
    // busca un espacio anterior a los 10 caracteres,
    for( n = n-1; n && (n>w || s[n]!=' '); --n);
    
    if( s[n] == ' ' )
    {
      // el texto anterior al espacio va en la línea inicial
      for(i=0; i<n; ++i)
      {
        ks0108Putchar(m*i, line*16, s[i]);
      }
      ++line;
      // el texto que sigue al espacio va en la línea siguiente
      for(i=n+1, j=0; s[i] && j<w; ++i, ++j)
      {
        ks0108Putchar(m*j, line*16, s[i]);
      }
    }else{
      // no hay espacio --> escribe la palabra hasta el límite del renglón
      for(i=0; s[i] && i<w; ++i)
      {
        ks0108Putchar(m*i, line*16, s[i]);
      }
    }
  }
  return line;
}

//--------------------------------------------------
// Presenta un bitmap de 128x64 en el display
//--------------------------------------------------
void ks0108Bitmap(unsigned char const * const bmp)
{
  int i;
  
  ks0108GotoXY(0,0);
  
  for(i=0; i<1024; ++i)
    ks0108WriteData(bmp[i]);
}

//--------------------------------------------------
// Presenta un ícono en el display
// bmp[0] indica el ancho del gráfico en píxeles
// bmp[1] indica la altura del gráfico en píxeles
//--------------------------------------------------
void ks0108Icono(int x, int y, unsigned char const * const bmp)
{
  int i, j;
  
  ks0108GotoXY(x,y);
  
  for(i=0; i < bmp[1]/8; ++i)
  {
    ks0108GotoXY(x, y+8*i);
    
    for(j=0; j < bmp[0]; ++j)
      ks0108WriteData(bmp[ bmp[0] * i + j + 2 ] );
  }
}

