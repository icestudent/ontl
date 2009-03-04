/**\file*********************************************************************
 *                                                                     \brief
 *  30.3 Mutual exclusion [thread.mutex]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_MUTEX
#define NTL__STLX_MUTEX

#include "chrono.hxx"
#include "system_error.hxx"

#ifndef NTL__SUBSYSTEM_KM
# include "../nt/mutex.hxx"
#endif

namespace std
{
  /**
   *	@addtogroup threads 30 Thread support library [thread]
   *  @{
   **/
  /**
   *	@defgroup thread_mutex 30.3 Mutual exclusion [thread.mutex]
   *
   *  A mutex object facilitates protection against data races and allows thread-safe synchronization of data
   *  between threads. A %thread \i owns a mutex from the time it successfully calls one of the lock functions until
   *  it calls unlock. Mutexes may be either recursive or non-recursive, and may grant simultaneous ownership
   *  to one or many threads. The mutex types supplied by the standard library provide exclusive ownership
   *  semantics: only one %thread may own the mutex at a time. Both recursive and non-recursive mutexes are
   *  supplied.
   *
   *  @{
   **/

  /// do not acquire ownership of the mutex
  struct defer_lock_t   {};
  /// try to acquire ownership of the mutex without blocking
  struct try_to_lock_t  {};
  /// assume the calling thread has already obtained mutex ownership and manage it
  struct adopt_lock_t   {};
  
#ifndef __BCPLUSPLUS__
  __declspec(selectany) extern const defer_lock_t defer_lock = {};
  __declspec(selectany) extern const try_to_lock_t try_to_lock = {};
  __declspec(selectany) extern const adopt_lock_t adopt_lock = {};
#else
  __declspec(selectany) extern const defer_lock_t defer_lock;
  __declspec(selectany) extern const try_to_lock_t try_to_lock;
  __declspec(selectany) extern const adopt_lock_t adopt_lock;
#endif

#ifndef NTL__SUBSYSTEM_KM
  namespace __
  {
    namespace mtx
    {
      using ntl::nt::base_mutex;
    }
  }
#endif

  /**
   *	@brief Class mutex [30.3.1.1 thread.mutex.class]
   *
   *  The class mutex provides a non-recursive mutex with exclusive ownership semantics. If one %thread owns a
   *  mutex object, attempts by another %thread to acquire ownership of that object will fail (for \c try_lock()) or
   *  block (for \c lock()) until the owning %thread has released ownership with a call to \c unlock().
   **/
  typedef __::mtx::base_mutex<false> mutex;

  /**
   *	@brief Class recursive_mutex [30.3.1.2 thread.mutex.recursive]
   *
   *  The class recursive_mutex provides a recursive mutex with exclusive ownership semantics. If one %thread
   *  owns a recursive_mutex object, attempts by another %thread to acquire ownership of that object will fail
   *  (for \c try_lock()) or block (for \c lock()) until the first %thread has completely released ownership.
   **/
  typedef __::mtx::base_mutex<true> recursive_mutex;

  /**
   *	@brief Class timed_mutex [30.3.2.1 thread.timedmutex.class]
   *
   *  The class timed_mutex provides a non-recursive mutex with exclusive ownership semantics. If one %thread
   *  owns a timed_mutex object, attempts by another %thread to acquire ownership of that object will fail (for
   *  \c try_lock()) or block (for \c lock(), \c try_lock_for(), and \c try_lock_until()) until the owning %thread has
   *  released ownership with a call to \c unlock() or the call to \c try_lock_for() or \c try_lock_until() times out
   *  (having failed to obtain ownership).
   **/
  class timed_mutex:
    public __::mtx::base_mutex<false>
  {
  public:
    timed_mutex()
    {}
    ~timed_mutex()
    {}

    /**
     *	@brief The function attempts to obtain ownership of the mutex within the time specified by \c rel_time.
     *
     *  If the time specified by \c rel_time is less than or equal to 0, the function attempts to obtain ownership
     *  without blocking (as if by calling \c try_lock()). The function shall return within the time specified by
     *  rel_time only if it has obtained ownership of the mutex object.
     **/
    template <class Rep, class Period>
    bool try_lock_for(const chrono::duration<Rep, Period>& rel_time) __ntl_nothrow
    {
      if(thread_locked()) // throw system_error(resource_deadlock_would_occur)
        assert(!"Current thread already owns the mutex.");
      const ntl::nt::system_duration period = chrono::duration_cast<ntl::nt::system_duration>(rel_time);
      return period.count() <= 0 ? try_lock() : ntl::nt::success( wait( -1i64*period.count(), true ) );
    }

    /**
     *	@brief The function attempts to obtain ownership of the mutex by the time specified by \c abs_time.
     *
     *  If \c abs_time has already passed, the function attempts to obtain ownership without blocking (as if
     *  by calling \c try_lock()). The function shall return before the time specified by \c abs_time only if it
     *  has obtained ownership of the mutex object.
     **/
    template <class Clock, class Duration>
    bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time) __ntl_nothrow
    {
      if(thread_locked()) // throw system_error(resource_deadlock_would_occur)
        assert(!"Current thread already owns the mutex.");
      const ntl::nt::system_duration period = chrono::duration_cast<ntl::nt::system_duration>(abs_time.time_since_epoch());
      return period.count() == 0 ? try_lock() : ntl::nt::success( wait(period.count(), true) );
    }

  private:
    timed_mutex(const timed_mutex&) __deleted;
    timed_mutex& operator=(const timed_mutex&) __deleted;
  };


  /**
   *	@brief Class recursive_timed_mutex [30.3.2.2 thread.timedmutex.recursive]
   *
   *  The class recursive_timed_mutex provides a recursive mutex with exclusive ownership semantics. If one
   *  %thread owns a recursive_timed_mutex object, attempts by another %thread to acquire ownership of that
   *  object will fail (for \c try_lock()) or block (for \c lock(), \c try_lock_for(), and \c try_lock_until()) until the
   *  owning %thread has completely released ownership or the call to \c try_lock_for() or \c try_lock_until() times
   *  out (having failed to obtain ownership).
   **/
  class recursive_timed_mutex:
    __::mtx::base_mutex<true>
  {
  public:
    recursive_timed_mutex()
    {}
    ~recursive_timed_mutex()
    {}

    /**
     *	@brief The function attempts to obtain ownership of the mutex within the time specified by \c rel_time.
     *
     *  If the time specified by \c rel_time is less than or equal to 0, the function attempts to obtain ownership
     *  without blocking (as if by calling \c try_lock()). The function shall return within the time specified by
     *  rel_time only if it has obtained ownership of the mutex object.
     **/
    template <class Rep, class Period>
    bool try_lock_for(const chrono::duration<Rep, Period>& rel_time) __ntl_nothrow
    {
      const ntl::nt::system_duration period = duration_cast<system_duration>(rel_time);
      return period.count() <= 0 ? try_lock() : ntl::nt::success( wait( -1i64*period.count(), true ) );
    }

    /**
     *	@brief The function attempts to obtain ownership of the mutex by the time specified by \c abs_time.
     *
     *  If \c abs_time has already passed, the function attempts to obtain ownership without blocking (as if
     *  by calling \c try_lock()). The function shall return before the time specified by \c abs_time only if it
     *  has obtained ownership of the mutex object.
     **/
    template <class Clock, class Duration>
    bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time) __ntl_nothrow
    {
      const ntl::nt::system_duration period = duration_cast<system_duration>(rel_time);
      return period.count() == 0 ? try_lock() : ntl::nt::success( wait(period.count()), true );
    }

  private:
    recursive_timed_mutex(const recursive_timed_mutex&) __deleted;
    recursive_timed_mutex& operator=(const recursive_timed_mutex&) __deleted;
  };


  /**
   *	@brief Class template lock_guard [30.3.3.1 thread.lock.guard]
   *
   *  An object of type lock_guard controls the ownership of a mutex object within a scope. A lock_guard
   *  object maintains ownership of a mutex object throughout the lock_guard object's lifetime. The behavior
   *  of a program is undefined if the mutex referenced by pm does not exist for the entire lifetime (3.8) of the
   *  lock_guard object.
   **/
  template <class Mutex>
  class lock_guard 
  {
  public:
    typedef Mutex mutex_type;

    /** Locks the mutex \c m */
    explicit lock_guard(mutex_type& m)
      :m(m)
    {
      m.lock();
    }

    /** Takes the mutex \m without locking it */
    lock_guard(mutex_type& m, adopt_lock_t) __ntl_nothrow
      :m(m)
    {}

    /** Unlocks the mutex */
    ~lock_guard() __ntl_nothrow
    {
      m.unlock();
    }
    
  private:
    mutex_type& m;

    lock_guard(lock_guard const&) __deleted;
    lock_guard& operator=(lock_guard const&) __deleted;
  };


  /**
   *	@brief Class template unique_lock [30.3.3.2 thread.lock.unique]
   *
   *  An object of type unique_lock controls the ownership of a mutex within a scope. Mutex ownership may
   *  be acquired at construction or after construction, and may be transferred, after acquisition, to another
   *  unique_lock object. Objects of type unique_lock are not copyable but are movable. The behavior of a
   *  program is undefined if the contained pointer \c pm is not null and the mutex pointed to by \c pm does not exist
   *  for the entire remaining lifetime (3.8) of the unique_lock object.
   **/
  template <class Mutex>
  class unique_lock 
  {
    #ifndef NTL__CXX_EXPLICITOP
    typedef const Mutex* unspecified_bool_type;
    #endif
  public:
    typedef Mutex mutex_type;

    // 30.3.3.2.1 construct/copy/destroy
    unique_lock() __ntl_nothrow
      :m(), owns(false)
    {}

    explicit unique_lock(mutex_type& m)
      :m(&m), owns(true)
    {
      this->m->lock();
    }

    unique_lock(mutex_type& m, defer_lock_t) __ntl_nothrow
      :m(&m), owns(false)
    {}

    unique_lock(mutex_type& m, try_to_lock_t) __ntl_nothrow
      :m(&m)
    {
      owns = this->m->try_lock();
    }

    unique_lock(mutex_type& m, adopt_lock_t) __ntl_nothrow
      :m(&m), owns(true)
    {}

    template <class Clock, class Duration>
    unique_lock(mutex_type& m, const chrono::time_point<Clock, Duration>& abs_time) __ntl_nothrow
      :m(&m)
    {
      owns = this->m->try_lock_until(abs_time);
    }
    
    template <class Rep, class Period>
    unique_lock(mutex_type& m, const chrono::duration<Rep, Period>& rel_time) __ntl_nothrow
      :m(&m)
    {
      owns = this->m->try_lock_for(rel_time);
    }
    
    ~unique_lock() __ntl_nothrow
    {
      if(owns)
        m->unlock();
    }
    
    unique_lock(unique_lock&& u) __ntl_nothrow
      :m(), owns(false)
    {
      swap(u);
    }

    unique_lock& operator=(unique_lock&& u) __ntl_nothrow
    {
      if(owns)
        m->unlock();
      m = u.m,
        owns = u.owns;
      u.m = nullptr,
        u.owns = false;
      return *this;
    }
    
    // 30.3.3.2.2 locking
    void lock() __ntl_throws(system_error)
    {
      // TODO: throw operation_not_permitted — if pm is null.
      if(m){
        m->lock();
        owns = true;
      }
    }

    bool try_lock() __ntl_throws(system_error)
    {
      // TODO: throw operation_not_permitted — if pm is null.
      return m ? (owns = m->try_lock()) : false;
    }

    template <class Rep, class Period>
    bool try_lock_for(const chrono::duration<Rep, Period>& rel_time) __ntl_throws(system_error)
    {
      return m ? (owns = m->try_lock_for(rel_time)) : false;
    }
    
    template <class Clock, class Duration>
    bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time) __ntl_throws(system_error)
    {
      return m ? (owns = m->try_lock_until(abs_time)) : false;
    }
    
    void unlock() __ntl_throws(system_error)
    {
      // TODO: throw operation_not_permitted — if owns if false.
      if(m)
        m->unlock();
      owns = false;
    }
    
    // 30.3.3.2.3 modifiers
    void swap(unique_lock&& u) __ntl_nothrow
    {
      std::swap(m, u.m);
      std::swap(owns, u.owns);
    }

    mutex_type *release() __ntl_nothrow
    {
      mutex_type* pm = m;
      m = nullptr;
      owns = false;
      return pm;
    }

    // 30.3.3.2.4 observers
    bool owns_lock() const __ntl_nothrow    { return owns; }
    mutex_type* mutex() const __ntl_nothrow { return m;    }

    #ifdef NTL__CXX_EXPLICITOP
    explicit operator bool () const;
    #else
    operator unspecified_bool_type() const __ntl_nothrow { return (unspecified_bool_type)owns; }
    #endif

  private:
    mutex_type *m;
    bool owns;

    unique_lock(unique_lock const&) __deleted;
    unique_lock& operator=(unique_lock const&) __deleted;
  };


  template <class Mutex>
  void swap(unique_lock<Mutex>& x, unique_lock<Mutex>& y)  { x.swap(y); }
#ifdef NTL__CXX_RV
  template <class Mutex>
  void swap(unique_lock<Mutex>&& x, unique_lock<Mutex>& y) { x.swap(y); }
  template <class Mutex>
  void swap(unique_lock<Mutex>& x, unique_lock<Mutex>&& y) { x.swap(y); }
#endif


#ifdef NTL__CXX_VT
  template <class L1, class L2, class... L3> 
  int try_lock(L1&, L2&, L3&...);
  template <class L1, class L2, class... L3> 
  void lock(L1&, L2&, L3&...);
#else
  template <class L1, class L2, class L3> 
  int try_lock(L1& m1, L2& m2, L3& m3)
  {
    if(!m1.try_lock()){
      return 0;
    }else if(!m2.try_lock()){
      m1.unlock();
      return 1;
    }else if(!m3.try_lock()){
      m2.unlock();
      m1.unlock();
      return 2;
    }else
      return -1;
  }

  template <class L1, class L2, class L3> 
  void lock(L1& m1, L2& m2, L3& m3)
  {
    if(!m1.lock()){
      return 0;
    }else if(!m2.lock()){
      m1.unlock();
      return 1;
    }else if(!m3.lock()){
      m2.unlock();
      m1.unlock();
      return 2;
    }else
      return -1;
  }

#endif

  //////////////////////////////////////////////////////////////////////////
#pragma region thread.once

  /**
   *	@brief Call once [30.3.5 thread.once]
   *
   *  The class once_flag is an opaque data structure that call_once uses to initialize data without causing a data race or deadlock.
   **/
  struct once_flag 
  {
    /** Constructs an object of type once_flag */
    constexpr once_flag()
      :inited_(false), locked_(unlocked)
    {}

    struct lock
    {
      inline
      lock(once_flag& flag)
        :flag_(flag)
      {
        flag_.enter();
      }
      inline 
      ~lock()
      {
        flag_.exit();
      }
    private:
      once_flag& flag_;
    };

    inline uint32_t ready() const volatile { return inited_; }
    inline void set_ready()
    {
      inited_ = true;
    }

  protected:
    enum state_t { unlocked, locked };
    /**
     *	@brief Takes or waits for the object's ownership.
     *
     *  The wait implemented through a simple cycle with the short %thread sleep. 
     *  Such wait takes about 60 times more CPU time than kernel synchronization object,
     *  but it's still invisible to the common system performance and it is faster and lightweight.
     **/
    void enter()
    {
      while(ntl::atomic::compare_exchange(locked_, locked, unlocked) == locked)
        ntl::nt::NtDelayExecution(false, -1);
    }
    /** Releases object's ownership */
    inline void exit()
    {
      ntl::atomic::exchange(locked_, unlocked);
    }
  private:
    // native platform types (faster)
    volatile uint32_t locked_;
    volatile uint32_t inited_;

    once_flag(const once_flag&) __deleted;
    once_flag& operator=(const once_flag&) __deleted;
  };

#ifdef NTL__CXX_VT
  template<class Callable, class ...Args>
  void call_once(once_flag& flag, Callable func, Args&&... args);
#else

  // because we still don't have a variadic templates, we have to write all this code :-\ 
  namespace __
  {
    template<bool member_function>
    struct call_once_impl
    {
      template<class Callable, class Arg1>
      static inline void call_once(Callable func, Arg1 arg1)
      {
        func(arg1);
      }
      template<class Callable, class Arg1, class Arg2>
      static inline void call_once(Callable func, Arg1 arg1, Arg2 arg2)
      {
        func(arg1, arg2);
      }
      template<class Callable, class Arg1, class Arg2, class Arg3>
      static inline void call_once(Callable func, Arg1 arg1, Arg2 arg2, Arg3 arg3)
      {
        func(arg1, arg2, arg3);
      }
    };
    template<>
    struct call_once_impl<true>
    {
      template<class Callable, class T>
      static inline void call_once(Callable mem_fun, T* obj)
      {
        (obj->*mem_fun)();
      }
      template<class Callable, class T>
      static inline void call_once(Callable mem_fun, T obj)
      {
        (obj.*mem_fun)();
      }
      template<class Callable, class T, class Arg1>
      static inline void call_once(Callable mem_fun, T* obj, Arg1 arg1)
      {
        (obj->*mem_fun)(arg1);
      }
      template<class Callable, class T, class Arg1>
      static inline void call_once(Callable mem_fun, T obj, Arg1 arg1)
      {
        (obj.*mem_fun)(arg1);
      }
      template<class Callable, class T, class Arg1, class Arg2>
      static inline void call_once(Callable mem_fun, T* obj, Arg1 arg1, Arg2 arg2)
      {
        (obj->*mem_fun)(arg1, arg2);
      }
      template<class Callable, class T, class Arg1, class Arg2>
      static inline void call_once(Callable mem_fun, T obj, Arg1 arg1, Arg2 arg2)
      {
        (obj.*mem_fun)(arg1, arg2);
      }
    };
  }

  // 30.3.5.2 Function call_once [thread.once.callonce]
  template<class Callable>
  void call_once(once_flag& flag, Callable func)
  {
    static_assert(std::is_member_function_pointer<Callable>::value == false, "must not be a member pointer function");
    if(flag.ready())
      return;

    once_flag::lock lock(flag);
    if(!flag.ready()){
      func();
      flag.set_ready();
    }
  }

  template<class Callable, class Arg1>
  void call_once(once_flag& flag, Callable func, Arg1 arg1)
  {
    if(flag.ready())
      return;

    once_flag::lock lock(flag);
    if(!flag.ready()){
      __::call_once_impl<std::is_member_function_pointer<Callable>::value>::call_once(func, arg1);
      flag.set_ready();
    }
  }

  template<class Callable, class Arg1, class Arg2>
  void call_once(once_flag& flag, Callable func, Arg1 arg1, Arg2 arg2)
  {
    if(flag.ready())
      return;

    once_flag::lock lock(flag);
    if(!flag.ready()){
      __::call_once_impl<std::is_member_function_pointer<Callable>::value>::call_once(func, arg1, arg2);
      flag.set_ready();
    }
  }

  template<class Callable, class Arg1, class Arg2, class Arg3>
  void call_once(once_flag& flag, Callable func, Arg1 arg1, Arg2 arg2, Arg1 arg3)
  {
    if(flag.ready())
      return;

    once_flag::lock lock(flag);
    if(!flag.ready()){
      __::call_once_impl<std::is_member_function_pointer<Callable>::value>::call_once(func, arg1, arg2, arg3);
      flag.set_ready();
    }
  }


#endif
#pragma endregion

  /** @} thread_mutex */
  /** @} threads */
} // std

#endif // NTL__STLX_MUTEX
