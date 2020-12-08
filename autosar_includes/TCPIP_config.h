/*
 * TCPIP_config.h
 *
 *  Created on: Dec 25, 2019
 *      Author: lenovo
 */

#ifndef TCPIP_CONFIG_H_
#define TCPIP_CONFIG_H_

#include "autosar_includes/TcpIp.h"
#include "autosar_includes/ComStack_Types.h"
#include "autosar_includes/EthIf_config.h"
#include "Eth.h"
#include "SoAd_Cbk.h"

#define max_no_buffers  10
extern TcpIpLocalAddr TcpIpLocalAddr_list [2];
extern struct TcpIp_SocketType sockets_list[TcpIpTcpSocketMax_no] ;
#endif /* TCPIP_CONFIG_H_ */
