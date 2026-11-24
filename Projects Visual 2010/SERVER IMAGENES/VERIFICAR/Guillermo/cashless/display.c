//----------------------------------------
// display.c
// Funciones para manejar el display LCD
//----------------------------------------

#include <string.h>

#include "cashless.h"
#include "tcpip.h"
#include "display.h"
#include "ks0108.h"
#include "support.h"
#include "msp430x14x.h"
#include "mgmt.h"
#include "bitmap.h"

//----------------------------------------
// Enviar un comando al display LCD
//----------------------------------------
void LCD_command(unsigned char cmd)
{
  P4OUT &= ~LCD_RS;
  P2OUT = cmd;
  P4OUT |= LCD_ENA;
  asm("nop");
  asm("nop");
  P4OUT &= ~LCD_ENA;
  asm("nop");
  asm("nop");
  P4OUT |= LCD_RS;  
  DelayCycles(500);
}

//----------------------------------------
// Enviar un caracter al display LCD
//----------------------------------------
void LCD_write(unsigned char data)
{
  P4OUT |= LCD_RS;
  P2OUT = data;
  P4OUT |= LCD_ENA;
  asm("nop");
  asm("nop");
  P4OUT &= ~LCD_ENA;
  DelayCycles(500);
}

//----------------------------------------
// Borrar el display LCD
//----------------------------------------
void LCD_clear()
{
  LCD_command(0x01);

  DelayCycles(13200);    // 1.65 ms
}

//---------------------------------------------------
// Posicionar el cursor del display LCD en el inicio
//---------------------------------------------------
void LCD_home()
{
  LCD_command(0x02);

  DelayCycles(13200);    // 1.65 ms
}

//------------------------------------------------------
// Posicionar el cursor del display LCD en la 2da línea
//------------------------------------------------------
void LCD_line2()
{
  LCD_command(0x0C0);
}

//------------------------------------------------------
// Imprime un mensaje en el display, discriminando si
// es LCD de 2x16 caracteres, 128x64 píxels o una
// PC con touch screen
//------------------------------------------------------
void LCD_print(const TOUMSG * msg)
{
  int line;
  switch( Features.display )
  {
  case DISPLAY_KS0108:
    ks0108ClearScreen();
//    if( msg == &MSG_MAQUINA_LIBRE )
//    {
//      ks0108Bitmap(BMP_MAQUINA_LIBRE);
//    }else{
      line = ks0108Puts(0, (char *)msg->txt[0]);
      ks0108Puts(++line, (char *)msg->txt[1]);
//    }
    break;
  case DISPLAY_2x16:
  case FEATURES_DEFAULT:
    LCD_puts(1,msg->txt[0]);
    LCD_puts(2,msg->txt[1]);
    LCD_backlight_on();    // enciende la luz de fondo del display LCD
    break;
  case DISPLAY_IP:
    touchMsgBuffer.msgCode  = TOUCH_DISPLAY;
    memcpy(&touchMsgBuffer.display, msg, sizeof(TOUMSG));
    touchMsgBuffer.hash = RANDOM();
    touchMsgBuffer.crc = CRC((unsigned char *)&touchMsgBuffer,sizeof(touchMsgBuffer)-2,0);
    sendUDP( (unsigned char *) &touchMsgBuffer, sizeof(touchMsgBuffer), SERV_TOUCH, UDP_LOCAL_PORT_CLI, UDP_PORT_TOUCH );
    break;
  }
}

//-----------------------------------------------
// Envía una cadena de caracteres al display LCD
//-----------------------------------------------
void LCD_puts( int line, const char * s )
{
  int i;

  if( Features.display == DISPLAY_KS0108 )
  {
    ks0108Puts(line, (char *)s);
  }else{
    if( line == 1 ) LCD_home(); else LCD_line2();
    
    for(i=0; i<16 && s[i] && s[i]!=13; ++i)
      LCD_write( s[i] );
    
    while( i++ < 16 )
      LCD_write(' ');      // completa con espacios hasta el final del renglón
  }
}

//----------------------------------------
// Inicializa el display LCD
//----------------------------------------
void LCD_init()
{
  switch(Features.display)
  {
  case DISPLAY_KS0108:
    ks0108_init();
    break;
  case DISPLAY_2x16:
  case FEATURES_DEFAULT:
    delay(_100ms);		// 100ms delay after power-up
  
    LCD_command(0x30);		// Function set		
     delay(_100ms);                // Delay more than 39us
  
    LCD_command(0x30);		// Function set		
    delay(_100ms);	        // Delay more than 39us
  
    LCD_command(0x30);		// Function set		
    delay(_100ms);		// Delay more than 39us
  
    LCD_command(0x3C);		// 1/16 duty cycle.  5x11 font
    delay(_100ms);                // Delay more than 39us
  
    LCD_command(0x0C);            // Disp on, Blink off, Cursor off
    delay(_100ms);                // Delay more than 39us
    
    LCD_command(0x01);            // Clear/Home
    delay(2);                     // Delay more than 1.53ms
    
    LCD_command(0x06);            // Increment right
    delay(2);                     // Delay more than 1.53ms	
    
    P1OUT |= LCD_BACKLIGHT; // enciende la luz de fondo del display LCD
    break;
  }
}
