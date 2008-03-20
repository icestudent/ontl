/**\file*********************************************************************
 *                                                                     \brief
 *  18.7 Exception handling [support.exception]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_EXCEPTION
#define NTL__STLX_EXCEPTION

#ifndef STLX__USE_EXCEPTIONS
  #ifdef _MSC_VER
    #ifdef _CPPUNWIND
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
#define __ntl_throw(X)  throw(X)
#define __ntl_rethrow   throw
#else
#define __ntl_try
#define __ntl_catch(X)  while(false)
#pragma warning(disable:4127)// conditional expression is constant
inline void __ntl_throw_impl() {}
#define __ntl_throw(X)  __ntl_throw_impl()
#define __ntl_rethrow (0)
#endif

#ifdef _MSC_VER
//C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#pragma warning(disable:4290)
#endif

namespace std {

/**\addtogroup  lib_language_support *** Language support library [18] ******
 *@{*/

/**\defgroup  lib_support_exception **** Exception handling [18.6] **********
 *@{*/

/// 18.7.1 Class exception [exception]
class exception
{
  public:
    exception() throw() {}
    exception(const exception&) throw() {}
    exception& operator=(const exception&) throw() {}
    virtual ~exception() throw() {}
    virtual const char* what() const throw() { return "exception"; }
};

/// Class bad_exception [18.6.2.1 lib.bad.exception]
class bad_exception : public exception
{
  public:
    bad_exception() throw() {}
    bad_exception(const bad_exception&) throw() {}
    bad_exception& operator=(const bad_exception&) throw() {}
    virtual ~bad_exception() throw() {}
    virtual const char* what() const throw() { return "bad_exception"; }
};

/// Type unexpected_handler [18.6.2.2 lib.unexpected.handler]
/// @note an unexpected_handler shall not return.
typedef void (*unexpected_handler)();

/// set_unexpected [18.6.2.3 lib.set.unexpected]
unexpected_handler set_unexpected(unexpected_handler f) throw();

/// unexpected [18.6.2.4 lib.unexpected]
void unexpected();

/// Type terminate_handler [18.6.3.1 lib.terminate.handler]
typedef void (*terminate_handler)();

/// set_terminate [18.6.3.2 lib.set.terminate]
terminate_handler set_terminate(terminate_handler f) throw();

/// terminate [18.6.3.3 lib.terminate]
void terminate();

/// uncaught_exception [18.6.4 lib.uncaught]
bool uncaught_exception() throw();

/**@} lib_support_exception */
/**@} lib_language_support */

}//namespace std

extern "C"
__forceinline
void __stdcall _CxxThrowException(void * /*pObject*/, _s__ThrowInfo const * /*pObjectInfo*/)
{
#if 0
    const DWORD CPP_EXCEPTION    = 0xE06D7363; 
    const DWORD MS_MAGIC         = 0x19930520;
    const void * args[] ={ (void*)MS_MAGIC,
                              pObject,
                              pObjectInfo };
    RaiseException(CPP_EXCEPTION,
                   EXCEPTION_NONCONTINUABLE,
                   sizeof(args)/sizeof(args[0]),
                   args);
#endif
}

#endif//#ifndef NTL__STLX_EXCEPTION
