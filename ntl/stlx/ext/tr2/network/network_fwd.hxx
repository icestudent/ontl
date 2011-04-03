/**\file*********************************************************************
 *                                                                     \brief
 *  Network
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORKFWD
#define NTL__STLX_TR2_NETWORKFWD
#pragma once

#include "../../../iosfwd.hxx"
#include "../../../stdstring.hxx"
#include "../../../system_error.hxx"

namespace std { namespace tr2 {

  /// TR2 Network library
  namespace network
  {

  // Sockets:
  class socket_base;
  template<class Protocol, class SocketService>
  class basic_socket;
  template<class Protocol> class datagram_socket_service;
  template<class Protocol, class DatagramSocketService = datagram_socket_service<Protocol> >
  class basic_datagram_socket;
  template<class Protocol> class stream_socket_service;
  template<class Protocol, class StreamSocketService = stream_socket_service<Protocol> >
  class basic_stream_socket;
  template<class Protocol> class socket_acceptor_service;
  template<class Protocol, class SocketAcceptorService = socket_acceptor_service<Protocol> >
  class basic_socket_acceptor;

  // Socket streams:
  template<class Protocol, class StreamSocketService = stream_socket_service<Protocol> >
  class basic_socket_streambuf;
  template<class Protocol, class StreamSocketService = stream_socket_service<Protocol> >
  class basic_socket_iostream;

  /// Internet protocol
  namespace ip
  {
    class address;
    // address comparisons:
    bool operator==(const address&, const address&);
    bool operator!=(const address&, const address&);
    bool operator< (const address&, const address&);
    bool operator> (const address&, const address&);
    bool operator<=(const address&, const address&);
    bool operator>=(const address&, const address&);
    // address I/O:
    template<class CharT, class Traits>
    basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>&, const address&);

    class address_v4;
    // address_v4 comparisons:
    bool operator==(const address_v4&, const address_v4&);
    bool operator!=(const address_v4&, const address_v4&);
    bool operator< (const address_v4&, const address_v4&);
    bool operator> (const address_v4&, const address_v4&);
    bool operator<=(const address_v4&, const address_v4&);
    bool operator>=(const address_v4&, const address_v4&);
    // address_v4 I/O:
    template<class CharT, class Traits>
    basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>&, const address_v4&);

    class address_v6;
    // address_v6 comparisons:
    bool operator==(const address_v6&, const address_v6&);
    bool operator!=(const address_v6&, const address_v6&);
    bool operator< (const address_v6&, const address_v6&);
    bool operator> (const address_v6&, const address_v6&);
    bool operator<=(const address_v6&, const address_v6&);
    bool operator>=(const address_v6&, const address_v6&);
    // address_v6 I/O:
    template<class CharT, class Traits>
    basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>&, const address_v6&);

    template<class InternetProtocol>
    class basic_endpoint;
    // basic_endpoint comparisons:
    template<class InternetProtocol>
    bool operator==(const basic_endpoint<InternetProtocol>&, const basic_endpoint<InternetProtocol>&);
    template<class InternetProtocol>
    bool operator!=(const basic_endpoint<InternetProtocol>&, const basic_endpoint<InternetProtocol>&);
    template<class InternetProtocol>
    bool operator< (const basic_endpoint<InternetProtocol>&, const basic_endpoint<InternetProtocol>&);
    template<class InternetProtocol>
    bool operator> (const basic_endpoint<InternetProtocol>&, const basic_endpoint<InternetProtocol>&);
    template<class InternetProtocol>
    bool operator<=(const basic_endpoint<InternetProtocol>&, const basic_endpoint<InternetProtocol>&);
    template<class InternetProtocol>
    bool operator>=(const basic_endpoint<InternetProtocol>&, const basic_endpoint<InternetProtocol>&);
    // basic_endpoint I/O:
    template<class CharT, class Traits, class InternetProtocol>
    basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>&, const basic_endpoint<InternetProtocol>&);

    class resolver_query_base;
    template<class InternetProtocol>
    class basic_resolver_query;
    template<class InternetProtocol>
    class basic_resolver_entry;
    template<class InternetProtocol>
    class basic_resolver_iterator;
    template<class InternetProtocol>
    class resolver_service;
    template<class InternetProtocol,
    class ResolverService = resolver_service<InternetProtocol> >
    class basic_resolver;

    string host_name();
    string host_name(error_code&);

    class tcp;
    // tcp comparisons:
    bool operator==(const tcp& a, const tcp& b);
    bool operator!=(const tcp& a, const tcp& b);
    bool operator< (const tcp& a, const tcp& b);
    bool operator> (const tcp& a, const tcp& b);
    bool operator<=(const tcp& a, const tcp& b);
    bool operator>=(const tcp& a, const tcp& b);

    class udp;
    // udp comparisons:
    bool operator==(const udp& a, const udp& b);
    bool operator!=(const udp& a, const udp& b);
    bool operator< (const udp& a, const udp& b);
    bool operator> (const udp& a, const udp& b);
    bool operator<=(const udp& a, const udp& b);
    bool operator>=(const udp& a, const udp& b);

#ifdef NTL__DOC
    class v6_only;

    /// IP Unicast socket options
    namespace unicast
    {
      class hops;
    } // unicast

    /// IP Multicast socket options
    namespace multicast
    {
      class join_group;
      class leave_group;
      class outbound_interface;
      class hops;
      class enable_loopback;
    } // multicast
#else
    namespace unicast
    {}
    namespace multicast
    {}
#endif 
  } // ip
}}}
#endif // NTL__STLX_TR2_NETWORKFWD
