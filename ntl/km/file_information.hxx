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
using nt::file_standard_information;
using nt::file_network_open_information;

using nt::query_information_file_t;
using nt::set_information_file_t;

NTL__EXTERNAPI query_information_file_t ZwQueryInformationFile;
NTL__EXTERNAPI set_information_file_t   ZwSetInformationFile;


using nt::file_basic_information;
using nt::file_disposition_information;


template<class InformationClass>
struct file_information
: public nt::file_information_base <InformationClass,
                                    ZwQueryInformationFile,
                                    ZwSetInformationFile>
{
  file_information(legacy_handle file_handle) throw()
  : nt::file_information_base<InformationClass, ZwQueryInformationFile, ZwSetInformationFile>(file_handle)
  {/**/}

  file_information(
    legacy_handle             file_handle,
    const InformationClass &  info) throw()
  : nt::file_information_base<InformationClass,
                              ZwQueryInformationFile,
                              ZwSetInformationFile>(file_handle, info)
  {/**/}
};


///}

/**@} file_information */

}//namespace km
}//namespace ntl

#endif//#ifndef NTL__KM_FILE_INFORMATION
