#pragma once

#include "../../../../../nt/iocp.hxx"
#include "../../../../../nt/system_error.hxx"
#include "../../../../../atomic.hxx"
#include "op.hxx"

namespace std { namespace tr2 { namespace sys {

	class iocp_service:
		public __::timer_scheduler
	{
		typedef ntl::nt::ntstatus ntstatus;
		typedef ntl::nt::status   status;
		typedef __::async_operation async_operation;

		// system codes
		static const ntl::nt::ntstatus stop_service_code = ntl::nt::status::system_shutdown;
	public:
		static io_service::id id;

		explicit iocp_service(io_service& ios)
			: timer_scheduler(ios)
		{
			assert(!has_service<timer_scheduler>(ios));
			add_service<timer_scheduler>(ios, this);
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

		void add_timer(void* timer, async_operation* op) override
		{
			if(shutdown.test()) {
				//post_immediate_completion(op);
				return;
			}

			work_started();

			// how we can wait timer here?
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

						op->complete(*this, std::make_error_code(entry.Status), entry.Information);
						work_finished();
						return true;
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
			ntl::nt::wait_for(iocp, true);
			iocp.reset();
		}

	private:
		ntl::nt::io_completion_port iocp;
		ntl::atomic::value_t workers;
		ntl::atomic::flag_t stopper, shutdown;


	};


}}}
