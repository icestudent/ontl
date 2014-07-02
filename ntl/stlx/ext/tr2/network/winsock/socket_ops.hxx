#pragma once

#include "../iocp/op.hxx"

namespace std { namespace tr2 { namespace sys {

  namespace __
  {

    /** void(const std::error_code& ec, size_t bytes_transferred) */
    template<class WriteHandler, class Buffers>
    struct socket_send_operation: async_operation
    {
      typedef WriteHandler Handler;
      typedef typename async_operation::ptr<socket_send_operation, Handler> ptr;

      socket_send_operation(Handler& h, const Buffers& buffers)
        : async_operation(do_complete)
        , fn(move(h))
        , buffers(buffers)
      {}

    protected:
      static void do_complete(iocp_service* owner, async_operation* base, const error_code& ec, size_t transferred)
      {
        if(!owner)
          return;

        ptr::type* self = static_cast<ptr::type*>(base);
        ptr p(self, &self->fn);

        // TODO: check cancel
        // TODO: map error values

        using std::tr2::sys::io_handler_invoke;
        io_handler_invoke(bind_handler(self->fn, ec, transferred), &self->fn);
      }

    private:
      Handler fn;
      Buffers buffers;
    };

    /** void(const std::error_code& ec, size_t bytes_transferred) */
    template<class ReadHandler, class Buffers>
    struct socket_recv_operation: async_operation
    {
      typedef ReadHandler Handler;
      typedef typename async_operation::ptr<socket_recv_operation, Handler> ptr;

      socket_recv_operation(Handler& h, const Buffers& buffers, bool stream, bool empty)
        : async_operation(do_complete)
        , fn(std::forward<Handler>(h))
        , buffers(buffers)
        , is_stream(stream)
        , is_empty(empty)
      {}

    protected:
      static void do_complete(iocp_service* owner, async_operation* base, const error_code& ec, size_t transferred)
      {
        if(!owner)
          return;

        ptr::type* self = static_cast<ptr::type*>(base);
        ptr p(self, &self->fn);

        // TODO: check cancel
        // TODO: map error values
        std::error_code e = ec;
        if(e) {
          if(e == ntl::nt::status::buffer_overflow)
            e = std::make_error_code(std::tr2::network::error::message_size);
        }
        if(!e && transferred == 0 && self->is_stream && !self->is_empty)
          e = std::make_error_code(network::error::eof);

        using std::tr2::sys::io_handler_invoke;
        io_handler_invoke(bind_handler(self->fn, e, transferred), &self->fn);
      }

    private:
      Handler fn;
      Buffers buffers;
      bool is_stream, is_empty;
    };
  } // __ ns

}}}
