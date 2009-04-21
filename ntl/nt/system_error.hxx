/**\file*********************************************************************
 *                                                                     \brief
 *  19.4 System error support [syserr]
 *
 ****************************************************************************
 */
#ifndef NTL__NT_SYSTEMERROR
#define NTL__NT_SYSTEMERROR

#include "../stlx/system_error.hxx"
#include "status.hxx"

#include "../pe/image.hxx"
#include "peb.hxx"

namespace ntl { namespace nt {
  static std::string strerror(ntstatus Status);
}}

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

    extern const native_error_category& native_category;
    
    inline const native_error_category& get_native_category()
    {
      return native_category;
    }

    inline error_code make_error_code(native_error::ntstatus st)
    {
      return error_code(static_cast<int>(st), native_error::native_category);
    }

    inline error_condition make_error_condition(native_error::ntstatus st)
    {
      return error_condition(static_cast<int>(st), native_error::native_category);
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

  struct message_resource_entry
  {
    uint16_t Length;
    uint16_t IsUnicode:1;
    uint8_t  Text[1];
  };

  NTL__EXTERNAPI
    uint32_t __stdcall
    RtlNtStatusToDosError(ntstatus Status);

  NTL__EXTERNAPI
    ntstatus __stdcall
    RtlFindMessage(
      const void* DllHandle,
      uint32_t MessageTableId,
      uint32_t MessageLanguageId,
      uint32_t MessageId,
      message_resource_entry **MessageEntry
      );

  NTL__EXTERNAPI
  ntstatus __stdcall
    RtlFormatMessage(
      wchar_t* MessageFormat,
      uint32_t MaximumWidth,
      bool IgnoreInserts,
      bool ArgumentsAreAnsi,
      bool ArgumentsAreAnArray,
      va_list *Arguments,
      wchar_t* Buffer,
      uint32_t Length,
      uint32_t* ReturnLength
      );

  static std::string strerror(ntstatus Status)
  {
    std::string re("");

    static const uint32_t error_mid_not_found = 317;

    uint32_t errcode = RtlNtStatusToDosError(Status);
    if(errcode == error_mid_not_found)
      return re;

    static const pe::image* kernel = 0;
    if(!kernel){
      peb::find_dll find_kernel(&peb::instance());
      kernel = find_kernel("kernel32.dll");
    }
    if(!kernel)
      return re;

    static const uint32_t rt_messagetable = 11,
      lang_system_default = 0x800,
      lang_user_default = 0x400;

    message_resource_entry* mre = nullptr;
    ntstatus st = RtlFindMessage(kernel, rt_messagetable, lang_system_default, errcode, &mre);
    if(success(st)){
      if(mre->IsUnicode){
        const_unicode_string cus((const wchar_t*)mre->Text, mre->Length);
        re.resize(mre->Length+1);
        ansi_string as(re);
        st = RtlUnicodeStringToAnsiString(as, cus, false);
        if(success(st))
          re.resize(std::char_traits<char>::length(as.data()));
        else
          re.clear();
      }else{
        re.assign((const char*)mre->Text, mre->Length);
      }
    }
    return re;
  }

}} 

#endif // NTL__NT_SYSTEMERROR
