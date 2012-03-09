/**\file*********************************************************************
 *                                                                     \brief
 *  19.4 System error support [syserr]
 *
 ****************************************************************************
 */
#ifndef NTL__NT_STRERROR
#define NTL__NT_STRERROR
#pragma once

#include "status.hxx"

#include "../pe/image.hxx"
#include "peb.hxx"

namespace ntl { namespace nt { 
  
  namespace win32_error {
    enum error_type;
  }
  typedef win32_error::error_type win32error;

  struct message_resource_entry
  {
    uint16_t Length;
    uint16_t IsUnicode:1;
    uint8_t  Text[1];
  };

  NTL_EXTERNAPI
  uint32_t __stdcall
  RtlNtStatusToDosError(ntstatus Status);

  NTL_EXTERNAPI
  ntstatus __stdcall
  RtlFindMessage(
    const void* DllHandle,
    uint32_t MessageTableId,
    uint32_t MessageLanguageId,
    uint32_t MessageId,
    message_resource_entry **MessageEntry
    );

  NTL_EXTERNAPI
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

#ifndef NTL_SUBSYSTEM_KM
  namespace __
  {
    inline const message_resource_entry* find_mre(win32error ErrorCode)
    {
      static const pe::image* kernel = 0;
      if(!kernel){
        peb::find_dll find_kernel(&peb::instance());
        kernel = find_kernel("kernel32.dll");
      }
      if(!kernel)
        return nullptr;

      static const uint32_t rt_messagetable = 11,
        lang_system_default = 0x800,
        lang_user_default = 0x400;

      message_resource_entry* mre = nullptr;
      RtlFindMessage(kernel, rt_messagetable, lang_system_default, ErrorCode, &mre);
      return mre;
    }

    inline const message_resource_entry* find_mre(ntstatus Status)
    {
      static const uint32_t error_mid_not_found = 317;
      uint32_t errcode = RtlNtStatusToDosError(Status);
      if(errcode == error_mid_not_found)
        return nullptr;
      return find_mre(static_cast<win32error>(errcode));
    }

    inline void mre2str(const message_resource_entry* mre, std::string& re)
    {
      if(mre){
        if(mre->IsUnicode){
          const_unicode_string cus((const wchar_t*)mre->Text, mre->Length);
          re.resize(mre->Length+1);
          ansi_string as(re);
          if(success(RtlUnicodeStringToAnsiString(as, cus, false)))
            re.resize(std::char_traits<char>::length(as.data()));
          else
            re.clear();
        }else{
          re.assign((const char*)mre->Text, mre->Length);
        }
        if(re.length() > 2){
          const size_t p = re.length() - 2;
          if(re.compare(p, 2, "\r\n") == 0)
            re.erase(p);
        }
      }
    }

    inline void mre2str(const message_resource_entry* mre, std::wstring& re)
    {
      if(mre){
        if(!mre->IsUnicode){
          const_ansi_string cas((const char*)mre->Text, mre->Length);
          re.resize(mre->Length+1);
          unicode_string us(re);
          if(success(RtlAnsiStringToUnicodeString(us, cas, false)))
            re.resize(std::char_traits<wchar_t>::length(us.data()));
          else
            re.clear();
        }else{
          re.assign((const wchar_t*)mre->Text, mre->Length);
        }
      }
    }
  }

  inline std::string strerror(ntstatus Status)
  {
    std::string re("");
    const message_resource_entry* mre = __::find_mre(Status);
    __::mre2str(mre, re);
    return std::move(re);
  }

  inline std::wstring wstrerror(ntstatus Status)
  {
    std::wstring re(L"");
    const message_resource_entry* mre = __::find_mre(Status);
    __::mre2str(mre, re);
    return std::move(re);
  }
  
  inline std::string strerror(win32error ErrorCode)
  {
    std::string re("");
    const message_resource_entry* mre = __::find_mre(ErrorCode);
    __::mre2str(mre, re);
    return std::move(re);
  }

  inline std::wstring wstrerror(win32error ErrorCode)
  {
    std::wstring re(L"");
    const message_resource_entry* mre = __::find_mre(ErrorCode);
    __::mre2str(mre, re);
    return std::move(re);
  }

#else

  inline std::string strerror(ntstatus Status)
  {
    char buf[32];
    const char msg[] = "system error code ";
    std::strcpy(buf, msg);
    _itoa(Status, buf+_countof(msg), 10);
    return std::string(buf);
  }

  inline std::wstring wstrerror(ntstatus Status)
  {
    wchar_t buf[32];
    const wchar_t msg[] = L"system error code ";
    std::wcscpy(buf, );
    _itow(Status, buf+_countof(msg), 10);
    return std::wstring(buf);
  }

#endif
}} 
#endif // NTL__NT_STRERROR
