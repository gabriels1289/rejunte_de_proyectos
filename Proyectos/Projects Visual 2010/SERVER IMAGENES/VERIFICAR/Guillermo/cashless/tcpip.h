//------------------------------------------------------------------------------
// Name: tcpip.h
// Archivo de cabecera de tcpip.c
//------------------------------------------------------------------------------

#ifndef __TCPIP_H
#define __TCPIP_H

#include "msp430x14x.h"

// Direcciones IP y máscaras por default

#define MYIP_1              172                 // our internet protocol (IP) address
#define MYIP_2              16
#define MYIP_3              0
#define MYIP_4              254

#define SUBMASK_1           255                 // subnet mask
#define SUBMASK_2           255
#define SUBMASK_3           0
#define SUBMASK_4           0

#define GWIP_1              172                // standard gateway (used if remote
#define GWIP_2              16                 // IP is no part of our subnet)
#define GWIP_3              0
#define GWIP_4              1

#define SERVER_EVT_IP_1     172                // IP del servidor de eventos
#define SERVER_EVT_IP_2     16
#define SERVER_EVT_IP_3     0
#define SERVER_EVT_IP_4     2

#define SERVER_MET_IP_1     172                // IP del servidor de contadores
#define SERVER_MET_IP_2     16
#define SERVER_MET_IP_3     0
#define SERVER_MET_IP_4     2

#define SERVER_CASH_IP_1    172                // IP del servidor de cashless
#define SERVER_CASH_IP_2    16
#define SERVER_CASH_IP_3    0
#define SERVER_CASH_IP_4    2

#define SERVER_MGMT_IP_1    172                // IP del servidor de administración
#define SERVER_MGMT_IP_2    16
#define SERVER_MGMT_IP_3    0
#define SERVER_MGMT_IP_4    2

#define SERVER_TOUCH_IP_1   172                // IP de la PC con touch screen
#define SERVER_TOUCH_IP_2   16
#define SERVER_TOUCH_IP_3   100
#define SERVER_TOUCH_IP_4   2

#define SERVER_GRAF_IP_1    172                // IP del servidor que envía gráficos
#define SERVER_GRAF_IP_2    16
#define SERVER_GRAF_IP_3    100
#define SERVER_GRAF_IP_4    2

// Estructura para almacenar las direcciones IP, MAC y antiguedad de la MAC de los servidores
typedef struct 
{
  unsigned int IP[2];
  unsigned int MAC[3];
  unsigned int MAC_age;
}ServerAddrType;

// Indices a las entradas correspondientes a los distintos servidores en la tabla ARP
#define SERV_EVENTS          0
#define SERV_METERS          1
#define SERV_CASH            2
#define SERV_MGMT            3
#define SERV_TOUCH           4
#define SERV_GRAF            5

#define TOTAL_SERVERS        6                   // Cantidad total de servidores

#define MAX_MAC_AGE          6000                // máx. tiempo sin refrescar la MAC del servidor

#define MAX_ETH_TX_DATA_SIZE 60                  // 2nd buffer, used for ARP, ICMP, TCP (even!)

#define MAX_UDP_TX_DATA_SIZE 144                 // max. outgoing UDP data size
#define MAX_UDP_RX_DATA_SIZE 546                 // max. incoming UDP data size

#define DEFAULT_TTL          64                  // Time To Live sent with packets

// Ethernet network layer definitions
#define ETH_DA_OFS           0                   // Destination MAC address (48 Bit)
#define ETH_SA_OFS           6                   // Source MAC address (48 Bit)
#define ETH_TYPE_OFS         12                  // Type field (16 Bit)
#define ETH_DATA_OFS         14                  // Frame Data
#define ETH_HEADER_SIZE      14

#define FRAME_ARP            (0x0806)            // frame types (stored in Type/Length field)
#define FRAME_IP             (0x0800)

// IPv4 layer definitions
#define IP_VER_IHL_TOS_OFS   (ETH_DATA_OFS + 0)  // Version, Header Length, Type of Service
#define IP_TOTAL_LENGTH_OFS  (ETH_DATA_OFS + 2)  // IP Frame's Total Length
#define IP_IDENT_OFS         (ETH_DATA_OFS + 4)  // Identifying Value
#define IP_FLAGS_FRAG_OFS    (ETH_DATA_OFS + 6)  // Flags and Fragment Offset
#define IP_TTL_PROT_OFS      (ETH_DATA_OFS + 8)  // Frame's Time to Live, Protocol
#define IP_HEAD_CHKSUM_OFS   (ETH_DATA_OFS + 10) // IP Frame's Header Checksum
#define IP_SOURCE_OFS        (ETH_DATA_OFS + 12) // Source Address (32 Bit)
#define IP_DESTINATION_OFS   (ETH_DATA_OFS + 16) // Destination Address (32 Bit)
#define IP_DATA_OFS          (ETH_DATA_OFS + 20) // Frame Data (if no options)
#define IP_HEADER_SIZE       20                  // w/o options

#define IP_VER_IHL           (0x4500)            // IPv4, Header Length = 5x32 bit
#define IP_TOS_D             (0x0010)            // TOS low delay
#define IP_TOS_T             (0x0008)            // TOS high throughput
#define IP_TOS_R             (0x0004)            // TOS high reliability

#define IP_FLAG_DONTFRAG     (0x4000)            // don't fragment IP frame
#define IP_FLAG_MOREFRAG     (0x2000)            // more fragments available
#define IP_FRAGOFS_MASK      (0x1fff)            // indicates where this fragment belongs

#define PROT_ICMP            1                   // Internet Control Message Protocol
#define PROT_TCP             6                   // Transmission Control Protocol
#define PROT_UDP             17                  // User Datagram Protocol

// ARP definitions
#define ARP_HARDW_OFS        (ETH_DATA_OFS + 0)  // Hardware address type
#define ARP_PROT_OFS         (ETH_DATA_OFS + 2)  // Protocol
#define ARP_HLEN_PLEN_OFS    (ETH_DATA_OFS + 4)  // byte length of each hardw. / prot. address
#define ARP_OPCODE_OFS       (ETH_DATA_OFS + 6)  // Opcode
#define ARP_SENDER_HA_OFS    (ETH_DATA_OFS + 8)  // Hardw. address of sender of this packet
#define ARP_SENDER_IP_OFS    (ETH_DATA_OFS + 14) // IP address of sender
#define ARP_TARGET_HA_OFS    (ETH_DATA_OFS + 18) // Hardw. address of target of this packet
#define ARP_TARGET_IP_OFS    (ETH_DATA_OFS + 24) // IP address of target
#define ARP_FRAME_SIZE       28

#define HARDW_ETH10          1                   // hardware-type 10Mbps Ethernet
#define IP_HLEN_PLEN         (0x0604)            // MAC = 6 byte long, IP = 4 byte long
#define OP_ARP_REQUEST       1                   // operations for ARP-frames
#define OP_ARP_ANSWER        2

// ICMP definitions
#define ICMP_TYPE_CODE_OFS   (IP_DATA_OFS + 0)   // type of message
#define ICMP_CHKSUM_OFS      (IP_DATA_OFS + 2)   // checksum of ICMP-message (16 bit)
#define ICMP_DATA_OFS        (IP_DATA_OFS + 4)
#define ICMP_HEADER_SIZE     4

#define ICMP_ECHO            8                   // message is an echo request
#define ICMP_ECHO_REPLY      0                   // message is an echo reply

// UDP layer definitions
#define UDP_SRCPORT_OFS      (IP_DATA_OFS + 0)   // Source Port (16 bit)
#define UDP_DESTPORT_OFS     (IP_DATA_OFS + 2)   // Destination Port (16 bit)
#define UDP_LENGTH_OFS       (IP_DATA_OFS + 4)   // Datagram Length (Data + UDP header) (16 bit)
#define UDP_CHKSUM_OFS       (IP_DATA_OFS + 6)   // Checksum Field (16 bit)
#define UDP_HEADER_SIZE      8                   // size w/o options

#define UDP_DATA_OFS         (IP_DATA_OFS + UDP_HEADER_SIZE)   // Datos del datagrama UDP

// Puertos UDP en el o los servidores
// Obs.: el servidor tiene 10 instancias del servicio de cashless,
// 1203, 1213, 1223, ...., 1293. La decena se define
// en función del 3er octeto de la dirección IP propia
#define UDP_PORT_HELLO       1200
#define UDP_PORT_EVENTS      1201
#define UDP_PORT_METERS      1202
#define UDP_PORT_CASH        (1203+((MyCfgIP[1]&255)%10)*10)
#define UDP_PORT_TEST        1204
#define UDP_PORT_MGMT        1205
#define UDP_PORT_TOUCH       1206

// Puertos locales
#define UDP_LOCAL_PORT_CLI   1234   // Para enviar eventos, contadores, cashless y hello
#define UDP_LOCAL_PORT_TEST  1204   // Para enviar y recibir mensajes de prueba
#define UDP_LOCAL_PORT_MGMT  1235   // Para enviar y recibir comandos y respuestas de gestión

// macro: word access to MSP430 memory location
#define ACCESS_UINT(Base, Offset)   (*(unsigned int *)((unsigned char *)&(Base) + (Offset)))
// macro: endianess conversion
#define SWAPB(Word)                 ((unsigned int)((Word) << 8) | ((Word) >> 8))

// definitions for 'TransmitControl'
#define SEND_FRAME1         (0x01)   // enviar trama de datos
#define SEND_FRAME2         (0x02)   // enviar trama con ARP o ICMP

void InitEthernet(void);             // Inicializa el controlador Ethernet

void InitIP(void);                   // inicializa direcciones MAC e IP

void DoNetworkStuff(void);           // network and TCP/IP event processing

                                     // envía un datagrama UDP al servidor indicado
unsigned char sendUDP(unsigned char * msg, unsigned int msglen, unsigned int nServer, unsigned int srcPort, unsigned int destPort);

void sendARP(unsigned int nServer);  // envía un requerimiento UDP al servidor indicado

__interrupt void TCPClockHandler(void);

// exported variables
extern __no_init unsigned int MyCfgIP[2];        // dirección IP configurable en la FLASH de datos

extern unsigned int MyIP[];                      // local IP address
extern unsigned int SubnetMask[];                // subnet mask (outbount connections)
extern unsigned int GatewayIP[];                 // gateway IP addr (outbount connections)

extern ServerAddrType ServerAddr[TOTAL_SERVERS]; // Direcciones IP y MAC de los servidores

extern unsigned int UDPLocalPort;                // UDP ports
extern unsigned int UDPRemotePort;
extern unsigned int RemoteMAC[3];                // MAC address of current TCP-session
extern unsigned int RemoteIP[2];                 // IP address of current TCP-session
extern unsigned int UDPRxDataCount;              // nr. of bytes rec'd (UDP_RX_BUF)
extern unsigned int UDPTxDataCount;              // nr. of bytes to send (UDP_TX_BUF)
extern unsigned int TxFrame1Mem[];               // outgoing TCP segment or UDP datagram
extern unsigned int TxFrame2Mem[];               // outgoing ARP or ICMP datagram
extern unsigned int RxUDPBufferMem[];            // data of received UDP segment

extern unsigned char TransmitControl;            // !=0 if anything to transmit
extern volatile unsigned int ARPTimer;           // time elapsed since ARP request

// Direcciones MAC, IP y Nro de puerto de la última trama recibida
extern unsigned int RecdFrameMAC[3];             // 48 bit MAC
extern unsigned int RecdFrameIP[2];              // 32 bit IP
extern unsigned int RecdFrameUDPPort;            // 16 bit UDP port

// Punteros a los buffers de datos UDP 
#define UDP_TX_BUF ((unsigned char *)TxFrame1Mem + ETH_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE)
#define UDP_RX_BUF ((unsigned char *)RxUDPBufferMem)

// Puntero al buffer para TX de datos ARP o ICMP 
// Obs.: este buffer se define externo para que pueda ser usado desde otros
// módulos con otra finalidad (por ej.: como buffer para transmisión de
// comandos a la VGM o al display)
#define ARP_ICMP_TX_BUF ((unsigned char *)TxFrame2Mem)

#endif
