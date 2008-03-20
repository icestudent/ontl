/**\file*********************************************************************
 *                                                                     \brief
 *  27.6.3 Standard manipulators [lib.std.manip]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_IOMANIP
#define NTL__STLX_IOMANIP

#include "iosfwd.hxx"

namespace std {

T1 resetiosflags(ios_base::fmtflags mask);

T2 setiosflags(ios_base::fmtflags mask);

T3 setbase(int base);

template<charT> T4 setfill(charT c);

T5 setprecision(int n);

T6 setw(int n);

}//namespace std

#endif//#ifndef NTL__STLX_IOMANIP
