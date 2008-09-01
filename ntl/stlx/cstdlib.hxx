/**\file*********************************************************************
 *                                                                     \brief
 *  C Library General utilities (ISO9899 7.20)
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CSTDLIB
#define NTL__STLX_CSTDLIB

#include "cstddef.hxx"

extern "C"
{
  typedef void __cdecl vfv_t(void);
  typedef int  __cdecl ifv_t(void);
  typedef void __cdecl vfi_t(int);

#ifndef _INC_STDLIB// MSVC compatibility

// ISO 9899
//

///\name  7.20.1 Numeric conversion functions

/// 7.20.1.1 The atof function
double
NTL__CRTCALL
  atof(const char *nptr);

/// 7.20.1.2 The atoi function
int
NTL__CRTCALL
  atoi(const char *nptr);

/// 7.20.1.2 The atol function
long int
NTL__CRTCALL
  atol(const char *nptr);

/// 7.20.1.3 The strtod function
double
NTL__CRTCALL
  strtod(const char * __restrict nptr, char ** __restrict endptr);

/// 7.20.1.4 The strtol function
long int
NTL__CRTCALL
  strtol(const char * __restrict nptr, char ** __restrict endptr, int base);

/// 7.20.1.4 The strtoul function
unsigned long int
NTL__CRTCALL
  strtoul(const char * __restrict nptr, char ** __restrict endptr, int base);


///\name  7.20.2 Pseudo-random sequence generation functions

/// 7.20.2.1 The rand function
int
NTL__CRTCALL
  rand(void);

#ifndef RAND_MAX
#define RAND_MAX 0x7fff
#endif

//STATIC_ASSERT(RAND_MAX <= INT_MAX);

/// 7.20.2.2 The srand function
void
NTL__CRTCALL
  srand(unsigned int seed);


///\name  7.20.3 Memory management functions

/// 7.20.3.1 The calloc function
__declspec(restrict) __declspec(noalias)
void *
NTL__CRTCALL
  calloc(size_t nmemb, size_t size);

/// 7.20.3.2 The free function
__declspec(noalias)
void
NTL__CRTCALL
  free(void *ptr);

/// 7.20.3.3 The malloc function
__declspec(restrict) __declspec(noalias)
void *
NTL__CRTCALL
  malloc(size_t size);

/// 7.20.3.4 The realloc function
__declspec(restrict) __declspec(noalias)
void *
NTL__CRTCALL
  realloc(void *ptr, size_t size);


///\name  7.20.4 Communication with the environment

/// 7.20.4.1 The abort function + 18.3/3
void
NTL__CRTCALL
  abort();

/// 7.20.4.2 The atexit function
int __cdecl atexit(vfv_t func);

#define EXIT_SUCCESS  0
#define EXIT_FAILURE  1

/// 7.20.4.3 The exit function
void
NTL__CRTCALL
  exit(int status);

/// 7.20.4.5 The getenv function
char *
NTL__CRTCALL
  getenv(const char *name);

/// 7.20.4.6 The system function
int
NTL__CRTCALL
  system(const char *string);


///\name  7.20.5 Searching and sorting utilities

/// 7.20.5.1 The bsearch function
void *
NTL__CRTCALL
  bsearch(const void *key, const void *base, size_t nmemb, size_t size,
              int (*compar)(const void *, const void *));

/// 7.20.5.2 The qsort function
void
NTL__CRTCALL
  qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *));


///\name  7.20.6 Integer arithmetic functions

/// 7.20.6.1 The abs, labs and llabs functions
int
NTL__CRTCALL
  abs(int j);

long int
NTL__CRTCALL
  labs(long int j);

#if 0
/// 7.20.6.2 The div, ldiv, and lldiv functions
div_t div(int numer, int denom);
ldiv_t ldiv(long int numer, long int denom);
lldiv_t lldiv(long long int numer, long long int denom);
#endif


///\name  7.20.7 Multibyte/wide character conversion functions

/// 7.20.7.1 The mblen function
int
NTL__CRTCALL
  mblen(const char *s, size_t n);

/// 7.20.7.2 The mbtowc function
int
NTL__CRTCALL
  mbtowc(wchar_t * __restrict pwc, const char * __restrict s, size_t n);

/// 7.20.7.3 The wctomb function
int
NTL__CRTCALL
  wctomb(char *s, wchar_t wc);


///\name  7.20.8 Multibyte/wide string conversion functions

/// 7.20.8.1 The mbstowcs function
size_t
NTL__CRTCALL
  mbstowcs(wchar_t * __restrict pwcs, const char * __restrict s, size_t n);

/// 7.20.8.2 The wcstombs function
size_t
NTL__CRTCALL
  wcstombs(char * __restrict s, const wchar_t * __restrict pwcs, size_t n);

#endif// #ifndef _INC_STDLIB

/// 7.20.1.2 The atoll function
long long int
NTL__CRTCALL
  atoll(const char *nptr);

/// 7.20.1.3 The strtof function
float
NTL__CRTCALL
  strtof(const char * __restrict nptr, char ** __restrict endptr);


/// 7.20.1.3 The strtold function
long double
NTL__CRTCALL
  strtold(const char * __restrict nptr, char ** __restrict endptr);

/// 7.20.1.4 The strtoll function
long long int
NTL__CRTCALL
  strtoll(const char * __restrict nptr, char ** __restrict endptr, int base);

/// 7.20.1.4 The strtoull function
unsigned long long int
NTL__CRTCALL
  strtoull(const char * __restrict nptr, char ** __restrict endptr, int base);

/// 7.20.4.4 The _Exit function
void
NTL__CRTCALL
  _Exit(int status);

long long int
NTL__CRTCALL
  llabs(long long int j);




//////////////////////////////////////////////////////////////////////////
/* c++ initializers */
extern vfv_t* __xc_a[];
extern vfv_t* __xc_z[];

}; // extern "C"
///@}

namespace std {

/**\addtogroup  lib_language_support *** Language support library [18] ******
 *@{*/
/**\addtogroup  lib_support_start_term * Start and termination [18.3] *******
 *@{*/


//extern "C"    int atexit(void (*f)(void));
//extern "C++"  int atexit(void (*f)(void));


/**@} lib_support_start_term
 */

/**\addtogroup  lib_general_utilities ** Library General utilities [C 7.20] *******
 *@{*/

using ::atof;
using ::atoi;
using ::atol;
using ::atoll;
using ::strtod;
using ::strtof;
using ::strtold;
using ::strtol;
using ::strtoll;
using ::strtoul;
using ::strtoull;

using ::rand;
using ::srand;

using ::calloc;
using ::free;
using ::malloc;
using ::realloc;

using ::abort;
using ::atexit;
using ::exit;
using ::_Exit;

using ::getenv;
using ::system;

using::bsearch;
using::qsort;

using::abs;
using::labs;
using::llabs;

#if 0
using::div;
using::ldiv;
using::lldiv;
#endif

using::mblen;
using::mbtowc;
using::wctomb;

using::mbstowcs;
using::wcstombs;

/**@} lib_general_utilities
 */
/**@} lib_language_support */

}//namespace std

#endif//#ifndef NTL__STLX_CSTDLIB
