#pragma once

#include "op.hxx"

namespace std { namespace tr2 { namespace sys {

  namespace __
  {
    /** void() */
    template<class CompletionHandler>
    struct completion_operation: async_operation
    {
      typedef CompletionHandler Handler;
      typedef typename async_operation::ptr<completion_operation, Handler> ptr;

      completion_operation(Handler& h)
        : async_operation(do_complete)
        , fn(std::forward<Handler>(h))
      {}

    protected:
      static void do_complete(iocp_service* owner, async_operation* base, const error_code& /*ec*/, size_t /*transferred*/)
      {
        if(!owner)
          return;    // called if async_op::destroy

        ptr::type* self = static_cast<ptr::type*>(base);
        ptr p(self, &self->fn);

        using std::tr2::sys::io_handler_invoke;
        io_handler_invoke(self->fn, &self->fn);
      }

    private:
      Handler fn;
    };

  } // __ ns

}}}
