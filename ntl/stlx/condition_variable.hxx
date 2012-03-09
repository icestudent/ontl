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

namespace std
{
  /**
   *	@addtogroup threads 30 Thread support library [thread]
   *  @{
   *
   *	@defgroup thread_condition 30.4 Condition variables [thread.condition]
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
   *	@brief Class condition_variable [30.4.1 thread.condition.condvar]
   *
   *  Class condition_variable provides a condition variable that can only wait on a Lock, allowing maximum effciency on some platforms. 
   **/
  class condition_variable 
  {
  public:
    condition_variable();
    ~condition_variable();
    

    void notify_one();
    void notify_all();
    
    void wait(unique_lock<mutex>& lock);
    
    template <class Predicate>
    void wait(unique_lock<mutex>& lock, Predicate pred);
    
    template <class Clock, class Duration>
    cv_status wait_until(unique_lock<mutex>& lock, const chrono::time_point<Clock, Duration>& abs_time);

    template <class Clock, class Duration, class Predicate>
    bool wait_until(unique_lock<mutex>& lock, const chrono::time_point<Clock, Duration>& abs_time, Predicate pred);

    template <class Rep, class Period>
    cv_status wait_for(unique_lock<mutex>& lock, const chrono::duration<Rep, Period>& rel_time);

    template <class Rep, class Period, class Predicate>
    bool wait_for(unique_lock<mutex>& lock, const chrono::duration<Rep, Period>& rel_time, Predicate pred);

    typedef uintptr_t native_handle_type; // See 30.1.3
    native_handle_type native_handle(); // See 30.1.3

  private:
    condition_variable(const condition_variable&) __deleted;
    condition_variable& operator=(const condition_variable&) __deleted;
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
