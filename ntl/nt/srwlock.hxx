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
        void* _;
      };
    } // rtl

    class srwlock:
      public rtl::srwlock
    {
    public:

      template<bool Exclusive>
      class guard;

      srwlock()
      {
        _ = 0;
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

  } 
}
#endif //NTL__NT_SRWLOCK
