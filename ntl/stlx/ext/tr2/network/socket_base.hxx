/**\file*********************************************************************
 *                                                                     \brief
 *  Socket base declarations
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_SOCKET_BASE
#define NTL__STLX_TR2_NETWORK_SOCKET_BASE
#pragma once

#include "system_network.hxx"

namespace std { namespace tr2 { namespace network {

  namespace __ { namespace network
  {
    template<typename T, int L, int N, int L6 = L, int N6 = N>
    class integral_socket_option
    {
    public:
      integral_socket_option()
        :v()
      {}
      explicit integral_socket_option(T v)
        :v(v)
      {}

      integral_socket_option& operator=(T v)
      {
        return this->v = v, *this;
      }

      // get
      T value() const { return v; }

      // extensible
      template<class Protocol> int level(const Protocol& p) const             { return p.family() == ntl::network::constants::af_inet6 ? level6 : level4; }
      template<class Protocol> int name(const Protocol& p) const              { return p.family() == ntl::network::constants::af_inet6 ? name6 : name4; }
      template<class Protocol> T* data(const Protocol&)                       { return &v; }
      template<class Protocol> const T* data(const Protocol&) const           { return &v; }
      template<class Protocol> size_t size(const Protocol&) const             { return sizeof(T); }
      template<class Protocol> void resize(const Protocol&, size_t s) __ntl_throws(std::length_error)
      {
        if(s != sizeof(T))
          __ntl_throw(std::length_error("invalid integral socket option size"));
      }

    public:
      // for service use
      typedef T   storage_type;
      enum {
        level4  = L,
        level6  = L6,
        name4   = N,
        name6   = N6,
      };

    protected:
      T v;
    };

    template<int L, int N, int L6 = L, int N6 = N>
    class boolean_socket_option:
      public integral_socket_option<uint32_t, L, N, L6, N6>
    {
    public:
      boolean_socket_option()
      {}

      explicit boolean_socket_option(bool v)
        :integral_socket_option(v)
      {}

      // get
      bool value() const { return v != 0; }
      operator bool() const { return v != 0; }
      bool operator!() const{ return !v; }

      // extensible
      template<class Protocol> size_t size(const Protocol&) const             { return sizeof(int); }
      template<class Protocol> void resize(const Protocol&, size_t s) __ntl_throws(std::length_error)
      {
        if(!(s == sizeof(bool) || s == sizeof(int)))
          __ntl_throw(std::length_error("invalid boolean socket option size"));
      }
    };
  }} // __


  /**
   *	@brief 5.7.2. Class socket_base
   **/
  class socket_base
  {
  public:
    ///\name socket options
    class linger;
#ifdef NTL_DOC
    /** Determines whether a socket permits sending of broadcast messages, if supported by the protocol. */
    class broadcast;
    /** Determines whether debugging information is recorded by the underlying protocol. */
    class debug;
    /** Determines whether outgoing messages bypass standard routing facilities. */
    class do_not_route;
    /** Determines whether a socket permits sending of keep_alive messages, if supported by the protocol.  */
    class keep_alive;
    /** Determines whether out-of-band data (also known as urgent data) is received inline.  */
    class out_of_band_inline;
    /** Specifies the size of the receive buffer associated with a socket.  */
    class receive_buffer_size;
    /** Specifies the minimum number of bytes to process for socket input operations. */
    class receive_low_watermark;
    /** Determines whether the validation of endpoints used for binding a socket should allow the reuse of local endpoints, 
        if supported by the protocol.  */
    class reuse_address;
    /** Specifies the size of the send buffer associated with a socket.  */
    class send_buffer_size;
    /** Specifies the minimum number of bytes to process for socket output operations.  */
    class send_low_watermark;

    ///\name socket control commands
    /** Determine amount of data that can be read without blocking. */
    class bytes_readable;
    /** Specifies if nonblocking mode should be enabled. */
    class nonblocking_io;
    /** Determine whether or not all OOB data has been read. */
    class at_oob_mark;
    ///\}

    /** Different ways a socket may be shutdown. */
    enum shutdown_type {
      /// Shutdown the receive side of the socket.
      shutdown_receive,
      /// Shutdown the send side of the socket.
      shutdown_send,
      /// Shutdown both send and receive on the socket.
      shutdown_both
    };

    /** Bitmask type for flags that can be passed to send and receive operations. */
    enum message_flags {
      /** Default flag. */
      message_default,
      /** Peek at incoming data without removing it from the input queue. */
      message_peek,
      /** Process out-of-band data. */
      message_out_of_band,
      /** Specify that the data should not be subject to routing. */
      message_do_not_route
    };
#else
    // socket options
    typedef __::network::boolean_socket_option<ntl::network::constants::sol_socket, ntl::network::constants::so_broadcast> broadcast;
    typedef __::network::boolean_socket_option<ntl::network::constants::sol_socket, ntl::network::constants::so_debug>     debug;
    typedef __::network::boolean_socket_option<ntl::network::constants::sol_socket, ntl::network::constants::so_dontroute> do_not_route;
    typedef __::network::boolean_socket_option<ntl::network::constants::sol_socket, ntl::network::constants::so_keepalive> keep_alive;
    typedef __::network::boolean_socket_option<ntl::network::constants::sol_socket, ntl::network::constants::so_oobinline> out_of_band_inline;
    typedef __::network::boolean_socket_option<ntl::network::constants::sol_socket, ntl::network::constants::so_reuseaddr> reuse_address;

    typedef __::network::integral_socket_option<int, ntl::network::constants::sol_socket, ntl::network::constants::so_rcvbuf>  receive_buffer_size;
    typedef __::network::integral_socket_option<int, ntl::network::constants::sol_socket, ntl::network::constants::so_rcvlowat>receive_low_watermark;
    typedef __::network::integral_socket_option<int, ntl::network::constants::sol_socket, ntl::network::constants::so_sndbuf>  send_buffer_size;
    typedef __::network::integral_socket_option<int, ntl::network::constants::sol_socket, ntl::network::constants::so_sndlowat>send_buffer_low_watermark;

    // socket control codes
    typedef __::network::boolean_socket_option<-1, ntl::network::constants::fionbio>              nonblocking_io;
    typedef __::network::boolean_socket_option<-1, ntl::network::constants::siocatmark>           at_mark;
    typedef __::network::integral_socket_option<uint32_t, -1, ntl::network::constants::fionread>  bytes_readable;

    typedef ntl::network::constants::shutdown_type shutdown_type;
    static const ntl::network::constants::shutdown_type shutdown_receive = ntl::network::constants::shutdown_receive;
    static const ntl::network::constants::shutdown_type shutdown_send = ntl::network::constants::shutdown_send;
    static const ntl::network::constants::shutdown_type shutdown_both = ntl::network::constants::shutdown_both;

    typedef ntl::network::constants::message_flags message_flags;
    static const ntl::network::constants::message_flags message_default = ntl::network::constants::message_default;
    static const ntl::network::constants::message_flags message_peek = ntl::network::constants::message_peek;
    static const ntl::network::constants::message_flags message_out_of_band = ntl::network::constants::message_out_of_band;
    static const ntl::network::constants::message_flags message_do_not_route = ntl::network::constants::message_do_not_route;
#endif

    /** Waitable socket checks for */
    enum wait_type {
      wait_none,

      /** readability */
      read    = 0x01,
      /** writability */
      write   = 0x02,
      /** errors */
      except  = 0x04,
      /** errors */
      error   = except,
    };
    __ntl_bitmask_type(wait_type, friend inline);


    /** The maximum length of the queue of pending incoming connections. */
    static const int max_connections = std::numeric_limits<int>::__max;

  protected:
    /** Protected destructor to prevent deletion through this type. */
    ~socket_base() // __default;
    {}
  };



  /** The linger class represents a socket option for controlling the behaviour when a socket is closed and unsent data is present.  */
  class socket_base::linger
  {
  public:
    linger()
    {}
    linger(bool e, int t)
      :v(e, static_cast<uint16_t>(t))
    {}

    bool enabled() const { return v.first != 0; }
    void enabled(bool e) { v.first = e != 0; }

    int timeout() const { return v.second; }
    void timeout(int t) { v.second = static_cast<uint16_t>(t); }

    template<class Protocol> int level(const Protocol&) const         { return ntl::network::constants::sol_socket; }
    template<class Protocol> int name(const Protocol&) const          { return ntl::network::constants::so_linger;  }
    template<class Protocol> uint16_t* data(const Protocol&)          { return &v.first;     }
    template<class Protocol> const uint16_t* data(const Protocol&) const { return &v.first;  }
    template<class Protocol> size_t size(const Protocol&) const       { return sizeof(v);    }
    template<class Protocol> void resize(const Protocol&, size_t s)
    {
      if(s != sizeof(v))
        __ntl_throw(std::length_error("invalid linger socket option size"));
    }
  protected:
    std::pair<uint16_t, uint16_t> v;
  };

#ifndef NTL_DOC
  namespace ip
  {
    /**
     *	@brief 5.9.16. Class ip::v6_only
     **/
    typedef __::network::boolean_socket_option<ntl::network::constants::sol_ipv6, ntl::network::constants::ipv6_v6only> v6_only;

    namespace unicast
    {
      /**
       *	@brief 5.9.17. Class ip::unicast::hops
       **/
      typedef __::network::integral_socket_option<int,
        ntl::network::constants::sol_ip, ntl::network::constants::ip_ttl,
        ntl::network::constants::sol_ipv6, ntl::network::constants::ipv6_unicast_hops> hops;
    }

    namespace multicast
    {
      /**
       *	@brief 5.9.20. Class ip::multicast::hops
       **/
      typedef __::network::integral_socket_option<int,
        ntl::network::constants::sol_ip, ntl::network::constants::ip_multicast_ttl,
        ntl::network::constants::sol_ipv6, ntl::network::constants::ipv6_multicast_hops> hops;

      /**
       *	@brief 5.9.21. Class ip::multicast::enable_loopback
       **/
      typedef __::network::integral_socket_option<int,
        ntl::network::constants::sol_ip, ntl::network::constants::ip_multicast_loop,
        ntl::network::constants::sol_ipv6, ntl::network::constants::ipv6_multicast_loop> enable_loopback;
    }

  } // ip
#endif

}}}
#endif // NTL__STLX_TR2_NETWORK_SOCKET_BASE
