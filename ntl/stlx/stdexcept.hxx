/**\file*********************************************************************
 *                                                                     \brief
 *  Diagnostics library [19 lib.diagnostics]
 *  Exception classes [19.1 lib.std.exceptions]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_STDEXCEPT
#define NTL__STLX_STDEXCEPT

#ifndef NTL__STLX_STRING
#include "string.hxx"
#endif
#ifndef NTL__STLX_STDEXCEPT_FWD
#include "stdexcept_fwd.hxx"
#endif

namespace std
{

/// 19.1.1 Class logic_error [logic.error]
class logic_error : public exception
{
  public:
    explicit logic_error(const string& what_arg) : msg(what_arg) {}
    explicit logic_error(const char* what_arg )  : msg(what_arg) {}
    virtual const char* what() const __ntl_nothrow { return msg.c_str(); }
    ~logic_error() /*__ntl_nothrow*/ {}
  private:
    const string msg;
  protected:
    logic_error operator=(const logic_error&) {/* do not use */ return *this; }
};

/// 19.1.2 Class domain_error [domain.error]
class domain_error : public logic_error
{
  public:
    explicit domain_error(const string& what_arg) : logic_error(what_arg) {}
    explicit domain_error(const char* what_arg )  : logic_error(what_arg) {}
};

/// 19.1.3 Class invalid_argument [invalid.argument]
class invalid_argument : public logic_error
{
  public:
    explicit invalid_argument(const string& what_arg) : logic_error(what_arg) {}
    explicit invalid_argument(const char* what_arg )  : logic_error(what_arg) {}
};

/// 19.1.4 Class length_error [length.error]
class length_error : public logic_error
{
  public:
    explicit length_error(const string& what_arg) : logic_error(what_arg) {}
    explicit length_error(const char* what_arg )  : logic_error(what_arg) {}
};

/// 19.1.5 Class out_of_range [lib.out.of.range]
class out_of_range : public logic_error
{
  public:
    explicit out_of_range(const string& what_arg) : logic_error(what_arg) {}
    explicit out_of_range(const char* what_arg )  : logic_error(what_arg) {}
};

/// 19.1.6 Class runtime_error [lib.runtime.error]
class runtime_error : public exception
{
  public:
    explicit runtime_error(const string& what_arg) : msg(what_arg) {}
    explicit runtime_error(const char* what_arg )  : msg(what_arg) {}
    virtual const char* what() const __ntl_nothrow { return msg.c_str(); }
    ~runtime_error() /*__ntl_nothrow*/ {}
  private:
    const string msg;
  protected:
    runtime_error operator=(const runtime_error&) {/* do not use */ return *this; }
};

/// 19.1.7 Class range_error [lib.range.error]
class range_error : public runtime_error
{
  public:
    explicit range_error(const string& what_arg) : runtime_error(what_arg) {}
    explicit range_error(const char* what_arg )  : runtime_error(what_arg) {}
};

/// 19.1.8 Class overflow_error [lib.overflow.error]
class overflow_error : public runtime_error
{
  public:
    explicit overflow_error(const string& what_arg) : runtime_error(what_arg) {}
    explicit overflow_error(const char* what_arg )  : runtime_error(what_arg) {}
};

/// 19.1.9 Class underflow_error [lib.underflow.error]
class underflow_error : public runtime_error
{
  public:
    explicit underflow_error(const string& what_arg) : runtime_error(what_arg) {}
    explicit underflow_error(const char* what_arg )  : runtime_error(what_arg) {}
};


///\name raise exceptions halpers
inline void __throw_out_of_range(const char* msg)
{
  __ntl_throw(out_of_range(msg));
  (void)msg;
}
///\}

}//namespace std

#endif//#ifndef NTL__STLX_STDEXCEPT
