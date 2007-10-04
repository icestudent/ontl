/**\file*********************************************************************
 *                                                                     \brief
 *
 *
 ****************************************************************************
 */

#ifndef NTL__BASEDEF
#define NTL__BASEDEF

#include "cstddef"
#include "cstdint"
#include "vector"

/// NT Native Template Lybrary
namespace ntl {

/**\addtogroup  ntative_types ********** NT Native Types ********************
 *@{*/

///\name  Exact-width integer types
using std::int8_t;
using std::uint8_t;
using std::int16_t;
using std::uint16_t;
using std::int32_t;
using std::uint32_t;
using std::int64_t;
using std::uint64_t;

///\name  Integer types capable of holding object pointers
using std::intptr_t;
using std::uintptr_t;

using std::ptrdiff_t;

///\name  

typedef std::vector<uint8_t> raw_data;

///@}

}//namespace ntl

#endif//#ifndef NTL__BASEDEF
