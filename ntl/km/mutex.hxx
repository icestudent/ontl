/**\file*********************************************************************
 *                                                                     \brief
 *  Synchronization support
 *
 ****************************************************************************
 */
#ifndef NTL__KM_MUTEX
#define NTL__KM_MUTEX
#pragma once

#include "basedef.hxx"
#include "event.hxx"

namespace ntl {
  namespace km {

    //
    // fast mutexes.
    //
    struct fast_mutex;
    typedef void(__fastcall *fast_mutex_control_routine)(fast_mutex* FastMutex);

    NTL_EXTERNVAR fast_mutex_control_routine ExAcquireFastMutexUnsafe, ExReleaseFastMutexUnsafe;


#if defined(_NTHAL_) && defined(_M_IX86)
    NTL_EXTERNVAR fast_mutex_control_routine ExiAcquireFastMutex, ExiReleaseFastMutex;
    NTL_EXTERNAPI bool __fastcall ExiTryToAcquireFastMutex (fast_mutex* FastMutex);
#else
    NTL_EXTERNVAR fast_mutex_control_routine ExAcquireFastMutex, ExReleaseFastMutex;
    NTL_EXTERNAPI bool __fastcall ExTryToAcquireFastMutex (fast_mutex* FastMutex);
#endif // _NTHAL_ && _M_IX86


    //
    // guarded mutexes (2k3+)
    //
    struct kguarded_mutex;
    typedef void(__fastcall *guarded_mutex_control_routine)(kguarded_mutex* Mutex);

    NTL_EXTERNVAR guarded_mutex_control_routine KeInitializeGuardedMutex, KeAcquireGuardedMutex, KeReleaseGuardedMutex;
    NTL_EXTERNVAR guarded_mutex_control_routine KeAcquireGuardedMutexUnsafe, KeReleaseGuardedMutexUnsafe;
    NTL_EXTERNAPI bool __fastcall KeTryToAcquireGuardedMutex(kguarded_mutex* Mutex);

    struct kthread;
    struct fast_mutex
    {
      enum lock
      {
        bit_v       = 0x00, // Lock bit as a bit number
        lockbit     = 0x01, // Actual lock bit, 1 = Unlocked, 0 = Locked
        waiter_woken= 0x02, // A single waiter has been woken to acquire this lock
        waiter_inc  = 0x04  // Increment value to change the waiters count
      };

      volatile uint32_t Count;
      kthread* Owner;
      uint32_t Contention;
      synchronization_event Gate;
      uint32_t OldIrql;

      fast_mutex()
        :Count(lockbit), Owner(NULL),
        Contention(0)
      {
      }

      void acquire() { ExAcquireFastMutex(this); }
      bool try_acquire() { return ExTryToAcquireFastMutex(this); }
      void release() { ExReleaseFastMutex(this); }
      void acquire_unsafe() { ExAcquireFastMutexUnsafe(this); }
      void release_unsafe() { ExReleaseFastMutexUnsafe(this); }

      bool is_owned() const volatile { return (Count & lockbit) == 0; }
    private:
      fast_mutex(const fast_mutex&);
      fast_mutex& operator=(const fast_mutex&);
    };


    //
    // Mutex object
    //
    typedef struct kmutant kmutex;

    NTL_EXTERNAPI void __stdcall KeInitializeMutex (kmutex* Mutex, uint32_t Level);
    NTL_EXTERNAPI int32_t __stdcall KeReadStateMutex(kmutex* Mutex);
    NTL_EXTERNAPI int32_t __stdcall KeReleaseMutex (kmutex* Mutex, bool Wait);

    struct kmutant
    {
      dispatcher_header   Header;
      list_entry          MutantListEntry;
      kthread            *OwnerThread;
      bool                Abandoned;
      bool                ApcDisable;

      kmutant()
      {
        KeInitializeMutex(this, 0);
      }

      int32_t release(bool wait = false)
      {
        return KeReleaseMutex(this, wait);
      }

      int32_t read_state()
      {
        return KeReadStateMutex(this);
      }
    };


    // guarded mutex

    struct kguarded_mutex
    {
      enum lock
      {
        bit_v       = 0x00, // lock bit as bit number
        lockbit     = 0x01,
        waiter_woken= 0x02,
        waiter_inc  = 0x04
      };

      volatile int32_t Count;       // lock count
      kthread*  Owner;              // current lock owner
      uint32_t  Contention;
      kevent    Event;
      int16_t   KernelApcDisable;   // \
                                    // |  CombinedApcDisable;
      int16_t   SpecialApcDisable;  // /

      kguarded_mutex() { KeInitializeGuardedMutex(this); }

      void acquire() { KeAcquireGuardedMutex(this); }
      void release() { KeReleaseGuardedMutex(this); }

      void acquire_unsafe() { KeAcquireGuardedMutexUnsafe(this); }
      void release_unsafe() { KeReleaseGuardedMutexUnsafe(this); }

      bool try_acquire() { return KeTryToAcquireGuardedMutex(this); }

      bool is_owned() const volatile { return (Count & lockbit) == 0; }

      // std lock interface
      void lock()  { acquire(); }
      void unlock(){ release(); }
      bool try_lock() { return try_acquire(); }
    };



#ifdef NTL__STLX_SYSTEM_ERROR
#pragma warning(disable:4127) // conditional expression is constant

    /// STL default base mutex object
    template<bool recursive>
    class base_mutex:
      protected event<NotificationEvent>
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
          if(recursive && thread_locked())
            release();
          else 
            // UB
            assert(!"This mutex owned by other thread or deletion of owning mutex is not allowed.");
        }
      }

      /** Blocks the calling thread until ownership of the mutex can be obtained for the calling thread. */
      void lock() __ntl_throws(std::system_error)
      {
        if(!recursive){
          if(thread_locked()){ // throw system_error(resource_deadlock_would_occur)
            assert(!"Current thread already owns the mutex.");
            return;
          }
        }
        acquire();
      }

      /** Attempts to obtain ownership of the mutex for the calling thread without blocking. */
      bool try_lock() __ntl_nothrow
      {
        if(!recursive){
          if(thread_locked()){ // throw system_error(resource_deadlock_would_occur)
            assert(!"Current thread already owns the mutex.");
            return false;
          }
        }
        return try_acquire();
      }

      /** Releases the calling thread’s ownership of the mutex. */
      void unlock() __ntl_nothrow
      {
        if(!locked()){
          assert(!"Unlocking non-locked mutex."); // throw system_error(operation_not_permitted)
        }else
          release();
      }

      // Provide access to implementation details.
      typedef event<NotificationEvent>* native_handle_type;
      native_handle_type native_handle()
      {
        return this;
      }

    protected:
      typedef ntl::atomic_exec<kguarded_mutex> lock_guard;

      void acquire();
      bool try_acquire();
      void release();
      bool locked();
      bool thread_locked();

    private:
      kguarded_mutex mtx;

      base_mutex(const base_mutex&) __deleted;
      base_mutex& operator=(const base_mutex&) __deleted;
    };

#pragma warning(default:4127)
#endif // NTL__STLX_SYSTEM_ERROR

  }
}
#endif // NTL__NT_MUTEX
