/**\file*********************************************************************
 *                                                                     \brief
 *  Kernel Mode pools
 *
 ****************************************************************************
 */
#ifndef NTL__KM_POOL
#define NTL__KM_POOL
#pragma once

#include "../cstddef"

#ifndef NTL__POOL_TAG
#define NTL__POOL_TAG 'LTN_'  // _NTL
#endif

#ifndef NTL_NO_AUTOLINK
# pragma comment(lib,    "ntoskrnl.lib")
#endif

namespace ntl {
namespace km {


enum pool_type
{
  NonPagedPool,
  PagedPool,
  NonPagedPoolMustSucceed,
  DontUseThisType,
  NonPagedPoolCacheAligned,
  PagedPoolCacheAligned,
  NonPagedPoolCacheAlignedMustS,
  MaxPoolType,
  NonPagedPoolSession = 32,
  PagedPoolSession,
  NonPagedPoolMustSucceedSession,
  DontUseThisTypeSession,
  NonPagedPoolCacheAlignedSession,
  PagedPoolCacheAlignedSession,
  NonPagedPoolCacheAlignedMustSSession,
};


NTL_EXTERNAPI
__noalias
void * __restrict __stdcall
  ExAllocatePoolWithTag(pool_type, size_t, unsigned long tag);

NTL_EXTERNAPI
__noalias
void __stdcall
  ExFreePoolWithTag(void * p, unsigned long tag);


template<pool_type PoolType = PagedPool>
class pool
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    __noalias
    static __forceinline
    void * __restrict alloc(size_t size, unsigned long tag = NTL__POOL_TAG)
    {
      return ExAllocatePoolWithTag(PoolType, size, tag);
    }

    __noalias
    static __forceinline
    void free(void * const p, unsigned long tag = NTL__POOL_TAG)
    {
      ExFreePoolWithTag(p, tag);
    }

};//class pool


}//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_POOL
