/**\file*********************************************************************
 *                                                                     \brief
 *  18.7 Exception handling [support.exception]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_EXCEPTION
#define NTL__STLX_EXCEPTION

#include "excptdef.hxx"

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
    exception() __ntl_nothrow {}
    exception(const exception&) __ntl_nothrow {}
    exception& operator=(const exception&) __ntl_nothrow {}
    virtual ~exception() __ntl_nothrow {}
    virtual const char* what() const __ntl_nothrow { return "exception"; }
};

/// Class bad_exception [18.6.2.1 lib.bad.exception]
class bad_exception : public exception
{
  public:
    bad_exception() __ntl_nothrow {}
    bad_exception(const bad_exception&) __ntl_nothrow {}
    bad_exception& operator=(const bad_exception&) __ntl_nothrow {}
    virtual ~bad_exception() __ntl_nothrow {}
    virtual const char* what() const __ntl_nothrow { return "bad_exception"; }
};

/// Type unexpected_handler [18.6.2.2 lib.unexpected.handler]
/// @note an unexpected_handler shall not return.
typedef void (*unexpected_handler)();

/// set_unexpected [18.6.2.3 lib.set.unexpected]
unexpected_handler set_unexpected(unexpected_handler f) __ntl_nothrow;

/// unexpected [18.6.2.4 lib.unexpected]
void unexpected();

/// Type terminate_handler [18.6.3.1 lib.terminate.handler]
typedef void (*terminate_handler)();

/// set_terminate [18.6.3.2 lib.set.terminate]
terminate_handler set_terminate(terminate_handler f) __ntl_nothrow;

/// terminate [18.6.3.3 lib.terminate]
__declspec(noreturn)
void terminate();

/// uncaught_exception [18.6.4 lib.uncaught]
bool uncaught_exception() __ntl_nothrow;

/**@} lib_support_exception */
/**@} lib_language_support */

}//namespace std

#endif//#ifndef NTL__STLX_EXCEPTION
