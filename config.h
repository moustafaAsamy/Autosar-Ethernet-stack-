/*
 * config.h
 *
 *  Created on: Oct 20, 2019
 *      Author: lenovo
 */


#ifndef CONFIG_H_
#define CONFIG_H_

#include "lwip_int.h"

#define ECU1 0
#define ECU2 1

#define  systic_rate  160000      // 10 ms
extern struct netif netIf_List[5];
extern const uint8_t mac[6] ;

#endif /* CONFIG_H_ */
