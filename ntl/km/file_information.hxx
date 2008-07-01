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

NTL__EXTERNAPI query_information_file_t ZwQueryInformationFile;
NTL__EXTERNAPI set_information_file_t   ZwSetInformationFile;

using nt::file_basic_information;
using nt::file_standard_information;
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


template<>
struct file_information<file_rename_information>
{
    file_information(
      legacy_handle                   file_handle,
      const file_rename_information & info) __ntl_nothrow
    : status_(_set(file_handle, &info,
              sizeof(info) + info.FileNameLength - sizeof(wchar_t)))
    {/**/}

    operator bool() const { return nt::success(status_); }

    operator ntstatus() const { return status_; }

    static __forceinline
    ntstatus
      _set(
        legacy_handle   file_handle,
        const void *    info,
        unsigned long   info_length
        )
    {
      io_status_block iosb;
      return ZwSetInformationFile(file_handle, &iosb, info, info_length,
                                  file_rename_information::info_class_type);
    }

  private:
    ntstatus    status_;
    
};


///}

/**@} file_information */

}//namespace km
}//namespace ntl

#endif//#ifndef NTL__KM_FILE_INFORMATION
