// enc28j60.c
// manejo del controlador ethernet enc28j60 a través del puerto spi

#include "msp430x14x.h"
#include "support.h"
#include "tcpip.h"
#include "cs8900.h"
#include "enc28j60.h"
#include "cashless.h"

#define RX_BUF_START 0x0000
#define RX_BUF_END   0x19ff
#define TX_BUF_START 0x1a00
#define TX_BUF_END   0x1fff

// Funciones para la configuración y manejo del puerto SPI
// P5.0 : STE1   (al CS)
// P5.1 : SIMO1  (al SI)
// P5.2 : SOMI1  (del SO)
// P5.3 : UCLK1  (al SCK)

static unsigned int nextPacketPointer;   // Dirección de la próxima trama a leer del Buffer de Rx
                                         // Se actualiza con cada trama recibida
//------------------------------
// Inicializa el puerto SPI1
//------------------------------
void spi_init()
{
  P5SEL = 0x0e;     // P5.1, P5.2 y P5.3 SPI - El resto GPIO
  P5DIR |= 0x01;    // P5.0 configurado como salida
  P5OUT |= 0x01;    // P5.0 en 1
  
  U1CTL = CHAR | SYNC | MM | SWRST;  // 8 bits char, USART=SPI, master, reset 
  U1TCTL = CKPH | SSEL0 | STC;       // sck=idle low, spiclk=ACLK, 3 wires 
  U1BR1 = 0x00; 
  U1BR0 = 0x02;                      // spi_clk = ACLK/2 = 1 Mhz 
  U1MCTL = 0x00;                     // modulation factor = 0 
  ME2 |= USPIE1;                     // enable module
  U1CTL &= ~SWRST;                   // leave RESET, enable SPI
}

//------------------------------
// Activa CS del ENC28J60
//------------------------------
void spi_start(void)
{
  P5OUT |= 0x01;        // se asegura que CS (P5.0) esté en 1 antes de empezar
  P5OUT &= 0xfe;        // pone CS a 0
}

//------------------------------
// Desactiva CS del ENC28J60
//------------------------------
void spi_stop(void)
{
  P5OUT &= 0xfe;        // se asegura que CS (P5.0) esté en 0 antes de empezar
  P5OUT |= 0x01;        // pone CS a 1
}

//-------------------------------------
// Transmite un byte por el puerto SPI
//-------------------------------------
void spi_wr(unsigned char data)
{
  while( !(IFG2&UTXIFG1) );     // espera que el buffer de transmisión se libere
  
  TXBUF1 = data;                // envía el dato 
  
  while( !(IFG2&URXIFG1) );     // espera que se complete el ciclo TX / RX
  
  IFG2 &= ~URXIFG1;             // resetea el flag de completo 
}                                                    

//----------------------------------------
// Lee un byte recibido por el puerto SPI
//----------------------------------------
unsigned char spi_rd(void)
{
  spi_wr(0xff);           // envía FF para mover el clock y obtener la respuesta
  return RXBUF1;          // retorna el byte recibido
} 

//----------------------------------------
// Lee un byte de un registro del ENC28J60
//----------------------------------------
unsigned char read28J60(unsigned char opcode, unsigned char addr, unsigned char dummyByte)
{
  spi_start();
  spi_wr((addr & 0x1f)|opcode);   // envía comando y dirección
  if(dummyByte)
    spi_wr(0xff);                 // envía FF para mover el clock y obtener un byte dummy
  spi_wr(0xff);                   // envía FF para mover el clock y obtener la respuesta
  spi_stop();
  return RXBUF1;                  // retorna el byte recibido
} 

//---------------------------------------------
// Escribe en un registro de control
//---------------------------------------------
void write28J60(unsigned char opcode, unsigned char addr, unsigned char data)
{
  spi_start();
  spi_wr((addr & 0x1f)|opcode);   // envía comando y dirección
  spi_wr(data);                   // envía dato
  spi_stop();
}

//-------------------------------------------------
// Retorna una palabra obtenida del buffer de Rx
// recibida por el ENC28J60 (little-endian)
//-------------------------------------------------
unsigned int ReadFrame28J60()
{
  unsigned int h, l;
  
  spi_start();
  spi_wr(OPC_RBM);        // envía comando

  spi_wr(0xff);           // envía FF para mover el clock y obtener un byte
  l = RXBUF1;             // almacena el byte recibido (LSB)

  spi_wr(0xff);           // envía FF para mover el clock y obtener otro
  h = RXBUF1;             // almacena el byte recibido (MSB)

  spi_stop();

  return (h<<8)|l;
} 

//-------------------------------------------------
// Retorna una palabra obtenida del buffer de Rx
// recibida por el ENC28J60 (big-endian)
//-------------------------------------------------
unsigned int ReadFrameBE28J60()
{
  unsigned int h, l;
  
  spi_start();
  spi_wr(OPC_RBM);        // envía comando

  spi_wr(0xff);           // envía FF para mover el clock y obtener un byte
  h = RXBUF1;             // almacena el byte recibido (LSB)

  spi_wr(0xff);           // envía FF para mover el clock y obtener otro
  l = RXBUF1;             // almacena el byte recibido (MSB)

  spi_stop();

  return (h<<8)|l;
}

//--------------------------------------------------
// Lee nbytes del buffer de Rx y los deposita en buf
//--------------------------------------------------
void CopyFromFrame28J60(unsigned char * buf, unsigned int nbytes)
{
  unsigned int i;
  
  spi_start();
  spi_wr(OPC_RBM);        // envía comando

  for(i=0; i<nbytes; ++i)
  {
    spi_wr(0xff);           // envía FF para mover el clock y obtener un byte
    buf[i] = RXBUF1;        // almacena el byte recibido
  }
  spi_stop();
} 

//--------------------------------------------------
// Lee nbytes del buffer de Rx y los descarta
//--------------------------------------------------
void DummyReadFrame28J60(unsigned int nbytes)
{
  unsigned int i;
  
  spi_start();
  spi_wr(OPC_RBM);        // envía comando

  for(i=0; i<nbytes; ++i)
  {
    spi_wr(0xff);           // envía FF para mover el clock y obtener un byte (que descarta)
  }
  spi_stop();
} 

//---------------------------------------------
// Escribe nbytes en el buffer de transmisión
// y los envía por Ethernet
//---------------------------------------------
void CopyToFrame28J60(unsigned char *buf, unsigned int nbytes)
{
  unsigned int i, endTxBuf=0x1a00 + nbytes;

//static unsigned char txStatus[6];  // Esto sería para verificar el estado de la transmisión
  
  // Selecciona banco 0
  write28J60(OPC_BFC, ECON1, BSEL1 | BSEL0);

  write28J60(OPC_WCR, ETXSTL, 0x00);
  write28J60(OPC_WCR, ETXSTH, 0x1a);

  write28J60(OPC_WCR, EWRPTL, 0x00);
  write28J60(OPC_WCR, EWRPTH, 0x1a);

  spi_start();
  spi_wr(OPC_WBM);        // envía comando
  spi_wr(0x0e);           // per packet control byte (0x0e --> PHUGEEN, PPADN, PCRCEN)
  for(i=0; i<nbytes; ++i)
  {
    spi_wr( buf[i] );   // envía el próximo byte
  }
  spi_stop();

  write28J60(OPC_WCR, ETXNDL, endTxBuf&0xff);
  write28J60(OPC_WCR, ETXNDH, (endTxBuf>>8)&0xff);

  write28J60(OPC_BFC, EIR, TXIF);
  write28J60(OPC_BFS, EIE, TXIE | INTIE);

  // ERRATAFIX
  write28J60(OPC_BFS, ECON1, TXRST);
  write28J60(OPC_BFC, ECON1, TXRST);
  write28J60(OPC_BFC, EIR, TXERIF | TXIF);

  // Inicia transmisión
  write28J60(OPC_BFS, ECON1, TXRTS);
  
  // Espera fin de transmisión
  while( !(read28J60(OPC_RCR, EIR, 0) & TXIF) );

  // Espera 1 ms
  for(i=0; i<40; ++i)
    DelayCycles(200);
 
  // Resetea bit de transmisión
  write28J60(OPC_BFC, ECON1, TXRTS);  

/*
  // Esto sería para leer el estado con que finalizó la transmisión
  write28J60(OPC_WCR, ERDPTL, (endTxBuf+1) & 0xff);
  write28J60(OPC_WCR, ERDPTH, (endTxBuf+1) >> 8);

  CopyFromFrame28J60(txStatus,6);
*/
}

//--------------------------------------------------
// Escribe en un registro físico (PHY) del ENC28J60
//--------------------------------------------------
void write28J60PHY(unsigned char phyReg, unsigned int data)
{
  // Selecciona banco 2
  write28J60(OPC_BFC, ECON1, BSEL0);
  write28J60(OPC_BFS, ECON1, BSEL1);

  // Configura registro PHY
  write28J60(OPC_WCR, MIREGADR, phyReg);
  write28J60(OPC_WCR, MIWRL, data & 0xff);
  write28J60(OPC_WCR, MIWRH, (data >> 8) & 0xff);

  // Selecciona banco 3
  write28J60(OPC_BFS, ECON1, BSEL0);
  write28J60(OPC_BFS, ECON1, BSEL1);

  // Espera que se complete la operación anterior
  while( read28J60(OPC_RCR, MISTAT, 1) & MIBUSY );
}

//---------------------------------------------
// Inicializa el ENC28J60
//---------------------------------------------
void Init28J60()
{
  unsigned int i, j;

  // Espera que se estabilice el oscilador
  while( !(read28J60(OPC_RCR, ESTAT, 0) & CLKRDY) );

  // Espera 5 ms
  DelayCycles(40000);

  // Selecciona banco 3
  write28J60(OPC_BFS, ECON1, BSEL1 | BSEL0);
/*
  // Esto sería para consultar la revisión de hardware del ENC28J60
  // Obs.: declarar unsigned char rev;
  rev = read28J60(OPC_RCR, EREVID, 0);        // Consulta la revisión de hardware del ENC28J60
*/
  // System reset
  spi_start();
  spi_wr(OPC_SRC);
  spi_stop();

  // Espera 5 ms
  DelayCycles(40000);
  
  // Deshabilita Rx
  write28J60(OPC_BFC, ECON1, RXEN);

  // Mantiene las lógicas de Tx y Rx reseteadas
  write28J60(OPC_BFS, ECON1, RXRST);
  write28J60(OPC_BFS, ECON1, TXRST);

  // Selecciona banco 0
  write28J60(OPC_BFC, ECON1, BSEL1 | BSEL0);

  // Buffer de transmisión
  // Inicio
  write28J60(OPC_WCR, ETXSTL, TX_BUF_START & 0xff);
  write28J60(OPC_WCR, ETXSTH, (TX_BUF_START >> 8) & 0xff);
  // Fin
  write28J60(OPC_WCR, ETXNDL, TX_BUF_END & 0xff);
  write28J60(OPC_WCR, ETXNDH, (TX_BUF_END >> 8) & 0xff);
  // Puntero al buffer de transmisión
  write28J60(OPC_WCR, EWRPTL, TX_BUF_START & 0xff);
  write28J60(OPC_WCR, EWRPTH, (TX_BUF_START >> 8) & 0xff);

  // Buffer de recepción
  // Inicio
  write28J60(OPC_WCR, ERXSTL, RX_BUF_START & 0xff);
  write28J60(OPC_WCR, ERXSTH, (RX_BUF_START >> 8) & 0xff);
  // Fin
  write28J60(OPC_WCR, ERXNDL, RX_BUF_END & 0xff);
  write28J60(OPC_WCR, ERXNDH, (RX_BUF_END >> 8) & 0xff);
  // Puntero al buffer de recepción
  write28J60(OPC_WCR, ERDPTL, RX_BUF_START & 0xff);
  write28J60(OPC_WCR, ERDPTH, (RX_BUF_START >> 8) & 0xff);

  nextPacketPointer = 0;  

  write28J60(OPC_WCR, ERXRDPTL, RX_BUF_END & 0xff);
  write28J60(OPC_WCR, ERXRDPTH, (RX_BUF_END >> 8) & 0xff);

  // Selecciona banco 1
  write28J60(OPC_BFS, ECON1, BSEL0);
  write28J60(OPC_BFC, ECON1, BSEL1);

  // Rx filter control
  write28J60(OPC_WCR, ERXFCON, UCEN | CRCEN | BCEN);

  // Selecciona banco 2
  write28J60(OPC_BFC, ECON1, BSEL0);
  write28J60(OPC_BFS, ECON1, BSEL1);

  // Configura Rx
  write28J60(OPC_BFS, MACON1, MARXEN);
  write28J60(OPC_WCR, MACLCON2, 63);
  // pad 60+CRC / CRC / verifica type/len / half-duplex
  write28J60(OPC_WCR, MACON3, PADCFG0 | TXCRCEN | FRMLNEN);
  // cuando tiene que transmitir, espera indefinidamente hasta que el medio esté libre
  write28J60(OPC_BFS, MACON4, DEFER);
  // Max length 1518 bytes  
  write28J60(OPC_WCR, MAMXFLL, 1518 & 0x00ff);
  write28J60(OPC_WCR, MAMXFLH, 1518 >> 8);
  // Back-to-back interpacket gap
  write28J60(OPC_WCR, MABBIPG, 0x12);
  write28J60(OPC_WCR, MAIPGL, 0x12);
  write28J60(OPC_WCR, MAIPGH, 0x0c);
  
  // Selecciona banco 3
  write28J60(OPC_BFS, ECON1, BSEL0);
  write28J60(OPC_BFS, ECON1, BSEL1);

  // Configura dirección MAC  
  // Si tiene configurada una dirección MAC en la flash de datos,
  // la obtiene de allí, si no, usa la dirección MAC por defecto
  if( MyCfgMAC[0] == 0xFFFF && MyCfgMAC[1] == 0xFFFF && MyCfgMAC[2] == 0xFFFF)
  {
    // No tiene configurada una MAC en la Flash de datos --> usa la MAC default
    MyMAC[0] = MYMAC_1 + (unsigned int)(MYMAC_2 << 8);
    MyMAC[1] = MYMAC_3 + (unsigned int)(MYMAC_4 << 8);
    MyMAC[2] = MYMAC_5 + (unsigned int)(MYMAC_6 << 8);
  }else{
    // Toma la MAC configurada en la flash de datos
    // Obs.: el primer bit debe ser cero (IA)
    MyMAC[0] = MyCfgMAC[0];
    MyMAC[1] = MyCfgMAC[1];
    MyMAC[2] = MyCfgMAC[2];
  }
  write28J60(OPC_WCR, MAADR1, MyMAC[0]&0xff);
  write28J60(OPC_WCR, MAADR2, MyMAC[0]>>8);
  write28J60(OPC_WCR, MAADR3, MyMAC[1]&0xff);
  write28J60(OPC_WCR, MAADR4, MyMAC[1]>>8);
  write28J60(OPC_WCR, MAADR5, MyMAC[2]&0xff);
  write28J60(OPC_WCR, MAADR6, MyMAC[2]>>8);
 
  write28J60(OPC_BFS, ECON2, AUTOINC);
  
  // Configura registros PHY
  write28J60PHY(PHCON2, HDLDIS);  // Inhabilita el loopback
  write28J60PHY(PHCON1, 0x0000);  // Habilita Tx
  write28J60PHY(PHLCON, 0x3126);  // LedA Tx, LedB Rx, parpadeo 70 ms

  // Quita el reset de las lógicas de Tx y Rx
  write28J60(OPC_BFC, ECON1, RXRST);
  write28J60(OPC_BFC, ECON1, TXRST);

  // Habilita Rx
  write28J60(OPC_BFS, ECON1, RXEN);

  // vacía el buffer de Rx
  while( Rx28J60(&i, &j) );
  
  // Resetea flags indicadores de interrupción
  write28J60( OPC_WCR, EIR, 0 );

  // Espera 100 ms
  for(i=0; i<20; ++i)
    DelayCycles(40000);
}

//-------------------------------------------------------
// Verifica si hay una trama recibida OK por el ENC28J60
// Retorna : 0 (no o trama defectuosa)
//           1 (si)
// En caso de haber una trama recibida, actualiza la
// variable nextPacketPointer y retorna la longitud
// y la palabra de estado de la trama recibida.
//-------------------------------------------------------
unsigned int Rx28J60(unsigned int * frameLength, unsigned int * frameStatus)
{
  write28J60(OPC_BFC, ECON1, BSEL1 | BSEL0);  // Banco 0

  // Apunta a la próxima trama recibida
  write28J60(OPC_WCR, ERDPTL, nextPacketPointer & 0xff);
  write28J60(OPC_WCR, ERDPTH, nextPacketPointer >> 8);

  // Banco 1
  write28J60(OPC_BFS, ECON1, BSEL0);
  write28J60(OPC_BFC, ECON1, BSEL1);
  
  // Consulta si hay alguna trama recibida
  if( read28J60(OPC_RCR, EPKTCNT, 0) )
  {
    // Hay una trama recibida --> actualiza el puntero a la próxima trama
    nextPacketPointer = ReadFrame28J60();
    *frameLength = ReadFrame28J60();
    *frameStatus = ReadFrame28J60();

    return 1;
  }
  return 0;
}

//---------------------------------------------------------
// Libera el espacio ocupado por la trama recién procesada
//---------------------------------------------------------
void free28J60RxBuffer()
{
  unsigned int npp;

  if( nextPacketPointer == RX_BUF_START )
    npp = RX_BUF_END;
  else
    npp = nextPacketPointer - 1;
    
  write28J60(OPC_BFC, ECON1, BSEL1 | BSEL0);  // Banco 0

  // Libera el espacio de la última trama leida
  write28J60(OPC_WCR, ERXRDPTL, npp & 0xff);
  write28J60(OPC_WCR, ERXRDPTH, npp >> 8);

  // Decrementa el contador de tramas recibidas
  write28J60(OPC_BFS, ECON2, PKTDEC);
}

//---------------------------------------------
// Testea el ENC28J60
//---------------------------------------------
void test28J60()
{
/*
  static unsigned int i, st, n, b[4];
  
  write28J60(OPC_BFC, ECON1, BSEL1 | BSEL0);  // Banco 0
  write28J60(OPC_WCR, ERDPTL, 0);
  write28J60(OPC_WCR, ERDPTH, 0);

  write28J60(OPC_BFS, ECON1, BSEL0);  // Banco 1
  write28J60(OPC_WCR, EPKTCNT, 0);
  
  while( !Rx28J60(&n, &st) );

  CopyFromFrame28J60((unsigned char *)&RecdFrameMAC, 6);           // store SA (for our answer)
  CopyFromFrame28J60((unsigned char *)&RecdFrameMAC, 6);           // store SA (for our answer)
    
  for(i=0; i<4; ++i)
    b[i] = ReadFrame28J60();

  return;
*/
}

