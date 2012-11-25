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
/**\addtogroup  lib_support_limits ***** 18.3 Implementation properties [support.limits]
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

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

/// Class template numeric_limits [18.2.1.1 lib.numeric.limits]
template<class T>
class numeric_limits
{
  public:
    static constexpr const  bool  is_specialized    = false;
    static constexpr        T     min()             __ntl_nothrow;// { return T(false); }
    static constexpr        T     max()             __ntl_nothrow;// { return T(false); }
    static constexpr        T     lowest()          __ntl_nothrow;// { return T(false); }
    static constexpr const  int   digits            = 0;
    static constexpr const  int   digits10          = 0;
    static constexpr const  bool  is_signed         = false;
    static constexpr const  bool  is_integer        = false;
    static constexpr const  bool  is_exact          = false;
    static constexpr const  int   radix             = 0;
    static constexpr        T     epsilon()         __ntl_nothrow;// { return T(false); }
    static constexpr        T     round_error()     __ntl_nothrow;// { return T(false); }
    static constexpr const  int   min_exponent      = 0;
    static constexpr const  int   min_exponent10    = 0;
    static constexpr const  int   max_exponent      = 0;
    static constexpr const  int   max_exponent10    = 0;
    static constexpr const  bool  has_infinity      = false;
    static constexpr const  bool  has_quiet_NaN     = false;
    static constexpr const  bool  has_signaling_NaN = false;
    static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
    static constexpr const  bool  has_denorm_loss   = false;
    static constexpr        T     infinity()        __ntl_nothrow;// { return T(false); }
    static constexpr        T     quiet_NaN()       __ntl_nothrow;// { return T(false); }
    static constexpr        T     signaling_NaN()   __ntl_nothrow;// { return T(false); }
    static constexpr        T     denorm_min()      __ntl_nothrow;// { return T(false); }
    static constexpr const  bool  is_iec559         = false;
    static constexpr const  bool  is_bounded        = false;
    static constexpr const  bool  is_modulo         = false;
    static constexpr const  bool  traps             = false;
    static constexpr const  bool  tinyness_before   = false;
    static constexpr const  float_round_style round_style = round_toward_zero;
}; //template class numeric_limits

template<>
class numeric_limits<bool>
{
    typedef bool  T;

  public:
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min()         __ntl_nothrow { return false; }
    static constexpr        T     max()         __ntl_nothrow { return true; }
    static constexpr        T     lowest()      __ntl_nothrow { return false; }
    static constexpr const  int   digits            = 1;
    static constexpr const  int   digits10          = 0;
    static constexpr const  bool  is_signed         = false;
    static constexpr const  bool  is_integer        = true;
    static constexpr const  bool  is_exact          = true;
    static constexpr const  int   radix             = 2;
    static constexpr        T     epsilon()       __ntl_nothrow { return 0; }
    static constexpr        T     round_error()   __ntl_nothrow { return 0; }
    static constexpr const  int   min_exponent      = 0;
    static constexpr const  int   min_exponent10    = 0;
    static constexpr const  int   max_exponent      = 0;
    static constexpr const  int   max_exponent10    = 0;
    static constexpr const  bool  has_infinity      = false;
    static constexpr const  bool  has_quiet_NaN     = false;
    static constexpr const  bool  has_signaling_NaN = false;
    static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
    static constexpr const  bool  has_denorm_loss   = false;
    static constexpr        T     infinity()      __ntl_nothrow { return 0; }
    static constexpr        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static constexpr        T     signaling_NaN() __ntl_nothrow { return 0; }
    static constexpr        T     denorm_min()    __ntl_nothrow { return 0; }
    static constexpr const  bool  is_iec559         = false;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = true;
    static constexpr const  bool  traps             = false;
    static constexpr const  bool  tinyness_before   = false;
    static constexpr const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static constexpr const  T     __min             = false;
    static constexpr const  T     __max             = true;
    static constexpr const  T     __lowest          = false;
    ///\}
}; //template class numeric_limits<bool>

template<> class numeric_limits<char>
{
    typedef char  T;

  public:
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min()         __ntl_nothrow { return CHAR_MIN; }
    static constexpr        T     max()         __ntl_nothrow { return CHAR_MAX; }
    static constexpr        T     lowest()      __ntl_nothrow { return CHAR_MIN; }
    static constexpr const  int   digits            = CHAR_BIT + (CHAR_MIN>>(CHAR_BIT-1));
    static constexpr const  int   digits10          = digits * 301 / 1000;
    static constexpr const  bool  is_signed         = (CHAR_MIN>>(CHAR_BIT-1)) != 0;
    static constexpr const  bool  is_integer        = true;
    static constexpr const  bool  is_exact          = true;
    static constexpr const  int   radix             = 2;
    static constexpr        T     epsilon()       __ntl_nothrow { return 0; }
    static constexpr        T     round_error()   __ntl_nothrow { return 0; }
    static constexpr const  int   min_exponent      = 0;
    static constexpr const  int   min_exponent10    = 0;
    static constexpr const  int   max_exponent      = 0;
    static constexpr const  int   max_exponent10    = 0;
    static constexpr const  bool  has_infinity      = false;
    static constexpr const  bool  has_quiet_NaN     = false;
    static constexpr const  bool  has_signaling_NaN = false;
    static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
    static constexpr const  bool  has_denorm_loss   = false;
    static constexpr        T     infinity()      __ntl_nothrow { return 0; }
    static constexpr        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static constexpr        T     signaling_NaN() __ntl_nothrow { return 0; }
    static constexpr        T     denorm_min()    __ntl_nothrow { return 0; }
    static constexpr const  bool  is_iec559         = false;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = true;
    static constexpr const  bool  traps             = false;
    static constexpr const  bool  tinyness_before   = false;
    static constexpr const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static constexpr const  T     __min             = CHAR_MIN;
    static constexpr const  T     __max             = CHAR_MAX;
    static constexpr const  T     __lowest          = CHAR_MIN;
    ///\}
}; //template class numeric_limits<char>

template<> class numeric_limits<wchar_t>
{
    typedef wchar_t T;

  public:
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min()         __ntl_nothrow { return WCHAR_MIN; }
    static constexpr        T     max()         __ntl_nothrow { return WCHAR_MAX; }
    static constexpr        T     lowest()      __ntl_nothrow { return WCHAR_MIN; }
    static constexpr const  int   digits            = sizeof(T) * CHAR_BIT - (WCHAR_MIN ? 1 : 0);
    static constexpr const  int   digits10          = digits * 301 / 1000;
    static constexpr const  bool  is_signed         = (WCHAR_MIN ? 1 : 0);
    static constexpr const  bool  is_integer        = true;
    static constexpr const  bool  is_exact          = true;
    static constexpr const  int   radix             = 2;
    static constexpr        T     epsilon()       __ntl_nothrow { return 0; }
    static constexpr        T     round_error()   __ntl_nothrow { return 0; }
    static constexpr const  int   min_exponent      = 0;
    static constexpr const  int   min_exponent10    = 0;
    static constexpr const  int   max_exponent      = 0;
    static constexpr const  int   max_exponent10    = 0;
    static constexpr const  bool  has_infinity      = false;
    static constexpr const  bool  has_quiet_NaN     = false;
    static constexpr const  bool  has_signaling_NaN = false;
    static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
    static constexpr const  bool  has_denorm_loss   = false;
    static constexpr        T     infinity()      __ntl_nothrow { return 0; }
    static constexpr        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static constexpr        T     signaling_NaN() __ntl_nothrow { return 0; }
    static constexpr        T     denorm_min()    __ntl_nothrow { return 0; }
    static constexpr const  bool  is_iec559         = false;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = true;
    static constexpr const  bool  traps             = false;
    static constexpr const  bool  tinyness_before   = false;
    static constexpr const  float_round_style round_style = round_toward_zero;
}; //template class numeric_limits<wchar_t>

template<> class numeric_limits<signed char>
{
    typedef signed char T;

  public:
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min()         __ntl_nothrow { return SCHAR_MIN; }
    static constexpr        T     max()         __ntl_nothrow { return SCHAR_MAX; }
    static constexpr        T     lowest()      __ntl_nothrow { return SCHAR_MIN; }
    static constexpr const  int   digits            = CHAR_BIT - 1;
    static constexpr const  int   digits10          = digits * 301 / 1000;
    static constexpr const  bool  is_signed         = true;
    static constexpr const  bool  is_integer        = true;
    static constexpr const  bool  is_exact          = true;
    static constexpr const  int   radix             = 2;
    static constexpr        T     epsilon()       __ntl_nothrow { return 0; }
    static constexpr        T     round_error()   __ntl_nothrow { return 0; }
    static constexpr const  int   min_exponent      = 0;
    static constexpr const  int   min_exponent10    = 0;
    static constexpr const  int   max_exponent      = 0;
    static constexpr const  int   max_exponent10    = 0;
    static constexpr const  bool  has_infinity      = false;
    static constexpr const  bool  has_quiet_NaN     = false;
    static constexpr const  bool  has_signaling_NaN = false;
    static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
    static constexpr const  bool  has_denorm_loss   = false;
    static constexpr        T     infinity()      __ntl_nothrow { return 0; }
    static constexpr        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static constexpr        T     signaling_NaN() __ntl_nothrow { return 0; }
    static constexpr        T     denorm_min()    __ntl_nothrow { return 0; }
    static constexpr const  bool  is_iec559         = false;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = true;
    static constexpr const  bool  traps             = false;
    static constexpr const  bool  tinyness_before   = false;
    static constexpr const  float_round_style round_style = round_toward_zero;
}; //template class numeric_limits<signed char>

template<> class numeric_limits<short>
{
    typedef short T;

  public:
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min()         __ntl_nothrow { return SHRT_MIN; }
    static constexpr        T     max()         __ntl_nothrow { return SHRT_MAX; }
    static constexpr        T     lowest()      __ntl_nothrow { return SHRT_MIN; }
    static constexpr const  int   digits            = sizeof(T) * CHAR_BIT - 1;
    static constexpr const  int   digits10          = digits * 301 / 1000;
    static constexpr const  bool  is_signed         = true;
    static constexpr const  bool  is_integer        = true;
    static constexpr const  bool  is_exact          = true;
    static constexpr const  int   radix             = 2;
    static constexpr        T     epsilon()       __ntl_nothrow { return 0; }
    static constexpr        T     round_error()   __ntl_nothrow { return 0; }
    static constexpr const  int   min_exponent      = 0;
    static constexpr const  int   min_exponent10    = 0;
    static constexpr const  int   max_exponent      = 0;
    static constexpr const  int   max_exponent10    = 0;
    static constexpr const  bool  has_infinity      = false;
    static constexpr const  bool  has_quiet_NaN     = false;
    static constexpr const  bool  has_signaling_NaN = false;
    static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
    static constexpr const  bool  has_denorm_loss   = false;
    static constexpr        T     infinity()      __ntl_nothrow { return 0; }
    static constexpr        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static constexpr        T     signaling_NaN() __ntl_nothrow { return 0; }
    static constexpr        T     denorm_min()    __ntl_nothrow { return 0; }
    static constexpr const  bool  is_iec559         = false;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = true;
    static constexpr const  bool  traps             = false;
    static constexpr const  bool  tinyness_before   = false;
    static constexpr const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static constexpr const  T     __min             = SHRT_MIN;
    static constexpr const  T     __max             = SHRT_MAX;
    static constexpr const  T     __lowest          = SHRT_MIN;
    ///\}
}; //template class numeric_limits<short>

template<> class numeric_limits<int>
{
    typedef int T;

  public:
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min()         __ntl_nothrow { return INT_MIN; }
    static constexpr        T     max()         __ntl_nothrow { return INT_MAX; }
    static constexpr        T     lowest()      __ntl_nothrow { return INT_MIN; }
    static constexpr const  int   digits            = sizeof(T) * CHAR_BIT - 1;
    static constexpr const  int   digits10          = digits * 301 / 1000;
    static constexpr const  bool  is_signed         = true;
    static constexpr const  bool  is_integer        = true;
    static constexpr const  bool  is_exact          = true;
    static constexpr const  int   radix             = 2;
    static constexpr        T     epsilon()       __ntl_nothrow { return 0; }
    static constexpr        T     round_error()   __ntl_nothrow { return 0; }
    static constexpr const  int   min_exponent      = 0;
    static constexpr const  int   min_exponent10    = 0;
    static constexpr const  int   max_exponent      = 0;
    static constexpr const  int   max_exponent10    = 0;
    static constexpr const  bool  has_infinity      = false;
    static constexpr const  bool  has_quiet_NaN     = false;
    static constexpr const  bool  has_signaling_NaN = false;
    static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
    static constexpr const  bool  has_denorm_loss   = false;
    static constexpr        T     infinity()      __ntl_nothrow { return 0; }
    static constexpr        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static constexpr        T     signaling_NaN() __ntl_nothrow { return 0; }
    static constexpr        T     denorm_min()    __ntl_nothrow { return 0; }
    static constexpr const  bool  is_iec559         = false;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = true;
    static constexpr const  bool  traps             = false;
    static constexpr const  bool  tinyness_before   = false;
    static constexpr const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static constexpr const  T     __min             = INT_MIN;
    static constexpr const  T     __max             = INT_MAX;
    static constexpr const  T     __lowest          = INT_MIN;
    ///\}
}; //template class numeric_limits<int>

template<> class numeric_limits<long>
{
    typedef long T;

  public:
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min()         __ntl_nothrow { return LONG_MIN; }
    static constexpr        T     max()         __ntl_nothrow { return LONG_MAX; }
    static constexpr        T     lowest()      __ntl_nothrow { return LONG_MIN; }
    static constexpr const  int   digits            = sizeof(T) * CHAR_BIT - 1;
    static constexpr const  int   digits10          = digits * 301 / 1000;
    static constexpr const  bool  is_signed         = true;
    static constexpr const  bool  is_integer        = true;
    static constexpr const  bool  is_exact          = true;
    static constexpr const  int   radix             = 2;
    static constexpr        T     epsilon()       __ntl_nothrow { return 0; }
    static constexpr        T     round_error()   __ntl_nothrow { return 0; }
    static constexpr const  int   min_exponent      = 0;
    static constexpr const  int   min_exponent10    = 0;
    static constexpr const  int   max_exponent      = 0;
    static constexpr const  int   max_exponent10    = 0;
    static constexpr const  bool  has_infinity      = false;
    static constexpr const  bool  has_quiet_NaN     = false;
    static constexpr const  bool  has_signaling_NaN = false;
    static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
    static constexpr const  bool  has_denorm_loss   = false;
    static constexpr        T     infinity()      __ntl_nothrow { return 0; }
    static constexpr        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static constexpr        T     signaling_NaN() __ntl_nothrow { return 0; }
    static constexpr        T     denorm_min()    __ntl_nothrow { return 0; }
    static constexpr const  bool  is_iec559         = false;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = true;
    static constexpr const  bool  traps             = false;
    static constexpr const  bool  tinyness_before   = false;
    static constexpr const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static constexpr const  T     __min             = LONG_MIN;
    static constexpr const  T     __max             = LONG_MAX;
    static constexpr const  T     __lowest          = LONG_MIN;
    ///\}
}; //template class numeric_limits<long>

template<> class numeric_limits<long long>
{
    typedef long long T;

  public:
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min()         __ntl_nothrow { return LLONG_MIN; }
    static constexpr        T     max()         __ntl_nothrow { return LLONG_MAX; }
    static constexpr        T     lowest()      __ntl_nothrow { return LLONG_MIN; }
    static constexpr const  int   digits            = sizeof(T) * CHAR_BIT - 1;
    static constexpr const  int   digits10          = digits * 301 / 1000;
    static constexpr const  bool  is_signed         = true;
    static constexpr const  bool  is_integer        = true;
    static constexpr const  bool  is_exact          = true;
    static constexpr const  int   radix             = 2;
    static constexpr        T     epsilon()       __ntl_nothrow { return 0; }
    static constexpr        T     round_error()   __ntl_nothrow { return 0; }
    static constexpr const  int   min_exponent      = 0;
    static constexpr const  int   min_exponent10    = 0;
    static constexpr const  int   max_exponent      = 0;
    static constexpr const  int   max_exponent10    = 0;
    static constexpr const  bool  has_infinity      = false;
    static constexpr const  bool  has_quiet_NaN     = false;
    static constexpr const  bool  has_signaling_NaN = false;
    static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
    static constexpr const  bool  has_denorm_loss   = false;
    static constexpr        T     infinity()      __ntl_nothrow { return 0; }
    static constexpr        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static constexpr        T     signaling_NaN() __ntl_nothrow { return 0; }
    static constexpr        T     denorm_min()    __ntl_nothrow { return 0; }
    static constexpr const  bool  is_iec559         = false;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = true;
    static constexpr const  bool  traps             = false;
    static constexpr const  bool  tinyness_before   = false;
    static constexpr const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static constexpr const  T     __min             = LLONG_MIN;
    static constexpr const  T     __max             = LLONG_MAX;
    static constexpr const  T     __lowest          = LLONG_MIN;
    ///\}
}; //template class numeric_limits<long long>

template<> class numeric_limits<unsigned char>
{
    typedef unsigned char T;

  public:
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min()         __ntl_nothrow { return 0; }
    static constexpr        T     max()         __ntl_nothrow { return UCHAR_MAX; }
    static constexpr        T     lowest()      __ntl_nothrow { return 0; }
    static constexpr const  int   digits            = CHAR_BIT - 1;
    static constexpr const  int   digits10          = digits * 301 / 1000;
    static constexpr const  bool  is_signed         = false;
    static constexpr const  bool  is_integer        = true;
    static constexpr const  bool  is_exact          = true;
    static constexpr const  int   radix             = 2;
    static constexpr        T     epsilon()       __ntl_nothrow { return 0; }
    static constexpr        T     round_error()   __ntl_nothrow { return 0; }
    static constexpr const  int   min_exponent      = 0;
    static constexpr const  int   min_exponent10    = 0;
    static constexpr const  int   max_exponent      = 0;
    static constexpr const  int   max_exponent10    = 0;
    static constexpr const  bool  has_infinity      = false;
    static constexpr const  bool  has_quiet_NaN     = false;
    static constexpr const  bool  has_signaling_NaN = false;
    static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
    static constexpr const  bool  has_denorm_loss   = false;
    static constexpr        T     infinity()      __ntl_nothrow { return 0; }
    static constexpr        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static constexpr        T     signaling_NaN() __ntl_nothrow { return 0; }
    static constexpr        T     denorm_min()    __ntl_nothrow { return 0; }
    static constexpr const  bool  is_iec559         = false;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = true;
    static constexpr const  bool  traps             = false;
    static constexpr const  bool  tinyness_before   = false;
    static constexpr const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static constexpr const  T     __min             = 0;
    static constexpr const  T     __max             = UCHAR_MAX;
    static constexpr const  T     __lowest          = 0;
    ///\}
}; //template class numeric_limits<unsigned char>

template<> class numeric_limits<unsigned short>
{
    typedef unsigned short T;

  public:
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min()         __ntl_nothrow { return 0; }
    static constexpr        T     max()         __ntl_nothrow { return USHRT_MAX; }
    static constexpr        T     lowest()      __ntl_nothrow { return 0; }
    static constexpr const  int   digits            = sizeof(T) * CHAR_BIT;
    static constexpr const  int   digits10          = digits * 301 / 1000;
    static constexpr const  bool  is_signed         = false;
    static constexpr const  bool  is_integer        = true;
    static constexpr const  bool  is_exact          = true;
    static constexpr const  int   radix             = 2;
    static constexpr        T     epsilon()       __ntl_nothrow { return 0; }
    static constexpr        T     round_error()   __ntl_nothrow { return 0; }
    static constexpr const  int   min_exponent      = 0;
    static constexpr const  int   min_exponent10    = 0;
    static constexpr const  int   max_exponent      = 0;
    static constexpr const  int   max_exponent10    = 0;
    static constexpr const  bool  has_infinity      = false;
    static constexpr const  bool  has_quiet_NaN     = false;
    static constexpr const  bool  has_signaling_NaN = false;
    static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
    static constexpr const  bool  has_denorm_loss   = false;
    static constexpr        T     infinity()      __ntl_nothrow { return 0; }
    static constexpr        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static constexpr        T     signaling_NaN() __ntl_nothrow { return 0; }
    static constexpr        T     denorm_min()    __ntl_nothrow { return 0; }
    static constexpr const  bool  is_iec559         = false;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = true;
    static constexpr const  bool  traps             = false;
    static constexpr const  bool  tinyness_before   = false;
    static constexpr const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static constexpr const  T     __min             = 0;
    static constexpr const  T     __max             = USHRT_MAX;
    static constexpr const  T     __lowest          = 0;
    ///\}
}; //template class numeric_limits<unsigned shrt>

template<> class numeric_limits<unsigned int>
{
    typedef unsigned int T;

  public:
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min()         __ntl_nothrow { return 0; }
    static constexpr        T     max()         __ntl_nothrow { return UINT_MAX; }
    static constexpr        T     lowest()      __ntl_nothrow { return 0; }
    static constexpr const  int   digits            = sizeof(T) * CHAR_BIT;
    static constexpr const  int   digits10          = digits * 301 / 1000;
    static constexpr const  bool  is_signed         = false;
    static constexpr const  bool  is_integer        = true;
    static constexpr const  bool  is_exact          = true;
    static constexpr const  int   radix             = 2;
    static constexpr        T     epsilon()       __ntl_nothrow { return 0; }
    static constexpr        T     round_error()   __ntl_nothrow { return 0; }
    static constexpr const  int   min_exponent      = 0;
    static constexpr const  int   min_exponent10    = 0;
    static constexpr const  int   max_exponent      = 0;
    static constexpr const  int   max_exponent10    = 0;
    static constexpr const  bool  has_infinity      = false;
    static constexpr const  bool  has_quiet_NaN     = false;
    static constexpr const  bool  has_signaling_NaN = false;
    static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
    static constexpr const  bool  has_denorm_loss   = false;
    static constexpr        T     infinity()      __ntl_nothrow { return 0; }
    static constexpr        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static constexpr        T     signaling_NaN() __ntl_nothrow { return 0; }
    static constexpr        T     denorm_min()    __ntl_nothrow { return 0; }
    static constexpr const  bool  is_iec559         = false;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = true;
    static constexpr const  bool  traps             = false;
    static constexpr const  bool  tinyness_before   = false;
    static constexpr const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static constexpr const  T     __min             = 0;
    static constexpr const  T     __max             = UINT_MAX;
    static constexpr const  T     __lowest          = 0;
    ///\}
}; //template class numeric_limits<unsigned int>

template<> class numeric_limits<unsigned long>
{
    typedef unsigned long T;

  public:
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min()         __ntl_nothrow { return 0; }
    static constexpr        T     max()         __ntl_nothrow { return ULONG_MAX; }
    static constexpr        T     lowest()      __ntl_nothrow { return 0; }
    static constexpr const  int   digits            = sizeof(T) * CHAR_BIT;
    static constexpr const  int   digits10          = digits * 301 / 1000;
    static constexpr const  bool  is_signed         = false;
    static constexpr const  bool  is_integer        = true;
    static constexpr const  bool  is_exact          = true;
    static constexpr const  int   radix             = 2;
    static constexpr        T     epsilon()       __ntl_nothrow { return 0; }
    static constexpr        T     round_error()   __ntl_nothrow { return 0; }
    static constexpr const  int   min_exponent      = 0;
    static constexpr const  int   min_exponent10    = 0;
    static constexpr const  int   max_exponent      = 0;
    static constexpr const  int   max_exponent10    = 0;
    static constexpr const  bool  has_infinity      = false;
    static constexpr const  bool  has_quiet_NaN     = false;
    static constexpr const  bool  has_signaling_NaN = false;
    static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
    static constexpr const  bool  has_denorm_loss   = false;
    static constexpr        T     infinity()      __ntl_nothrow { return 0; }
    static constexpr        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static constexpr        T     signaling_NaN() __ntl_nothrow { return 0; }
    static constexpr        T     denorm_min()    __ntl_nothrow { return 0; }
    static constexpr const  bool  is_iec559         = false;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = true;
    static constexpr const  bool  traps             = false;
    static constexpr const  bool  tinyness_before   = false;
    static constexpr const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static constexpr const  T     __min             = 0;
    static constexpr const  T     __max             = ULONG_MAX;
    static constexpr const  T     __lowest          = 0;
    ///\}
}; //template class numeric_limits<unsigned long>

template<> class numeric_limits<unsigned long long>
{
    typedef unsigned long long T;

  public:
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min()         __ntl_nothrow { return 0; }
    static constexpr        T     max()         __ntl_nothrow { return ULLONG_MAX; }
    static constexpr        T     lowest()      __ntl_nothrow { return 0; }
    static constexpr const  int   digits            = sizeof(T) * CHAR_BIT;
    static constexpr const  int   digits10          = digits * 301 / 1000;
    static constexpr const  bool  is_signed         = false;
    static constexpr const  bool  is_integer        = true;
    static constexpr const  bool  is_exact          = true;
    static constexpr const  int   radix             = 2;
    static constexpr        T     epsilon()       __ntl_nothrow { return 0; }
    static constexpr        T     round_error()   __ntl_nothrow { return 0; }
    static constexpr const  int   min_exponent      = 0;
    static constexpr const  int   min_exponent10    = 0;
    static constexpr const  int   max_exponent      = 0;
    static constexpr const  int   max_exponent10    = 0;
    static constexpr const  bool  has_infinity      = false;
    static constexpr const  bool  has_quiet_NaN     = false;
    static constexpr const  bool  has_signaling_NaN = false;
    static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
    static constexpr const  bool  has_denorm_loss   = false;
    static constexpr        T     infinity()      __ntl_nothrow { return 0; }
    static constexpr        T     quiet_NaN()     __ntl_nothrow { return 0; }
    static constexpr        T     signaling_NaN() __ntl_nothrow { return 0; }
    static constexpr        T     denorm_min()    __ntl_nothrow { return 0; }
    static constexpr const  bool  is_iec559         = false;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = true;
    static constexpr const  bool  traps             = false;
    static constexpr const  bool  tinyness_before   = false;
    static constexpr const  float_round_style round_style = round_toward_zero;

    ///\name Constant value extension due to constexpr absent
    static constexpr const  T     __min             = 0;
    static constexpr const  T     __max             = ULLONG_MAX;
    static constexpr const  T     __lowest          = 0;
    ///\}
}; //template class numeric_limits<unsigned long long>

namespace __
{
#if defined(__GNUC__) || defined(__clang__)
  inline constexpr float float_infinity()     { return __builtin_huge_valf(); }
  inline constexpr float float_quiet_NaN()    { return __builtin_nanf("");    }
  inline constexpr float float_signaling_NaN(){ return __builtin_nansf("");   }
  inline constexpr float float_denorm_min()   { return __FLT_DENORM_MIN__;    }

  inline constexpr double double_infinity()     { return __builtin_huge_valf(); }
  inline constexpr double double_quiet_NaN()    { return __builtin_nanf("");    }
  inline constexpr double double_signaling_NaN(){ return __builtin_nansf("");   }
  inline constexpr double double_denorm_min()   { return __DBL_DENORM_MIN__;    }

  inline constexpr long double long_double_infinity()     { return __builtin_huge_valf(); }
  inline constexpr long double long_double_quiet_NaN()    { return __builtin_nanf("");    }
  inline constexpr long double long_double_signaling_NaN(){ return __builtin_nansf("");   }
  inline constexpr long double long_double_denorm_min()   { return __LDBL_DENORM_MIN__;   }
#else
  union fvalue { uint8_t raw[sizeof(float)]; float value; };
  static_assert(sizeof(fvalue) == 4, "unsupported float size");
  inline constexpr float float_infinity()
  {
    static constexpr const fvalue infinity      = { 0x00, 0x00, 0x80, 0x7F };
    return infinity.value;
  }
  inline constexpr float float_quiet_NaN()
  {
    static constexpr const fvalue quiet_NaN     = { 0x00, 0x00, 0xC0, 0x7F };
    return quiet_NaN.value;
  }
  inline constexpr float float_signaling_NaN()
  {
    static constexpr const fvalue signaling_NaN = { 0x01, 0x00, 0x80, 0x7F };
    return signaling_NaN.value;
  }
  inline constexpr float float_denorm_min()
  {
    static constexpr const fvalue denorm_min    = { 0x01, 0x00, 0x00, 0x00 };
    return denorm_min.value;
  }

  union dvalue { uint8_t raw[sizeof(double)]; double  value;};
  STATIC_ASSERT(sizeof(dvalue) == 8);
  inline constexpr double double_infinity()
  {
    static constexpr const dvalue infinity =
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x7F };
    return infinity.value;
  }
  inline constexpr double double_quiet_NaN()
  {
    static constexpr const dvalue quiet_NaN =
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x7F };
    return quiet_NaN.value;
  }
  inline constexpr double double_signaling_NaN()
  {
    static constexpr const dvalue signaling_NaN =
    { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x7F };
    return signaling_NaN.value;
  }
  inline constexpr double double_denorm_min()
  {
    static constexpr const dvalue denorm_min =
    { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    return denorm_min.value;
  }
#if DBL_DIG == LDBL_DIG
  union ldvalue { uint8_t raw[sizeof(double)]; long double value;};
  STATIC_ASSERT(sizeof(ldvalue) == 8);
  inline constexpr long double long_double_infinity()
  {
    static constexpr const ldvalue infinity =
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x7F };
    return infinity.value;
  }
  inline constexpr long double long_double_quiet_NaN()
  {
    static constexpr const ldvalue quiet_NaN =
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x7F };
    return quiet_NaN.value;
  }
  inline constexpr long double long_double_signaling_NaN()
  {
    static constexpr const ldvalue signaling_NaN =
    { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x7F };
    return signaling_NaN.value;
  }
  inline constexpr long double long_double_denorm_min()
  {
    static constexpr const ldvalue denorm_min =
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
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min() __ntl_nothrow { return FLT_MIN; }
    static constexpr        T     max() __ntl_nothrow { return FLT_MAX; }
    static constexpr        T     lowest()      __ntl_nothrow { return FLT_MIN; }
    static constexpr const  int   digits            = FLT_MANT_DIG;
    static constexpr const  int   digits10          = FLT_DIG;
    static constexpr const  bool  is_signed         = true;
    static constexpr const  bool  is_integer        = false;
    static constexpr const  bool  is_exact          = false;
    static constexpr const  int   radix             = FLT_RADIX;
    static constexpr        T     epsilon()     __ntl_nothrow { return FLT_EPSILON; }
    static constexpr        T     round_error() __ntl_nothrow { return 0.5; }
    static constexpr const  int   min_exponent      = FLT_MIN_EXP;
    static constexpr const  int   min_exponent10    = FLT_MIN_10_EXP;
    static constexpr const  int   max_exponent      = FLT_MAX_EXP;
    static constexpr const  int   max_exponent10    = FLT_MAX_10_EXP;
    static constexpr const  bool  has_infinity      = true;
    static constexpr const  bool  has_quiet_NaN     = true;
    static constexpr const  bool  has_signaling_NaN = true;
    static constexpr const  float_denorm_style  has_denorm  = denorm_present;
    static constexpr const  bool  has_denorm_loss   = true;
    static constexpr        T infinity()  __ntl_nothrow      { return __::float_infinity(); }
    static constexpr        T quiet_NaN() __ntl_nothrow      { return __::float_quiet_NaN(); }
    static constexpr        T signaling_NaN() __ntl_nothrow  { return __::float_signaling_NaN(); }
    static constexpr        T denorm_min() __ntl_nothrow     { return __::float_denorm_min(); }
    static constexpr const  bool  is_iec559         = true;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = false;
    static constexpr const  bool  traps             = true;
    static constexpr const  bool  tinyness_before   = true;
    static constexpr const  float_round_style round_style = round_to_nearest;
    ///\}
}; //template class numeric_limits<float>

template<> class numeric_limits<double>
{
    typedef double T;

  public:
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min() __ntl_nothrow { return DBL_MIN; }
    static constexpr        T     max() __ntl_nothrow { return DBL_MAX; }
    static constexpr        T     lowest()      __ntl_nothrow { return DBL_MIN; }
    static constexpr const  int   digits            = DBL_MANT_DIG;
    static constexpr const  int   digits10          = DBL_DIG;
    static constexpr const  bool  is_signed         = true;
    static constexpr const  bool  is_integer        = false;
    static constexpr const  bool  is_exact          = false;
    static constexpr const  int   radix             = DBL_RADIX;
    static constexpr        T     epsilon()     __ntl_nothrow { return DBL_EPSILON; }
    static constexpr        T     round_error() __ntl_nothrow { return 0.5; }
    static constexpr const  int   min_exponent      = DBL_MIN_EXP;
    static constexpr const  int   min_exponent10    = DBL_MIN_10_EXP;
    static constexpr const  int   max_exponent      = DBL_MAX_EXP;
    static constexpr const  int   max_exponent10    = DBL_MAX_10_EXP;
    static constexpr const  bool  has_infinity      = true;
    static constexpr const  bool  has_quiet_NaN     = true;
    static constexpr const  bool  has_signaling_NaN = true;
    static constexpr const  float_denorm_style  has_denorm  = denorm_present;
    static constexpr const  bool  has_denorm_loss   = true;
    static constexpr        T infinity()  __ntl_nothrow      { return __::double_infinity(); }
    static constexpr        T quiet_NaN() __ntl_nothrow      { return __::double_quiet_NaN(); }
    static constexpr        T signaling_NaN() __ntl_nothrow  { return __::double_signaling_NaN(); }
    static constexpr        T denorm_min() __ntl_nothrow     { return __::double_denorm_min(); }
    static constexpr const  bool  is_iec559         = true;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = false;
    static constexpr const  bool  traps             = true;
    static constexpr const  bool  tinyness_before   = true;
    static constexpr const  float_round_style round_style = round_to_nearest;
}; //template class numeric_limits<double>

template<> class numeric_limits<long double>
{
    typedef long double T;

  public:
    static constexpr const  bool  is_specialized    = true;
    static constexpr        T     min() __ntl_nothrow { return LDBL_MIN; }
    static constexpr        T     max() __ntl_nothrow { return LDBL_MAX; }
    static constexpr        T     lowest() __ntl_nothrow { return LDBL_MIN; }
    static constexpr const  int   digits            = LDBL_MANT_DIG;
    static constexpr const  int   digits10          = LDBL_DIG;
    static constexpr const  bool  is_signed         = true;
    static constexpr const  bool  is_integer        = false;
    static constexpr const  bool  is_exact          = false;
    static constexpr const  int   radix             = LDBL_RADIX;
    static constexpr        T     epsilon()     __ntl_nothrow { return LDBL_EPSILON; }
    static constexpr        T     round_error() __ntl_nothrow { return 0.5; }
    static constexpr const  int   min_exponent      = LDBL_MIN_EXP;
    static constexpr const  int   min_exponent10    = LDBL_MIN_10_EXP;
    static constexpr const  int   max_exponent      = LDBL_MAX_EXP;
    static constexpr const  int   max_exponent10    = LDBL_MAX_10_EXP;
    static constexpr const  bool  has_infinity      = true;
    static constexpr const  bool  has_quiet_NaN     = true;
    static constexpr const  bool  has_signaling_NaN = true;
    static constexpr const  float_denorm_style  has_denorm  = denorm_present;
    static constexpr const  bool  has_denorm_loss   = true;
    static constexpr        T infinity()  __ntl_nothrow      { return __::long_double_infinity(); }
    static constexpr        T quiet_NaN() __ntl_nothrow      { return __::long_double_quiet_NaN(); }
    static constexpr        T signaling_NaN() __ntl_nothrow  { return __::long_double_signaling_NaN(); }
    static constexpr        T denorm_min() __ntl_nothrow     { return __::long_double_denorm_min(); }
    static constexpr const  bool  is_iec559         = true;
    static constexpr const  bool  is_bounded        = true;
    static constexpr const  bool  is_modulo         = false;
    static constexpr const  bool  traps             = true;
    static constexpr const  bool  tinyness_before   = true;
    static constexpr const  float_round_style round_style = round_to_nearest;
}; //template class numeric_limits<long double>

#if defined(NTL_CXX_CHARS_TYPES)

template<> class numeric_limits<char16_t>
{
  typedef char16_t T;

public:
  static constexpr const  bool  is_specialized    = true;
  static constexpr        T     min()         __ntl_nothrow { return 0; }
  static constexpr        T     max()         __ntl_nothrow { return ~(T)0; }
  static constexpr        T     lowest()      __ntl_nothrow { return 0; }
  static constexpr const  int   digits            = sizeof(T) * CHAR_BIT;
  static constexpr const  int   digits10          = digits * 301 / 1000;
  static constexpr const  bool  is_signed         = false;
  static constexpr const  bool  is_integer        = true;
  static constexpr const  bool  is_exact          = true;
  static constexpr const  int   radix             = 2;
  static constexpr        T     epsilon()       __ntl_nothrow { return 0; }
  static constexpr        T     round_error()   __ntl_nothrow { return 0; }
  static constexpr const  int   min_exponent      = 0;
  static constexpr const  int   min_exponent10    = 0;
  static constexpr const  int   max_exponent      = 0;
  static constexpr const  int   max_exponent10    = 0;
  static constexpr const  bool  has_infinity      = false;
  static constexpr const  bool  has_quiet_NaN     = false;
  static constexpr const  bool  has_signaling_NaN = false;
  static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
  static constexpr const  bool  has_denorm_loss   = false;
  static constexpr        T     infinity()      __ntl_nothrow { return 0; }
  static constexpr        T     quiet_NaN()     __ntl_nothrow { return 0; }
  static constexpr        T     signaling_NaN() __ntl_nothrow { return 0; }
  static constexpr        T     denorm_min()    __ntl_nothrow { return 0; }
  static constexpr const  bool  is_iec559         = false;
  static constexpr const  bool  is_bounded        = true;
  static constexpr const  bool  is_modulo         = true;
  static constexpr const  bool  traps             = false;
  static constexpr const  bool  tinyness_before   = false;
  static constexpr const  float_round_style round_style = round_toward_zero;

  ///\name Constant value extension due to constexpr absent
  static constexpr const  T     __min             = 0;
  static constexpr const  T     __max             = ~(T)0;
  static constexpr const  T     __lowest          = 0;
  ///\}
}; //template class numeric_limits<char16_t>

template<> class numeric_limits<char32_t>
{
  typedef char32_t T;

public:
  static constexpr const  bool  is_specialized    = true;
  static constexpr        T     min()         __ntl_nothrow { return 0; }
  static constexpr        T     max()         __ntl_nothrow { return ~(T)0; }
  static constexpr        T     lowest()      __ntl_nothrow { return 0; }
  static constexpr const  int   digits            = sizeof(T) * CHAR_BIT;
  static constexpr const  int   digits10          = digits * 301 / 1000;
  static constexpr const  bool  is_signed         = false;
  static constexpr const  bool  is_integer        = true;
  static constexpr const  bool  is_exact          = true;
  static constexpr const  int   radix             = 2;
  static constexpr        T     epsilon()       __ntl_nothrow { return 0; }
  static constexpr        T     round_error()   __ntl_nothrow { return 0; }
  static constexpr const  int   min_exponent      = 0;
  static constexpr const  int   min_exponent10    = 0;
  static constexpr const  int   max_exponent      = 0;
  static constexpr const  int   max_exponent10    = 0;
  static constexpr const  bool  has_infinity      = false;
  static constexpr const  bool  has_quiet_NaN     = false;
  static constexpr const  bool  has_signaling_NaN = false;
  static constexpr const  float_denorm_style  has_denorm  = denorm_absent;
  static constexpr const  bool  has_denorm_loss   = false;
  static constexpr        T     infinity()      __ntl_nothrow { return 0; }
  static constexpr        T     quiet_NaN()     __ntl_nothrow { return 0; }
  static constexpr        T     signaling_NaN() __ntl_nothrow { return 0; }
  static constexpr        T     denorm_min()    __ntl_nothrow { return 0; }
  static constexpr const  bool  is_iec559         = false;
  static constexpr const  bool  is_bounded        = true;
  static constexpr const  bool  is_modulo         = true;
  static constexpr const  bool  traps             = false;
  static constexpr const  bool  tinyness_before   = false;
  static constexpr const  float_round_style round_style = round_toward_zero;

  ///\name Constant value extension due to constexpr absent
  static constexpr const  T     __min             = 0;
  static constexpr const  T     __max             = ~(T)0;
  static constexpr const  T     __lowest          = 0;
  ///\}
}; //template class numeric_limits<char32_t>

#endif

/**@} lib_support_limits */
/**@} lib_language_support */

}//namespace std

#endif//#ifndef NTL__STLX_LIMITS
