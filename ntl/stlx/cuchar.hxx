/**\file*********************************************************************
 *                                                                     \brief
 *  21.5 Null-terminated sequence utilities [strings]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_CUCHAR
#define NTL__STLX_CUCHAR

#include "cstdint.hxx"
#include "cwchar.hxx"

#ifndef NTL__CXX_CHARS
# define __STDC_UTF_16__ 1
# define __STDC_UTF_32__ 0
#else
# pragma error todo
#endif

namespace std {

#ifndef NTL__CXX_CHARS
  typedef uint_least16_t char16_t;
  typedef uint_least32_t char32_t;
#endif

  size_t c16rtomb(char *__restrict s, char16_t wc, mbstate_t *__restrict ps);
  size_t c32rtomb(char *__restrict s, char16_t wc, mbstate_t *__restrict ps);

  size_t mbrtoc16(char16_t *__restrict pwc, const char *__restrict s, size_t n, mbstate_t *__restrict ps);
  size_t mbrtoc32(char16_t *__restrict pwc, const char *__restrict s, size_t n, mbstate_t *__restrict ps);

} // std
#endif // NTL__STLX_CUCHAR
