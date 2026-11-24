//-----------------------------------------------------
// mgmt.c
// Funciones para el manejo de mensajes de gestión.
// Manejo de la actualización de firmware.
// Rutinas de test.
//-----------------------------------------------------

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "msp430x14x.h"
#include "support.h"
#include "cs8900.h"
#include "tcpip.h"
#include "cashless.h"
#include "flash.h"
#include "fram.h"
#include "mgmt.h"
#include "display.h"
#include "ks0108.h"
#include "card.h"
#include "md5.h"

const char version[20]={"5.04"};
char newVersion[20];

const char *protocol[4]={"SAS","DACOM","GAMMA","EIBE"};

// características opcionales configurables por software,
// que se almacenan en la FLASH de datos

#pragma location=0x1080
__no_init FeaturesType Features;

// Punteros a los buffers de Tx y Rx UDP
static MgmtMsgType * const pr = (MgmtMsgType *) UDP_RX_BUF;
static MgmtMsgType * const pt = (MgmtMsgType *) UDP_TX_BUF;

volatile unsigned int waitCodeSegTimer;// Timer para esperar un segmento de código pedido para actualizar el programa
unsigned int waitingCodeSeg;           // Offset inicial del segmento de código que está esperando (0xFFFF no espera)

static void updateIP_MAC(void);
static void setFeatures(void);
static void getNewPgm(void);
static void saveNewCodeSeg(void);

//-------------------------------------------------
// Procesamiento de mensaje de gestión recibido
//-------------------------------------------------
void processMgmtMsg()
{
  unsigned int i;
  void (*f)(void);

  if( CRC((unsigned char *)pr, UDPRxDataCount, MGMT_CRC_SEED ) )
     return;

  // Inicializa el buffer de transmisión con ceros, por si tiene que enviar una respuesta
  for(i=0; i<sizeof(pt->msg1); ++i)
    ((unsigned char *)pt)[i] = 0;
  
  switch(pr->msg1.op)
  {
  case MGMT_GET_VERSION:
    // Prepara y envía mensaje informando versión de software
    pt->msg1.op = MGMT_SEND_VERSION;
    strcpy(pt->msg1.version, version);
    pt->msg1.crc = CRC( (unsigned char *)pt, sizeof(pt->msg1)-2, MGMT_CRC_SEED );
    sendUDP((unsigned char *)pt, sizeof(pt->msg1), SERV_MGMT, UDP_LOCAL_PORT_MGMT, UDP_PORT_MGMT);
    break;
    
  case MGMT_SET_VERSION:
    // Inicia el proceso de reemplazar el código por una nueva versión
    if( UDPRxDataCount == sizeof(pr->msg1) )
    {
      strcpy(newVersion, pr->msg1.version);
      getNewPgm();
    }
    break;
    
  case MGMT_SET_IP_MAC:
    // Actualiza IP y MAC del módulo y reinicia el Controlador Ethernet
    if( UDPRxDataCount == sizeof(pr->msg1) )
      updateIP_MAC();      
    break;
    
  case MGMT_SEND_CODESEG:
    // Recibió un segmento de código. Lo debe almacenar en un área temporaria
    // para luego usarlo en el reemplazo de todo el software
    if(UDPRxDataCount == sizeof(pr->msg2))
      saveNewCodeSeg();
    break;
    
  case MGMT_SEND_BITMAP:
    if( !getCardStatus() && !waitingCashinReply && !waitingCashoutReply )
    {
      // Recibió la mitad de un bitmap. Lo presenta en el display
      if(pr->msg2.offset == 0)
        ks0108GotoXY(0,0);
      else
        ks0108GotoXY(0,32);
      for(i=0; i<512; ++i)
        ks0108WriteData(pr->msg2.data[i]);
    }
    break;
    
  case MGMT_GET_FEATURES:
    // Prepara y envía mensaje informando las features configuradas en el módulo
    pt->msg3.op = MGMT_SEND_FEATURES;
    memcpy(&pt->msg3.features, &Features, sizeof(FeaturesType));
    pt->msg3.crc = CRC( (unsigned char *)pt, sizeof(pt->msg3)-2, MGMT_CRC_SEED );
    sendUDP((unsigned char *)pt, sizeof(pt->msg3), SERV_MGMT, UDP_LOCAL_PORT_MGMT, UDP_PORT_MGMT);
    break;
    
  case MGMT_SET_FEATURES:
    // Recibió un conjunto de nuevas features a configurar en el módulo
    if(UDPRxDataCount == sizeof(pr->msg3))
      setFeatures();
    break;

  case MGMT_DELE_FEATURES:
    // Recibió la orden de borrar las features y volver a los valores por defecto
    flash_clr((unsigned int *)&Features);
    
    // Reinicia el módulo para que tengan efecto los cambios
    f=(void (*)(void))(*(unsigned int *)0x0FFFE);
    f();                        // salta adonde apunta el vector de reset

    break;

  case MGMT_GET_HASH_MD5:
    // Prepara y envía mensaje informando versión de software
    MD5(pr->msg4.seed, pr->msg4.seedLen, pt->msg4.digest);
    pt->msg4.op = MGMT_SEND_HASH_MD5;
    pt->msg4.crc = CRC( (unsigned char *)pt, sizeof(pt->msg4)-2, MGMT_CRC_SEED );
    sendUDP((unsigned char *)pt, sizeof(pt->msg4), SERV_MGMT, UDP_LOCAL_PORT_MGMT, UDP_PORT_MGMT);
    break;

  case MGMT_GET_HASH_CRC16:
    // Prepara y envía mensaje informando versión de software
    i = CRC((unsigned char *)0x1100, (unsigned int)0xffff-0x1100+1, (unsigned int)pr->msg4.seed[0] << 8 | pr->msg4.seed[1]);
    pt->msg4.op = MGMT_SEND_HASH_CRC16;
    pt->msg4.digest[0] = i >> 8;
    pt->msg4.digest[1] = i & 0x00ff;
    pt->msg4.crc = CRC( (unsigned char *)pt, sizeof(pt->msg4)-2, MGMT_CRC_SEED );
    sendUDP((unsigned char *)pt, sizeof(pt->msg4), SERV_MGMT, UDP_LOCAL_PORT_MGMT, UDP_PORT_MGMT);
    break;
  }
}

//------------------------------------------------------------------
// Actualiza IP y MAC del módulo y reinicia el Controlador Ethernet
//------------------------------------------------------------------
static void updateIP_MAC()
{
  // Borra el segmento correspondiente en la flash de información
  flash_clr(&MyCfgIP[0]);
 
  // Asigna nueva IP
  MyIP[0] = pr->msg1.IP[0] + (unsigned int)(pr->msg1.IP[1] << 8);
  MyIP[1] = pr->msg1.IP[2] + (unsigned int)(pr->msg1.IP[3] << 8);

#if 1  // Esto va para la versión final
  // Asigna la nueva IP de la PC con touch screen asociada al módulo
  ServerAddr[SERV_TOUCH].IP[0] = MyIP[0];
  ServerAddr[SERV_TOUCH].IP[1] = MyIP[1] + 100;
#endif  
  // Graba la nueva IP en la flash de información
  flash_ww( &MyCfgIP[0], MyIP[0] );
  flash_ww( &MyCfgIP[1], MyIP[1] );
 
  // Asigna nueva MAC
  MyMAC[0] = tiempo.ticks << 1;  // pone en cero el bit de multicast
  MyMAC[1] = MyIP[0];
  MyMAC[2] = MyIP[1];

  // Graba la nueva MAC en la flash de información
  flash_ww( &MyCfgMAC[0], MyMAC[0] );
  flash_ww( &MyCfgMAC[1], MyMAC[1] );
  flash_ww( &MyCfgMAC[2], MyMAC[2] );
   
  // Reinicia el controlador Ethernet para que tome la nueva MAC
  InitEthernet();
}

//------------------------------------------------------------------
// Configura las features del módulo
//------------------------------------------------------------------
static void setFeatures()
{
  int i;
  void (*f)(void);
  
  // Borra el segmento correspondiente en la flash de información
  flash_clr((unsigned int *)&Features);
  
  // Graba las nuevas Features recibidas
  for(i=0; i<sizeof(FeaturesType); ++i)
    flash_ww( ((unsigned int *)&Features)+i, *(((unsigned int *)&pr->msg3.features)+i) );

  // Reinicia el módulo para que tengan efecto los cambios
  f=(void (*)(void))(*(unsigned int *)0x0FFFE);
  f();                        // salta adonde apunta el vector de reset
}

//--------------------------------------------------------------
// Obtiene una nueva imagen del programa, lo graba en la flash
// de código y reinicia el módulo
//--------------------------------------------------------------
static void getNewPgm()
{
  // Si ya está en proceso de obtener la nueva imagen, descarta el mensaje
  if( waitingCodeSeg != 0xFFFF )
    return;
  
  // Luego pide el primer segmento de la nueva imagen.
  // Si bien la memoria de programa comienza en 0x1100, para comenzar
  // en una dirección múltiplo de 512 (0x0200), comienza pidiendo
  // desde 0x1000. Luego, cuando grabe en la flash, lo hará desde 0x1100
  // desde 0x1100, porque entre 0x1000 y 0x10ff está la flash usada
  // para almacenar datos y parámetros.
  
  pt->msg1.op = MGMT_GET_CODESEG;
  strcpy(pt->msg1.version, newVersion);
  pt->msg1.offset = OFFSET_FLASH;
  pt->msg1.crc = CRC( (unsigned char *)pt, sizeof(pt->msg1)-2, MGMT_CRC_SEED );
  sendUDP((unsigned char *)pt, sizeof(pt->msg1), SERV_MGMT, UDP_LOCAL_PORT_MGMT, UDP_PORT_MGMT);

  waitingCodeSeg = OFFSET_FLASH;
  waitCodeSegTimer = 0;
}

//--------------------------------------------------------------
// Guarda el nuevo segmento de código recibido, en el segmento
// correspondiente del área de almacenamiento temporario
//--------------------------------------------------------------
static void saveNewCodeSeg()
{
  unsigned int i, j, addr;
  unsigned char *pb, chkBuf[16];
  
  // Verifica que esté esperando un segmento de código, 
  // que coincida la versión recibida y el offset del segmento,
  // si no descarta el mensaje.
  if( waitingCodeSeg == 0x0FFFF )
  {
    return;                        // No está esperando ningún segmento
  }
  
  if( strcmp( pr->msg2.version, newVersion ) )
  {
    waitingCodeSeg = 0x0FFFF;      // Versión incorrecta, aborta el proceso
    return;
  }

  if( waitingCodeSeg != pr->msg2.offset )
  {
    waitingCodeSeg = 0x0FFFF;      // No es el segmento que está esperando, aborta el proceso
    return;
  }

  // Guarda el fragmento recibido en la memoria eeprom externa,
  // a partir del mismo offset que le correspondería en la flash
  
  // Como cada escritura en la eeprom puede ser de hasta 128 bytes,
  // dentro de la misma página, la escritura la realiza en 4 pasos

  // dirección inicial en la EEPROM
  addr = pr->msg2.offset;
  // puntero a los datos (nuevo segmento de código) en el mensaje
  pb = (unsigned char *)pr->msg2.code;

  for(i=0; i<4; ++i, pb+=128, addr+=128)
  {
    framWrite(addr, pb, 128);
  }
  
  // Verifica que los datos grabados en la memoria estén correctos,
  // si no lo están aborta el proceso
  
  // dirección inicial en la EEPROM
  addr = pr->msg2.offset;
  // puntero a los datos (nuevo segmento de código) en el mensaje
  pb = (unsigned char *)pr->msg2.code;
  
  for(i=0; i<32; ++i, pb+=16, addr+=16)
  {
    framRead(addr, chkBuf, 16);
    for(j=0; j<16; ++j)
    {
      if(pb[j] != chkBuf[j])
      {
        // Error -> aborta el proceso e informa al servidor
        pt->msg1.op = MGMT_ABORT_DWNLD;
        strcpy(pt->msg1.version, newVersion);
        pt->msg1.offset = waitingCodeSeg;
        pt->msg1.crc = CRC( (unsigned char *)pt, sizeof(pt->msg1)-2, MGMT_CRC_SEED );
        sendUDP((unsigned char *)pt, sizeof(pt->msg1), SERV_MGMT, UDP_LOCAL_PORT_MGMT, UDP_PORT_MGMT);
        return;
      }
    }
  }
  // Si corresponde, pide un próximo segmento
  if( pr->msg2.offset == OFFSET_INTVECT )
  {
    // Recibió la imagen completa, ordena instalarla y reiniciar el procesador
    stopWatchdog();
    flashPgm();
  }else{
    // Pide el siguiente segmento
    pt->msg1.op = MGMT_GET_CODESEG;
    strcpy(pt->msg1.version, newVersion);
    waitingCodeSeg += 512;
    pt->msg1.offset = waitingCodeSeg;
    pt->msg1.crc = CRC( (unsigned char *)pt, sizeof(pt->msg1)-2, MGMT_CRC_SEED );
    sendUDP((unsigned char *)pt, sizeof(pt->msg1), SERV_MGMT, UDP_LOCAL_PORT_MGMT, UDP_PORT_MGMT);
    waitCodeSegTimer = 0;
  }
}

//-----------------------------------------------------------
// Lazo de ejecución para cuando se inicia el módulo
// en modo test (reset con el pulsador test apretado)
//-----------------------------------------------------------
void testLoop(void)
{
  // Procesa la recepción y transmisión por Ethernet / IP
  
  BEEP(_200ms);
  LCD_print(&MSG_ENTRANDO_MODO_TEST);
  delay(_1seg);

  // Presenta dirección IP y Nro de versión
  toumsg->cod = 0;
  if( MyCfgIP[0]==0xffff && MyCfgIP[1]==0xffff )
    sprintf(toumsg->txt[0], "%u.%u.%u.%u", MyIP[0]&0xff, (MyIP[0]>>8)&0xff, MyIP[1]&0xff, (MyIP[1]>>8)&0xff);
  else
    sprintf(toumsg->txt[0], "%u.%u.%u.%u", MyCfgIP[0]&0xff, (MyCfgIP[0]>>8)&0xff, MyCfgIP[1]&0xff, (MyCfgIP[1]>>8)&0xff);

  sprintf(toumsg->txt[1], "%s/%s", protocol[(P6IN & SELE_PROTO) >> 1], version);
  LCD_print(toumsg);
  delay(_3seg);

  // Presenta dirección IP y Nro de versión
  toumsg->cod = 0;
  sprintf(toumsg->txt[0], "DEBUG Y GESTION:");
  sprintf(toumsg->txt[1], "%u.%u.%u.%u", ServerAddr[SERV_MGMT].IP[0]&0xff,
                                        (ServerAddr[SERV_MGMT].IP[0]>>8)&0xff,
                                         ServerAddr[SERV_MGMT].IP[1]&0xff,
                                        (ServerAddr[SERV_MGMT].IP[1]>>8)&0xff);
  LCD_print(toumsg);
  delay(_2seg);

  while( testMode )
  {
    WDTimer = 0;      // reinicia timer auxiliar del watchdog

    if( TEST_PRESSED() )
    {
      pollingTimer = 0;
      delay(_400ms);
      while( TEST_PRESSED() )
      {
        if( pollingTimer == _500ms )
          BEEP(_200ms);
        if( pollingTimer == _1500ms )
          BEEP(_200ms);
      }        
      if( pollingTimer < _1seg )
      {
        testMode = 0;       // sale del loop y vuelve al modo normal
        TESTLED_OFF();
      }
    }        
    // Es lo único que hace en modo test, a fin de permitir la
    // recepción de comandos de prueba desde la red
    DoNetworkStuff();
  }
  BEEP(_200ms);
  LCD_print(&MSG_SALIENDO_MODO_TEST);
  delay(_1500ms);
  if( getCardStatus() == cardStatus && !doorOpen )
  {
    if( cardStatus )
      LCD_print(&MSG_MAQUINA_EN_JUEGO);
    else
      LCD_print(&MSG_MAQUINA_LIBRE);
  }else if(doorOpen) {
    LCD_print(&MSG_PUERTA_ABIERTA);
  }
}

//-----------------------------------------------------------
// Ejecuta algún test solicitado en forma remota por UDP
// Obs.: usa para armar las respuestas el mismo buffer de
// recepción UDP
// También modifica el valor de UDPRxDataCount para indicar
// la longitud de la respuesta
//-----------------------------------------------------------
void remoteTest()
{
  unsigned int i, msgLen=UDPRxDataCount;
  char * msg = (char *)UDP_RX_BUF;

  LCD_clear();
  
  if(UDPRxDataCount < 8)
  {
    sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
    return;
  }
  
  // Convierte el comando a mayúsculas
  for(i=1; i<8; ++i)
    msg[i] = toupper(msg[i]);

  // Reemplaza el '>' del comando por el '<' a usar en la respuesta
  msg[0] = '<';
  
  // Pruebas del display LCD
  
  if( !strncmp( msg, "<PNTLCDA,", 9 ) )
  {
    test_PNTLCDA();

  }else if( !strncmp( msg, "<PNTLCDH,", 9 ) )
  {
    // Imprime dígitos hexa codificados en ASCII
      
  }else if( !strncmp( msg, "<CLRDLCD", 8 ) )
  {
    // Borra el display
    LCD_clear();
    sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
    
  }else if( !strncmp( msg, "<BACKLON", 8 ) )
  {
    // Enciende el backlight
    LCD_backlight_on();
    sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
  
  }else if( !strncmp( msg, "<BACKLOF", 8 ) )
  {
    // Apaga el backlight
    LCD_backlight_off();
    sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
    
  }else if( !strncmp( msg, "<WRRDCOM", 8 ) )
  {
    // Prueba de la comunicación serie
    test_WRRDCOM();

  }else if( !strncmp( msg, "<RDJMPTS", 8 ) )
  {
    test_RDJMPTS();
    
  }else if( !strncmp( msg, "<WREEMEM,", 9 ) )
  {
    // Prueba de escritura de una página en memoria EEPROM externa
    test_WREEMEM();

  }else if( !strncmp( msg, "<RDEEMEM,", 9 ) )
  {
    // Prueba de lectura de una página de memoria EEPROM externa
    test_RDEEMEM();
    
  }else if( !strncmp( msg, "<CLEEMEM,", 9 ) )
  {
    // Borrado de una página de memoria externa (graba FF)
    test_CLEEMEM();
    
  }else if( !strncmp( msg, "<RSTCARD", 8 ) )
  {
    // Resetea la tarjeta y lee el mensaje de respuesta al reset (32 bytes)
    test_RSTCARD();
      
  }else if( !strncmp( msg, "<WRICARD,", 9 ) )
  {
    // Escribe una secuencia de bytes en la smart card
    test_WRICARD();

  }else if( !strncmp( msg, "<REACARD,", 9 ) )
  {
    // Lee una secuencia de bytes de la smart card
    test_REACARD();

  }else if( !strncmp( msg, "<RDCRDSW", 8 ) )
  {
    // Testea si hay una tarjeta insertada
    if( getCardStatus() ) strcpy(msg+8, ",1\r"); else strcpy(msg+8, ",0\r");

    sendUDP( (unsigned char *)msg, 11, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );

  }else if( !strncmp( msg, "<WBUZZER,", 9 ) )
  {
    // Testea el buzzer
    test_WBUZZER();
    
  }else if( !strncmp( msg, "<TESTLED,", 9 ) )
  {
    // Testea el led de test
    test_TESTLED();
    
  }else if( !strncmp( msg, "<TESTART", 8 ) )
  {
    // Ingresa al modo test
    testMode = 1;
    sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
    
  }else if( !strncmp( msg, "<TESTEND", 8 ) )
  {
    // Sale del modo test
    testMode = 0;
    sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
    
  }else if( !strncmp( msg, "<GETVERS", 8 ) )
  {
    // Retorna la versión del firmware
    sprintf(msg+8, ",%s\r", version);
    msgLen = strlen(msg);
    sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
    
  }else if( !strncmp( msg, "<GETIPAD", 8 ) )
  {
    // Retorna la versión del firmware
    sprintf(msg+8, ",%u.%u.%u.%u\r", MyCfgIP[0]&0xff, (MyCfgIP[0]>>8)&0xff, MyCfgIP[1]&0xff, (MyCfgIP[1]>>8)&0xff);
    msgLen = strlen(msg);
    sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
    
  }else if( !strncmp( msg, "<TESTRST", 8 ) )
  {
    // Reinicia el módulo
    sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
    delay(_100ms);
    void (*f)(void);
    f=(void (*)(void))(*(unsigned int *)0x0FFFE);
    f();                        // salta adonde apunta el vector de reset
    
  }else if( !strncmp( msg, "<POLLING", 8 ) )
  {
    // Consulta o modifica el período de polling para SAS
    test_POLLING_PERIOD();
      
  }else if( !strncmp( msg, "<TSTPAUX", 8 ) )
  {
    // Chequea el estado de los puertos auxiliares.
    // P6.4 a P6.7 salidas; P1.0, P1.3 a P1.5 entradas
    test_AUX_PORTS();

  }else if( !strncmp( msg, "<DEBUGON", 8 ) )
  {
    // activa el envío de mensajes de debug al puerto de test del servidor de mgmt
    debug = 1;
    sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );

  }else if( !strncmp( msg, "<DEBUGOF", 8 ) )
  {
    // activa el envío de mensajes de logs al puerto de test del servidor de mgmt
    debug = 0;
    sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );

  }else{
    // Comando inválido, retorna mensaje de error
    strcpy(msg, "<ERROR\r");
    msgLen = strlen(msg);
    sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
  }
}

//------------------------------------------------
// Convierte hexa codificado en ASCII a hexa
//------------------------------------------------
unsigned char ascii2hexa(unsigned char a)
{
  unsigned char i;
  for(i=0; i<16; ++i)
    if( a == hexa[i] )
      return i;
  return 0xff;
}

//------------------------------------------------
// convierte hexa codificado en ASCII a hexa
//------------------------------------------------

//------------------------------------------------
// Imprime un texto en ASCII en el display
//------------------------------------------------
static void test_PNTLCDA()
{
  unsigned int i;
  char * msg = (char *)UDP_RX_BUF;

  // Primero busca el retorno de carro
  for(i=9; i<42; ++i)
    if( msg[i] == 13 )
      msg[i] = 0;
  
  LCD_clear();
  LCD_puts(1, msg+9 );
  if( i > 25 )
  {
    LCD_puts(2, msg+25 );
  }
  msg[i] = 13;
  
  sendUDP( (unsigned char *)msg, i+1, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
}      

//-------------------------------------------------------------
// Lee el estado de los jumpers J1 y J2, y el pulsador de test
//-------------------------------------------------------------
static void test_RDJMPTS()
{
  unsigned int msgLen=UDPRxDataCount;
  char * msg = (char *)UDP_RX_BUF;
  
  // Lectura del estado de jumpers y pulsador de test
  msg[8]  = ',';
  msg[9]  = '0' + (( P6IN & 0x02 ) >>1 );      // J1
  msg[10] = ',';
  msg[11] = '0' + (( P6IN & 0x04 ) >>2 );      // J2
  msg[12] = ',';
  msg[13] = '0' + (~P6IN & 0x01);              // Test
  msg[14] = 13;                                // CR
  msgLen = 15;

  sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
}

//-------------------------------------------------------------
// Testea la comunicación serie
//-------------------------------------------------------------
static void test_WRRDCOM()
{
  unsigned int i, n, msgLen=UDPRxDataCount;
  unsigned char h, l, c;
  char * msg = (char *)UDP_RX_BUF;

  if( msg[8] == ',' )
  {
    // El comando contiene la lista de valores hexa codificados en ASCII a transmitir
    // Transmite byte a byte y espera el eco de cada uno
    if(msgLen > MAX_UDP_TX_DATA_SIZE)
    {
      msgLen = MAX_UDP_TX_DATA_SIZE;
      msg[msgLen - 1] = 13;
    }
      
    for(i=9; i < (msgLen-1); i+=2)
    {
      if( isxdigit(msg[i]) ) h = ascii2hexa( msg[i] ); else break;
      if( isxdigit(msg[i]) ) l = ascii2hexa( msg[i+1] ); else break;
      c = (h << 4)| (l & 0x0f);
      
      UART0_tx(c, 0);
      if( !UART0_rx(1) || rx_buffer[0] != c )
      {
        // No se recibió respuesta o es incorrecta
        msg[i] = msg[i+1] = '*';
      }
    }
  }else if( msg[8] == 13 )
  {
    // El comando no tiene argumentos
    // Transmite 256 bytes, con valores desde 00 a FF e informa la cantidad de errores
    for(i=0, n=0; i<256; ++i)
    {
      UART0_tx( (unsigned char)i, 0);
      if( !UART0_rx(1) || rx_buffer[0] != i )
        ++n;
    }
    sprintf( msg, "<WRRDCOM,%-u\r", n);
    msgLen = strlen(msg);
  }    
  sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
}

//-------------------------------------------------------------
// Prueba de escritura en la memoria EEPROM externa
//-------------------------------------------------------------
static void test_WREEMEM()
{
  unsigned int i, j, k, n, addr, msgLen=UDPRxDataCount;
  unsigned char h, l;
  char * msg = (char *)UDP_RX_BUF;
  
  // Escritura de una página en memoria externa
  // Obtiene el Nro de página
  sscanf( msg+9, "%u", &addr );
  // busca el comienzo de la cadena de caracteres
  if(addr<10)
    k = 10;   // la coma o el CR deben estar en la posición 10
  else if (addr < 100)
    k = 11;
  else
    k = 12;
  
  // Convierte el nro de página en una dirección
  addr *= 128;
  
  if( msg[k] == ',' )
  {
    // Convierte a hexa
    for( i=k+1, j=k+1, n=0; n<128 && j<msgLen; ++i, ++n )
    {
      if( isxdigit(msg[j]) ) h = ascii2hexa( msg[j++] ); else break;
      if( isxdigit(msg[j]) ) l = ascii2hexa( msg[j++] ); else break;
      msg[i] = (h << 4)| (l & 0x0f);
    }
    framWrite( addr, (unsigned char *)msg+k+1, n );
    // calcula el CRC del string grabado, para poder verficar
    // posteriormente la correctitud de la grabación
    i=CRC((unsigned char *)msg+k+1, n, 0);
    // Lee para verificar que la escritura haya sido exitosa
    framRead( addr, (unsigned char *)msg+k+1, n );
    j=CRC((unsigned char *)msg+k+1, n, 0);
    sprintf( msg+k, ",%-u\r", i-j );
    msgLen = strlen(msg);
  }else if( msg[k] == 13 )
  {
    // Primero graba 0x5A
    for(j=0; j<128; ++j)
      msg[k+1+j] = 0x5A;
    framWrite( addr, (unsigned char *)msg+k+1, 128 );
    framRead( addr, (unsigned char *)msg+k+1, 128 );
    for(j=0, n=0; j<128; ++j)
      if( msg[k+1+j] != 0x5A )
        ++n;
    if(n)
    {
      sprintf( msg+k, ",%-u\r", n );
      msgLen = strlen(msg);
    }else{
      // Luego graba 0xA5
      for(j=0; j<128; ++j)
        msg[k+1+j] = 0xa5;
      framWrite( addr, (unsigned char *)msg+k+1, 128 );
      framRead( addr, (unsigned char *)msg+k+1, 128 );
      for(j=0, n=0; j<128; ++j)
        if( msg[k+1+j] != 0xA5 )
          ++n;
      sprintf( msg+k, ",%-u\r", n );
      msgLen = strlen(msg);
    }
  }else
  {
    strcpy(msg, "<WREEMEM,ERROR EN COMANDO\r");
    msgLen = strlen(msg);
  }
  sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
}

//-------------------------------------------------------------
// Prueba de lectura en la memoria EEPROM externa
//-------------------------------------------------------------
static void test_RDEEMEM()
{
  unsigned int i, j, k, n, addr, msgLen=UDPRxDataCount;
  char * msg = (char *)UDP_RX_BUF;
  
  // Obtiene el Nro de página
  sscanf( msg+9, "%u", &addr );
  // busca donde debe ir la coma
  if(addr<10) k = 10;
  else if (addr < 100) k = 11;
  else k = 12;
  msg[k++] = ',';
  
  // Convierte el nro de página en una dirección
  addr *= 128;
  
  framRead( addr, (unsigned char *)msg+k, 128 );
  // Codifica el hexa en ascii
  j = k + 255;
  i = k + 127;
  
  msg[j+1] = 13;
  msgLen = j + 2;
  
  for(n=0; n<128; ++n)
  {
    msg[j--] = hexa[ msg[i] & 0x0f ];
    msg[j--] = hexa[ msg[i--] >> 4 ];
  }
  // Envía la respuesta fraccionada
  msg[0] = '<';
  
  for(i=0; i<msgLen; i+=128)
  {
    if( (msgLen - i) > 128 ) n = 128; else n = msgLen - i;
    
    sendUDP( (unsigned char *)msg+i, n, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
  }
}

//-------------------------------------------------------------
// Borrado de una página de la memoria EEPROM externa
//-------------------------------------------------------------
static void test_CLEEMEM()
{
  unsigned int i, addr, msgLen=UDPRxDataCount;
  char * msg = (char *)UDP_RX_BUF;
  
  sscanf( msg+9, "%u", &addr );
  addr *= 128;
  
  for(i=0; i<128; ++i)
    msg[msgLen + i] = 0xff;
  
  framWrite( addr, (unsigned char *)msg + msgLen, 128 );

  sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
}

//-------------------------------------------------------------
// Resetea una smart card y obtiene el answer-to-reset
//-------------------------------------------------------------
static void test_RSTCARD()
{
  unsigned int i, j, n, msgLen=UDPRxDataCount;
  char * msg = (char *)UDP_RX_BUF;
  
  msg[8] = ',';
  // Resetea la tarjeta y lee el mensaje de respuesta al reset (32 bytes)
  cardAnswerToReset((unsigned char *)msg + 9);

  // Codifica el hexa en ascii
  j = 16;
  i = 12;
  
  msg[j+1] = 13;
  msgLen = j + 2;
  
  for(n=0; n<4; ++n)
  {
    msg[j--] = hexa[ msg[i] & 0x0f ];
    msg[j--] = hexa[ msg[i--] >> 4 ];
  }
  
  sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
}

//-------------------------------------------------------------
// Escribe una secuencia de bytes en la smart card
//-------------------------------------------------------------
static void test_WRICARD()
{
  unsigned int i, j, k, n, addr, msgLen=UDPRxDataCount;
  unsigned char h, l;
  char * msg = (char *)UDP_RX_BUF;
  
  // Obtiene la dirección inicial
  sscanf( msg+9, "%u", &addr );
  // busca el comienzo de la cadena de caracteres
  if(addr<10)
    k = 10;   // la coma o el CR deben estar en la posición 10
  else if (addr < 100)
    k = 11;
  else
    k = 12;
  
  if( addr>=32 && msg[k] == ',' )
  {
    // Convierte a hexa
    for( i=k+1, j=k+1, n=0; n<224 && j<msgLen; ++i, ++n )
    {
      if( isxdigit(msg[j]) ) h = ascii2hexa( msg[j++] ); else break;
      if( isxdigit(msg[j]) ) l = ascii2hexa( msg[j++] ); else break;
      msg[i] = (h << 4)| (l & 0x0f);
    }
    cardWrite((unsigned char *)&msg[k+1], addr, n);
    msg[k] = 13;
    msgLen = k+1;
  }else{
    strcat( msg+k+1, "COMANDO ERRÓNEO\r" );
    msgLen = strlen(msg);
  }
  sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
}

//-------------------------------------------------------------
// Lee n datos de una smart card
//-------------------------------------------------------------
static void test_REACARD()
{
  unsigned int i, j, k, n, addr, msgLen=UDPRxDataCount;
  char * msg = (char *)UDP_RX_BUF;
  
  sscanf(msg+9, "%u,%u", &addr, &n);
  
  k = msgLen - 1;
  
  msg[k++] = ',';
  
  // Resetea la tarjeta y lee el mensaje de respuesta al reset (32 bytes)
  cardRead(CMD_READ_MAIN, (unsigned char *)msg + k, addr, n);

  // Codifica el hexa en ascii
  j = k + 2 * n - 1;
  i = k + n - 1;
  
  msg[j+1] = 13;
  msgLen = j + 2;
  
  for(k=0; k<n; k++)
  {
    msg[j--] = hexa[ msg[i] & 0x0f ];
    msg[j--] = hexa[ msg[i--] >> 4 ];
  }
  sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
}

//----------------------------------------------
// Prueba el buzzer
//----------------------------------------------
static void test_WBUZZER()
{
  unsigned int n, msgLen=UDPRxDataCount;
  char * msg = (char *)UDP_RX_BUF;
  
  sscanf(msg+9, "%u", &n);

  switch(n)
  {
  case 0:
    BUZZER_OFF();
    break;
  case 255:
    BUZZER_ON();
    break;
  default:
    BUZZER_ON();
    delay(n);
    BUZZER_OFF();
    break;
  }
  sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
}

//----------------------------------------------
// Prueba el led de test
//----------------------------------------------
static void test_TESTLED()
{
  unsigned int n, msgLen=UDPRxDataCount;
  char * msg = (char *)UDP_RX_BUF;
  
  sscanf(msg+9, "%u", &n);

  switch(n)
  {
  case 0:
    TESTLED_OFF();
    break;
  case 255:
    TESTLED_ON();
    break;
  default:
    TESTLED_ON();
    delay(n);
    TESTLED_OFF();
    break;
  }
  sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
}

//------------------------------------------------------
// Consulta o modifica el período de polling para SAS
//------------------------------------------------------
static void test_POLLING_PERIOD()
{
  unsigned int n, msgLen=UDPRxDataCount;
  char * msg = (char *)UDP_RX_BUF;
  
  if(msg[8]==',')
  {
    sscanf(msg+9,"%u", &n);
    if(n>=40 && n<=2000)
      pollingPeriod = n/10;
    sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
  }else if(msg[8]==13)
  {
    // Retorna el período de polling
    sprintf(msg+8, ",%u\r", pollingPeriod*10);
    msgLen = strlen(msg);
    sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
  }
}

//------------------------------------------------------
// Chequea el estado de los puertos auxiliares
// (deben estar puenteados)
//------------------------------------------------------
static void test_AUX_PORTS()
{
  unsigned int n, msgLen=UDPRxDataCount;
  char * msg = (char *)UDP_RX_BUF;
  
  P6OUT = P6IN & 0x0f;
  delay(2);
  n = P1IN;
  n=((n >> 2) | (n&1))& 0x0f;
  sprintf(msg+8, ",%02X", n);
  
  P6OUT = P6IN | 0xf0;
  delay(2);
  n = P1IN;
  n=((n >> 2) | (n&1))& 0x0f;
  sprintf(msg+strlen(msg), ",%02X\r", n);
  
  msgLen = strlen(msg);
  sendUDP( (unsigned char *)msg, msgLen, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
}
