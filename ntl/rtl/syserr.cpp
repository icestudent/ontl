#include "../nt/basedef.hxx"
#include "../stlx/system_error.hxx"
#include "../stlx/cstdlib.hxx"

// NT system error support
#if defined(NTL__SUBSYSTEM_NT) && !defined(NTL__SUBSYSTEM_KM)

# include "../nt/system_error.hxx"
namespace
{
  static const std::native_error::native_error_category native_category_instance;
}
const std::native_error::native_error_category& std::native_error::native_category = native_category_instance;

#endif // subsystem nt

namespace
{
  static const std::__::generic_error_category generic_category_instance;
  static const std::__::system_error_category system_category_instance;
  static const std::__::iostream_error_category iostream_category_instance;
}

namespace std
{
  extern const error_category& generic_category = generic_category_instance;
  extern const error_category& system_category = system_category_instance;
  extern const error_category& iostream_category = iostream_category_instance;

  // TODO: implement generic error value mapping to the current subsystem error values
  string __::generic_error_category::message(int ev) const
  {
    char buf[32];
    strcpy(buf, "generic error code ");
    _itoa(ev, buf+sizeof("generic error code"), 10);
    return string(buf);
  }

  // TODO: implement generic system error message or map it to native when in NT
  string __::system_error_category::message(int ev) const
  {
    char buf[32];
    strcpy(buf, "system error code ");
    _itoa(ev, buf+sizeof("system error code"), 10);
    return string(buf);
  }

  string __::iostream_error_category::message(int ev) const
  {
    char buf[32];
    strcpy(buf, "iostream error code ");
    _itoa(ev, buf+sizeof("iostream error code"), 10);
    return string(buf);
  }

  // TODO: detect posix errno
  error_condition __::system_error_category::default_error_condition(int ev) const __ntl_nothrow
  {
    return error_condition(ev, system_category);
  }
}
