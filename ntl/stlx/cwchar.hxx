/**\file*********************************************************************
 *                                                                     \brief
 *  21.5 Null-terminated sequence utilities [strings]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_CWCHAR
#define NTL__STLX_CWCHAR
#pragma once

#ifndef NTL__STLX_CSTRING
#include "cstring.hxx"
#endif
#include "cstdarg.hxx"

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

/**\addtogroup  lib_strings ************ 21 Strings library [strings]
 *@{*/
/**\addtogroup  lib_c_strings ********** 21.5 Null-terminated sequence utilities [c.strings]
 *@{*/
/**\addtogroup  lib_c_strings_cwchar Extended multibyte and wide character utilities
 *@{*/
  using ::mbstate_t;
  using ::wint_t;

  ///\name Formatted wide character input/output functions
  NTL_EXTERNAPI
    size_t NTL_CRTIMP
    _snwprintf(wchar_t*buffer, size_t count, const wchar_t*format, ...);

  ///\warning not available in W2K ntoskrnl
  NTL_EXTERNAPI
    size_t NTL_CRTIMP
    _vsnwprintf(wchar_t*buffer, size_t count, const wchar_t*format, va_list argptr);

  ///\name General wide string utilities

  ///\name Wide string numeric conversion functions
  int NTL_CRTCALL _wtoi(const wchar_t* str);
  long int NTL_CRTCALL _wtol(const wchar_t*nptr);

  double wcstod(const wchar_t* __restrict nptr, wchar_t** __restrict endptr);
  float wcstof(const wchar_t* __restrict nptr, wchar_t** __restrict endptr);
  long double wcstold(const wchar_t* __restrict nptr, wchar_t** __restrict endptr);

  long int wcstol(const wchar_t* __restrict nptr, wchar_t** __restrict endptr, int base);
  long long int wcstoll(const wchar_t* __restrict nptr, wchar_t** __restrict endptr, int base);
  unsigned long int wcstoul(const wchar_t* __restrict nptr, wchar_t** __restrict endptr, int base);
  unsigned long long int wcstoull(const wchar_t* __restrict nptr, wchar_t** __restrict endptr, int base);

  ///\name Wide string copying functions
#ifdef _MSC_VER
  NTL_EXTERNAPI int      __cdecl wcscmp(const wchar_t * s1, const wchar_t * s2);
  NTL_EXTERNAPI wchar_t* __cdecl wcscpy(wchar_t * const dst, const wchar_t * const src);
  NTL_EXTERNAPI size_t   __cdecl wcslen(const wchar_t * const s);
  NTL_EXTERNAPI wchar_t* __cdecl wcscat(wchar_t * const dst, const wchar_t * const src);
#pragma intrinsic(wcscmp,wcscpy,wcslen,wcscat)
#endif

#ifndef _MSC_VER
__forceinline
wchar_t* NTL_CRTCALL wcscpy(wchar_t* dst, const wchar_t* src)
{
  assert(dst);
  assert(src);
  while ( (*dst++ = *src++) != 0 ) {/**/}
  return dst;
}
#endif

__forceinline
wchar_t* NTL_CRTCALL wcsncpy(wchar_t* dst, const wchar_t* src, size_t n)
{
  assert(dst);
  assert(src);
  while (n && (*dst++ = *src++) != 0)
    n--;
  if(n)
    *dst++ = 0;
  return dst;
}

__forceinline
wchar_t* NTL_CRTCALL wmemcpy(wchar_t* __restrict s1, const wchar_t* __restrict s2, size_t n)
{
  memcpy(s1, s2, n*sizeof(wchar_t));
  return s1;
}

__forceinline
wchar_t* NTL_CRTCALL wmemmove(wchar_t*s1, const wchar_t*s2, size_t n)
{
  memmove(s1, s2, n*sizeof(wchar_t));
  return s1;
}

///\name Wide string concatenation functions
#ifndef _MSC_VER
NTL_EXTERNAPI wchar_t* NTL_CRTIMP wcscat(wchar_t* __restrict s1, const wchar_t* __restrict s2);
#endif
NTL_EXTERNAPI wchar_t* NTL_CRTIMP wcsncat(wchar_t* __restrict s1, const wchar_t* __restrict s2, size_t n);

///\name Wide string comparison functions
#ifndef _MSC_VER
__forceinline
int NTL_CRTCALL wcscmp(const wchar_t*s1, const wchar_t*s2)
{
  assert(s1);
  assert(s2);
  for ( ; ; ++s1, ++s2 ) if ( *s1 != *s2 || !*s1 ) return *s1 - *s2;
}
#endif

int NTL_CRTCALL wcscoll(const wchar_t*s1, const wchar_t*s2);

__forceinline
int NTL_CRTCALL wcsncmp(const wchar_t* s1, const wchar_t* s2, size_t n)
{
  assert(s1);
  assert(s2);
  for ( ; n; --n, ++s1, ++s2 ) if ( *s1 != *s2 || !*s1 ) return *s1 - *s2;
  return static_cast<int>(n); // n == 0
}

size_t NTL_CRTCALL wcsxfrm(wchar_t* __restrict s1, const wchar_t* __restrict s2, size_t n);

__forceinline
int NTL_CRTCALL wmemcmp(const wchar_t* s1, const wchar_t* s2, size_t n)
{
  return memcmp(s1, s2, n*sizeof(wchar_t));
}

///\name Wide string search functions
NTL_EXTERNAPI wchar_t* NTL_CRTIMP wcschr(const wchar_t*s, wchar_t c);

NTL_EXTERNAPI size_t   NTL_CRTIMP wcscspn(const wchar_t*s1, const wchar_t*s2);

wchar_t* NTL_CRTCALL wcspbrk(const wchar_t*s1, const wchar_t*s2);

NTL_EXTERNAPI wchar_t* NTL_CRTIMP wcsrchr(const wchar_t*s, wchar_t c);

NTL_EXTERNAPI size_t   NTL_CRTIMP wcsspn(const wchar_t*s1, const wchar_t*s2);

NTL_EXTERNAPI wchar_t* NTL_CRTIMP wcsstr(const wchar_t* str, const wchar_t* substr);

wchar_t* NTL_CRTCALL wcstok(wchar_t* __restrict s1, const wchar_t* __restrict s2, wchar_t** __restrict ptr);

wchar_t* NTL_CRTCALL wmemchr(const wchar_t*s, wchar_t c, size_t n);

///\name Miscellaneous functions

#ifndef _MSC_VER
__forceinline
size_t NTL_CRTCALL wcslen(const wchar_t* const s)
{
  assert(s);
  size_t count = 0;
  while ( s[count] ) count++;
  return count;
}
#endif

__forceinline
wchar_t* NTL_CRTCALL wmemset(wchar_t*s, wchar_t c, size_t n)
{
  memset(s, c, n*sizeof(wchar_t));
  return s;
}

///\name Wide character time conversion functions

///\name Multibyte/wide string conversion functions

NTL_EXTERNAPI size_t NTL_CRTIMP mbstowcs(wchar_t* __restrict wcstr, const char* __restrict mbstr, size_t count);

NTL_EXTERNAPI size_t NTL_CRTIMP wcstombs(char* __restrict mbstr, const wchar_t* __restrict wcstr, size_t count);

///\}

/**@} lib_c_strings_cwchar */
/**@} lib_c_strings */
/**@} lib_strings */

}//namespace std

#endif//#ifndef NTL__STLX_CWCHAR
