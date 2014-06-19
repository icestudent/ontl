#pragma once

namespace std { namespace tr2 { namespace sys {

  class iocp_service;
  namespace __
  {
    struct async_operation;

    /** Timer scheduler interface */
    struct timer_scheduler:  io_service::service
    {
      timer_scheduler(io_service& ios)
        : service(ios)
        , handler()
        , ctx()
      {}

      typedef void add_timer_t(void* ctx, const void* timer, async_operation* op);

      void add_timer(const void* timer, async_operation* op)
      {
        assert(handler);
        if(handler)
          handler(ctx, timer, op);
      }



    private:
      //std::function<void(/*const*/ void*, async_operation*)> handler;
      add_timer_t* handler;
      void* ctx;

      friend class iocp_service;
      void shutdown_service() override
      {}
    };


    /** Base async operation */
    struct async_operation: ntl::nt::overlapped
    {
      void complete(iocp_service& owner, const error_code& ec, size_t transferred) const
      {
        handle(&owner, const_cast<async_operation*>(this), ec, transferred);
      }

      void destroy()
      {
        handle(nullptr, this, error_code(), 0);
      }

      bool is_async_operation() const { return this && signature == signature_const; }


      template<class Op, class Handler>
      struct ptr
      {
        Handler* fn;
        void* v;              // memory for op
        async_operation* op;  // op itself
        size_t cb;

        //template<class Op>
        explicit ptr(Handler& fn)
          : fn(&fn)
          , cb(sizeof(Op))
          , op()
          , v()
        {
          using std::tr2::sys::io_handler_allocate;
          v = io_handler_allocate(cb, &fn);
          op = new (v) Op(fn);
        }

        ~ptr()
        {
          reset();
        }

        void release()
        {
          v = op = nullptr;
        }

        void reset()
        {
          using std::tr2::sys::io_handler_deallocate;
          if(op) {
            op->~async_operation();
            op = nullptr;
          }
          if(v) {
            io_handler_deallocate(v, cb, fn);
            v = nullptr;
          }
        }
      };

    protected:
      typedef void handler_t(iocp_service* owner, async_operation* base, const error_code& ec, size_t transferred);

    protected:
      virtual ~async_operation(){}

      explicit async_operation(handler_t* fn)
        : handle(fn)
        , next()
        , ready(false)
        , signature(signature_const)
      {
        reset();
      }

      void reset()
      {
        ntl::bzero(static_cast<ntl::nt::overlapped&>(*this));
        ready = false;
      }

    private:
      friend class iocp_service;
      static const uint32_t signature_const = 0xFEDCBA91;
      const uint32_t signature;
      async_operation* next;
      handler_t* handle;
      ntl::atomic::flag_t ready;
    };

  } // __ ns

}}}
