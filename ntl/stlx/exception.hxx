/**\file*********************************************************************
 *                                                                     \brief
 *  18.8 Exception handling [support.exception]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_EXCEPTION
#define NTL__STLX_EXCEPTION
#pragma once

#ifndef NTL__STLX_CSTDDEF
# include "cstddef.hxx"
#endif

#ifndef NTL__STLX_EXCPTDEF
# include "excptdef.hxx"
#endif

#ifdef _MSC_VER
//C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#pragma warning(disable:4290)
#endif

#if STLX__USE_EXCEPTIONS == 0
# pragma push_macro("__noreturn")
# undef __noreturn
# define __noreturn
#endif


namespace std {

/**\addtogroup  lib_language_support *** 18 Language support library [language.support]
 *@{*/

/**\defgroup  lib_support_exception **** 18.8 Exception handling [support.exception]
 *@{*/

/// 18.8.1 Class exception [exception]
class exception
{
  public:
    exception() __ntl_nothrow {}
    exception(const exception&) __ntl_nothrow {}
    exception& operator=(const exception&) __ntl_nothrow { return *this; }
    virtual ~exception() /*__ntl_nothrow */{}
    virtual const char* what() const __ntl_nothrow { return "exception"; }
};

///\name 18.8.2 Violating exception-specifications [exception.unexpected]

/// Class bad_exception [18.8.2.1 bad.exception]
class bad_exception : public exception
{
  public:
    bad_exception() __ntl_nothrow {}
    bad_exception(const bad_exception&) __ntl_nothrow {}
    bad_exception& operator=(const bad_exception&) __ntl_nothrow { return *this; }
    virtual ~bad_exception() __ntl_nothrow {}
    virtual const char* what() const __ntl_nothrow { return "bad_exception"; }
};

  /**
   *	@brief Type unexpected_handler [18.8.2.2 unexpected.handler]
   *  @details The type of a handler function to be called by unexpected() when a function attempts 
   *  to throw an exception not listed in its exception-specification.
   *  @note An unexpected_handler shall not return.
   **/
  /// @note an unexpected_handler shall not return.
  typedef void (*unexpected_handler)();

  /**
   *	@brief set_unexpected [18.8.2.3 set.unexpected]
   *  @details Establishes the function designated by \c f as the current \c unexpected_handler.
   *  @return The previous \c unexpected_handler.
   **/
  unexpected_handler set_unexpected(unexpected_handler f) __ntl_nothrow;

  /**
   *	@brief unexpected [18.8.2.4 unexpected]
   *  @details Calls the \c unexpected_handler function in effect immediately after evaluating 
   *  the \e throw-expression (18.8.2.2), if called by the implementation, or calls the current \c unexpected_handler, 
   *  if called by the program.
   **/
  __noreturn
  void unexpected();

  ///\}

  /**
   *	@brief Type terminate_handler [18.8.3.1 terminate.handler]
   *  @details The type of a handler function to be called by terminate() when terminating exception processing.
   **/
  typedef void (*terminate_handler)();

  /**
   *	@brief set_terminate [18.8.3.2 set.terminate]
   *  @details Establishes the function designated by \c f as the current handler function for terminating exception processing.
   *  @return The previous \c terminate_handler.
   **/
  terminate_handler set_terminate(terminate_handler f) __ntl_nothrow;

  /**
   *	@brief terminate [18.8.3.3 terminate]
   *  @details Calls the \c terminate_handler function in effect immediately after evaluating 
   *  the throw-expression (18.8.3.1), if called by the implementation, or calls the current \c terminate_handler function,
   *  if called by the program.
   **/
  __noreturn
  void terminate();


  /**
   *	@brief uncaught_exception [18.8.4 uncaught]
   *  @details Returns \c true after completing evaluation of a \e throw-expression until either completing initialization
   *  of the exception-declaration in the matching handler or entering unexpected() due to the throw; or
   *  after entering terminate() for any reason other than an explicit call to terminate().
   *  @note When uncaught_exception() is \c true, throwing an exception can result in a call of terminate()
   **/
  bool uncaught_exception() __ntl_nothrow;

/**@} lib_support_exception */
/**@} lib_language_support */
}//namespace std
#endif//#ifndef NTL__STLX_EXCEPTION
