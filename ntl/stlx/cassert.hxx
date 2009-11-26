/**\file*********************************************************************
 *                                                                     \brief
 *  Assertions [lib.assertions 19.2]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_CASSERT
#define NTL__STLX_CASSERT
#pragma once

#include "cstddef.hxx"

//namespace std {


namespace ntl
{
  typedef void (*assert_handler)(const char* expr, const char* file, int line);

  __declspec(selectany) assert_handler __assert_handler = 0;

  inline assert_handler set_assert_handler(assert_handler handler)
  {
    assert_handler const a = __assert_handler;
    __assert_handler = handler;
    return a;
  }
} // ntl

/// Custom \c assert handler routine
typedef ntl::assert_handler __assert_handler;

/** Sets a custom \c assert handler */
inline __assert_handler __set_assert_handler(__assert_handler handler)
{
  return ntl::set_assert_handler(handler);
}

/** \c assert caller */
inline void __ntl_assert(const char* expr, const char* file, int line)
{
  ntl::assert_handler handler = ntl::__assert_handler;
  if(handler)
    handler(expr, file, line);
  else
    __debugbreak();
}

/// ISO C 7.2/1 The assert macro is redefined according to the current state
///             of NDEBUG each time that <assert.h> is included.
#undef assert

/// \c assert macros
#ifndef NTL__DEBUG
  #define assert(expr) __noop
  #define _assert_msg(msg) __noop
  #define _assert_string(msg) __noop
#else
  #define assert(expr) \
    if ( !!(expr) ); else if(ntl::__assert_handler)\
      __ntl_assert("Assertion ("#expr") failed in "__func__,__FILE__,__LINE__);\
      else __debugbreak();\
    ((void)0)
#define _assert_msg(msg) \
  if(ntl::__assert_handler)\
    __ntl_assert("Assertion (" msg ") failed in "__func__,__FILE__,__LINE__);\
  else __debugbreak();\
      ((void)0)
#define _assert_string(msg) \
  if(ntl::__assert_handler)\
  __ntl_assert(msg,__FILE__,__LINE__);\
  else __debugbreak();\
  ((void)0)
#endif

//}//namespace std

#ifdef _MSC_VER
namespace std 
{
  namespace __
  {
    extern "C" inline void __cdecl purecall_handler(void)
    {
      _assert_msg("pure virtual function called");
    }
    static void (__cdecl *__pchandler__)() = &purecall_handler;
  }
}
#ifdef _M_X64
# pragma comment(linker, "/alternatename:_purecall=purecall_handler")
#else
# pragma comment(linker, "/alternatename:__purecall=_purecall_handler")
#endif

#endif // msc
#endif//#ifndef NTL__STLX_CASSERT
