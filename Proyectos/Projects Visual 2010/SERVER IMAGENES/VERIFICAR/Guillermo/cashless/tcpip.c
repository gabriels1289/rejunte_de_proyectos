//------------------------------------------------------------------------------
// tcpip.c
// Implementa el stack TCP/IP (sólo para UDP)
//------------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "msp430x14x.h"
#include "support.h"
#include "cs8900.h"
#include "enc28j60.h"
#include "tcpip.h"
#include "cashless.h"
#include "flash.h"
#include "mgmt.h"

// Parámetros IP propios

#pragma location=0x1000
__no_init unsigned int MyCfgIP[2];         // dirección IP configurable en la FLASH de datos

unsigned int MyIP[2];                      // "MYIP1.MYIP2.MYIP3.MYIP4"

unsigned int SubnetMask[2];                // "SUBMASK1.SUBMASK2.SUBMASK3.SUBMASK4"

unsigned int GatewayIP[2];                 // "GWIP1.GWIP2.GWIP3.GWIP4"

// Direcciones IP y MAC de los servidores
ServerAddrType ServerAddr[TOTAL_SERVERS];

// variables
static unsigned char TransmitControl;     // !=0 if anything to transmit
volatile unsigned int ARPTimer;           // time elapsed since ARP request

// Direcciones MAC, IP y Puertos a usar en la preparación de la trama a enviar

unsigned int RemoteMAC[3];                // MAC Remota
unsigned int RemoteIP[2];                 // IP Remota

unsigned int UDPLocalPort;                // Puerto UDP local para usar como cliente
unsigned int UDPRemotePort;               // Puerto UDP remoto

unsigned int UDPRxDataCount;              // nro de bytes de datos recibidos en un datagrama UDP
unsigned int UDPTxDataCount;              // nro de bytes a enviar en un datagrama UDP

// Propiedades de la última trama recibida

unsigned int RecdFrameLength;             // Longitud de la trama, reportada por el Controlador Ethernet
unsigned int RecdFrameMAC[3];             // MAC origen de la trama recibida (48 bits)
unsigned int RecdFrameIP[2];              // IP de origen del paquete recibido (32 bits)
unsigned int RecdFrameUDPPort;            // Puerto de origen del datagrama UDP recibido (16 bits)
unsigned int RecdIPFrameLength;           // Longitud del paquete IP (16 bits)

// Buffers de transmisión y recepción (deben estar alineados por palabras -word-aligned-)

// Buffer para transmisión de tramas de datos (UDP)
unsigned int TxFrame1Mem[(ETH_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE + MAX_UDP_TX_DATA_SIZE + 1) >> 1];

// Buffer para transmisión de tramas ARP e ICMP
unsigned int TxFrame2Mem[(ETH_HEADER_SIZE + MAX_ETH_TX_DATA_SIZE + 1) >> 1];

// Buffer para almacenar los datos recibidos en un datagrama UDP
unsigned int RxUDPBufferMem[(MAX_UDP_RX_DATA_SIZE + 1) >> 1];

// Cantidad de bytes a enviar en TxFrame1
static unsigned int TxFrame1Size;

// Cantidad de bytes a enviar en TxFrame2
static unsigned char TxFrame2Size;

// Prototipos de funciones estáticas

//------------------------------------------------------------------------------
// Handlers for incoming frames
//------------------------------------------------------------------------------
static void ProcessEthBroadcastFrame(void);
static void ProcessEthIAFrame(void);
static void ProcessICMPFrame(void);
static void ProcessUDPFrame(void);

//------------------------------------------------------------------------------
// Prototipos de funciones usadas para llenar los buffers de transmisión
//------------------------------------------------------------------------------
static void PrepareARP_REQUEST(void);
static void PrepareARP_ANSWER(void);
static void PrepareICMP_ECHO_REPLY(void);
static void PrepareUDP_DATA_FRAME(void);

//------------------------------------------------------------------------------
// Calcula el checksum del rango de direcciones indicado.
// Si se indica, computa también el pseudo-header del encabezado IP
//------------------------------------------------------------------------------
static unsigned int CalcChecksum(void *Start, unsigned int Count, unsigned char pseudoHeader);

//------------------------------------------------------------------------------
//Verifica si la IP propia no está configurada en la Flash
// de datos, en cuyo caso configura como IP propia la TargetIP.
// También usa ese mismo valor para configurar la MAC propia.
//------------------------------------------------------------------------------
static void checkIP_MAC_cfg(unsigned int IP[]);

//------------------------------------------------------------------------------
// Verifica si la trama proviene de algún servidor, a fin de
// refrescar o actualizar la MAC del mismo
//------------------------------------------------------------------------------
static void updateMACAddress(void);

//------------------------------------------------------------------------------
// Funciones auxiliares que seleccionan el driver ethernet
// que corresponde al modelo de placa
//------------------------------------------------------------------------------
void CopyToFrame(void *Source, unsigned int Size);
void CopyFromFrame(void *Dest, unsigned int Size);
void DummyReadFrame(unsigned int Size);
unsigned int ReadFrame(void);
unsigned int ReadFrameBE(void);

void InitEthernet()
{
  if( HW_type == HW_OLIMEX )  Init8900();
  if( HW_type == HW_BB2 )     {spi_init(); Init28J60();}
}

void CopyToFrame(void *Source, unsigned int Size)
{
  if( HW_type == HW_OLIMEX )  CopyToFrame8900(Source, Size);
  if( HW_type == HW_BB2 )     CopyToFrame28J60(Source, Size);
}

void CopyFromFrame(void *Dest, unsigned int Size)
{
  if( HW_type == HW_OLIMEX )  CopyFromFrame8900(Dest, Size);
  if( HW_type == HW_BB2 )     CopyFromFrame28J60(Dest, Size);
}

void DummyReadFrame(unsigned int Size)
{
  if( HW_type == HW_OLIMEX )  DummyReadFrame8900(Size);
  if( HW_type == HW_BB2 )     DummyReadFrame28J60(Size);
}

unsigned int ReadFrame(void)
{
  if( HW_type == HW_OLIMEX )  return ReadFrame8900();
  if( HW_type == HW_BB2 )     return ReadFrame28J60();
  return 0;
}

unsigned int ReadFrameBE(void)
{
  if( HW_type == HW_OLIMEX )  return ReadFrameBE8900();
  if( HW_type == HW_BB2 )     return ReadFrameBE28J60();
  return 0;
}

//------------------------------------------------------------------------------
// Inicializar direcciones IP y MAC de los servidores
//------------------------------------------------------------------------------
void InitIP()
{
  int i,j;

  if( MyCfgIP[0] == 0xFFFF && MyCfgIP[1] == 0xFFFF )
  {
    // No tiene configurada una IP en la Flash de datos --> usa la IP default
    MyIP[0] = MYIP_1 + (unsigned int)(MYIP_2 << 8);
    MyIP[1] = MYIP_3 + (unsigned int)(MYIP_4 << 8);
  }else{
    // Toma la IP propia de la flash de datos
    MyIP[0] = MyCfgIP[0];
    MyIP[1] = MyCfgIP[1];
  }

  // "SUBMASK1.SUBMASK2.SUBMASK3.SUBMASK4"
  SubnetMask[0] = SUBMASK_1 + (unsigned int)(SUBMASK_2 << 8);
  SubnetMask[1] = SUBMASK_3 + (unsigned int)(SUBMASK_4 << 8);

  // "GWIP1.GWIP2.GWIP3.GWIP4"
  GatewayIP[0] = GWIP_1 + (unsigned int)(GWIP_2 << 8);
  GatewayIP[1] = GWIP_3 + (unsigned int)(GWIP_4 << 8);

  // direcciones IP de los servidores
  if(Features.IP_server_evt[0] == 0xffff)
  {
    ServerAddr[SERV_EVENTS].IP[0] = SERVER_EVT_IP_1 + (unsigned int)(SERVER_EVT_IP_2 << 8);
    ServerAddr[SERV_EVENTS].IP[1] = SERVER_EVT_IP_3 + (unsigned int)(SERVER_EVT_IP_4 << 8);
  }else{
    ServerAddr[SERV_EVENTS].IP[0] = Features.IP_server_evt[0];
    ServerAddr[SERV_EVENTS].IP[1] = Features.IP_server_evt[1];
  }
    
  if(Features.IP_server_met[0] == 0xffff)
  {
    ServerAddr[SERV_METERS].IP[0] = SERVER_MET_IP_1 + (unsigned int)(SERVER_MET_IP_2 << 8);
    ServerAddr[SERV_METERS].IP[1] = SERVER_MET_IP_3 + (unsigned int)(SERVER_MET_IP_4 << 8);
  }else{
    ServerAddr[SERV_METERS].IP[0] = Features.IP_server_met[0];
    ServerAddr[SERV_METERS].IP[1] = Features.IP_server_met[1];
  }
  
  if(Features.IP_server_cash[0] == 0xffff)
  {
    ServerAddr[SERV_CASH].IP[0] = SERVER_CASH_IP_1 + (unsigned int)(SERVER_CASH_IP_2 << 8);
    ServerAddr[SERV_CASH].IP[1] = SERVER_CASH_IP_3 + (unsigned int)(SERVER_CASH_IP_4 << 8);
  }else{
    ServerAddr[SERV_CASH].IP[0] = Features.IP_server_cash[0];
    ServerAddr[SERV_CASH].IP[1] = Features.IP_server_cash[1];
  }
  
  if(Features.IP_server_mgmt[0] == 0xffff)
  {
    ServerAddr[SERV_MGMT].IP[0] = SERVER_MGMT_IP_1 + (unsigned int)(SERVER_MGMT_IP_2 << 8);
    ServerAddr[SERV_MGMT].IP[1] = SERVER_MGMT_IP_3 + (unsigned int)(SERVER_MGMT_IP_4 << 8);
  }else{
    ServerAddr[SERV_MGMT].IP[0] = Features.IP_server_mgmt[0];
    ServerAddr[SERV_MGMT].IP[1] = Features.IP_server_mgmt[1];
  }
  
  if(Features.IP_server_graf[0] == 0xffff)
  {
    ServerAddr[SERV_GRAF].IP[0] = SERVER_GRAF_IP_1 + (unsigned int)(SERVER_GRAF_IP_2 << 8);
    ServerAddr[SERV_GRAF].IP[1] = SERVER_GRAF_IP_3 + (unsigned int)(SERVER_GRAF_IP_4 << 8);
  }else{
    ServerAddr[SERV_GRAF].IP[0] = Features.IP_server_graf[0];
    ServerAddr[SERV_GRAF].IP[1] = Features.IP_server_graf[1];
  }
#if 1
  // Esto va para la versión final
  ServerAddr[SERV_TOUCH].IP[0] = MyIP[0];
  ServerAddr[SERV_TOUCH].IP[1] = MyIP[1] + 100;
#else
  // Esto va para probar con el cashtest simulando la PC-touch screen
  ServerAddr[SERV_TOUCH].IP[0] = SERVER_TOUCH_IP_1 + (unsigned int)(SERVER_TOUCH_IP_2 << 8);
  ServerAddr[SERV_TOUCH].IP[1] = SERVER_TOUCH_IP_3 + (unsigned int)(SERVER_TOUCH_IP_4 << 8);
#endif
  
  for(i=0; i<TOTAL_SERVERS; ++i)
  {
    ServerAddr[i].MAC_age = MAX_MAC_AGE + 1;
    
    for(j=0; j<3; j++)
      ServerAddr[i].MAC[j] = 0;
  }

  UDPLocalPort = 1234;

  TransmitControl = 0;
}

//--------------------------------------------------------------------------------
// Procesamiento de las tramas Ethernet recibidas.
// Algunas de ellas pueden dar lugar a transmisiones, que son manejadas desde acá.
// Esta función debe ser invocada lo más frecuentemente posible.
//--------------------------------------------------------------------------------
void DoNetworkStuff(void)
{
  unsigned int ActRxEvent;  // para copiar el registro de eventos del CS8900
                            // o la palabra de estado del ENC28J60

  // Este procesamiento se hace discriminando el tipo de controladora Ethernet
  switch( HW_type )
  {
  case HW_OLIMEX:
    Write8900(ADD_PORT, PP_RxEvent);               // point to RxEvent
    ActRxEvent = Read8900(DATA_PORT);              // read, implied skip of last frame
    
    if (ActRxEvent & RX_OK)
    {
      // Las próximas dos palabras deben leerse con el MSB en primer lugar (CS8900 AN181 Page 2)
      ReadHB1ST8900(RX_FRAME_PORT);                    // ignora la palabra de RxStatus
      RecdFrameLength = ReadHB1ST8900(RX_FRAME_PORT);  // obtiene la longitud real de la trama
      
      if (ActRxEvent & RX_IA)             ProcessEthIAFrame();
      else if (ActRxEvent & RX_BROADCAST) ProcessEthBroadcastFrame();
    }
    break;
    
  case HW_BB2:
    // Consulta si hay una trama recibida, y si la hay obtiene
    // la palabra de estado y la longitud de la trama.
    // Luego, procesa la trama recibida
    if( Rx28J60(&RecdFrameLength, &ActRxEvent) )
    {
      if(ActRxEvent & RX_28J60_OK)
      {
        if(ActRxEvent & RX_28J60_BROADCAST)
          ProcessEthBroadcastFrame();
        else if( !(ActRxEvent & RX_28J60_MULTICAST) )
          ProcessEthIAFrame();
      }
      free28J60RxBuffer();  // Libera el espacio de memoria ocupado por la trama
    }
    break;
  }
  
  // Si hay una trama ARP Reply o ICMP Echo Reply para enviar, la transmite
  if (TransmitControl & SEND_FRAME2)
  {
    // Si usa CS8900 consulta si está disponible para aceptar una trama
    // Si usa ENC28J60 transmite directamente
    if( HW_type == HW_OLIMEX )
    {
      RequestSend(TxFrame2Size);
      if( !Rdy4Tx() )
        return;
    }
    
    CopyToFrame((unsigned char *)TxFrame2Mem, TxFrame2Size);
    
    TransmitControl &= ~SEND_FRAME2;           // clear tx-flag
  }
}
//------------------------------------------------------------------------------
// Envía un req de ARP al servidor
//------------------------------------------------------------------------------
void sendARP(unsigned int nServer)
{
  RemoteIP[0]=ServerAddr[nServer].IP[0];
  RemoteIP[1]=ServerAddr[nServer].IP[1];
  PrepareARP_REQUEST();
  
  if( HW_type == HW_OLIMEX )
  {
    RequestSend(TxFrame2Size);
    if( !Rdy4Tx() )           // CS8900 ready to accept our frame?
      return;
  }
  CopyToFrame((unsigned char *)TxFrame2Mem, TxFrame2Size);

  TransmitControl &= ~SEND_FRAME2;           // clear tx-flag

  ARPTimer = 0;
}

//------------------------------------------------------------------------------
// Envía un datagrama UDP al servidor
// Retorna 1 si envió, 0 si no pudo enviar
//------------------------------------------------------------------------------
unsigned char sendUDP(unsigned char * msg, unsigned int msglen, unsigned int nServer, unsigned int srcPort, unsigned int destPort)
{
  // Si no tiene la MAC del servidor, envía un requerimiento de ARP y espera la respuesta
  if( ServerAddr[nServer].MAC_age > MAX_MAC_AGE )
  {
    sendARP(nServer);
    // Espera hasta 400 ms por la respuesta de ARP
    while( ServerAddr[nServer].MAC_age > MAX_MAC_AGE )
    {
      if( ARPTimer >= _400ms )
      {
        // Sale por timeout --> aborta la transmisión
        return 0;
      }
      DoNetworkStuff();
    }
  }
  // Copia el mensaje al buffer de transmisión
  memcpy(UDP_TX_BUF, msg, msglen);
  UDPTxDataCount = msglen;
  RemoteMAC[0] = ServerAddr[nServer].MAC[0];
  RemoteMAC[1] = ServerAddr[nServer].MAC[1];
  RemoteMAC[2] = ServerAddr[nServer].MAC[2];
  RemoteIP[0] = ServerAddr[nServer].IP[0];
  RemoteIP[1] = ServerAddr[nServer].IP[1];
  
  UDPLocalPort = srcPort;
  UDPRemotePort = destPort;
  
  PrepareUDP_DATA_FRAME();

  if( HW_type == HW_OLIMEX )
  {
    RequestSend(TxFrame1Size);
    if( !Rdy4Tx() )                  // CS8900 ready to accept our frame?
      return 0;
  }
  
  CopyToFrame((unsigned char *)TxFrame1Mem, TxFrame1Size);

  return 1;
}

//------------------------------------------------------------------------------
// Verifica si la trama proviene de algún servidor, a fin de refrescar o
// actualizar la MAC del mismo
//------------------------------------------------------------------------------
static void updateMACAddress()
{
  unsigned int i;
  
  for(i=0; i<TOTAL_SERVERS; ++i)
  {
    if ((RecdFrameIP[0] == ServerAddr[i].IP[0]) && (RecdFrameIP[1] == ServerAddr[i].IP[1]))
    {
      ServerAddr[i].MAC[0] = RecdFrameMAC[0];
      ServerAddr[i].MAC[1] = RecdFrameMAC[1];
      ServerAddr[i].MAC[2] = RecdFrameMAC[2];
      ServerAddr[i].MAC_age = 0;              // Resetea antiguedad de la dirección MAC
    }
  }
}

//------------------------------------------------------------------------------
// Verifica si la IP propia no está configurada en la Flash de datos,
// en cuyo caso configura como IP propia la TargetIP.
// También usa ese mismo valor para configurar la MAC propia.
//------------------------------------------------------------------------------
//static void checkIP_MAC_cfg(unsigned int TargetIP[])
//{
//  if( MyCfgIP[0] == 0xFFFF && MyCfgIP[1] == 0xFFFF )
//  {
//    // No tiene configurada una IP en la Flash de datos -->
//    // --> asume que la TargetIP será la propia y la graba en la Flash
//    MyIP[0] = TargetIP[0];
//    MyIP[1] = TargetIP[1];
//    
//    // Graba la IP en la flash de información
//    flash_ww( &MyCfgIP[0], MyIP[0] );
//    flash_ww( &MyCfgIP[1], MyIP[1] );
//  }
//  
//  if( MyCfgMAC[0] == 0xFFFF && MyCfgMAC[1] == 0xFFFF && MyCfgMAC[2] == 0xFFFF )
//  {
//    // No tiene configurada una MAC en la Flash de datos -->
//    // --> genera una dirección MAC a partir de la TargetIP y la graba en la Flash
//    MyMAC[0] = 0;
//    MyMAC[1] = TargetIP[0];
//    MyMAC[2] = TargetIP[1];
//
//    // Graba la MAC en la flash de información
//    flash_ww( &MyCfgMAC[0], MyMAC[0] );
//    flash_ww( &MyCfgMAC[1], MyMAC[1] );
//    flash_ww( &MyCfgMAC[2], MyMAC[2] );
//    
//    // Inicializa el controlador Ethernet para que tome la nueva MAC
//    InitEthernet();
//  }
//}

//------------------------------------------------------------------------------
// Procesa una trama broadcast recibida. La única opción aceptada es que
// se trate de un ARP request.
//------------------------------------------------------------------------------
static void ProcessEthBroadcastFrame(void)
{
  unsigned int TargetIP[2];
  
  DummyReadFrame(6);                         // ignore DA (FF-FF-FF-FF-FF-FF)
  CopyFromFrame(&RecdFrameMAC, 6);           // store SA (for our answer)
  
  if (ReadFrameBE() == FRAME_ARP)            // get frame type, check for ARP
    if (ReadFrameBE() == HARDW_ETH10)        // Ethernet frame
      if (ReadFrameBE() == FRAME_IP)         // check protocol
        if (ReadFrameBE() == IP_HLEN_PLEN)   // check HLEN, PLEN
          if (ReadFrameBE() == OP_ARP_REQUEST)
          {
            DummyReadFrame(6);               // ignore sender's hardware address
            CopyFromFrame(&RecdFrameIP, 4);  // read sender's protocol address
            DummyReadFrame(6);               // ignore target's hardware address
            CopyFromFrame(&TargetIP, 4);     // read target's protocol address
            
            // Verifica si la IP propia no está configurada en la Flash de datos,
            // y en ese caso configura como IP propia la TargetIP.
            // También usa ese mismo valor para asignar una MAC única.
            //checkIP_MAC_cfg(TargetIP);
            
            // Verifica si la trama proviene de algún servidor, a fin de aprovechar
            // para refrescar la MAC del mismo y ahorrar envíos de ARP
            updateMACAddress();
            
            // Verifica si el ARP Request es para nosotros
            if ((MyIP[0] == TargetIP[0]) && (MyIP[1] == TargetIP[1]))
              PrepareARP_ANSWER();               // si --> preparar trama de ARP_ANSWER
          }
}

//------------------------------------------------------------------------------
// Procesa una trama entrante cuya dirección MAC de destino coincide con
// la propia MAC (pasó el filtro que aplica el Controlador Ethernet en base a la
// dirección individual que tiene seteada como propia).
// (individual addressed = IA)
//------------------------------------------------------------------------------
static void ProcessEthIAFrame(void)
{
  unsigned int TargetIP[2];
  unsigned char ProtocolType;

  DummyReadFrame(6);                         // ignore DA
  CopyFromFrame(RecdFrameMAC, 6);            // store SA (for our answer)

  switch (ReadFrameBE())                     // get frame type
  {
    case FRAME_ARP :                         // check for ARP
      if (ReadFrameBE() == HARDW_ETH10)      // check for the right prot. etc.
        if (ReadFrameBE() == FRAME_IP)
          if (ReadFrameBE() == IP_HLEN_PLEN)
            if (ReadFrameBE() == OP_ARP_ANSWER)
            {
              CopyFromFrame(RemoteMAC, 6);     // extrae la MAC del remitente
              CopyFromFrame(RecdFrameIP, 4);   // extrae la IP del remitente

              // Verifica si la trama proviene de algún servidor,
              // a fin de actualizar la dirección MAC del mismo
              updateMACAddress();
            }
      break;
    case FRAME_IP :                                    // check for IP-type
      if ((ReadFrameBE() & 0xff00 ) == IP_VER_IHL)     // IPv4, IHL=5 (20 Bytes Header)
      {                                                // ignore Type Of Service
        RecdIPFrameLength = ReadFrameBE();             // get IP frame's length
        ReadFrameBE();                                 // ignore identification

        if (!(ReadFrameBE() & (IP_FLAG_MOREFRAG | IP_FRAGOFS_MASK)))  // only unfragm. frames
        {
          ProtocolType = ReadFrameBE();                // get protocol, ignore TTL
          ReadFrameBE();                               // ignore checksum
          RecdFrameIP[0] = ReadFrame();                // get source IP
          RecdFrameIP[1] = ReadFrame();
          TargetIP[0] = ReadFrame();                   // get destination IP
          TargetIP[1] = ReadFrame();

          // Verifica si la trama proviene de algún servidor,
          // a fin de actualizar la dirección MAC del mismo
          updateMACAddress();
                
          if ((MyIP[0] == TargetIP[0]) && (MyIP[1] == TargetIP[1]))
          { 
            // El paquete es para nosotros --> lo procesa según el protocolo
            switch (ProtocolType)
            {
              case PROT_ICMP :
                ProcessICMPFrame();
                break;
              case PROT_TCP :
                //ProcessTCPFrame();
                break;
              case PROT_UDP :
                ProcessUDPFrame();
                break;
            }
          }
        }      
      }
      break;
  }
}

//------------------------------------------------------------------------------
// easyWEB internal function
// we've just rec'd an ICMP-frame (Internet Control Message Protocol)
// check what to do and branch to the appropriate sub-function
//------------------------------------------------------------------------------
static void ProcessICMPFrame(void)
{
  unsigned int ICMPTypeAndCode;

  ICMPTypeAndCode = ReadFrameBE();               // get Message Type and Code
  ReadFrameBE();                                 // ignore ICMP checksum

  switch (ICMPTypeAndCode >> 8)                  // check type
  {
    case ICMP_ECHO :                             // is echo request?
      PrepareICMP_ECHO_REPLY();                  // echo as much as we can...
      break;
  }
}

//------------------------------------------------------------------------------
// Procesa una trama UDP recibida
//------------------------------------------------------------------------------
static void ProcessUDPFrame(void)
{
  unsigned int UDPSourcePort;                    // datagram's source port
  unsigned int UDPDestPort;                      // datagram's destination port
  unsigned int UDPLength;                        // datagram's length
  unsigned int UDPChecksum;                      // datagram's checksum
  unsigned int NrOfDataBytes;                    // real number of data

  UDPSourcePort = ReadFrameBE();             // get ports
  UDPDestPort = ReadFrameBE();

  UDPLength = (unsigned long)ReadFrameBE();     // get datagram length
  UDPChecksum = (unsigned long)ReadFrameBE();   // get datagram checksum

  NrOfDataBytes = RecdIPFrameLength - IP_HEADER_SIZE - UDP_HEADER_SIZE;     // seg. text length

  if (NrOfDataBytes > MAX_UDP_RX_DATA_SIZE) return;    // descarta el paquete si es más largo que lo esperado

  if (NrOfDataBytes)                                   // data available?
  {
    // Si el datagrama transporta datos, los copia al buffer
    CopyFromFrame(RxUDPBufferMem, NrOfDataBytes);
    UDPRxDataCount = NrOfDataBytes;
  }
  // Guarda el puerto de origen como puerto remoto, para una eventual respuesta
  RecdFrameUDPPort = UDPSourcePort;

  // Procesa el mensaje recibido. Primero discrimina de acuerdo al puerto de destino.
  switch(UDPDestPort)
  {
  case UDP_LOCAL_PORT_CLI:  //------------------------------------------
    // Datagrama recibido para el puerto local cliente.
    // Deberían ser respuestas a mensajes enviados previamente.
    // Ahora discrimina de acuerdo al puerto de origen.
    switch(UDPSourcePort)
    {
    case UDP_PORT_EVENTS:
      if(RecdFrameIP[0]==ServerAddr[SERV_EVENTS].IP[0] && RecdFrameIP[1]==ServerAddr[SERV_EVENTS].IP[1])
        checkEventAck();
      break;
    case UDP_PORT_METERS:
      // No espera mensajes enviados al puerto de contadores
      break;
    case UDP_PORT_HELLO:
      // Recibió un datagrama por el puerto de ECHO del servidor --> lo verifica
      // para ver si lo marca como vivo y actualiza fecha y hora local
      if(RecdFrameIP[0]==ServerAddr[SERV_CASH].IP[0] && RecdFrameIP[1]==ServerAddr[SERV_CASH].IP[1])
        checkHelloReply();
      break;
    case UDP_PORT_TOUCH:
      // Recibió un datagrama enviado desde el puerto de la PC-touch screen --> verifica
      // si la IP de origen es la correcta
      if(RecdFrameIP[0]==ServerAddr[SERV_TOUCH].IP[0] && RecdFrameIP[1]==ServerAddr[SERV_TOUCH].IP[1])
        processTouchReply();
      break;
    default:
      if( UDPSourcePort == UDP_PORT_CASH )
      {
        // Recibió una respuesta a un mensaje de cashless --> lo procesa
        if(RecdFrameIP[0]==ServerAddr[SERV_CASH].IP[0] && RecdFrameIP[1]==ServerAddr[SERV_CASH].IP[1])
          processCashlessReply();
      }
      break;
    }
    break;
    //--------------------------Fin UDP_LOCAL_PORT_CLI--------
    
  case UDP_LOCAL_PORT_TEST: //--------------------------------------
    // Datagrama recibido para el puerto de test
    // Verifica que venga del servidor servidor de test
    if(UDPSourcePort==UDP_PORT_TEST && RecdFrameIP[0]==ServerAddr[SERV_MGMT].IP[0] && RecdFrameIP[1]==ServerAddr[SERV_MGMT].IP[1])
      // Recibió un mensaje de prueba para efectuar algunas acciones de testeo
      processTestMsg();
    break;
    //--------------------------Fin UDP_LOCAL_PORT_TEST-------
    
  case UDP_LOCAL_PORT_MGMT: //--------------------------------------
    // Datagrama recibido para el puerto de gestión.
    // Verifica que provengan del servidor y puerto adecuado.
    // Pueden ser comandos o respuestas.
    if(UDPSourcePort==UDP_PORT_MGMT && RecdFrameIP[0]==ServerAddr[SERV_MGMT].IP[0] && RecdFrameIP[1]==ServerAddr[SERV_MGMT].IP[1])
      processMgmtMsg();
    break;
    //--------------------------Fin UDP_LOCAL_PORT_MGMT-------
  }
}

//------------------------------------------------------------------------------
// Prepara el buffer TxFrame2 para enviar un ARP-request
// La MAC a averiguar puede ser la de RemoteIP o la de GatewayIP 
// TxFrame2Size queda cargado con la longitud de la trama a enviar
//------------------------------------------------------------------------------
static void PrepareARP_REQUEST(void)
{
  // Ethernet
  ACCESS_UINT(TxFrame2Mem, ETH_DA_OFS) = 0xffff;  // we don't know opposites MAC!
  ACCESS_UINT(TxFrame2Mem, ETH_DA_OFS + 2) = 0xffff;
  ACCESS_UINT(TxFrame2Mem, ETH_DA_OFS + 4) = 0xffff;
  ACCESS_UINT(TxFrame2Mem, ETH_SA_OFS) = MyMAC[0];
  ACCESS_UINT(TxFrame2Mem, ETH_SA_OFS + 2) = MyMAC[1];
  ACCESS_UINT(TxFrame2Mem, ETH_SA_OFS + 4) = MyMAC[2];
  ACCESS_UINT(TxFrame2Mem, ETH_TYPE_OFS) = SWAPB(FRAME_ARP);

  // ARP
  ACCESS_UINT(TxFrame2Mem, ARP_HARDW_OFS) = SWAPB(HARDW_ETH10);
  ACCESS_UINT(TxFrame2Mem, ARP_PROT_OFS) = SWAPB(FRAME_IP); 
  ACCESS_UINT(TxFrame2Mem, ARP_HLEN_PLEN_OFS) = SWAPB(IP_HLEN_PLEN);
  ACCESS_UINT(TxFrame2Mem, ARP_OPCODE_OFS) = SWAPB(OP_ARP_REQUEST);
  ACCESS_UINT(TxFrame2Mem, ARP_SENDER_HA_OFS) = MyMAC[0];
  ACCESS_UINT(TxFrame2Mem, ARP_SENDER_HA_OFS + 2) = MyMAC[1];
  ACCESS_UINT(TxFrame2Mem, ARP_SENDER_HA_OFS + 4) = MyMAC[2];
  ACCESS_UINT(TxFrame2Mem, ARP_SENDER_IP_OFS) = MyIP[0];
  ACCESS_UINT(TxFrame2Mem, ARP_SENDER_IP_OFS + 2) = MyIP[1];
  ACCESS_UINT(TxFrame2Mem, ARP_TARGET_HA_OFS) = 0;
  ACCESS_UINT(TxFrame2Mem, ARP_TARGET_HA_OFS + 2) = 0;
  ACCESS_UINT(TxFrame2Mem, ARP_TARGET_HA_OFS + 4) = 0;
  ACCESS_UINT(TxFrame2Mem, ARP_TARGET_IP_OFS) = 0;
  ACCESS_UINT(TxFrame2Mem, ARP_TARGET_IP_OFS + 2) = 0;

  if (((RemoteIP[0] ^ MyIP[0]) & SubnetMask[0]) || ((RemoteIP[1] ^ MyIP[1]) & SubnetMask[1]))
  {
    ACCESS_UINT(TxFrame2Mem, ARP_TARGET_IP_OFS) = GatewayIP[0];
    ACCESS_UINT(TxFrame2Mem, ARP_TARGET_IP_OFS + 2) = GatewayIP[1];
  }else{
    ACCESS_UINT(TxFrame2Mem, ARP_TARGET_IP_OFS) = RemoteIP[0];
    ACCESS_UINT(TxFrame2Mem, ARP_TARGET_IP_OFS + 2) = RemoteIP[1];
  }
  TxFrame2Size = ETH_HEADER_SIZE + ARP_FRAME_SIZE;
  TransmitControl |= SEND_FRAME2;
}

//------------------------------------------------------------------------------
// Prepara el buffer TxFrame2 con un ARP-answer (reply)
// Las direcciones MAC e IP remotas son las obtenidas de la última trama
// recibida: RecdFrameMAC y recdFrameIP  --> esta función debe invocarse
// inmediatamente después de haber recibido un ARP request.
//------------------------------------------------------------------------------
static void PrepareARP_ANSWER(void)
{
  // Ethernet
  ACCESS_UINT(TxFrame2Mem, ETH_DA_OFS) = RecdFrameMAC[0];
  ACCESS_UINT(TxFrame2Mem, ETH_DA_OFS + 2) = RecdFrameMAC[1];
  ACCESS_UINT(TxFrame2Mem, ETH_DA_OFS + 4) = RecdFrameMAC[2];
  ACCESS_UINT(TxFrame2Mem, ETH_SA_OFS) = MyMAC[0];
  ACCESS_UINT(TxFrame2Mem, ETH_SA_OFS + 2) = MyMAC[1];
  ACCESS_UINT(TxFrame2Mem, ETH_SA_OFS + 4) = MyMAC[2];
  ACCESS_UINT(TxFrame2Mem, ETH_TYPE_OFS) = SWAPB(FRAME_ARP);

  // ARP
  ACCESS_UINT(TxFrame2Mem, ARP_HARDW_OFS) = SWAPB(HARDW_ETH10);
  ACCESS_UINT(TxFrame2Mem, ARP_PROT_OFS) = SWAPB(FRAME_IP); 
  ACCESS_UINT(TxFrame2Mem, ARP_HLEN_PLEN_OFS) = SWAPB(IP_HLEN_PLEN);
  ACCESS_UINT(TxFrame2Mem, ARP_OPCODE_OFS) = SWAPB(OP_ARP_ANSWER);
  ACCESS_UINT(TxFrame2Mem, ARP_SENDER_HA_OFS) = MyMAC[0];
  ACCESS_UINT(TxFrame2Mem, ARP_SENDER_HA_OFS + 2) = MyMAC[1];
  ACCESS_UINT(TxFrame2Mem, ARP_SENDER_HA_OFS + 4) = MyMAC[2];
  ACCESS_UINT(TxFrame2Mem, ARP_SENDER_IP_OFS) = MyIP[0];
  ACCESS_UINT(TxFrame2Mem, ARP_SENDER_IP_OFS + 2) = MyIP[1];
  ACCESS_UINT(TxFrame2Mem, ARP_TARGET_HA_OFS) = RecdFrameMAC[0];
  ACCESS_UINT(TxFrame2Mem, ARP_TARGET_HA_OFS + 2) = RecdFrameMAC[1];
  ACCESS_UINT(TxFrame2Mem, ARP_TARGET_HA_OFS + 4) = RecdFrameMAC[2];
  ACCESS_UINT(TxFrame2Mem, ARP_TARGET_IP_OFS) = RecdFrameIP[0];
  ACCESS_UINT(TxFrame2Mem, ARP_TARGET_IP_OFS + 2) = RecdFrameIP[1];

  TxFrame2Size = ETH_HEADER_SIZE + ARP_FRAME_SIZE;
  TransmitControl |= SEND_FRAME2;
}

//------------------------------------------------------------------------------
// Prepara el buffer TxFrame2 para enviar un ICMP-echo-reply
// Obs.1: limita el tamaño de la respuesta al máximo tamaño del 
// Obs.2: usa como IP y MAC de destino las recientemente recibidas
// --> Esta función debe invocarse inmediatamente después de haber recibido
// un ECHO Request
//------------------------------------------------------------------------------
static void PrepareICMP_ECHO_REPLY(void)
{
  unsigned int ICMPDataCount;

  if (RecdIPFrameLength > MAX_ETH_TX_DATA_SIZE)  // don't overload TX-buffer
    ICMPDataCount = MAX_ETH_TX_DATA_SIZE - IP_HEADER_SIZE - ICMP_HEADER_SIZE;
  else
    ICMPDataCount = RecdIPFrameLength - IP_HEADER_SIZE - ICMP_HEADER_SIZE;

  // Ethernet
  ACCESS_UINT(TxFrame2Mem, ETH_DA_OFS) = RecdFrameMAC[0];
  ACCESS_UINT(TxFrame2Mem, ETH_DA_OFS + 2) = RecdFrameMAC[1];
  ACCESS_UINT(TxFrame2Mem, ETH_DA_OFS + 4) = RecdFrameMAC[2];
  ACCESS_UINT(TxFrame2Mem, ETH_SA_OFS) = MyMAC[0];
  ACCESS_UINT(TxFrame2Mem, ETH_SA_OFS + 2) = MyMAC[1];
  ACCESS_UINT(TxFrame2Mem, ETH_SA_OFS + 4) = MyMAC[2];
  ACCESS_UINT(TxFrame2Mem, ETH_TYPE_OFS) = SWAPB(FRAME_IP);
  
  // IP   
  ACCESS_UINT(TxFrame2Mem, IP_VER_IHL_TOS_OFS) = SWAPB(IP_VER_IHL);
  ACCESS_UINT(TxFrame2Mem, IP_TOTAL_LENGTH_OFS) = __swap_bytes(IP_HEADER_SIZE + ICMP_HEADER_SIZE + ICMPDataCount);
  ACCESS_UINT(TxFrame2Mem, IP_IDENT_OFS) = 0;
  ACCESS_UINT(TxFrame2Mem, IP_FLAGS_FRAG_OFS) = 0;
  ACCESS_UINT(TxFrame2Mem, IP_TTL_PROT_OFS) = SWAPB((DEFAULT_TTL << 8) | PROT_ICMP);
  ACCESS_UINT(TxFrame2Mem, IP_HEAD_CHKSUM_OFS) = 0;
  ACCESS_UINT(TxFrame2Mem, IP_SOURCE_OFS) = MyIP[0];
  ACCESS_UINT(TxFrame2Mem, IP_SOURCE_OFS + 2) = MyIP[1];
  ACCESS_UINT(TxFrame2Mem, IP_DESTINATION_OFS) = RecdFrameIP[0];
  ACCESS_UINT(TxFrame2Mem, IP_DESTINATION_OFS + 2) = RecdFrameIP[1];
  ACCESS_UINT(TxFrame2Mem, IP_HEAD_CHKSUM_OFS) = 
    CalcChecksum((unsigned char *)TxFrame2Mem + IP_VER_IHL_TOS_OFS, IP_HEADER_SIZE, 0);

  // ICMP
  ACCESS_UINT(TxFrame2Mem, ICMP_TYPE_CODE_OFS) = SWAPB(ICMP_ECHO_REPLY << 8);
  ACCESS_UINT(TxFrame2Mem, ICMP_CHKSUM_OFS) = 0;                // initialize checksum field
  CopyFromFrame((unsigned char *)TxFrame2Mem + ICMP_DATA_OFS, ICMPDataCount);  // get data to echo...
  ACCESS_UINT(TxFrame2Mem, ICMP_CHKSUM_OFS) = 
    CalcChecksum((unsigned char *)TxFrame2Mem + IP_DATA_OFS, ICMPDataCount + ICMP_HEADER_SIZE, 0);

  TxFrame2Size = ETH_HEADER_SIZE + IP_HEADER_SIZE + ICMP_HEADER_SIZE + ICMPDataCount;
  TransmitControl |= SEND_FRAME2;
}

//------------------------------------------------------------------------------
// Prepares the TxFrame1-buffer to send a general UDP data frame
//------------------------------------------------------------------------------
static void PrepareUDP_DATA_FRAME()
{
  // Ethernet
  ACCESS_UINT(TxFrame1Mem, ETH_DA_OFS) = RemoteMAC[0];
  ACCESS_UINT(TxFrame1Mem, ETH_DA_OFS + 2) = RemoteMAC[1];
  ACCESS_UINT(TxFrame1Mem, ETH_DA_OFS + 4) = RemoteMAC[2];
  ACCESS_UINT(TxFrame1Mem, ETH_SA_OFS) = MyMAC[0];
  ACCESS_UINT(TxFrame1Mem, ETH_SA_OFS + 2) = MyMAC[1];
  ACCESS_UINT(TxFrame1Mem, ETH_SA_OFS + 4) = MyMAC[2];
  ACCESS_UINT(TxFrame1Mem, ETH_TYPE_OFS) = SWAPB(FRAME_IP);
  
  // IP
  ACCESS_UINT(TxFrame1Mem, IP_VER_IHL_TOS_OFS) = SWAPB(IP_VER_IHL);
  ACCESS_UINT(TxFrame1Mem, IP_TOTAL_LENGTH_OFS) = SWAPB(IP_HEADER_SIZE + UDP_HEADER_SIZE + UDPTxDataCount);
  ACCESS_UINT(TxFrame1Mem, IP_IDENT_OFS) = 0;
  ACCESS_UINT(TxFrame1Mem, IP_FLAGS_FRAG_OFS) = 0;
  ACCESS_UINT(TxFrame1Mem, IP_TTL_PROT_OFS) = SWAPB((DEFAULT_TTL << 8) | PROT_UDP);
  ACCESS_UINT(TxFrame1Mem, IP_HEAD_CHKSUM_OFS) = 0;
  ACCESS_UINT(TxFrame1Mem, IP_SOURCE_OFS) = MyIP[0];
  ACCESS_UINT(TxFrame1Mem, IP_SOURCE_OFS + 2) = MyIP[1];
  ACCESS_UINT(TxFrame1Mem, IP_DESTINATION_OFS) = RemoteIP[0];
  ACCESS_UINT(TxFrame1Mem, IP_DESTINATION_OFS + 2) = RemoteIP[1];
  ACCESS_UINT(TxFrame1Mem, IP_HEAD_CHKSUM_OFS) = 
    CalcChecksum((unsigned char *)TxFrame1Mem + IP_VER_IHL_TOS_OFS, IP_HEADER_SIZE, 0);
  
  // UDP
  ACCESS_UINT(TxFrame1Mem, UDP_SRCPORT_OFS) = __swap_bytes(UDPLocalPort);
  ACCESS_UINT(TxFrame1Mem, UDP_DESTPORT_OFS) = __swap_bytes(UDPRemotePort);
  ACCESS_UINT(TxFrame1Mem, UDP_LENGTH_OFS) = SWAPB(UDP_HEADER_SIZE + UDPTxDataCount);      // data bytes to send
  ACCESS_UINT(TxFrame1Mem, UDP_CHKSUM_OFS) = 0;  // initalize checksum
  ACCESS_UINT(TxFrame1Mem, UDP_CHKSUM_OFS) =
      CalcChecksum((unsigned char *)TxFrame1Mem + UDP_SRCPORT_OFS, UDP_HEADER_SIZE + UDPTxDataCount, 2);

  TxFrame1Size = ETH_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE + UDPTxDataCount;
}

//------------------------------------------------------------------------------
// Calcula el checksum sobre el rango de direcciones indicado por Start y Count.
// Si pseudo-header == 1, incluye el pseudo-header del encabezado TCP
// Si pseudo-header == 2, incluye el pseudo-header del encabezado UDP
//------------------------------------------------------------------------------
static unsigned int CalcChecksum(void *Start, unsigned int Count, unsigned char pseudoHeader)
{
  unsigned long Sum = 0;
  unsigned int *pStart = (unsigned int *) Start;

  if(pseudoHeader)
  {
    // Calcula el checksum para el encabezado TCP o UDP
    // --> incluye el pseudo-encabezado IP
    Sum += MyIP[0];
    Sum += MyIP[1];
    Sum += RemoteIP[0];
    Sum += RemoteIP[1];
    Sum += __swap_bytes(Count);  // TCP/UDP header length plus data length
    switch(pseudoHeader)
    {
    case 1:
      Sum += SWAPB(PROT_TCP);
      break;
    case 2:
      Sum += SWAPB(PROT_UDP);
      break;
    }
  }
  
  while (Count > 1)                              // sum words
  {                            
    Sum += *pStart++;
    Count -= 2;
  }

  if (Count)                                     // add left-over byte, if any
    Sum += *(unsigned char *)pStart;
  
  while (Sum >> 16)                              // fold 32-bit sum to 16 bits
    Sum = (Sum & 0xFFFF) + (Sum >> 16);
  
  return ~Sum;
}

//------------------------------------------------------------------------------
// Timer A ISR
// Se ejecuta cada 262 ms en modo contínuo
// o cada 10 ms en modo "up", con un valor de recarga de 2500
// Usada para actualizar distintos timers
//------------------------------------------------------------------------------
#pragma vector = TIMERA1_VECTOR
__interrupt void TCPClockHandler(void)
{
  unsigned int i;
  
  if (TAIV == 10)                                // check for timer overflow, reset int.-flag
  {
    ++delayTimer;
    ++ARPTimer;
    ++waitLongPollTimer;
    ++sendMetersTimer;
    ++waitEventTimer;
    ++waitEventAckTimer;
    ++sendHelloTimer;
    ++waitCashReplyTimer;
    ++pollingTimer;
    ++cardReaderTimer;
    ++waitCodeSegTimer;
    ++rxByteTimer;
    ++waitClientCardTimer;
    ++blinkingLedTimer;
    
    if( watchdogEnabled() && ++WDTimer < _10seg )
      resetWatchdog();

    for(i=0; i<TOTAL_SERVERS; ++i) ServerAddr[i].MAC_age++;
    
    actualizaFechaHora();
  }
}

