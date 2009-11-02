/**\file*********************************************************************
 *                                                                     \brief
 *  NT native heap
 *
 ****************************************************************************
 */
#ifndef NTL__NT_HEAP
#define NTL__NT_HEAP
#pragma once

#include "basedef.hxx"
#include "peb.hxx"

#ifndef NTL__NO_AUTOLINK
# pragma comment(lib, "ntdll.lib")
#endif

namespace ntl {
namespace nt {

///\name  Legacy API

struct rtl_heap { };

typedef rtl_heap * heap_ptr;

struct rtl_heap_parameters;


NTL__EXTERNAPI
__declspec(restrict)
heap_ptr __stdcall
  RtlCreateHeap(
    uint32_t              Flags,
    void *                HeapBase    __optional,
    size_t                ReserveSize __optional,
    size_t                CommitSize  __optional,
    void *                Lock        __optional,
    rtl_heap_parameters * Parameters  __optional
    );

NTL__EXTERNAPI
ntstatus __stdcall
  RtlDestroyHeap(
    heap_ptr HeapHandle
    );

NTL__EXTERNAPI
__declspec(restrict)
void * __stdcall
  RtlAllocateHeap(
    heap_ptr  HeapHandle,
    uint32_t  Flags,
    size_t    Size
    );

NTL__EXTERNAPI
bool __stdcall
  RtlFreeHeap(
    heap_ptr      HeapHandle,
    uint32_t      Flags,
    const void *  BaseAddress
    );

NTL__EXTERNAPI
size_t
__stdcall
RtlSizeHeap(
    heap_ptr  HeapHandle,
    uint32_t Flags,
    const void* BaseAddress
    );

NTL__EXTERNAPI
ntstatus
__stdcall
RtlZeroHeap(
    heap_ptr  HeapHandle,
    uint32_t Flags
    );

NTL__EXTERNAPI
void
__stdcall
RtlProtectHeap(
    heap_ptr  HeapHandle,
    bool MakeReadOnly
    );

NTL__EXTERNAPI
bool
__stdcall
RtlLockHeap(
    heap_ptr  HeapHandle
    );

NTL__EXTERNAPI
bool
__stdcall
RtlUnlockHeap(
    heap_ptr  HeapHandle
    );

NTL__EXTERNAPI
void*
__stdcall
RtlReAllocateHeap(
    heap_ptr  HeapHandle,
    uint32_t Flags,
    void* BaseAddress,
    size_t Size
    );

NTL__EXTERNAPI
bool
__stdcall
RtlGetUserInfoHeap(
    heap_ptr  HeapHandle,
    uint32_t Flags,
    const void* BaseAddress,
    void* *UserValue __optional,
    uint32_t* UserFlags __optional
    );

NTL__EXTERNAPI
bool
__stdcall
RtlSetUserValueHeap(
    heap_ptr  HeapHandle,
    uint32_t Flags,
    const void* BaseAddress,
    void* UserValue
    );

NTL__EXTERNAPI
bool
__stdcall
RtlSetUserFlagsHeap(
    heap_ptr  HeapHandle,
    uint32_t Flags,
    const void* BaseAddress,
    uint32_t UserFlagsReset,
    uint32_t UserFlagsSet
    );

struct rtl_heap_tag_info
{
    uint32_t NumberOfAllocations;
    uint32_t NumberOfFrees;
    size_t BytesAllocated;
};


NTL__EXTERNAPI
uint32_t
__stdcall
RtlCreateTagHeap(
    heap_ptr  HeapHandle,
    uint32_t Flags,
    wchar_t* TagPrefix __optional,
    wchar_t* TagNames
    );

NTL__EXTERNAPI
wchar_t*
__stdcall
RtlQueryTagHeap(
    heap_ptr  HeapHandle,
    uint32_t Flags,
    uint16_t TagIndex,
    bool ResetCounters,
    rtl_heap_tag_info* TagInfo __optional
    );

NTL__EXTERNAPI
ntstatus
__stdcall
RtlExtendHeap(
    heap_ptr  HeapHandle,
    uint32_t Flags,
    void* Base,
    size_t Size
    );

NTL__EXTERNAPI
size_t
__stdcall
RtlCompactHeap(
    heap_ptr  HeapHandle,
    uint32_t Flags
    );

NTL__EXTERNAPI
bool
__stdcall
RtlValidateHeap(
    heap_ptr  HeapHandle,
    uint32_t Flags,
    const void* BaseAddress
    );

NTL__EXTERNAPI
bool
__stdcall
RtlValidateProcessHeaps();

NTL__EXTERNAPI
uint32_t
__stdcall
RtlGetProcessHeaps(
    uint32_t NumberOfHeaps,
    void* *ProcessHeaps
    );


typedef
  ntstatus __stdcall 
  rtl_enum_heaps_routine(
    heap_ptr  HeapHandle,
    void* Parameter
    );

NTL__EXTERNAPI
ntstatus
__stdcall
RtlEnumProcessHeaps(
    rtl_enum_heaps_routine* EnumRoutine,
    void* Parameter
    );

struct rtl_heap_usage_entry 
{
    rtl_heap_usage_entry *Next;
    void* Address;
    size_t Size;
    uint16_t AllocatorBackTraceIndex;
    uint16_t TagIndex;
};

struct rtl_heap_usage {
    uint32_t Length;
    size_t BytesAllocated;
    size_t BytesCommitted;
    size_t BytesReserved;
    size_t BytesReservedMaximum;
    rtl_heap_usage_entry* Entries;
    rtl_heap_usage_entry* AddedEntries;
    rtl_heap_usage_entry* RemovedEntries;
    uintptr_t Reserved[ 8 ];
};

NTL__EXTERNAPI
ntstatus
__stdcall
RtlUsageHeap(
    heap_ptr  HeapHandle,
    uint32_t Flags,
    rtl_heap_usage* Usage
    );

struct rtl_heap_walk_entry
{
    void*   DataAddress;
    size_t   DataSize;
    uint8_t   OverheadBytes;
    uint8_t   SegmentIndex;
    uint16_t  Flags;
    union   {
        struct {
            size_t Settable;
            uint16_t TagIndex;
            uint16_t AllocatorBackTraceIndex;
            uint32_t Reserved[ 2 ];
        } Block;
        struct {
            uint32_t CommittedSize;
            uint32_t UnCommittedSize;
            void* FirstEntry;
            void* LastEntry;
        } Segment;
    };
};

NTL__EXTERNAPI
ntstatus
__stdcall
RtlWalkHeap(
    heap_ptr  HeapHandle,
    rtl_heap_walk_entry* Entry
    );

struct rtl_heap_entry 
{
    size_t Size;
    uint16_t Flags;
    uint16_t AllocatorBackTraceIndex;
    union {
        struct {
            size_t Settable;
            uint32_t Tag;
        } s1;
        struct {
            size_t CommittedSize;
            void* FirstBlock;
        } s2;
    } u;
};

struct rtl_heap_tag 
{
    uint32_t NumberOfAllocations;
    uint32_t NumberOfFrees;
    size_t BytesAllocated;
    uint16_t TagIndex;
    uint16_t CreatorBackTraceIndex;
    wchar_t TagName[ 24 ];
} ;

struct rtl_heap_information 
{
    void* BaseAddress;
    uint32_t Flags;
    uint16_t EntryOverhead;
    uint16_t CreatorBackTraceIndex;
    size_t BytesAllocated;
    size_t BytesCommitted;
    uint32_t NumberOfTags;
    uint32_t NumberOfEntries;
    uint32_t NumberOfPseudoTags;
    uint32_t PseudoTagGranularity;
    uint32_t Reserved[ 5 ];
    rtl_heap_tag* Tags;
    rtl_heap_entry* Entries;
};

struct rtl_process_heaps 
{
    uint32_t NumberOfHeaps;
    rtl_heap_information Heaps[ 1 ];
};

enum heap_information_class 
{
    HeapCompatibilityInformation
};

struct rtl_heap_parameters
{
  uint32_t Length;
  uint32_t SegmentReserve;
  uint32_t SegmentCommit;
  uint32_t DeCommitFreeBlockThreshold;
  uint32_t DeCommitTotalFreeThreshold;
  uint32_t MaximumAllocationSize;
  uint32_t VirtualMemoryThreshold;
  uint32_t InitialCommit;
  uint32_t InitialReserve;
  ntstatus (__stdcall *CommitRoutine)(void* Base, void** CommitAddress, uint32_t* CommitSize);
  uint32_t Reserved[2];
};

NTL__EXTERNAPI
ntstatus
__stdcall
RtlSetHeapInformation (
    heap_ptr  HeapHandle,
    heap_information_class HeapInformationClass,
    const void* HeapInformation __optional,
    size_t HeapInformationLength __optional
    );

NTL__EXTERNAPI
ntstatus
__stdcall
RtlQueryHeapInformation (
    heap_ptr  HeapHandle,
    heap_information_class HeapInformationClass,
    void* HeapInformation __optional,
    size_t HeapInformationLength __optional,
    size_t* ReturnLength __optional
    );

uint32_t
__stdcall
RtlMultipleAllocateHeap (
    heap_ptr  HeapHandle,
    uint32_t Flags,
    size_t Size,
    uint32_t Count,
    void* * Array
    );

uint32_t
__stdcall
RtlMultipleFreeHeap (
    heap_ptr  HeapHandle,
    uint32_t Flags,
    uint32_t Count,
    void* * Array
    );
///}

class heap
{
  heap(const heap&) __deleted;
  const heap& operator=(const heap&) __deleted;

  ///////////////////////////////////////////////////////////////////////////
public:

  enum flag
  {
    none,
    /** disable heap synchronization */
    no_serialize              = 0x00000001,
    growable                  = 0x00000002,
    /** generates exception on failures */
    generate_exceptions       = 0x00000004,
    zero_memory               = 0x00000008,
    /** grow memory in place only */
    realloc_in_place_only     = 0x00000010,
    tail_checking_enabled     = 0x00000020,
    free_checking_enabled     = 0x00000040,
    /** leaves adjacent blocks of heap memory separate when they are freed */
    disable_coalesce_on_free  = 0x00000080,
    create_align_16           = 0x00010000,
    create_enable_tracing     = 0x00020000,
    create_enable_execute     = 0x00040000
  };

  friend flag operator | (flag m, flag m2) { return bitwise_or(m, m2); }

  static __forceinline
  void *
    alloc(
      heap_ptr  heap,
      size_t    size,
      flag      flags = none)
  {
    return RtlAllocateHeap(heap, flags, size);
  }

  static __forceinline
  heap_ptr
    create(
      flag        flags           = growable,
      size_t      initial_size    = 0,
      size_t      maximum_size    = 0,
      void  *     base            = 0
      )
  {
    return RtlCreateHeap(flags, base, maximum_size, initial_size,
      0 /* user-allocated heap lock not used */, 0);
  }

  static __forceinline
  ntstatus
    destroy(heap_ptr hp)
  {
    return RtlDestroyHeap(hp);
  }

  static __forceinline
  bool
    free(
      heap_ptr            heap,
      const void * const  p,
      flag                flags = none)
  {
    return RtlFreeHeap(heap, flags, p);
  }

  static __forceinline
  bool validate(heap_ptr heap, const void* const p = NULL, flag flags = none)
  {
    return RtlValidateHeap(heap, flags, p);
  }

  static __forceinline
  bool validate_all()
  {
    return RtlValidateProcessHeaps();
  }


  explicit __forceinline
  heap(
    flag      flags           = growable,
    size_t    initial_size    = 0,
    size_t    maximum_size    = 0,
    void  *   base            = 0
    )
    : h(create(flags, initial_size, maximum_size, base))
  {/**/}

  ~heap() { destroy(h); }

#ifdef NTL__CXX_RV
  heap(heap&& rh)
    :h(rh.h)
  {
    rh.h = nullptr;
  }

  heap& operator=(heap&& rh)
  {
    if(this != &rh){
      destroy(h);
      h = rh.h;
      rh.h = nullptr;
    }
    return *this;
  }
#endif

  void * alloc(
    size_t  size,
    flag    flags = none) const
  {
    return alloc(h, size, flags);
  }

  bool free(
    const void * const  p,
    flag                flags = none) const
  {
    return free(h, p, flags);
  }

  heap_ptr get() const { return h; }

  // locks heap
  __forceinline
  bool lock()
  {
    return RtlLockHeap(h);
  }

  // unlocks heap
  __forceinline
  bool unlock()
  {
    return RtlUnlockHeap(h);
  }

  // makes heap read only
  __forceinline
  void protect()
  {
    RtlProtectHeap(h, true);
  }

  // makes heap writeable
  __forceinline
  void unprotect()
  {
    RtlProtectHeap(h, false);
  }

  // validate heap entry
  bool validate(const void* const p, flag flags = none)
  {
    return validate(h, p, flags);
  }

  // validate entire heap
  bool validate(flag flags = none)
  {
    return validate(h, NULL, flags);
  }

private:
  heap_ptr h;

};//class heap


struct process_heap
{
  process_heap() {}

  __forceinline
  void * alloc(
    size_t      size,
    heap::flag  flags = heap::none) const
  {
    return heap::alloc(*this, size, flags);
  }

  __forceinline
  bool free(
    const void * const p,
    heap::flag flags = heap::none) const
  {
    return heap::free(*this, p, flags);
  }

  __forceinline
    operator heap_ptr() const
  {
    heap_ptr p = teb::get(&teb::ProcessEnvironmentBlock)->ProcessHeap;
    return p;
  }
};


}//namespace nt
}//namespace ntl


#endif//#ifndef NTL__NT_HEAP
