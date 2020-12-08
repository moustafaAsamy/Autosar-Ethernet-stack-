#include "autosar_includes/TcpIp.h"
#include "autosar_includes/ComStack_Types.h"
#include "autosar_includes/TCPIP_config.h"
#include "Eth.h"
#include "SoAd_Cbk.h"
#include "lwip/tcp.h"
#include "lwip/tcp_impl.h"
#include "lwip/etharp.h"
#include "lwip_int.h"
#include "lwip/udp.h"
#include <stdlib.h>
#include "lwip_int.h"

extern struct netif netIf_List[5];
extern void enc_send_packet(const uint8_t *buf, uint16_t count);
sint8 check_socket_list(void);
buffer buffer_list[20];

/*
 * TRUE : connection will immediately be terminated by sending a RST-Segment and releasing all related resources.
 * FALSE: connection will be terminated after performing a regular connection termination handshake and releasing all related resources.
 *
 * take care that when we are going to call the tcp output in main or directly
 *
 */

Std_ReturnType TcpIp_Close(TcpIp_SocketIdType SocketId, boolean Abort)
{
    TcpIp_EventType Event ;
    if (sockets_list[SocketId].Protocol == TCPIP_IPPROTO_TCP)
    {
        if(Abort==1)
            {
                if((struct tcp_pcb *) sockets_list[SocketId].ptr_connection == NULL)
                        {
                            return E_NOT_OK;
                        }
             tcp_abandon((struct tcp_pcb *) sockets_list[SocketId].ptr_connection,1);
             Event =TCPIP_TCP_RESET;
             SoAd_TcpIpEvent( SocketId , Event ); //  Notify the upper
             return E_OK;
            }
            else
            {
                if((struct tcp_pcb *) sockets_list[SocketId].ptr_connection == NULL)
                {
                    return E_NOT_OK;
                }
                err_t error = tcp_close((struct tcp_pcb *) sockets_list[SocketId].ptr_connection);
                Event =TCPIP_TCP_CLOSED;
                SoAd_TcpIpEvent( SocketId, Event );   // Notify the upper
                if (error == ERR_OK)     { return E_OK;}
                else {return E_NOT_OK;}
            }

    }
    else if(sockets_list[SocketId].Protocol == TCPIP_IPPROTO_UDP)
    {
        Event =TCPIP_UDP_CLOSED;
        SoAd_TcpIpEvent( SocketId, Event );   // Notify the upper
        mem_free(   (struct udp_pcb* )(sockets_list[SocketId].ptr_connection ) );
        sockets_list[SocketId].ptr_connection =NULL;  // to avoid dangling pointer issue
    }

}
// we make a socket in listen , where it was blinded to any or specifed
// the connection inherit the ip address from the incoming frme
Std_ReturnType TcpIp_TcpListen( TcpIp_SocketIdType SocketId, uint16 MaxChannels )
{
    struct tcp_pcb * TCB = tcp_new();
       if( TCB !=NULL)
       {
         sockets_list[SocketId].ptr_connection = TCB;
         uint8 j =sockets_list[SocketId].TcpIpAddrId;
         tcp_listen( (struct tcp_pcb *) sockets_list[SocketId].ptr_connection , sockets_list[SocketId].port ,   (ip_addr_t *) &(TcpIpLocalAddr_list[j].TcpIpStaticIpAddressConfig_t.TcpIpStaticIpAddress)) ;
         sockets_list[SocketId].state = listen;
         tcp_err((struct tcp_pcb *) sockets_list[SocketId].ptr_connection, error_event);

       return E_OK;
       }
       else
       {
          uint8 error_code =TCPIP_E_NOBUFS;
                  //function erorr
                  return E_NOT_OK;
       }
}

Std_ReturnType TcpIp_SoAdGetSocket( TcpIp_DomainType Domain, TcpIp_ProtocolType Protocol, TcpIp_SocketIdType* SocketIdPtr )
{
    uint8 error_code =0;
        if (  !((Domain == TCPIP_AF_INET ) ||(Domain == TCPIP_AF_INET6  ) ))
        {
         error_code |= TCPIP_E_AFNOSUPPORT ;
        //function error
         return E_NOT_OK;
        }

        if ( !((Protocol ==TCPIP_IPPROTO_TCP ) || (Protocol == TCPIP_IPPROTO_UDP) ))
        {
            error_code |=  TCPIP_E_PROTOCOL;
            //function error
            return E_NOT_OK;
        }

        sint8 index = check_socket_list();
        if (index<0)
        {return E_NOT_OK;}                              /*The request has not been accepted: no free socket*/

        sockets_list[index].Domain   = Domain;
        sockets_list[index].Protocol = Protocol;
        sockets_list[index].SocketID = index;
        sockets_list[index].used = 1;
        sockets_list[index].data_flag =0;
        sockets_list[index].state = unconnected;
        *SocketIdPtr=index;
        return E_OK ;                                  /*The request has  been accepted there is a free socket*/
}

Std_ReturnType TcpIp_Bind( TcpIp_SocketIdType SocketId, TcpIp_LocalAddrIdType LocalAddrId, uint16* PortPtr )
{
        uint8 error_code =0;
        if(LocalAddrId > 9)                                  // Assume the max no of address is 10
         {
             error_code |= TCPIP_E_ADDRNOTAVAIL;
             return E_NOT_OK;
         }
        uint8 i = 0;
        /* tcp socket */
        if ( sockets_list[SocketId].Protocol ==  TCPIP_IPPROTO_TCP  )
        {
        for(i=0;i<TcpIpTcpSocketMax_no;i++)
         {
           if ((sockets_list[i].used == 1 )&&(sockets_list[i].port == *PortPtr) &&( i!=SocketId))
             {
                uint8 j = sockets_list[i].TcpIpAddrId ;
                if(j!=LocalAddrId)
                {
                  if ( TcpIpLocalAddr_list[j].TcpIpStaticIpAddressConfig_t.TcpIpStaticIpAddress ==TcpIpLocalAddr_list[LocalAddrId].TcpIpStaticIpAddressConfig_t.TcpIpStaticIpAddress )
                  {
                      error_code =  TCPIP_E_ADDRINUSE;
                      return E_NOT_OK;
                  }
               }
             }
         }
        sockets_list[SocketId].port = *PortPtr;
        sockets_list[SocketId].TcpIpAddrId = LocalAddrId;
        return E_OK;
        }
        /* udp socket */
        else if ( sockets_list[SocketId].Protocol ==  TCPIP_IPPROTO_UDP )
        {
            for(i=0;i<TcpIpTcpSocketMax_no;i++)
                     {
                       if ((sockets_list[i].used == 1 ) &&( i!=SocketId) &&(sockets_list[i].port == *PortPtr))
                         {
                            uint8 j = sockets_list[i].TcpIpAddrId ;
                            if(j!=LocalAddrId)
                            {
                              if ( TcpIpLocalAddr_list[j].TcpIpStaticIpAddressConfig_t.TcpIpStaticIpAddress ==TcpIpLocalAddr_list[LocalAddrId].TcpIpStaticIpAddressConfig_t.TcpIpStaticIpAddress )
                              {
                                  error_code =  TCPIP_E_ADDRINUSE;
                                  return E_NOT_OK;
                              }
                           }
                         }
                     }
                 sockets_list[SocketId].port = *PortPtr;
                 sockets_list[SocketId].TcpIpAddrId = LocalAddrId;
                 struct udp_pcb* PCB =  udp_new();
                 if ( PCB != NULL)
                 {
                     err_t   error_code  = udp_bind (PCB, (ip_addr_t *)  &(TcpIpLocalAddr_list[LocalAddrId].TcpIpStaticIpAddressConfig_t.TcpIpStaticIpAddress) ,sockets_list[SocketId].port);
                       if ( error_code == ERR_OK )
                       {
                           //udp_connect(PCB,(ip_addr_t *)  &(TcpIpLocalAddr_list[LocalAddrId].TcpIpStaticIpAddressConfig_t.TcpIpStaticIpAddress) ,sockets_list[SocketId].port);
                           udp_recv(PCB,  UDP_recv, NULL);
                           PCB->socket_id = SocketId;
                           sockets_list[SocketId].ptr_connection =PCB;
                           sockets_list[SocketId].port = *PortPtr;
                           sockets_list[SocketId].TcpIpAddrId = LocalAddrId;
                           return E_OK;

                       }
                 }
                 else
                 {
                     return E_NOT_OK ;
                 }
        }
}

Std_ReturnType TcpIp_TcpConnect( TcpIp_SocketIdType SocketId, const TcpIp_SockAddrType* RemoteAddrPtr )
{
    uint8 error_code =0;
    if(sockets_list[SocketId].used == 0) // not used socket
       {
        error_code |=TCPIP_E_INV_ARG;
                     //function erorr
        return E_NOT_OK;
       }
    if(sockets_list[SocketId].Protocol != TCPIP_IPPROTO_TCP)
    {
        error_code |=TCPIP_E_PROTOTYPE;
              //function erorr
        return E_NOT_OK;
    }
    if ((sockets_list[SocketId].state == connected) || (sockets_list[SocketId].state == established ))
        {
        error_code |=TCPIP_E_ISCONN;
                      //function erorr
                return E_NOT_OK;
        }

    struct tcp_pcb * TCB = tcp_new();
    if( TCB !=NULL)
    {
    TCB->socket_ID = SocketId;
    sockets_list[SocketId].ptr_connection = TCB ;
    uint8 j = sockets_list[SocketId].TcpIpAddrId ;
    err_t result =tcp_bind(TCB, (ip_addr_t *)& TcpIpLocalAddr_list[j].TcpIpStaticIpAddressConfig_t.TcpIpStaticIpAddress, sockets_list[SocketId].port) ;
   if( result == ERR_USE)
       {
       error_code |=TCPIP_E_ADDRINUSE;
       //function erorr
       return E_NOT_OK;
       }
   else if(result == ERR_BUF)
   {
       error_code |=TCPIP_E_NOBUFS;
       //function erorr
       return E_NOT_OK;
   }
   if (result !=ERR_OK)
    {return E_NOT_OK;}

    tcp_err( (struct tcp_pcb *)sockets_list[SocketId].ptr_connection , error_event);
    sint8 error =tcp_connect( (struct tcp_pcb *)sockets_list[SocketId].ptr_connection  , (ip_addr_t * ) &(RemoteAddrPtr->TcpIp_SockAddrInetType_t.addr) , RemoteAddrPtr->TcpIp_SockAddrInetType_t.port , conection_established);

    if ( (error == ERR_MEM )|| (error == ERR_USE) )
    {
        error_code |=TCPIP_E_NOBUFS;
        //function erorr
         return E_NOT_OK;
    }
    else if (error == ERR_OK)
    {
        //sockets_list[SocketId].data_flag =1; extra
        return E_OK;
    }
    }

    else
    {
        error_code |=TCPIP_E_NOBUFS;
                //function erorr
                 return E_NOT_OK;
    }
}

/*
 * The service TcpIp_TcpReceived() shall increase the TCP receive window of
 *  the socket specified by SocketId considering the number of finally consumed bytes specified by Length.
 */
Std_ReturnType TcpIp_TcpReceived( TcpIp_SocketIdType SocketId, uint32 Length )
{
   tcp_recved((struct tcp_pcb *)sockets_list[SocketId].ptr_connection, Length);             // Indicate that you have received and increase the receive window .
   return E_OK;
}



/*
 * This service requests transmission of data via TCP to a remote node.
 *  The transmission of the data is decoupled is done later by the main function.
 *  Enqueue TCP segment only
 */
Std_ReturnType TcpIp_TcpTransmit( TcpIp_SocketIdType SocketId, const uint8* DataPtr, uint32 AvailableLength, boolean ForceRetrieve )
{
    uint8 error_code =0;
    if(( (struct tcp_pcb * )sockets_list[SocketId].ptr_connection !=NULL) &&  (sockets_list[SocketId].Protocol == TCPIP_IPPROTO_TCP ) && (sockets_list[SocketId].used ==1))
          {
     if (sockets_list[SocketId].state!= established)
         { return E_NOT_OK;}      /* make sure that you already have been connected otherwise refuse the the transmission */
     err_t error= tcp_write( (struct tcp_pcb *) sockets_list[SocketId].ptr_connection,   DataPtr  , AvailableLength  , 0x01);
     if ( (error == ERR_MEM ) || (error == ERR_USE) )
         {
             error_code |=TCPIP_E_NOBUFS;
             //function erorr
              return E_NOT_OK;
         }
     else
         {
            err_t result = tcp_output((struct tcp_pcb * )sockets_list[SocketId].ptr_connection);
             if (  result == ERR_OK)
               {
                 //UARTprintf("  %X \n",1);
                 return E_OK ;
               }
             else
                 {
                     return E_NOT_OK;
                 }
         }
          }
    else
    {
        return E_NOT_OK;
    }
}
// there will be a place holders till eth and eth_if are implemented ,the parameters will be unused till the 2 modules are completed
//void TcpIp_RxIndication( uint8 CtrlIdx, Eth_FrameType FrameType, boolean IsBroadcast, const uint8* PhysAddrPtr, const uint8* DataPtr, uint16 LenByte )
//{
//    struct pbuf *pBuf  = pbuf_alloc(PBUF_RAW, LenByte);
//    if(pBuf != NULL)
//    {
//    {memcpy(pBuf->payload, DataPtr, LenByte);}
//    if(ethernet_input(pBuf, &netIf_List[0]) != ERR_OK)
//     { pbuf_free(pBuf);}
//    }
//}

void TcpIp_RxIndication( uint8_t CtrlIdx, uint8_t FrameType, uint8_t IsBroadcast, const uint8_t* PhysAddrPtr, const uint8_t* DataPtr, uint16_t LenByte )
{
    struct pbuf *pBuf  = pbuf_alloc(PBUF_RAW, LenByte,PBUF_POOL);
    struct pbuf *intial_buffer  = pBuf;
    int copy_lenght,pos =0;
    if(pBuf != NULL)
    {
        while(pBuf &&  (LenByte>0))
        {
            //copy_lenght = PBUF_POOL_BUFSIZE < LenByte ? PBUF_POOL_BUFSIZE : LenByte ;
            memcpy(pBuf->payload, DataPtr+pos, copy_lenght = PBUF_POOL_BUFSIZE < LenByte ? PBUF_POOL_BUFSIZE : LenByte );
            LenByte = LenByte - PBUF_POOL_BUFSIZE <0 ? 0 : LenByte - PBUF_POOL_BUFSIZE ;
            pBuf=pBuf->next;
            pos = copy_lenght+pos;
        }
    if(ethernet_input(intial_buffer, &netIf_List[0]) != ERR_OK)
     { pbuf_free(intial_buffer);}
    }
}

void TcpIp_MainFunction( void )
{
 uint8 index=0;
 for (index =0; index <20;index ++)
   {
     if (buffer_list[index].used == 1)
     {
          Eth_BufIdxType  BufIdxPtr =0;
          EthIf_ProvideTxBuffer( buffer_list[index].controller_id, 0, 0, & BufIdxPtr,   (uint8 **)(&buffer_list[index].p->payload)   , &buffer_list[index].len ) ;
          if(buffer_list[index].p->free_buffer)
          {
              pbuf_free(buffer_list[index].p);
          }
          else
          {
//              pbuf_header(buffer_list[index].p, -14);
//              pbuf_header(buffer_list[index].p, -20);
          }
          buffer_list[index].p = 0;                 // to avoid dangle pointer
          buffer_list[index].dest_mac =0;
          buffer_list[index].controller_id =0;
          buffer_list[index].len =0;
          buffer_list[index].used =0;
     }
   }
}

Std_ReturnType TcpIp_UdpTransmit( TcpIp_SocketIdType SocketId, const uint8* DataPtr, const TcpIp_SockAddrType* RemoteAddrPtr, uint16 TotalLength )
{
    if(( (struct udp_pcb * )sockets_list[SocketId].ptr_connection !=NULL) &&  (sockets_list[SocketId].Protocol == TCPIP_IPPROTO_UDP ) && (sockets_list[SocketId].used ==1))
      {
        err_t result = udp_send((struct udp_pcb * )sockets_list[SocketId].ptr_connection,(uint8* ) DataPtr, TotalLength,  (ip_addr_t * ) &(RemoteAddrPtr->TcpIp_SockAddrInetType_t.addr) , RemoteAddrPtr->TcpIp_SockAddrInetType_t.port);
         if (result  ==ERR_OK )
        {
         return E_OK;
        }
     else if( (result == ERR_MEM ) || (result == ERR_USE) )
     {
         result =TCPIP_E_NOBUFS;
         //function erorr
          return E_NOT_OK;
     }
       }
    else
    {
        return E_NOT_OK;
    }
}




err_t enc_low_level_output(struct netif *netif, struct pbuf *p , const uint8* PhysAddrPtr , u8_t free_buffer  )
{
    uint8 i =0;
    for(i=0;i<max_no_buffers;i++) // maxium no of buffers is 10
    {
      if( buffer_list[i].used == 0 )
      {
          buffer_list[i].p = p;
          buffer_list[i].free_buffer = free_buffer;
          buffer_list[i].controller_id = netif->ctr_ID;
          buffer_list[i].dest_mac = PhysAddrPtr;
          buffer_list[i].len = p->len;
          buffer_list[i].used =1;
          return ERR_OK;
      }
    }
    return ERR_MEM;
}




sint8 check_socket_list(void)
{
    uint8 i =0;
    for(i=0;i<TcpIpTcpSocketMax_no;i++)
    {
        if( sockets_list[i].used==0)
        {return i;}
    }
    return -1 ;
}

void UDP_CONNECT( TcpIp_SocketIdType SocketId  , TcpIp_SockAddrType * RemoteAddrPtr_udp)
{

udp_connect( (struct udp_pcb *) sockets_list[SocketId].ptr_connection  , (ip_addr_t *) (&RemoteAddrPtr_udp->TcpIp_SockAddrInetType_t.addr) ,  RemoteAddrPtr_udp->TcpIp_SockAddrInetType_t.port );

}









