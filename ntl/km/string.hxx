/**\file*********************************************************************
 *                                                                     \brief
 *  Kernel Mode NT string types
 *
 ****************************************************************************
 */

#ifndef NTL__KM_STRING
#define NTL__KM_STRING

#include "../nt/string.hxx"

namespace ntl {
namespace km {

using  nt::unicode_string;
using  nt::const_unicode_string;

using  nt::ansi_string;
using  nt::const_ansi_string;

}//namespace ntl
}//namespace nt

#endif//#ifndef NTL__KM_STRING
