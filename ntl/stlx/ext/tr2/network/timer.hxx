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
#include "../../chrono.hxx"
#include "../../thread.hxx"

#include "io_service_fwd.hxx"

namespace std { namespace tr2 { namespace sys {

  struct date_time;

  /// 5.4.2.1. Time traits
  template<class Time> struct time_traits;

  /// 5.4.3. time_traits specialisations
  template<>
  struct time_traits<std::chrono::system_clock::time_point>
  {
    typedef std::chrono::system_clock::time_point time_type;
    typedef time_type::duration                   duration_type;

    static time_type now() { return time_type::clock::now(); }

    static time_type add(const time_type& t, const duration_type& d)        { return t + d; }
    static duration_type subtract(const time_type& t1, const time_type& t2) { return t1 - t2; }
    static bool less_than(const time_type& t1, const time_type& t2)         { return t1 < t2; }

    static duration_type to_std_duration(const duration_type& d) { return d; }
  };



  /**
   *	@brief 5.4.4. Class template deadline_timer_service
   **/
  template<class Time, class TimeTraits>
  class deadline_timer_service:
    public io_service::service
  {
  public:
    static io_service::id id;

    // types:
    typedef TimeTraits                          traits_type;
    typedef Time                                time_type;
    typedef typename TimeTraits::duration_type  duration_type;
    typedef unspecified implementation_type;

    explicit deadline_timer_service(io_service& ios);

    /** Initialises the timer implementation \p impl. */
    void construct(implementation_type& impl)
    {}
    /** Cleans up resources owned by the timer implementation \p impl. Cancels asynchronous wait operations. */
    void destroy(implementation_type& impl)
    {
      error_code ec;
      cancel(impl, ec);
    }

    /**  Causes any outstanding asynchronous wait operations to complete as soon as possible. Handlers for 
      cancelled operations shall be passed the error code \c error::operation_canceled.  */
    size_t cancel(implementation_type& impl, error_code& ec) __ntl_nothrow;

    /** Returns the expiry time associated with the timer implementation impl */
    time_type expires_at(const implementation_type& impl) const __ntl_nothrow
    {
      return tp;
    }

    /** Sets the expiry time associated with the timer implementation \p impl. Implicitly cancels asynchronous 
      wait operations */
    size_t expires_at(implementation_type& impl, const time_type& t, error_code& ec) __ntl_nothrow
    {
      size_t c = cancel(impl, ec);
      tp = t;
      return c;
    }

    /** Returns a duration from now to the expiry time associated with the timer implementation \p impl */
    duration_type expires_from_now(const implementation_type& impl) const __ntl_nothrow
    {
      return traits_type::substract(tp, traits_time::now());
    }

    /** Sets the expiry time associated with the timer implementation \p impl. Implicitly cancels asynchronous 
      wait operations */
    size_t expires_from_now(implementation_type& impl, const duration_type& d, error_code& ec) __ntl_nothrow
    {
      size_t c = cancel(impl, ec);
      tp = traits_type::add(traits_type::now(), d);
      return c;
    }

    /** Performs a synchronous wait operation associated with the timer implementation \p impl.  */
    error_code wait(implementation_type& impl, error_code& ec) __ntl_nothrow;

    /**  Initiates an asynchronous wait operation that is performed via the io_service object returned by 
      get_io_service() and behaves according to asynchronous operation requirements.  */
    template<class WaitHandler>
    void async_wait(implementation_type& impl, WaitHandler handler);

  private:
    virtual void shutdown_service();

  private:
    time_type tp;
  };

  template<class Time, class TimeTraits = time_traits<Time>, class TimerService = deadline_timer_service<Time, TimeTraits> >
  class basic_deadline_timer;




  /**
   *	@brief 5.4.5. Class template basic_deadline_timer
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
    explicit basic_deadline_timer(io_service& ios)
      :basic_io_object(ios)
    {}
    basic_deadline_timer(io_service& ios, const time_type& t)
      :basic_io_object(ios)
    {
      error_code ec;
      svc.expires_at(impl, t, ec);
      if(ec)
        __ntl_throw(system_error(ec));
    }
    basic_deadline_timer(io_service& ios, const duration_type& d)
      :basic_io_object(ios)
    {
      error_code ec;
      svc.expires_from_now(impl, d, ec);
      if(ec)
        __ntl_throw(system_error(ec));
    }

    ///\name members:
    size_t cancel()
    {
      error_code ec;
      size_t c = svc.cancel(impl, ec);
      if(ec)
        __ntl_throw(system_error(ec));
      return c;
    }
    size_t cancel(error_code& ec) __ntl_nothrow
    {
      return svc.cancel(impl, ec);
    }

    time_type expires_at() const  __ntl_nothrow
    {
      return svc.expires_at(impl);
    }

    size_t expires_at(const time_type& t)
    {
      error_code ec;
      size_t c = svc.expires_at(t, ec);
      if(ec)
        __ntl_throw(system_error(ec));
      return c;
    }
    size_t expires_at(const time_type& t, error_code& ec)
    {
      return svc.expires_at(impl, t, ec);
    }

    duration_type expires_from_now() const __ntl_nothrow
    {
      return svc.expires_from_now(impl);
    }

    size_t expires_from_now(const duration_type& d)
    {
      error_code ec;
      size_t c = svc.expires_from_now(impl, d, ec);
      if(ec)
        __ntl_throw(system_error(ec));
      return c;
    }
    size_t expires_from_now(const duration_type& d, error_code& ec)
    {
      return svc.expires_from_now(impl, d, ec);
    }

    void wait()
    {
      error_code ec;
      svc->wait(impl, ec);
      if(ec)
        __ntl_throw(system_error(ec));
    }

    error_code wait(error_code& ec)
    {
      return svc->wait(impl, ec);
    }
    
    template <class WaitHandler>
    void async_wait(WaitHandler handler)
    {
      svc->async_wait(impl, handler);
    }
    ///\}
  };

  typedef basic_deadline_timer<date_time> deadline_timer;

}}}
#endif // NTL__STLX_TR2_TIMER
