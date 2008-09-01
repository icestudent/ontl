/**\file*********************************************************************
 *                                                                     \brief
 *  NT time
 *
 ****************************************************************************
 */

#ifndef NTL__NT_TIME
#define NTL__NT_TIME

#include "../stdint.h"
#include "basedef.hxx"
#include "shared_data.hxx"

namespace ntl {
namespace nt {

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

typedef system_time::type systime_t;
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


}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_TIME
