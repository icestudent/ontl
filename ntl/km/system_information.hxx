/**\file*********************************************************************
 *                                                                     \brief
 *  System Information
 *
 ****************************************************************************
 */
#ifndef NTL__KM_SYSTEM_INFORMATION
#define NTL__KM_SYSTEM_INFORMATION
#pragma once

#include "basedef.hxx"
#include "../nt/system_information.hxx"


namespace ntl {
namespace km {

/**\addtogroup  system_information ********* System Information *************
 *@{*/

using nt::system_basic_information;
using nt::system_thread_information;
using nt::system_process_information;
using nt::system_processes;

using nt::rtl_process_module_information;
using nt::system_modules_information;


using nt::query_system_information_t;
//using nt::set_system_information_t;

NTL_EXTERNAPI query_system_information_t ZwQuerySystemInformation;


template<class InformationClass>
struct system_information
: public nt::system_information_base <InformationClass,
                                      ZwQuerySystemInformation>
{};


/**@} system_information */

}//namespace km
}//namespace ntl

#endif//#ifndef NTL__KM_SYSTEM_INFORMATION
