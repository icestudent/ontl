/**\file*********************************************************************
 *                                                                     \brief
 *  Stream socket
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_STREAMSOCKET
#define NTL__STLX_TR2_NETWORK_STREAMSOCKET
#pragma once

#include "basic_socket.hxx"

namespace std { namespace tr2 { namespace network {

  /**
   *	@brief 5.7.10. Class template basic_stream_socket
   **/
  template<class Protocol, class StreamSocketService>
  class basic_stream_socket:
    public basic_socket<Protocol, StreamSocketService>
  {
  public:
    ///\name types:
    typedef Protocol protocol_type;
    typedef typename Protocol::endpoint endpoint_type;
    typedef typename StreamSocketService::native_type native_type;

    ///\name constructors:
    explicit basic_stream_socket(tr2::sys::io_service& ios)
      :basic_socket(ios)
    {}
    basic_stream_socket(tr2::sys::io_service& ios, const protocol_type& protocol)
      :basic_socket(ios, protocol)
    {}
    basic_stream_socket(tr2::sys::io_service& ios, const endpoint_type& endpoint)
      :basic_socket(ios, endpoint)
    {}
    basic_stream_socket(tr2::sys::io_service& ios, const protocol_type& protocol, const native_type& native_socket)
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
    template<class MutableBufferSequence, class ReadHandler>
    void async_receive(const MutableBufferSequence& buffers, ReadHandler handler)
    {
      service.async_receive(impl, buffers, socket_base::message_default, handler);
    }
    template<class MutableBufferSequence, class ReadHandler>
    void async_receive(const MutableBufferSequence& buffers, socket_base::message_flags flags, ReadHandler handler)
    {
      service.async_receive(impl, buffers, flags, handler);
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
    template<class ConstBufferSequence, class WriteHandler>
    void async_send(const ConstBufferSequence& buffers, WriteHandler handler)
    {
      service.async_send(impl, buffers, socket_base::message_default, handler);
    }
    template<class ConstBufferSequence, class WriteHandler>
    void async_send(const ConstBufferSequence& buffers, socket_base::message_flags flags, WriteHandler handler)
    {
      service.async_send(impl, buffers, flags, handler);
    }
    template<class MutableBufferSequence>
    size_t read_some(const MutableBufferSequence& buffers, error_code& ec = throws())
    {
      error_code e;
      size_t re = service.receive(impl, buffers, socket_base::message_default, e);
      throw_system_error(e, ec);
      return re;
    }
    template<class ConstBufferSequence>
    size_t write_some(const ConstBufferSequence& buffers, error_code& ec = throws())
    {
      error_code e;
      size_t re = service.send(impl, buffers, socket_base::message_default, e);
      throw_system_error(e, ec);
      return re;
    }
    template<class MutableBufferSequence, class ReadHandler>
    void async_read_some(const MutableBufferSequence& buffers, ReadHandler handler)
    {
      service.async_receive(impl, buffers, socket_base::message_default, handler);
    }
    template<class ConstBufferSequence, class WriteHandler>
    void async_write_some(const ConstBufferSequence& buffers, WriteHandler handler)
    {
      service.async_send(impl, buffers, socket_base::message_default, handler);
    }
    ///\}
  };

}}}
#endif // NTL__STLX_TR2_NETWORK_STREAMSOCKET
