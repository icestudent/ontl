/**\file*********************************************************************
 *                                                                     \brief
 *  Lookaside Lists support
 *
 ****************************************************************************
 */
#ifndef NTL__KM_LOOKASIDE_LIST
#define NTL__KM_LOOKASIDE_LIST
#pragma once

#include "basedef.hxx"
#include "pool.hxx"


namespace ntl {
namespace km {


NTL_EXTERNAPI
slist_entry *  __fastcall
  ExInterlockedPopEntrySList(
    slist_header *  ListHead,
    kspin_lock *    Lock
    );

NTL_EXTERNAPI
slist_entry *  __fastcall
  ExInterlockedPushEntrySList(
    slist_header *  ListHead,
    slist_entry *   ListEntry,
    kspin_lock *    Lock
    );


struct general_lookaside_pool
{
  typedef
  void *
    allocate_function_t (
      pool_type PoolType,
      size_t    NumberOfBytes,
      uint32_t  Tag
      );

  typedef
  void
    free_function_t (void * Buffer);

  slist_header          ListHead;
  uint16_t              Depth;
  uint16_t              MaximumDepth;
  uint32_t              TotalAllocates;
  union
  {
    uint32_t            AllocateMisses;
    uint32_t            AllocateHits;
  };
  uint32_t              TotalFrees;
  union
  {
    uint32_t            FreeMisses;
    uint32_t            FreeHits;
  };
  pool_type             Type;
  uint32_t              Tag;
  uint32_t              Size;
  allocate_function_t * Allocate;
  free_function_t *     Free;
  list_entry            ListEntry;
  uint32_t              LastTotalAllocates;
  union
  {
    uint32_t            LastAllocateMisses;
    uint32_t            LastAllocateHits;
  };
  uint32_t              Future[2];
};
//STATIC_ASSERT(sizeof(general_lookaside_pool)==0x48);


#pragma warning(push)
#pragma warning(disable:4324) // 'ntl::km::general_lookaside' : structure was padded due to __declspec(align())
alignas(SYSTEM_CACHE_ALIGNMENT_SIZE)
struct general_lookaside : public general_lookaside_pool {};
#pragma warning(pop)


template<typename CellType = void>
struct npaged_lookaside_list : public general_lookaside
{
    kspin_lock  Lock__ObsoleteButDoNotDelete;

    typedef CellType  value_type;

  explicit
  npaged_lookaside_list(
    allocate_function_t *   Allocate    = 0,
    free_function_t *       Free        = 0,
    uint32_t                Tag         = NTL__POOL_TAG)
  {
    _initialize(this, Allocate, Free, Tag);
  }

  ~npaged_lookaside_list()
  {
    _delete(this);
  }

  static __forceinline
  void _initialize(
    npaged_lookaside_list * Lookaside,
    allocate_function_t *   Allocate    = 0,
    free_function_t *       Free        = 0,
    uint32_t                Tag         = NTL__POOL_TAG
    );

  static __forceinline
  void _delete(npaged_lookaside_list * Lookaside);

  value_type * allocate()
  {
    ++TotalAllocates;
    void * entry = ExInterlockedPopEntrySList(&ListHead, &Lock__ObsoleteButDoNotDelete);
    if ( !entry )
    {
      ++AllocateMisses;
      entry = Allocate(Type, Size, Tag);
    }
    return reinterpret_cast<value_type*>(entry);
  }

  void free(void * entry)
  {
    ++TotalFrees;
    if ( ListHead.Depth < Depth )
        ExInterlockedPushEntrySList(&ListHead, (slist_entry*)entry,
                                    &Lock__ObsoleteButDoNotDelete);
    else
    {
      ++FreeMisses;
      Free(entry);
    }
  }

};


NTL_EXTERNAPI
void __stdcall
  ExInitializeNPagedLookasideList(
    general_lookaside *                       Lookaside,
    general_lookaside::allocate_function_t *  Allocate  __optional,
    general_lookaside::free_function_t *      Free      __optional,
    uint32_t            Flags,
    size_t              Size,
    uint32_t            Tag,
    uint16_t            Depth
    );

NTL_EXTERNAPI
void __stdcall
  ExDeleteNPagedLookasideList(
    general_lookaside * Lookaside);

template<typename CellType>
void npaged_lookaside_list<CellType>::_delete(npaged_lookaside_list<CellType> * Lookaside)
{
  ExDeleteNPagedLookasideList(Lookaside);
}

template<typename CellType>
void npaged_lookaside_list<CellType>::_initialize(
  npaged_lookaside_list<CellType> * Lookaside,
  allocate_function_t *             Allocate,
  free_function_t *                 Free,
  uint32_t                          Tag)
{
  ExInitializeNPagedLookasideList(Lookaside, Allocate, Free, 0, sizeof(CellType), Tag, 0);
}


}//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_LOOKASIDE_LIST
