/**\file*********************************************************************
*                                                                     \brief
*  Slim reader/writer lock
*  @warning SRWLock implemented at NT 6.0+ and requires "ntdllp.lib",
*  which can be linked only at VC 16+ (VS 2010)
*
****************************************************************************
*/
#ifndef NTL__NT_SRWLOCK
#define NTL__NT_SRWLOCK
#pragma once

#include "basedef.hxx"

namespace ntl {
  namespace nt {
    namespace rtl
    {
      struct srwlock;

      NTL_EXTERNAPI
        void __stdcall RtlAcquireReleaseSRWLockExclusive(rtl::srwlock* SRWLock);
      NTL_EXTERNAPI
        void __stdcall RtlAcquireSRWLockExclusive(rtl::srwlock* SRWLock);
      NTL_EXTERNAPI
        void __stdcall RtlAcquireSRWLockShared(rtl::srwlock* SRWLock);
      NTL_EXTERNAPI
        void __stdcall RtlInitializeSRWLock(rtl::srwlock* SRWLock);
      NTL_EXTERNAPI
        void __stdcall RtlReleaseSRWLockExclusive(rtl::srwlock* SRWLock);
      NTL_EXTERNAPI
        void __stdcall RtlReleaseSRWLockShared(rtl::srwlock* SRWLock);
      NTL_EXTERNAPI
        bool __stdcall RtlTryAcquireSRWLockExclusive(rtl::srwlock* SRWLock);
      NTL_EXTERNAPI
        bool __stdcall RtlTryAcquireSRWLockShared(rtl::srwlock* SRWLock);
      NTL_EXTERNAPI
        void __stdcall RtlUpdateClonedSRWLock(rtl::srwlock* SRWLock);

      struct srwlock
      {
        union 
        {
          struct
          {
            uintptr_t Locked  :1;
            uintptr_t Waiting :1;
            uintptr_t Waking  :1;
            uintptr_t MultipleShared :1;
            uintptr_t Shared  :28;
          };
          uintptr_t Value; //-V117
          void* _; //-V117
        };
      };
    } // rtl

    class srwlock:
      public rtl::srwlock
    {
    public:

      template<bool Exclusive>
      class guard;

      template<bool Exclusive>
      class try_lock;

      constexpr srwlock()
      {
        Value = 0;
      }

      void acquire(bool exclusive)
      {
        exclusive ? rtl::RtlAcquireSRWLockExclusive(this) : rtl::RtlAcquireSRWLockShared(this);
      }
      void release(bool exclusive)
      {
        exclusive ? rtl::RtlReleaseSRWLockExclusive(this) : rtl::RtlReleaseSRWLockShared(this);
      }

      bool try_acquire(bool exclusive)
      {
        return exclusive ? rtl::RtlTryAcquireSRWLockExclusive(this) : rtl::RtlTryAcquireSRWLockShared(this);
      }

      void acquire_release()
      {
        rtl::RtlAcquireReleaseSRWLockExclusive(this);
      }
    };



    template<bool Exclusive>
    class srwlock::guard
    {
    public:
      static const bool exclusive = Exclusive;

      explicit guard(srwlock& m)
        :m(m)
      {
        m.acquire(exclusive);
      }
      ~guard()
      {
        m.release(exclusive);
      }
    private:
      srwlock& m;

      guard(const guard&) __deleted;
      guard& operator=(const guard&) __deleted;
    };



    template<bool Exclusive>
    class srwlock::try_lock
    {
    public:
      static const bool exclusive = Exclusive;

      explicit try_lock(srwlock& m)
        :m(m)
      {
        locked = m.try_acquire(exclusive);
      }
      ~try_lock()
      {
        if(locked)
          m.release(exclusive);
      }

      bool owns_lock() const { return locked; }

      __explicit_operator_bool() const { return __explicit_bool(locked); }

    private:
      srwlock& m;
      bool locked;

      try_lock(const try_lock&) __deleted;
      try_lock& operator=(const try_lock&) __deleted;
    };

  } 
}
#endif //NTL__NT_SRWLOCK
