/**\file*********************************************************************
 *                                                                     \brief
 *  Basic socket
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_BASICSOCKET
#define NTL__STLX_TR2_NETWORK_BASICSOCKET
#pragma once

#include "io_service.hxx"
#include "socket_base.hxx"
#include "basic_io_object.hxx"
#include "../../../chrono.hxx"
//#include "../../../unordered_map.hxx"

namespace std { namespace tr2 { namespace network {

  /**
   *	@brief 5.7.6. Class template basic_socket
   **/
  template<class Protocol, class SocketService>
  class basic_socket:
    public tr2::sys::basic_io_object<SocketService>,
    public socket_base
  {
  public:
    ///\name types:
    typedef typename SocketService::native_type native_type;
    typedef Protocol protocol_type;
    typedef typename Protocol::endpoint endpoint_type;

    ///\name members:
    native_type native() { return service.native(impl); }

    error_code open(const protocol_type& protocol = protocol_type(), error_code& ec = throws())
    {
      error_code e;
      service.open(impl, protocol, e);
      return throw_system_error(e, ec);
    }

    error_code assign(const protocol_type& protocol, const native_type& native_socket, error_code& ec = throws())
    {
      error_code e;
      service.assign(impl, protocol, native_socket, e);
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

    bool at_mark(error_code& ec = throws()) const
    {
      error_code e;
      bool re = service.at_mark(impl, e);
      throw_system_error(e, ec);
      return re;
    }

    size_t available(error_code& ec = throws()) const
    {
      error_code e;
      size_t re = service.available(impl, e);
      throw_system_error(e, ec);
      return re;
    }

    error_code bind(const endpoint_type& endpoint, error_code& ec = throws())
    {
      error_code e;
      service.bind(impl, endpoint, e);
      return throw_system_error(e, ec);
    }

    error_code shutdown(shutdown_type what, error_code& ec = throws())
    {
      error_code e;
      service.shutdown(impl, what, e);
      return throw_system_error(e, ec);
    }

    endpoint_type local_endpoint(error_code& ec = throws()) const
    {
      error_code e;
      endpoint_type re = service.local_endpoint(impl, e);
      throw_system_error(e, ec);
      return re;
    }

    endpoint_type remote_endpoint(error_code& ec = throws()) const
    {
      error_code e;
      endpoint_type re = service.remote_endpoint(impl, e);
      throw_system_error(e, ec);
      return re;
    }

    error_code connect(const endpoint_type& endpoint, error_code& ec = throws())
    {
      error_code e;
      if(is_open() == false){
        service.open(impl, endpoint.protocol(), e);
        if(e)
          return throw_system_error(e, ec);
      }
      service.connect(impl, endpoint, e);
      return throw_system_error(e, ec);
    }

    template<class ConnectHandler>
    void async_connect(const endpoint_type& endpoint, ConnectHandler handler)
    {
      if(is_open() == false){
        error_code e;
        service.open(impl, endpoint.protocol(), e);
        if(e){
          service.get_io_service().post(__::network::bind_handler(handler, e));
          return;
        }
      }
      service.async_connect(impl, endpoint, handler);
    }

    bool wait(socket_base::wait_type check, std::error_code& ec)
    {
      error_code e;
      bool re = service.wait(impl, check, e);
      throw_system_error(e, ec);
      return re;
    }

    template <class Rep, class Period>
    bool wait_for(socket_base::wait_type check, const std::chrono::duration<Rep, Period>& rel_time, std::error_code& ec)
    {
      error_code e;
      bool re = service.wait_for(impl, check, rel_time, e);
      throw_system_error(e, ec);
      return re;
    }


  protected:
    ///\name constructors:
    explicit basic_socket(tr2::sys::io_service& ios)
      :basic_io_object(ios)
    {}

    basic_socket(tr2::sys::io_service& ios, const protocol_type& protocol)
      :basic_io_object(ios)
    {
      error_code ec;
      service.open(impl, protocol, ec);
      throw_system_error(ec);
    }

    basic_socket(tr2::sys::io_service& ios, const endpoint_type& endpoint)
      :basic_io_object(ios)
    {
      error_code ec;
      service.open(impl, endpoint.protocol(), ec);
      throw_system_error(ec);
      service.bind(impl, endpoint, ec);
      throw_system_error(ec);
    }
    basic_socket(tr2::sys::io_service& ios, const protocol_type& protocol, const native_type& native_socket)
      :basic_io_object(ios)
    {
      error_code ec;
      service.assign(impl, protocol, native_socket, ec);
      throw_system_error(ec);
    }
    ~basic_socket()
    {}
    ///\}
  };



  ///\name Connection list support
  template<class Protocol, class SocketService, class EndpointIterator>
  inline EndpointIterator connect(basic_socket<Protocol, SocketService>& sock, EndpointIterator begin, EndpointIterator end, error_code& ec = throws())
  {
    error_code e = make_error_code(error::not_found);
    for(;e && begin != end; ++begin){
      sock.close(e);
      sock.connect(*begin, e);
    }
    throw_system_error(e, ec);
    return begin;
  }
  template<class Protocol, class SocketService, class EndpointIterator, class ConnectionCondition>
  inline EndpointIterator connect(basic_socket<Protocol, SocketService>& sock, EndpointIterator begin, EndpointIterator end, ConnectionCondition predicate, error_code& ec = throws())
  {
    error_code e;
    for(;begin != end; ++begin){
      begin = predicate(e, begin);
      if(begin == end)
        break;
      sock.close(e);
      sock.connect(*begin, e);
      if(!e)
        break;
    }
    throw_system_error(e, ec);
    return begin;
  }

  template<class Protocol, class SocketService, class EndpointIterator>
  inline EndpointIterator connect(basic_socket<Protocol, SocketService>& sock, EndpointIterator begin, error_code& ec = throws())
  {
    EndpointIterator end;
    return connect<Protocol, SocketService, EndpointIterator>(sock, begin, end, ec);
  }

  template<class Protocol, class SocketService, class EndpointIterator, class ConnectionCondition>
  inline EndpointIterator connect(basic_socket<Protocol, SocketService>& sock, EndpointIterator begin, ConnectionCondition predicate, error_code& ec = throws())
  {
    EndpointIterator end;
    return connect<Protocol, SocketService, EndpointIterator>(sock, begin, end, predicate, ec);
  }
  ///\}

#if 0
  /** Waitable sockets check */
  template<class Rep, class Period>
  inline size_t wait_for(sys::io_service& ios, std::unordered_map<typename Socket::native_type, socket_base::wait_type>& sockets, 
    const std::chrono::duration<Rep, Period>& rel_time, error_code& ec = throws())
  {
    typename Socket::service_type svc(ios);
    typename Socket::implementation_type impl;
    svc.construct(impl);

    using namespace ntl::network::winsock;
    typedef std::unordered_map<typename Socket::native_type, socket_base::wait_type> map;

    // map socket set to fd_sets
    fd_set r,w,e;
    for(map::iterator i = sockets.begin(), end = sockets.end(); i != end; ++i){
      if(i->second & socket_base::read)
        r.set(i->first);
      if(i->second & socket_base::write)
        w.set(i->first);
      if(i->second & socket_base::error)
        e.set(i->first);
      i->second = socket_base::wait_none;
    }

    // construct time
    timeval time;
    const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(rel_time);
    time.sec  = sec.count();
    time.usec = std::chrono::duration_cast<std::chrono::seconds>(rel_time - sec).count();

    std::error_code err;
    int re = 123;
    if(re == -1){
      const sockerror::type errc = sockerror::get();
      err = std::make_error_code(static_cast<std::tr2::network::error::error_type>(errc));
    }else{
      // map back a fd_sets to the socket set 
      for(uint32_t i = 0; i < r.count; i++)
        sockets[ r.array[i] ] |= socket_base::read;
      for(uint32_t i = 0; i < w.count; i++)
        sockets[ w.array[i] ] |= socket_base::write;
      for(uint32_t i = 0; i < e.count; i++)
        sockets[ e.array[i] ] |= socket_base::error;
    }
    svc.destroy(impl);

    throw_system_error(err, ec);
    return static_cast<size_t>(re);
  }
#endif

}}}
#endif // NTL__STLX_TR2_NETWORK_BASICSOCKET
