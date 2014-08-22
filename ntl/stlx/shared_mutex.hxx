/**\file*********************************************************************
 *                                                                     \brief
 *  30.3 Mutual exclusion [thread.mutex]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_SHAREDMUTEX
#define NTL__STLX_SHAREDMUTEX
#pragma once

#include "mutex.hxx"

namespace std
{
  /**
   *	@addtogroup threads 30 Thread support library [thread]
   *  @{
   *
   *	@addtogroup thread_mutex 30.3 Mutual exclusion [thread.mutex]
   *  @{
   **/

  /**
   *	@brief Class shared_mutex [30.4.1.4.1 thread.sharedtimedmutex.class]
   *	
   *	The shared_mutex class is a synchronization primitive that can be used to protect shared data from being 
   *	simultaneously accessed by multiple threads. In contrast to other mutex types which facilitate exclusive access, 
   *	a shared_mutex has two levels of access: 
   *  * shared - several threads can share ownership of the same mutex. 
   *  * exclusive - only one thread can own the mutex. 
   *  
   *  Shared mutexes are usually used in situations, when multiple readers can access the same resource at the same time 
   *  without causing data races, but only one writer can do so.
   **/
  class shared_mutex: public ntl::nt::base_shared_mutex
  {
  public:
    shared_mutex()
    {}
    ~shared_mutex()
    {}
  };


  class shared_timed_mutex: public ntl::nt::base_shared_mutex
  {
  public:
    shared_timed_mutex()
    {}
    ~shared_timed_mutex()
    {}

    template <class Rep, class Period>
    bool try_lock_for(const chrono::duration<Rep, Period>& rel_time) __ntl_nothrow
    {
      if(is_owner()) // throw system_error(resource_deadlock_would_occur)
        _assert_msg("Current thread already owns the mutex.");
      const ntl::nt::system_duration period = chrono::duration_cast<ntl::nt::system_duration>(rel_time);
      return period.count() <= 0 ? try_lock() : ntl::nt::success( wait_exclusive( -1i64*period.count() ) );
    }

    template <class Clock, class Duration>
    bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time) __ntl_nothrow
    {
      if(is_owner()) // throw system_error(resource_deadlock_would_occur)
        _assert_msg("Current thread already owns the mutex.");
      const ntl::nt::system_duration period = chrono::duration_cast<ntl::nt::system_duration>(abs_time.time_since_epoch());
      return period.count() == 0 ? try_lock() : ntl::nt::success( wait_exclusive(period.count() ) );
    }

    template <class Rep, class Period>
    bool try_lock_shared_for(const chrono::duration<Rep, Period>& rel_time) __ntl_nothrow
    {
      if(is_owner()) // throw system_error(resource_deadlock_would_occur)
        _assert_msg("Current thread already owns the mutex.");
      const ntl::nt::system_duration period = chrono::duration_cast<ntl::nt::system_duration>(rel_time);
      return period.count() <= 0 ? try_lock_shared() : ntl::nt::success( wait_shared( -1i64*period.count() ) );
    }

    template <class Clock, class Duration>
    bool try_lock_shared_until(const chrono::time_point<Clock, Duration>& abs_time) __ntl_nothrow
    {
      if(is_owner()) // throw system_error(resource_deadlock_would_occur)
        _assert_msg("Current thread already owns the mutex.");
      const ntl::nt::system_duration period = chrono::duration_cast<ntl::nt::system_duration>(abs_time.time_since_epoch());
      return period.count() == 0 ? try_lock_shared() : ntl::nt::success( wait_shared(period.count()) );
    }

  };


  /** @} thread_mutex */
  /** @} threads */
} // std
#endif // NTL__STLX_SHAREDMUTEX
