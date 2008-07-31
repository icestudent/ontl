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

inline clock_t clock(void) { return ntl::nt::query_system_time(); }

//time_t time(time_t *timer);

/**@} lib_date_time */
/**@} lib_utilities */

}//namespace std

#endif//#ifndef NTL__STLX_CTIME
