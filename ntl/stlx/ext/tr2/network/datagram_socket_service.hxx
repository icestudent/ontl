/**\file*********************************************************************
 *                                                                     \brief
 *  Datagram socket service
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_DATAGRAMSOCKETSERVICE
#define NTL__STLX_TR2_NETWORK_DATAGRAMSOCKETSERVICE
#pragma once

#include "io_service.hxx"
#include "winsock/service_sockets.hxx"

namespace std { namespace tr2 { namespace network {

  /**
   *	@brief 5.7.7. Class template datagram_socket_service
   *
   *  Default service implementation for a datagram socket.
   **/
  template<class Protocol>
  class datagram_socket_service:
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
    explicit datagram_socket_service(tr2::sys::io_service& ios)
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
    bool at_mark(const implementation_type& impl, error_code& ec) const{ return svc.at_mark(impl, ec); }
    size_t available(const implementation_type& impl, error_code& ec) const{ return svc.available(impl, ec); }
    error_code bind(implementation_type& impl, const endpoint_type& endpoint, error_code& ec){ return svc.bind(impl, endpoint, ec); }
    error_code shutdown(implementation_type& impl, socket_base::shutdown_type how, error_code& ec){ return svc.shutdown(impl, how, ec); }
    endpoint_type local_endpoint(const implementation_type& impl, error_code& ec) const{ return svc.local_endpoint(impl, ec); }
    endpoint_type remote_endpoint(const implementation_type& impl, error_code& ec) const{ return svc.remote_endpoint(impl, ec); }
    error_code connect(implementation_type& impl, const endpoint_type& endpoint, error_code& ec){ return svc.connect(impl, endpoint, ec); }
    template<class ConnectHandler>
    void async_connect(implementation_type& impl, const endpoint_type& endpoint, ConnectHandler handler){ return svc.async_connect(impl, endpoint, handler); }
    template<class MutableBufferSequence>
    size_t receive(implementation_type& impl, const MutableBufferSequence& buffers, socket_base::message_flags flags, error_code& ec){ return svc.receive(impl, buffers, flags, ec); }
    template<class MutableBufferSequence, class ReadHandler>
    void async_receive(implementation_type& impl, const MutableBufferSequence& buffers, socket_base::message_flags flags, ReadHandler handler){ return svc.async_receive(impl, buffers, flags, handler); }
    template<class MutableBufferSequence>
    size_t receive_from(implementation_type& impl, const MutableBufferSequence& buffers, endpoint_type& sender, socket_base::message_flags flags, error_code& ec){ return svc.receive_from(impl, buffers, sender, flags, ec); }
    template<class MutableBufferSequence, class ReadHandler>
    void async_receive_from(implementation_type& impl, const MutableBufferSequence& buffers, endpoint_type& sender, socket_base::message_flags flags, ReadHandler handler){ return svc.async_receive_from(impl, buffers, sender, flags, handler); }
    template<class ConstBufferSequence>
    size_t send(implementation_type& impl, const ConstBufferSequence& buffers, socket_base::message_flags flags, error_code& ec){ return svc.send(impl, buffers, flags, ec); }
    template<class ConstBufferSequence, class WriteHandler>
    void async_send(implementation_type& impl, const ConstBufferSequence& buffers, socket_base::message_flags flags, WriteHandler handler){ return svc.async_send(impl, buffers, flags, handler); }
    template<class ConstBufferSequence>
    size_t send_to(implementation_type& impl, const ConstBufferSequence& buffers, const endpoint_type& destination, socket_base::message_flags flags, error_code& ec){ return svc.send_to(impl, buffers, destination, flags, ec); }
    template<class ConstBufferSequence, class WriteHandler>
    void async_send_to(implementation_type& impl, const ConstBufferSequence& buffers, const endpoint_type& destination, socket_base::message_flags flags, WriteHandler handler){ return svc.async_send_to(impl, buffers, destination, flags, handler); }
    bool wait(const implementation_type& impl, socket_base::wait_type check, std::error_code& ec) { return svc.wait(impl, check, ec); }
    template <class Rep, class Period>
    bool wait_for(const implementation_type& impl, socket_base::wait_type check, const std::chrono::duration<Rep, Period>& rel_time, std::error_code& ec) { return svc.wait_for(impl, check, rel_time, ec); }
    ///\}
  private:
    void shutdown_service() { svc.shutdown_service(); }

  private:
    service_implementation_type svc;
  };

}}}
#endif // NTL__STLX_TR2_NETWORK_DATAGRAMSOCKETSERVICE
