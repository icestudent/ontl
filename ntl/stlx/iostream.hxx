/**\file*********************************************************************
 *                                                                     \brief
 *  Standard iostream objects [27.3 lib.iostream.objects]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_IOSTREAM
#define NTL__STLX_IOSTREAM

#include "iosfwd.hxx"
#if 1
#include <ios>
#include <streambuf>
#include <istream>
#include <ostream>
#endif

namespace std {

/**\addtogroup  lib_input_output ******* [27] Input/output library [input.output] **********
 *@{*/

///\name  Standard iostream objects [27.3 lib.iostream.objects]
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

#endif//#ifndef NTL__STLX_IOSTREAM
