#pragma once

#include "op.hxx"

namespace std { namespace tr2 { namespace sys {

	namespace __
	{
		template<class Handler>
		struct wait_operation: async_operation
		{
			typedef typename async_operation::ptr<wait_operation, Handler> ptr;

			wait_operation(Handler& h)
				: async_operation(do_complete)
				, fn(h)
			{}

		protected:
			static void do_complete(iocp_service* owner, async_operation* base, const error_code& ec, size_t transferred)
			{
				if(!owner)
					return;		// called if async_op::destroy

				wait_operation* self = static_cast<wait_operation*>(base);

				sys::binder1<Handler, std::error_code> binder(self->fn, ec);
				
				using std::tr2::sys::io_handler_invoke;
				io_handler_invoke(binder, &self->fn);
			}

		private:
			Handler fn;

		};
	} // __ ns

}}}
