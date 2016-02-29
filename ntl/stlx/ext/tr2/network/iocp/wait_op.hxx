#pragma once

#include "op.hxx"

namespace std { namespace tr2 { namespace sys {

  namespace __
  {
    /** void(const std::error_code& ec) */
    template<class WaitHandler>
    struct wait_operation: async_operation
    {
      typedef WaitHandler Handler;
      typedef typename async_operation::ptr<wait_operation, Handler> ptr;

      wait_operation(Handler& h, bool periodic)
        : async_operation(do_complete)
        , fn(std::forward<Handler>(h))
        , periodic(periodic)
      {}

    protected:
      static void do_complete(iocp_service* owner, async_operation* base, const error_code& ec, size_t /*transferred*/)
      {
        if(!owner)
          return;    // called if async_op::destroy

        ptr::type* self = static_cast<ptr::type*>(base);

        if(self->periodic && !ec) {
          // this operation must be called more times, so destroy handler only from timer.cancel (ec = operation_aborted)
          using std::tr2::sys::io_handler_invoke;
          io_handler_invoke(bind_handler(self->fn, ec), &self->fn);
          return;
        }

        // destroy handler
        ptr p(self, &self->fn);

        using std::tr2::sys::io_handler_invoke;
        io_handler_invoke(bind_handler(self->fn, ec), &self->fn);
      }

    private:
      Handler fn;
      bool periodic;
    };

  } // __ ns
}}}
