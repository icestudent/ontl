/**\file*********************************************************************
 *                                                                     \brief
 *  Synchronization support
 *
 ****************************************************************************
 */
#ifndef NTL__NT_MUTEX
#define NTL__NT_MUTEX
#pragma once

#include "resource.hxx"

namespace ntl {
  namespace nt {

#pragma warning(disable:4127) // conditional expression is constant

    /// STL default base mutex object
    /// In NT subsystem implemented as futex.
    template<bool recursive>
    class base_mutex:
      protected critical_section
    {
    public:
      /** Constructs mutex object */
      base_mutex()
      {}

      /**
       *	@note The behavior of a program is undefined if it destroys a recursive_mutex object owned by any thread.
       **/
      ~base_mutex()
      {
        // NOTE: see @note
        // detect who locks the mutex
        if(locked()){
          if(recursive && is_owner())
            release();
          else 
            // UB
            _assert_msg("This mutex owned by other thread or deletion of owning mutex is not allowed.");
        }
      }

      /** Blocks the calling thread until ownership of the mutex can be obtained for the calling thread. */
      void lock() __ntl_throws(std::system_error)
      {
        if(!recursive){
          if(is_owner()){ // throw system_error(resource_deadlock_would_occur)
            _assert_msg("Current thread already owns the mutex.");
            return;
          }
        }
        acquire();
      }

      /** Attempts to obtain ownership of the mutex for the calling thread without blocking. */
      bool try_lock() __ntl_nothrow
      {
        if(!recursive){
          if(is_owner()){ // throw system_error(resource_deadlock_would_occur)
            _assert_msg("Current thread already owns the mutex.");
            return false;
          }
        }
        return try_acquire();
      }

      /** Releases the calling thread’s ownership of the mutex. */
      void unlock() __ntl_nothrow
      {
        if(!locked()){
          _assert_msg("Unlocking non-locked mutex."); // throw system_error(operation_not_permitted)
        }else
          release();
      }

      // Provide access to implementation details.
      typedef ntl::nt::rtl::critical_section* native_handle_type;
      native_handle_type native_handle()
      {
        return this;
      }

    private:
      base_mutex(const base_mutex&) __deleted;
      base_mutex& operator=(const base_mutex&) __deleted;
    };

#pragma warning(default:4127)
  }
}
#endif // NTL__NT_MUTEX