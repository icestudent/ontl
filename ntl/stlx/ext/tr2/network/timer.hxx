/**\file*********************************************************************
 *                                                                     \brief
 *  Timers
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_TIMER
#define NTL__STLX_TR2_TIMER
#pragma once

#include "io_service.hxx"
#include "basic_io_object.hxx"

#include "../../chrono.hxx"
#include "../../thread.hxx"
#include "../../../nt/timer.hxx"
#include "../../../nt/system_error.hxx"

#include "../../function.hxx"

#include "iocp/wait_op.hxx"
#include "io_futures.hxx"
#include "placeholders.hxx"

namespace std { namespace tr2 { namespace sys {

  typedef std::chrono::system_clock::time_point date_time;

  /// 5.4.2.1. Time traits
  template<class Time> struct time_traits;

  /// 5.4.3. time_traits specialisations
  template<>
  struct time_traits<date_time>
  {
    typedef date_time             time_type;
    typedef time_type::duration   duration_type;

    static time_type now() { return time_type::clock::now(); }

    static time_type add(const time_type& t, const duration_type& d)        { return t + d; }
    static duration_type subtract(const time_type& t1, const time_type& t2) { return t1 - t2; }
    static bool less_than(const time_type& t1, const time_type& t2)         { return t1 < t2; }

    static duration_type to_std_duration(const duration_type& d) { return d; }
  };

  namespace __
  {
    template<class Time, class TimeTraits, bool Periodic>
    class timer_service_impl
    {
      typedef __::timer_scheduler scheduler_type;
    public:
      // types:
      typedef TimeTraits                          traits_type;
      typedef Time                                time_type;
      typedef typename TimeTraits::duration_type  duration_type;

      struct implementation_type:
        private ntl::noncopyable
      {
        ntl::nt::timer  tm;
        time_type       tp;
        duration_type   period;
        bool            periodic;

        implementation_type()
          :tm(Periodic ? tm.auto_reset : tm.manual_reset)
          ,periodic()
        {}

        bool reset(const time_type& t)
        {
          tp = t;
          period = duration_type();
          return tm.set(t, nullptr, this);
        }
        bool reset(const time_type& t, const duration_type& d)
        {
          tp = t;
          period = d;
          return tm.set(t, d, nullptr, this);
        }

      private:
        static void __stdcall routine(void* ctx, uint32_t, int32_t)
        {
          implementation_type* self = reinterpret_cast<implementation_type*>(ctx);
          if(self->handler)
            self->handler(error_code()); // error_code = success
        }
      };

    public:
      explicit timer_service_impl(io_service& ios)
        : scheduler(use_service<scheduler_type>(ios))
      {}

      /** Cleans up resources owned by the timer implementation \p impl. Cancels asynchronous wait operations. */
      void destroy(implementation_type& impl)
      {
        error_code ec;
        cancel(impl, ec);
        impl.tm.reset();
      }

      /**  Causes any outstanding asynchronous wait operations to complete as soon as possible. Handlers for 
      cancelled operations shall be passed the error code \c error::operation_canceled.  */
      size_t cancel(implementation_type& impl, error_code& ec) __ntl_nothrow
      {
        ec.clear();
        size_t c = scheduler.remove_timer(impl.tm.get());
        if(!impl.tm.cancel())
          ec = std::make_error_code(impl.tm.last_status());
        return c;
      }

      /** Returns the expiry time associated with the timer implementation impl */
      time_type expires_at(const implementation_type& impl) const __ntl_nothrow { return impl.tp; }

      /** Returns a duration from now to the expiry time associated with the timer implementation \p impl */
      duration_type expires_from_now(const implementation_type& impl) const __ntl_nothrow
      {
        return traits_type::subtract(impl.tp, traits_type::now());
      }

      /** Performs a synchronous wait operation associated with the timer implementation \p impl.  */
      error_code wait(implementation_type& impl, error_code& ec) __ntl_nothrow
      {
        using namespace ntl::nt;
        ntstatus st = NtWaitForSingleObject(impl.tm.get(), true, infinite_timeout());
        if(success(st))
          ec.clear();
        else
          ec = std::make_error_code(st);
        return ec;
      }

      /**  Initiates an asynchronous wait operation that is performed via the io_service object returned by 
        get_io_service() and behaves according to asynchronous operation requirements.  */
      template<class WaitHandler>
      void async_wait(implementation_type& impl, WaitHandler handler)
      {
        // periodic timer does not destroy wait handler after first call
        typedef __::wait_operation<WaitHandler> op;
        typename op::ptr p (handler, Periodic);

        const __::timer_scheduler::timer_data data = { impl.tm.get(), p.op, 
          std::chrono::duration_cast<ntl::nt::system_duration>(impl.tp.time_since_epoch()).count(),
          std::chrono::duration_cast<ntl::nt::system_duration>(impl.period) };

        scheduler.add_timer(data.h, &data);
        p.release();
      }

    private:
      scheduler_type& scheduler;
    };
  }

  template<class Time, class TimeTraits> class deadline_timer_service;
  template<class Time, class TimeTraits> class periodic_timer_service;

  template<class Time, class TimeTraits = time_traits<Time>, class TimerService = deadline_timer_service<Time, TimeTraits> >
  class basic_deadline_timer;

  template<class Time, class TimeTraits = time_traits<Time>, class TimerService = periodic_timer_service<Time, TimeTraits> >
  class basic_periodic_timer;

  /** Typedef for the typical usage of timer based on the system clock.. */
  typedef basic_deadline_timer<date_time> deadline_timer;

  /** Typedef for the periodic timer based on the system clock. */
  typedef basic_periodic_timer<date_time> periodic_timer;


  // Wait completion handler
  template<class F>
  class WaitHandler:
    public sys::CompleteHandler<F, void>
  {
  public:
    explicit WaitHandler(F f)
      : CompleteHandler(f)
    {}
  };


  /**
   *  @brief 5.4.4. Class template deadline_timer_service
   **/
  template<class Time, class TimeTraits>
  class deadline_timer_service:
    public io_service::service
   ,public __::timer_service_impl<Time, TimeTraits, false>
  {
  public:
    static io_service::id id;

    explicit deadline_timer_service(io_service& ios)
      : service(ios)
      , timer_service_impl(ios)
    {}

    /** Initialises the timer implementation \p impl. */
    void construct(implementation_type& impl)
    {
      impl.periodic = false;
    }

    using timer_service_impl::expires_at;
    using timer_service_impl::expires_from_now;

    /** Sets the expiry time associated with the timer implementation \p impl. Implicitly cancels asynchronous 
      wait operations */
    size_t expires_at(implementation_type& impl, const time_type& t, error_code& ec) __ntl_nothrow
    {
      const size_t c = cancel(impl, ec);
      if(!impl.reset(t))
        ec = std::make_error_code(impl.tm.last_status());
      return c;
    }


    /** Sets the expiry time associated with the timer implementation \p impl. Implicitly cancels asynchronous 
      wait operations */
    size_t expires_from_now(implementation_type& impl, const duration_type& d, error_code& ec) __ntl_nothrow
    {
      const size_t c = cancel(impl, ec);
      if(!impl.reset(traits_type::add(traits_type::now(), d)))
        ec = std::make_error_code(impl.tm.last_status());
      return c;
    }

  private:
    virtual void shutdown_service()
    {}
  };


  /**
   *  @brief 5.4.5. Class template basic_deadline_timer
   *  @details The basic_deadline_timer class template provides the ability to perform a blocking or asynchronous wait for a timer to expire.
   **/
  template<class Time, class TimeTraits, class TimerService>
  class basic_deadline_timer:
    public basic_io_object<TimerService>
  {
  public:
    ///\name types:
    typedef TimeTraits traits_type;
    typedef Time time_type;
    typedef typename TimeTraits::duration_type duration_type;

    ///\name constructors:

    /** Constructor. */
    explicit basic_deadline_timer(io_service& ios)
      :basic_io_object(ios)
    {}

    /** Construct timer which fired at \p when time. */
    explicit basic_deadline_timer(io_service& ios, const time_type& when)
      :basic_io_object(ios)
    {
      error_code ec;
      service.expires_at(impl, when, ec);
      if(ec)
        __ntl_throw(system_error(ec));
    }

    /** Construct timer which fired \p after time. */
    explicit basic_deadline_timer(io_service& ios, const duration_type& after)
      :basic_io_object(ios)
    {
      error_code ec;
      service.expires_from_now(impl, after, ec);
      if(ec)
        __ntl_throw(system_error(ec));
    }

    ///\name members:

    /** Cancel any asynchronous operations that are waiting on the timer. */
    size_t cancel()
    {
      error_code ec;
      size_t c = service.cancel(impl, ec);
      if(ec)
        __ntl_throw(system_error(ec));
      return c;
    }

    /** Cancel any asynchronous operations that are waiting on the timer. */
    size_t cancel(error_code& ec) __ntl_nothrow
    {
      return service.cancel(impl, ec);
    }

    /** Get the timer's expiry time as an absolute time. */
    time_type expires_at() const  __ntl_nothrow
    {
      return service.expires_at(impl);
    }

    /** Set the timer's expiry time as an absolute time. */
    size_t expires_at(const time_type& time)
    {
      error_code ec;
      size_t c = service.expires_at(impl, time, ec);
      if(ec)
        __ntl_throw(system_error(ec));
      return c;
    }

    /** Set the timer's expiry time as an absolute time. */
    size_t expires_at(const time_type& time, error_code& ec)
    {
      return service.expires_at(impl, time, ec);
    }

    /** Get the timer's expiry time relative to now. */
    duration_type expires_from_now() const __ntl_nothrow
    {
      return service.expires_from_now(impl);
    }

    /** Set the timer's expiry time relative to now. */
    size_t expires_from_now(const duration_type& after)
    {
      error_code ec;
      size_t c = service.expires_from_now(impl, after, ec);
      if(ec)
        __ntl_throw(system_error(ec));
      return c;
    }

    /** Set the timer's expiry time relative to now. */
    size_t expires_from_now(const duration_type& after, error_code& ec)
    {
      return service.expires_from_now(impl, after, ec);
    }

    /** Perform a blocking wait on the timer. */
    void wait()
    {
      error_code ec;
      service.wait(impl, ec);
      if(ec)
        __ntl_throw(system_error(ec));
    }

    /** Perform a blocking wait on the timer. */
    error_code wait(error_code& ec)
    {
      return service.wait(impl, ec);
    }
    
   /** Start an asynchronous wait on the timer. */
   template <class Handler>
    typename WaitHandler<Handler>::type async_wait(Handler handler)
    {
      WaitHandler<Handler> wrap(handler);
      service.async_wait(impl, wrap.handler);
      return wrap.result.get();
    }
    ///\}
  };


  
  /**
   *  @brief 5.4.6. Class template periodic_timer_service
   **/
  template<class Time, class TimeTraits>
  class periodic_timer_service:
    public io_service::service
   ,public __::timer_service_impl<Time, TimeTraits, true>
  {
  public:
    static io_service::id id;

    explicit periodic_timer_service(io_service& ios)
      : service(ios)
      , timer_service_impl(ios)
    {}

    /** Initialises the timer implementation \p impl. */
    void construct(implementation_type& impl)
    {
      impl.periodic = true;
    }

    using timer_service_impl::expires_at;
    using timer_service_impl::expires_from_now;

    /** Sets the expiry time associated with the timer implementation \p impl. Implicitly cancels asynchronous 
      wait operations */
    size_t expires_at(implementation_type& impl, const time_type& time, const duration_type& period, error_code& ec) __ntl_nothrow
    {
      const size_t c = cancel(impl, ec);
      if(!impl.reset(time, period))
        ec = std::make_error_code(impl.tm.last_status());
      return c;
    }

    /** Sets the expiry time associated with the timer implementation \p impl. Implicitly cancels asynchronous 
      wait operations */
    size_t expires_from_now(implementation_type& impl, const duration_type& after, const duration_type& period, error_code& ec) __ntl_nothrow
    {
      const size_t c = cancel(impl, ec);
      if(!impl.reset(traits_type::add(traits_type::now(), after), period))
        ec = std::make_error_code(impl.tm.last_status());
      return c;
    }

  private:
    virtual void shutdown_service()
    {}
  };


  /**
   *  @brief 5.4.7. Class template basic_periodic_timer
   *  @details The basic_periodic_timer class template provides the ability to perform a blocking or asynchronous periodic timer.
   **/
  template<class Time, class TimeTraits, class TimerService>
  class basic_periodic_timer:
    public basic_io_object<TimerService>
  {
  public:
    ///\name types:
    typedef TimeTraits traits_type;
    typedef Time time_type;
    typedef typename TimeTraits::duration_type duration_type;

    ///\name constructors:

    /** Constructor. */
    explicit basic_periodic_timer(io_service& ios)
      :basic_io_object(ios)
    {}

    /** Construct timer which fired at \p when time with \p period calls. */
    explicit basic_periodic_timer(io_service& ios, const time_type& when, const duration_type& period)
      :basic_io_object(ios)
    {
      error_code ec;
      service.expires_at(impl, when, period, ec);
      if(ec)
        __ntl_throw(system_error(ec));
    }

    /** Construct timer which fired \p after time with \p period calls. */
    explicit basic_periodic_timer(io_service& ios, const duration_type& after, const duration_type& period)
      :basic_io_object(ios)
    {
      error_code ec;
      service.expires_from_now(impl, after, period, ec);
      if(ec)
        __ntl_throw(system_error(ec));
    }

    /** Construct timer which fired every \p period time. */
    explicit basic_periodic_timer(io_service& ios, const duration_type& period)
      :basic_io_object(ios)
    {
      error_code ec;
      service.expires_from_now(impl, period, period, ec);
      if(ec)
        __ntl_throw(system_error(ec));
    }

    ///\name members:

    /** Cancel any asynchronous operations that are waiting on the timer. */
    size_t cancel()
    {
      error_code ec;
      size_t c = service.cancel(impl, ec);
      if(ec)
        __ntl_throw(system_error(ec));
      return c;
    }

    /** Cancel any asynchronous operations that are waiting on the timer. */
    size_t cancel(error_code& ec) __ntl_nothrow
    {
      return service.cancel(impl, ec);
    }

    /** Get the timer's expiry time as an absolute time. */
    time_type expires_at() const  __ntl_nothrow
    {
      return service.expires_at(impl);
    }

    /** Set the timer's expiry time as an absolute time with \p period. */
    size_t expires_at(const time_type& time, const duration_type& period)
    {
      error_code ec;
      size_t c = service.expires_at(time, period, ec);
      if(ec)
        __ntl_throw(system_error(ec));
      return c;
    }

    /** Set the timer's expiry time as an absolute time with \p period. */
    size_t expires_at(const time_type& time, const duration_type& period, error_code& ec)
    {
      return service.expires_at(impl, time, period, ec);
    }

    /** Get the timer's expiry time relative to now. */
    duration_type expires_from_now() const __ntl_nothrow
    {
      return service.expires_from_now(impl);
    }

    /** Set the timer's \p period time relative to now. */
    size_t expires_from_now(const duration_type& period)
    {
      error_code ec;
      size_t c = service.expires_from_now(impl, period, period, ec);
      if(ec)
        __ntl_throw(system_error(ec));
      return c;
    }

    /** Set the timer's \p period time relative to now. */
    size_t expires_from_now(const duration_type& d, error_code& ec)
    {
      return service.expires_from_now(impl, d, d, ec);
    }

    /** Perform a blocking wait on the timer. */
    void wait()
    {
      error_code ec;
      service.wait(impl, ec);
      if(ec)
        __ntl_throw(system_error(ec));
    }

    /** Perform a blocking wait on the timer. */
    error_code wait(error_code& ec)
    {
      return service.wait(impl, ec);
    }
    
    /** Start an asynchronous wait on the timer. */
    template <class Handler>
    typename WaitHandler<Handler>::type async_wait(Handler handler)
    {
      WaitHandler<Handler> wrap(handler);
      service.async_wait(impl, wrap.handler);
      return wrap.result.get();
    }
    ///\}
  };

}}}
#endif // NTL__STLX_TR2_TIMER
