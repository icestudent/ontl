/**\file*********************************************************************
 *                                                                     \brief
 *  Diagnostics library [19 lib.diagnostics]
 *  Exception classes [19.1 lib.std.exceptions]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_STDEXCEPT_FWD
#define NTL__STLX_STDEXCEPT_FWD
#pragma once

#ifndef NTL__STLX_EXCPTDEF
#include "excptdef.hxx"
#endif

namespace std
{
  template<class charT>
    struct char_traits;

  template<class T>
    class allocator;

  template <class charT,
            class traits    = char_traits<charT>,
            class Allocator = allocator<charT> >
  class basic_string;
  typedef basic_string<char> string;

  class logic_error;
    class domain_error;
    class invalid_argument;
    class length_error;
    class out_of_range;

  class runtime_error;
    class range_error;
    class overflow_error;
    class underflow_error;

  ///\name exception helpers
  inline void __throw_out_of_range(const char* msg = "out of range");
  inline void __throw_length_error(const char* msg = "length exceeds its maximum allowable size");
  inline void __throw_invalid_argument(const char* msg = "invalid argument");
  ///\}
}

#if STLX__USE_EXCEPTIONS
# include "stdexcept.hxx"
#else
# include "cassert.hxx"
namespace std {
#pragma warning(push)
#pragma warning(disable:4100)// 'msg' : unreferenced formal parameter
  inline void __throw_out_of_range(const char* msg)      { _assert_string(msg); }
  inline void __throw_length_error(const char* msg)      { _assert_string(msg); }
  inline void __throw_invalid_argument(const char* msg)  { _assert_string(msg); }
#pragma warning(pop)
}

#endif

#endif // NTL__STLX_STDEXCEPT_FWD
