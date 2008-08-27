/**\file*********************************************************************
 *                                                                     \brief
 *  Integer types ISO9899: 7.18
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CTIME
#define NTL__STLX_CTIME

#include "cstddef.hxx"
#include "../nt/time.hxx"

namespace std {

/**\addtogroup  lib_utilities *** 20 General utilities library [utilities] **
 *@{*/
/**\addtogroup  lib_date_time *** 20.9 Date and time functions [date.time] **
 *@{*/

#define CLOCKS_PER_SEC system_time_resolution

typedef ntl::nt::systime_t clock_t;
typedef ntl::nt::systime_t time_t;

inline clock_t clock(void) { return ntl::nt::query_system_time(); }

inline time_t time(time_t* timer = 0)
{
  // Number of 100 nanosecond units from 1/1/1601 to 1/1/1970
  static const int64_t epoch_bias = 116444736000000000i64;
  ntl::nt::systime_t ntime = ntl::nt::query_system_time();
  time_t ct = static_cast<time_t>((ntime - epoch_bias) / 10000000i64);
  if(timer)
    *timer = ct;
  return ct;
}

/**@} lib_date_time */
/**@} lib_utilities */

}//namespace std

#endif//#ifndef NTL__STLX_CTIME
