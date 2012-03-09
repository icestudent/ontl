/**\file*********************************************************************
 *                                                                     \brief
 *  Winsock Kernel
 *
 ****************************************************************************
 */
#ifndef NTL__KM_WINSOCK
#define NTL__KM_WINSOCK
#pragma once

#include "basedef.hxx"
#include "irp.hxx"

namespace ntl { namespace km {

  /**\addtogroup ntl_km Kernel-mode subsystem library 
   @{*/

  /// Winsock Kernel library (WSK)
  namespace wsk {

    /**\defgroup ntl_km_wsk Winsock Kernel library
      Winsock Kernel (WSK) is a kernel-mode Network Programming Interface (NPI). 
     @{*/

#ifndef NTL__NO_AUTOLINK
# pragma comment(lib, "netio.lib")
#endif

    /**\defgroup net_socket_types Common socket types
     @{*/
    namespace af
    {
      enum type: uint16_t {
        unspec,
        unix,
        inet,
        implink,
        pup,
        chaos,
        ns,
        ipx = ns,
        iso,
        osi = iso,
        ecma,
        datakit,
        ccitt,
        sna,
        decnet,
        dli,
        lat,
        hylink,
        appletalk,
        netbios,
        voiceview,
        firefox,
        unknown1,
        ban,
        atm,
        inet6,
        cluster,
        af_12844,
        irda,
        netdes    = 28,
        // xp+
        tcnprocess,
        tcnmessage,
        iclfxbm,
        // vista+
        bth,
        // win7+
        link,
        max
      };
      static_assert(irda == 26, "brb");
      static_assert(max == 34, "invalid address_family code");
    }

    typedef af::type address_family;

    namespace ipproto
    {
      enum type: uint8_t {
        hopopts       = 0,
        icmp          = 1,
        igmp          = 2,
        ggp           = 3,
        ipv4          = 4,
        st            = 5,
        tcp           = 6,
        cbt           = 7,
        egp           = 8,
        igp           = 9,
        pup           = 12,
        udp           = 17,
        idp           = 22,
        rdp           = 27,
        ipv6          = 41,
        routing       = 43,
        fragment      = 44,
        esp           = 50,
        ah            = 51,
        icmpv6        = 58,
        none          = 59,
        dstopts       = 60,
        nd            = 77,
        iclfxbm       = 78,
        pim           = 103,
        pgm           = 113,
        l2tp          = 115,
        sctp          = 132,
        raw           = 255,
        max           = 256
        //reserved_raw  = 257,
        //reserved_ipsec  = 258,
        //reserved_ipsecoffload  = 259,
        //reserved_max  = 260
      };
    }
#pragma warning(disable:4201) // nameless union
    struct in_addr
    {
      union 
      {
        struct { uint8_t b1, b2, b3, b4; };
        struct { uint16_t w1, w2; };
        uint32_t addr;
      };
    };
#pragma warning(default:4201)

    struct in_addr6
    {
      union
      {
        uint8_t   Byte[16];
        uint16_t  Word[8];
      };
    };
    typedef in_addr6 in6_addr;

    struct sockaddr
    {
      enum type: uint16_t
      {
        stream = 1,
        dgram,
        raw,
        rdm,
        seqpacket
      };
      int16_t   family;
      uint16_t  port;
    };

    struct sockaddr_in: sockaddr
    {
      in_addr   addr;
      char      reserved[8];
    };

    struct sockaddr_in6: sockaddr
    {
      uint32_t  flowinfo;
      in6_addr  addr;
      uint32_t  scope_id;
    };

    struct hostent
    {
      char*   name;
      char**  aliases;
      int16_t addrtype;
      int16_t length;
      char**  addr_list;
    };

    struct addrinfo
    {
      int32_t   flags;
      int32_t   family;
      int32_t   socktype;
      int32_t   protocol;
      size_t    addrlen;
      char*     caconname;
      sockaddr* addr;
      addrinfo* next;
    };

    struct addrinfoW
    {
      int32_t     flags;
      int32_t     family;
      int32_t     socktype;
      int32_t     protocol;
      size_t      addrlen;
      wchar_t*    caconname;
      sockaddr*   addr;
      addrinfoW*  next;
    };

    struct addrinfoex
    {
      int32_t     flags;
      int32_t     family;
      int32_t     socktype;
      int32_t     protocol;
      size_t      addrlen;
      char*       caconname;
      sockaddr*   addr;
      void*       blob;
      size_t      bloblen;
      guid*       provider;
      addrinfoex* next;
    };

    struct addrinfoexW
    {
      int32_t     flags;
      int32_t     family;
      int32_t     socktype;
      int32_t     protocol;
      size_t      addrlen;
      wchar_t*    caconname;
      sockaddr*   addr;
      void*       blob;
      size_t      bloblen;
      guid*       provider;
      addrinfoexW*next;
    };

    namespace wsa
    {
      struct cmsghdr
      {
        size_t  len;
        int     level;
        int     type;
        /* followed by byte data[] */
      };
    } // wsa

    /** @} net_socket_types */

    /**\defgroup wsk_types wsk types
     @{*/
    typedef struct {} client;

    struct socket
    {
      enum flags {
        basic,
        listen,
        connection,
        datagram   = 4
      };
      const void* Dispatch;
    };

    struct tdi_map
    {
      sockaddr::type  SocketType;
      address_family  AddressFamily;
      ipproto::type   Protocol;
      const wchar_t*  TdiDeviceName;
    };

    struct control_client
    {
      enum code {
        TransportListQuery = 2,
        TransportListChange,
        CacheSd,
        ReleaseSd,
        TdiDevicenameMapping,
        SetStaticEventCallbacks,
        TdiBehaviour
      };

      struct transport
      {
        static const code ControlCode = TransportListQuery;
        uint16_t        Version;
        sockaddr::type  SocketType;
        ipproto::type   Protocol;
        address_family  AddressFamily;
        guid            ProviderId;
      };

      struct event_callback_control
      {
        static const code ControlCode = SetStaticEventCallbacks;
        const guid* NpiId;
        uint32_t    EventMask;
      };

      struct tdi_map_info
      {
        static const code ControlCode = TransportListQuery;
        const uint32_t  ElementCount;
        const tdi_map*  Map;
      };
    };


    struct buf_t
    {
      mdl*      Mdl;
      uint32_t  Offset;
      size_t    Length;
    };

    struct inspect_id
    {
      enum action {
        WskInspectReject,
        WskInspectAccept,
        WskInspectPend,
        WskInspectMax
      };
      uintptr_t Key;
      uint32_t  SerialNumber;
    };

    /**@}*/

    /**\defgroup wsk_events wsk events
     @{*/
    typedef ntstatus __stdcall client_event_t(
        __in void*  ClientContext,
        __in uint32_t  Event,
        __in void*  Information __optional,
        __in size_t  InformationLength
        );

    ///\name socket-level events
    typedef ntstatus __stdcall receive_event_t (
      __in_opt void*  SocketContext,
      __in uint32_t      Flags,
      __in_opt struct data_indication* DataIndication,
      __in size_t     BytesIndicated,
      __inout size_t *BytesAccepted
      );

    typedef ntstatus __stdcall disconnect_event_t (
      __in_opt void* SocketContext,
      __in uint32_t     Flags
      );

    typedef ntstatus __stdcall send_backlog_event_t (
      __in_opt void* SocketContext,
      __in size_t    IdealBacklogSize
      );

    typedef ntstatus __stdcall accept_event_t (
      __in_opt void*         SocketContext,
      __in uint32_t             Flags,
      __in sockaddr*         LocalAddress,
      __in sockaddr*         RemoteAddress,
      __in_opt socket*   AcceptSocket,
      __deref_out_opt void* *AcceptSocketContext,
      __deref_out_opt const struct client_connection_dispatch **AcceptSocketDispatch
      );

    typedef inspect_id::action __stdcall inspect_event_t (
      __in_opt void*           SocketContext,
      __in sockaddr*           LocalAddress,
      __in sockaddr*           RemoteAddress,
      __in_opt inspect_id* InspectID
      );

    typedef ntstatus __stdcall abort_event_t (
      __in_opt void*       SocketContext,
      __in inspect_id* InspectID
      );

    typedef ntstatus __stdcall receive_from_event_t(
      __in void*    SocketContext,
      __in uint32_t Flags,
      __in struct datagram_indication* DataIndication __optional
      );

    ///\}
    /** @} */

    /**\defgroup wsk_provider_fn wsk provider functions
     @{*/
    ///\name client-level functions
    ///\cond __doxygen_failure
    typedef ntstatus (__stdcall *PSocket) (
      __in client*              Client,
      __in address_family       AddressFamily,
      __in sockaddr::type       SocketType,
      __in ipproto::type        Protocol,
      __in socket::flags        Flags,
      __in_opt void*                SocketContext,
      __in_opt const void          *Dispatch,
      __in_opt eprocess*            OwningProcess,
      __in_opt ethread*             OwningThread,
      __in_opt security_descriptor* SecurityDescriptor,
      __inout irp*                  Irp
      );

    typedef ntstatus (__stdcall *PSocketConnect) (
      __in client*                               Client,
      __in uint16_t                                    SocketType,
      __in uint32_t                                     Protocol,
      __in sockaddr*                                 LocalAddress,
      __in sockaddr*                                 RemoteAddress,
      __reserved uint32_t                               Flags,
      __in_opt void*                                 SocketContext,
      __in_opt const struct client_connection_dispatch *Dispatch,
      __in_opt eprocess*                             OwningProcess,
      __in_opt ethread*                              OwningThread,
      __in_opt security_descriptor*                  SecurityDescriptor,
      __inout irp*                                   Irp
      );

    typedef ntstatus (__stdcall *PControlClient) (
      __in client*                    Client,
      __in uint32_t                          ControlCode,
      __in size_t                         InputSize,
      __in_opt void*    InputBuffer,
      __in size_t                         OutputSize,
      __out_opt void*  OutputBuffer,
      __out_opt size_t                   *OutputSizeReturned,
      __inout_opt irp*                    Irp
      );

    typedef ntstatus (__stdcall * PGetAddressInfo) (
      __in client*          Client,
      __in_opt unicode_string*  NodeName,
      __in_opt unicode_string*  ServiceName,
      __in_opt uint32_t            NameSpace,
      __in_opt guid            *Provider,    
      __in_opt addrinfoexW     *Hints,
      __deref_out addrinfoexW **Result, 
      __in_opt eprocess*        OwningProcess,
      __in_opt ethread*         OwningThread,
      __inout irp*              Irp
      );

    typedef void (__stdcall *PFreeAddressInfo) (
      __in client*  Client,
      __in addrinfoexW* AddrInfo
      );

    typedef ntstatus (__stdcall *PGetNameInfo) (
      __in client*          Client,
      __in sockaddr*            SockAddr,
      __in uint32_t                SockAddrLength,
      __out_opt unicode_string* NodeName,
      __out_opt unicode_string* ServiceName,
      __in uint32_t                Flags,
      __in_opt eprocess*        OwningProcess,
      __in_opt ethread*         OwningThread,
      __inout irp*              Irp
      );
    ///\endcond

    ///\name socket-level functions


    ///\}
    /** @} */

    /**\addtogroup wsk_types wsk types
     @{*/
    struct client_npi
    {
      void* ClientContext;
      const struct client_dispatch *Dispatch;
    };

    struct client_dispatch
    {
      uint16_t  Version;
      uint16_t  Reserved;
      client_event_t* WskClientEvent;
    };

    struct client_listen_dispatch
    {
      accept_event_t*  WskAcceptEvent;
      inspect_event_t*  WskInspectEvent;
      abort_event_t*  WskAbortEvent;
    };

    struct data_indication
    {
      data_indication *Next;
      buf_t            Buffer;
    };

    struct datagram_indication
    {
      datagram_indication *Next;
      buf_t                Buffer;
      struct wsa::cmsghdr* ControlInfo;
      uint32_t             ControlInfoLength;
      sockaddr*            RemoteAddress;
    };

    struct client_datagram_dispatch
    {
      receive_from_event_t*  WskReceiveFromEvent;
    };

    struct client_connection_dispatch
    {
      receive_event_t*       WskReceiveEvent;
      disconnect_event_t*    WskDisconnectEvent;
      send_backlog_event_t*  WskSendBacklogEvent;
    };

    struct registration
    {
      uint64_t   ReservedRegistrationState;
      void*      ReservedRegistrationContext;
      kspin_lock ReservedRegistrationLock;
    };

    struct provider_characteristics
    {
      uint16_t  HighestVersion;
      uint16_t  LowestVersion;
    };

    struct provider_dispatch
    {
      uint16_t          Version;
      uint16_t          Reserved;
      PSocket           WskSocket;
      PSocketConnect    WskSocketConnect;
      PControlClient    WskControlClient;
      // win7+
      PGetAddressInfo   WskGetAddressInfo;
      PFreeAddressInfo  WskFreeAddressInfo;
      PGetNameInfo      WskGetNameInfo;    
    };

    struct provider_npi
    {
      client*                  Client;
      const provider_dispatch *Dispatch;    
    };
    /** @} */

    /**\defgroup wsk_imports wsk functions
     @{*/
    NTL_EXTERNAPI 
    ntstatus __stdcall
      WskRegister(
      __in const client_npi* WskClientNpi,
      __out registration& WskRegistration
      );

    NTL_EXTERNAPI
    void __stdcall
      WskDeregister(
      __in registration& WskRegistration
      );

    NTL_EXTERNAPI
    ntstatus __stdcall
      WskQueryProviderCharacteristics(
      __in registration& WskRegistration,
      __out provider_characteristics* WskProviderCharacteristics
      );

    NTL_EXTERNAPI
    ntstatus __stdcall
      WskCaptureProviderNPI(
      __in registration& WskRegistration,
      __in uint32_t WaitTimeout,
      __out provider_npi* WskProviderNpi
      );

    NTL_EXTERNAPI
    void __stdcall
      WskReleaseProviderNPI(
      __in registration& WskRegistration
      );
    /** @} */

    /** @} ntl_km_wsk */
  } // wsk
  /** @} ntl_km */
 } // km
} // ntl
#endif // NTL__KM_WINSOCK
