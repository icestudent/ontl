/**\file*********************************************************************
 *                                                                     \brief
 *  NT timers
 *
 ****************************************************************************
 */
#ifndef NTL__NT_TIMER
#define NTL__NT_TIMER
#pragma once

#include "time.hxx"
#include "object.hxx"
#include "../stlx/chrono.hxx"


namespace ntl {
namespace nt {

///\name Timer API

enum timer_type {
  NotificationTimer,
  SynchronizationTimer
};

typedef void __stdcall timer_apc_routine(void* TimerContext, uint32_t TimerLowValue, int32_t TimerHighValue);
typedef void __stdcall timer_queue_routine(void* TimerContext, bool TimerOrWaitFired);

enum timer_information_class
{
  TimerBasicInformation
};

struct timer_basic_information
{
  static const timer_information_class info_class_type = TimerBasicInformation;

  int64_t RemainingTime;
  bool TimerState;
};


// timers
NTL__EXTERNAPI
ntstatus __stdcall NtCreateTimer(handle* TimerHandle, uint32_t DesiredAccess, const object_attributes* ObjectAttributes, timer_type TimerType);

NTL__EXTERNAPI
ntstatus __stdcall NtOpenTimer(handle* TimerHandle, uint32_t DesiredAccess, const object_attributes* ObjectAttributes);

NTL__EXTERNAPI
ntstatus __stdcall NtSetTimer(legacy_handle TimerHandle, const int64_t& DueTime, timer_apc_routine* TimerApcRoutine, void* TimerContext, bool ResumeTimer, int32_t Period, bool* PreviousState);

NTL__EXTERNAPI
ntstatus __stdcall NtCancelTimer(legacy_handle TimerHandle, bool* CurrentState);

NTL__EXTERNAPI
ntstatus __stdcall NtQueryTimer(legacy_handle TimerHandle, timer_information_class TimerInformationClass, void* TimerInformation, uint32_t TimerInformationLength, uint32_t* ReturnLength);


// timer queues

struct worker_thread
{
  enum flags_t {
    ExecuteDefault              = 0x00,
    ExecuteInIoThread           = 0x01,
    ExecuteInUiThread           = 0x02,
    ExecuteInWaitThread         = 0x04,
    ExecuteOnlyOnce             = 0x08,
    ExecuteLongFunction         = 0x10,
    ExecuteInTimerThread        = 0x20,
    ExecuteInPersistentIoThread = 0x40,
    ExecuteInPersistentThread   = 0x80,
    TransferImpersonation       = 0x100
  };
};
typedef worker_thread::flags_t worker_thread_flags;


NTL__EXTERNAPI
ntstatus __stdcall RtlCreateTimerQueue(handle* TimerQueueHandle);

NTL__EXTERNAPI
ntstatus __stdcall RtlCreateTimer(
                    legacy_handle         Queue,
                    handle*               TimerHandle,
                    timer_queue_routine*  Callback,
                    void*                 Context,
                    uint32_t              DueTime,
                    uint32_t              Period,
                    worker_thread_flags   Flags
                    );

NTL__EXTERNAPI
ntstatus __stdcall RtlUpdateTimer(legacy_handle TimerQueueHandle, legacy_handle TimerHandle, uint32_t DueTime, uint32_t Period);

NTL__EXTERNAPI
ntstatus __stdcall RtlDeleteTimer(legacy_handle TimerQueueHandle, legacy_handle TimerHandle, legacy_handle CompletionEvent);

NTL__EXTERNAPI
ntstatus __stdcall RtlDeleteTimerQueue(legacy_handle TimerQueueHandle);

NTL__EXTERNAPI
ntstatus __stdcall RtlDeleteTimerQueueEx(legacy_handle TimerQueueHandle, legacy_handle CompletionEvent);


class timer;
} // namespace nt

  template<>
  struct device_traits<nt::timer>:
    private device_traits<>
  {
    enum access_mask {
      query_state   = 1,
      modify_state  = 2,
      synchronize   = device_traits<>::synchronize,
      query         = synchronize | query_state,
      all_access    = standard_rights_required | synchronize | query_state | modify_state
    };

    friend access_mask operator | (access_mask m, access_mask m2)
    {
      return bitwise_or(m, m2);
    }

    friend access_mask operator | (access_mask m, nt::access_mask m2)
    {
      return m | static_cast<access_mask>(m2);
    }

    friend access_mask operator | (nt::access_mask m, access_mask m2)
    {
      return m2 | m;
    }
  };

namespace nt {

/**
 *	@brief Timer
 **/
class timer:
  public handle,
  public device_traits<timer>,
  public last_status_t
{
public:
  /** Manual-reset timer: a timer whose state remains signaled until a new due time was established. */
  static const timer_type NotificationTimer = NotificationTimer;
  /** Auto-reset timer: a timer whose state remains signaled until a thread completes a wait operation on the timer object. */
  static const timer_type SynchronizationTimer = SynchronizationTimer;

  static const timer_type manual_reset = NotificationTimer,
    auto_reset = SynchronizationTimer;

public:
  ///\name create timer

  /** Creates an unnamed timer object */
  explicit timer(timer_type TimerType, access_mask DesiredAccess = all_access)
  {
    const object_attributes oa;
    last_status_ = NtCreateTimer(this, DesiredAccess, &oa, TimerType);
  }

  /** Creates a named timer object */
  explicit timer(const object_attributes& TimerName, timer_type TimerType, access_mask DesiredAccess = all_access)
  {
    last_status_ = NtCreateTimer(this, DesiredAccess, &TimerName, TimerType);
  }
  explicit timer(const const_unicode_string& TimerName, timer_type TimerType, access_mask DesiredAccess = all_access)
  {
    const object_attributes oa(TimerName);
    last_status_ = NtCreateTimer(this, DesiredAccess, &oa, TimerType);
  }

  ///\name open timer

  /** Opens a named timer object */
  explicit timer(const object_attributes& TimerName, access_mask DesiredAccess = modify_state)
  {
    last_status_ = NtOpenTimer(this, DesiredAccess, &TimerName);
  }
  explicit timer(const const_unicode_string& TimerName, access_mask DesiredAccess = modify_state)
  {
    const object_attributes oa(TimerName);
    last_status_ = NtOpenTimer(this, DesiredAccess, &oa);
  }

  /** Activates the specified waitable timer. */
  template <class Clock, class Duration>
  bool set(const std::chrono::time_point<Clock, Duration>& abs_time, timer_apc_routine* TimerApcRoutine, void* TimerContext)
  {
    assert(get());
    return success(last_status_ = NtSetTimer(get(), std::chrono::duration_cast<system_duration>(abs_time.time_since_epoch()).count(), TimerApcRoutine, TimerContext, true, 0, nullptr));
  }

  template <class Clock, class Duration, class PeriodRep, class PeriodType>
  bool set(const std::chrono::time_point<Clock, Duration>& abs_time, timer_apc_routine* TimerApcRoutine, void* TimerContext, const std::chrono::duration<PeriodRep, PeriodType>& Period)
  {
    assert(get());
    int32_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(Period).count();
    return success(last_status_ = NtSetTimer(get(), std::chrono::duration_cast<system_duration>(abs_time.time_since_epoch()).count(), TimerApcRoutine, TimerContext, true, static_cast<int32_t>(ms), nullptr));
  }

  /** Activates the specified waitable timer. */
  template <class Rep, class Period>
  bool set(const std::chrono::duration<Rep, Period>& rel_time, timer_apc_routine* TimerApcRoutine, void* TimerContext)
  {
    assert(get());
    return success(last_status_ = NtSetTimer(get(), -1i64*std::chrono::duration_cast<system_duration>(rel_time).count(), TimerApcRoutine, TimerContext, true, 0, nullptr));
  }

  template <class Rep, class Period, class PeriodRep, class PeriodType>
  bool set(const std::chrono::duration<Rep, Period>& rel_time, const std::chrono::duration<PeriodRep, PeriodType>& Period, timer_apc_routine* TimerApcRoutine, void* TimerContext)
  {
    assert(get());
    int64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(Period).count();
    return success(last_status_ = NtSetTimer(get(), -1i64*std::chrono::duration_cast<system_duration>(rel_time).count(), TimerApcRoutine, TimerContext, true, static_cast<int32_t>(ms), nullptr));
  }

  /** Sets the specified waitable timer to the inactive state. */
  bool cancel()
  {
    assert(get());
    bool re = false;
    return last_status_ = NtCancelTimer(get(), &re), re;
  }

  /** Queries the state and remaining time of waitable timer. */
  bool query(int64_t& remaining, bool& state) const
  {
    assert(get());
    timer_basic_information tbi;
    bool re = success(last_status_ = NtQueryTimer(get(), tbi.info_class_type, &tbi, sizeof(tbi), nullptr));
    if(re)
      remaining = tbi.RemainingTime,
      state = tbi.TimerState;
    return re;
  }

  ///\}
};


}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_TIMER
