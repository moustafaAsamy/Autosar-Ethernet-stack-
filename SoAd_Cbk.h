/*
 * SoAd_Cbk.h
 *
 *  Created on: Nov 17, 2019
 *      Author: lenovo
 */

#ifndef SOAD_CBK_H_
#define SOAD_CBK_H_

#include "autosar_includes/TcpIp.h"
#include "autosar_includes/ComStack_Types.h"
#include "Eth.h"

void SoAd_RxIndication( TcpIp_SocketIdType SocketId, const TcpIp_SockAddrType* RemoteAddrPtr, const uint8* BufPtr, uint16 Length );
BufReq_ReturnType SoAd_CopyTxData( TcpIp_SocketIdType SocketId, uint8* BufPtr, uint16 BufLength );
void SoAd_TxConfirmation( TcpIp_SocketIdType SocketId, uint16 Length );
Std_ReturnType SoAd_TcpAccepted( TcpIp_SocketIdType SocketId, TcpIp_SocketIdType SocketIdConnected, const TcpIp_SockAddrType* RemoteAddrPtr );
void SoAd_TcpConnected( TcpIp_SocketIdType SocketId );
void SoAd_TcpIpEvent( TcpIp_SocketIdType SocketId, TcpIp_EventType Event );



#endif /* SOAD_CBK_H_ */
