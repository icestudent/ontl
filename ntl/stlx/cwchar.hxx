/**\file*********************************************************************
 *                                                                     \brief
 *  21.4 Null-terminated sequence utilities [lib.c.strings]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CWCHAR
#define NTL__STLX_CWCHAR

#include "cstddef.hxx"
#include "cassert.hxx"
#include "cstdarg.hxx"

#ifndef WCHAR_MIN
#define WCHAR_MIN   0
#endif

#ifndef WCHAR_MAX
#define WCHAR_MAX   0xffff
#endif

typedef int mbstate_t;

typedef unsigned short wint_t;

#ifndef WEOF
#define WEOF ((wint_t)0xFFFF)
#endif

namespace std {

/**\addtogroup  lib_strings ************ Strings library [21] ****************
 *@{*/
/**\addtogroup  lib_c_strings ********** Null-terminated sequence utilities [21.4]
 *@{*/

__forceinline
int
NTL__CRTCALL
  wcscmp(const wchar_t *s1, const wchar_t *s2)
{
  _Assert(s1);
  _Assert(s2);
  for ( ; ; ++s1, ++s2 ) if ( *s1 != *s2 || !*s1 ) return *s1 - *s2;
}

__forceinline
int
NTL__CRTCALL
  wcsncmp(const wchar_t * s1, const wchar_t * s2, size_t n)
{
  _Assert(s1);
  _Assert(s2);
  for ( ; n; --n, ++s1, ++s2 ) if ( *s1 != *s2 || !*s1 ) return *s1 - *s2;
  return static_cast<int>(n); // n == 0  
}

__forceinline 
wchar_t *
  wcscpy(wchar_t * dst, const wchar_t * src)
{
  _Assert(dst);
  _Assert(src);
  while ( (*dst++ = *src++) != 0 ) {/**/}
  return dst;
}


__forceinline 
size_t
NTL__CRTCALL
  wcslen(const wchar_t * const s)
{
  _Assert(s);
  size_t count = 0;
  while ( s[count] ) count++;
  return count;
}

NTL__EXTERNAPI size_t _cdecl
  _snwprintf(wchar_t *buffer, size_t count, const wchar_t *format, ...);
NTL__EXTERNAPI size_t _cdecl
  _vsnwprintf(wchar_t *buffer, size_t count, const wchar_t *format, va_list argptr);

/**@} lib_c_strings */
/**@} lib_strings */

}//namespace std

#endif//#ifndef NTL__STLX_CWCHAR
