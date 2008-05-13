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
#include <new>

///\name  Single-object forms
inline
void * __cdecl
  operator new(std::size_t size) throw(std::bad_alloc)
{
  return ntl::nt::heap::alloc(ntl::nt::process_heap(), size);
}

inline
void __cdecl
  operator delete(void* ptr) throw()
{
  ntl::nt::heap::free(ntl::nt::process_heap(), ptr);
}

inline
void * __cdecl
  operator new(std::size_t size, const std::nothrow_t&) throw()
{
  return ntl::nt::heap::alloc(ntl::nt::process_heap(), size);
}

inline
void __cdecl
  operator delete(void* ptr, const std::nothrow_t&) throw()
{
  ntl::nt::heap::free(ntl::nt::process_heap(), ptr);
}


///\name  Array forms

inline
void * __cdecl
  operator new[](std::size_t size) throw(std::bad_alloc)
{
  return ntl::nt::heap::alloc(ntl::nt::process_heap(), size);
}

inline
void __cdecl
  operator delete[](void* ptr) throw()
{
  ntl::nt::heap::free(ntl::nt::process_heap(), ptr);
}

inline
void * __cdecl
  operator new[](std::size_t size, const std::nothrow_t&) throw()
{
  return ntl::nt::heap::alloc(ntl::nt::process_heap(), size);
}

inline
void __cdecl
  operator delete[](void* ptr, const std::nothrow_t&) throw()
{
  ntl::nt::heap::free(ntl::nt::process_heap(), ptr);
}


#endif//#ifndef NTL_NO_NEW 

#endif//#ifndef NTL__NT_NEW
