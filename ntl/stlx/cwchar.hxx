/**\file*********************************************************************
 *                                                                     \brief
 *  21.5 Null-terminated sequence utilities [strings]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CWCHAR
#define NTL__STLX_CWCHAR

#include "cstddef.hxx"
#include "cassert.hxx"
#include "cstdarg.hxx"
#include "cstring.hxx"

#ifndef WCHAR_MIN
#define WCHAR_MIN   0
#endif

#ifndef WCHAR_MAX
#define WCHAR_MAX   0xffff
#endif

/** an object type other than an array type that can hold the conversion state
  information necessary to convert between sequences of multibyte characters and wide
  characters */
typedef int mbstate_t;

/** an integer type unchanged by default argument promotions that can hold any
  value corresponding to members of the extended character set, as well as at least one
  value that does not correspond to any member of the extended character set */
typedef unsigned short wint_t;

#ifndef WEOF
  // expands to a constant expression of type wint_t whose value does not correspond to any member of the extended character set.
# define WEOF ((wint_t)0xFFFF)
#endif

namespace std {

/**\addtogroup  lib_strings ************ 21 Strings library [strings] ****************
 *@{*/
/**\addtogroup  lib_c_strings ********** 21.5 Null-terminated sequence utilities [c.strings]
 *@{*/
/**\addtogroup  lib_c_strings_cwchar Extended multibyte and wide character utilities
 *@{*/


  ///\name Formatted wide character input/output functions
  NTL__EXTERNAPI
    size_t NTL__CRTIMP
    _snwprintf(wchar_t*buffer, size_t count, const wchar_t*format, ...);

  NTL__EXTERNAPI
    size_t NTL__CRTIMP
    _vsnwprintf(wchar_t*buffer, size_t count, const wchar_t*format, va_list argptr);

  ///\name General wide string utilities

  ///\name Wide string numeric conversion functions
  int NTL__CRTCALL _wtoi(const wchar_t* str);
  long int NTL__CRTCALL _wtol(const wchar_t*nptr);

  double wcstod(const wchar_t* __restrict nptr, wchar_t** __restrict endptr);
  float wcstof(const wchar_t* __restrict nptr, wchar_t** __restrict endptr);
  long double wcstold(const wchar_t* __restrict nptr, wchar_t** __restrict endptr);

  long int wcstol(const wchar_t* __restrict nptr, wchar_t** __restrict endptr, int base);
  long long int wcstoll(const wchar_t* __restrict nptr, wchar_t** __restrict endptr, int base);
  unsigned long int wcstoul(const wchar_t* __restrict nptr, wchar_t** __restrict endptr, int base);
  unsigned long long int wcstoull(const wchar_t* __restrict nptr, wchar_t** __restrict endptr, int base);

  ///\name Wide string copying functions
__forceinline
wchar_t* NTL__CRTCALL wcscpy(wchar_t* dst, const wchar_t* src)
{
  _Assert(dst);
  _Assert(src);
  while ( (*dst++ = *src++) != 0 ) {/**/}
  return dst;
}

__forceinline
wchar_t* NTL__CRTCALL wcsncpy(wchar_t* dst, const wchar_t* src, size_t n)
{
  _Assert(dst);
  _Assert(src);
  while (n && (*dst++ = *src++) != 0)
    n--;
  if(n)
    *dst++ = 0;
  return dst;
}

__forceinline
wchar_t* NTL__CRTCALL wmemcpy(wchar_t* __restrict s1, const wchar_t* __restrict s2, size_t n)
{
  memcpy(s1, s2, n*sizeof(wchar_t));
  return s1;
}

__forceinline
wchar_t* NTL__CRTCALL wmemmove(wchar_t*s1, const wchar_t*s2, size_t n)
{
  memmove(s1, s2, n*sizeof(wchar_t));
  return s1;
}

///\name Wide string concatenation functions
NTL__EXTERNAPI wchar_t* NTL__CRTIMP wcscat(wchar_t* __restrict s1, const wchar_t* __restrict s2);
NTL__EXTERNAPI wchar_t* NTL__CRTIMP wcsncat(wchar_t* __restrict s1, const wchar_t* __restrict s2, size_t n);

///\name Wide string comparison functions
__forceinline
int NTL__CRTCALL wcscmp(const wchar_t*s1, const wchar_t*s2)
{
  _Assert(s1);
  _Assert(s2);
  for ( ; ; ++s1, ++s2 ) if ( *s1 != *s2 || !*s1 ) return *s1 - *s2;
}

int NTL__CRTCALL wcscoll(const wchar_t*s1, const wchar_t*s2);

__forceinline
int NTL__CRTCALL wcsncmp(const wchar_t* s1, const wchar_t* s2, size_t n)
{
  _Assert(s1);
  _Assert(s2);
  for ( ; n; --n, ++s1, ++s2 ) if ( *s1 != *s2 || !*s1 ) return *s1 - *s2;
  return static_cast<int>(n); // n == 0
}

size_t NTL__CRTCALL wcsxfrm(wchar_t* __restrict s1, const wchar_t* __restrict s2, size_t n);

__forceinline
int NTL__CRTCALL wmemcmp(const wchar_t* s1, const wchar_t* s2, size_t n)
{
  return memcmp(s1, s2, n*sizeof(wchar_t));
}

///\name Wide string search functions
NTL__EXTERNAPI wchar_t* NTL__CRTIMP wcschr(const wchar_t*s, wchar_t c);

NTL__EXTERNAPI size_t   NTL__CRTIMP wcscspn(const wchar_t*s1, const wchar_t*s2);

wchar_t* NTL__CRTCALL wcspbrk(const wchar_t*s1, const wchar_t*s2);

NTL__EXTERNAPI wchar_t* NTL__CRTIMP wcsrchr(const wchar_t*s, wchar_t c);

NTL__EXTERNAPI size_t   NTL__CRTIMP wcsspn(const wchar_t*s1, const wchar_t*s2);

NTL__EXTERNAPI wchar_t* NTL__CRTIMP wcsstr(const wchar_t*s1, const wchar_t*s2);

wchar_t* NTL__CRTCALL wcstok(wchar_t* __restrict s1, const wchar_t* __restrict s2, wchar_t** __restrict ptr);

wchar_t* NTL__CRTCALL wmemchr(const wchar_t*s, wchar_t c, size_t n);

///\name Miscellaneous functions

__forceinline
size_t NTL__CRTCALL wcslen(const wchar_t* const s)
{
  _Assert(s);
  size_t count = 0;
  while ( s[count] ) count++;
  return count;
}

__forceinline
wchar_t* NTL__CRTCALL wmemset(wchar_t*s, wchar_t c, size_t n)
{
  memset(s, c, n*sizeof(wchar_t));
  return s;
}

///\name Wide character time conversion functions

///\name Multibyte/wide string conversion functions

NTL__EXTERNAPI size_t NTL__CRTIMP mbstowcs(wchar_t* __restrict wcstr, const char* __restrict mbstr, size_t count);

NTL__EXTERNAPI size_t NTL__CRTIMP wcstombs(char* __restrict mbstr, const wchar_t* __restrict wcstr, size_t count);

///\}

/**@} lib_c_strings_cwchar */
/**@} lib_c_strings */
/**@} lib_strings */

}//namespace std

#endif//#ifndef NTL__STLX_CWCHAR
