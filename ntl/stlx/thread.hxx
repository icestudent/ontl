/**\file*********************************************************************
*                                                                     \brief
*  19.4 Thread support library [thread]
*
****************************************************************************
*/
#ifndef NTL__STLX_THREAD
#define NTL__STLX_THREAD

#include "system_error.hxx"
#include "ostream.hxx"

namespace std {

  /**
  *	@group thread Thread support library
  *  @{
  **/

  class thread;

  void swap(thread& x, thread& y);
  //void swap(thread&& x, thread& y);
  //void swap(thread& x, thread&& y);

  namespace this_thread {

    thread::id get_id();
    void yield();
    void sleep(const system_time& abs_time);
    template <class Duration>
    void sleep(const Duration& rel_time);

  }

  /// 30.2.1 thread.thread.class
  class thread {
  public:
    // types:
    class id;
    typedef struct {}* native_handle_type; // See 30.1.3

    // construct/copy/destroy:
    thread();
    template <class F> explicit thread(F f);
    //template <class F, class ...Args> thread(F&& f, Args&&... args);
    ~thread();
    thread(const thread&) = delete;
    thread(thread&&);
    thread& operator=(const thread&) = delete;
    thread& operator=(thread&&);

    // members:
    void swap(thread&&);
    bool joinable() const;
    void join();
    void detach();
    id get_id() const;
    native_handle_type native_handle(); // See 30.1.3

    // static members:
    static unsigned hardware_concurrency();

  };


  /// 30.2.1.1 thread.thread.id
  class thread::id {
  public:
    id();
  };

  bool operator==(thread::id x, thread::id y);
  bool operator!=(thread::id x, thread::id y);
  bool operator<(thread::id x, thread::id y);
  bool operator<=(thread::id x, thread::id y);
  bool operator>(thread::id x, thread::id y);
  bool operator>=(thread::id x, thread::id y);

  template<class charT, class traits>
  basic_ostream<charT, traits>&
    operator<< (basic_ostream<charT, traits>&& out, thread::id id);


  /** @} thread */
} // std

#endif // NTL__STLX_THREAD