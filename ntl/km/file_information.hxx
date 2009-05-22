/**\file*********************************************************************
 *                                                                     \brief
 *  File Information
 *
 ****************************************************************************
 */

#ifndef NTL__KM_FILE_INFORMATION
#define NTL__KM_FILE_INFORMATION

#include "basedef.hxx"
#include "handle.hxx"
#include "../nt/file_information.hxx"


namespace ntl {
namespace km {

/**\addtogroup  file_information ********* File Information ***************
 *@{*/


using nt::file_information_class;

using nt::query_information_file_t;
using nt::set_information_file_t;

typedef ntstatus __stdcall io_query_information_file_t(
  file_object*            FileObject,
  file_information_class  FileInformationClass,
  uint32_t                Length,
  void*                   FileInformation,
  uint32_t*               ReturnedLength
  );

typedef ntstatus __stdcall io_set_information_file_t(
  file_object*            FileObject,
  file_information_class  FileInformationClass,
  uint32_t                Length,
  void*                   FileInformation
  );

NTL__EXTERNAPI query_information_file_t ZwQueryInformationFile;
NTL__EXTERNAPI set_information_file_t   ZwSetInformationFile;
NTL__EXTERNAPI io_query_information_file_t IoQueryFileInformation;
NTL__EXTERNAPI io_set_information_file_t   IoSetInformation;

using nt::file_basic_information;
using nt::file_standard_information;
//using nt::file_name_information;
using nt::file_rename_information;
using nt::file_disposition_information;
using nt::file_end_of_file_information;
using nt::file_network_open_information;

template<class InformationClass>
struct file_information
: public nt::file_information_base <InformationClass,
                                    ZwQueryInformationFile,
                                    ZwSetInformationFile>
{
  file_information(legacy_handle file_handle) __ntl_nothrow
  : nt::file_information_base<InformationClass, ZwQueryInformationFile, ZwSetInformationFile>(file_handle)
  {/**/}

  file_information(
    legacy_handle             file_handle,
    const InformationClass &  info) __ntl_nothrow
  : nt::file_information_base<InformationClass,
                              ZwQueryInformationFile,
                              ZwSetInformationFile>(file_handle, info)
  {/**/}
};



namespace __
{
  template<class info_class>
  struct file_rename_information_impl
  {
    operator bool() const { return nt::success(status_); }
    operator ntstatus() const { return status_; }

    file_rename_information_impl(legacy_handle file_handle, const const_unicode_string& new_name, bool replace_if_exists, legacy_handle root_directory)
    {
      info_class::ptr p(static_cast<info_class*>(info_class::alloc(new_name, replace_if_exists, root_directory).release()));
      status_ = p 
        ? _set(file_handle, p.get(), sizeof(info_class)+p->FileNameLength-sizeof(wchar_t))
        : status::insufficient_resources;
    }

    static __forceinline
      ntstatus
      _set(
      legacy_handle   file_handle,
      const void *    info,
      unsigned long   info_length
      )
    {
      io_status_block iosb;
      return NtSetInformationFile(file_handle, &iosb, info, info_length, info_class::info_class_type);
    }

  protected:
    ntstatus    status_;
  };
}

template<>
struct file_information<file_rename_information>:
  __::file_rename_information_impl<file_rename_information>
{
  file_information(legacy_handle file_handle, const const_unicode_string& new_name, bool replace_if_exists, legacy_handle root_directory = legacy_handle())
    :file_rename_information_impl(file_handle, new_name, replace_if_exists, root_directory)
  {}
};

///}

/**@} file_information */

}//namespace km
}//namespace ntl

#endif//#ifndef NTL__KM_FILE_INFORMATION
