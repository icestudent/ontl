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

  static const std::__::exception_error_category exception_category_instance;
  static std::error_code throws_error_code_instance(0, exception_category_instance);
}
namespace std
{
  extern const error_category& generic_category = generic_category_instance;
  extern const error_category& system_category = system_category_instance;

  extern const __::exception_error_category& exception_category = exception_category_instance;
  extern error_code& throws_error_code = throws_error_code_instance;


  // TODO: implement generic error value mapping to the current subsystem error values
  string __::generic_error_category::message(int ev) const
  {
    char buf[12];
    _itoa(ev, buf, 10);
    string msg("generic error code ");
    return msg += buf;
  }

  // TODO: implement generic system error message or map it to native when in NT
  string __::system_error_category::message(int ev) const
  {
    char buf[12];
    _itoa(ev, buf, 10);
    string msg("system error code ");
    return msg += buf;
  }

  // TODO: detect posix errno
  error_condition __::system_error_category::default_error_condition(int ev) const __ntl_nothrow
  {
    return error_condition(ev, system_category);
  }
}
