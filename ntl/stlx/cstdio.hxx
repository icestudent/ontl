/**\file*********************************************************************
 *                                                                     \brief
 *  C Library files utilities [c.files]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_CSTDIO
#define NTL__STLX_CSTDIO
#pragma once

#ifndef NTL__STLX_CSTDDEF
#include "cstddef.hxx"
#endif
#include "cstdarg.hxx"
//using std::va_list;


#ifndef _INC_STDLIB// MSVC compatibility

NTL__EXTERNAPI size_t __cdecl
  sprintf(char *buffer, const char *format, ...);

NTL__EXTERNAPI size_t __cdecl
  _snprintf(char *buffer, size_t count, const char *format, ...);

NTL__EXTERNAPI size_t __cdecl
  _vsnprintf(char *buffer, size_t count, const char *format, va_list argptr);

NTL__EXTERNAPI wchar_t* __cdecl _wcslwr(wchar_t * str);

//NTL__EXTERNAPI size_t _cdecl
//  _snwprintf(wchar_t *buffer, size_t count, const wchar_t *format, ...);
//NTL__EXTERNAPI size_t _cdecl
//  _vsnwprintf(wchar_t *buffer, size_t count, const wchar_t *format, va_list argptr);


#endif


namespace std {

/**\addtogroup  lib_language_support *** 18 Language support library [language.support]
 *@{*/

/**\addtogroup  lib_general_utilities ** C Library filees [c.files  ]
 *@{*/

  typedef long long fpos_t;

  using ::_snprintf; using ::_vsnprintf;


/**@} lib_general_utilities
 */
/**@} lib_language_support */

}//namespace std

#endif //#ifndef NTL__STLX_CSTDIO
