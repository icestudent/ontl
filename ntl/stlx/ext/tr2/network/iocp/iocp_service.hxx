#pragma once

#include <nt/iocp.hxx>
#include <nt/event.hxx>
#include <nt/thread.hxx>
#include <nt/srwlock.hxx>
#include <nt/system_error.hxx>
#include <atomic.hxx>
#include <vector>

#include "op.hxx"
#include "complete_op.hxx"

namespace std { namespace tr2 { namespace sys {

  namespace iocp {

  class iocp_service:
    public io_service::service
  {
    typedef ntl::nt::ntstatus ntstatus;
    typedef ntl::nt::status   status;
    typedef __::async_operation async_operation;
    typedef __::timer_scheduler::timer_data timer_data;
    typedef ntl::nt::overlapped overlapped;
    
    // sort by time, unique by handle
    typedef std::vector<timer_data> timer_queue; // can't use std::priority_queue because of erase(timer)

    struct find_timer
    {
      ntl::nt::legacy_handle h;
      bool operator()(const timer_queue::value_type& t) const
      {
        return h == t.h;
      }
      explicit find_timer(ntl::nt::legacy_handle h)
        : h(h)
      {}
    };
    
    // system codes
    static const ntl::nt::ntstatus
      stop_service_code   = ntl::nt::status::system_shutdown,
      custom_result_code  = ntl::nt::status::more_entries;

    __::timer_scheduler& scheduler;
  public:
    static io_service::id id;

    explicit iocp_service(io_service& ios)
      : service(ios)
      , timer_event(ntl::nt::SynchronizationEvent)
      , timer_thread(&iocp_service::timer_proc, this, true) // create_suspended
      , scheduler(use_service<__::timer_scheduler>(ios))
      , self_id()
    {
      scheduler.handler =
        &iocp_service::add_timer_;
      scheduler.ctx = this;
        //std::bind(&iocp_service::add_timer, this, std::placeholders::_1, std::placeholders::_2);
    }

    bool stopped() const /*volatile*/
    {
      return this->stopper.test();
    }

    void stop() /*volatile*/
    {
      if(stopper.test_and_set() == false) {
        if(stopping.test_and_set() == false) {
          check(iocp.set_completion(nullptr, stop_service_code));
        }
      }
    }

    void reset()
    {
      assert(!can_dispatch());
      stopper.clear();
    }

    size_t run_one(error_code& ec)
    {
      return do_poll(true, ec);
    }

    size_t poll_one(error_code& ec)
    {
      return do_poll(false, ec);
    }

    template<class CompletionHandler>
    void dispatch(CompletionHandler& handler) /*volatile*/
    {
      if(can_dispatch()) {
        // we are inside run() thread, its safe to call handler
        using std::tr2::sys::io_handler_invoke;
        return io_handler_invoke(handler, &handler);
      }

      // we are outside run() thread
      post(handler);
    }

    template<class CompletionHandler>
    void post(CompletionHandler& handler) /*volatile*/
    {
      typedef __::completion_operation<CompletionHandler> op;
      typename op::ptr p (handler);

      post_immediate_completion(p.op);
      
      p.release();
    }


    bool register_handle(ntl::nt::legacy_handle handle, std::error_code& ec)
    {
      if(!ntl::nt::success(iocp.attach(handle, nullptr)))
        return ec = std::make_error_code(iocp.last_status()), false;
      return ec.clear(), true;
    }

    void on_completion(async_operation* op, const ntl::nt::ntstatus st = ntl::nt::status::success, size_t transferred = 0) /*volatile*/
    {
      op->ready.set();
      iocp.set_completion(nullptr, st, transferred, static_cast<overlapped*>(op));
    }

    void on_completion(async_operation* op, const std::error_code& ec, size_t transferred = 0)
    {
      op->Internal1 = ec.value();
      op->Internal2 = reinterpret_cast<uintptr_t>(&ec.category());
      op->Offset = transferred;
      op->ready.set();
      iocp.set_completion(nullptr, custom_result_code, transferred, static_cast<overlapped*>(op));
    }

    void on_pending(async_operation* op)
    {
      if(op->ready.exchange_if_equal(true, false) == true) {
        iocp.set_completion(nullptr, custom_result_code, 0, static_cast<overlapped*>(op));
      }
    }

    void work_started()
    {
      ++workers;
    }

    void work_finished()
    {
      if(--workers == 0)
        stop();
    }

  protected:
    void check(ntl::nt::ntstatus st)
    {
      if(!ntl::nt::success(st)) {
        std::error_code ec = std::make_error_code(st);
        throw_system_error(ec);
      }
    }

    std::error_code make_error_code(ntl::nt::ntstatus st, const async_operation* op)
    {
      return st == custom_result_code 
        ? std::error_code(static_cast<int>(op->Internal1), *reinterpret_cast<const std::error_category*>(op->Internal2))
        : std::make_error_code(st);
    }

    size_t add_timer(const void* timer, const timer_data* data)
    {
      if(!data)
        return remove_timer(timer);

      if(shutdown.test()) {
        post_immediate_completion(data->op);
        return false;
      }

      work_started();

      wlock lock(timer_lock);
      const ntl::nt::legacy_handle t = reinterpret_cast<ntl::nt::legacy_handle>(timer);
      timer_queue::iterator tm = std::find_if(timers.begin(), timers.end(), find_timer(t));
      if(tm != timers.end()) {
        timers.erase(tm);
      }
      timers.push_back(*data);
      timer_event.set();
      timer_thread.resume();
      return true;
    }

    size_t remove_timer(const void* timer)
    {
      if(shutdown.test())
        return 0;

      wlock lock(timer_lock);
      const ntl::nt::legacy_handle t = reinterpret_cast<ntl::nt::legacy_handle>(timer);
      timer_queue::iterator tm = std::find_if(timers.begin(), timers.end(), find_timer(t));
      if(tm != timers.end()) {
        async_operation* op = tm->op;
        timers.erase(tm);

        // wake up thread to recalculate timers
        timer_event.set();

        // complete handler with aborted code
        on_completion(op, std::make_error_code(std::tr2::network::error::operation_aborted));

        return 1;
      }
      return 0;
    }

  public:
    /** Is we inside service? If yes, we can dispatch immediately */
    bool can_dispatch() const /*volatile*/
    {
      return ntl::nt::this_thread::id() == self_id;
    }

    /** Request invocation of the given operation and return immediately. */
    void post_immediate_completion(async_operation* op)
    {
      work_started();
      post_deferred_completion(op);
    }

    /** Request invocation of the given operation and return immediately. */
    void post_deferred_completion(async_operation* op)
    {
      op->ready.set();
      iocp.set_completion(nullptr, static_cast<overlapped*>(op));
    }

  protected:
    void complete(const async_operation* op, const std::error_code& ec, size_t transferred)
    {
      struct finish
      {
        iocp_service* self;
        ~finish()     { self->work_finished(); }
      } finish_work = {this};

      op->complete(*this, ec, transferred);
    }

    bool do_poll(bool block, error_code& ec)
    {
      struct current_thread_t
      {
        iocp_service* iocp;
        current_thread_t(iocp_service* self)
          :iocp(self)
        {
          ntl::atomic::generic_op::exchange(iocp->self_id, ntl::nt::this_thread::id());
        }
        ~current_thread_t()
        {
          ntl::atomic::generic_op::exchange(iocp->self_id, static_cast<ntl::nt::legacy_handle>(nullptr));
        }
      };

      ec.clear();
      if(workers.compare(0) == 0) {
        stop();
        return false;
      }
      
      const ntl::nt::system_duration instant;
      current_thread_t set_current_thread(this);
      for(;;) {

        ntl::nt::io_completion_port::entry entry;
        ntstatus st = block ? iocp.pop_completion(entry) : iocp.pop_completion(entry, instant);

        if(!ntl::nt::success(st))
        {
          // iocp error
          ntl::dbg::bp();
          ec = std::make_error_code(st);
          return false;
        }
        else if(st == ntl::nt::status::timeout)
        {
          // no ready operations
          if(block)
            continue;
          return false;
        }
        else if(entry.Status == stop_service_code)
        {
          // incoming stop event
          stopping.clear();
          if(stopper.test())
          {
            if(stopping.test_and_set() == false) {
              // still stopping, wake thread
              st = iocp.set_completion(nullptr, stop_service_code);
              if(!ntl::nt::success(st))
                ec = std::make_error_code(st);
            }
            return false;
          }
        }
        else if(entry.Apc)
        {
          // incoming async event 
          const ntl::nt::overlapped* lp = static_cast<const ntl::nt::overlapped*>(entry.Apc);
          const async_operation* op = static_cast<const async_operation*>(lp);
          assert(op->is_async_operation());

          if(op->ready.test()) {

            complete(op, make_error_code(entry.Status, op), entry.Information);
            return true;
          } else {
            ntl::dbg::bp();
            op = nullptr;
          }
        }
        continue;
      }
      //return false;
    }

  private:
    void shutdown_service() override
    {
      timer_thread.resume(true);
      shutdown.set();
      timer_event.set();
      while(workers > 0) {
        ntl::nt::io_completion_port::entry entry;
        ntstatus st = iocp.pop_completion(entry);
        if(entry.Apc) {
          async_operation* op = static_cast<async_operation*>(const_cast<void*>(entry.Apc));
          assert(op->is_async_operation());
          op->destroy();
          --workers;
        }
      }
      const std::array<ntl::nt::legacy_handle,2> objects = {iocp.get(), timer_thread.get()};
      ntl::nt::wait_for(objects, true, true);
      iocp.reset();
      timer_thread.reset();
    }

    static uint32_t __stdcall timer_proc(void* Parameter)
    { return static_cast<iocp_service*>(Parameter)->timer_worker(); }
    static size_t add_timer_(void* ctx, const void* timer, const timer_data* data)
    { return static_cast<iocp_service*>(ctx)->add_timer(timer, data); }

    uint32_t __stdcall timer_worker()
    {
      using namespace ntl::nt;
      ntl::nt::this_thread::setname("iocp::timer_proc");

      uint32_t count;
      legacy_handle handles[2] = { timer_event.get() };

      do {

        timer_data first;
        {
          count = 1;
          wlock lock(timer_lock);
          if(!timers.empty()) {
            // remove the last timer now to eliminate a possible concurrent change when fired
            if(timers.size() > 1)
              std::sort(timers.begin(), timers.end(), std::greater<timer_queue::value_type>());
            first = timers.back();
            //timers.pop_back();
            handles[count++] = first.h;
          }
        }

        ntstatus st = NtWaitForMultipleObjects(count, handles, wait_type::WaitAny, false, infinite_timeout());
        if(st == ntl::nt::status::wait_0) {
          // queue was changed
          continue;
        }
        else if(st == status::wait_1) {
          // timer fired
          wlock lock(timer_lock);
          timer_queue::iterator tm = std::find_if(timers.begin(), timers.end(), find_timer(first.h));
          if(tm != timers.end()) {
            // fired and not erased yet
            timers.erase(tm);
            post_deferred_completion(first.op);
          } else {
            // if erased, operation_aborted will be sent 
            first.h = 0;
          }
        }

      } while(shutdown.test(false));

      ntl::nt::this_thread::exit(status::success);
    }

  private:
    ntl::nt::io_completion_port iocp;
    ntl::atomic::value_t workers;
    ntl::atomic::flag_t stopper, stopping, shutdown;
    ntl::nt::legacy_handle volatile self_id;
    
    // timers
    ntl::nt::user_thread timer_thread;
    ntl::nt::srwlock timer_lock;
    ntl::nt::user_event timer_event;
    timer_queue timers;
    typedef ntl::nt::srwlock::guard<false> rlock;
    typedef ntl::nt::srwlock::guard<true>  wlock;
  };

} // iocp ns
}}}
