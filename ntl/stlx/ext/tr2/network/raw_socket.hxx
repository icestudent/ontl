/**\file*********************************************************************
 *                                                                     \brief
 *  Datagram socket
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_RAWSOCKET
#define NTL__STLX_TR2_NETWORK_RAWSOCKET
#pragma once

#include "basic_socket.hxx"

namespace std { namespace tr2 { namespace network {

  /**
   *	@brief 5.7.8. Class template basic_raw_socket
   **/
  template<class Protocol, class RawSocketService>
  class basic_raw_socket:
    public basic_socket<Protocol, RawSocketService>
  {
  public:
    ///\name types:
    typedef typename RawSocketService::native_type native_type;
    typedef Protocol protocol_type;
    typedef typename Protocol::endpoint endpoint_type;

    ///\name constructors:
    explicit basic_raw_socket(tr2::sys::io_service& ios)
      :basic_socket<Protocol, RawSocketService>(ios)
    {}

    basic_raw_socket(tr2::sys::io_service& ios, const protocol_type& protocol)
      :basic_socket(ios, protocol)
    {}

    basic_raw_socket(tr2::sys::io_service& ios, const endpoint_type& endpoint)
      :basic_socket(ios, endpoint)
    {}

    basic_raw_socket(tr2::sys::io_service& ios, const protocol_type& protocol, const native_type& native_socket)
      :basic_socket(ios, protocol, native_socket)
    {}

    ///\name members:
    template<class MutableBufferSequence>
    size_t receive(const MutableBufferSequence& buffers, error_code& ec = throws())
    {
      error_code e;
      size_t re = service.receive(impl, buffers, socket_base::message_default, e);
      throw_system_error(e, ec);
      return re;
    }
    template<class MutableBufferSequence>
    size_t receive(const MutableBufferSequence& buffers, socket_base::message_flags flags, error_code& ec = throws())
    {
      error_code e;
      size_t re = service.receive(impl, buffers, flags, e);
      throw_system_error(e, ec);
      return re;
    }
    template<class MutableBufferSequence, class Handler>
    typename sys::ReadHandler<Handler>::type async_receive(const MutableBufferSequence& buffers, Handler handler)
    {
      sys::ReadHandler<Handler> wrap(handler);
      service.async_receive(impl, buffers, socket_base::message_default, wrap.handler);
      return wrap.result.get();
    }
    template<class MutableBufferSequence, class Handler>
    typename sys::ReadHandler<Handler>::type async_receive(const MutableBufferSequence& buffers, socket_base::message_flags flags, Handler handler)
    {
      sys::ReadHandler<Handler> wrap(handler);
      service.async_receive(impl. buffers, flags, wrap.handler);
      return wrap.result.get();
    }
    template<class MutableBufferSequence>
    size_t receive_from(const MutableBufferSequence& buffers, endpoint_type& sender, error_code& ec = throws())
    {
      error_code e;
      size_t re = service.receive_from(impl, buffers, sender, socket_base::message_default, e);
      throw_system_error(e, ec);
      return re;
    }
    template<class MutableBufferSequence>
    size_t receive_from(const MutableBufferSequence& buffers, endpoint_type& sender, socket_base::message_flags flags, error_code& ec = throws())
    {
      error_code e;
      size_t re = service.receive_from(impl, buffers, sender, flags, e);
      throw_system_error(e, ec);
      return re;
    }
    template<class MutableBufferSequence, class Handler>
    typename sys::ReadHandler<Handler>::type async_receive_from(const MutableBufferSequence& buffers, endpoint_type& sender, Handler handler)
    {
      sys::ReadHandler<Handler> wrap(handler);
      service.async_receive_from(impl, buffers, sender, socket_base::message_default, wrap.handler);
      return wrap.result.get();
    }
    template<class MutableBufferSequence, class Handler>
    typename sys::ReadHandler<Handler>::type async_receive_from(const MutableBufferSequence& buffers, endpoint_type& sender, socket_base::message_flags flags, Handler handler)
    {
      sys::ReadHandler<Handler> wrap(handler);
      service.async_receive_from(impl, buffers, sender, flags, wrap.handler);
      return wrap.result.get();
    }

    template<class ConstBufferSequence>
    size_t send(const ConstBufferSequence& buffers, error_code& ec = throws())
    {
      error_code e;
      size_t re = service.send(impl, buffers, socket_base::message_default, e);
      throw_system_error(e, ec);
      return re;
    }
    template<class ConstBufferSequence>
    size_t send(const ConstBufferSequence& buffers, socket_base::message_flags flags, error_code& ec = throws())
    {
      error_code e;
      size_t re = service.send(impl, buffers, flags, e);
      throw_system_error(e, ec);
      return re;
    }
    template<class ConstBufferSequence, class Handler>
    typename sys::WriteHandler<Handler>::type async_send(const ConstBufferSequence& buffers, Handler handler)
    {
      sys::WriteHandler<Handler> wrap(handler);
      service.async_send(impl, buffers, socket_base::message_default, wrap.handler);
      return wrap.result.get();
    }
    template<class ConstBufferSequence, class Handler>
    typename sys::WriteHandler<Handler>::type async_send(const ConstBufferSequence& buffers, socket_base::message_flags flags, Handler handler)
    {
      sys::WriteHandler<Handler> wrap(handler);
      service.async_send(impl, buffers, flags, handler);
      return wrap.result.get();
    }
    template<class ConstBufferSequence>
    size_t send_to(const ConstBufferSequence& buffers, const endpoint_type& destination, error_code& ec = throws())
    {
      error_code e;
      size_t re = service.send_to(impl, buffers, destination, socket_base::message_default, e);
      throw_system_error(e, ec);
      return re;
    }
    template<class ConstBufferSequence>
    size_t send_to(const ConstBufferSequence& buffers, const endpoint_type& destination, socket_base::message_flags flags, error_code& ec = throws())
    {
      error_code e;
      size_t re = service.send_to(impl, buffers, destination, flags, e);
      throw_system_error(e, ec);
      return re;
    }
    template<class ConstBufferSequence, class Handler>
    typename sys::WriteHandler<Handler>::type async_send_to(const ConstBufferSequence& buffers, const endpoint_type& destination, Handler handler)
    {
      sys::WriteHandler<Handler> wrap(handler);
      service.async_send_to(impl, buffers, destination, socket_base::message_default, wrap.handler);
      return wrap.result.get();
    }
    template<class ConstBufferSequence, class Handler>
    typename sys::WriteHandler<Handler>::type async_send_to(const ConstBufferSequence& buffers, const endpoint_type& destination, socket_base::message_flags flags, Handler handler)
    {
      sys::WriteHandler<Handler> wrap(handler);
      service.async_send_to(impl, buffers, destination, flags, wrap.handler);
      return wrap.result.get();
    }
    ///\}
  };

}}}
#endif // NTL__STLX_TR2_NETWORK_RAWSOCKET
