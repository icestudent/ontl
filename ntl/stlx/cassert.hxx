/**\file*********************************************************************
 *                                                                     \brief
 *  Assertions [lib.assertions 19.2]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CASSERT
#define NTL__STLX_CASSERT

//namespace std {

#ifdef _DEBUG
#define NTL__DEBUG
#endif

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
};

/// ISO C 7.2/1 The assert macro is redefined according to the current state
///             of NDEBUG each time that <assert.h> is included.
#undef assert

/// \c assert macros
#ifdef NDEBUG
  #define assert(expr)
#else
  #define assert(expr) \
    if ( !!(expr) ); else if(ntl::__assert_handler)\
      __ntl_assert("Assertion ("#expr") failed in "__func__,__FILE__,__LINE__);\
      else __debugbreak();\
    ((void)0)
#endif

//}//namespace std

#endif//#ifndef NTL__STLX_CASSERT
