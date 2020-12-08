/*
 * config.c
 *
 *  Created on: Oct 22, 2019
 *      Author: lenovo
 */

#include "config.h"


struct netif netIf_List[5] ={0};


#if ECU1
 const uint8_t mac[6] ={0x32,0x45,0x67,0x89,0xAB,0xCD};
#endif
#if ECU2
 const uint8_t mac[6] = {0x34,0x45,0x67,0x89,0xeB,0xeD};
#endif




