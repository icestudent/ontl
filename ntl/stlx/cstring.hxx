/**\file*********************************************************************
 *                                                                     \brief
 *  21.4 Null-terminated sequence utilities [lib.c.strings]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_CSTRING
#define NTL__STLX_CSTRING
#pragma once

// MSVC winnt.h" compatibility
extern "C++" {

#ifndef NTL__STLX_CSTDDEF
#include "cstddef.hxx"
#endif

#include "cassert.hxx"

#ifdef __ICL
#pragma warning(disable:2259) // [remark] non-pointer conversion from
#endif

namespace std
{

/**\addtogroup  lib_strings ************ 21 Strings library [strings]
 *@{*/
/**\addtogroup  lib_c_strings ********** 21.5 Null-terminated sequence utilities [c.strings]
 *@{*/
/**\addtogroup  lib_c_strings_cstring String handling
 *@{*/

#ifdef _MSC_VER
  // It is rumored that
  // "The compiler may call the function and not replace the function call
  // with inline instructions, _if_it_will_result_in_better_performance_"
  // And this is right in some cases.
  NTL_EXTERNAPI int    __cdecl memcmp(const void * s1, const void * s2, size_t n);
  NTL_EXTERNAPI void * __cdecl memcpy(void * const dst, const void * const src, size_t n);
  NTL_EXTERNAPI void * __cdecl memset(void * const s, int c, size_t n);
  NTL_EXTERNAPI int    __cdecl strcmp(const char * s1, const char * s2);
  NTL_EXTERNAPI char * __cdecl strcpy(char * const dst, const char * const src);
  NTL_EXTERNAPI size_t __cdecl strlen(const char * const s);
  NTL_EXTERNAPI char * __cdecl strcat(char * const dst, const char * const src);
#pragma intrinsic(memcmp, memcpy, memset, strcmp, strcpy, strlen, strcat)
#endif


///\name Copying functions

namespace ext {
__forceinline
void *
  memcpy(void * const dst, const void * const src, size_t n)
{
  assert(dst || n == 0);
  assert(src || n == 0);
  const char * s = reinterpret_cast<const char*>(src);
  char * d = reinterpret_cast<char*>(dst);
  while ( n-- ) *d++ = *s++;
  return d;
} }

#ifndef _MSC_VER
__forceinline
void *
NTL_CRTCALL
  memcpy(void * const dst, const void * const src, size_t n)
{
  assert(dst || n == 0);
  assert(src || n == 0);
  ext::memcpy(dst, src, n);
  return dst;
}
#endif

namespace ext {
__forceinline
void *
  memcpybw(void * const dst, const void * const src, size_t n)
{
  assert(dst || n == 0);
  assert(src || n == 0);
  const char * const s = reinterpret_cast<const char*>(src);
  char * const d = reinterpret_cast<char*>(dst);
  while ( n-- ) d[n] = s[n];
  return dst;
} }

__forceinline
void *
NTL_CRTCALL
  memmove(void * const dst, const void * const src, size_t n)
{
  assert(dst || n == 0);
  assert(src || n == 0);
  return ( reinterpret_cast<char*>(dst) < reinterpret_cast<const char*>(src) )
     ? memcpy(dst, src, n) : ext::memcpybw(dst, src, n);
}

namespace ext {
  __forceinline
    char *
    strcpy(char * dst, const char * src)
  {
    assert(dst);
    assert(src);
    while ( (*dst++ = *src++) != 0 ) {/**/}
    return dst;
  } }

#ifndef _MSC_VER
__forceinline
char *
NTL_CRTCALL
strcpy(char * const dst, const char * const src)
{
  assert(dst);
  assert(src);
  ext::strcpy(dst, src);
  return dst;
}

#endif

__forceinline
char* NTL_CRTCALL strncpy(char* dst, const char* src, size_t n)
{
  assert(dst || n == 0);
  assert(src || n == 0);
  while (n && (*dst++ = *src++) != 0)
    n--;
  if(n)
    *dst++ = 0;
  return dst;
}

///\name Concatenation functions
namespace ext {
__forceinline
char *
  strcat(char * dst, const char * src)
{
  assert(dst);
  assert(src);
  while ( *dst ) ++dst;
  do *dst++ = *src; while ( *src++ );
  return dst;
} }

#ifndef _MSC_VER
__forceinline
char *
NTL_CRTCALL
  strcat(char * const dst, const char * const src)
{
  assert(dst);
  assert(src);
  ext::strcat(dst, src);
  return dst;
}
#endif

NTL_EXTERNAPI char* NTL_CRTIMP strncat(char * __restrict s1, const char * __restrict s2, size_t n);

///\name Comparison functions
#ifndef _MSC_VER
__forceinline
int
NTL_CRTCALL
memcmp(const void * s1, const void * s2, size_t n)
{
  assert(s1 || n == 0);
  assert(s2 || n == 0);
  const unsigned char * p  = reinterpret_cast<const unsigned char*>(s1);
  const unsigned char * p2 = reinterpret_cast<const unsigned char*>(s2);
  for ( const unsigned char * const end = p + n; p != end; ++p, ++p2 )
    if ( *p != *p2 ) return *p - *p2;
  return 0;
}

__forceinline
int
NTL_CRTCALL
strcmp(const char * s1, const char * s2)
{
  assert(s1);
  assert(s2);
  for ( ; ; ++s1, ++s2 )
    if ( !*s1 || *s1 != *s2 )
      return  *reinterpret_cast<const unsigned char*>(s1)
      - *reinterpret_cast<const unsigned char*>(s2);
}
#endif

int NTL_CRTCALL strcoll(const char *s1, const char *s2);

__forceinline
int
NTL_CRTCALL
strncmp(const char * s1, const char * s2, size_t n)
{
  assert(s1 || n == 0);
  assert(s2 || n == 0);
  for ( ; n; --n, ++s1, ++s2 ){
    if ( !*s1 || *s1 != *s2 ){
      return  *reinterpret_cast<const unsigned char*>(s1)
      - *reinterpret_cast<const unsigned char*>(s2);
    }
  }
  return static_cast<int>(n); // n == 0
}

size_t NTL_CRTCALL strxfrm(char * __restrict s1, const char * __restrict s2, size_t n);

///\name Search functions
__forceinline
const void * NTL_CRTCALL memchr(const void * const mem, const int c, size_t n)
{
  assert(mem || n == 0);
  const unsigned char * p = reinterpret_cast<const unsigned char*>(mem);
  while ( n-- )
    if ( static_cast<unsigned char>(c) != *p ) ++p;
    else return p;
  return 0;
}

__forceinline
void * NTL_CRTCALL memchr(void * const mem, const int c, size_t n)
{
  return const_cast<void*>(memchr(const_cast<const void* const>(mem), c, n));
}

__forceinline
const char * NTL_CRTCALL strchr(const char * const s, int c)
{
  assert(s);
  const char* p = s;
  while ( *p )
    if ( static_cast<unsigned char>(c) != *p ) ++p;
    else return p;
  return 0;
}

__forceinline
char * NTL_CRTCALL strchr(char * const s, int c)
{
  return const_cast<char*>(strchr(const_cast<const char*>(s),c));
}

size_t NTL_CRTCALL strcspn(const char *s1, const char *s2);
char * NTL_CRTCALL strpbrk(const char *s1, const char *s2);

NTL_EXTERNAPI char * NTL_CRTIMP strrchr(const char *s, int c);
NTL_EXTERNAPI size_t NTL_CRTIMP strspn(const char *s1, const char *s2);
NTL_EXTERNAPI char * NTL_CRTIMP strstr(const char *str, const char *substr);

char * NTL_CRTCALL strtok(char * __restrict s1, const char * __restrict s2);

///\name Miscellaneous functions
namespace ext {
__forceinline
void *
  memset(void * const s, int c, size_t n)
{
  assert(s || n == 0);
  unsigned char * p = reinterpret_cast<unsigned char*>(s);
  while ( n-- )
    *p++ = static_cast<unsigned char>(c);
  return p;
} }

#ifndef _MSC_VER
__forceinline
void *
NTL_CRTCALL
  memset(void * const s, int c, size_t n)
{
  assert(s || n == 0);
  unsigned char * const p = reinterpret_cast<unsigned char*>(s);
  for ( unsigned i = 0; i != n; ++i )
    p[i] = static_cast<unsigned char>(c);
  return s;
}

__forceinline
size_t
NTL_CRTCALL
strlen(const char * const s)
{
  assert(s);
  size_t count = 0;
  while ( s[count] ) count++;
  return count;
}
#endif

__forceinline void NTL_CRTCALL bzero(void* s, size_t n)
{
  memset(s,0,n);
}

char* NTL_CRTCALL strdup(const char* s);

/**@} lib_c_strings_cstring */
/**@} lib_c_strings */
/**@} lib_strings */

}//namespace std

// functions exported by ntdll.dll and ntoskrnl.exe
#ifndef _INC_STDLIB// MSVC compatibility

NTL_EXTERNAPI
int NTL_CRTIMP
  _stricmp(
    const char * s1,
    const char * s2
    );

NTL_EXTERNAPI
int NTL_CRTIMP
  _wcsicmp(
    const wchar_t * s1,
    const wchar_t * s2
    );

NTL_EXTERNAPI
int NTL_CRTIMP
  _strnicmp(
    const char *  s1,
    const char *  s2,
    size_t        n
    );

NTL_EXTERNAPI
int NTL_CRTIMP
  _wcsnicmp(
    const wchar_t * s1,
    const wchar_t * s2,
    size_t          n
    );

  namespace std
  {
    inline int NTL_CRTCALL stricmp(const char* s1, const char* s2)
    {
      return _stricmp(s1,s2);
    }
    inline int NTL_CRTCALL wcsicmp(const wchar_t* s1, const wchar_t* s2)
    {
      return _wcsicmp(s1,s2);
    }
    inline int NTL_CRTCALL strnicmp(const char* s1, const char* s2, size_t n)
    {
      return _strnicmp(s1,s2,n);
    }
    inline int NTL_CRTCALL  wcsnicmp(const wchar_t* s1, const wchar_t* s2, size_t n)
    {
      return _wcsnicmp(s1,s2,n);
    }


  }

#endif // #ifndef _INC_STDLIB

#ifdef __ICL
#pragma warning(default:2259)
#endif

// MSVC winnt.h" compatibility
}//extern "C++" {

/// external CRT
namespace ntl { namespace msvcrt
{
  char* NTL_CRTCALL strtok(char* string, const char* format, char** context);
}}

#endif//#ifndef NTL__STLX_CSTRING
