/**\file*********************************************************************
 *                                                                     \brief
 *  new and delete operators
 *
 ****************************************************************************
 */

#ifndef NTL__NT_NEW
#define NTL__NT_NEW

#ifndef NTL_NO_NEW

#include "heap.hxx"
#include "../stlx/new.hxx"

#ifdef __ICL
#pragma warning(disable:522) // function attribute redeclared after called
#endif

namespace ntl
{
  extern "C" volatile std::new_handler __new_handler;
  extern "C" __declspec(selectany) std::new_handler const __new_handler_null_stub = 0;
#ifdef _M_X64
  #pragma comment(linker, "/alternatename:__new_handler=__new_handler_null_stub")
#else
  #pragma comment(linker, "/alternatename:___new_handler=___new_handler_null_stub")
#endif
}

extern "C" void __cdecl abort();

namespace std
{
  using ::abort;
}

///\name  Single-object forms

__forceinline
void* __cdecl operator new(std::size_t size) throw(std::bad_alloc)
{
#ifdef NTL_NO_NEW_HANDLERS
  return ntl::nt::heap::alloc(ntl::nt::process_heap(), size);
#else
  void* ptr;
  for(;;) {
    ptr = ntl::nt::heap::alloc(ntl::nt::process_heap(), size);
    if(ptr)
      return ptr;

    std::new_handler nh = ntl::__new_handler;
  #if STLX__USE_EXCEPTIONS
    if(!nh)
      __ntl_throw(std::bad_alloc());
  #else
    if(!nh)
      std::abort();
  #endif
    nh();
  }
#endif
}

__forceinline
void __cdecl operator delete(void* ptr) __ntl_nothrow
{
  ntl::nt::heap::free(ntl::nt::process_heap(), ptr);
}

#ifndef NTL_NO_NEW_HANDLERS
inline void __safe_call_new_handler(std::new_handler nh)
{
#if STLX__USE_EXCEPTIONS == 1
    try {
      nh();
    } 
    catch(const std::bad_alloc&){
    }
#else
    __try {
      nh();
    }
    __except(1){
    }
#endif
}
#endif 

__forceinline
void* __cdecl operator new(std::size_t size, const std::nothrow_t&) __ntl_nothrow
{
#ifdef NTL_NO_NEW_HANDLERS
  return ntl::nt::heap::alloc(ntl::nt::process_heap(), size);
#else
  void* ptr;
  for(;;) {
    ptr = ntl::nt::heap::alloc(ntl::nt::process_heap(), size);
    if(ptr)
      return ptr;

    std::new_handler nh = ntl::__new_handler;
    if ( nh )
      __safe_call_new_handler(nh);
    return 0;
  }
#endif
}

__forceinline
void __cdecl
  operator delete(void* ptr, const std::nothrow_t&) __ntl_nothrow
{
  ntl::nt::heap::free(ntl::nt::process_heap(), ptr);
}


///\name  Array forms

__forceinline
void* __cdecl operator new[](std::size_t size) throw(std::bad_alloc)
{
  // NOTE: 5.3.4/7 (N2960)
/***
  If the value of that expression is such that the size of the allocated object
  would exceed the implementation-defined limit, no storage is obtained 
  and the new-expression terminates by throwing an exception of a type 
  that would match a handler (15.3) of type std::bad_array_new_length (18.6.2.2).
 ***/
  return operator new(size);
}

__forceinline
void __cdecl operator delete[](void* ptr) __ntl_nothrow
{
  ntl::nt::heap::free(ntl::nt::process_heap(), ptr);
}

__forceinline
void* __cdecl operator new[](std::size_t size, const std::nothrow_t&) __ntl_nothrow
{
  return operator new(size, std::nothrow);
}

__forceinline
void __cdecl operator delete[](void* ptr, const std::nothrow_t&) __ntl_nothrow
{
  ntl::nt::heap::free(ntl::nt::process_heap(), ptr);
}

#ifdef __ICL
#pragma warning(default:444)
#endif

#endif//#ifndef NTL_NO_NEW

#endif//#ifndef NTL__NT_NEW
