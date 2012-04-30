/**\file*********************************************************************
 *                                                                     \brief
 *  Win API Errors support
 *
 ****************************************************************************
 */
#ifndef NTL__WIN_ERROR
#define NTL__WIN_ERROR
#pragma once

#include "../nt/win32_error.hxx"
#include "../stlx/system_error.hxx"

namespace ntl { namespace win {
  namespace error = nt::win32_error;

  using ntl::nt::win32error;
 }
}

namespace std
{
  using ntl::nt::make_error_code;
  using ntl::nt::make_error_condition;

}

#endif//#ifndef NTL__WIN_ERROR
