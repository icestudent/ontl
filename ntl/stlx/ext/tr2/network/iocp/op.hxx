#pragma once

namespace std { namespace tr2 { namespace sys {

  namespace iocp
  {
    class iocp_service;
  }

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

      struct timer_data 
      {
        ntl::nt::legacy_handle h;
        async_operation* op;
        ntl::nt::system_duration fire;
      };

      typedef size_t add_timer_t(void* ctx, const void* timer, const timer_data* data);

      void add_timer(const void* timer, const timer_data* data)
      {
        assert(handler);
        if(handler)
          handler(ctx, timer, data);
      }

      size_t remove_timer(const void* timer)
      {
        assert(handler);
        return handler ? handler(ctx, timer, nullptr) : 0;
      }


    private:
      //std::function<void(/*const*/ void*, async_operation*)> handler;
      add_timer_t* handler;
      void* ctx;

      friend class iocp::iocp_service;
      void shutdown_service() override
      {}
    };


    /** Base async operation */
    struct async_operation: ntl::nt::overlapped
    {
      typedef iocp::iocp_service iocp_service;

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

        typedef Op type;

        /** attach to existing handler pointers */
        explicit ptr(Op* op, Handler* fn)
          : fn(fn)
          , v(op)
          , op(op)
        {}

        /** allocate and construct smart handler pointer */
        explicit ptr(Handler& fn)
          : fn(&fn)
          , op()
          , v()
        {
          using std::tr2::sys::io_handler_allocate;
          v = io_handler_allocate(sizeof(Op), &fn);
          op = new (v) Op(fn);
        }

        template<typename A1>
        explicit ptr(Handler& fn, const A1& a1)
          : fn(&fn)
          , op()
          , v()
        {
          using std::tr2::sys::io_handler_allocate;
          v = io_handler_allocate(sizeof(Op), this->fn);
          op = new (v) Op(fn, a1);
        }

        template<typename A1, typename A2>
        explicit ptr(Handler& fn, const A1& a1, const A2& a2)
          : fn(&fn)
          , op()
          , v()
        {
          using std::tr2::sys::io_handler_allocate;
          v = io_handler_allocate(sizeof(Op), this->fn);
          op = new (v) Op(fn, a1, a2);
        }

        template<typename A1, typename A2, typename A3>
        explicit ptr(Handler& fn, const A1& a1, const A2& a2, const A3& a3)
          : fn(&fn)
          , op()
          , v()
        {
          using std::tr2::sys::io_handler_allocate;
          v = io_handler_allocate(sizeof(Op), this->fn);
          op = new (v) Op(fn, a1, a2, a3);
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
            Op* derived = static_cast<Op*>(op);
            derived->~Op();
            op = nullptr;
          }
          if(v) {
            io_handler_deallocate(v, sizeof(Op), this->fn);
            v = nullptr;
          }
        }
      };

    protected:
      typedef void handler_t(iocp_service* owner, async_operation* base, const error_code& ec, size_t transferred);

    protected:
      //virtual ~async_operation(){}

      explicit async_operation(handler_t* fn)
        : handle(fn)
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
      handler_t* handle;
      ntl::atomic::flag_t ready;
    };

  } // __ ns

}}}

namespace std
{
  template<>
  struct less<std::tr2::sys::__::timer_scheduler::timer_data>
  {
    typedef std::tr2::sys::__::timer_scheduler::timer_data T;
    bool operator()(const T& x, const T& y) const
    {
      return x.fire < y.fire;
    }
  };
  template<>
  struct greater<std::tr2::sys::__::timer_scheduler::timer_data>
  {
    typedef std::tr2::sys::__::timer_scheduler::timer_data T;
    bool operator()(const T& x, const T& y) const
    {
      return x.fire > y.fire;
    }
  };
}
