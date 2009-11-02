/**\file*********************************************************************
 *                                                                     \brief
 *  Numeric limits [18.2.1 lib.limits]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_LIMITS
#define NTL__STLX_LIMITS
#pragma once

#ifndef NTL__STLX_CSTDDEF
#include "cstddef.hxx"
#endif

#ifndef NTL__STLX_CSTDINT
#include "cstdint.hxx"
#endif
#include "cfloat.hxx"

#ifndef NTL__STLX_EXCPTDEF
#include "excptdef.hxx"
#endif

namespace std {

  /**\addtogroup  lib_language_support *** 18 Language support library [language.support]
 *@{*/
/**\addtogroup  lib_support_limits ***** 18.2 Implementation properties [support.limits]
 *@{*/

/// Type float_round_style [18.2.1.3 lib.round.style]
enum float_round_style
{
  round_indeterminate       = -1,
  round_toward_zero         = 0,
  round_to_nearest          = 1,
  round_toward_infinity     = 2,
  round_toward_neg_infinity = 3
};

/// Type float_denorm_style [18.2.1.4 lib.denorm.style]
enum float_denorm_style
{
  denorm_indeterminate  = -1,
  denorm_absent         = 0,
  denorm_present        = 1
};

/// Class template numeric_limits [18.2.1.1 lib.numeric.limits]
template<class T>
class numeric_limits
{
  public:
    static const  bool  is_specialized    = false;
    static        T     min()             __ntl_nothrow;// { return T(false); }
    static        T     max()             __ntl_nothrow;// { return T(false); }
    static        T     lowest()          __ntl_nothrow;// { return T(false); }
    static const  int   digits            = 0;
    static const  int   digits10          = 0;
    static const  bool  is_signed         = false;
    static const  bool  is_integer        = false;
    static const  bool  is_exact          = false;
    static const  int   radix             = 0;
    static        T     epsilon()         __ntl_nothrow;// { return T(false); }
    static        T     round_error()     __ntl_nothrow;// { return T(false); }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()        __ntl_nothrow;// { return T(false); }
    static        T     quiet_NaN()       __ntl_nothrow;// { return T(false); }
    static        T     signaling_NaN()   __ntl_nothrow;// { return T(false); }
    static        T     denorm_min()      __ntl_nothrow;// { return T(false); }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = false;
    static const  bool  is_modulo         = false;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
}; //template class numeric_limits

template<>
class numeric_limits<bool>
{
    typedef bool  T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min()         __ntl_nothrow { return false; }
    static        T     max()         __ntl_nothrow { return true; }
    static        T     lowest()      __ntl_nothrow { return false; }
    static const  int   digits            = 1;
    static const  int   digits10          = 0;
    static const  bool  is_signed         = false;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       __ntl_nothrow { return 0; }
    static        T     round_error()   __ntl_nothrow { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      __ntl_nothrow { return 0; }
    static        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static        T     signaling_NaN() __ntl_nothrow { return 0; }
    static        T     denorm_min()    __ntl_nothrow { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static const  T     __min             = false;
    static const  T     __max             = true;
    static const  T     __lowest          = false;
}; //template class numeric_limits<bool>

template<> class numeric_limits<char>
{
    typedef char  T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min()         __ntl_nothrow { return CHAR_MIN; }
    static        T     max()         __ntl_nothrow { return CHAR_MAX; }
    static        T     lowest()      __ntl_nothrow { return CHAR_MIN; }
    static const  int   digits            = CHAR_BIT + (CHAR_MIN>>(CHAR_BIT-1));
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = (CHAR_MIN>>(CHAR_BIT-1)) != 0;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       __ntl_nothrow { return 0; }
    static        T     round_error()   __ntl_nothrow { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      __ntl_nothrow { return 0; }
    static        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static        T     signaling_NaN() __ntl_nothrow { return 0; }
    static        T     denorm_min()    __ntl_nothrow { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static const  T     __min             = CHAR_MIN;
    static const  T     __max             = CHAR_MAX;
    static const  T     __lowest          = CHAR_MIN;
}; //template class numeric_limits<char>

template<> class numeric_limits<wchar_t>
{
    typedef wchar_t T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min()         __ntl_nothrow { return WCHAR_MIN; }
    static        T     max()         __ntl_nothrow { return WCHAR_MAX; }
    static        T     lowest()      __ntl_nothrow { return WCHAR_MIN; }
    static const  int   digits            = sizeof(T) * CHAR_BIT - (WCHAR_MIN ? 1 : 0);
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = (WCHAR_MIN ? 1 : 0);
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       __ntl_nothrow { return 0; }
    static        T     round_error()   __ntl_nothrow { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      __ntl_nothrow { return 0; }
    static        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static        T     signaling_NaN() __ntl_nothrow { return 0; }
    static        T     denorm_min()    __ntl_nothrow { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
}; //template class numeric_limits<wchar_t>

template<> class numeric_limits<signed char>
{
    typedef signed char T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min()         __ntl_nothrow { return SCHAR_MIN; }
    static        T     max()         __ntl_nothrow { return SCHAR_MAX; }
    static        T     lowest()      __ntl_nothrow { return SCHAR_MIN; }
    static const  int   digits            = CHAR_BIT - 1;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       __ntl_nothrow { return 0; }
    static        T     round_error()   __ntl_nothrow { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      __ntl_nothrow { return 0; }
    static        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static        T     signaling_NaN() __ntl_nothrow { return 0; }
    static        T     denorm_min()    __ntl_nothrow { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
}; //template class numeric_limits<signed char>

template<> class numeric_limits<short>
{
    typedef short T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min()         __ntl_nothrow { return SHRT_MIN; }
    static        T     max()         __ntl_nothrow { return SHRT_MAX; }
    static        T     lowest()      __ntl_nothrow { return SHRT_MIN; }
    static const  int   digits            = sizeof(T) * CHAR_BIT - 1;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       __ntl_nothrow { return 0; }
    static        T     round_error()   __ntl_nothrow { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      __ntl_nothrow { return 0; }
    static        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static        T     signaling_NaN() __ntl_nothrow { return 0; }
    static        T     denorm_min()    __ntl_nothrow { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static const  T     __min             = SHRT_MIN;
    static const  T     __max             = SHRT_MAX;
    static const  T     __lowest          = SHRT_MIN;
}; //template class numeric_limits<short>

template<> class numeric_limits<int>
{
    typedef int T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min()         __ntl_nothrow { return INT_MIN; }
    static        T     max()         __ntl_nothrow { return INT_MAX; }
    static        T     lowest()      __ntl_nothrow { return INT_MIN; }
    static const  int   digits            = sizeof(T) * CHAR_BIT - 1;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       __ntl_nothrow { return 0; }
    static        T     round_error()   __ntl_nothrow { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      __ntl_nothrow { return 0; }
    static        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static        T     signaling_NaN() __ntl_nothrow { return 0; }
    static        T     denorm_min()    __ntl_nothrow { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static const  T     __min             = INT_MIN;
    static const  T     __max             = INT_MAX;
    static const  T     __lowest          = INT_MIN;
}; //template class numeric_limits<int>

template<> class numeric_limits<long>
{
    typedef long T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min()         __ntl_nothrow { return LONG_MIN; }
    static        T     max()         __ntl_nothrow { return LONG_MAX; }
    static        T     lowest()      __ntl_nothrow { return LONG_MIN; }
    static const  int   digits            = sizeof(T) * CHAR_BIT - 1;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       __ntl_nothrow { return 0; }
    static        T     round_error()   __ntl_nothrow { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      __ntl_nothrow { return 0; }
    static        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static        T     signaling_NaN() __ntl_nothrow { return 0; }
    static        T     denorm_min()    __ntl_nothrow { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static const  T     __min             = LONG_MIN;
    static const  T     __max             = LONG_MAX;
    static const  T     __lowest          = LONG_MIN;
}; //template class numeric_limits<long>

template<> class numeric_limits<long long>
{
    typedef long long T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min()         __ntl_nothrow { return LLONG_MIN; }
    static        T     max()         __ntl_nothrow { return LLONG_MAX; }
    static        T     lowest()      __ntl_nothrow { return LLONG_MIN; }
    static const  int   digits            = sizeof(T) * CHAR_BIT - 1;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       __ntl_nothrow { return 0; }
    static        T     round_error()   __ntl_nothrow { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      __ntl_nothrow { return 0; }
    static        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static        T     signaling_NaN() __ntl_nothrow { return 0; }
    static        T     denorm_min()    __ntl_nothrow { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static const  T     __min             = LLONG_MIN;
    static const  T     __max             = LLONG_MAX;
    static const  T     __lowest          = LLONG_MIN;
}; //template class numeric_limits<long long>

template<> class numeric_limits<unsigned char>
{
    typedef unsigned char T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min()         __ntl_nothrow { return 0; }
    static        T     max()         __ntl_nothrow { return UCHAR_MAX; }
    static        T     lowest()      __ntl_nothrow { return 0; }
    static const  int   digits            = CHAR_BIT - 1;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = false;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       __ntl_nothrow { return 0; }
    static        T     round_error()   __ntl_nothrow { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      __ntl_nothrow { return 0; }
    static        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static        T     signaling_NaN() __ntl_nothrow { return 0; }
    static        T     denorm_min()    __ntl_nothrow { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static const  T     __min             = 0;
    static const  T     __max             = UCHAR_MAX;
    static const  T     __lowest          = 0;
}; //template class numeric_limits<unsigned char>

template<> class numeric_limits<unsigned short>
{
    typedef unsigned short T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min()         __ntl_nothrow { return 0; }
    static        T     max()         __ntl_nothrow { return USHRT_MAX; }
    static        T     lowest()      __ntl_nothrow { return 0; }
    static const  int   digits            = sizeof(T) * CHAR_BIT;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = false;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       __ntl_nothrow { return 0; }
    static        T     round_error()   __ntl_nothrow { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      __ntl_nothrow { return 0; }
    static        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static        T     signaling_NaN() __ntl_nothrow { return 0; }
    static        T     denorm_min()    __ntl_nothrow { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static const  T     __min             = 0;
    static const  T     __max             = USHRT_MAX;
    static const  T     __lowest          = 0;
}; //template class numeric_limits<unsigned shrt>

template<> class numeric_limits<unsigned int>
{
    typedef unsigned int T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min()         __ntl_nothrow { return 0; }
    static        T     max()         __ntl_nothrow { return UINT_MAX; }
    static        T     lowest()      __ntl_nothrow { return 0; }
    static const  int   digits            = sizeof(T) * CHAR_BIT;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = false;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       __ntl_nothrow { return 0; }
    static        T     round_error()   __ntl_nothrow { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      __ntl_nothrow { return 0; }
    static        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static        T     signaling_NaN() __ntl_nothrow { return 0; }
    static        T     denorm_min()    __ntl_nothrow { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static const  T     __min             = 0;
    static const  T     __max             = UINT_MAX;
    static const  T     __lowest          = 0;
}; //template class numeric_limits<unsigned int>

template<> class numeric_limits<unsigned long>
{
    typedef unsigned long T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min()         __ntl_nothrow { return 0; }
    static        T     max()         __ntl_nothrow { return ULONG_MAX; }
    static        T     lowest()      __ntl_nothrow { return 0; }
    static const  int   digits            = sizeof(T) * CHAR_BIT;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = false;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       __ntl_nothrow { return 0; }
    static        T     round_error()   __ntl_nothrow { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      __ntl_nothrow { return 0; }
    static        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static        T     signaling_NaN() __ntl_nothrow { return 0; }
    static        T     denorm_min()    __ntl_nothrow { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static const  T     __min             = 0;
    static const  T     __max             = ULONG_MAX;
    static const  T     __lowest          = 0;
}; //template class numeric_limits<unsigned long>

template<> class numeric_limits<unsigned long long>
{
    typedef unsigned long long T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min()         __ntl_nothrow { return 0; }
    static        T     max()         __ntl_nothrow { return ULLONG_MAX; }
    static        T     lowest()      __ntl_nothrow { return 0; }
    static const  int   digits            = sizeof(T) * CHAR_BIT;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = false;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       __ntl_nothrow { return 0; }
    static        T     round_error()   __ntl_nothrow { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      __ntl_nothrow { return 0; }
    static        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static        T     signaling_NaN() __ntl_nothrow { return 0; }
    static        T     denorm_min()    __ntl_nothrow { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static const  T     __min             = 0;
    static const  T     __max             = ULLONG_MAX;
    static const  T     __lowest          = 0;
}; //template class numeric_limits<unsigned long long>

namespace __
{
#if defined(__GNUC__)
  inline float float_infinity()     { return __builtin_huge_valf(); }
  inline float float_quiet_NaN()    { return __builtin_nanf("");    }
  inline float float_signaling_NaN(){ return __builtin_nansf("");   }
  inline float float_denorm_min()   { return __FLT_DENORM_MIN__;    }

  inline double double_infinity()     { return __builtin_huge_valf(); }
  inline double double_quiet_NaN()    { return __builtin_nanf("");    }
  inline double double_signaling_NaN(){ return __builtin_nansf("");   }
  inline double double_denorm_min()   { return __DBL_DENORM_MIN__;    }

  inline long double long_double_infinity()     { return __builtin_huge_valf(); }
  inline long double long_double_quiet_NaN()    { return __builtin_nanf("");    }
  inline long double long_double_signaling_NaN(){ return __builtin_nansf("");   }
  inline long double long_double_denorm_min()   { return __LDBL_DENORM_MIN__;   }
#else
  union fvalue { uint8_t raw[sizeof(float)]; float value; };
  STATIC_ASSERT(sizeof(fvalue) == 4);
  inline float float_infinity()
  {
    static const fvalue infinity      = { 0x00, 0x00, 0x80, 0x7F };
    return infinity.value;
  }
  inline float float_quiet_NaN()
  {
    static const fvalue quiet_NaN     = { 0x00, 0x00, 0xC0, 0x7F };
    return quiet_NaN.value;
  }
  inline float float_signaling_NaN()
  {
    static const fvalue signaling_NaN = { 0x01, 0x00, 0x80, 0x7F };
    return signaling_NaN.value;
  }
  inline float float_denorm_min()
  {
    static const fvalue denorm_min    = { 0x01, 0x00, 0x00, 0x00 };
    return denorm_min.value;
  }

  union dvalue { uint8_t raw[sizeof(double)]; double  value;};
  STATIC_ASSERT(sizeof(dvalue) == 8);
  inline double double_infinity()
  {
    static const dvalue infinity =
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x7F };
    return infinity.value;
  }
  inline double double_quiet_NaN()
  {
    static const dvalue quiet_NaN =
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x7F };
    return quiet_NaN.value;
  }
  inline double double_signaling_NaN()
  {
    static const dvalue signaling_NaN =
    { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x7F };
    return signaling_NaN.value;
  }
  inline double double_denorm_min()
  {
    static const dvalue denorm_min =
    { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    return denorm_min.value;
  }
#if DBL_DIG == LDBL_DIG
  union ldvalue { uint8_t raw[sizeof(double)]; long double value;};
  STATIC_ASSERT(sizeof(ldvalue) == 8);
  inline long double long_double_infinity()
  {
    static const ldvalue infinity =
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x7F };
    return infinity.value;
  }
  inline long double long_double_quiet_NaN()
  {
    static const ldvalue quiet_NaN =
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x7F };
    return quiet_NaN.value;
  }
  inline long double long_double_signaling_NaN()
  {
    static const ldvalue signaling_NaN =
    { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x7F };
    return signaling_NaN.value;
  }
  inline long double long_double_denorm_min()
  {
    static const ldvalue denorm_min =
    { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    return denorm_min.value;
  }
#else
  union ldvalue { uint8_t raw[sizeof(long double)]; long double value;};
  #error undefined
#endif

#endif
} // namespace __

template<> class numeric_limits<float>
{
    typedef float T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min() __ntl_nothrow { return FLT_MIN; }
    static        T     max() __ntl_nothrow { return FLT_MAX; }
    static        T     lowest()      __ntl_nothrow { return FLT_MIN; }
    static const  int   digits            = FLT_MANT_DIG;
    static const  int   digits10          = FLT_DIG;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = false;
    static const  bool  is_exact          = false;
    static const  int   radix             = FLT_RADIX;
    static        T     epsilon()     __ntl_nothrow { return FLT_EPSILON; }
    static        T     round_error() __ntl_nothrow { return 0.5; }
    static const  int   min_exponent      = FLT_MIN_EXP;
    static const  int   min_exponent10    = FLT_MIN_10_EXP;
    static const  int   max_exponent      = FLT_MAX_EXP;
    static const  int   max_exponent10    = FLT_MAX_10_EXP;
    static const  bool  has_infinity      = true;
    static const  bool  has_quiet_NaN     = true;
    static const  bool  has_signaling_NaN = true;
    static const  float_denorm_style  has_denorm  = denorm_present;
    static const  bool  has_denorm_loss   = true;
    static        T infinity()  __ntl_nothrow      { return __::float_infinity(); }
    static        T quiet_NaN() __ntl_nothrow      { return __::float_quiet_NaN(); }
    static        T signaling_NaN() __ntl_nothrow  { return __::float_signaling_NaN(); }
    static        T denorm_min() __ntl_nothrow     { return __::float_denorm_min(); }
    static const  bool  is_iec559         = true;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = false;
    static const  bool  traps             = true;
    static const  bool  tinyness_before   = true;
    static const  float_round_style round_style = round_to_nearest;
}; //template class numeric_limits<float>

template<> class numeric_limits<double>
{
    typedef double T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min() __ntl_nothrow { return DBL_MIN; }
    static        T     max() __ntl_nothrow { return DBL_MAX; }
    static        T     lowest()      __ntl_nothrow { return DBL_MIN; }
    static const  int   digits            = DBL_MANT_DIG;
    static const  int   digits10          = DBL_DIG;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = false;
    static const  bool  is_exact          = false;
    static const  int   radix             = DBL_RADIX;
    static        T     epsilon()     __ntl_nothrow { return DBL_EPSILON; }
    static        T     round_error() __ntl_nothrow { return 0.5; }
    static const  int   min_exponent      = DBL_MIN_EXP;
    static const  int   min_exponent10    = DBL_MIN_10_EXP;
    static const  int   max_exponent      = DBL_MAX_EXP;
    static const  int   max_exponent10    = DBL_MAX_10_EXP;
    static const  bool  has_infinity      = true;
    static const  bool  has_quiet_NaN     = true;
    static const  bool  has_signaling_NaN = true;
    static const  float_denorm_style  has_denorm  = denorm_present;
    static const  bool  has_denorm_loss   = true;
    static        T infinity()  __ntl_nothrow      { return __::double_infinity(); }
    static        T quiet_NaN() __ntl_nothrow      { return __::double_quiet_NaN(); }
    static        T signaling_NaN() __ntl_nothrow  { return __::double_signaling_NaN(); }
    static        T denorm_min() __ntl_nothrow     { return __::double_denorm_min(); }
    static const  bool  is_iec559         = true;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = false;
    static const  bool  traps             = true;
    static const  bool  tinyness_before   = true;
    static const  float_round_style round_style = round_to_nearest;
}; //template class numeric_limits<double>

template<> class numeric_limits<long double>
{
    typedef long double T;

  public:
    static const  bool  is_specialized    = true;
    static        T     min() __ntl_nothrow { return LDBL_MIN; }
    static        T     max() __ntl_nothrow { return LDBL_MAX; }
    static        T     lowest() __ntl_nothrow { return LDBL_MIN; }
    static const  int   digits            = LDBL_MANT_DIG;
    static const  int   digits10          = LDBL_DIG;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = false;
    static const  bool  is_exact          = false;
    static const  int   radix             = LDBL_RADIX;
    static        T     epsilon()     __ntl_nothrow { return LDBL_EPSILON; }
    static        T     round_error() __ntl_nothrow { return 0.5; }
    static const  int   min_exponent      = LDBL_MIN_EXP;
    static const  int   min_exponent10    = LDBL_MIN_10_EXP;
    static const  int   max_exponent      = LDBL_MAX_EXP;
    static const  int   max_exponent10    = LDBL_MAX_10_EXP;
    static const  bool  has_infinity      = true;
    static const  bool  has_quiet_NaN     = true;
    static const  bool  has_signaling_NaN = true;
    static const  float_denorm_style  has_denorm  = denorm_present;
    static const  bool  has_denorm_loss   = true;
    static        T infinity()  __ntl_nothrow      { return __::long_double_infinity(); }
    static        T quiet_NaN() __ntl_nothrow      { return __::long_double_quiet_NaN(); }
    static        T signaling_NaN() __ntl_nothrow  { return __::long_double_signaling_NaN(); }
    static        T denorm_min() __ntl_nothrow     { return __::long_double_denorm_min(); }
    static const  bool  is_iec559         = true;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = false;
    static const  bool  traps             = true;
    static const  bool  tinyness_before   = true;
    static const  float_round_style round_style = round_to_nearest;
}; //template class numeric_limits<long double>

#ifdef NTL__CXX_CHARS

template<> class numeric_limits<char16_t>
{
  typedef char16_t T;

public:
  static const  bool  is_specialized    = true;
  static        T     min()         __ntl_nothrow { return 0; }
  static        T     max()         __ntl_nothrow { return ~(T)0; }
  static        T     lowest()      __ntl_nothrow { return 0; }
  static const  int   digits            = sizeof(T) * CHAR_BIT;
  static const  int   digits10          = digits * 301 / 1000;
  static const  bool  is_signed         = false;
  static const  bool  is_integer        = true;
  static const  bool  is_exact          = true;
  static const  int   radix             = 2;
  static        T     epsilon()       __ntl_nothrow { return 0; }
  static        T     round_error()   __ntl_nothrow { return 0; }
  static const  int   min_exponent      = 0;
  static const  int   min_exponent10    = 0;
  static const  int   max_exponent      = 0;
  static const  int   max_exponent10    = 0;
  static const  bool  has_infinity      = false;
  static const  bool  has_quiet_NaN     = false;
  static const  bool  has_signaling_NaN = false;
  static const  float_denorm_style  has_denorm  = denorm_absent;
  static const  bool  has_denorm_loss   = false;
  static        T     infinity()      __ntl_nothrow { return 0; }
  static        T     quiet_NaN()     __ntl_nothrow { return 0; }
  static        T     signaling_NaN() __ntl_nothrow { return 0; }
  static        T     denorm_min()    __ntl_nothrow { return 0; }
  static const  bool  is_iec559         = false;
  static const  bool  is_bounded        = true;
  static const  bool  is_modulo         = true;
  static const  bool  traps             = false;
  static const  bool  tinyness_before   = false;
  static const  float_round_style round_style = round_toward_zero;

  ///\name Constant value extension due to constexpr absent
  static const  T     __min             = 0;
  static const  T     __max             = ~(T)0;
  static const  T     __lowest          = 0;
}; //template class numeric_limits<char16_t>

template<> class numeric_limits<char32_t>
{
  typedef char32_t T;

public:
  static const  bool  is_specialized    = true;
  static        T     min()         __ntl_nothrow { return 0; }
  static        T     max()         __ntl_nothrow { return ~(T)0; }
  static        T     lowest()      __ntl_nothrow { return 0; }
  static const  int   digits            = sizeof(T) * CHAR_BIT;
  static const  int   digits10          = digits * 301 / 1000;
  static const  bool  is_signed         = false;
  static const  bool  is_integer        = true;
  static const  bool  is_exact          = true;
  static const  int   radix             = 2;
  static        T     epsilon()       __ntl_nothrow { return 0; }
  static        T     round_error()   __ntl_nothrow { return 0; }
  static const  int   min_exponent      = 0;
  static const  int   min_exponent10    = 0;
  static const  int   max_exponent      = 0;
  static const  int   max_exponent10    = 0;
  static const  bool  has_infinity      = false;
  static const  bool  has_quiet_NaN     = false;
  static const  bool  has_signaling_NaN = false;
  static const  float_denorm_style  has_denorm  = denorm_absent;
  static const  bool  has_denorm_loss   = false;
  static        T     infinity()      __ntl_nothrow { return 0; }
  static        T     quiet_NaN()     __ntl_nothrow { return 0; }
  static        T     signaling_NaN() __ntl_nothrow { return 0; }
  static        T     denorm_min()    __ntl_nothrow { return 0; }
  static const  bool  is_iec559         = false;
  static const  bool  is_bounded        = true;
  static const  bool  is_modulo         = true;
  static const  bool  traps             = false;
  static const  bool  tinyness_before   = false;
  static const  float_round_style round_style = round_toward_zero;

  ///\name Constant value extension due to constexpr absent
  static const  T     __min             = 0;
  static const  T     __max             = ~(T)0;
  static const  T     __lowest          = 0;
}; //template class numeric_limits<char32_t>

#endif

/**@} lib_support_limits */
/**@} lib_language_support */

}//namespace std

#endif//#ifndef NTL__STLX_LIMITS
