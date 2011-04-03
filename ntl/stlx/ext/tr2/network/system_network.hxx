#pragma once

namespace ntl {
  
  /// NTL Network subsystem
  namespace network {

  namespace constants
  {
    static const int so_maxconn = 0x7fffffff; // numeric_limits<int>

    enum family_type {
      af_unspec,
      af_unix,
      af_inet,
      af_implink,
      af_pup,
      af_chaos,
      af_ns,
      af_ipx          = af_ns,
      af_iso,
      af_osi          = af_iso,
      af_ecma,
      af_datakit,
      af_ccitt,
      af_sna,
      af_decnet,
      af_dli,
      af_lat,
      af_hylink,
      af_appletalk,
      af_netbios,
      af_voiceview,
      af_firefox,
      af_unknown1,
      af_ban,
      af_atm,
      af_inet6,
      af_cluster,
      af_12844,
      af_irda,
      af_netdes,
      af_tcnprocess,
      af_tcnmessage,
      af_iclfxbm,
      af_bth,
      af_max
    };

    enum socket_type {
      sock_stream = 1,
      sock_dgram,
      sock_raw,
      sock_rdm,
      sock_seqpacket
    };

    enum protocol_type {
      ipproto_hopopts       = 0,
      ipproto_icmp          = 1,
      ipproto_igmp          = 2,
      ipproto_ggp           = 3,
      ipproto_ipv4          = 4,
      ipproto_st            = 5,
      ipproto_tcp           = 6,
      ipproto_cbt           = 7,
      ipproto_egp           = 8,
      ipproto_igp           = 9,
      ipproto_pup           = 12,
      ipproto_udp           = 17,
      ipproto_idp           = 22,
      ipproto_rdp           = 27,
      ipproto_ipv6          = 41,
      ipproto_routing       = 43,
      ipproto_fragment      = 44,
      ipproto_esp           = 50,
      ipproto_ah            = 51,
      ipproto_icmpv6        = 58,
      ipproto_none          = 59,
      ipproto_dstopts       = 60,
      ipproto_nd            = 77,
      ipproto_iclfxbm       = 78,
      ipproto_pim           = 103,
      ipproto_pgm           = 113,
      ipproto_l2tp          = 115,
      ipproto_sctp          = 132,
      ipproto_raw           = 255,
      ipproto_max           = 256,

      ipproto_reserved_raw  = 257,
      ipproto_reserved_ipsec  = 258,
      ipproto_reserved_ipsecoffload  = 259,
      ipproto_reserved_max  = 260
    };

    enum socket_level_type {
      sol_socket = 0xffff,
      sol_tcp = ipproto_tcp,
      sol_ipx = 1000,
      sol_ip  = ipproto_hopopts,
      sol_ipv6= ipproto_ipv6
    };

    enum socket_option_name {
      so_debug        = 0x0001,
      so_acceptconn   = 0x0002,
      so_reuseaddr    = 0x0004,
      so_keepalive    = 0x0008,
      so_dontroute    = 0x0010,
      so_broadcast    = 0x0020,
      so_useloopback  = 0x0040,
      so_linger       = 0x0080,
      so_oobinline    = 0x0100,

      so_dontlinger   = (int)(~so_linger),
      so_exclusiveaddruse = ((int)(~so_reuseaddr)),

      so_sndbuf       = 0x1001,
      so_rcvbuf       = 0x1002,
      so_sndlowat     = 0x1003,
      so_rcvlowat     = 0x1004,
      so_sndtimeo     = 0x1005,
      so_rcvtimeo     = 0x1006,
      so_error        = 0x1007,
      so_type         = 0x1008,

      so_group_id       = 0x2001,
      so_group_priority = 0x2002,
      so_max_msg_size   = 0x2003,
      so_protocol_infoa = 0x2004,
      so_protocol_infow = 0x2005,
      pvd_config        = 0x3001,
      so_conditional_accept = 0x3002,

      tcp_nodelay     = 0x0001,

      ip_options               =  1,
      ip_hdrincl               =  2,
      ip_tos                   =  3,
      ip_ttl                   =  4,
      ip_multicast_if          =  9,
      ip_multicast_ttl         = 10,
      ip_multicast_loop        = 11,
      ip_add_membership        = 12,
      ip_drop_membership       = 13,
      ip_dontfragment          = 14,
      ip_add_source_membership = 15,
      ip_drop_source_membership= 16,
      ip_block_source          = 17,
      ip_unblock_source        = 18,
      ip_pktinfo               = 19,
      ip_hoplimit              = 21,
      ip_receive_broadcast     = 22,
      ip_recvif                = 24,
      ip_recvdstaddr           = 25,
      ip_iflist                = 28,
      ip_add_iflist            = 29,
      ip_del_iflist            = 30,
      ip_unicast_if            = 31,
      ip_rthdr                 = 32,
      ip_recvrthdr             = 38,

      ipv6_hopopts         =  1,
      ipv6_hdrincl         =  2,
      ipv6_unicast_hops    =  4,
      ipv6_multicast_if    =  9,
      ipv6_multicast_hops  = 10,
      ipv6_multicast_loop  = 11,
      ipv6_add_membership  = 12,
      ipv6_join_group      = ipv6_add_membership,
      ipv6_drop_membership = 13,
      ipv6_leave_group     = ipv6_drop_membership,
      ipv6_dontfrag        = 14,
      ipv6_pktinfo         = 19,
      ipv6_hoplimit        = 21,
      ipv6_protection_level= 23,
      ipv6_recvif          = 24,
      ipv6_recvdstaddr     = 25,
      ipv6_checksum        = 26,
      ipv6_v6only          = 27,
      ipv6_iflist          = 28,
      ipv6_add_iflist      = 29,
      ipv6_del_iflist      = 30,
      ipv6_unicast_if      = 31,
      ipv6_rthdr           = 32,
      ipv6_recvrthdr       = 38,
    };

    enum shutdown_type {
      shutdown_receive,
      shutdown_send,
      shutdown_both
    };

    enum message_flags {
      message_default = 0,
      message_out_of_band   = 0x01,
      message_peek          = 0x02,
      message_do_not_route  = 0x04,
      message_wait_all      = 0x08,
    };
    __ntl_bitmask_type(message_flags, );

  } // constants


#pragma warning(push, 4)
#pragma warning(disable: 4201)

  struct in_addr
  {
    union 
    {
      uint8_t   bytes[4];
      uint16_t  words[2];
      struct  { uint8_t b1, b2, b3, b4; };
      struct  { uint16_t w1, w2; };
      uint32_t  addr;
    };
  };

  struct in_addr6
  {
    union
    {
      uint8_t   bytes[16];
      uint16_t  words[8];
    };
  };
  typedef in_addr6 in6_addr;

  struct sockaddr_in_base
  {
    int16_t   family;
    uint16_t  port;
  };
  typedef sockaddr_in_base sockaddr;

  struct sockaddr_in: sockaddr_in_base
  {
    in_addr   addr;
    char      reserved[8];
  };

  struct sockaddr_in6: sockaddr_in_base
  {
    uint32_t  flowinfo;
    in6_addr  addr;
    uint32_t  scope_id;
  };

  struct ip_mreq
  {
    in_addr multiaddr;
    in_addr interface;
  };

  struct ipv6_mreq
  {
    in6_addr multiaddr;
    uint32_t interface;
  };


  // RFC 2553: sockaddr_storage
#define sa_family_t int16_t
#define _SS_MAXSIZE    32
#define _SS_ALIGNSIZE  (sizeof (int64_t))
#define _SS_PAD1SIZE   (_SS_ALIGNSIZE - sizeof (sa_family_t))
#define _SS_PAD2SIZE   (_SS_MAXSIZE - (sizeof (sa_family_t)+ _SS_PAD1SIZE + _SS_ALIGNSIZE))
  struct sockaddr_storage {
      sa_family_t  __ss_family;
      char      __ss_pad1[_SS_PAD1SIZE];
      int64_t   __ss_align;
      char      __ss_pad2[_SS_PAD2SIZE];
  };
#undef sa_family_t
#undef _SS_MAXSIZE
#undef _SS_ALIGNSIZE
#undef _SS_PAD1SIZE
#undef _SS_PAD2SIZE

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
    enum flags_type {
      ai_passive      = 0x0001,
      ai_canonname    = 0x0002,
      ai_numerichost  = 0x0004,
      ai_numericserv  = 0x0008,

      ai_all          = 0x0100,
      ai_addrconfig   = 0x0400,
      ai_v4mapped     = 0x0800,
    };
    int32_t flags;
    int32_t family;
    int32_t socktype;
    int32_t protocol;
    size_t  addrlen;
    char*   canonname;
    sockaddr* addr;
    addrinfo* next;
  };
#pragma warning(pop)

}}
