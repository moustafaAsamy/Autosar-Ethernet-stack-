/*
 * EthIf.h
 *
 *  Created on: Nov 21, 2019
 *      Author: lenovo
 */

#ifndef AUTOSAR_INCLUDES_ETHIF_H_
#define AUTOSAR_INCLUDES_ETHIF_H_

#include "autosar_includes/TcpIp.h"
#include "autosar_includes/ComStack_Types.h"
#include "Eth.h"


 struct EthIfController
{
   uint8 EthIfCtrlIdx ;
   uint16 EthIfCtrlMtu ;  //64 to 9000 Specifies the maximum transmission unit (MTU) of the EthIfCtrl in [bytes].
   uint32 EthIfMaxTxBufsTotal ;
} ;

Std_ReturnType EthIf_Transmit( uint8 CtrlIdx, Eth_BufIdxType BufIdx, Eth_FrameType FrameType, boolean TxConfirmation, uint16 LenByte, const uint8* PhysAddrPtr );
BufReq_ReturnType EthIf_ProvideTxBuffer( uint8 CtrlIdx, Eth_FrameType FrameType, uint8 Priority, Eth_BufIdxType* BufIdxPtr, uint8** BufPtr, uint16* LenBytePtr );
void EthIf_GetPhysAddr( uint8 CtrlIdx, uint8* PhysAddrPtr );
#endif /* AUTOSAR_INCLUDES_ETHIF_H_ */
