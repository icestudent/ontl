/**\file*********************************************************************
 *                                                                     \brief
 *  30.2 Thread support library [thread]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_THREAD
#define NTL__STLX_THREAD

#include "system_time.hxx"
#include "ostream.hxx"
#include "nt/teb.hxx"

namespace std {

  /**
   *	@group thread Thread support library
   *  @{
   **/

/// 30.2.1 thread.thread.class
class thread
{
//    thread(const thread&);// = delete;
//    thread& operator=(const thread&);// = delete;

  public:

    ///\name types:

    typedef ntl::nt::tib * native_handle_type; // See 30.1.3

    /// 30.2.1.1 thread.thread.id
    class id
    {
      public:
        id() : _() {}

        id(ntl::nt::tib* tib) : _(tib) {}

      friend bool operator==(id x, id y) { return x._ == y._; }
      friend bool operator!=(id x, id y) { return x._ != y._; }
      friend bool operator< (id x, id y) { return x._ <  y._; }
      friend bool operator<=(id x, id y) { return x._ <= y._; }
      friend bool operator> (id x, id y) { return x._ >  y._; }
      friend bool operator>=(id x, id y) { return x._ >= y._; }

      template<class charT, class traits>
      friend basic_ostream<charT, traits>&
        operator<<(basic_ostream<charT, traits>& out, id id);

      private: native_handle_type _;
      friend thread;

    };

    ///\name construct/copy/destroy:
    thread() {}

    template <class F> explicit thread(F f);

    //template <class F, class ...Args> thread(F&& f, Args&&... args);
    template <class F, class Arg1>
    inline
    thread(F& f, const Arg1& arg1);

    ~thread() { if ( joinable() ) detach(); }
    //thread(thread&);
    thread& operator=(thread&);

    ///\name members:
    void swap(thread& t);
    bool joinable() const { return get_id() != id(); }

    inline void join();

    void detach()
    {
      _Assert(joinable());
      tid._ = native_handle_type();
    }

    id get_id() const { return tid._; }
    native_handle_type native_handle() { return tid._; }

    ///\name static members:
    static unsigned hardware_concurrency();

    ///}

  private:

    volatile id tid;

};


void swap(thread& x, thread& y);
//void swap(thread&& x, thread& y);
//void swap(thread& x, thread&& y);

namespace this_thread {

  thread::id get_id()
  {
    return thread::id(&ntl::nt::teb::instance());
  }

  void yield() { ntl::yield(); }

  void sleep(const system_time& abs_time);
  template <class Duration>
  void sleep(const Duration& rel_time);

}

template <class F, class Arg1>
__forceinline
thread::thread(F& f, const Arg1& arg1)
: tid(reinterpret_cast<thread::native_handle_type>(-1))
{
  struct thread_thunk
  {
    thread & thr;
    F & f;
    const Arg1 & arg1;

    thread_thunk(thread & thr, F& f, const Arg1& arg1)
    : thr(thr), f(f), arg1(arg1) {}

    static void __stdcall
    thread_start_routine(void* param)
    {
      thread_thunk & p = *reinterpret_cast<thread_thunk*>(param);
      p.thr.tid._ = this_thread::get_id()._;
      p.f(p.arg1);
      p.thr.tid._ = native_handle_type();
    }
    private: void operator=(const thread_thunk&){}
  };

  thread_thunk param(*this, f, arg1);
  //_Assert(!this->joinable());

  {
    ntl::system_tread t(thread_thunk::thread_start_routine, &param);
    if ( !t.get() ) __ntl_throw (std::bad_alloc());
  }

  // wait till class instance is initialized by thread_start_routine
  while ( this->tid._ == reinterpret_cast<thread::native_handle_type>(-1) )
    this_thread::yield();

}

void thread::join()
{
  _Assert(joinable());
  while ( this->tid._ ) this_thread::yield();//sleep(1);
}

  /** @} thread */
} // std

#endif // NTL__STLX_THREAD
