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

#ifndef STLX__USE_RTTI
  #if defined(_MSC_VER)
    #ifdef _CPPRTTI
      #ifdef NTL_KM
        #pragma message("Kernel mode RTTI support was not implemented yet")
        #define STLX__USE_RTTI 0
      #else
        #define STLX__USE_RTTI 1
      #endif
    #else
      #define STLX__USE_RTTI 0
    #endif
  #elif defined(__GNUC__)
    #ifdef __GXX_RTTI
      #define STLX__USE_RTTI 1
    #else
      #define STLX__USE_RTTI 0
    #endif
  #else
    #error define STLX__USE_RTTI
  #endif
#endif

#if (STLX__USE_EXCEPTIONS == 1) && (STLX__USE_RTTI == 0)
// C++ exceptions with the disabled RTTI
# undef  STLX__USE_EXCEPTIONS
# define STLX__USE_EXCEPTIONS 3
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

#elif STLX__USE_EXCEPTIONS == 3
  // C++ exceptions without RTTI
  #define __ntl_try       try
  #define __ntl_catch     catch
  #define __ntl_throw(X)  throw X
  #define __ntl_rethrow   throw
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
