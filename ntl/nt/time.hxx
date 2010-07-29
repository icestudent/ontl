/**\file*********************************************************************
 *                                                                     \brief
 *  NT time & timers
 *
 ****************************************************************************
 */
#ifndef NTL__NT_TIME
#define NTL__NT_TIME
#pragma once

#include "basedef.hxx"
#include "shared_data.hxx"

namespace ntl {
namespace nt {

  enum timer_type {
    NotificationTimer,
    SynchronizationTimer
  };

  struct time_fields
  {
    int16_t Year;        // range [1601...]
    int16_t Month;       // range [1..12]
    int16_t Day;         // range [1..31]
    int16_t Hour;        // range [0..23]
    int16_t Minute;      // range [0..59]
    int16_t Second;      // range [0..59]
    int16_t Milliseconds;// range [0..999]
    int16_t Weekday;     // range [0..6] == [Sunday..Saturday]
  };

typedef systime_t file_time;

static const systime_t system_time_resolution = system_time::resolution;

systime_t inline query_system_time()
{
  return user_shared_data::instance().SystemTime.get();
}

NTL__EXTERNAPI
ntstatus __stdcall
  NtQuerySystemTime(systime_t* SystemTime);

NTL__EXTERNAPI
void __stdcall
  RtlTimeToTimeFields(systime_t* Time, time_fields* TimeFields);

NTL__EXTERNAPI
ntstatus __stdcall
  RtlSystemTimeToLocalTime(systime_t* SystemTime, int64_t* LocalTime);

///\name Timer API

typedef void timer_apc_routine(void* TimerContext, uint32_t TimerLowValue, int32_t TimerHighValue);

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

NTL__EXTERNAPI
ntstatus __stdcall NtCreateTimer(handle* TimerHandle, uint32_t DesiredAccess, const object_attributes* ObjectAttributes, timer_type TimerType);

NTL__EXTERNAPI
ntstatus __stdcall NtOpenTimer(handle* TimerHandle, uint32_t DesiredAccess, const object_attributes* ObjectAttributes);

NTL__EXTERNAPI
ntstatus __stdcall NtSetTimer(handle TimerHandle, const int64_t& DueTime, timer_apc_routine* TimerApcRoutine, void* TimerContext, bool ResumeTimer, int32_t Period, bool* PreviousState);

NTL__EXTERNAPI
ntstatus __stdcall NtCancelTimer(handle TimerHandle, bool* CurrentState);

NTL__EXTERNAPI
ntstatus __stdcall NtQueryTimer(handle TimerHandle, timer_information_class TimerInformationClass, void* TimerInformation, uint32_t TimerInformationLength, uint32_t* ReturnLength);

/**
 *	@brief Timer
 **/
class timer:
  public handle
{
  ///\name create timer
  explicit timer(timer_type TimerType, access_mask DesiredAccess)
  {
    const object_attributes oa;
    NtCreateTimer(this, DesiredAccess, &oa, TimerType);
  }

  explicit timer(timer_type TimerType, const const_unicode_string& TimerName, access_mask DesiredAccess)
  {
    const object_attributes oa(TimerName);
    NtCreateTimer(this, DesiredAccess, &oa, TimerType);
  }

  ///\name open timer
  explicit timer(access_mask DesiredAccess)
  {
    const object_attributes oa;
    NtOpenTimer(this, DesiredAccess, &oa);
  }

  explicit timer(const const_unicode_string& TimerName, access_mask DesiredAccess)
  {
    const object_attributes oa(TimerName);
    NtOpenTimer(this, DesiredAccess, &oa);
  }

  template <class Clock, class Duration>
  bool set(const std::chrono::time_point<Clock, Duration>& abs_time, timer_apc_routine* TimerApcRoutine, void* TimerContext)
  {
    return success(NtSetTimer(get(), std::chrono::duration_cast<system_duration>(abs_time.time_since_epoch()).count(), TimerApcRoutine, TimerContext, true, 0, nullptr));
  }

  template <class Rep, class Period>
  bool set(const std::chrono::duration<Rep, Period>& rel_time, timer_apc_routine* TimerApcRoutine, void* TimerContext)
  {
    return success(NtSetTimer(get(), -1i64*std::chrono::duration_cast<system_duration>(rel_time).count(), TimerApcRoutine, TimerContext, true, 0, nullptr));
  }

  bool cancel()
  {
    bool re = false;
    return NtCancelTimer(get(), &re), re;
  }

  bool query(int64_t& remaining, bool& state) const
  {
    timer_basic_information tbi;
    bool re = success(NtQueryTimer(get(), tbi.info_class_type, &tbi, sizeof(tbi), nullptr));
    if(re)
      remaining = tbi.RemainingTime,
      state = tbi.TimerState;
    return re;
  }

  ///\}
};

}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_TIME
