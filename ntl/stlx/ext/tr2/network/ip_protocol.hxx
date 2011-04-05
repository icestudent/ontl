/**\file*********************************************************************
 *                                                                     \brief
 *  Network
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_PROTOCOL
#define NTL__STLX_TR2_NETWORK_PROTOCOL
#pragma once

#include "network_fwd.hxx"
#include "system_network.hxx"
#include "basic_endpoint.hxx"

namespace std { namespace tr2 { namespace network {

  /// Internet protocol.
  namespace ip
  {
    /**
     *	@brief 5.9.13. Class ip::tcp
     **/
    class tcp
    {
    public:
      ///\name types:
      typedef basic_endpoint<tcp> endpoint;
      typedef basic_resolver<tcp> resolver;

      typedef basic_stream_socket<tcp>    socket;
      typedef basic_socket_acceptor<tcp>  acceptor;
      typedef basic_socket_iostream<tcp>  iostream;

      typedef __::network::boolean_socket_option<ntl::network::constants::sol_tcp, ntl::network::constants::tcp_nodelay> no_delay;

      ///\name static members:
      static tcp v4() { return tcp(ntl::network::constants::af_inet); }
      static tcp v6() { return tcp(ntl::network::constants::af_inet6); }

      ///\name extensible tcp
      int family() const  { return af; }
      int type() const    { return ntl::network::constants::sock_stream; }
      int protocol() const{ return ntl::network::constants::ipproto_tcp;}

      ///\name tcp comparisons:
      friend inline bool operator==(const tcp& a, const tcp& b) { return a.af == b.af; }
      friend inline bool operator!=(const tcp& a, const tcp& b) { return a.af != b.af; }
      ///\}
    protected:
      explicit tcp(ntl::network::constants::family_type af)
        :af(af)
      {}
    private:
      ntl::network::constants::family_type af;
    };



    /**
     *	@brief 5.9.15. Class ip::udp
     **/
    class udp
    {
    public:
      ///\name types:
      typedef basic_endpoint<udp>        endpoint;
      typedef basic_resolver<udp>        resolver;
      typedef basic_datagram_socket<udp> socket;

      ///\name static members:
      static udp v4() { return udp(ntl::network::constants::af_inet); }
      static udp v6() { return udp(ntl::network::constants::af_inet6); }

      ///\name extensible udp
      int family() const  { return af; }
      int type() const    { return ntl::network::constants::sock_dgram; }
      int protocol() const{ return ntl::network::constants::ipproto_udp;}

      ///\name udp comparisons:
      friend inline bool operator==(const udp& a, const udp& b) { return a.af == b.af; }
      friend inline bool operator!=(const udp& a, const udp& b) { return a.af != b.af; }
      ///\}
    protected:
      explicit udp(ntl::network::constants::family_type af)
        :af(af)
      {}
    private:
      ntl::network::constants::family_type af;
    };


    namespace multicast
    {
      /**
       *	@brief 5.9.19. Class ip::multicast::outbound_interface
       **/
      class outbound_interface:
        public ntl::network::winsock::sockoptions::outband_interface
      {
      public:
        // constructors:
        outbound_interface()
        {}
        explicit outbound_interface(const address_v4& network_interface)
        {
          address_v4::bytes_type bytes = network_interface.to_bytes();
          for(unsigned i = 0; i < 4; i++)
            v4.bytes[i] = bytes[i];
        }
        explicit outbound_interface(unsigned int network_interface)
        {
          v6 = network_interface;
        }
      };

      /**
       *	@brief 5.9.18. Multicast group management socket options
       **/
      class join_group:
        public ntl::network::winsock::sockoptions::join_group
      {
      public:
        join_group()
        {}
        explicit join_group(const address& multicast_group)
        {
          if(multicast_group.is_v6())
            set6(multicast_group.to_v6(), 0);
          else
            set4(multicast_group.to_v4(), address_v4());
        }
        explicit join_group(const address_v4& multicast_group, const address_v4& network_interface = address_v4::any())
        {
          set4(multicast_group, network_interface);
        }
        explicit join_group(const address_v6& multicast_group, unsigned int network_interface = 0)
        {
          set6(multicast_group, network_interface);
        }
      protected:
        void set4(const address_v4& multicast_group, const address_v4& network_interface)
        {
          address_v4::bytes_type group = multicast_group.to_bytes(),
            face = network_interface.to_bytes();
          std::memcpy(v4.multiaddr.bytes, group.data(), group.size());
          std::memcpy(v4.interface.bytes, face.data(), face.size());
        }
        void set6(const address_v6& multicast_group, unsigned int network_interface = 0)
        {
          address_v6::bytes_type bytes = multicast_group.to_bytes();
          std::memcpy(v6.multiaddr.bytes, bytes.data(), bytes.size());
          v6.interface = network_interface;
        }
      };
    }



  }
}}}
#endif // NTL__STLX_TR2_NETWORK_PROTOCOL
