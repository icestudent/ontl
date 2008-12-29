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
   *	@addtogroup thread Thread support library [thread]
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

  /// Class mutex [30.3.1.1 thread.mutex.class]
  typedef ntl::nt::base_mutex<false> mutex;

  /// Class recursive_mutex [30.3.1.2 thread.mutex.recursive]
  typedef ntl::nt::base_mutex<true> recursive_mutex;
#endif
  
  /// Class timed_mutex [30.3.2.1 thread.timedmutex.class]
  class timed_mutex:
    public __::base_mutex<false>
  {
  public:
    timed_mutex()
    {}
    ~timed_mutex()
    {}

    template <class Rep, class Period>
    bool try_lock_for(const chrono::duration<Rep, Period>& rel_time) __ntl_nothrow
    {
      if(thread_locked()) // throw system_error(resource_deadlock_would_occur)
        assert(!"Current thread already owns the mutex.");
      const ntl::nt::system_duration period = chrono::duration_cast<ntl::nt::system_duration>(rel_time);
      return period.count() <= 0 ? try_lock() : ntl::nt::success( wait( -1i64*period.count(), true ) );
    }

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


  /// Class recursive_timed_mutex [30.3.2.2 thread.timedmutex.recursive]
  class recursive_timed_mutex:
    __::base_mutex<true>
  {
  public:
    recursive_timed_mutex()
    {}
    ~recursive_timed_mutex()
    {}

    template <class Rep, class Period>
    bool try_lock_for(const chrono::duration<Rep, Period>& rel_time) __ntl_nothrow
    {
      const ntl::nt::system_duration period = duration_cast<system_duration>(rel_time);
      return period.count() <= 0 ? try_lock() : ntl::nt::success( wait( -1i64*period.count(), true ) );
    }

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


  /// Class template lock_guard [30.3.3.1 thread.lock.guard]
  template <class Mutex>
  class lock_guard 
  {
  public:
    typedef Mutex mutex_type;

    explicit lock_guard(mutex_type& m)
      :m(m)
    {
      m.lock();
    }

    lock_guard(mutex_type& m, adopt_lock_t) __ntl_nothrow
      :m(m)
    {}

    ~lock_guard() __ntl_nothrow
    {
      m.unlock();
    }
    
  private:
    mutex_type& m;

    lock_guard(lock_guard const&) __deleted;
    lock_guard& operator=(lock_guard const&) __deleted;
  };


  /// Class template unique_lock [30.3.3.2 thread.lock.unique]
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

  /// Call once [30.3.5 thread.once]
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
     *  The wait implemented through a simple cycle with the short thread sleep. 
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

  /** @} thread */
} // std

#endif // NTL__STLX_MUTEX
