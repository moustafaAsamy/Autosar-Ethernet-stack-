/*
 * TcpIp_Cbk.h
 *
 *  Created on: Nov 17, 2019
 *      Author: lenovo
 */

#ifndef TCPIP_CBK_H_
#define TCPIP_CBK_H_

#include "autosar_includes/TcpIp.h"
#include "autosar_includes/ComStack_Types.h"
#include "Eth.h"

void TcpIp_RxIndication( uint8 CtrlIdx, Eth_FrameType FrameType, boolean IsBroadcast, const uint8* PhysAddrPtr, const uint8* DataPtr, uint16 LenByte ) ;



#endif /* TCPIP_CBK_H_ */
