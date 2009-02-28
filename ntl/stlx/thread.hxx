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

  /**
   *	@brief thread class
   *
   *  The class thread provides a mechanism to create a new thread of execution, to join with a thread (i.e. wait
   *  for a thread to complete), and to perform other operations that manage and query the state of a thread.
   *  
   *  A thread object uniquely represents a particular thread of execution. That representation may be transferred
   *  to other thread objects in such a way that no two thread objects simultaneously represent the same thread
   *  of execution. A thread of execution is detached when no thread object represents that thread. Objects of
   *  class thread can be in a state that does not represent a thread of execution. 
   *
   *  @note A thread object does not represent a thread of execution after default construction, after being moved from, 
   *  or after a successful call to detach or join.
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
    typedef ntl::nt::legacy_handle native_handle_type;

    // construct/copy/destroy:
    thread()
      :h(),tid(),end(),cleanup()
    {}
    ~thread()
    {
      if(joinable())
        detach();
    }

    template <class F> explicit thread(F f);

  #ifdef NTL__CXX_VT
    template <class F, class ...Args> thread(F&& f, Args&&... args);
  #endif

  #ifdef NTL__CXX_RV
    thread(thread&&);
    thread& operator=(thread&&);
    void swap(thread&&) __ntl_nothrow;
  #else
    thread(thread&);
    thread& operator=(thread&);

    // members:
    void swap(thread&) __ntl_nothrow;
  #endif

    bool joinable() const __ntl_nothrow;

    void join(error_code& ec = throws()) __ntl_throws(system_error);
    void detach(error_code& ec = throws()) __ntl_throws(system_error);

    id get_id() const __ntl_nothrow;

    native_handle_type native_handle() const __ntl_nothrow { return h; }
    
    // static members:
    static unsigned hardware_concurrency()
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

  private:
    volatile native_handle_type h, tid, end;
    volatile uint32_t cleanup; // true if cleanup required
  };

  namespace this_thread
  {
    thread::id get_id();
  }

  /**
   *	@brief Unique thread identifier
   *
   *  An object of type thread::id provides a unique identifier for each thread of execution and a single distinct
   *  value for all thread objects that do not represent a thread of execution (30.2.1).
   *
   *  Each thread of execution has an associated thread::id object that is not equal to the thread::id object of any other thread of execution
   *  and that is not equal to the thread::id object of any std::thread object that does not represent threads
   *  of execution. 
   *
   *  The library may reuse the value of a thread::id of a terminated thread that can no longer be joined.
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
    /** Returns an object of type thread::id that uniquely identifies the current thread of execution. */
    inline thread::id get_id() __ntl_nothrow
    {
      return ntl::nt::teb::instance().ClientId.UniqueThread;
    }

    /** Offers the operating system the opportunity to schedule another thread. */
    inline void yield() __ntl_nothrow
    {
      // NOTE: i'm not sure about this
      ntl::nt::sleep(0, true);
    }

    /** Blocks the calling thread at least until the time specified by abs_time. */
    template <class Clock, class Duration>
    inline void sleep_until(const chrono::time_point<Clock, Duration>& abs_time)
    {
      ntl::nt::sleep_until(abs_time);
    }

    /** Blocks the calling thread for at least the time specified by rel_time. */
    template <class Rep, class Period>
    inline void sleep_for(const chrono::duration<Rep, Period>& rel_time)
    {
      ntl::nt::sleep_for(rel_time);
    }
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
    return h != 0;
  }

  inline void thread::join(error_code& ec) __ntl_throws(system_error)
  {
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
    h = 0;
  }

  inline void thread::detach(error_code& ec) __ntl_throws(system_error)
  {
    if(!h || !tid){
      ec = h ? posix_error::make_error_code(posix_error::no_such_process) :posix_error::make_error_code(posix_error::invalid_argument);
      if(ec == throws())
        __ntl_throw(system_error(ec));
      return;
    }
    ntl::atomic::increment(cleanup);
  }

  /** @} thread_thread */
  /** @} threads */
} // std

#endif // NTL__STLX_THREAD
