/**\file*********************************************************************
*                                                                     \brief
*  Conditional variables
*
****************************************************************************
*/
#ifndef NTL__NT_COND_VARIABLE
#define NTL__NT_COND_VARIABLE
#pragma once

#include "resource.hxx"
#include "srwlock.hxx"

namespace ntl {
  namespace nt {

    namespace rtl
    {
      struct condition_variable
      {
        enum lock_mode {
          lock_exclusive,
          lock_shared,
        };

        void* _;
      };

      NTL_EXTERNAPI
        void __stdcall RtlInitializeConditionVariable(rtl::condition_variable* ConditionVariable);

      NTL_EXTERNAPI
        ntstatus __stdcall RtlSleepConditionVariableCS(rtl::condition_variable* ConditionVariable, rtl::critical_section* CriticalSection, const systime_t& WaitTime);

      NTL_EXTERNAPI
        ntstatus __stdcall RtlSleepConditionVariableSRW(rtl::condition_variable* ConditionVariable, rtl::srwlock* SRWLock, const systime_t& WaitTime, rtl::condition_variable::lock_mode LockMode);

      NTL_EXTERNAPI
        void __stdcall RtlWakeAllConditionVariable(rtl::condition_variable* ConditionVariable);

      NTL_EXTERNAPI
        void __stdcall RtlWakeConditionVariable(rtl::condition_variable* ConditionVariable);

    }// rtl


    class condition_variable:
      public rtl::condition_variable
    {
    public:
      condition_variable()
      {
        _ = 0;
      }

      void wake()
      {
        rtl::RtlWakeConditionVariable(this);
      }
      void wake_all()
      {
        rtl::RtlWakeAllConditionVariable(this);
      }

      ntstatus sleep(rtl::critical_section& lock)
      {
        return rtl::RtlSleepConditionVariableCS(this, &lock, infinite_timeout());
      }
      ntstatus sleep(srwlock& lock, bool exclusive)
      {
        return rtl::RtlSleepConditionVariableSRW(this, &lock, infinite_timeout(), exclusive ? lock_exclusive : lock_shared);
      }

      template <class Rep, class Period>
      ntstatus sleep_for(const std::chrono::duration<Rep, Period>& rel_time, rtl::critical_section& lock)
      {
        return rtl::RtlSleepConditionVariableCS(this, &lock, -1i64*std::chrono::duration_cast<system_duration>(rel_time).count());
      }
      template <class Rep, class Period>
      ntstatus sleep_for(const std::chrono::duration<Rep, Period>& rel_time, srwlock& lock, bool exclusive)
      {
        return rtl::RtlSleepConditionVariableSRW(this, &lock, -1i64*std::chrono::duration_cast<system_duration>(rel_time).count(), exclusive ? lock_exclusive : lock_shared);
      }

      template <class Clock, class Duration>
      ntstatus sleep_until(const std::chrono::time_point<Clock, Duration>& abs_time, rtl::critical_section& lock)
      {
        return rtl::RtlSleepConditionVariableCS(this, &lock, std::chrono::duration_cast<system_duration>(abs_time.time_since_epoch()).count());
      }
      template <class Clock, class Duration>
      ntstatus sleep_until(const std::chrono::time_point<Clock, Duration>& abs_time, srwlock& lock, bool exclusive)
      {
        return rtl::RtlSleepConditionVariableSRW(this, &lock, std::chrono::duration_cast<system_duration>(abs_time.time_since_epoch()).count(), exclusive ? lock_exclusive : lock_shared);
      }

    };

  }
}
#endif //NTL__NT_COND_VARIABLE
