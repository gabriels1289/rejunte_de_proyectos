// enc28j60.h
// header file for enc28j60.c

// Direcciones de los registros de control

// Comunes a los cuatro bancos
#define EIE       0x1b
#define EIR       0x1c
#define ESTAT     0x1d
#define ECON2     0x1e
#define ECON1     0x1f     // Ethernet Control Register 1

// Bank 0
#define ERDPTL    0x00
#define ERDPTH    0x01
#define EWRPTL    0x02
#define EWRPTH    0x03
#define ETXSTL    0x04   // Dirección 1er byte de la trama a transmitir (LSB)
#define ETXSTH    0x05   // Dirección 1er byte de la trama a transmitir (MSB)
#define ETXNDL    0x06   // Dirección último byte de la trama a transmitir (LSB)
#define ETXNDH    0x07   // Dirección último byte de la trama a transmitir (MSB)
#define ERXSTL    0x08   // Comienzo del buffer de recepción (LSB)
#define ERXSTH    0x09   // Comienzo del buffer de recepción (MSB)
#define ERXNDL    0x0a   // Fin del buffer de recepción (LSB)
#define ERXNDH    0x0b   // Fin del buffer de recepción (MSB)
#define ERXRDPTL  0x0c
#define ERXRDPTH  0x0d
#define ERXWRPTL  0x0e
#define ERXWRPTH  0x0f
#define EDMASTL   0x10
#define EDMASTH   0x11
#define EDMANDL   0x12
#define EDMANDH   0x13
#define EDMADSTL  0x14
#define EDMADSTH  0x15
#define EDMACSL   0x16
#define EDMACSH   0x17

// Bank 1
#define ETH0      0x00
#define ETH1      0x01
#define ETH2      0x02
#define ETH3      0x03
#define ETH4      0x04
#define ETH5      0x05
#define ETH6      0x06
#define ETH7      0x07
#define EPMM0     0x08
#define EPMM1     0x09
#define EPMM2     0x0a
#define EPMM3     0x0b
#define EPMM4     0x0c
#define EPMM5     0x0d
#define EPMM6     0x0e
#define EPMM7     0x0f
#define EPMCSL    0x10
#define EPMCSH    0x11
#define EPMOL     0x14
#define EPMOH     0x15
#define ERXFCON   0x18
#define EPKTCNT   0x19

// Bank 2
#define MACON1    0x00
#define MACON3    0x02
#define MACON4    0x03
#define MABBIPG   0x04
#define MAIPGL    0x06
#define MAIPGH    0x07
#define MACLCON1  0x08
#define MACLCON2  0x09
#define MAMXFLL   0x0a
#define MAMXFLH   0x0b
#define MICMD     0x12
#define MIREGADR  0x14
#define MIWRL     0x16
#define MIWRH     0x17
#define MIRDL     0x18
#define MIRDH     0x19

// Bank 3
#define MAADR5    0x00
#define MAADR6    0x01
#define MAADR3    0x02
#define MAADR4    0x03
#define MAADR1    0x04
#define MAADR2    0x05
#define EBSTSD    0x06
#define EBSTCON   0x07
#define EBSTCSL   0x08
#define EBSTCSH   0x09
#define MISTAT    0x0a
#define EREVID    0x12
#define ECOCON    0x15
#define EFLOCON   0x17
#define EPAUSL    0x18
#define EPAUSH    0x19

// PHY Registers
#define PHCON1    0x00
#define PHSTAT1   0x01
#define PHID1     0x02
#define PHID2     0x03
#define PHCON2    0x10
#define PHSTAT2   0x11
#define PHIE      0x12
#define PHIR      0x13
#define PHLCON    0x14

// Definiciones de bits de cada registro

// EIE
#define TXIE      0x08
#define INTIE     0x80

// EIR
#define TXIF      0x08
#define TXERIF    0x02
#define RXERIF    0x01

// ECON1
#define TXRST     0x80    // Transmit logic reset
#define RXRST     0x40    // Receive logic reset
#define DMAST     0x20
#define CSUMEN    0x10
#define TXRTS     0x08    // Transmit request to send
#define RXEN      0x04    // Receive enable
#define BSEL1     0x02    // Bank select bit 1
#define BSEL0     0x01    // Bank select bit 0

// ECON2
#define AUTOINC   0x80
#define PKTDEC    0x40

// ESTAT
#define CLKRDY    0x01

// ERXFCON
#define UCEN      0x80
#define CRCEN     0x20
#define BCEN      0x01

// MACON1
#define MARXEN    0x01
#define PASSALL   0x02
#define RXPAUS    0x04
#define TXPAUS    0x08

// MACON3
#define FULDPX    0x01
#define FRMLNEN   0x02
#define HFRMEN    0x04
#define PHDREN    0x08
#define TXCRCEN   0x10
#define PADCFG0   0x20
#define PADCFG1   0x40
#define PADCFG2   0x80

// MACON4
#define DEFER     0x40

// MICMD
#define MIIRD     0x01

// MISTAT
#define MIBUSY    0x01

// PHCON1
#define PDPXMD    0x0100

// PHCON2
#define HDLDIS    0x0100

// Opcodes de los distintos comandos (bits 7-6-5)

#define OPC_RCR   0    // Read control register (opcode 0)
#define OPC_RBM   0x3a // Read buffer memory (opcode 1 y argument 0x1a)
#define OPC_WCR   0x40 // Write control register (opcode 2)
#define OPC_WBM   0x7a // Write buffer memory (opcode 3 y argument 0x1a)
#define OPC_BFS   0x80 // Bit field set (opcode 4)
#define OPC_BFC   0xa0 // Bit field clear (opcode 5)
#define OPC_SRC   0xff // System reset command (opcode 7 y argument 0x1f)

// Máscaras para testear el estado de una trama recibida

#define RX_28J60_OK        0x0080
#define RX_28J60_BROADCAST 0x0200
#define RX_28J60_MULTICAST 0x0100

// Prototipos de funciones

void spi_init(void);
void spi_start(void);
void spi_stop(void);
void spi_wr(unsigned char data);
unsigned char spi_rd(void);
unsigned char read28J60(unsigned char opcode, unsigned char addr, unsigned char dummy_byte);
void write28J60(unsigned char opcode, unsigned char addr, unsigned char data);
unsigned int ReadFrame28J60(void);
unsigned int ReadFrameBE28J60(void);
void CopyToFrame28J60(unsigned char *buf, unsigned int nbytes);
void CopyFromFrame28J60(unsigned char *buf, unsigned int nbytes);
void DummyReadFrame28J60(unsigned int nbytes);
void Init28J60(void);
void test28J60(void);
unsigned int Rx28J60(unsigned int * frameLength, unsigned int * frameStatus);
void write28J60PHY(unsigned char phyReg, unsigned int data);
void free28J60RxBuffer(void);
