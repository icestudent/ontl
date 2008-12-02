/**\file*********************************************************************
 *                                                                     \brief
 *  18.7 Exception handling [support.exception]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_EXCPTDEF
#define NTL__STLX_EXCPTDEF


#ifndef STLX__USE_EXCEPTIONS
  #if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
    #ifdef _CPPUNWIND
      #define STLX__USE_EXCEPTIONS 1
    #else
      #define STLX__USE_EXCEPTIONS 0
    #endif
  #elif defined(__GNUC__)
    #ifdef __EXCEPTIONS
      #define STLX__USE_EXCEPTIONS 1
    #else
      #define STLX__USE_EXCEPTIONS 0
    #endif
  #else
    #error define STLX__USE_EXCEPTIONS
  #endif
#endif

#if STLX__USE_EXCEPTIONS
  #define __ntl_try       try
  #define __ntl_catch     catch
  #define __ntl_throw(X)  throw X
  #define __ntl_rethrow   throw
  #define __ntl_throws    throw
  #define __ntl_nothrow   throw()
#else
  #define __ntl_try
  #define __ntl_catch(X)  while(false)
  #pragma warning(disable:4127)// conditional expression is constant
  inline void __ntl_throw_impl() {}
  #define __ntl_throw(X)  __ntl_throw_impl()
  #define __ntl_rethrow (0)
  #define __ntl_throws(...)
  #define __ntl_nothrow
#endif

#endif // NTL__STLX_EXCPTDEF
