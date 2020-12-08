/*
 * SoAd.c
 *
 *  Created on: Nov 17, 2019
 *      Author: lenovo
 */
#include "SoAd_Cbk.h"



void SoAd_RxIndication( TcpIp_SocketIdType SocketId, const TcpIp_SockAddrType* RemoteAddrPtr, const uint8* BufPtr, uint16 Length )
{
// you should copy the BufPtr in this function , as it will be freed after the call of this funcion
}
BufReq_ReturnType SoAd_CopyTxData( TcpIp_SocketIdType SocketId, uint8* BufPtr, uint16 BufLength )
{

}
void SoAd_TxConfirmation( TcpIp_SocketIdType SocketId, uint16 Length )
{

}
Std_ReturnType SoAd_TcpAccepted( TcpIp_SocketIdType SocketId, TcpIp_SocketIdType SocketIdConnected, const TcpIp_SockAddrType* RemoteAddrPtr )
{

}
void SoAd_TcpConnected( TcpIp_SocketIdType SocketId )
{

}
void SoAd_TcpIpEvent( TcpIp_SocketIdType SocketId, TcpIp_EventType Event )
{
  // take care if you recived close then reset
    // or you recived a reset
}
