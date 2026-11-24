/*******************************************************************************
 * @brief   ENC28J60 Ethernet controller driver.
 *
 *
 * @file    enc28j60.h
 * @author  masu (masu@noemail.non)
 * @date    2013-02-25
 * @version 1.0
 *
 * @note    This is a note.
 *
 * @since   2013-02-13 Initial version.
 ******************************************************************************/

#include "typedef.h"
#include "massert.h"
#include "drvEnc28j60.h"
#include "enc28j60.h"

static uint8_t  enc28j60_opRead     (const t_enc28j60Ctrl * apCtrl, uint8_t aCmd, uint8_t aAddr);
static void     enc28j60_opWrite    (const t_enc28j60Ctrl * apCtrl, uint8_t aCmd, uint8_t aAddr, uint8_t aData);
static void     enc28j60_bufferRead (const t_enc28j60Ctrl * apCtrl, uint8_t * apBuf, uint16_t aLen);
static void     enc28j60_bufferWrite(const t_enc28j60Ctrl * apCtrl, const uint8_t * apBuf, uint16_t aLen);
static void     enc28j60_bankSet    (const t_enc28j60Ctrl * apCtrl, uint8_t aAddr);
static uint8_t  enc28j60_rcr        (const t_enc28j60Ctrl * apCtrl, uint8_t aAddr);
static uint16_t enc28j60_rcr16      (const t_enc28j60Ctrl * apCtrl, uint8_t aAddr);
static void     enc28j60_wcr        (const t_enc28j60Ctrl * apCtrl, uint8_t aAddr, uint8_t aData);
static void     enc28j60_wcr16      (const t_enc28j60Ctrl * apCtrl, uint8_t aAddr, uint16_t aData);
static void     enc28j60_bfc        (const t_enc28j60Ctrl * apCtrl, uint8_t aAddr, uint8_t aMask);
static void     enc28j60_bfs        (const t_enc28j60Ctrl * apCtrl, uint8_t aAddr, uint8_t aMask);
static void     enc28j60_reset      (const t_enc28j60Ctrl * apCtrl);
static uint16_t enc28j60_phyRead	(const t_enc28j60Ctrl * apCtrl, uint8_t aAddr);
static void     enc28j60_phyWrite	(const t_enc28j60Ctrl * apCtrl, uint8_t aAddr, uint16_t aData);

static void enc28j60_attachIface  (t_enc28j60Ctrl * apCtrl, const t_enc28j60Init * apInit);
static void enc28j60_waitForClkRdy(const t_enc28j60Ctrl * apCtrl);
static void enc28j60_setMacAddr   (const t_enc28j60Ctrl * apCtrl, const unsigned char * apMacAddr);

static uint8_t  s_enc28j60_current_bank = 0;
static uint16_t s_enc28j60_rxrdpt       = 0;

#define enc28j60_select()   apCtrl->fcn_IfaceCsSelect()
#define enc28j60_unselect() apCtrl->fcn_IfaceCsUnselect()
#define enc28j60_rx() 	    apCtrl->fcn_IfaceReadWriteByte(0xff)
#define enc28j60_tx(data)   apCtrl->fcn_IfaceReadWriteByte(data)

/*!< ENC28J60 read command */
static uint8_t enc28j60_opRead(const t_enc28j60Ctrl * apCtrl, uint8_t aCmd, uint8_t aAddr)
{
	uint8_t data;

	enc28j60_select();

	enc28j60_tx(aCmd | (aAddr & ENC28J60_ADDR_MASK));
	data = enc28j60_rx();
	if(aAddr & ENC28J60_MACMII)
		data = enc28j60_rx();

	enc28j60_unselect();

	return data;
}

/*!< ENC28J60 write command */
static void enc28j60_opWrite(const t_enc28j60Ctrl * apCtrl, uint8_t aCmd, uint8_t aAddr, uint8_t aData)
{
	enc28j60_select();

	enc28j60_tx(aCmd | (aAddr & ENC28J60_ADDR_MASK));
	enc28j60_tx(aData);

	enc28j60_unselect();
}

// Read Rx/Tx buffer (at ERDPT)
static void enc28j60_bufferRead(const t_enc28j60Ctrl * apCtrl, uint8_t * apBuf, uint16_t aLen)
{
	enc28j60_select();

	enc28j60_tx(ENC28J60_SPI_RBM);
	while(aLen--)
		*(apBuf++) = enc28j60_rx();

	enc28j60_unselect();
}

// Write Rx/Tx buffer (at EWRPT)
static void enc28j60_bufferWrite(const t_enc28j60Ctrl * apCtrl, const uint8_t * apBuf, uint16_t aLen)
{
	enc28j60_select();

	enc28j60_tx(ENC28J60_SPI_WBM);
	while(aLen--)
		enc28j60_tx(*(apBuf++));

	enc28j60_unselect();
}

// Set register bank
static void enc28j60_bankSet(const t_enc28j60Ctrl * apCtrl, uint8_t aAddr)
{
	uint8_t bank;

	if (REQUIRE_BANK_SWITCH(aAddr) && \
		((aAddr & ENC28J60_BANK_MASK) != s_enc28j60_current_bank))
	{
		bank = (aAddr & ENC28J60_BANK_MASK) >> 5;

		enc28j60_opWrite(apCtrl, ENC28J60_SPI_BFC, ECON1, ECON1_BSEL1 | ECON1_BSEL0);
		enc28j60_opWrite(apCtrl, ENC28J60_SPI_BFS, ECON1, bank);

		s_enc28j60_current_bank = (aAddr & ENC28J60_BANK_MASK);
	}
}

// Read register
static uint8_t enc28j60_rcr(const t_enc28j60Ctrl * apCtrl, uint8_t aAddr)
{
	enc28j60_bankSet(apCtrl, aAddr);
	return enc28j60_opRead(apCtrl, ENC28J60_SPI_RCR, aAddr);
}

// Read register pair
static uint16_t enc28j60_rcr16(const t_enc28j60Ctrl * apCtrl, uint8_t aAddr)
{
	enc28j60_bankSet(apCtrl, aAddr);
	return enc28j60_opRead(apCtrl, ENC28J60_SPI_RCR, aAddr) | \
		  (enc28j60_opRead(apCtrl, ENC28J60_SPI_RCR, aAddr+1) << 8);
}

// Write register
static void enc28j60_wcr(const t_enc28j60Ctrl * apCtrl, uint8_t aAddr, uint8_t aData)
{
	enc28j60_bankSet(apCtrl, aAddr);
	enc28j60_opWrite(apCtrl, ENC28J60_SPI_WCR, aAddr, aData);
}

// Write register pair
static void enc28j60_wcr16(const t_enc28j60Ctrl * apCtrl, uint8_t aAddr, uint16_t aData)
{
	enc28j60_bankSet(apCtrl, aAddr);
	enc28j60_opWrite(apCtrl, ENC28J60_SPI_WCR, aAddr,   aData);
	enc28j60_opWrite(apCtrl, ENC28J60_SPI_WCR, aAddr+1, aData >> 8);
}

// Clear bits in register (reg &= ~mask)
static void enc28j60_bfc(const t_enc28j60Ctrl * apCtrl, uint8_t aAddr, uint8_t aMask)
{
	enc28j60_bankSet(apCtrl, aAddr);
	enc28j60_opWrite(apCtrl, ENC28J60_SPI_BFC, aAddr, aMask);
}

// Set bits in register (reg |= mask)
static void enc28j60_bfs(const t_enc28j60Ctrl * apCtrl, uint8_t aAddr, uint8_t aMask)
{
	enc28j60_bankSet(apCtrl, aAddr);
	enc28j60_opWrite(apCtrl, ENC28J60_SPI_BFS, aAddr, aMask);
}

// Initiate software reset
static void enc28j60_reset(const t_enc28j60Ctrl * apCtrl)
{
	enc28j60_select();

	enc28j60_tx(ENC28J60_SPI_SR);

	enc28j60_unselect();

	s_enc28j60_current_bank = ENC28J60_BANK0;

	/*!<
	 *  errata :: After sending an SPI Reset command, the PHY
	 *	clock is stopped but the ESTAT.CLKRDY bit is not
	 *	cleared.
	 */

	apCtrl->fcn_IfaceTimerUs(1000);
}

// Read PHY register
static uint16_t enc28j60_phyRead(const t_enc28j60Ctrl * apCtrl, uint8_t aAddr)
{
	enc28j60_wcr(apCtrl, MIREGADR, aAddr);
	enc28j60_bfs(apCtrl, MICMD, MICMD_MIIRD);

	apCtrl->fcn_IfaceTimerUs(11);	/* 10.24 us */
	while(enc28j60_rcr(apCtrl, MISTAT) & MISTAT_BUSY) {} ;

	enc28j60_bfc(apCtrl, MICMD, MICMD_MIIRD);
	return enc28j60_rcr16(apCtrl, MIRD);
}

// Write PHY register
static void enc28j60_phyWrite(const t_enc28j60Ctrl * apCtrl, uint8_t aAddr, uint16_t aData)
{
	enc28j60_wcr(apCtrl, MIREGADR, aAddr);
	enc28j60_wcr16(apCtrl, MIWR, aData);

	apCtrl->fcn_IfaceTimerUs(11);	/* 10.24 us */
	while(enc28j60_rcr(apCtrl, MISTAT) & MISTAT_BUSY) {} ;
}

static void enc28j60_attachIface(t_enc28j60Ctrl * apCtrl, const t_enc28j60Init * apInit)
{
	mASSERT(IS_ENC28J60INIT_VALID(apInit));

	apCtrl->fcn_IfaceInit          = apInit->fcn_IfaceInit;
	apCtrl->fcn_IfaceDeInit        = apInit->fcn_IfaceDeInit;
	apCtrl->fcn_IfaceReadWriteByte = apInit->fcn_IfaceReadWriteByte;
	apCtrl->fcn_IfaceCsSelect      = apInit->fcn_IfaceCsSelect;
	apCtrl->fcn_IfaceCsUnselect    = apInit->fcn_IfaceCsUnselect;
	apCtrl->fcn_IfaceTimerUs       = apInit->fcn_IfaceTimerUs;
}

static void enc28j60_waitForClkRdy(const t_enc28j60Ctrl * apCtrl) {
	while (ESTAT_CLKRDY != (ESTAT_CLKRDY & enc28j60_rcr(apCtrl, ESTAT))) {} ;
}

static void enc28j60_setMacAddr(const t_enc28j60Ctrl * apCtrl, const unsigned char * apMacAddr) {
	mASSERT(NULL != apMacAddr);

	enc28j60_wcr(apCtrl, MAADR5, apMacAddr[0]);
	enc28j60_wcr(apCtrl, MAADR4, apMacAddr[1]);
	enc28j60_wcr(apCtrl, MAADR3, apMacAddr[2]);
	enc28j60_wcr(apCtrl, MAADR2, apMacAddr[3]);
	enc28j60_wcr(apCtrl, MAADR1, apMacAddr[4]);
	enc28j60_wcr(apCtrl, MAADR0, apMacAddr[5]);
}

/*
 * Init & packet Rx/Tx
 */
void enc28j60_init(t_enc28j60Ctrl * apCtrl, const t_enc28j60Init * apInit)
{
	mASSERT(NULL != apCtrl);
	mASSERT(NULL != apInit);

	enc28j60_attachIface(apCtrl, apInit);
	apCtrl->fcn_IfaceInit();

	enc28j60_waitForClkRdy(apCtrl);

	// Reset ENC28J60
	enc28j60_reset(apCtrl);

	// Setup Rx buffer
	enc28j60_wcr16(apCtrl, ERXST,   ENC28J60_RXSTART);
	enc28j60_wcr16(apCtrl, ERXND,   ENC28J60_RXEND);
	enc28j60_wcr16(apCtrl, ERXRDPT, ENC28J60_RXSTART);
	s_enc28j60_rxrdpt = ENC28J60_RXSTART;

	// Setup Tx buffer
	enc28j60_wcr16(apCtrl, ETXST, ENC28J60_TXSTART);
	enc28j60_wcr16(apCtrl, ETXND, ENC28J60_TXSTART);
	enc28j60_wcr16(apCtrl, EWRPT, ENC28J60_TXSTART);

	// Filter
	enc28j60_wcr(apCtrl, ERXFCON, ERXFCON_UCEN  | \
								  ERXFCON_CRCEN | \
								  ERXFCON_MPEN  | \
								  ERXFCON_MCEN  | \
								  ERXFCON_BCEN);

	// Setup MAC
	enc28j60_wcr(apCtrl, MACON2, 0);
	enc28j60_wcr(apCtrl, MACON1, MACON1_TXPAUS | \
								 MACON1_RXPAUS | \
								 MACON1_MARXEN);

	enc28j60_wcr(apCtrl, MACON3, MACON3_PADCFG0 | \
								 MACON3_TXCRCEN | \
								 MACON3_FRMLNEN | \
								 MACON3_FULDPX);

	enc28j60_wcr16(apCtrl, MAMXFL, ENC28J60_MAXFRAME);
	enc28j60_wcr(apCtrl, MABBIPG, 0x15);
	enc28j60_wcr(apCtrl, MAIPGL,  0x12);
	enc28j60_wcr(apCtrl, MAIPGH,  0x0c);

	enc28j60_setMacAddr(apCtrl, apInit->macAddr);

	// Setup PHY
	enc28j60_phyWrite(apCtrl, PHCON1, PHCON1_PDPXMD); // Force full-duplex mode
	enc28j60_phyWrite(apCtrl, PHCON2, PHCON2_HDLDIS); // Disable loopback
	enc28j60_phyWrite(apCtrl, PHLCON, PHLCON_LEDA_LINK_STATUS    | \
									  PHLCON_LEDB_TXRX_ACTIVITY | \
									  PHLCON_LFRQ0 | PHLCON_STRCH);

	// Enable Rx packets
	enc28j60_bfs(apCtrl, ECON1, ECON1_RXEN);
}

void enc28j60_deInit(t_enc28j60Ctrl * apCtrl) {
	mASSERT(NULL != apCtrl);
	mASSERT(NULL != apCtrl->fcn_IfaceDeInit);

	apCtrl->fcn_IfaceDeInit();
}

void enc28j60_packetSend(const t_enc28j60Ctrl * apCtrl, const uint8_t *apData, uint16_t aLen)
{
	while(enc28j60_rcr(apCtrl, ECON1) & ECON1_TXRTS)
	{
		// TXRTS may not clear - ENC28J60 bug. We must reset
		// transmit logic in cause of Tx error
		if(enc28j60_rcr(apCtrl, EIR) & EIR_TXERIF)
		{
			enc28j60_bfs(apCtrl, ECON1, ECON1_TXRST);
			enc28j60_bfc(apCtrl, ECON1, ECON1_TXRST);
		}
	}

	enc28j60_wcr16(apCtrl, EWRPT, ENC28J60_TXSTART);
	enc28j60_wcr16(apCtrl, ETXST, ENC28J60_TXSTART);
	enc28j60_wcr16(apCtrl, ETXND, ENC28J60_TXSTART + aLen);

	// Write per-packet control byte
	enc28j60_opWrite(apCtrl, ENC28J60_SPI_WBM, 0, 0x00);

	// Write packet
	enc28j60_bufferWrite(apCtrl, apData, aLen);

	// Send the contents of the transmit buffer onto the network
	enc28j60_bfs(apCtrl, ECON1, ECON1_TXRTS);
}

uint16_t enc28j60_packetRecv(const t_enc28j60Ctrl * apCtrl, uint8_t *apData, uint16_t aLen)
{
	uint16_t rxlen, status;

	// Check if a packet has been received and buffered
	if (0 == enc28j60_rcr(apCtrl, EPKTCNT))
	{
		return (0);
	}

	// Set the read pointer to the start of the received packet
	enc28j60_wcr16(apCtrl, ERDPT, s_enc28j60_rxrdpt);

	// Read the next packet pointer
	enc28j60_bufferRead(apCtrl, (void *) &s_enc28j60_rxrdpt, sizeof(s_enc28j60_rxrdpt));

	// read the packet length
	enc28j60_bufferRead(apCtrl, (void *) &rxlen, sizeof(rxlen));

	// read the receive status
	enc28j60_bufferRead(apCtrl, (void *) &status, sizeof(status));

    if(status & RSV_RECEIVED_OK) {
    	mASSERT(rxlen >= ENC28J60_MINFRAME);
    	rxlen -= ENC28J60_SIZE_CRC;

    	mASSERT(aLen >= rxlen);
    	rxlen = (aLen < rxlen) ? aLen : rxlen;

    	enc28j60_bufferRead(apCtrl, apData, rxlen);
    } else {
    	rxlen = 0;
    }

    // Move the RX read pointer to the start of the next received packet
    // This frees the memory we just read out
    /*!<
     * errata :: The receive hardware may corrupt the circular
	 * receive buffer (including the Next Packet Pointer
	 * and receive status vector fields) when an even value
	 * is programmed into the ERXRDPTH:ERXRDPTL registers.
     */
    if (s_enc28j60_rxrdpt == ENC28J60_RXSTART) {
    	enc28j60_wcr16(apCtrl, ERXRDPT, s_enc28j60_rxrdpt);
    } else {
    	enc28j60_wcr16(apCtrl, ERXRDPT, s_enc28j60_rxrdpt - 1);
    }

    // Decrement the packet counter indicate we are done with this packet
    enc28j60_bfs(apCtrl, ECON2, ECON2_PKTDEC);

	return (rxlen);
}
