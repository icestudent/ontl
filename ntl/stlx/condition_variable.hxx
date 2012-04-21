/**\file*********************************************************************
 *                                                                     \brief
 *  30.4 Condition variables [thread.condition]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CONDVARIABLE
#define NTL__STLX_CONDVARIABLE
#pragma once

#include "chrono.hxx"
#include "mutex.hxx"
#include "../nt/cond_variable.hxx"

namespace std
{
  /**
   *	@addtogroup threads 30 Thread support library [thread]
   *  @{
   *
   *	@defgroup thread_condition 30.5 Condition variables [thread.condition]
   *
   *  Condition variables provide synchronization primitives used to block a %thread until notified by some other
   *  %thread that some condition is met or until a system time is reached. Class condition_variable provides
   *  a condition variable that can only wait on a \b Lock, allowing maximum effciency on some platforms. Class
   *  condition_variable_any provides a general condition variable that can wait on user-supplied lock types.
   *
   *  @{
   **/

#ifdef NTL_CXX_ENUM
  enum class cv_status { no_timeout, timeout };
#else
  __class_enum(cv_status) { no_timeout, timeout };};
#endif

  class condition_variable;
  class condition_variable_any;

  /**
   *	@brief Notify all at thread exit
   *
   *  Transfers ownership of the lock associated with \c lk into internal storage and schedules \c cond to
   *  be notified when the current thread exits, after all objects of thread storage duration associated with
   *  the current thread have been destroyed.
   **/
  void notify_all_at_thread_exit(condition_variable& cond, unique_lock<mutex> lk);


  /**
   *	@brief Class condition_variable [30.5.1 thread.condition.condvar]
   *
   *  Class condition_variable provides a condition variable that can only wait on a Lock, allowing maximum effciency on some platforms. 
   **/
  class condition_variable:
    protected ntl::nt::conditional_variable
  {
    struct monitor
    {
      volatile unsigned& waiters;
      monitor(volatile unsigned& waiters)
        :waiters(waiters)
      {
        ntl::atomic::increment(waiters);
      }
      ~monitor()
      {
        ntl::atomic::decrement(waiters);
      }
    };
  public:
    typedef ntl::nt::rtl::conditional_variable* native_handle_type;

    condition_variable()
      :waiters(0)
    {}
    ~condition_variable()
    {
      assert(waiters == 0);
    }
    
    /** If any threads are blocked waiting for \c *this, unblocks one of those theads. */
    void notify_one() __ntl_nothrow
    {
      wake();
    }

    /** Unblocks all threads that are blocked waiting for \c *this. */
    void notify_all() __ntl_nothrow
    {
      wake_all();
    }
    
    void wait(unique_lock<mutex>& lock)
    {
      monitor _(waiters);
      mutex::native_handle_type m = lock.mutex()->native_handle();
      __super::sleep(*m);
    }
    
    template <class Predicate>
    void wait(unique_lock<mutex>& lock, Predicate pred)
    {
      monitor _(waiters);
      mutex::native_handle_type m = lock.mutex()->native_handle();
      while(!pred())
        __super::sleep(*m);
    }
    
    template <class Clock, class Duration>
    cv_status wait_until(unique_lock<mutex>& lock, const chrono::time_point<Clock, Duration>& abs_time)
    {
      monitor _(waiters);
      mutex::native_handle_type m = lock.mutex()->native_handle();
      return __super::sleep_until(abs_time, *m) == ntl::nt::status::timeout ? cv_status::timeout : cv_status::no_timeout;
    }

    template <class Clock, class Duration, class Predicate>
    bool wait_until(unique_lock<mutex>& lock, const chrono::time_point<Clock, Duration>& abs_time, Predicate pred)
    {
      monitor _(waiters);
      mutex::native_handle_type m = lock.mutex()->native_handle();
      while(!pred()){
        if(__super::sleep_until(abs_time, *m) == ntl::nt::status::timeout)
          return pred();
      }
      return true;
    }

    template <class Rep, class Period>
    cv_status wait_for(unique_lock<mutex>& lock, const chrono::duration<Rep, Period>& rel_time)
    {
      monitor _(waiters);
      mutex::native_handle_type m = lock.mutex()->native_handle();
      return __super::sleep_for(rel_time, *m) == ntl::nt::status::timeout ? cv_status::timeout : cv_status::no_timeout;
    }

    template <class Rep, class Period, class Predicate>
    bool wait_for(unique_lock<mutex>& lock, const chrono::duration<Rep, Period>& rel_time, Predicate pred)
    {
      monitor _(waiters);
      mutex::native_handle_type m = lock.mutex()->native_handle();
      while(!pred()){
        if(__super::sleep_for(rel_time, *m) == ntl::nt::status::timeout)
          return pred();
      }
      return true;
    }

    native_handle_type native_handle()
    {
      return this;
    }

  private:
    condition_variable(const condition_variable&) __deleted;
    condition_variable& operator=(const condition_variable&) __deleted;

  private:
    volatile unsigned waiters;
  };



  /**
   *	@brief Class condition_variable_any [30.4.2 thread.condition.condvarany]
   *
   *  Class condition_variable_any provides a general condition variable that can wait on user-supplied lock types.
   **/
  class condition_variable_any
  {
  public:
    condition_variable_any();
    ~condition_variable_any();

    void notify_one();
    void notify_all();
    
    template <class Lock>
    void wait(Lock& lock);
    
    template <class Lock, class Predicate>
    void wait(Lock& lock, Predicate pred);
    
    template <class Lock, class Clock, class Duration>
    cv_status wait_until(Lock& lock, const chrono::time_point<Clock, Duration>& abs_time);
    
    template <class Lock, class Clock, class Duration, class Predicate>
    bool wait_until(Lock& lock, const chrono::time_point<Clock, Duration>& abs_time, Predicate pred);
    
    template <class Lock, class Rep, class Period>
    cv_status wait_for(Lock& lock, const chrono::duration<Rep, Period>& rel_time);
    
    template <class Lock, class Rep, class Period, class Predicate>
    bool wait_for(Lock& lock, const chrono::duration<Rep, Period>& rel_time, Predicate pred);
    
    typedef uintptr_t native_handle_type; // See 30.1.3
    native_handle_type native_handle(); // See 30.1.3

  private:
    condition_variable_any(const condition_variable_any&) __deleted;
    condition_variable_any& operator=(const condition_variable_any&) __deleted;
  };

  /** @} thread_condition */
  /** @} threads */
} // std
#endif // NTL__STLX_CONDVARIABLE
