/*
 * lwip_int.h
 *
 *  Created on: Sep 29, 2019
 *      Author: lenovo
 */

#ifndef LWIP_LWIP_INT_H_
#define LWIP_LWIP_INT_H_

#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include <lwip/etharp.h>
#include "lwip/tcp_impl.h"
#include "lwip/udp.h"
#include "string.h"


extern struct netif netIf_List[5] ;
void ECU_int(struct netif *netif, u8_t controller_id ,  ip_addr_t *ipaddr, ip_addr_t *netmask,ip_addr_t *gw);
void UDP_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p,ip_addr_t *addr, u16_t port);
extern udp_recv_fn recv ;
void lwIPPrivateInit(struct netif *netif, uint8_t controller_id ,  ip_addr_t *ip_addr, ip_addr_t *net_mask,ip_addr_t *gw_addr);
void LWIP_stack_int(struct netif *netif, uint8_t controller_id ,  ip_addr_t *ipaddr, ip_addr_t *netmask,ip_addr_t *gw);
void print(const char *pcString);
void application_timer(void);
void SimpleDelay(void);
void errror_event(void *arg, err_t err);
uint8_t Eth_TCP_Connect(uint16_t D_Port ,uint16_t S_Port, ip_addr_t * dest_address, ip_addr_t *local_addr );
void timer(void);
void ECU_START_RX(void);
void ECU_START_TX(void);
err_t conection_established(void *pvArg, struct tcp_pcb *psPcb, err_t iErr);
err_t TCPReceive(void *pvArg, struct tcp_pcb *psPcb, struct pbuf *psBuf, err_t iErr);
void SEND_data(void);
void update_data_TX_buffer(void);
void app_task_tx(void);
void error_event(struct tcp_pcb *psPcb);
void app_task_tx_tcp(void);
void app_task_tx_udp(void);


#endif /* LWIP_LWIP_INT_H_ */
