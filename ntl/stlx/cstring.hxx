/**\file*********************************************************************
 *                                                                     \brief
 *  21.4 Null-terminated sequence utilities [lib.c.strings]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CSTRING
#define NTL__STLX_CSTRING

#include "cstddef.hxx"
#include "cassert.hxx"


namespace std {

/**\addtogroup  lib_strings ************ Strings library [21] ****************
 *@{*/
/**\addtogroup  lib_c_strings ********** Null-terminated sequence utilities [21.4]
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

__forceinline 
const void *
NTL__CRTCALL
  memchr(const void * const mem, const int c, size_t n)
{
  _Assert(mem);
  const unsigned char * p = reinterpret_cast<const unsigned char*>(mem);
  while ( n-- )
    if ( static_cast<unsigned char>(c) != *p ) ++p;
    else return p;
  return 0;
}

__forceinline 
void *
NTL__CRTCALL
  memchr(void * const mem, const int c, size_t n)
{
  _Assert(mem);
  unsigned char * p = reinterpret_cast<unsigned char*>(mem);
  while ( n-- )
    if ( static_cast<unsigned char>(c) != *p ) ++p;
    else return p;
  return 0;
}

#ifndef _MSC_VER
__forceinline 
int
NTL__CRTCALL
  memcmp(const void * s1, const void * s2, size_t n)
{
  _Assert(s1);
  _Assert(s2);
  const unsigned char * p  = reinterpret_cast<const unsigned char*>(s1);
  const unsigned char * p2 = reinterpret_cast<const unsigned char*>(s2);
  for ( const unsigned char * const end = p + n; p != end; ++p, ++p2 )
    if ( *p != *p2 ) return *p - *p2;
  return 0;  
}
#endif

namespace x {
__forceinline 
void *
  memcpy(void * const dst, const void * const src, size_t n)
{
  _Assert(dst);
  _Assert(src);
  const char * s = reinterpret_cast<const char*>(src);
  char * d = reinterpret_cast<char*>(dst);
  while ( n-- ) *d++ = *s++;
  return d;
} }

#ifndef _MSC_VER
__forceinline 
void *
NTL__CRTCALL
  memcpy(void * const dst, const void * const src, size_t n)
{
  _Assert(dst);
  _Assert(src);
  x::memcpy(dst, src, n);
  return dst;
}
#endif

namespace x {
__forceinline 
void *
  memcpybw(void * const dst, const void * const src, size_t n)
{
  _Assert(dst);
  _Assert(src);
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
  _Assert(dst);
  _Assert(src);
  return ( reinterpret_cast<char*>(dst) < reinterpret_cast<const char*>(src) )
     ? memcpy(dst, src, n) : x::memcpybw(dst, src, n);
}


namespace x {
__forceinline 
void *
  memset(void * const s, int c, size_t n)
{
  _Assert(s);
  unsigned char * p = reinterpret_cast<unsigned char*>(s);
  while ( n-- )
    *p++ = static_cast<unsigned char>(c);
  return p;
} }

#ifndef _MSC_VER
__forceinline 
void *
NTL__CRTCALL
  memset(void * const s, int c, size_t n)
{
  _Assert(s);
  unsigned char * const p = reinterpret_cast<unsigned char*>(s);
  for ( unsigned i = 0; i != n; ++i )
    p[i] = static_cast<unsigned char>(c);
  return s;
}
#endif


namespace x {
__forceinline 
char *
  strcat(char * dst, const char * src)
{
  _Assert(dst);
  _Assert(src);
  while ( *dst ) ++dst;
  do *dst++ = *src; while ( *src++ );
  return dst;
} }

#ifndef _MSC_VER
__forceinline 
char *
NTL__CRTCALL
  strcat(char * const dst, const char * const src)
{
  _Assert(dst);
  _Assert(src);
  x::strcat(dst, src);
  return dst;
}
#endif


__forceinline 
const char *
NTL__CRTCALL
  strchr(const char * const s, int c)
{
  _Assert(s);
  while ( *s )
    if ( static_cast<unsigned char>(c) 
        == *reinterpret_cast<const unsigned char*>(s) )
      return s;
  return 0;
}

__forceinline 
char *
NTL__CRTCALL
  strchr(char * const s, int c)
{
  _Assert(s);
  while ( *s )
    if ( static_cast<unsigned char>(c) == *reinterpret_cast<unsigned char*>(s) )
      return s;
  return 0;
}


#ifndef _MSC_VER
__forceinline 
int
NTL__CRTCALL
  strcmp(const char * s1, const char * s2)
{
  _Assert(s1);
  _Assert(s2);
  for ( ; ; ++s1, ++s2 )
    if ( !*s1 || *s1 != *s2 )
      return  *reinterpret_cast<const unsigned char*>(s1)
            - *reinterpret_cast<const unsigned char*>(s2);
}
#endif

__forceinline 
int
NTL__CRTCALL
  strncmp(const char * s1, const char * s2, size_t n)
{
  _Assert(s1);
  _Assert(s2);
  for ( ; n; --n, ++s1, ++s2 )
    if ( !*s1 || *s1 != *s2 )
      return  *reinterpret_cast<const unsigned char*>(s1)
            - *reinterpret_cast<const unsigned char*>(s2);
  return static_cast<int>(n); // n == 0  
}


namespace x {
__forceinline 
char *
  strcpy(char * dst, const char * src)
{
  _Assert(dst);
  _Assert(src);
  while ( (*dst++ = *src++) != 0 ) {/**/}
  return dst;
} }

#ifndef _MSC_VER
__forceinline 
char *
NTL__CRTCALL
  strcpy(char * const dst, const char * const src)
{
  _Assert(dst);
  _Assert(src);
  x::strcpy(dst, src);
  return dst;
}

__forceinline 
size_t
NTL__CRTCALL
  strlen(const char * const s)
{
  _Assert(s);
  size_t count = 0;
  while ( s[count] ) count++;
  return count;
}
#endif

__forceinline 
char *
NTL__CRTCALL
strdup(const char * const src)
{
  if(!src)
    return NULL;
  size_t len = strlen(src);
  char* dst = new char[len];
  strcpy(dst, src);
  return dst;
}



/**@} lib_c_strings */
/**@} lib_strings */

}//namespace std

// functions exported by ntdll.dll and ntoskrnl.exe
#ifndef _INC_STDLIB// MSVC compatibility

NTL__EXTERNAPI
int __cdecl
  _stricmp(
    const char * s1,
    const char * s2 
    );

NTL__EXTERNAPI
int __cdecl
  _wcsicmp(
    const wchar_t * s1,
    const wchar_t * s2 
    );

NTL__EXTERNAPI
int __cdecl
  _strnicmp(
    const char *  s1,
    const char *  s2,
    size_t        n 
    );

NTL__EXTERNAPI
int __cdecl
  _wcsnicmp(
    const wchar_t * s1,
    const wchar_t * s2,
    size_t          n 
    );

#endif // #ifndef _INC_STDLIB

#endif//#ifndef NTL__STLX_CSTRING
