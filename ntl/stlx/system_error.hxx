/**\file*********************************************************************
*                                                                     \brief
*  19.4 System error support [syserr]
*
****************************************************************************
*/
#ifndef NTL__STLX_SYSTEM_ERROR
#define NTL__STLX_SYSTEM_ERROR

#include "type_traits.hxx"
#include "stdexcept.hxx"
#include "string.hxx"

namespace std {

  /**
   *	@group syserr System error support
   *  @{
   **/

  class error_category;
  class error_code;
  class error_condition;
  class system_error;

  template<class T>
  struct is_error_code_enum: public false_type {};

  template<class T>
  struct is_error_condition_enum: public false_type {};


  /// 19.4.1 syserr.errcat
  class error_category
  {
  public:
    virtual ~error_category();
#ifdef NTL__CXX_EF
    error_category(const error_category&) = delete;
    error_category& operator=(const error_category&) = delete;
#else
  private:
    error_category(const error_category&);
    error_category& operator=(const error_category&);
  public:
#endif
    virtual const char* name() const = 0;
    virtual error_condition default_error_condition(int ev) const;
    virtual bool equivalent(int code, const error_condition& condition) const;
    virtual bool equivalent(const error_code& code, int condition) const;
    virtual string message(int ev) const = 0;

    bool operator==(const error_category& rhs) const;
    bool operator!=(const error_category& rhs) const;
    bool operator<(const error_category& rhs) const;
  };

  /// 19.4.2 syserr.errcode
  class error_code
  {
  public:
    // 19.4.2.2 constructors:
    error_code();
    error_code(int val, const error_category& cat);

    template <class ErrorCodeEnum>
    error_code(ErrorCodeEnum e, typename enable_if<is_error_code_enum<ErrorCodeEnum>::value>::type * = 0);

    // 19.4.2.3 modifers:
    void assign(int val, const error_category& cat);
    template <class ErrorCodeEnum>
    typename enable_if<is_error_code_enum<ErrorCodeEnum>::value>::type& operator=(ErrorCodeEnum e);
    void clear();

    // 19.4.2.4 observers:
    int value() const;
    const error_category& category() const;
    error_condition default_error_condition() const;
    string message() const;
    /*explicit */operator bool() const;

private:
    //int val_; // exposition only
    //const error_category& cat_; // exposition only
  };


  /// 19.4.3 syserr.errcondition
  class error_condition
  {
  public:
    // 19.4.3.2 constructors:
    error_condition();
    error_condition(int val, const error_category& cat);
    template <class ErrorConditionEnum>
    error_condition(ErrorConditionEnum e, typename enable_if<is_error_condition_enum<ErrorConditionEnum>::value>::type* = 0);

    // 19.4.3.3 modifers:
    void assign(int val, const error_category& cat);
    template<typename ErrorConditionEnum>
    typename enable_if<is_error_condition_enum<ErrorConditionEnum>::value, error_code>::type& operator= (ErrorConditionEnum e);
    void clear();

    // 19.4.3.4 observers:
    int value() const;
    const error_category& category() const;
    string message() const;
    /*explicit */operator bool() const;

  private:
    //int val_; // exposition only
    //const error_category& cat_; // exposition only
  };

  // 19.4.3.5 non-member functions:
  bool operator<(const error_condition& lhs, const error_condition& rhs);


  /// 19.4.5.1 syserr.syserr
  class system_error : public runtime_error
  {
  public:
    system_error(error_code ec, const string& what_arg);
    system_error(error_code ec);
    system_error(int ev, const error_category& ecat,
      const string& what_arg);
    system_error(int ev, const error_category& ecat);
    const error_code& code() const __ntl_nothrow;
    const char* what() const __ntl_nothrow;
  };



  // 19.4.4 Comparison operators:
  bool operator==(const error_code& lhs, const error_code& rhs);
  bool operator==(const error_code& lhs, const error_condition& rhs);
  bool operator==(const error_condition& lhs, const error_code& rhs);
  bool operator==(const error_condition& lhs, const error_condition& rhs);
  bool operator!=(const error_code& lhs, const error_code& rhs);
  bool operator!=(const error_code& lhs, const error_condition& rhs);
  bool operator!=(const error_condition& lhs, const error_code& rhs);
  bool operator!=(const error_condition& lhs, const error_condition& rhs);


  namespace posix_error 
  {
    enum posix_errno {
      address_family_not_supported, // EAFNOSUPPORT
      address_in_use, // EADDRINUSE
      address_not_available, // EADDRNOTAVAIL
      already_connected, // EISCONN
      argument_list_too_long, // E2BIG
      argument_out_of_domain, // EDOM
      bad_address, // EFAULT
      bad_file_descriptor, // EBADF
      bad_message, // EBADMSG
      broken_pipe, // EPIPE
      connection_aborted, // ECONNABORTED
      connection_already_in_progress, // EALREADY
      connection_refused, // ECONNREFUSED
      connection_reset, // ECONNRESET
      cross_device_link, // EXDEV
      destination_address_required, // EDESTADDRREQ
      device_or_resource_busy, // EBUSY
      directory_not_empty, // ENOTEMPTY
      executable_format_error, // ENOEXEC
      file_exists, // EEXIST
      file_too_large, // EFBIG
      filename_too_long, // ENAMETOOLONG
      function_not_supported, // ENOSYS
      host_unreachable, // EHOSTUNREACH
      identifier_removed, // EIDRM
      illegal_byte_sequence, // EILSEQ
      inappropriate_io_control_operation, // ENOTTY
      interrupted, // EINTR
      invalid_argument, // EINVAL
      invalid_seek, // ESPIPE
      io_error, // EIO
      is_a_directory, // EISDIR
      message_size, // EMSGSIZE
      network_down, // ENETDOWN
      network_reset, // ENETRESET
      network_unreachable, // ENETUNREACH
      no_buffer_space, // ENOBUFS
      no_child_process, // ECHILD
      no_link, // ENOLINK
      no_lock_available, // ENOLCK
      no_message_available, // ENODATA
      no_message, // ENOMSG
      no_protocol_option, // ENOPROTOOPT
      no_space_on_device, // ENOSPC
      no_stream_resources, // ENOSR
      no_such_device_or_address, // ENXIO
      no_such_device, // ENODEV
      no_such_file_or_directory, // ENOENT
      no_such_process, // ESRCH
      not_a_directory, // ENOTDIR
      not_a_socket, // ENOTSOCK
      not_a_stream, // ENOSTR
      not_connected, // ENOTCONN
      not_enough_memory, // ENOMEM
      not_supported, // ENOTSUP
      operation_canceled, // ECANCELED
      operation_in_progress, // EINPROGRESS
      operation_not_permitted, // EPERM
      operation_not_supported, // EOPNOTSUPP
      operation_would_block, // EWOULDBLOCK
      owner_dead, // EOWNERDEAD
      permission_denied, // EACCES
      protocol_error, // EPROTO
      protocol_not_supported, // EPROTONOSUPPORT
      read_only_file_system, // EROFS
      resource_deadlock_would_occur, // EDEADLK
      resource_unavailable_try_again, // EAGAIN
      result_out_of_range, // ERANGE
      state_not_recoverable, // ENOTRECOVERABLE
      stream_timeout, // ETIME
      text_file_busy, // ETXTBSY
      timed_out, // ETIMEDOUT
      too_many_files_open_in_system, // ENFILE
      too_many_files_open, // EMFILE
      too_many_links, // EMLINK
      too_many_symbolic_link_levels, // ELOOP
      value_too_large, // EOVERFLOW
      wrong_protocol_type, // EPROTOTYPE
    };
  } // namespace posix_error
  
  template <> struct is_error_condition_enum<posix_error::posix_errno>
  : public true_type {};

  namespace posix_error
  {
    error_code make_error_code(posix_errno e);
    error_condition make_error_condition(posix_errno e);
  } // namespace posix_error

  /** @} syserr */
} // std
#endif // NTL__STLX_SYSTEM_ERROR
