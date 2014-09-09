#pragma once

#include "../iocp/op.hxx"

namespace std { namespace tr2 { namespace sys {

  namespace __
  {
    /* void(const std::error_code& ec, resolver::iterator it) */
    template<class ResolveHandler, class Service, class Query>
    struct resolve_operation: async_operation
    {
      typedef ResolveHandler Handler;
      typedef typename async_operation::ptr<resolve_operation, Handler> ptr;

      resolve_operation(Handler& h, Service* service, typename Service::implementation_type* impl, const Query& q)
        : async_operation(do_complete)
        , fn(std::forward<Handler>(h))
        , service(service)
        , impl(impl)
        , query(q)
      {}

      static void do_complete(iocp_service* owner, async_operation* base, const error_code& /*ec*/, size_t /*transferred*/)
      {
        if(!owner)
          return;

        ptr::type* self = static_cast<ptr::type*>(base);
        ptr p(self, &self->fn);

        if(self->impl) {
          self->result = self->service->resolve(*self->impl, self->query, self->ec);
          self->impl = nullptr;
          self->service->iocp.post_deferred_completion(self);
          p.release();
          return;
        } 

        using std::tr2::sys::io_handler_invoke;
        io_handler_invoke(bind_handler(self->fn, self->ec, self->result), &self->fn);
      }

    protected:
      Handler fn;
      Service* service;
      Query query;
      typename Service::implementation_type* impl;
      typename Service::iterator_type result;
      std::error_code ec;
    };
  }
}}}
