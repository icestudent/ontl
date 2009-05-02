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

/**\addtogroup  lib_language_support *** 18 Language support library [language.support]
 *@{*/

/**\defgroup  lib_support_exception **** 18.7 Exception handling [support.exception]
 *@{*/

/// 18.7.1 Class exception [exception]
class exception
{
  public:
    exception() __ntl_nothrow {}
    exception(const exception&) __ntl_nothrow {}
    exception& operator=(const exception&) __ntl_nothrow { return *this; }
    virtual ~exception() /*__ntl_nothrow */{}
    virtual const char* what() const __ntl_nothrow { return "exception"; }
};

/// Class bad_exception [18.7.2.1 lib.bad.exception]
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
   *	@brief Type unexpected_handler [18.7.2.2 unexpected.handler]
   *  @details The type of a handler function to be called by unexpected() when a function attempts 
   *  to throw an exception not listed in its exception-specification.
   *  @note An unexpected_handler shall not return.
   **/
  /// @note an unexpected_handler shall not return.
  typedef void (*unexpected_handler)();

  /**
   *	@brief set_unexpected [18.7.2.3 set.unexpected]
   *  @details Establishes the function designated by \c f as the current \c unexpected_handler.
   *  @return The previous \c unexpected_handler.
   **/
  unexpected_handler set_unexpected(unexpected_handler f) __ntl_nothrow;

  /**
   *	@brief unexpected [18.7.2.4 unexpected]
   *  @details Calls the \c unexpected_handler function in effect immediately after evaluating 
   *  the \e throw-expression (18.7.2.2), if called by the implementation, or calls the current \c unexpected_handler, 
   *  if called by the program.
   **/
  __declspec(noreturn)
  void unexpected();


  /**
   *	@brief Type terminate_handler [18.7.3.1 terminate.handler]
   *  @details The type of a handler function to be called by terminate() when terminating exception processing.
   **/
  typedef void (*terminate_handler)();

  /**
   *	@brief set_terminate [18.7.3.2 set.terminate]
   *  @details Establishes the function designated by \c f as the current handler function for terminating exception processing.
   *  @return The previous \c terminate_handler.
   **/
  terminate_handler set_terminate(terminate_handler f) __ntl_nothrow;

  /**
   *	@brief terminate [18.7.3.3 terminate]
   *  @details Calls the \c terminate_handler function in effect immediately after evaluating 
   *  the throw-expression (18.7.3.1), if called by the implementation, or calls the current \c terminate_handler function,
   *  if called by the program.
   **/
  __declspec(noreturn)
  void terminate();


  /**
   *	@brief uncaught_exception [18.7.4 uncaught]
   *  @details Returns \c true after completing evaluation of a \e throw-expression until either completing initialization
   *  of the exception-declaration in the matching handler or entering unexpected() due to the throw; or
   *  after entering terminate() for any reason other than an explicit call to terminate().
   *  @note When uncaught_exception() is \c true, throwing an exception can result in a call of terminate()
   **/
  bool uncaught_exception() __ntl_nothrow;


  /// Exception Propagation [18.7.5 propagation]
  ///
  typedef struct __exception_ptr { } exception_ptr;

  exception_ptr current_exception();

  void rethrow_exception(exception_ptr e);

  template<class E>
  exception_ptr copy_exception(E e);

  /// nested_exception [18.7.6 except.nested]
  class nested_exception
  {
  public:
    nested_exception() throw()
      :e(current_exception())
    {}

  #ifdef NTL__CXX_EF
    nested_exception(const nested_exception&) throw() = default;
    nested_exception& operator=(const nested_exception&) throw() = default;
    virtual ~nested_exception() = default;
  #else
    // default generated
  #endif

    // access functions
    void rethrow_nested() const; // [[noreturn]]
    exception_ptr nested_ptr() const { return e; }

  private:
    exception_ptr e;
  };

#ifdef NTL__CXX_RV
  template<class T> void throw_with_nested(T&& t); // [[noreturn]]
#else
  template<class T> void throw_with_nested(T& t); // [[noreturn]]
#endif

  template <class E> void rethrow_if_nested(const E& e);


/**@} lib_support_exception */
/**@} lib_language_support */

}//namespace std

#endif//#ifndef NTL__STLX_EXCEPTION
