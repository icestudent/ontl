/**\file*********************************************************************
 *                                                                     \brief
 *  Network errors support
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_ERROR
#define NTL__STLX_TR2_NETWORK_ERROR
#pragma once

#include "../../../system_error.hxx"
#include "../../../../nt/win32_error.hxx"

namespace std { namespace tr2 {
  
  /// TR2 Network library
  namespace network
  {

  /// Network error codes (alias)
  namespace network_error
  {}
  /// Network error codes
  namespace error = network_error;
  namespace network_error {
    enum error_type {
      success = ntl::nt::win32_error::success,

      // errno errors:
      address_family_not_supported = ntl::nt::win32_error::wsaeafnosupport,
      address_in_use = ntl::nt::win32_error::wsaeaddrinuse,
      address_not_available = ntl::nt::win32_error::wsaeaddrnotavail,
      already_connected = ntl::nt::win32_error::wsaeisconn,
      bad_file_descriptor = ntl::nt::win32_error::wsaebadf,
      broken_pipe = ntl::nt::win32_error::broken_pipe,
      connection_aborted = ntl::nt::win32_error::wsaeconnaborted,
      connection_already_in_progress = ntl::nt::win32_error::wsaealready,
      connection_refused = ntl::nt::win32_error::wsaeconnrefused,
      connection_reset = ntl::nt::win32_error::wsaeconnreset,
      destination_address_required = ntl::nt::win32_error::wsaedestaddrreq,
      host_unreachable = ntl::nt::win32_error::wsaehostunreach,
      interrupted = ntl::nt::win32_error::wsaeintr,
      invalid_argument = ntl::nt::win32_error::wsaeinval,
      message_size = ntl::nt::win32_error::wsaemsgsize,
      network_down = ntl::nt::win32_error::wsaenetdown,
      network_reset = ntl::nt::win32_error::wsaenetreset,
      network_unreachable = ntl::nt::win32_error::wsaenetunreach,
      no_buffer_space = ntl::nt::win32_error::wsaenobufs,
      no_protocol_option = ntl::nt::win32_error::wsaenoprotoopt,
      not_a_socket = ntl::nt::win32_error::wsaenotsock,
      not_connected = ntl::nt::win32_error::wsaenotconn,
      operation_canceled = ntl::nt::win32_error::wsaecancelled,
      operation_not_supported = ntl::nt::win32_error::wsaeopnotsupp,
      permission_denied = ntl::nt::win32_error::wsaeacces,
      protocol_not_supported = ntl::nt::win32_error::wsaeprotonosupport,
      timed_out = ntl::nt::win32_error::wsaetimedout,
      too_many_files_open = ntl::nt::win32_error::wsaemfile,
      too_many_files_open_in_system = ntl::nt::win32_error::wsaetoomanyrefs,
      wrong_protocol_type = ntl::nt::win32_error::wsaeprototype,

      // getaddrinfo errors:
      host_not_found = ntl::nt::win32_error::wsahost_not_found,
      host_not_found_try_again = ntl::nt::win32_error::wsatry_again,
      service_not_found = ntl::nt::win32_error::wsaservice_not_found,

      // miscellaneous errors:
      already_open = ntl::nt::win32_error::wsaealready,
      eof = ntl::nt::win32_error::handle_eof,
      not_found =  ntl::nt::win32_error::not_found
    };

    class network_error_category:
      public error_category
    {
    public:

      /** Returns a string naming the error category ("network") */
      const char *name() const __ntl_nothrow { return "network"; }

      virtual string message(int ev) const
      {
        return ntl::nt::strerror(static_cast<ntl::nt::win32error>(ev));
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

    inline const network_error_category& network_category()
    {
      return *std::__::static_storage<network_error_category>::get_object();
    }

    inline error_code make_error_code(error_type code)
    {
      return error_code(static_cast<int>(code), network_category());
    }

    inline error_condition make_error_condition(error_type code)
    {
      return error_condition(static_cast<int>(code), network_category());
    }

  } // network_error
  
}} 

  // std namespace

template <> struct is_error_condition_enum<tr2::network::error::error_type>
  : public true_type {};

  // bring this functions to the std namespace
  using tr2::network::error::make_error_code;
  using tr2::network::error::make_error_condition;


}
#endif // NTL__STLX_TR2_NETWORK_ERROR
