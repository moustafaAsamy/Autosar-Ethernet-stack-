/*
 * enc28j60.h
 *
 */

#ifndef ENC28J60_H_
#define ENC28J60_H_

#include <stdint.h>
#include <lwip/etharp.h>
#include "spi.h"

/* Pins */
//#define ENC_CS_PORT		    GPIO_PORTB_BASE
//#define ENC_INT_PORT		GPIO_PORTE_BASE
////#define ENC_RESET_PORT		GPIO_PORTA_BASE
//#define ENC_CS			GPIO_PIN_3
//#define ENC_INT			GPIO_PIN_4
////#define ENC_RESET		GPIO_PIN_2


#define stauts_PKTIE      0x1
#define stauts_TXIE       0x2
#define stauts_TXERIE     0x4
#define stauts_RXERIE     0x8

extern volatile int flagg;

void    enc_init(const uint8_t *mac);
void     recive_packet( uint8_t * recvied_packet);
uint8_t enc_read_reg(uint8_t reg, uint8_t bank);
void    enc_send_packet(const uint8_t *buf, uint16_t count);
//extern err_t enc_low_level_output(struct netif *netif, struct pbuf *p ,const uint8_t* PhysAddrPtr);



#endif /* ENC28J60_H_ */
