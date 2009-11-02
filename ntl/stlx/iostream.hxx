/**\file*********************************************************************
 *                                                                     \brief
 *  Standard iostream objects [27.3 lib.iostream.objects]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_IOSTREAM
#define NTL__STLX_IOSTREAM
#pragma once

#include "streambuf.hxx"
#include "istream.hxx"
#include "ostream.hxx"

namespace std
{

/**\addtogroup  lib_input_output ******* 27 Input/output library [input.output]
 *@{*/

///\name  27.3 Standard iostream objects [lib.iostream.objects]
extern istream  cin;
extern ostream  cout;
extern ostream  cerr;
extern ostream  clog;
extern wistream wcin;
extern wostream wcout;
extern wostream wcerr;
extern wostream wclog;

///@}
/**@} lib_input_output */

}//namespace std

namespace
{
  std::ios_base::Init __iostream_objects_initializer;
}

#endif // NTL__STLX_IOSTREAM
