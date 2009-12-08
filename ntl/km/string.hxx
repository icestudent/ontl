/**\file*********************************************************************
 *                                                                     \brief
 *  Kernel Mode NT string types
 *
 ****************************************************************************
 */
#ifndef NTL__KM_STRING
#define NTL__KM_STRING
#pragma once

#include "../nt/string.hxx"
#include "pool.hxx"

namespace ntl {
namespace km {

  /** Native string allocator \see nt::string_allocator */
  template<class charT>
  class string_allocator:
    public nt::string_allocator<charT>
  {
    // 'Strg'
    static const uint32_t km_string_tag = 'grtS';
  public:
    __noalias __forceinline pointer /*__restrict*/ allocate(size_type n, std::allocator<void>::const_pointer = 0)
    {
      return pool<PagedPool>::alloc(n*sizeof(value_type), km_string_tag);
    }

    __noalias __forceinline void deallocate(pointer p, size_type)
    {
      pool<PagedPool>::free(p, km_string_tag)
    }
  };

using  nt::unicode_string;
using  nt::const_unicode_string;

using  nt::ansi_string;
using  nt::const_ansi_string;

}//namespace ntl
}//namespace nt

#endif//#ifndef NTL__KM_STRING
