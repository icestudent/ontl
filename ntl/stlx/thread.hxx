/**\file*********************************************************************
 *                                                                     \brief
 *  30.2 Threads [thread.threads]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_THREAD
#define NTL__STLX_THREAD

#include "chrono.hxx"
#include "system_error.hxx"

#ifndef NTL__SUBSYSTEM_KM
  // UM implementation
# include "../nt/teb.hxx"
# include "../nt/system_information.hxx"
# include "../nt/new.hxx"

// replace this with corresponding headers!

# include "smart_ptr.hxx"
# include "function.hxx"
#else
# error Km unsupported currently
#endif

namespace std
{

  /**
   *	@defgroup threads 30 Thread support library [thread]
   *  
   *  The following subclauses describe components to create and manage threads (1.10), perform mutual exclusion, 
   *  and communicate conditions between threads.
   *
   *  @{
   **/
  /**
   *	@defgroup thread_thread 30.2 Threads [thread.threads]
   *  @{
   **/

  /// 30.2.2 Namespace this_thread [thread.thread.this]
  namespace this_thread
  {}

  namespace __
  {
    template<class>
    struct thread_cleanup
    {

    };

    struct thread_params_base
    {
      volatile uint32_t cleanup;
      ntl::nt::legacy_handle handle;

      thread_params_base()
        :cleanup(), handle()
      {}
      virtual ~thread_params_base(){}
      virtual void run() = 0;
    };

    template<class F, class Args = tuple<> >
    struct thread_params: thread_params_base
    {
      std::func::v1::function<void, Args> fn;
      Args args;

      explicit thread_params(F f, const Args& a)
        :fn(f), args(a)
      {}

      void run()
      {
        fn(args);
      }
    };
  }

  /**
   *	@brief thread class
   *
   *  The class thread provides a mechanism to create a new %thread of execution, to join with a %thread (i.e. wait
   *  for a %thread to complete), and to perform other operations that manage and query the state of a %thread.
   *  
   *  A thread object uniquely represents a particular %thread of execution. That representation may be transferred
   *  to other thread objects in such a way that no two thread objects simultaneously represent the same %thread
   *  of execution. A %thread of execution is detached when no thread object represents that %thread. Objects of
   *  class thread can be in a state that does not represent a %thread of execution. 
   *
   *  @note A thread object does not represent a %thread of execution after default construction, after being moved from, 
   *  or after a successful call to \c thread::detach or \c thread::join.
   **/
  class thread
  {
#ifdef NTL__CXX_RV
    thread(const thread&) __deleted;
    thread& operator=(const thread&) __deleted;
#endif
  public:
    // types:
    class id;
    /// Platform-specific %thread handle
    typedef ntl::nt::legacy_handle native_handle_type;

    /** Constructs a thread object that does not represent a %thread of execution. */
    thread()__ntl_nothrow
      :h(),tid(),cleanup()
    {}
    /**
     *	@brief Detaches from the thread if it joinable.
     *  @note Destroying a joinable thread can be unsafe if the %thread accesses objects or the standard library 
     *  unless the %thread performs explicit synchronization to ensure that it does not access the objects or the standard library 
     *  past their respective lifetimes. Terminating the process with \c _exit() or \c quick_exit() removes some of these obligations.
     **/
    ~thread()
    {
      if(joinable())
        detach();
    }

    /**
     *	@brief Constructs an object and executes \c f in a new %thread of execution.
     *  @param[in] f Callable object that will be called in the new %thread context
     *  @note Return value of provided callable object is ignored.
     *  @note If \c f terminates with an uncaught exception, \c std::terminate() shall be called.
     **/
    template <class F> explicit thread(F f);

  #ifdef NTL__CXX_VT
    template <class F, class ...Args> thread(F&& f, Args&&... args);
  #else
    template <class F, class A1> thread(F f, A1 a1);
    template <class F, class A1, class A2> thread(F f, A1 a1, A2 a2);
  #endif

  #ifdef NTL__CXX_RV
    thread(thread&&);
    thread& operator=(thread&&);
    void swap(thread&&) __ntl_nothrow;
  #else
    thread(const thread& r);
    thread& operator=(const thread& r);

    /** Swaps the current object's state with \c r */
    void swap(const thread& r) __ntl_nothrow;
  #endif

    /** Is this thread a joinable */
    bool joinable() const __ntl_nothrow;

    /** Waits for a thread to complete */
    void join(error_code& ec = throws()) __ntl_throws(system_error);
    /** Detaches from a thread of execution */
    void detach(error_code& ec = throws()) __ntl_throws(system_error);

    /** Returns a %thread id if object represents a valid %thread of execution or default constructed id otherwise */
    id get_id() const __ntl_nothrow;

    /** Returns the platform-specific %thread handle */
    native_handle_type native_handle() const __ntl_nothrow { return h; }
    
    /** Returns the number of hardware %thread contexts. */
    static unsigned hardware_concurrency();

  protected:
    static uint32_t __stdcall start_routine(void* Parm);
    bool alive() const __ntl_nothrow;
    bool check_alive() __ntl_nothrow;
    void start(__::thread_params_base* tp);

  private:
    mutable native_handle_type h, tid;
    mutable volatile uint32_t* cleanup; // true if cleanup required
  };

  namespace this_thread
  {
    thread::id get_id() __ntl_nothrow;
  }

  /**
   *	@brief Unique thread identifier
   *
   *  An object of type thread::id provides a unique identifier for each %thread of execution and a single distinct
   *  value for all thread objects that do not represent a %thread of execution (30.2.1).
   *
   *  Each %thread of execution has an associated thread::id object that is not equal to the thread::id object of any other %thread of execution
   *  and that is not equal to the thread::id object of any std::thread object that does not represent threads of execution. 
   *
   *  The library may reuse the value of a thread::id of a terminated %thread that can no longer be joined.
   *  @note Relational operators allow thread::id objects to be used as keys in associative containers.
   **/
  class thread::id
  {
  public:
    /** Constructs an object of type id */
    id():tid_()
    {}

    friend bool operator==(id x, id y) { return x.tid_ == y.tid_; }
    friend bool operator!=(id x, id y) { return x.tid_ != y.tid_; }
    friend bool operator< (id x, id y) { return x.tid_ <  y.tid_; }
    friend bool operator<=(id x, id y) { return x.tid_ <= y.tid_; }
    friend bool operator> (id x, id y) { return x.tid_ >  y.tid_; }
    friend bool operator>=(id x, id y) { return x.tid_ >= y.tid_; }

    template<class charT, class traits>
    friend basic_ostream<charT, traits>& operator<< (basic_ostream<charT, traits>& out, thread::id id)
    {
      return out << reinterpret_cast<uint32_t>(id.tid_);
    }
  protected:
    id(native_handle_type tid)
      :tid_(tid)
    {}

    friend class thread;
    friend id this_thread::get_id();
  private:
    native_handle_type tid_;
  };
  
  /** Swaps the states of \c x and \c y */
  inline void swap(thread& x, thread& y)  __ntl_nothrow { x.swap(y); }
#ifdef NTL__CXX_RV
  inline void swap(thread&& x, thread& y) __ntl_nothrow { y.swap(x); }
  inline void swap(thread& x, thread&& y) __ntl_nothrow { x.swap(y); }
#endif


  namespace this_thread
  {
    /** Returns an object of type thread::id that uniquely identifies the current %thread of execution. */
    inline thread::id get_id() __ntl_nothrow
    {
      return ntl::nt::teb::instance().ClientId.UniqueThread;
    }

    /** Offers the operating system the opportunity to schedule another %thread. */
    inline void yield() __ntl_nothrow
    {
      // NOTE: i'm not sure about this
      ntl::nt::sleep(0, true);
    }

    /** Blocks the calling %thread at least until the time specified by \c abs_time. */
    template <class Clock, class Duration>
    inline void sleep_until(const chrono::time_point<Clock, Duration>& abs_time)
    {
      ntl::nt::sleep_until(abs_time);
    }

    /** Blocks the calling %thread for at least the time specified by \c rel_time. */
    template <class Rep, class Period>
    inline void sleep_for(const chrono::duration<Rep, Period>& rel_time)
    {
      ntl::nt::sleep_for(rel_time);
    }
  }

  inline unsigned thread::hardware_concurrency()
  {
    static int threads = -1;
    if(threads == -1){
      threads = 0;

      using namespace ntl::nt;
      system_information<system_basic_information> basicInfo;
      if(basicInfo)
        threads = basicInfo->NumberOfProcessors;
    }
    return threads;
  }

  // thread implementation

  inline thread::id thread::get_id() const __ntl_nothrow
  {
    // if this isn't thread, default id; thread id otherwise
    return tid;
  }

  inline bool thread::joinable() const
  {
    // get_id() != id()
    return alive();
  }

  inline bool thread::check_alive() __ntl_nothrow
  {
    if(alive())
      return true;
    ntl::nt::close(h);
    h = nullptr;
    return false;
  }

  inline void thread::join(error_code& ec) __ntl_throws(system_error)
  {
    check_alive();
    if(!h || !tid || get_id() == this_thread::get_id()){
      if(!h){
        ec = posix_error::make_error_code(posix_error::invalid_argument);
      }else if(!tid){
        ec = posix_error::make_error_code(posix_error::no_such_process);
      }else{
        ec = posix_error::make_error_code(posix_error::resource_deadlock_would_occur);
      }
      if(ec == throws())
        __ntl_throw(system_error(ec));
      return;
    }
    using namespace ntl::nt;
    ntstatus st = NtWaitForSingleObject(h, true, infinite_timeout());
    if(st != status::wait_0){
      ec = posix_error::make_error_code(posix_error::no_such_process);
      if(ec == throws())
        __ntl_throw(system_error(ec));
      return;
    }
    // wait ok, release handle
    close(h);
    h = nullptr,
      cleanup = nullptr;
    tid = 0;
  }

  inline void thread::detach(error_code& ec) __ntl_throws(system_error)
  {
    // detect alive
    check_alive();
    if(!h || !tid){
      ec = h ? posix_error::make_error_code(posix_error::no_such_process) :posix_error::make_error_code(posix_error::invalid_argument);
      if(ec == throws())
        __ntl_throw(system_error(ec));
      return;
    }
    ntl::atomic::increment(*cleanup);
    h = nullptr;
    tid = 0;
  }

  // noinline :-\..
  inline uint32_t __stdcall thread::start_routine(void* Parm)
  {
    __::thread_params_base* tp = reinterpret_cast<__::thread_params_base*>(Parm);
    tp->run();
    if(ntl::atomic::compare_exchange(tp->cleanup, 0, 1)){
      // close handle
      ntl::nt::close(tp->handle);
    }
    delete tp;
    return 0;
  }

  inline bool thread::alive() const __ntl_nothrow
  {
    //return ntl::nt::NtWaitForSingleObject(h, false, 0) == ntl:nt::status::still_active;
    return ntl::nt::user_thread::exitstatus(h) == ntl::nt::status::still_active; // may be faster
  }

#ifdef NTL__CXX_RV
  thread(thread&&);
  thread& operator=(thread&&);
  void swap(thread&&) __ntl_nothrow;
#else
  inline thread::thread(const thread& r)
    :h(), tid(),cleanup()
  {
    swap(r);
  }

  inline thread& thread::operator=(const thread& r) __ntl_nothrow
  {
    error_code ec;
    if(joinable())
      detach(ec);
    swap(r);
  }

  // members:
  inline void thread::swap(const thread& r) __ntl_nothrow
  {
    volatile native_handle_type h0 = h, t0 = tid;
    volatile uint32_t* c0 = cleanup;
    h = r.h,
      tid = r.tid,
      cleanup = r.cleanup;
    r.h = h0,
      r.tid = t0,
      r.cleanup = c0;
  }
#endif

  inline void thread::start(__::thread_params_base* tp)
  {
    cleanup = &tp->cleanup;
    ntl::nt::user_thread ut(start_routine, tp, true); // suspended
    tp->handle = ut.get();
    tid = ut.get_id();

    // run
    ut.resume();
    h = ut.release();
  }

  //////////////////////////////////////////////////////////////////////////
  // thread construction

  template<class F>
  inline thread::thread(F f)
    :h(),tid(),cleanup()
  {
    // create
    __::thread_params<F>* tp = new __::thread_params<F>(f, make_tuple());
    start(tp);
  }

  template <class F, class A1>
  inline thread::thread(F f, A1 a1)
    :h(),tid(),cleanup()
  {
    typedef __::thread_params<F, tuple<A1> > tparams;
    tparams* tp = new tparams(f, std::move(make_tuple(a1)));
    start(tp);
  }

  template <class F, class A1, class A2>
  inline thread::thread(F f, A1 a1, A2 a2)
    :h(),tid(),cleanup()
  {
    typedef __::thread_params<F, tuple<A1,A2> > tparams;
    tparams* tp = new tparams(f, std::move(make_tuple(a1,a2)));
    start(tp);
  }


  //////////////////////////////////////////////////////////////////////////

  /** @} thread_thread */
  /** @} threads */
} // std

#endif // NTL__STLX_THREAD
