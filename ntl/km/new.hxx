/**\file*********************************************************************
 *                                                                     \brief
 *  new and delete operators
 *
 ****************************************************************************
 */
#ifndef NTL__KM_NEW
#define NTL__KM_NEW
#pragma once

namespace ntl {
  namespace km {
    __declare_tag constexpr const std::nothrow_t nonpaged;
  }
}

#ifndef NTL_NO_NEW

#include "pool.hxx"
#include "../stlx/new.hxx"

#ifdef __ICL
# pragma warning(push)
# pragma warning(disable:522) // function attribute redeclared after called
#endif

#if _MSC_FULL_VER >= 190023725
# pragma warning(push)
# pragma warning(disable:4595) // non-member operator new or delete functions may not be declared inline
#endif

///\name  Single-object forms

__forceinline
void * __cdecl
  operator new(std::size_t size)
{
  return ntl::km::pool<ntl::km::PagedPool>::alloc(size);
}

__forceinline
void __cdecl
  operator delete(void* ptr) __ntl_nothrow
{
  if ( ptr ) ntl::km::pool<ntl::km::PagedPool>::free(ptr);
}

__forceinline
void __cdecl operator delete(void* ptr, std::size_t) __ntl_nothrow
{
  operator delete(ptr);
}

__forceinline
void * __cdecl
  operator new(std::size_t size, const std::nothrow_t&) __ntl_nothrow
{
  return ntl::km::pool<ntl::km::NonPagedPool>::alloc(size);
}

__forceinline
void __cdecl
  operator delete(void* ptr, const std::nothrow_t&) __ntl_nothrow
{
  if ( ptr ) ntl::km::pool<ntl::km::NonPagedPool>::free(ptr);
}

__forceinline
void __cdecl operator delete(void* ptr, std::size_t, const std::nothrow_t& tag) __ntl_nothrow
{
  operator delete(ptr, tag);
}


///\name  Array forms

__forceinline
void * __cdecl
  operator new[](std::size_t size)
{
  return ntl::km::pool<ntl::km::PagedPool>::alloc(size);
}

__forceinline
void __cdecl
  operator delete[](void* ptr) __ntl_nothrow
{
  if ( ptr ) ntl::km::pool<ntl::km::PagedPool>::free(ptr);
}

__forceinline
void __cdecl operator delete[](void* ptr, std::size_t) __ntl_nothrow
{
  operator delete[](ptr);
}

__forceinline
void * __cdecl
  operator new[](std::size_t size, const std::nothrow_t&) __ntl_nothrow
{
  return ntl::km::pool<ntl::km::NonPagedPool>::alloc(size);
}

__forceinline
void __cdecl
  operator delete[](void* ptr, const std::nothrow_t&) __ntl_nothrow
{
  if ( ptr ) ntl::km::pool<ntl::km::NonPagedPool>::free(ptr);
}

__forceinline
void __cdecl operator delete[](void* ptr, std::size_t, const std::nothrow_t& tag) __ntl_nothrow
{
  operator delete[](ptr, tag);
}

#if defined(__ICL) || _MSC_FULL_VER >= 190023725
# pragma warning(pop)
#endif

#endif//#ifndef NTL_NO_NEW

#endif//#ifndef NTL__KM_NEW
