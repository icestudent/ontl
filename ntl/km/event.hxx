/**\file*********************************************************************
 *                                                                     \brief
 *  Events
 *
 ****************************************************************************
 */

#ifndef NTL__KM_EVENT
#define NTL__KM_EVENT

#include "basedef.hxx"
#include "../nt/event.hxx"

namespace ntl {
  namespace km {

    using nt::event_type;
    using nt::SynchronizationEvent; // auto-reset
    using nt::NotificationEvent;    // manual-reset

    struct kevent { dispatcher_header Header; };


    NTL__EXTERNAPI
      void __stdcall
      KeInitializeEvent(
      kevent *    Event,
      event_type  Type,
      bool        State
      );

    NTL__EXTERNAPI
      long __stdcall
      KeSetEvent(
      kevent *  Event,
      kpriority Increment,
      bool      Wait
      );

    template <event_type Type>
    struct event : kevent
    {
      event(bool Signaled = false)
      {
        KeInitializeEvent(this, Type, Signaled);
      }

      long set(
        kpriority Increment = 0,
        bool      Wait      = false)
      {
        return KeSetEvent(this, Increment, Wait);
      }
    };

    typedef event<NotificationEvent> notification_event;
    typedef event<SynchronizationEvent> synchronization_event;



    //
    // fast mutexes.
    //
    struct fast_mutex;

    NTL__EXTERNAPI
      void __fastcall
      ExAcquireFastMutexUnsafe (fast_mutex* FastMutex);

    NTL__EXTERNAPI
      void __fastcall
      ExReleaseFastMutexUnsafe (fast_mutex* FastMutex);


#if defined(_NTHAL_) && defined(_M_IX86)

    NTL__EXTERNAPI
      void __fastcall
      ExiAcquireFastMutex (fast_mutex* FastMutex);

    NTL__EXTERNAPI
      void __fastcall
      ExiReleaseFastMutex (fast_mutex* FastMutex);

    NTL__EXTERNAPI
      void __fastcall
      ExiTryToAcquireFastMutex (fast_mutex* FastMutex);

#else

    NTL__EXTERNAPI
      void __fastcall
      ExAcquireFastMutex (fast_mutex* FastMutex);

    NTL__EXTERNAPI
      void __fastcall
      ExReleaseFastMutex (fast_mutex* FastMutex);

    NTL__EXTERNAPI
      bool __fastcall
      ExTryToAcquireFastMutex (fast_mutex* FastMutex);

#endif // _NTHAL_ && _M_IX86


    struct kthread;
    struct fast_mutex {

      enum {
        FM_LOCK_BIT_V        = 0x0, // Lock bit as a bit number
        FM_LOCK_BIT          = 0x1, // Actual lock bit, 1 = Unlocked, 0 = Locked
        FM_LOCK_WAITER_WOKEN = 0x2, // A single waiter has been woken to acquire this lock
        FM_LOCK_WAITER_INC   = 0x4 // Increment value to change the waiters count
      };
      volatile uint32_t Count;
      kthread* Owner;
      uint32_t Contention;
      synchronization_event Gate;
      uint32_t OldIrql;

      fast_mutex()
        :Count(FM_LOCK_BIT), Owner(NULL),
        Contention(0)
      {
      }

      void acquire()
      {
        ExAcquireFastMutex(this);
      }
      bool try_acquire()
      {
        return ExTryToAcquireFastMutex(this);
      }
      void release()
      {
        ExReleaseFastMutex(this);
      }
      void acquire_unsafe()
      {
        ExAcquireFastMutexUnsafe(this);
      }
      void release_unsafe()
      {
        ExReleaseFastMutexUnsafe(this);
      }
    };


    //
    // Mutex object
    //
    struct kmutant;

    NTL__EXTERNAPI
      void __stdcall
      KeInitializeMutex (
      kmutant* Mutex,
      uint32_t Level
      );
    NTL__EXTERNAPI
      int32_t __stdcall
      KeReadStateMutex (
      kmutant* Mutex
      );
    NTL__EXTERNAPI
      int32_t __stdcall
      KeReleaseMutex (
      kmutant* Mutex,
      bool Wait
      );

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
        bit_v = 0x00,
        bit   = 0x01,
        waiter_woken  = 0x02,
        waiter_inc    = 0x04
      };

      int32_t   Count;
      kthread*  Owner;
      uint32_t  Contention;
      kgate     Gate;
      int16_t   KernelApcDisable;
      int16_t   SpecialApcDisable;
    };

  }// namespace km
}// namespace ntl

#endif //
