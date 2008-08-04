/**\file*********************************************************************
 *                                                                     \brief
 *  NT time
 *
 ****************************************************************************
 */

#ifndef NTL__KM_TIME
#define NTL__KM_TIME

#include "basedef.hxx"
#include "../nt/shared_data.hxx"
#include "../nt/time.hxx"

namespace ntl {
  namespace km {

    using nt::time_fields;
    using nt::system_time;
    using nt::RtlTimeToTimeFields;

    struct ktimer
    {
      dispatcher_header Header;
      uint64_t          DueTime;
      list_entry        TimerListEntry;
      kdpc *            Dpc;
      long              Period;
    };

    __forceinline
      void KeQuerySystemTime(int64_t* CurrentTime)
    {
      *CurrentTime = user_shared_data::instance().SystemTime.get();
    }

    __forceinline
      void KeQueryTickCount(int64_t* TickCount)
    {
      *TickCount = user_shared_data::instance().TickCountQuad;
    }

    NTL__EXTERNAPI
      void __stdcall
      ExSystemTimeToLocalTime(int64_t* SystemTime, int64_t* LocalTime);


  }//namespace km
}//namespace ntl

#endif//#ifndef NTL__KM_TIME
