/**\file*********************************************************************
 *                                                                     \brief
 *  21.4 Null-terminated sequence utilities [lib.c.strings]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CSTRING
#define NTL__STLX_CSTRING

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
  NTL__EXTERNAPI int    __cdecl memcmp(const void * s1, const void * s2, size_t n);
  NTL__EXTERNAPI void * __cdecl memcpy(void * const dst, const void * const src, size_t n);
  NTL__EXTERNAPI void * __cdecl memset(void * const s, int c, size_t n);
  NTL__EXTERNAPI int    __cdecl strcmp(const char * s1, const char * s2);
  NTL__EXTERNAPI char * __cdecl strcpy(char * const dst, const char * const src);
  NTL__EXTERNAPI size_t __cdecl strlen(const char * const s);
  NTL__EXTERNAPI char * __cdecl strcat(char * const dst, const char * const src);
#pragma intrinsic(memcmp, memcpy, memset, strcmp, strcpy, strlen, strcat)
#endif


///\name Copying functions
#ifndef _MSC_VER
namespace __ {
__forceinline
void *
  memcpy(void * const dst, const void * const src, size_t n)
{
  assert(dst);
  assert(src);
  const char * s = reinterpret_cast<const char*>(src);
  char * d = reinterpret_cast<char*>(dst);
  while ( n-- ) *d++ = *s++;
  return d;
} }

__forceinline
void *
NTL__CRTCALL
  memcpy(void * const dst, const void * const src, size_t n)
{
  assert(dst);
  assert(src);
  __::memcpy(dst, src, n);
  return dst;
}
#endif

namespace __ {
__forceinline
void *
  memcpybw(void * const dst, const void * const src, size_t n)
{
  assert(dst);
  assert(src);
  const char * const s = reinterpret_cast<const char*>(src);
  char * const d = reinterpret_cast<char*>(dst);
  while ( n-- ) d[n] = s[n];
  return dst;
} }

__forceinline
void *
NTL__CRTCALL
  memmove(void * const dst, const void * const src, size_t n)
{
  assert(dst);
  assert(src);
  return ( reinterpret_cast<char*>(dst) < reinterpret_cast<const char*>(src) )
     ? memcpy(dst, src, n) : __::memcpybw(dst, src, n);
}

#ifndef _MSC_VER
namespace __ {
  __forceinline
    char *
    strcpy(char * dst, const char * src)
  {
    assert(dst);
    assert(src);
    while ( (*dst++ = *src++) != 0 ) {/**/}
    return dst;
  } }

__forceinline
char *
NTL__CRTCALL
strcpy(char * const dst, const char * const src)
{
  assert(dst);
  assert(src);
  __::strcpy(dst, src);
  return dst;
}

#endif

__forceinline
char* NTL__CRTCALL strncpy(char* dst, const char* src, size_t n)
{
  assert(dst);
  assert(src);
  while (n && (*dst++ = *src++) != 0)
    n--;
  if(n)
    *dst++ = 0;
  return dst;
}

///\name Concatenation functions
#ifndef _MSC_VER
namespace __ {
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

__forceinline
char *
NTL__CRTCALL
  strcat(char * const dst, const char * const src)
{
  assert(dst);
  assert(src);
  __::strcat(dst, src);
  return dst;
}

NTL__EXTERNAPI char* NTL__CRTIMP strncat(char * __restrict s1, const char * __restrict s2, size_t n);
#endif

///\name Comparison functions
#ifndef _MSC_VER
__forceinline
int
NTL__CRTCALL
memcmp(const void * s1, const void * s2, size_t n)
{
  assert(s1);
  assert(s2);
  const unsigned char * p  = reinterpret_cast<const unsigned char*>(s1);
  const unsigned char * p2 = reinterpret_cast<const unsigned char*>(s2);
  for ( const unsigned char * const end = p + n; p != end; ++p, ++p2 )
    if ( *p != *p2 ) return *p - *p2;
  return 0;
}

__forceinline
int
NTL__CRTCALL
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

int NTL__CRTCALL strcoll(const char *s1, const char *s2);

__forceinline
int
NTL__CRTCALL
strncmp(const char * s1, const char * s2, size_t n)
{
  assert(s1);
  assert(s2);
  for ( ; n; --n, ++s1, ++s2 )
    if ( !*s1 || *s1 != *s2 )
      return  *reinterpret_cast<const unsigned char*>(s1)
      - *reinterpret_cast<const unsigned char*>(s2);
  return static_cast<int>(n); // n == 0
}

size_t NTL__CRTCALL strxfrm(char * __restrict s1, const char * __restrict s2, size_t n);

///\name Search functions
__forceinline
const void * NTL__CRTCALL memchr(const void * const mem, const int c, size_t n)
{
  assert(mem);
  const unsigned char * p = reinterpret_cast<const unsigned char*>(mem);
  while ( n-- )
    if ( static_cast<unsigned char>(c) != *p ) ++p;
    else return p;
  return 0;
}

__forceinline
void * NTL__CRTCALL memchr(void * const mem, const int c, size_t n)
{
  return const_cast<void*>(memchr(mem, c, n));
}

__forceinline
const char * NTL__CRTCALL strchr(const char * const s, int c)
{
  assert(s);
  while ( *s )
    if ( static_cast<unsigned char>(c)
      == *reinterpret_cast<const unsigned char*>(s) )
      return s;
  return 0;
}

__forceinline
char * NTL__CRTCALL strchr(char * const s, int c)
{
  return const_cast<char*>(strchr(s,c));
}

size_t NTL__CRTCALL strcspn(const char *s1, const char *s2);
char * NTL__CRTCALL strpbrk(const char *s1, const char *s2);

NTL__EXTERNAPI char * NTL__CRTIMP strrchr(const char *s, int c);
NTL__EXTERNAPI size_t NTL__CRTIMP strspn(const char *s1, const char *s2);
NTL__EXTERNAPI char * NTL__CRTIMP strstr(const char *s1, const char *s2);

char * NTL__CRTCALL strtok(char * __restrict s1, const char * __restrict s2);

///\name Miscellaneous functions
#ifndef _MSC_VER
namespace __ {
__forceinline
void *
  memset(void * const s, int c, size_t n)
{
  assert(s);
  unsigned char * p = reinterpret_cast<unsigned char*>(s);
  while ( n-- )
    *p++ = static_cast<unsigned char>(c);
  return p;
} }

__forceinline
void *
NTL__CRTCALL
  memset(void * const s, int c, size_t n)
{
  assert(s);
  unsigned char * const p = reinterpret_cast<unsigned char*>(s);
  for ( unsigned i = 0; i != n; ++i )
    p[i] = static_cast<unsigned char>(c);
  return s;
}

__forceinline
size_t
NTL__CRTCALL
strlen(const char * const s)
{
  assert(s);
  size_t count = 0;
  while ( s[count] ) count++;
  return count;
}
#endif


inline char* NTL__CRTCALL strdup(const char* s)
{
  if(!s)
    return nullptr;
  char* const p = new char[strlen(s)];
  return p ? strcpy(p, s) : p;
}

/**@} lib_c_strings_cstring */
/**@} lib_c_strings */
/**@} lib_strings */

}//namespace std

// functions exported by ntdll.dll and ntoskrnl.exe
#ifndef _INC_STDLIB// MSVC compatibility

NTL__EXTERNAPI
int NTL__CRTIMP
  _stricmp(
    const char * s1,
    const char * s2
    );

NTL__EXTERNAPI
int NTL__CRTIMP
  _wcsicmp(
    const wchar_t * s1,
    const wchar_t * s2
    );

NTL__EXTERNAPI
int NTL__CRTIMP
  _strnicmp(
    const char *  s1,
    const char *  s2,
    size_t        n
    );

NTL__EXTERNAPI
int NTL__CRTIMP
  _wcsnicmp(
    const wchar_t * s1,
    const wchar_t * s2,
    size_t          n
    );

#endif // #ifndef _INC_STDLIB

#ifdef __ICL
#pragma warning(default:2259)
#endif

#endif//#ifndef NTL__STLX_CSTRING
