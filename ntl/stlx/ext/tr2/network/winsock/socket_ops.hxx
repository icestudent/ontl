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
        , fn(std::forward<Handler>(h))
        , buffers(buffers)
      {}

    protected:
      static void do_complete(iocp_service* owner, async_operation* base, const error_code& ec, size_t transferred)
      {
        if(!owner)
          return;    // called if async_op::destroy

        socket_send_operation* self = static_cast<socket_send_operation*>(base);
        ptr p(self, &self->fn);

        // TODO: check cancel

        using std::tr2::sys::io_handler_invoke;
        io_handler_invoke(bind_handler(self->fn, ec, transferred), &self->fn);
      }

    private:
      Handler fn;
      Buffers buffers;
    };

  } // __ ns

}}}
