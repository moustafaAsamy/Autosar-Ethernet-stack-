/*
 * TcpIp.h
 *
 *  Created on: Nov 7, 2019
 *      Author: lenovo
 */

#ifndef AUTOSAR_INCLUDES_TCPIP_H_
#define AUTOSAR_INCLUDES_TCPIP_H_


#include <stdint.h>
#include "Std_Types.h"
#include"autosar_includes/EthIf.h"
#define TcpIpTcpSocketMax_no 10
#define max_connection 10
typedef enum {
    TCPIP_IPPROTO_TCP = 0x06,
    TCPIP_IPPROTO_UDP = 0x11,
} TcpIp_ProtocolType;

/**
 * @brief TcpIp address families.
 * @req   SWS_TCPIP_00009
 */
typedef enum {
    TCPIP_AF_INET     = 0x02,   // ip4
    TCPIP_AF_INET6    = 0x1c,   // ip6
} TcpIp_DomainType;

/**
 * @brief Specifies the TcpIp state for a specific EthIf controller.
 * @req   SWS_TCPIP_00073
 */
typedef enum {
    /**
     * TCP/IP stack state for a specific EthIf controller is
     * ONLINE, i.e. communication via at least one IP address is
     * possible.
     */
    TCPIP_STATE_ONLINE,

    /**
     * TCP/IP stack state for a specific EthIf controller is
     * ONHOLD, i.e. no communication is currently possible
     * (e.g. link down).
     */
    TCPIP_STATE_ONHOLD,

    /**
     * TCP/IP stack state for a specific EthIf controller is
     * OFFLINE, i.e. no communication is possible.
     */
    TCPIP_STATE_OFFLINE,

    /**
     * TCP/IP stack state for a specific EthIf controller is
     * STARTUP, i.e. IP address assignment in progress or
     * ready for manual start, communication is currently not
     * possible.
     */
    TCPIP_STATE_STARTUP,

    /* TCP/IP stack state for a specific EthIf controller is
     * SHUTDOWN, i.e. release of resources using the EthIf
     * controller, release of IP address assignment.
     */
    TCPIP_STATE_SHUTDOWN,
} TcpIp_StateType;

/**
 * @brief Specifies the state of local IP address assignment
 * @req   SWS_TCPIP_00082
 */
typedef enum {
    /**
     * local IP address is assigned
     */
    TCPIP_IPADDR_STATE_ASSIGNED,

    /**
     * local IP address is assigned, but cannot be used as the network is not active
     */
    TCPIP_IPADDR_STATE_ONHOLD,

    /**
     * local IP address is unassigned
     */
    TCPIP_IPADDR_STATE_UNASSIGNED,
} TcpIp_IpAddrStateType;

/**
 * @brief Events reported by TcpIp.
 * @req   SWS_TCPIP_00031
 */
typedef enum {
    /**
     * TCP connection was reset, TCP socket and all related resources have been released.
     */
    TCPIP_TCP_RESET,

    /**
     * TCP connection was closed successfully, TCP socket and all related resources have been released.
     */
    TCPIP_TCP_CLOSED,

    /**
     * A FIN signal was received on the TCP connection, TCP socket is still valid.
     */
    TCPIP_TCP_FIN_RECEIVED,

    /**
     * UDP socket and all related resources have been released.
     */
    TCPIP_UDP_CLOSED,
} TcpIp_EventType;

typedef enum {
    /**
     * operation completed successfully.
     */
    TCPIP_OK,

    /**
     * operation failed.
     */
    TCPIP_E_NOT_OK,

    /**
     * operation failed because of an ARP/NDP cache miss.
     */
    TCPIP_E_PHYS_ADDR_MISS,
} TcpIp_ReturnType;

/**
 * @brief  Type for the specification of all supported Parameter IDs.
 * @req    SWS_TCPIP_00126
 */
typedef enum {
    /**
     * @brief Specifies the maximum TCP receive window for the socket.
     */
    TCPIP_PARAMID_TCP_RXWND_MAX            = 0x00,

    /**
     * @brief Specifies the frame priority for outgoing frames on the socket.
     */
    TCPIP_PARAMID_FRAMEPRIO                = 0x01,

    /**
     * @brief Specifies if the Nagle Algorithm according to IETF RFC 896 is enabled or not.
     */
    TCPIP_PARAMID_TCP_NAGLE                = 0x02,

    /**
     * Specifies if TCP Keep Alive Probes are sent on the socket connection.
     */
    TCPIP_PARAMID_TCP_KEEPALIVE            = 0x03,

    /**
     * @brief Specifies the time to live value for outgoing frames on the socket.
     *
     * For IPv6 this parameter specifies the value of the HopLimit field used in the IPv6 header.
     */
    TCPIP_PARAMID_TTL                      = 0x04,

    /**
     * @brief Specifies the time in [s] between the last data packet sent (simple ACKs are not considered data) and the first keepalive probe.
     */
    TCPIP_PARAMID_TCP_KEEPALIVE_TIME       = 0x05,

    /**
     * @brief Specifies the maximum number of times that a keepalive probe is retransmitted.
     */
    TCPIP_PARAMID_TCP_KEEPALIVE_PROBES_MAX = 0x06,

    /**
     * @brief Specifies the interval in [s] between subsequent keepalive probes.
     */
    TCPIP_PARAMID_TCP_KEEPALIVE_INTERVAL   = 0x07,

    /**
     * Start of vendor specific range of parameter IDs.
     */
    TCPIP_PARAMID_VENDOR_SPECIFIC          = 0x80,
} TcpIp_ParamIdType;

/**
 * @brief Development Errors
 * @req SWS_TCPIP_00042
 * @{
 */

/** @brief API service called before initializing the module */
#define TCPIP_E_NOTINIT        0x01u

/** @brief API service called with NULL pointer */
#define TCPIP_E_PARAM_POINTER  0x02u

/** @brief Invalid argument */
#define TCPIP_E_INV_ARG        0x03u

/** @brief no buffer space available */
#define TCPIP_E_NOBUFS         0x04u

/** @brief Message to long */
#define TCPIP_E_MSGSIZE        0x07u

/** @brief Protocol wrong type for socket */
#define TCPIP_E_PROTOCOL       0x08u

/** @brief Compatibility with AUTOSAR invalid doc */
#define TCPIP_E_PROTOTYPE      TCPIP_E_PROTOCOL

/** @brief Address already in use */
#define TCPIP_E_ADDRINUSE      0x09u

/** @brief Can't assignt requested address */
#define TCPIP_E_ADDRNOTAVAIL   0x0Au

/** @brief Socket is already connected */
#define TCPIP_E_ISCONN         0x0Bu

/** @brief Socket is not connected */
#define TCPIP_E_NOTCONN        0x0Cu

/** @brief Protocol is not available */
#define TCPIP_E_NOPROTOOPT     0x0Du

/** @brief Address family not supported by protocol family */
#define TCPIP_E_AFNOSUPPORT    0x0Eu

/** @brief Invalid configuration set selection */
#define TCPIP_E_INIT_FAILED    0x0Fu




typedef struct {
    TcpIp_DomainType   domain;
    uint16           port;
    uint32           addr[1];
} TcpIp_SockAddrInetType;

typedef struct {
    TcpIp_DomainType domain;
    TcpIp_SockAddrInetType TcpIp_SockAddrInetType_t ;
} TcpIp_SockAddrType;

typedef struct {
    uint32 InetAddr[1] ;
    uint8 PhysAddr[6];
    uint8 State ;
}TcpIp_ArpCacheEntryType;

typedef enum{
TCPIP_ARP_ENTRY_STATIC,
TCPIP_ARP_ENTRY_VALID,
TCPIP_ARP_ENTRY_STALE,
TCPIP_ARP_ENTRY_EMPTY
}ARP_ENTRY_state;

typedef enum{
TCPIP_IPADDR_ASSIGNMENT_STATIC ,         //Static configured IPv4/IPv6 address.
TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL_DOIP,  //Linklocal IPv4/IPv6 address assignment using DoIP parameters.
TCPIP_IPADDR_ASSIGNMENT_DHCP  ,          // Dynamic configured IPv4/IPv6 address by DHCP.
TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL,       //Linklocal IPv4/IPv6 address assignment.
TCPIP_IPADDR_ASSIGNMENT_IPV6_ROUTER,     //Dynamic configured IPv4/IPv6 address by Router Advertisement.
TCPIP_IPADDR_ASSIGNMENT_ALL,             //All configured TcpIp-AssignmentMethods with TcpIpAssignmentTrigger set to TCPIP_MANUAL
}TcpIp_IpAddrAssignmentType;
/**
 * @brief socket identifier type for unique identification of a TcpIp stack socket.
 *        TCPIP_SOCKETID_INVALID shall specify an invalid socket handle.
 * @req   SWS_TCPIP_00038
 */
typedef uint16 TcpIp_SocketIdType;

/**
 * @brief Address identification type for unique identification of a local IP address and
 *        EthIf Controller configured in the TcpIp module.
 * @req   SWS_TCPIP_00030
 */
typedef uint8  TcpIp_LocalAddrIdType;

#define TCPIP_IPADDR_ANY       0x0u
#define  ANY                   0x00000000
#define TCPIP_IP6ADDR_ANY      0x0u
#define TCPIP_PORT_ANY         0x0u
#define TCPIP_SOCKETID_INVALID (TcpIp_SocketIdType)0xffffu
#define TCPIP_LOCALADDRID_ANY  (TcpIp_LocalAddrIdType)0xffu

typedef enum {
    SC1,
    SC2,
    SC3
}TcpIpScalabilityClass;

typedef struct {
uint8 TcpIpArpEnabled;
uint8 TcpIpIpV4Enabled;
uint8 TcpIpLocalAddrIpv4EntriesMax;
uint8 TcpIpPathMtuDiscoveryEnabled;
}TcpIpIpV4General;

typedef struct {
    uint32 TcpIpBufferMemory;  //Memory size in bytes reserved for TCP/IP buffers.
    uint8 TcpIpDevErrorDetect; //true: detection and notification is enabled.
    uint8 TcpIpDhcpServerEnabled ;
    uint8 TcpIpGetAndResetMeasurementDataApi;
    uint32 TcpIpMainFunctionPeriod ;  //Period of TcpIp_MainFunction in [s].
    TcpIpScalabilityClass TcpIpScalabilityClass_t ;
    uint8 TcpIpTcpEnabled ;
    uint16 TcpIpTcpSocketMax ;  //Maximum number of TCP sockets
    uint8 TcpIpUdpEnabled ;
    uint16 TcpIpUdpSocketMax ;
    TcpIpIpV4General TcpIpIpV4General_t ;
}TcpIpGeneral;




typedef struct{
  //  TcpIpArpConfigRef * TcpIpArpConfigRef_t;
}TcpIpIpV4Ctrl;

typedef struct{
    TcpIpIpV4Ctrl TcpIpIpV4Ctrl_t;
}TcpIpIpVXCtrl;

typedef struct{
uint8 TcpIpIpFramePrioDefault ;
struct EthIfController *TcpIpEthIfCtrlRef ; //Reference to EthIf controller where the IP address shall be assigned.
TcpIpIpVXCtrl TcpIpIpVXCtrl_t;
}TcpIpCtrl;





typedef struct{
uint8 TcpIpArpNumGratuitousARPonStartup ;
uint8 TcpIpArpPacketQueueEnabled ;
uint32 TcpIpArpTableEntryTimeout ;
uint16 TcpIpArpTableSizeMax;
}TcpIpArpConfig;

typedef struct{
    TcpIpArpConfig TcpIpArpConfig_t ;
}TcpIpIpV4Config;


typedef enum {
    TCPIP_DHCP  ,
    TCPIP_IPV6_ROUTER,
    TCPIP_LINKLOCAL,
    TCPIP_LINKLOCAL_DOIP,
    TCPIP_STATIC
}TcpIpAssignmentMethod;

typedef enum {
    TCPIP_AUTOMATIC ,   //Assignment shall be initiated automatically by TCP/IP stack.
    TCPIP_MANUAL        // Assignment shall be initiated manually via TcpIp_RequestIpAddrAssignment().
}TcpIpAssignmentTrigger;

//This container is a subcontainer of TcpIpLocalAddr and specifies the assignment policy for the IP address.
typedef struct{
TcpIpAssignmentMethod TcpIpAssignmentMethod_t;
uint8 TcpIpAssignmentPriority ;
TcpIpAssignmentTrigger TcpIpAssignmentTrigger_t;
}TcpIpAddrAssignment;

typedef struct {
    uint32 TcpIpDefaultRouter ;
    uint32 TcpIpNetmask;
    uint32 TcpIpStaticIpAddress ;
}TcpIpStaticIpAddressConfig;

typedef enum {
    TCPIP_ANYCAST ,
    TCPIP_MULTICAST,
    TCPIP_UNICAST,
}TcpIpAddressType;

typedef struct
{
 TcpIpAddressType TcpIpAddressType_t ;
 uint16 TcpIpAddrId ;
 TcpIp_DomainType TcpIpDomainType_t;
 TcpIpCtrl * TcpIpCtrlRef_t ;// Reference to a TcpIpCtrl specifying the EthIf Controller where the IP address shall be assigned , each ip address must have only one adaptor
 TcpIpAddrAssignment TcpIpAddrAssignment_t ;
 TcpIpStaticIpAddressConfig TcpIpStaticIpAddressConfig_t;
}TcpIpLocalAddr;






typedef struct{
    TcpIpIpV4Config TcpIpIpV4Config_t;
}TcpIpIpConfig;

typedef struct {
    uint8 TcpIpTcpCongestionAvoidanceEnabled ;
uint8 TcpIpTcpFastRecoveryEnabled;
uint8 TcpIpTcpFastRetransmitEnabled;
uint32 TcpIpTcpFinWait2Timeout;
uint8 TcpIpTcpKeepAliveEnabled;
uint32 TcpIpTcpKeepAliveInterval;
uint32 TcpIpTcpKeepAliveProbesMax;
uint32 TcpIpTcpKeepAliveTime;
uint8 TcpIpTcpMaxRtx;
uint32 TcpIpTcpMsl;
uint8 TcpIpTcpNagleEnabled ;
uint16 TcpIpTcpReceiveWindowMax ;
uint32 TcpIpTcpRetransmissionTimeout ;
uint8 TcpIpTcpSynMaxRtx ;
uint8 TcpIpTcpTtl ;
}TcpIpTcpConfig;

typedef struct{
    TcpIpCtrl TcpIpCtrl_t;
    TcpIpIpConfig TcpIpIpConfig_t;
    TcpIpLocalAddr TcpIpLocalAddr_t ;
    TcpIpTcpConfig TcpIpTcpConfig_t ;
}TcpIpConfig;

typedef enum
{
    unconnected,
    listen,
    connected,
    established,
}socket_state;

struct TcpIp_SocketType
{
    TcpIp_DomainType   Domain;         /* IP4 or IP6 */
    TcpIp_ProtocolType Protocol;       /* TCP or UDP */
    TcpIp_SocketIdType SocketID;       /* socket id assigned by the TCPIP module */
    uint16 port;                       /* port no.*/
    uint16 TcpIpAddrId ;
    void  *ptr_connection  ;          /* array to connections , need to be casted to TCB */
    uint8 used  ;                                   /* 0 refer to free , 1 refer to used */
    socket_state state;                      // connected = 1, established = 2
    uint8 data_flag ;
};

typedef struct {
  uint8 used ;
  struct pbuf *p;
  uint8 controller_id;
  const uint8 * dest_mac ;
  uint16 len ;
  uint8 free_buffer ;
}buffer;

#define NO_controllers 3
#define max_cache      10

extern buffer buffer_list[20];

Std_ReturnType TcpIp_SoAdGetSocket( TcpIp_DomainType Domain, TcpIp_ProtocolType Protocol, TcpIp_SocketIdType* SocketIdPtr );
Std_ReturnType TcpIp_TcpListen( TcpIp_SocketIdType SocketId, uint16 MaxChannels );
Std_ReturnType TcpIp_Bind( TcpIp_SocketIdType SocketId, TcpIp_LocalAddrIdType LocalAddrId, uint16* PortPtr );
Std_ReturnType TcpIp_TcpConnect( TcpIp_SocketIdType SocketId, const TcpIp_SockAddrType* RemoteAddrPtr ) ;
Std_ReturnType TcpIp_TcpReceived( TcpIp_SocketIdType SocketId, uint32 Length );
Std_ReturnType TcpIp_Close(TcpIp_SocketIdType SocketId, boolean Abort);
Std_ReturnType TcpIp_UdpTransmit( TcpIp_SocketIdType SocketId, const uint8* DataPtr, const TcpIp_SockAddrType* RemoteAddrPtr, uint16 TotalLength );
Std_ReturnType TcpIp_TcpTransmit( TcpIp_SocketIdType SocketId, const uint8* DataPtr, uint32 AvailableLength, boolean ForceRetrieve );
void TcpIp_MainFunction( void );
void UDP_CONNECT( TcpIp_SocketIdType SocketId  , TcpIp_SockAddrType * RemoteAddrPtr_udp);
#endif /* AUTOSAR_INCLUDES_TCPIP_H_ */
