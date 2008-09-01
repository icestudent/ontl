/**\file*********************************************************************
 *                                                                     \brief
 *  NT native heap
 *
 ****************************************************************************
 */

#ifndef NTL__NT_HEAP
#define NTL__NT_HEAP

#include "basedef.hxx"
#include "peb.hxx"

#pragma comment(lib, "ntdll.lib")

namespace ntl {
namespace nt {

///\name  Legacy API

struct rtl_heap { };

typedef rtl_heap * heap_ptr;

struct rtl_heap_parameters;


__declspec(restrict)
NTL__EXTERNAPI
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

__declspec(restrict)
NTL__EXTERNAPI
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

///}

class heap
{
    heap(const heap&);
    const heap& operator=(const heap&);

  ///////////////////////////////////////////////////////////////////////////
  public:

    enum flag
    {
      no_serialize              = 0x00000001,
      growable                  = 0x00000002,
      generate_exceprions       = 0x00000004,
      zero_memory               = 0x00000008,
      realloc_in_place_only     = 0x00000010,
      tail_checking_enabled     = 0x00000020,
      free_checking_enabled     = 0x00000040,
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
        flag      flags = static_cast<flag>(0))
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
        flag                flags = static_cast<flag>(0))
      {
        return RtlFreeHeap(heap, flags, p);
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

    void * alloc(
      size_t  size,
      flag    flags = static_cast<flag>(0)) const
    {
      return alloc(h, size, flags);
    }

    bool free(
      const void * const  p,
      flag                flags = static_cast<flag>(0)) const
    {
      return free(h, p, flags);
    }

    heap_ptr get() const { return h; }

  private:
    heap_ptr h;

};//class heap


struct process_heap
{
    process_heap() {}

  __forceinline
  void * alloc(
    size_t      size,
    heap::flag  flags = static_cast<heap::flag>(0)) const
  {
    return heap::alloc(*this, size, flags);
  }

  __forceinline
  bool free(
    const void * const p,
    heap::flag flags = static_cast<heap::flag>(0)) const
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
