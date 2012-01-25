/**\file*********************************************************************
 *                                                                     \brief
 *  Socket acceptor service
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_SOCKETACCEPTORSERVICE
#define NTL__STLX_TR2_NETWORK_SOCKETACCEPTORSERVICE
#pragma once

#include "io_service.hxx"
#include "winsock/service_sockets.hxx"

namespace std { namespace tr2 { namespace network {


  /**
   *	@brief 5.7.11. Class template socket_acceptor_service
   **/
  template<class Protocol>
  class socket_acceptor_service:
    public tr2::sys::io_service::service
  {
    typedef ntl::network::winsock::socket_service<Protocol> service_implementation_type;
  public:
    static tr2::sys::io_service::id id;

    ///\name types:
    typedef Protocol protocol_type;
    typedef typename Protocol::endpoint endpoint_type;
    typedef typename service_implementation_type::implementation_type implementation_type;
    typedef typename service_implementation_type::native_type native_type;

    ///\name constructors:
    explicit socket_acceptor_service(tr2::sys::io_service& ios)
      :service(ios), svc(ios)
    {}

    ///\name members:
    void construct(implementation_type& impl){ return svc.construct(impl); }
    void destroy(implementation_type& impl){ return svc.destroy(impl); }

    error_code open(implementation_type& impl, const protocol_type& protocol, error_code& ec){ return svc.open(impl, protocol, ec); }
    error_code assign(implementation_type& impl, const protocol_type& protocol, const native_type& native_socket, error_code& ec){ return svc.assign(impl, protocol, native_socket, ec); }
    bool is_open(const implementation_type& impl) const{ return svc.is_open(impl); }
    error_code close(implementation_type& impl, error_code& ec){ return svc.close(impl, ec); }
    native_type native(implementation_type& impl){ return svc.native(impl); }
    error_code cancel(implementation_type& impl, error_code& ec){ return svc.cancel(impl, ec); }
    template<class SettableSocketOption>
    error_code set_option(implementation_type& impl, const SettableSocketOption& option, error_code& ec){ return svc.set_option(impl, option, ec); }
    template<class GettableSocketOption>
    error_code get_option(const implementation_type& impl, GettableSocketOption& option, error_code& ec) const{ return svc.get_option(impl, option, ec); }
    template<class IoControlCommand>
    error_code io_control(implementation_type& impl, IoControlCommand& command, error_code& ec){ return svc.io_control(impl, command, ec); }
    error_code bind(implementation_type& impl, const endpoint_type& endpoint, error_code& ec){ return svc.bind(impl, endpoint, ec); }
    std::error_code listen(implementation_type& impl, int backlog, std::error_code& ec) { return svc.listen(impl, backlog, ec); }
    endpoint_type local_endpoint(const implementation_type& impl, error_code& ec) const{ return svc.local_endpoint(impl, ec); }

    template<class SocketService>
    error_code accept(implementation_type& impl, basic_socket<Protocol, SocketService>& socket, endpoint_type* endpoint, error_code& ec)
    {
      implementation_type client;
      std::bzero(&client, sizeof(client));
      svc.accept(impl, client, endpoint, ec);
      if(!ec){
        socket.assign(client.family() == ntl::network::constants::af_inet6 ? protocol_type::v6() : protocol_type::v4(), client.s, ec);
      }
      return ec;
    }

    template<class SocketService, class AcceptHandler>
    void async_accept(implementation_type& impl, basic_socket<Protocol, SocketService>& socket, endpoint_type* endpoint, AcceptHandler handler) { return svc.async_accept(impl, socket, endpoint, handler); }

    ///\}
  private:
    void shutdown_service() { svc.shutdown_service(); }

  private:
    service_implementation_type svc;
  };

}}}
#endif // NTL__STLX_TR2_NETWORK_SOCKETACCEPTORSERVICE
