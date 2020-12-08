

#include "lwip_int.h"
#include "autosar_includes/TCPIP_config.h"
#include "autosar_includes/TcpIp.h"
#include "config.h"
#include "utils/lwiplib.h"

uint32_t g_ui32IPMode = IPADDR_USE_STATIC;


#if ECU1
TcpIp_SocketIdType  SocketIdPtr ;
uint16 port =35;
TcpIp_SockAddrType RemoteAddrPtr ;

TcpIp_SockAddrType RemoteAddrPtr_udp ;

TcpIp_SocketIdType  SocketIdPtr_UDP ;
uint16 port_UDP =60;
extern uint8 Udp_buffer_TX [1800];

int main()
  {
        /* the parameters for core and eth and eth_IF intilization */
        /* ______________________________________________________  */
        ip_addr_t *ip =     (ip_addr_t *)  &(TcpIpLocalAddr_list[0].TcpIpStaticIpAddressConfig_t.TcpIpStaticIpAddress);
        ip_addr_t *net_ip = (ip_addr_t *)  & (TcpIpLocalAddr_list[0].TcpIpStaticIpAddressConfig_t.TcpIpNetmask);
        ip_addr_t *gt_ip =  (ip_addr_t *)  &(TcpIpLocalAddr_list[0].TcpIpStaticIpAddressConfig_t.TcpIpDefaultRouter);
        ECU_int(&netIf_List[0], 0 ,  ip   , net_ip ,gt_ip);

        /* the parameters of the remote tcp socket  */
        /* _______________________________________  */
        RemoteAddrPtr.TcpIp_SockAddrInetType_t.addr[0]=htonl(0xfbb03014);
        RemoteAddrPtr.TcpIp_SockAddrInetType_t.domain= TCPIP_AF_INET;
        RemoteAddrPtr.TcpIp_SockAddrInetType_t.port= 70;

        /* the parameters of the remote udp socket  */
        /* _______________________________________  */
        RemoteAddrPtr_udp.TcpIp_SockAddrInetType_t.addr[0]=htonl(0xfbb03014);
        RemoteAddrPtr_udp.TcpIp_SockAddrInetType_t.domain= TCPIP_AF_INET;
        RemoteAddrPtr_udp.TcpIp_SockAddrInetType_t.port= 50;

        /* SOAD starts to get a  TCP socket and open a connection with the remote socket */
        /* ______________________________________________________________________________*/
        Std_ReturnType result =TcpIp_SoAdGetSocket( TCPIP_AF_INET, TCPIP_IPPROTO_TCP,  &SocketIdPtr );
         if ( result == E_OK )
             {
             Std_ReturnType result = TcpIp_Bind( SocketIdPtr, 0, &port );
                  if(result == E_OK )
                  {
                      int x= TcpIp_TcpConnect( SocketIdPtr, & RemoteAddrPtr) ;
                  }
             }
         result =TcpIp_SoAdGetSocket( TCPIP_AF_INET, TCPIP_IPPROTO_UDP,  &SocketIdPtr_UDP );
         if ( result == E_OK )
                      {
                       TcpIp_Bind( SocketIdPtr_UDP, 0, &port_UDP );
                       UDP_CONNECT( SocketIdPtr_UDP  ,  & RemoteAddrPtr_udp);
                      }
while(1)
{

}
}

void app_task_tx_udp(void)
{
     static int i ;
     memset(Udp_buffer_TX ,'5'+ i++ ,900);
     Std_ReturnType result  =TcpIp_UdpTransmit( SocketIdPtr_UDP, Udp_buffer_TX, &RemoteAddrPtr_udp, 900 ) ;
}
#endif

#if ECU2

TcpIp_SocketIdType SocketIdPtr ;
uint16 port =70;
TcpIp_SockAddrType RemoteAddrPtr ;

TcpIp_SocketIdType  SocketIdPtr_UDP ;
uint16 port_UDP =50;
TcpIp_SockAddrType RemoteAddrPtr_udp ;
int main()
  {

        /* the parameters of the remote tcp socket  */
        /* _______________________________________  */
        ip_addr_t *ip =     (ip_addr_t *)  &(TcpIpLocalAddr_list[0].TcpIpStaticIpAddressConfig_t.TcpIpStaticIpAddress);
        ip_addr_t *net_ip = (ip_addr_t *)  & (TcpIpLocalAddr_list[0].TcpIpStaticIpAddressConfig_t.TcpIpNetmask);
        ip_addr_t *gt_ip =  (ip_addr_t *)  &(TcpIpLocalAddr_list[0].TcpIpStaticIpAddressConfig_t.TcpIpDefaultRouter);
        ECU_int(&netIf_List[0], 0 ,  ip   , net_ip ,gt_ip);


        /* SOAD starts to get a socket and listen to clients */
        /* ________________________________________________________________________  */
        TcpIp_SoAdGetSocket( TCPIP_AF_INET, TCPIP_IPPROTO_TCP,  &SocketIdPtr );
        TcpIp_Bind( SocketIdPtr, 0, &port);
        TcpIp_TcpListen( SocketIdPtr,0);


        /* the parameters of the remote udp socket  */
        /* _______________________________________  */
        RemoteAddrPtr_udp.TcpIp_SockAddrInetType_t.addr[0]=htonl(0xfbb03020);
        RemoteAddrPtr_udp.TcpIp_SockAddrInetType_t.domain= TCPIP_AF_INET;
        RemoteAddrPtr_udp.TcpIp_SockAddrInetType_t.port= 60;

        /* SOAD starts to get a socket and listen to clients */
        /* ________________________________________________________________________  */
        TcpIp_SoAdGetSocket( TCPIP_AF_INET, TCPIP_IPPROTO_UDP,  &SocketIdPtr_UDP );
        TcpIp_Bind( SocketIdPtr_UDP, 0, &port_UDP );
        UDP_CONNECT( SocketIdPtr_UDP  ,  & RemoteAddrPtr_udp);


while(1)
{

}
}
#endif
