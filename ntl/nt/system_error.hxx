/**\file*********************************************************************
 *                                                                     \brief
 *  19.4 System error support [syserr]
 *
 ****************************************************************************
 */
#ifndef NTL__NT_SYSTEMERROR
#define NTL__NT_SYSTEMERROR
#pragma once

#include "../stlx/system_error.hxx"
#include "strerror.hxx"

namespace std
{
  /**
   *	@addtogroup syserr 19.4 System error support [syserr]
   *  @{
   **/

  /**
   *	@defgroup nt_syserr Native API system error support [nt.syserr]
   *  @{
   **/

  namespace native_error
  {
    using ntl::nt::status;
    using ntl::nt::ntstatus;
  }

  template <> struct is_error_condition_enum<native_error::ntstatus>
  : public true_type {};

  namespace native_error
  {
    class native_error_category:
      public error_category
    {
    public:

      /** Returns a string naming the error category ("native") */
      const char *name() const __ntl_nothrow { return "native"; }

      virtual string message(int ev) const
      {
        return ntl::nt::strerror(static_cast<ntl::nt::ntstatus>(ev));
      }

      error_condition default_error_condition(int ev) const __ntl_nothrow
      {
        return error_condition(ev, *this);
      }
      bool equivalent(int code, const error_condition& condition) const __ntl_nothrow
      {
        return default_error_condition(code) == condition;
      }

      bool equivalent(const error_code& code, int condition) const __ntl_nothrow
      {
        return *this == code.category() && code.value() == condition;
      }
    };

    inline const native_error_category& native_category()
    {
      return *std::__::static_storage<native_error_category>::get_object();
    }

    inline error_code make_error_code(native_error::ntstatus st)
    {
      return error_code(static_cast<int>(st), native_error::native_category());
    }

    inline error_condition make_error_condition(native_error::ntstatus st)
    {
      return error_condition(static_cast<int>(st), native_error::native_category());
    }
  }

  // bring this functions to the std namespace
  using native_error::make_error_code;
  using native_error::make_error_condition;

  /** @} nt_syserr */
  /** @} syserr */
} // std


namespace ntl { namespace nt {

  // bring this functions to the nt namespace
  using std::native_error::make_error_code;
  using std::native_error::make_error_condition;

}} 

#endif // NTL__NT_SYSTEMERROR
