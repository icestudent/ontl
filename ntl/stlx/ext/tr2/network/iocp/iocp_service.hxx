#pragma once

#include <nt/iocp.hxx>
#include <nt/event.hxx>
#include <nt/thread.hxx>
#include <nt/srwlock.hxx>
#include <nt/system_error.hxx>
#include <atomic.hxx>
#include <vector>
#include "op.hxx"

namespace std { namespace tr2 { namespace sys {

	class iocp_service:
    public io_service::service
	{
		typedef ntl::nt::ntstatus ntstatus;
		typedef ntl::nt::status   status;
		typedef __::async_operation async_operation;

		// system codes
		static const ntl::nt::ntstatus stop_service_code = ntl::nt::status::system_shutdown;

    __::timer_scheduler& scheduler;
	public:
		static io_service::id id;

		explicit iocp_service(io_service& ios)
			: service(ios)
      , timer_event(timer_event.auto_reset)
			, timer_thread(&iocp_service::timer_proc, this)
      , scheduler(use_service<__::timer_scheduler>(ios))
		{
      scheduler.handler =
        &iocp_service::add_timer_;
      scheduler.ctx = this;
        //std::bind(&iocp_service::add_timer, this, std::placeholders::_1, std::placeholders::_2);
		}

		bool stopped() const
		{
			return this->stopper.test();
		}

		void stop()
		{
			if(stopper.test_and_set() == false) {
				check(iocp.set_completion(nullptr, stop_service_code));
			}
		}

		void reset()
		{
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
    void dispatch(CompletionHandler handler);

    template<class CompletionHandler>
    void post(CompletionHandler handler);

	protected:
		void check(ntl::nt::ntstatus st)
		{
			if(!ntl::nt::success(st)) {
				std::error_code ec = std::make_error_code(st);
				throw_system_error(ec);
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

		void add_timer(const void* timer, async_operation* op)
		{
			if(shutdown.test()) {
				post_immediate_completion(op);
				return;
			}

			work_started();

			wlock lock(timer_lock);
      timers.push_back(std::make_pair(reinterpret_cast<ntl::nt::legacy_handle>(timer), op));
      timer_event.pulse();
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
      iocp.set_completion(nullptr, op);
    }

    void complete(const async_operation* op, const std::error_code& ec, size_t transferred)
    {
      op->complete(*this, ec, transferred);
      work_finished();
    }

		bool do_poll(bool block, error_code& ec)
		{
			ec.clear();
			if(workers.compare(0) == 0) {
				stop();
				return false;
			}

			const ntl::nt::system_duration instant;
			for(;;) {

				ntl::nt::io_completion_port::entry entry;
				ntstatus st = block ? iocp.pop_completion(entry) : iocp.pop_completion(entry, instant);

				if(st == ntl::nt::status::timeout) {
					// no ready operations
					if(block)
						continue;
					return false;
				}
				else if(!ntl::nt::success(st)) {
					// iocp error
					ec = std::make_error_code(st);
					return false;
				}
				else if(entry.Status == stop_service_code) {
					// stop service
					if(stopper.test()) {
						return false;
					}
				}else if(entry.Apc) {
					const async_operation* op = static_cast<const async_operation*>(entry.Apc);
					assert(op->is_async_operation());

					if(op->ready.test()) {
            complete(op, std::make_error_code(entry.Status), entry.Information);
						return true;
          } else {
            op = nullptr;
          }
				}
				continue;
			}
			return false;
		}

	private:
		void shutdown_service() override
		{
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
    static void add_timer_(void* ctx, const void* timer, async_operation* op)
    { return static_cast<iocp_service*>(ctx)->add_timer(timer, op); }

    uint32_t __stdcall timer_worker()
		{
			using namespace ntl::nt;
      this_thread::setname("iocp::timer_proc");

			size_t count;
			legacy_handle handles[64];

      handles[0] = timer_event.get();

			do {
				{
					rlock lock(timer_lock);
					size_t size = timers.size();
          for(count = 0; count != size; ++count) {
						handles[count+1] = timers[count].first;
          }
				}

				ntstatus st = NtWaitForMultipleObjects(count+1, handles, wait_type::WaitAny, false, infinite_timeout());
        if(st == ntl::nt::status::wait_0) {
          // timers changed
          continue;;
        }
				else if(st >= status::wait_1 && st <= status::wait_63) {
					count = st - status::wait_1;
					wlock lock(timer_lock);
					post_deferred_completion(timers[count].second);
          timers.erase(timers.begin() + count);
				}
			} while(shutdown.test(false));

			ntl::nt::this_thread::exit(status::success);
		}

	private:
		ntl::nt::io_completion_port iocp;
		ntl::atomic::value_t workers;
		ntl::atomic::flag_t stopper, shutdown;
    //ntl::nt::user_event shutdown;
		
		// timers
    std::vector<std::pair<ntl::nt::legacy_handle, async_operation*>> timers;
		ntl::nt::user_thread timer_thread;
		ntl::nt::srwlock timer_lock;
    ntl::nt::user_event timer_event;
    typedef ntl::nt::srwlock::guard<false> rlock;
    typedef ntl::nt::srwlock::guard<true>  wlock;
	};


}}}
