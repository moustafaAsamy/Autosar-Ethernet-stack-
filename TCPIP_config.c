/*
 * TCPIP_config.c
 *
 *  Created on: Dec 25, 2019
 *      Author: lenovo
 */




#include "autosar_includes/TcpIp.h"
#include "autosar_includes/ComStack_Types.h"
#include "autosar_includes/EthIf_config.h"
#include "Eth.h"
#include "SoAd_Cbk.h"
#include "config.h"


struct TcpIp_SocketType sockets_list[TcpIpTcpSocketMax_no] ={0};















TcpIpGeneral TcpIpGeneral_t =
{
 .TcpIpTcpSocketMax = 5,
 .TcpIpUdpSocketMax = 5,
 .TcpIpDevErrorDetect= 1,
 .TcpIpDhcpServerEnabled =0,
 .TcpIpTcpEnabled=1,
 .TcpIpUdpEnabled=1,
 .TcpIpScalabilityClass_t= SC1,
 .TcpIpBufferMemory =5000,
 .TcpIpMainFunctionPeriod = 10,//MS
};


TcpIpTcpConfig TcpIpTcpConfig_t =
{
 .TcpIpTcpSynMaxRtx =6,
 .TcpIpTcpFastRecoveryEnabled =0,
 .TcpIpTcpFastRetransmitEnabled=0,
 .TcpIpTcpNagleEnabled =0,
 .TcpIpTcpTtl =255,
 .TcpIpTcpRetransmissionTimeout =250,// ms
 .TcpIpTcpMaxRtx=6,
 .TcpIpTcpReceiveWindowMax =5000,
};

#if ECU1
TcpIpCtrl TcpIpCtrlRef_list[2]=
{
 {
  .TcpIpEthIfCtrlRef = & EthIfController_list[0],
 },
 {
  .TcpIpEthIfCtrlRef = &EthIfController_list[0],
 }
};

TcpIpLocalAddr TcpIpLocalAddr_list [2]=
{
 {
  .TcpIpAddrId=0,
  .TcpIpDomainType_t= TCPIP_AF_INET,
  .TcpIpCtrlRef_t =&TcpIpCtrlRef_list[0],
  .TcpIpAddressType_t=TCPIP_UNICAST,
  .TcpIpAddrAssignment_t=
     {
      .TcpIpAssignmentMethod_t = TCPIP_STATIC,
     },
     .TcpIpStaticIpAddressConfig_t =
     {
      . TcpIpDefaultRouter =0xfbb02526,
      . TcpIpNetmask =0xfbb00000,
      . TcpIpStaticIpAddress =0xfbb03020 ,
     },
 },
 {
   .TcpIpAddrId=1,
   .TcpIpDomainType_t= TCPIP_AF_INET,
   .TcpIpCtrlRef_t = &TcpIpCtrlRef_list[1],
   .TcpIpAddressType_t=TCPIP_UNICAST,
   .TcpIpAddrAssignment_t=
   {
    .TcpIpAssignmentMethod_t = TCPIP_STATIC,
   },
   .TcpIpStaticIpAddressConfig_t =
   {
      . TcpIpDefaultRouter =0x22242526,
      . TcpIpNetmask =0xfbb00000,
      . TcpIpStaticIpAddress =0xfbb03014 ,
   },
 }
};
#endif
#if ECU2
TcpIpCtrl TcpIpCtrlRef_list[2]=
{
 {
  .TcpIpEthIfCtrlRef = & EthIfController_list[0],
 },
 {
  .TcpIpEthIfCtrlRef = &EthIfController_list[0],
 }
};
TcpIpLocalAddr TcpIpLocalAddr_list [2]=
{
 {
  .TcpIpAddrId=0,
  .TcpIpDomainType_t= TCPIP_AF_INET,
  .TcpIpCtrlRef_t =&TcpIpCtrlRef_list[0],
  .TcpIpAddressType_t=TCPIP_UNICAST,
  .TcpIpAddrAssignment_t=
     {
      .TcpIpAssignmentMethod_t = TCPIP_STATIC,
     },
     .TcpIpStaticIpAddressConfig_t =
     {
      . TcpIpDefaultRouter =0xfbb02526,
      . TcpIpNetmask =0xfbb00000,
      . TcpIpStaticIpAddress =0xfbb03014 ,
     },
 },
 {
   .TcpIpAddrId=1,
   .TcpIpDomainType_t= TCPIP_AF_INET,
   .TcpIpCtrlRef_t = &TcpIpCtrlRef_list[1],
   .TcpIpAddressType_t=TCPIP_UNICAST,
   .TcpIpAddrAssignment_t=
   {
    .TcpIpAssignmentMethod_t = TCPIP_STATIC,
   },
   .TcpIpStaticIpAddressConfig_t =
   {
      . TcpIpDefaultRouter =0x22242526,
      . TcpIpNetmask =0xfbb00000,
      . TcpIpStaticIpAddress =0xfbb03020 ,
   },
 }
};
#endif




