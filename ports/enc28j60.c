#include "enc28j60.h"
#include "enc28j60reg.h"
#include <stdint.h>
#include <stdlib.h>
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include <driverlib/interrupt.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include <inc/hw_ints.h>
#include "config.h"
#include <stdbool.h>
#include "lwipopts.h"
//#include "autosar_includes/TcpIp.h"
//#include "autosar_includes/ComStack_Types.h"
//#include "Eth.h"

#define TX_START	(0x1FFF - 0x600)
#define RX_END		(TX_START-1)
extern struct netif g_sNetIF;
static uint8_t enc_current_bank;
static uint16_t enc_next_packet;
uint8_t global_flag=0;
uint8_t incomming_frame[1514]={0};
int count =0;
/* Internal low-level register access functions*/
static uint8_t enc_rcr(uint8_t reg);
static void enc_wcr(uint8_t reg, uint8_t val);
static uint8_t enc_rcr_m(uint8_t reg);
static void enc_rbm(uint8_t *buf, uint16_t count);
static void enc_wbm(const uint8_t *buf, uint16_t count);
static void enc_bfs(uint8_t reg, uint8_t mask);
static void enc_bfc(uint8_t reg, uint8_t mask);
static void enc_switch_bank(uint8_t new_bank);
extern void reciving_autosar(int data_count );
/* Internal high-level register access functions*/
//static uint8_t enc_read_reg(uint8_t reg, uint8_t bank);
static void enc_write_reg(uint8_t reg, uint8_t bank, uint8_t value);
static uint8_t enc_read_mreg(uint8_t reg, uint8_t bank);
static void enc_set_bits(uint8_t reg, uint8_t bank, uint8_t mask);
static void enc_clear_bits(uint8_t reg, uint8_t bank, uint8_t mask);

/* Macros for accessing registers.
 * These macros should be used instead of calling the functions directly.
 * They simply pass the register's bank as an argument, so the caller
 * doesn't have to deal with that.
 */
#define READ_REG(reg)             enc_read_reg(reg, reg ## _BANK)
#define WRITE_REG(reg, value)     enc_write_reg(reg, reg ## _BANK, value)
#define READ_MREG(reg)            enc_read_mreg(reg, reg ## _BANK)
#define SET_REG_BITS(reg, mask)   enc_set_bits(reg, reg ## _BANK, mask)
#define CLEAR_REG_BITS(reg, mask) enc_clear_bits(reg, reg ## _BANK, mask)


static uint16_t enc_phy_read(uint8_t addr);
static void enc_set_rx_area(uint16_t start, uint16_t end);
static void enc_set_mac_addr(const uint8_t *mac_addr);
extern void trasmit_process(void);
void reciving_error(void);
void reciving(int data_count );
int recive_packet_error( uint8_t * recvied_packet);
volatile  int flagg=0;


void enc_reset(void) {
	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
	spi_send(0xFF);
	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}
/**
 * Read Control Register (RCR)
 */
uint8_t enc_rcr(uint8_t reg) {
	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
	spi_send(reg);
	uint8_t b = spi_send(0xFF); // Dummy

	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
	return b;
}
/**
 * Write Control Register (WCR)
 */
void enc_wcr(uint8_t reg, uint8_t val) {
	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
	spi_send(0x40 | reg);
	spi_send(val);
	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}
/**
 * Read Control Register for MAC an MII registers.
 * Reading MAC and MII registers produces an initial dummy
 * byte. Presumably because it takes longer to fetch the values
 * of those registers.
 */
uint8_t enc_rcr_m(uint8_t reg) {
	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
	spi_send(reg);
	spi_send(0xFF);
	uint8_t b = spi_send(0xFF); // Dummy
	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
	return b;
}
/**
 * Read Buffer Memory.
 *
 */
void enc_rbm(uint8_t *buf, uint16_t count) {
	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0); //
	spi_send(0x20 | 0x1A);
	int i;
	for (i = 0; i < count; i++) {
		*buf = spi_send(0xFF);
		buf++;
	}
	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}
/**
 * Write Buffer Memory.
 */
void enc_wbm(const uint8_t *buf, uint16_t count) {
	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
	spi_send(0x60 | 0x1A);
	int i;
	for (i = 0; i < count; i++) {
		spi_send(*buf);
		buf++;
	}
	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}
/**
 * Bit Field Set.
 * Set the bits of argument 'mask' in the register 'reg'.
 * Not valid for MAC and MII registers.
 */
void enc_bfs(uint8_t reg, uint8_t mask) {
	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
	spi_send(0x80 | reg);
	spi_send(mask);
	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}
/**
 * Bit Field Clear.
 * Clear the bits of argument 'mask' in the register 'reg'.
 * Not valid for MAC and MII registers.
 */
void enc_bfc(uint8_t reg, uint8_t mask) {
	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
	spi_send(0xA0 | reg);
	spi_send(mask);
	MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}
 /**
 * Switch memory bank to 'new_bank'
 */
void enc_switch_bank(uint8_t new_bank) {
	if (new_bank == enc_current_bank || new_bank == ANY_BANK) {
		return;
	}
	uint8_t econ1 = enc_rcr(ENC_ECON1);

	econ1 &= ~ENC_ECON1_BSEL_MASK;
	econ1 |= (new_bank & ENC_ECON1_BSEL_MASK) << ENC_ECON1_BSEL_SHIFT;
	enc_wcr(ENC_ECON1, econ1);
	enc_current_bank = new_bank;
}
/**
 * High level register read. Switches bank as appropriate.
 */
uint8_t enc_read_reg(uint8_t reg, uint8_t bank) {
	if (bank != enc_current_bank) {
		enc_switch_bank(bank);
	}

	return enc_rcr(reg);
}

/**
 * High level bit field set. Switches bank as appropriate.
 */
void enc_set_bits(uint8_t reg, uint8_t bank, uint8_t mask) {
	if (bank != enc_current_bank) {
		enc_switch_bank(bank);
	}

	enc_bfs(reg, mask);
}

/**
 * High level bit field clear. Switches bank as appropriate.
 */
void enc_clear_bits(uint8_t reg, uint8_t bank, uint8_t mask) {
	if (bank != enc_current_bank) {
		enc_switch_bank(bank);
	}

	enc_bfc(reg, mask);
}

/**
 * High level MAC/MII register read. Switches bank as appropriate.
 */
uint8_t enc_read_mreg(uint8_t reg, uint8_t bank) {
	if (bank != enc_current_bank) {
		enc_switch_bank(bank);
	}

	return enc_rcr_m(reg);
}

/**
 * High level register write. Switches bank as appropriate.
 */
void enc_write_reg(uint8_t reg, uint8_t bank, uint8_t value) {
	if (bank != enc_current_bank) {
		enc_switch_bank(bank);
	}

	enc_wcr(reg, value);
}

/**
 * Read value from PHY address.
 * Reading procedure is described in ENC28J60 datasheet
 * section 3.3.
 */
uint16_t enc_phy_read(uint8_t addr) {
	/*
	 1. Write the address of the PHY register to read
	 from into the MIREGADR register.*/
	WRITE_REG(ENC_MIREGADR, addr);

	/*2. Set the MICMD.MIIRD bit. The read operation
	 begins and the MISTAT.BUSY bit is set.*/
	WRITE_REG(ENC_MICMD, 0x1);

	/*3. Wait 10.24 μs. Poll the MISTAT.BUSY bit to be
	 certain that the operation is complete. While
	 busy, the host controller should not start any
	 MIISCAN operations or write to the MIWRH
	 register.
	 When the MAC has obtained the register
	 contents, the BUSY bit will clear itself.*/

	/* Assuming that we are running at 1MHz, a single cycle is
	 * 1 us */
	MAP_SysCtlDelay(((MAP_SysCtlClockGet()/3)/1000));

	uint8_t stat;
	do {
		stat = READ_MREG(ENC_MISTAT);
	} while (stat & ENC_MISTAT_BUSY);

	/*4. Clear the MICMD.MIIRD bit.*/
	WRITE_REG(ENC_MICMD, 0x00);

	/*5. Read the desired data from the MIRDL and
	 MIRDH registers. The order that these bytes are
	 accessed is unimportant.
	 */
	uint16_t ret;
	ret = READ_MREG(ENC_MIRDL) & 0xFF;
	ret |= READ_MREG(ENC_MIRDH) << 8;

	return ret;
}
/**
 * Write value to PHY address.
 * Reading procedure is described in ENC28J60 datasheet
 * section 3.3.
 */
void enc_phy_write(uint8_t addr, uint16_t value) {
	WRITE_REG(ENC_MIREGADR, addr);
	WRITE_REG(ENC_MIWRL, value & 0xFF);
	WRITE_REG(ENC_MIWRH, value >> 8);

	MAP_SysCtlDelay(((MAP_SysCtlClockGet()/3)/1000));

	uint8_t stat;
	do {
		stat = READ_MREG(ENC_MISTAT);
	} while (stat & ENC_MISTAT_BUSY);
}

/**
 * Set the memory area to use for receiving packets.
 */
void enc_set_rx_area(uint16_t start, uint16_t end) {
	WRITE_REG(ENC_ERXSTL, start & 0xFF);
	WRITE_REG(ENC_ERXSTH, (start >> 8) & 0xFFF);

	WRITE_REG(ENC_ERXNDL, end & 0xFF);
	WRITE_REG(ENC_ERXNDH, (end >> 8) & 0xFFF);

	WRITE_REG(ENC_ERXRDPTL, start & 0xFF);
	WRITE_REG(ENC_ERXRDPTH, (start >> 8) & 0xFFF);
}
/**
 * Set the MAC address.
 */
void enc_set_mac_addr(const uint8_t *mac_addr) {
	WRITE_REG(ENC_MAADR1, mac_addr[0]);
	WRITE_REG(ENC_MAADR2, mac_addr[1]);
	WRITE_REG(ENC_MAADR3, mac_addr[2]);
	WRITE_REG(ENC_MAADR4, mac_addr[3]);
	WRITE_REG(ENC_MAADR5, mac_addr[4]);
	WRITE_REG(ENC_MAADR6, mac_addr[5]);
}
/**
 * Read the MAC address.
 */
void enc_get_mac_addr(uint8_t *mac_addr) {
  mac_addr[0] = READ_REG(ENC_MAADR1);
  mac_addr[1] = READ_REG(ENC_MAADR2);
  mac_addr[2] = READ_REG(ENC_MAADR3);
  mac_addr[3] = READ_REG(ENC_MAADR4);
  mac_addr[4] = READ_REG(ENC_MAADR5);
  mac_addr[5] = READ_REG(ENC_MAADR6);
}

void intrupt_enable(uint8_t source)
{
    CLEAR_REG_BITS(ENC_EIR,ENC_EIR_RXERIF|ENC_EIR_TXERIF|ENC_EIR_TXIF|ENC_EIR_PKTIF);
    SET_REG_BITS(ENC_EIE,source);
}
void enc_init(const uint8_t *mac) {
    /* Initialize next_packet. */
    enc_next_packet = 0x000;
    /* reset */
    enc_reset();
    uint8_t reg =0;
    /* wait to be ready before mac and phy config. */
    do {
        reg = READ_REG(ENC_ESTAT);
    } while ((reg & ENC_ESTAT_CLKRDY) == 0);
    enc_switch_bank(0);
    SET_REG_BITS(ENC_ECON1, ENC_ECON1_TXRST);
    CLEAR_REG_BITS(ENC_ECON1, ENC_ECON1_RXEN);
    SET_REG_BITS(ENC_ECON2, ENC_ECON2_AUTOINC);
    /* Initialize RX BUFFER SPACE  */
    enc_set_rx_area(0x000, RX_END);
    /* Initialize phy */
    uint16_t phyreg = enc_phy_read(ENC_PHSTAT2);
    phyreg &= ~ENC_PHSTAT2_DPXSTAT;
    enc_phy_write(ENC_PHSTAT2, phyreg);
    phyreg = enc_phy_read(ENC_PHCON1);
    phyreg &= ~ENC_PHCON_PDPXMD;
    enc_phy_write(ENC_PHCON1, phyreg);
    /* Setup receive filter to receive * broadcast, multicast and unicast to the given MAC */
    WRITE_REG(ENC_ERXFCON, ENC_ERXFCON_UCEN | ENC_ERXFCON_CRCEN | ENC_ERXFCON_BCEN | ENC_ERXFCON_MCEN);
    /* Initialize MAC */
    enc_set_mac_addr(mac);
    WRITE_REG(ENC_MACON1, ENC_MACON1_TXPAUS | ENC_MACON1_RXPAUS | ENC_MACON1_MARXEN);
    WRITE_REG(ENC_MACON3,(0x1 << ENC_MACON3_PADCFG_SHIFT) | ENC_MACON3_TXRCEN |/*ENC_MACON3_FULDPX |*/ENC_MACON3_FRMLNEN);
    WRITE_REG(ENC_MAMXFLL, 1518 & 0xFF);
    WRITE_REG(ENC_MAMXFLH, (1518 >> 8) & 0xFF);
    WRITE_REG(ENC_MABBIPG, 0x12); // half duplex
    //WRITE_REG(ENC_MABBIPG, 0x15); // full duplex
    WRITE_REG(ENC_MAIPGL, 0x12);
    WRITE_REG(ENC_MAIPGH, 0x0C);
    /* enable the INTERRUPT global */
    intrupt_enable(ENC_EIE_RXERIE|ENC_EIE_TXERIE|ENC_EIE_TXIE|ENC_EIE_PKTIE|ENC_EIE_INTIE);
    CLEAR_REG_BITS(ENC_ECON1, ENC_ECON1_TXRST | ENC_ECON1_RXRST);
    //    SET_REG_BITS(ENC_EIE, ENC_EIE_INTIE | ENC_EIE_PKTIE |ENC_EIE_TXIE);
//    intrupt_enable(ENC_EIE_TXERIE |ENC_EIE_TXIE |ENC_EIE_RXERIE);
    /* dummy check to see the saved local mac address */
    uint8_t mc[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    enc_get_mac_addr(mc);
    /* enable the reception */
    CLEAR_REG_BITS(ENC_ECON1, ENC_ECON1_TXRST | ENC_ECON1_RXRST);
    SET_REG_BITS(ENC_ECON1, ENC_ECON1_RXEN);
}
void recive_packet( uint8_t * recvied_packet)
{
        uint8_t header[6];
        uint8_t *status = header + 2;
        WRITE_REG(ENC_ERDPTL, enc_next_packet & 0xFF);          // start reading from the next packet position , saved from the intlization or the last reception
        WRITE_REG(ENC_ERDPTH, (enc_next_packet >> 8) & 0xFF);
        enc_rbm(header, 6);                                     // read the header
        enc_next_packet = header[0] | (header[1] << 8);         // automatically updated by the hardware after any reception
        uint16_t data_count = status[0] | (status[1] << 8);
        if(data_count>1536){return;}
        if (status[2] & (1 << 7))                               // reception is done
        {  enc_rbm(recvied_packet, data_count);}                // read the frame , put it in passed pointer
        uint16_t erxst = READ_REG(ENC_ERXSTL) | (READ_REG(ENC_ERXSTH) << 8);
        if (enc_next_packet == erxst)                           // free space of read packet
         {  WRITE_REG(ENC_ERXRDPTL, READ_REG(ENC_ERXNDL));
            WRITE_REG(ENC_ERXRDPTH, READ_REG(ENC_ERXNDH));
        } else {
            WRITE_REG(ENC_ERXRDPTL, (enc_next_packet-1) & 0xFF);
            WRITE_REG(ENC_ERXRDPTH, ((enc_next_packet-1) >> 8) & 0xFF);
        }
        uint8_t x = READ_REG(ENC_EPKTCNT) ;
        SET_REG_BITS(ENC_ECON2, ENC_ECON2_PKTDEC);              // decerment the no. of reciceved but unproceced packets , if = 0 pktie will be cleared
        reciving_autosar(data_count);
}
int recive_packet_error( uint8_t * recvied_packet)
{
        uint8_t header[6];
        uint8_t *status = header + 2;
        WRITE_REG(ENC_ERDPTL, enc_next_packet & 0xFF);          // start reading from the next packet position , saved from the intlization or the last reception
        WRITE_REG(ENC_ERDPTH, (enc_next_packet >> 8) & 0xFF);
        enc_rbm(header, 6);                                     // read the header
        enc_next_packet = header[0] | (header[1] << 8);         // automatically updated by the hardware after any reception
        uint16_t data_count = status[0] | (status[1] << 8);
        if(data_count>1536){return 0;}
        if (status[2] & (1 << 7))                               // reception is done
        {  enc_rbm(recvied_packet, data_count);}                // read the frame , put it in passed pointer
        uint16_t erxst = READ_REG(ENC_ERXSTL) | (READ_REG(ENC_ERXSTH) << 8);
        if (enc_next_packet == erxst)                           // free space of read packet
         {  WRITE_REG(ENC_ERXRDPTL, READ_REG(ENC_ERXNDL));
            WRITE_REG(ENC_ERXRDPTH, READ_REG(ENC_ERXNDH));
        } else {
            WRITE_REG(ENC_ERXRDPTL, (enc_next_packet-1) & 0xFF);
            WRITE_REG(ENC_ERXRDPTH, ((enc_next_packet-1) >> 8) & 0xFF);
        }
        uint8_t x = READ_REG(ENC_EPKTCNT) ;
        SET_REG_BITS(ENC_ECON2, ENC_ECON2_PKTDEC);              // decerment the no. of reciceved but unproceced packets , if = 0 pktie will be cleared
        CLEAR_REG_BITS(ENC_EIR, ENC_EIR_RXERIF);               // the error flage was not cleared , so it still make the status = 1 , so the interrupt is hold low  and not changed
        int counter =READ_REG(ENC_EPKTCNT);
        reciving_autosar(data_count);
        return counter ;
}
extern void ethernet_handler(void)
{
     uint8_t c = 0;c= c;
     uint8_t y =0;y=y;
     uint8_t pin_mask = GPIOIntStatus(GPIO_PORTD_BASE, true) & 0xFF;    // read the status of the INT pin
     GPIOIntClear(GPIO_PORTD_BASE, pin_mask);
   while(  (READ_REG(ENC_EIR) !=0) &&  (READ_REG(ENC_ESTAT) & ENC_ESTAT_INT==ENC_ESTAT_INT)  )
      {
       uint8_t pin_mask = GPIOIntStatus(GPIO_PORTD_BASE, true) & 0xFF;    // read the status of the INT pin
       GPIOIntClear(GPIO_PORTD_BASE, pin_mask);
       uint8_t stauts =0;
       stauts = READ_REG(ENC_EIR);                                         // read the flags
       global_flag = 0;
         if (stauts & ENC_EIR_PKTIF)                                          // cleared after processing
         {
         global_flag |= stauts_PKTIE;
         flagg=1;
         recive_packet(incomming_frame);
         }
         if (stauts & ENC_EIR_TXERIF)                                    // cleared after processing , BFC command to clear the EIR.TXERIF bit
         {
             global_flag |= stauts_TXERIE;
             trasmit_process();
         }
         if (stauts & ENC_EIR_TXIF)
         {
            global_flag |= stauts_TXIE;
            trasmit_process();
         }
         if (stauts & ENC_EIR_RXERIF)                                     // cleared after processing , BFC command to clear the EIR.RXERIF bit
         {
            global_flag |= stauts_RXERIE;
            while(recive_packet_error(incomming_frame))
            { }
            CLEAR_REG_BITS(ENC_ESTAT, ENC_ESTAT_BUFFER);  /* buffer error after processing all unprocessed which made new packets to be discard */

         }
}
}
extern void trasmit_process(void)
{
    if (global_flag & stauts_TXERIE)
    {
    CLEAR_REG_BITS(ENC_EIR, ENC_EIR_TXERIF);
    CLEAR_REG_BITS(ENC_ESTAT, ENC_ESTAT_TXABRT);
    }
    if (global_flag & stauts_TXIE)
    {
     CLEAR_REG_BITS(ENC_EIR, ENC_EIR_TXIF);
    }
}

extern void enc_send_packet(const uint8_t *buf, uint16_t count) {
  WRITE_REG(ENC_ETXSTL, TX_START & 0xFF);
  WRITE_REG(ENC_ETXSTH, TX_START >> 8);

  WRITE_REG(ENC_EWRPTL, TX_START & 0xFF);
  WRITE_REG(ENC_EWRPTH, TX_START >> 8);


  uint8_t control = 0x00;
  enc_wbm(&control, 1);

  enc_wbm(buf, count);

  uint16_t tx_end = TX_START + count;
  WRITE_REG(ENC_ETXNDL, tx_end & 0xFF);
  WRITE_REG(ENC_ETXNDH, tx_end >> 8);

  ///* Eratta 12 */
  SET_REG_BITS(ENC_ECON1, ENC_ECON1_TXRST);
  CLEAR_REG_BITS(ENC_ECON1, ENC_ECON1_TXRST);

  CLEAR_REG_BITS(ENC_EIR, ENC_EIR_TXIF);
  SET_REG_BITS(ENC_ECON1, ENC_ECON1_TXRTS);


  /* Busy wait for the transmission to complete */
  while (true) {
    uint8_t r = READ_REG(ENC_ECON1);
    if ((r & ENC_ECON1_TXRTS) == 0)
      break;
  }

  /* Read status bits */
  uint8_t status[7];
  tx_end++;
  WRITE_REG(ENC_ERDPTL, tx_end & 0xFF);
  WRITE_REG(ENC_ERDPTH, tx_end >> 8);
  enc_rbm(status, 7);
  uint16_t transmit_count = status[0] | (status[1] << 8);
  if (status[2] & 0x80)
  {      transmit_count=transmit_count;}
}

//void TcpIp_RxIndication( uint8_t CtrlIdx, uint8_t FrameType, uint8_t IsBroadcast, const uint8_t* PhysAddrPtr, const uint8_t* DataPtr, uint16_t LenByte )
//{
//    struct pbuf *pBuf  = pbuf_alloc(PBUF_RAW, LenByte,PBUF_POOL);
//    struct pbuf *intial_buffer  = pBuf;
//    int copy_lenght,pos =0;
//    if(pBuf != NULL)
//    {
//        while(pBuf &&  (LenByte>0))
//        {
//            //copy_lenght = PBUF_POOL_BUFSIZE < LenByte ? PBUF_POOL_BUFSIZE : LenByte ;
//            memcpy(pBuf->payload, DataPtr+pos, copy_lenght = PBUF_POOL_BUFSIZE < LenByte ? PBUF_POOL_BUFSIZE : LenByte );
//            LenByte = LenByte - PBUF_POOL_BUFSIZE <0 ? 0 : LenByte - PBUF_POOL_BUFSIZE ;
//            pBuf=pBuf->next;
//            pos = copy_lenght+pos;
//        }
//    if(ethernet_input(intial_buffer, &g_sNetIF) != ERR_OK)
//     { pbuf_free(intial_buffer);}
//    }
//}

void reciving_autosar(int data_count )
{
  TcpIp_RxIndication( 0, 0, 0, 0, incomming_frame, data_count-4);
}


//err_t EthIf_ProvideTxBuffer(struct netif *netif, struct pbuf *BufPtr)
//{
//
//        uint8_t frame[1514]={0};
//        uint8_t * frame_1=(uint8_t *)BufPtr->payload;
//        uint8_t *frame_ptr = &frame[0];
//        //int x = *LenBytePtr;
//        memcpy(frame_ptr, frame_1 , BufPtr->tot_len );
//        int c = 0;
//        if ( BufPtr->tot_len  <64)
//        {c=64;}
//        else
//        { c=BufPtr->tot_len ;}
//        enc_send_packet(frame,c);
//
//}
