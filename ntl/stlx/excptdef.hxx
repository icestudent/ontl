/**\file*********************************************************************
 *                                                                     \brief
 *  18.7 Exception handling [support.exception]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_EXCPTDEF
#define NTL__STLX_EXCPTDEF
#pragma once

#ifndef NTL__STLX_CSTDDEF
#include "cstddef.hxx" // for STLX__USE_EXCEPTIONS
#endif

#if STLX__USE_EXCEPTIONS == 1
  // C++ exceptions
  #define __ntl_try       try
  #define __ntl_catch     catch
  #define __ntl_throw(X)  throw X
  #define __ntl_rethrow   throw
  #define __ntl_throws(...) throw(__VA_ARGS__)
  #define __ntl_nothrow   throw()

#elif STLX__USE_EXCEPTIONS == 2
  // SEH exceptions

  template<class E>
  inline void __ntl_seh_throw_impl(const E&){}

  #define __ntl_try       __try
  #define __ntl_catch     __except(1)
  #define __ntl_throw(X)  __ntl_seh_throw_impl(X)
  #define __ntl_rethrow (0)
  #define __ntl_throws(...)
  #define __ntl_nothrow


#else
  #define __ntl_try
  #define __ntl_catch(X)  while(false)
  #define __ntl_throw(X)  __ntl_throw_impl()
  #define __ntl_rethrow (0)
  #define __ntl_throws(...)
  #define __ntl_nothrow

  #pragma warning(disable:4127)// conditional expression is constant (because while(false))

  inline void __ntl_throw_impl() {}
#endif

#endif // NTL__STLX_EXCPTDEF
