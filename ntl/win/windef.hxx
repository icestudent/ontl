/**\file*********************************************************************
 *                                                                     \brief
 *  Basic Win API types 
 *
 ****************************************************************************
 */

#ifndef NTL__WIN_WINDEF
#define NTL__WIN_WINDEF

#include "../nt/basedef.hxx"

namespace ntl {
namespace win {

/**\addtogroup  winapi_types_support *** Win32 API support library **********
 *@{*/


using nt::slist_header;
using nt::single_list_entry;
using nt::slist_entry;
using nt::list_entry;
using nt::list_head;

using nt::access_mask;
using nt::no_access;
using nt::delete_access;
using nt::read_control;
using nt::write_dac;
using nt::write_owner;
using nt::synchronize;
using nt::standard_rights_required;
using nt::standard_rights_read;
using nt::standard_rights_write;
using nt::standard_rights_execute;
using nt::standard_rights_all;
using nt::specific_rights_all;
using nt::access_system_security;
using nt::maximum_allowed;
using nt::generic_read;
using nt::generic_write;
using nt::generic_execute;
using nt::generic_all;

/**@} winapi_types_support */


}//namespace win
}//namespace ntl

#endif//#ifndef NTL__WIN_WINDEF
