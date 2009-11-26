/**\file*********************************************************************
 *                                                                     \brief
 *  C Library General utilities (ISO9899 7.20)
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_CSTDLIB
#define NTL__STLX_CSTDLIB
#pragma once

#ifndef NTL__STLX_CSTDINT
# include "cstddef.hxx"
#endif
#include "cassert.hxx"

#ifndef NTL__ATOMIC
# include "../atomic.hxx"
#endif

#ifndef NTL__EXT_NUMERIC_CONVERSIONS
#include "ext/numeric_conversions.hxx"
#endif

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
inline
int
NTL__CRTCALL
  atoi(const char *nptr)
{
  return ntl::numeric::strtoi(nptr, 0, 10);
}

/// 7.20.1.2 The atol function
inline
long int
NTL__CRTCALL
  atol(const char *nptr)
{
  return ntl::numeric::strtol(nptr, 0, 10);
}

/// 7.20.1.3 The strtod function
double
NTL__CRTCALL
  strtod(const char * __restrict nptr, char ** __restrict endptr);

/// 7.20.1.4 The strtol function
inline
long int
NTL__CRTCALL
  strtol(const char * __restrict nptr, char ** __restrict endptr, int base)
{
  return ntl::numeric::strtol(nptr, (const char**)endptr, base);
}

/// 7.20.1.4 The strtoul function
inline
unsigned long int
NTL__CRTCALL
  strtoul(const char * __restrict nptr, char ** __restrict endptr, int base)
{
  return ntl::numeric::strtoul(nptr, (const char**)endptr, base);
}

// C library extension
inline
NTL__CRTCALL
char* __cdecl
  _itoa(int value, char* strbuf, int radix)
{
  return ntl::numeric::itoa(value, strbuf, ntl::numeric::max_number_size, radix);
}

inline
NTL__CRTCALL
wchar_t* __cdecl
  _itow(int value, wchar_t* strbuf, int radix)
{
  return ntl::numeric::itow(value, strbuf, ntl::numeric::max_number_size, radix);
}

///\name  7.20.2 Pseudo-random sequence generation functions

__declspec(selectany) std::uint32_t __ntl_rand_seed = 0;

/// 7.20.2.1 The rand function
inline int NTL__CRTCALL rand(void)
{
  ntl::atomic::compare_exchange(__ntl_rand_seed, static_cast<uint32_t>(ntl::intrinsic::rdtsc()), 0); // init seed if 0
  std::uint32_t seed = __ntl_rand_seed;
  ntl::intrinsic::_ReadBarrier();
  std::uint32_t val = ntl::nt::RtlRandom(&seed);
  ntl::atomic::exchange(__ntl_rand_seed, seed); // store new seed back. rand() is not atomic function, but it doesn't matter
  return val;
}

#ifndef RAND_MAX
#define RAND_MAX 0xffffffff // LONG_MAX
#endif

//STATIC_ASSERT(RAND_MAX <= INT_MAX);

/// 7.20.2.2 The srand function
inline void NTL__CRTCALL srand(unsigned int seed)
{
  ntl::atomic::exchange(__ntl_rand_seed, seed);
}


///\name  7.20.3 Memory management functions

/// 7.20.3.1 The calloc function
__noalias
void* __restrict 
NTL__CRTCALL
  calloc(size_t nmemb, size_t size);

/// 7.20.3.2 The free function
__noalias
void
NTL__CRTCALL
  free(void *ptr);

/// 7.20.3.3 The malloc function
__noalias
void* __restrict 
NTL__CRTCALL
  malloc(size_t size);

/// 7.20.3.4 The realloc function
__noalias
void* __restrict
NTL__CRTCALL
  realloc(void *ptr, size_t size);


///\name  7.20.4 Communication with the environment

/// 7.20.4.1 The abort function + 18.3/3
void __cdecl abort();

/// 7.20.4.2 The atexit function
int __cdecl atexit(vfv_t* func);

#define EXIT_SUCCESS  0
#define EXIT_FAILURE  1

/// 7.20.4.3 The exit function
void NTL__CRTCALL exit(int status);

int __cdecl at_quick_exit(vfv_t* f);

void NTL__CRTCALL quick_exit(int status);


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
int __cdecl abs(int n);
long int __cdecl labs(long int n);
#ifdef _MSC_VER
long long __cdecl _abs64(long long n);
#pragma intrinsic(abs,labs,_abs64)
#if _MSC_VER >= 1600
long long __cdecl llabs(long long int n);
#pragma intrinsic(llabs)
#else
inline long long int llabs(long long int n) { return _abs64(n); }
#endif
#endif

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
NTL__EXTERNAPI int NTL__CRTIMP mbtowc(wchar_t * __restrict pwc, const char * __restrict s, size_t n);

/// 7.20.7.3 The wctomb function
NTL__EXTERNAPI int NTL__CRTIMP wctomb(char *s, wchar_t wc);

#endif// #ifndef _INC_STDLIB

/// 7.20.1.2 The atoll function
inline
long long int
NTL__CRTCALL
  atoll(const char *nptr)
{
  return ntl::numeric::strtoll(nptr, 0, 10);
}

/// 7.20.1.3 The strtof function
float
NTL__CRTCALL
  strtof(const char * __restrict nptr, char ** __restrict endptr);


/// 7.20.1.3 The strtold function
long double
NTL__CRTCALL
  strtold(const char * __restrict nptr, char ** __restrict endptr);

/// 7.20.1.4 The strtoll function
inline
long long int
NTL__CRTCALL
  strtoll(const char * __restrict nptr, char ** __restrict endptr, int base)
{
  return ntl::numeric::strtoll(nptr, (const char**)endptr, base);
}

/// 7.20.1.4 The strtoull function
inline
unsigned long long int
NTL__CRTCALL
  strtoull(const char * __restrict nptr, char ** __restrict endptr, int base)
{
  return ntl::numeric::strtoull(nptr, (const char**)endptr, base);
}

/// 7.20.4.4 The _Exit function
void
NTL__CRTCALL
  _Exit(int status);





//////////////////////////////////////////////////////////////////////////
/* c++ initializers */
extern vfv_t* __xc_a[];
extern vfv_t* __xc_z[];

} // extern "C"
///@}

namespace ntl
{

  /**
   *	@brief C++ static initialization
   *	@param[in] init initialization or termination flag
   *  @sa crt_initializer
   *
   *  This function initializes and destroys the C++ static objects. It must be called before and after the main program code runs.
   *  For example:
   \code
   int main()
   {
      ntl::__init_crt(true);
      int ret_code = do_main_work();
      ntl::__init_crt(false);
      return ret_code;
   }
   \endcode
   **/
  extern "C" void __cdecl __init_crt(bool init);

  extern "C" void __cdecl __init_iostream_objects(bool init);

  /**
   *	@brief C++ static initialization helper
   *
   *  This class helps to initialize the C++ static objects initialization and destruction. For usage see following code:
   *  @sa __init_crt
   \code
   int main()
   {
      ntl::crt_initializer __crt;
      return do_main_work();
   }
   \endcode
   **/
  struct crt_initializer
  {
    crt_initializer()
    {
      __init_crt(true);
    }
    ~crt_initializer()
    {
      __init_crt(false);
    }
  };
} // ntl


namespace std {

/**\addtogroup  lib_language_support *** 18 Language support library [language.support]
 *@{*/

/**\addtogroup  lib_general_utilities ** C Library filees [c.files  ]
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
using ::exit;
using ::quick_exit;
using ::atexit;
using ::at_quick_exit;

using ::getenv;
using ::system;

using::bsearch;
using::qsort;

using::abs;
using::labs;
using::llabs;

inline long abs(long n) { return labs(n); }
inline long long abs(long long n) { return _abs64(n); }

#if 0
using::div;
using::ldiv;
using::lldiv;
#endif

using::mblen;
using::mbtowc;
using::wctomb;

/**@} lib_general_utilities
 */
/**@} lib_language_support */

}//namespace std

#endif//#ifndef NTL__STLX_CSTDLIB
