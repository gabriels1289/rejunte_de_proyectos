//--------------------------------------------------------
// dacom.c
// Implementa las funciones específicas para las máquinas
// con protocolo DACOM (Aristocrat australianas)
//--------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "msp430x14x.h"
#include "tcpip.h"
#include "cashless.h"
#include "card.h"
#include "display.h"
#include "dacom.h"

#undef DEBUG_DACOM_EVENTS

// Funciones para el manejo de las máquinas DACOM

static unsigned char frameXor(unsigned char *, unsigned int nBytes);
static void processFrame1(void);
static void processFrame2(void);
static void processFrame3(void);

static unsigned char eventos[7];      // Para almacenar los bytes que contienen información de eventos

static unsigned char evt_buf[MAX_EVENTOS]; // Buffer para almacenar eventos en espera de ser transmitidos
static unsigned char evt_buflen;           // Cantidad de eventos en el buffer de eventos

// Macros para ver si la VGM está jugando o no
// Se deben consultar después de llamar a processFrame1
#define VGM_READY()      ((eventos[0]&0x03)==0x01)
#define VGM_BUSY()       ((eventos[0]&0x03)==0x00)
#define VGM_PLAYING()    ((eventos[0]&0x03)==0x10)

//--------------------------------------------------------
// Inicializa variables usadas para Dacom
//--------------------------------------------------------
void initDacom()
{
  evt_buflen = 0;
  dacomHandpayPending = 0;
}

//--------------------------------------------------------
// Recepción y procesamiento de tramas enviadas por la VGM
// Retorna: 0 si no procesó ninguna trama
//          1 si procesó Trama 1
//          2 si procesó Trama 2
//          3 si procesó Trama 3
//--------------------------------------------------------
unsigned char dacomProcessing(void)
{
  if( rx_buflen==128 && !frameXor(rx_buffer, rx_buflen) )
  {
    if( rx_buffer[1]==0x00 ) //&& rx_buffer[2]==0x00 && rx_buffer[3]==0x01 )
    {
      // Trama tipo 1: contadores y eventos
      processFrame1();
      rx_buflen = 0;
      return 1;
    }else if( rx_buffer[1]==0x22 /* && rx_buffer[2]==0x01 && rx_buffer[3]==0x00 &&
              rx_buffer[7]==0x5A && rx_buffer[8]==0xA5 && rx_buffer[9]==0x00 */ && rx_buffer[10]==0x01 )
    {
      // Trama tipo 2: info de billetes y aceptador de billetes
      processFrame2();
      rx_buflen = 0;
      return 2;      
    }else if( rx_buffer[1]==0x22 /* && rx_buffer[2]==0x01 && rx_buffer[3]==0x00 &&
              rx_buffer[7]==0x5A && rx_buffer[8]==0xA5 && rx_buffer[9]==0x00 */ && rx_buffer[10]==0x02 )
    { 
      // Trama tipo 3: info del juego y créditos jugados
      processFrame3();
      rx_buflen = 0;
      return 3;      
    }
  }
  return 0;
}

//---------------------------------------------------
// Calcula la Xor de toda la trama
//---------------------------------------------------
static unsigned char frameXor(unsigned char *buf, unsigned int nBytes)
{
  unsigned char xor=0;
  unsigned int i;
  for(i=1; i<nBytes; ++i)
  {
    xor ^= buf[i];
  }
  return xor;
}

//---------------------------------------------------
// Procesa una trama de tipo 1
//---------------------------------------------------
static void processFrame1()
{
  unsigned char i, j;
  unsigned long cred;
  static unsigned long cred51=0;
  
  // games since last power up
  longPollBuffer.lp18[0] = rx_buffer[76];
  longPollBuffer.lp18[1] = rx_buffer[75];
  // games since last door open
  longPollBuffer.lp18[2] = rx_buffer[80];
  longPollBuffer.lp18[3] = rx_buffer[79];

  // denomination
  longPollBuffer.lp1f[5]  = rx_buffer[84] & 0x0F + 10 * (rx_buffer[84] >> 4) + 100 * (rx_buffer[85] & 0x0F);
    
  // current credits
  for(i=0, j=64; i<4; ++i, --j)
    longPollBuffer.lp1a[i] = rx_buffer[j];
  // como el mensaje viene en centavos, hay que dividirlo
  // por la denominación para obtener el valor en créditos  
  cred = bcd2long(longPollBuffer.lp1a,8) / bcd2long(&longPollBuffer.lp1f[5],2);
  long2bcd(longPollBuffer.lp1a,cred,8);
  
  // coin in
  for(i=0, j=19; i<4; ++i, --j)
    longPollBuffer.lp1c[i] = rx_buffer[j];
  // coin out
  for(j=24; i<8; ++i, --j)
    longPollBuffer.lp1c[i] = rx_buffer[j];
  // drop
  for(j=29; i<12; ++i, --j)
    longPollBuffer.lp1c[i] = rx_buffer[j];
  // jackpot
  for(j=34; i<16; ++i, --j)
    longPollBuffer.lp1c[i] = rx_buffer[j];
  
  // Obs.: games played y games won (en centavos) vienen en Trama3
  // Door open no fue encontrado en ninguna de las tramas
  
  // Power reset meter
  for(i=28, j=74; i<32; ++i, --j)
    longPollBuffer.lp1c[i] = rx_buffer[j];

  // True coin in
  for(i=0, j=54; i<4; ++i, --j)
    longPollBuffer.lp2a[i] = rx_buffer[j];

  // True coin out
  for(i=0, j=59; i<4; ++i, --j)
    longPollBuffer.lp2b[i] = rx_buffer[j];

#ifndef DEBUG_DACOM_EVENTS
  // Procesamiento a nivel de bits para detectar eventos
  // Byte 8
  if( eventos[0] != rx_buffer[8] )
  {
    // Analiza los bits 0 y 1, que indican si la VGM está jugando o no
    if( ((eventos[0] & 0x03) != (rx_buffer[8] & 0x03)) && evt_buflen < MAX_EVENTOS )
    {
      switch( rx_buffer[8] & 0x03 )
      {
      case 0x01:
        /*
        if( (eventos[0] & 0x03) == 0x02 )
          // Terminó una jugada
          evt_buf[evt_buflen++] = EVT_GAME_ENDED;
        */
        // Obs.: Si bien detecta el evento, no lo envía para no
        // sobrecargar al servidor con eventos que no interesan
        break;  
      case 0x02:
        /*
        Comenzó una jugada
        evt_buf[evt_buflen++] = EVT_GAME_STARTED;
        */
        // Obs.: Si bien detecta el evento, no lo envía para no
        // sobrecargar al servidor con eventos que no interesan
        break;  
      }
    }    
    // Analiza el bit 2, que indica si hay algún error de memoria
    if( ((eventos[0] & 0x04) != (rx_buffer[8] & 0x04))  && evt_buflen < MAX_EVENTOS )
    {
      if( rx_buffer[8] & 0x04 )
        evt_buf[evt_buflen++] = EVT_CMOS_RAM_BAD_DEVICE;
    }    
    // Analiza el bit 3, que indica que fue reseteado un error de memoria
    if( ((eventos[0] & 0x08) != (rx_buffer[8] & 0x08))  && evt_buflen < MAX_EVENTOS )
    {
      if( rx_buffer[8] & 0x08 )
        evt_buf[evt_buflen++] = EVT_MEMORY_ERROR_RESET;
    }    
    // Analiza el bit 4, que indica que finalizó una transferencia (cashin o cashout)
    if( ((eventos[0] & 0x10) != (rx_buffer[8] & 0x10))  && evt_buflen < MAX_EVENTOS )
    {
      if( rx_buffer[8] & 0x10 )
        evt_buf[evt_buflen++] = EVT_AFT_TRANSFER_COMPLETE;
    }    
    eventos[0] = rx_buffer[8];
  }
  
  // Byte 9
  if( eventos[1] != rx_buffer[9] )
  {
    // Analiza el bit 1, que significa cobrar pendiente (pago por Hopper)
//    if( ((eventos[0] & 0x02) != (rx_buffer[9] & 0x02))  && evt_buflen < MAX_EVENTOS )
//    {
//      if( rx_buffer[9] & 0x02 )
//      {
//        evt_buf[evt_buflen++] = EVT_????;
//      }
//    }    
    // Analiza el bit 2, que indica pago manual pendiente
    if( ((eventos[1] & 0x04) != (rx_buffer[9] & 0x04))  && evt_buflen < MAX_EVENTOS )
    {
      if( rx_buffer[9] & 0x04 )
      {
        cred51 = cred;
        evt_buf[evt_buflen++] = EVT_HANDPAY_PENDING;
        dacomHandpayPending = 1;
        LCD_print(&MSG_NO_PERMITIDO);
      }else{
        evt_buf[evt_buflen++] = EVT_HANDPAY_RESET;
        dacomHandpayPending = 0;
        if( !cred )
        {
          // Prepara el mensaje a enviar al servidor
          cashMsgBuffer.msgCode = CASHOUT_REQ;
          cashMsgBuffer.protocol = 'D';
          
          memcpy(cashMsgBuffer.lp1c, longPollBuffer.lp1c, 32);
          
          sprintf((char *)cashMsgBuffer.monto, "%010lu", cred51);
          cashMsgBuffer.hash = RANDOM();
          cashMsgBuffer.crc = CRC((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer)-2,0);
          cifrar((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer));
          
          // Envía mensaje al servidor, para que acredite el monto descargado
          sendUDP( (unsigned char *)&cashMsgBuffer, sizeof(cashMsgBuffer), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_CASH );
          waitingCashoutReply = 1;
          waitCashReplyTimer = 0;
        }else{
          LCD_print(&MSG_MAQUINA_EN_JUEGO);
        }
      }
    }    
    eventos[1] = rx_buffer[9];
  }

  // Byte 10
  if( eventos[2] != rx_buffer[10] )
  {
    // Analiza el bit 0, que indica puerta abierta o cerrada
    if( ((eventos[2] & 0x01) != (rx_buffer[10] & 0x01))  && evt_buflen < MAX_EVENTOS )
    {
      if( rx_buffer[10] & 0x01 )
      {
        evt_buf[evt_buflen++] = EVT_MAIN_DOOR_OPEN;
        LCD_print(&MSG_PUERTA_ABIERTA);
      }else{
        evt_buf[evt_buflen++] = EVT_MAIN_DOOR_CLOSED;
        if( cardStatus )
        {
          LCD_print(&MSG_MAQUINA_EN_JUEGO);
        }else{
          LCD_print(&MSG_MAQUINA_LIBRE);
        }
      }
    }    
    // Analiza el bit 5, que indica COIN IN TILT
    if( ((eventos[2] & 0x20) != (rx_buffer[10] & 0x20))  && evt_buflen < MAX_EVENTOS )
    {
      if( rx_buffer[10] & 0x20 )
        evt_buf[evt_buflen++] = EVT_COIN_IN_TILT;
    }    
    // Analiza el bit 6, que indica errores del hopper
    if( ((eventos[2] & 0x40) != (rx_buffer[10] & 0x40)) && evt_buflen < MAX_EVENTOS )
    {
      if( rx_buffer[10] & 0x40 )
        evt_buf[evt_buflen++] = EVT_HOPPER_LEVEL_LOW;
    }    
    eventos[2] = rx_buffer[10];
  }
  
  // Byte 11
  if( eventos[3] != rx_buffer[11] )
  {
    // Analiza el bit 0, que indica entrada al menú de auditoría
    if( ((eventos[3] & 0x01) != (rx_buffer[11] & 0x01)) && evt_buflen < MAX_EVENTOS )
    {
      if( rx_buffer[11] & 0x01 )
        evt_buf[evt_buflen++] = EVT_ATTENDANT_MENU_ENTER;
      else
        evt_buf[evt_buflen++] = EVT_ATTENDANT_MENU_EXIT;
    }    
    // Analiza el bit 1, que indica entrada al menu de self test
    if( ((eventos[3] & 0x02) != (rx_buffer[11] & 0x02)) && evt_buflen < MAX_EVENTOS )
    {
      if( rx_buffer[11] & 0x02 )
        evt_buf[evt_buflen++] = EVT_SELF_TEST_MENU_ENTER;
       else
        evt_buf[evt_buflen++] = EVT_SELF_TEST_MENU_EXIT;
    }    
    // Analiza el bit 2, que indica errores de power save
    if( ((eventos[3] & 0x04) != (rx_buffer[11] & 0x04)) && evt_buflen < MAX_EVENTOS )
    {
      if( rx_buffer[11] & 0x04 )
        evt_buf[evt_buflen++] = EVT_POWER_SAVE_ERROR;
    }    
    eventos[3] = rx_buffer[11];
  }
  // Envía el mensaje de eventos al servidor  (OJO -> completar / ver el manejo como cola de eventos)
  if( evt_buflen && !waitingEventAck )
  {
    for(i=0; i<evt_buflen; ++i)
      bufEventos.evento[i] = evt_buf[i];
        
    // Cantidad de eventos
    bufEventos.nEventos = evt_buflen;
    // Estampa fecha y hora
    sprintf(bufEventos.fechaHora,"%04d%02d%02d%02d%02d%02d",tiempo.anio,tiempo.mes,tiempo.dia,tiempo.hora,tiempo.min,tiempo.seg);
    // Agrega el código de hash
    bufEventos.hash = RANDOM();
    // Luego calcula el CRC
    bufEventos.crc = CRC((unsigned char *)&bufEventos,sizeof(EventMsgType)-2,0);
    // cifra el mensaje
    cifrar((unsigned char *)&bufEventos,sizeof(EventMsgType));
    // Envía el mensaje al servidor
    sendUDP( (unsigned char *) &bufEventos, sizeof(EventMsgType), SERV_EVENTS, UDP_LOCAL_PORT_CLI, UDP_PORT_EVENTS );
    // activa la espera del ACK
    waitingEventAck=1;
    waitEventAckTimer=0;
    
    // resetea el buffer de eventos para permitir el registro de nuevos
    // mientras espera el ACK a este mensaje
    evt_buflen = 0;
  }
  // Envía el mensaje de eventos al servidor  (OJO -> completar / ver el manejo como cola de eventos)
  if( evt_buflen && !waitingEventAck )
  {
    for(i=0; i<evt_buflen; ++i)
      bufEventos.evento[i] = evt_buf[i];
        
    // Cantidad de eventos
    bufEventos.nEventos = evt_buflen;
    // Estampa fecha y hora
    sprintf(bufEventos.fechaHora,"%04d%02d%02d%02d%02d%02d",tiempo.anio,tiempo.mes,tiempo.dia,tiempo.hora,tiempo.min,tiempo.seg);
    // agrega el código de hash
    bufEventos.hash = RANDOM();
    // Luego calcula el CRC
    bufEventos.crc = CRC((unsigned char *)&bufEventos,sizeof(EventMsgType)-2,0);
    // cifra el mensaje
    cifrar((unsigned char *)&bufEventos,sizeof(EventMsgType));
    // Envía el mensaje al servidor
    sendUDP( (unsigned char *) &bufEventos, sizeof(EventMsgType), SERV_EVENTS, UDP_LOCAL_PORT_CLI, UDP_PORT_EVENTS );
    // activa la espera del ACK
    waitingEventAck=1;
    waitEventAckTimer=0;
    
    // resetea el buffer de eventos para permitir el registro de nuevos
    // mientras espera el ACK a este mensaje
    evt_buflen = 0;
  }
#else
  // Para probar eventos Dacom
  i=0;
  if( eventos[0] != rx_buffer[8] )
  {
    eventos[0] = rx_buffer[8];
    bufEventos.evento[i++] = 0x18;
    bufEventos.evento[i++] = rx_buffer[4];
    bufEventos.evento[i++] = ((eventos[0] & 0x80)>>3) | ((eventos[0] & 0x40)>>6);
    bufEventos.evento[i++] = ((eventos[0] & 0x20)>>1) | ((eventos[0] & 0x10)>>4);
    bufEventos.evento[i++] = ((eventos[0] & 0x08)<<1) | ((eventos[0] & 0x04)>>2);
    bufEventos.evento[i++] = ((eventos[0] & 0x02)<<3) | (eventos[0] & 0x01);
  }
  if( eventos[1] != rx_buffer[9] )
  {
    eventos[1] = rx_buffer[9];
    bufEventos.evento[i++] = 0x19;
    bufEventos.evento[i++] = rx_buffer[4];
    bufEventos.evento[i++] = ((eventos[1] & 0x80)>>3) | ((eventos[1] & 0x40)>>6);
    bufEventos.evento[i++] = ((eventos[1] & 0x20)>>1) | ((eventos[1] & 0x10)>>4);
    bufEventos.evento[i++] = ((eventos[1] & 0x08)<<1) | ((eventos[1] & 0x04)>>2);
    bufEventos.evento[i++] = ((eventos[1] & 0x02)<<3) | (eventos[1] & 0x01);
  }
  if( eventos[2] != rx_buffer[10] )
  {
    eventos[2] = rx_buffer[10];
    bufEventos.evento[i++] = 0x1A;
    bufEventos.evento[i++] = rx_buffer[4];
    bufEventos.evento[i++] = ((eventos[2] & 0x80)>>3) | ((eventos[2] & 0x40)>>6);
    bufEventos.evento[i++] = ((eventos[2] & 0x20)>>1) | ((eventos[2] & 0x10)>>4);
    bufEventos.evento[i++] = ((eventos[2] & 0x08)<<1) | ((eventos[2] & 0x04)>>2);
    bufEventos.evento[i++] = ((eventos[2] & 0x02)<<3) | (eventos[2] & 0x01);
  }
  if( eventos[3] != rx_buffer[11] )
  {
    eventos[3] = rx_buffer[11];
    bufEventos.evento[i++] = 0x1B;
    bufEventos.evento[i++] = rx_buffer[4];
    bufEventos.evento[i++] = ((eventos[3] & 0x80)>>3) | ((eventos[3] & 0x40)>>6);
    bufEventos.evento[i++] = ((eventos[3] & 0x20)>>1) | ((eventos[3] & 0x10)>>4);
    bufEventos.evento[i++] = ((eventos[3] & 0x08)<<1) | ((eventos[3] & 0x04)>>2);
    bufEventos.evento[i++] = ((eventos[3] & 0x02)<<3) | (eventos[3] & 0x01);
  }
  if(i)
  {
    // Cantidad de eventos
    bufEventos.nEventos = i;
    // Estampa fecha y hora
    sprintf(bufEventos.fechaHora,"%04d%02d%02d%02d%02d%02d",tiempo.anio,tiempo.mes,tiempo.dia,tiempo.hora,tiempo.min,tiempo.seg);
    // Agrega el código de hash
    bufEventos.hash = RANDOM();
    // Luego calcula el CRC
    bufEventos.crc = CRC((unsigned char *)&bufEventos,sizeof(EventMsgType)-2,0);
    // cifra el mensaje
    cifrar((unsigned char *)&bufEventos,sizeof(EventMsgType));
    // Envía el mensaje al servidor
    if( sendUDP( (unsigned char *) &bufEventos, sizeof(EventMsgType), SERV_EVENTS, UDP_LOCAL_PORT_CLI, UDP_PORT_EVENTS ) )
    {
      // activa la espera del ACK
      waitingEventAck=1;
      waitEventAckTimer=0;
    }
  }
#endif
}

//---------------------------------------------------
// Procesa una trama de tipo 2
//---------------------------------------------------
static void processFrame2()
{
  unsigned char i, j;

  // Billetes de $ 5
  for(i=0, j=18; i<4; ++i, --j)
    longPollBuffer.lp33[i] = rx_buffer[j];
  // Billetes de $ 10
  for(i=0, j=23; i<4; ++i, --j)
    longPollBuffer.lp34[i] = rx_buffer[j];
  // Billetes de $ 20
  for(i=0, j=28; i<4; ++i, --j)
    longPollBuffer.lp35[i] = rx_buffer[j];
  // Billetes de $ 50
  for(i=0, j=33; i<4; ++i, --j)
    longPollBuffer.lp36[i] = rx_buffer[j];
  // Billetes de $ 100
  for(i=0, j=38; i<4; ++i, --j)
    longPollBuffer.lp37[i] = rx_buffer[j];

#ifndef DEBUG_DACOM_EVENTS
  // Procesamiento a nivel de bits para detectar eventos
  // Byte 11
  if( eventos[4] != rx_buffer[11] )
  {
    // Analiza el bit 0, que indica puerta de billetes abierta
    if( ((eventos[4] & 0x01) != (rx_buffer[11] & 0x01)) && evt_buflen < MAX_EVENTOS )
    {
      if( rx_buffer[11] & 0x01 )
        evt_buf[evt_buflen++] = EVT_CASH_DOOR_OPEN;
      else
        evt_buf[evt_buflen++] = EVT_CASH_DOOR_CLOSED;
    }    
    // Analiza el bit 1, que indica error de aceptador
    if( ((eventos[4] & 0x02) != (rx_buffer[11] & 0x02)) && evt_buflen < MAX_EVENTOS )
    {
      if( rx_buffer[11] & 0x02 )
        evt_buf[evt_buflen++] = EVT_BILL_ACCEPTOR_HW_FAILURE;
    }    
    // Analiza el bit 2, que indica error de billete
    if( ((eventos[4] & 0x04) != (rx_buffer[11] & 0x04)) && evt_buflen < MAX_EVENTOS )
    {
      if( rx_buffer[11] & 0x04 )
        evt_buf[evt_buflen++] = EVT_BILL_REJECTED;

      // Falta tratar el bit 5, que indica aceptador deshabilitado
    }    
    eventos[4] = rx_buffer[11];
  }
  // Envía el mensaje de eventos al servidor  (OJO -> completar / ver el manejo como cola de eventos)
  if( evt_buflen && !waitingEventAck )
  {
    for(i=0; i<evt_buflen; ++i)
      bufEventos.evento[i] = evt_buf[i];
        
    // Cantidad de eventos
    bufEventos.nEventos = evt_buflen;
    // Estampa fecha y hora
    sprintf(bufEventos.fechaHora,"%04d%02d%02d%02d%02d%02d",tiempo.anio,tiempo.mes,tiempo.dia,tiempo.hora,tiempo.min,tiempo.seg);
    // Agrega el código de hash
    bufEventos.hash = RANDOM();
    // Luego calcula el CRC
    bufEventos.crc = CRC((unsigned char *)&bufEventos,sizeof(EventMsgType)-2,0);
    // cifra el mensaje
    cifrar((unsigned char *)&bufEventos,sizeof(EventMsgType));
    // Envía el mensaje al servidor
    sendUDP( (unsigned char *) &bufEventos, sizeof(EventMsgType), SERV_EVENTS, UDP_LOCAL_PORT_CLI, UDP_PORT_EVENTS );
    // activa la espera del ACK
    waitingEventAck=1;
    waitEventAckTimer=0;
    
    // resetea el buffer de eventos para permitir el registro de nuevos
    // mientras espera el ACK a este mensaje
    evt_buflen = 0;
  }
#else  
  // Para probar eventos dacom
  i=0;
  if( eventos[4] != rx_buffer[11] )
  {
    eventos[4] = rx_buffer[11];
    bufEventos.evento[i++] = 0x2B;
    bufEventos.evento[i++] = ((eventos[4] & 0x80)>>3) | ((eventos[4] & 0x40)>>6);
    bufEventos.evento[i++] = ((eventos[4] & 0x20)>>1) | ((eventos[4] & 0x10)>>4);
    bufEventos.evento[i++] = ((eventos[4] & 0x08)<<1) | ((eventos[4] & 0x04)>>2);
    bufEventos.evento[i++] = ((eventos[4] & 0x02)<<3) | (eventos[4] & 0x01);
  }

  if(i)
  {
    // Cantidad de eventos
    bufEventos.nEventos = i;
    // Estampa fecha y hora
    sprintf(bufEventos.fechaHora,"%04d%02d%02d%02d%02d%02d",tiempo.anio,tiempo.mes,tiempo.dia,tiempo.hora,tiempo.min,tiempo.seg);
    // Agrega el código de hash
    bufEventos.hash = RANDOM();
    // Luego calcula el CRC
    bufEventos.crc = CRC((unsigned char *)&bufEventos,sizeof(EventMsgType)-2,0);
    // cifra el mensaje
    cifrar((unsigned char *)&bufEventos,sizeof(EventMsgType));
    // Envía el mensaje al servidor
    if( sendUDP( (unsigned char *) &bufEventos, sizeof(EventMsgType), SERV_EVENTS, UDP_LOCAL_PORT_CLI, UDP_PORT_EVENTS ) )
    {
      // activa la espera del ACK
      waitingEventAck=1;
      waitEventAckTimer=0;
    }
  }
#endif
}

//---------------------------------------------------
// Procesa una trama de tipo 3
//---------------------------------------------------
static void processFrame3()
{
  unsigned char i, j;

  // Contador de jugadas
  for(i=16, j=36; i<20; ++i, --j)
    longPollBuffer.lp1c[i] = rx_buffer[j];
  // Centavos ganados
  for(i=20, j=31; i<24; ++i, --j)
    longPollBuffer.lp1c[i] = rx_buffer[j];

#ifndef DEBUG_DACOM_EVENTS
  // Procesamiento a nivel de bits para detectar eventos
  // Byte 11
  if( eventos[5] != rx_buffer[11] )
  {
//    // Analiza el bit 1, que indica ????
//    if( ((eventos[5] & 0x02) != (rx_buffer[11] & 0x02)) && evt_buflen < MAX_EVENTOS )
//    {
//      if( rx_buffer[11] & 0x02 )
//        evt_buf[evt_buflen++] = EVT_???????;
//    }    
  }
  // Envía el mensaje de eventos al servidor  (OJO -> completar / ver el manejo como cola de eventos)
  if( evt_buflen && !waitingEventAck )
  {
    for(i=0; i<evt_buflen; ++i)
      bufEventos.evento[i] = evt_buf[i];
        
    // Cantidad de eventos
    bufEventos.nEventos = evt_buflen;
    // Estampa fecha y hora
    sprintf(bufEventos.fechaHora,"%04d%02d%02d%02d%02d%02d",tiempo.anio,tiempo.mes,tiempo.dia,tiempo.hora,tiempo.min,tiempo.seg);
    // Agrega el código de hash
    bufEventos.hash = RANDOM();
    // Luego calcula el CRC
    bufEventos.crc = CRC((unsigned char *)&bufEventos,sizeof(EventMsgType)-2,0);
    // cifra el mensaje
    cifrar((unsigned char *)&bufEventos,sizeof(EventMsgType));
    // Envía el mensaje al servidor
    sendUDP( (unsigned char *) &bufEventos, sizeof(EventMsgType), SERV_EVENTS, UDP_LOCAL_PORT_CLI, UDP_PORT_EVENTS );
    // activa la espera del ACK
    waitingEventAck=1;
    waitEventAckTimer=0;
    
    // resetea el buffer de eventos para permitir el registro de nuevos
    // mientras espera el ACK a este mensaje
    evt_buflen = 0;
  }
#else  
  // Para probar eventos dacom
  i=0;
  if( eventos[5] != rx_buffer[11] )
  {
    eventos[5] = rx_buffer[11];
    bufEventos.evento[i++] = 0x3B;
    bufEventos.evento[i++] = ((eventos[5] & 0x80)>>3) | ((eventos[5] & 0x40)>>6);
    bufEventos.evento[i++] = ((eventos[5] & 0x20)>>1) | ((eventos[5] & 0x10)>>4);
    bufEventos.evento[i++] = ((eventos[5] & 0x08)<<1) | ((eventos[5] & 0x04)>>2);
    bufEventos.evento[i++] = ((eventos[5] & 0x02)<<3) | (eventos[5] & 0x01);
  }
  if(i)
  {
    // Cantidad de eventos
    bufEventos.nEventos = i;
    // Estampa fecha y hora
    sprintf(bufEventos.fechaHora,"%04d%02d%02d%02d%02d%02d",tiempo.anio,tiempo.mes,tiempo.dia,tiempo.hora,tiempo.min,tiempo.seg);
    // Agrega el código de hash
    bufEventos.hash = RANDOM();
    // Luego calcula el CRC
    bufEventos.crc = CRC((unsigned char *)&bufEventos,sizeof(EventMsgType)-2,0);
    // cifra el mensaje
    cifrar((unsigned char *)&bufEventos,sizeof(EventMsgType));
    // Envía el mensaje al servidor
    if( sendUDP( (unsigned char *) &bufEventos, sizeof(EventMsgType), SERV_EVENTS, UDP_LOCAL_PORT_CLI, UDP_PORT_EVENTS ) )
    {
      // activa la espera del ACK
      waitingEventAck=1;
      waitEventAckTimer=0;
    }
  }
#endif
}

//---------------------------------------------------
// Carga créditos a la VGM
//---------------------------------------------------
unsigned char dacom_cashin(unsigned long monto)
{
  unsigned char nFrame, i, xor, m[3];
  
  if( !monto )
  {
    // Si el monto a cargar es cero, no hace nada
    // y da por finalizada la carga
    return DACOM_SUCCESS;
  }
  
  // Espera recibir una trama de tipo 1
  delayTimer = 0;
  do
  { nFrame = dacomProcessing();
    if(nFrame == 1 && (VGM_READY() || VGM_BUSY()))
      break;
  }while( delayTimer < _5seg );
  
  if( !(nFrame == 1 && (VGM_READY() || VGM_BUSY())) )
  {
    LCD_print(&MSG_ERROR_RETIRE_TARJETA);
    return DACOM_NO_RESPONSE;
  }
  
  // Prepara el primer mensaje de cashin
  tx_buffer[0] = 0xFF;
  tx_buffer[1] = 0xA1;
  
  tx_buffer[2] = 0x01;    // VGM ID
  tx_buffer[3] = 0x00;
  tx_buffer[4] = 0x00;
  
  long2bcd(m, monto, 6);  // Créditos a cargar
  tx_buffer[5] = m[2];
  tx_buffer[6] = m[1];
  tx_buffer[7] = m[0];

  for(i=1, xor=0; i<8; ++i)
    xor ^= tx_buffer[i];
  
  tx_buffer[8] = xor & 0x0F;
  tx_buffer[9] = xor & 0xF0;

  // Envía el primer mensaje
  for(i=0; i<10; ++i)
    UART0_tx(tx_buffer[i],0);

  // Espera la respuesta
  if( !UART0_rx(10) )
    return DACOM_NO_RESPONSE;           // La VGM no respondió
  
  // Verifica que el mensaje recibido sea igual al enviado
  for(i=0; i<10; ++i)
    if( rx_buffer[i] != tx_buffer[i] )
      return DACOM_BAD_RESPONSE;        // La respuesta no coincide con el mensaje enviado
  
  // Envía el mensaje de confirmación
  tx_buffer[1] = 0xA2;

  for(i=1, xor=0; i<8; ++i)
    xor ^= tx_buffer[i];
  
  tx_buffer[8] = xor & 0x0F;
  tx_buffer[9] = xor & 0xF0;

  // Envía el segundo mensaje para confirmar la carga
  for(i=0; i<10; ++i)
    UART0_tx(tx_buffer[i],0);
  
  return DACOM_SUCCESS;
}

//---------------------------------------------------
// Descarga créditos de la VGM
//---------------------------------------------------
unsigned char dacom_cashout(unsigned long * monto)
{
  unsigned char nFrame, i, xor;
  
  // Espera recibir una trama de tipo 1
  delayTimer = 0;
  do
  { nFrame = dacomProcessing();
    if(nFrame == 1 && (VGM_READY() || VGM_BUSY()))
      break;
  }while( 1 );
  
  // Prepara el primer mensaje de cashout
  tx_buffer[0] = 0xFF;
  tx_buffer[1] = 0xB1;
  
  tx_buffer[2] = 0x01;  // VGM ID
  tx_buffer[3] = 0x00;
  tx_buffer[4] = 0x00;
  
  tx_buffer[5] = longPollBuffer.lp1a[3];  // Créditos a descargar
  tx_buffer[6] = longPollBuffer.lp1a[2];
  tx_buffer[7] = longPollBuffer.lp1a[1];

  for(i=1, xor=0; i<8; ++i)
    xor ^= tx_buffer[i];
  
  tx_buffer[8] = xor & 0x0F;
  tx_buffer[9] = xor & 0xF0;

  *monto = bcd2long(longPollBuffer.lp1a, 8);
  
  if( ! (*monto) )
  {
    // Si la máquina no tiene crédito, no hace nada
    // y da por finalizada la descarga
    return DACOM_SUCCESS;
  }
    
  // Envía el primer mensaje
  for(i=0; i<10; ++i)
    UART0_tx(tx_buffer[i],0);

  // Espera la respuesta
  if( !UART0_rx(10) )
    return DACOM_NO_RESPONSE;           // La VGM no respondió
  
  // Verifica que el mensaje recibido sea igual al enviado
  for(i=0; i<10; ++i)
    if( rx_buffer[i] != tx_buffer[i] )
      return DACOM_BAD_RESPONSE;        // La respuesta no coincide con el mensaje enviado
  
  // Envía el mensaje de confirmación
  tx_buffer[1] = 0xB2;

  for(i=1, xor=0; i<8; ++i)
    xor ^= tx_buffer[i];
  
  tx_buffer[8] = xor & 0x0F;
  tx_buffer[9] = xor & 0xF0;

  // Envía el segundo mensaje para confirmar la descarga
  for(i=0; i<10; ++i)
    UART0_tx(tx_buffer[i],0);
  
  // Luego de la descarga, espera recibir una trama de tipo 1
  // que indique que la máquina ha quedado descargada
  delayTimer = 0;
  for(i=0; i<10; )
  { 
    if( dacomProcessing() == 1 && (VGM_READY() || VGM_BUSY()) )
    {
      ++i;
      if( bcd2long(longPollBuffer.lp1a, 8) == 0 )
        // La máquina no tiene crédito --> da por
        // finalizada exitosamente la descarga
        return DACOM_SUCCESS;
    }
  }
  // Luego de recibidas 10 tramas tipo 1, ninguna indicó que
  // la máquina hubiera quedado descargada --> aborta la descarga
  return DACOM_BAD_RESPONSE;
}

