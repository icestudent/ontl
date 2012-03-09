/**\file*********************************************************************
 *                                                                     \brief
 *  File Information
 *
 ****************************************************************************
 */
#ifndef NTL__KM_FILE_INFORMATION
#define NTL__KM_FILE_INFORMATION
#pragma once

#include "basedef.hxx"
#include "handle.hxx"
#include "../nt/file_information.hxx"


namespace ntl {
namespace km {

/**\addtogroup  file_information ********* File Information ***************
 *@{*/

using nt::file_attribute;

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

NTL_EXTERNAPI query_information_file_t ZwQueryInformationFile;
NTL_EXTERNAPI set_information_file_t   ZwSetInformationFile;
NTL_EXTERNAPI io_query_information_file_t IoQueryFileInformation;
NTL_EXTERNAPI io_set_information_file_t   IoSetInformation;

using nt::file_basic_information;
using nt::file_standard_information;
//using nt::file_name_information;
using nt::file_rename_information;
using nt::file_disposition_information;
using nt::file_end_of_file_information;
using nt::file_network_open_information;
using nt::file_internal_information;
using nt::file_position_information;

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
      return ZwSetInformationFile(file_handle, &iosb, info, info_length, info_class::info_class_type);
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

///\name  FileLinkInformation == 11
#pragma warning(disable:4510 4610)
struct file_link_information: file_rename_information
{
  static const file_information_class info_class_type = nt::FileLinkInformation;

  typedef std::unique_ptr<file_link_information> ptr;
};
#pragma warning(default:4510 4610)

template<>
struct file_information<file_link_information>:
  __::file_rename_information_impl<file_link_information>
{
  file_information(legacy_handle file_handle, const const_unicode_string& new_name, bool replace_if_exists, legacy_handle root_directory = legacy_handle())
    :file_rename_information_impl(file_handle, new_name, replace_if_exists, root_directory)
  {}
};

///}

/**@} file_information */

NTL_EXTERNAPI query_information_file_t ZwQueryVolumeInformationFile;
NTL_EXTERNAPI set_information_file_t ZwSetVolumeInformationFile;

using nt::fs_information_class;

template<class InformationClass>
struct volume_information:
  public nt::file_information_base <InformationClass, ZwQueryVolumeInformationFile, ZwSetVolumeInformationFile>
{
  volume_information(legacy_handle file_handle) __ntl_nothrow
    :file_information_base<InformationClass, ZwQueryVolumeInformationFile, ZwSetVolumeInformationFile>(file_handle)
  {}

  volume_information(legacy_handle file_handle, const InformationClass& info) __ntl_nothrow
    :file_information_base<InformationClass, NtQueryVolumeInformationFile, NtSetVolumeInformationFile>(file_handle, info)
  {}
};

using nt::file_fs_volume_information;
using nt::file_fs_label_information;
using nt::file_fs_size_information;
using nt::file_fs_device_information;
using nt::file_fs_attribute_information;
using nt::file_fs_control_information;

//using nt::volume_information<nt::file_fs_volume_information>;
template<>
struct volume_information<file_fs_volume_information>
{
  typedef file_fs_volume_information info_class;

  explicit volume_information(legacy_handle volume_handle, bool with_label = true)
  {
    // length of the label is 34 characters long max
    for(uint32_t length = sizeof(info_class)+sizeof(wchar_t)*34; ptr.reset(new char[length]), ptr; length*= 2)
    {
      status_ = query(volume_handle, ptr.get(), length);
      if(status_ == status::success){
        break;
      }else if(status_ != status::buffer_overflow){
        ptr.reset();
        break;
      }else if(!with_label && status_ == status::buffer_overflow){
        break;
      }
    }
  }

  const info_class* operator->() const { return data(); }
  info_class* operator->() { return data(); }

  info_class* data() { return reinterpret_cast<info_class*>(ptr.get()); }
  const info_class* data() const { return reinterpret_cast<const info_class*>(ptr.get()); }
  operator const void*() const { return ptr.get(); }

  operator bool() const { return nt::success(status_); }
  operator ntstatus() const { return status_; }

  static __forceinline
    ntstatus query(
    legacy_handle volume_handle,
    void*     volume_information,
    uint32_t  volume_information_length
    )
  {
    io_status_block iosb;
    return NtQueryVolumeInformationFile(volume_handle, &iosb,
      volume_information, volume_information_length, info_class::info_class_type);
  }
private:
  std::unique_ptr<char[]> ptr;
  ntstatus status_;
};


}//namespace km
}//namespace ntl

#endif//#ifndef NTL__KM_FILE_INFORMATION
