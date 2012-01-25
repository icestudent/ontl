/**\file*********************************************************************
 *                                                                     \brief
 *  Basic socket acceptor
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_BASICSOCKETACCEPTOR
#define NTL__STLX_TR2_NETWORK_BASICSOCKETACCEPTOR
#pragma once

#include "basic_socket.hxx"
#include "socket_acceptor_service.hxx"

namespace std { namespace tr2 { namespace network {

  /**
   *	@brief 5.7.12. Class template basic_socket_acceptor
   **/
   template<class Protocol, class SocketAcceptorService>
  class basic_socket_acceptor:
    public tr2::sys::basic_io_object<SocketAcceptorService>,
    public socket_base
  {
  public:
    ///\name types:
    typedef typename SocketAcceptorService::native_type native_type;
    typedef Protocol protocol_type;
    typedef typename Protocol::endpoint endpoint_type;

    ///\name constructors:
    explicit basic_socket_acceptor(tr2::sys::io_service& ios)
      :basic_io_object(ios)
    {}
    basic_socket_acceptor(tr2::sys::io_service& ios, const protocol_type& protocol)
      :basic_io_object(ios)
    {
      error_code ec;
      service.open(impl, protocol, ec);
      throw_system_error(ec);
    }
    basic_socket_acceptor(tr2::sys::io_service& ios, const endpoint_type& endpoint, bool reuse_addr = true)
      :basic_io_object(ios)
    {
      error_code ec;
      service.open(impl, endpoint.protocol(), ec);
      throw_system_error(ec);
      if(ec) return;
      if(reuse_addr){
        service.set_option(impl, reuse_address(true), ec);
        throw_system_error(ec);
        if(ec) return;
      }
      service.bind(impl, endpoint, ec);
      throw_system_error(ec);
      if(ec) return;
      service.listen(impl, max_connections, ec);
      throw_system_error(ec);
    }

    basic_socket_acceptor(tr2::sys::io_service& ios, const protocol_type& protocol, const native_type& native_acceptor)
      :basic_io_object(ios)
    {
      service.assign(impl, protocol, native_acceptor, ec);
      throw_system_error(ec);
    }

    ///\name members:
    native_type native() { return service.native(); }

    error_code open(const protocol_type& protocol = protocol_type(), error_code& ec = throws())
    {
      error_code e;
      service.open(impl, protocol, e);
      return throw_system_error(e, ec);
    }

    error_code assign(const protocol_type& protocol, const native_type& native_acceptor, error_code& ec = throws())
    {
      error_code e;
      service.assign(impl, protocol, native_acceptor, e);
      return throw_system_error(e, ec);
    }

    bool is_open() const { return service.is_open(impl); }

    error_code close(error_code& ec = throws())
    {
      error_code e;
      service.close(impl, e);
      return throw_system_error(e, ec);
    }

    error_code cancel(error_code& ec = throws())
    {
      error_code e;
      service.cancel(impl, e);
      return throw_system_error(e, ec);
    }

    template<class SettableSocketOption>
    error_code set_option(const SettableSocketOption& option, error_code& ec = throws())
    {
      error_code e;
      service.set_option(impl, option, e);
      return throw_system_error(e, ec);
    }

    template<class GettableSocketOption>
    error_code get_option(GettableSocketOption& option, error_code& ec = throws()) const
    {
      error_code e;
      service.get_option(impl, option, e);
      return throw_system_error(e, ec);
    }

    template<class IoControlCommand>
    error_code io_control(IoControlCommand& command, error_code& ec = throws())
    {
      error_code e;
      service.io_control(impl, command, e);
      return throw_system_error(e, ec);
    }

    error_code bind(const endpoint_type& endpoint, error_code& ec = throws())
    {
      error_code e;
      service.bind(impl, endpoint, e);
      return throw_system_error(e, ec);
    }

    void listen(int backlog = max_connections, error_code& ec = throws())
    {
      error_code e;
      service.listen(impl, backlog, e);
      return throw_system_error(e, ec);
    }

    endpoint_type local_endpoint(error_code& ec = throws()) const
    {
      error_code e;
      endpoint_type re = service.local_endpoint(impl, e);
      throw_system_error(e, ec);
      return re;
    }

    template<class SocketService>
    error_code accept(basic_socket<Protocol, SocketService>& socket, error_code& ec = throws())
    {
      error_code e;
      service.accept(impl, socket, nullptr, e);
      return throw_system_error(e, ec);
    }
    template<class SocketService>
    error_code accept(basic_socket<Protocol, SocketService>& socket, endpoint_type& endpoint, error_code& ec)
    {
      error_code e;
      service.accept(socket, socket, &endpoint, e);
      return throw_system_error(e, ec);
    }
    template<class SocketService, class AcceptHandler>
    void async_accept(basic_socket<Protocol, SocketService>& socket, AcceptHandler handler)
    {
      service.async_accept(socket, nullptr, handler);
    }
    template<class SocketService, class AcceptHandler>
    void async_accept(basic_socket<Protocol, SocketService>& socket, endpoint_type& endpoint, AcceptHandler handler)
    {
      service.async_accept(socket, &endpoint, handler);
    }
  };

}}}
#endif // NTL__STLX_TR2_NETWORK_BASICSOCKETACCEPTOR
