/**\file*********************************************************************
 *                                                                     \brief
 *  Numeric limits [18.2.1 lib.limits]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_LIMITS
#define NTL__STLX_LIMITS

#include "climits.hxx"
#include "cfloat.hxx"

namespace std {

/**\addtogroup  lib_language_support *** Language support library [18] ******
 *@{*/
/**\addtogroup  lib_support_limits ***** Implementation properties [18.2] ***
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
  denorm_present        = 1,
};

/// Class template numeric_limits [18.2.1.1 lib.numeric.limits]
template<class T>
class numeric_limits
{
  public:
    static const  bool  is_specialized    = false;
    static        T     (min)()         throw();
    static        T     (max)()         throw();
    static const  int   digits            = 0;
    static const  int   digits10          = 0;
    static const  bool  is_signed         = false;
    static const  bool  is_integer        = false;
    static const  bool  is_exact          = false;
    static const  int   radix             = 0;
    static        T     epsilon()       throw();
    static        T     round_error()   throw();
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      throw();
    static        T     quiet_NaN()     throw();
    static        T     signaling_NaN() throw();
    static        T     denorm_min()    throw();
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = false;
    static const  bool  is_modulo         = false;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
};//template class numeric_limits

template<>
class numeric_limits<bool>
{
    typedef bool  T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)()         throw() { return false; }
    static        T     (max)()         throw() { return true; }
    static const  int   digits            = 1;
    static const  int   digits10          = 0;
    static const  bool  is_signed         = false;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       throw() { return 0; }
    static        T     round_error()   throw() { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      throw() { return 0; }
    static        T     quiet_NaN()     throw() { return 0; }
    static        T     signaling_NaN() throw() { return 0; }
    static        T     denorm_min()    throw() { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
};//template class numeric_limits<bool> 

template<> class numeric_limits<char>
{
    typedef char  T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)()         throw() { return CHAR_MIN; }
    static        T     (max)()         throw() { return CHAR_MAX; }
    static const  int   digits            = CHAR_BIT + (CHAR_MIN>>(CHAR_BIT-1));
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = (CHAR_MIN>>(CHAR_BIT-1)) != 0;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       throw() { return 0; }
    static        T     round_error()   throw() { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      throw() { return 0; }
    static        T     quiet_NaN()     throw() { return 0; }
    static        T     signaling_NaN() throw() { return 0; }
    static        T     denorm_min()    throw() { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
};//template class numeric_limits<char> 

template<> class numeric_limits<wchar_t>
{
    typedef wchar_t T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)()         throw() { return WCHAR_MIN; }
    static        T     (max)()         throw() { return WCHAR_MAX; }
    static const  int   digits            = sizeof(T) * CHAR_BIT - (WCHAR_MIN ? 1 : 0);
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = (WCHAR_MIN ? 1 : 0);
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       throw() { return 0; }
    static        T     round_error()   throw() { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      throw() { return 0; }
    static        T     quiet_NaN()     throw() { return 0; }
    static        T     signaling_NaN() throw() { return 0; }
    static        T     denorm_min()    throw() { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
};//template class numeric_limits<wchar_t> 

template<> class numeric_limits<signed char>
{
    typedef signed char T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)()         throw() { return SCHAR_MIN; }
    static        T     (max)()         throw() { return SCHAR_MAX; }
    static const  int   digits            = CHAR_BIT - 1;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       throw() { return 0; }
    static        T     round_error()   throw() { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      throw() { return 0; }
    static        T     quiet_NaN()     throw() { return 0; }
    static        T     signaling_NaN() throw() { return 0; }
    static        T     denorm_min()    throw() { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
};//template class numeric_limits<signed char> 

template<> class numeric_limits<short>
{
    typedef short T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)()         throw() { return SHRT_MIN; }
    static        T     (max)()         throw() { return SHRT_MAX; }
    static const  int   digits            = sizeof(T) * CHAR_BIT - 1;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       throw() { return 0; }
    static        T     round_error()   throw() { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      throw() { return 0; }
    static        T     quiet_NaN()     throw() { return 0; }
    static        T     signaling_NaN() throw() { return 0; }
    static        T     denorm_min()    throw() { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
};//template class numeric_limits<short> 

template<> class numeric_limits<int>
{
    typedef int T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)()         throw() { return INT_MIN; }
    static        T     (max)()         throw() { return INT_MAX; }
    static const  int   digits            = sizeof(T) * CHAR_BIT - 1;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       throw() { return 0; }
    static        T     round_error()   throw() { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      throw() { return 0; }
    static        T     quiet_NaN()     throw() { return 0; }
    static        T     signaling_NaN() throw() { return 0; }
    static        T     denorm_min()    throw() { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
};//template class numeric_limits<int> 

template<> class numeric_limits<long>
{
    typedef long T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)()         throw() { return LONG_MIN; }
    static        T     (max)()         throw() { return LONG_MAX; }
    static const  int   digits            = sizeof(T) * CHAR_BIT - 1;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       throw() { return 0; }
    static        T     round_error()   throw() { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      throw() { return 0; }
    static        T     quiet_NaN()     throw() { return 0; }
    static        T     signaling_NaN() throw() { return 0; }
    static        T     denorm_min()    throw() { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
};//template class numeric_limits<long> 

template<> class numeric_limits<long long>
{
    typedef long long T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)()         throw() { return LLONG_MIN; }
    static        T     (max)()         throw() { return LLONG_MAX; }
    static const  int   digits            = sizeof(T) * CHAR_BIT - 1;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       throw() { return 0; }
    static        T     round_error()   throw() { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      throw() { return 0; }
    static        T     quiet_NaN()     throw() { return 0; }
    static        T     signaling_NaN() throw() { return 0; }
    static        T     denorm_min()    throw() { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
};//template class numeric_limits<long long> 

template<> class numeric_limits<unsigned char>
{
    typedef unsigned char T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)()         throw() { return 0; }
    static        T     (max)()         throw() { return UCHAR_MAX; }
    static const  int   digits            = CHAR_BIT - 1;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = false;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       throw() { return 0; }
    static        T     round_error()   throw() { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      throw() { return 0; }
    static        T     quiet_NaN()     throw() { return 0; }
    static        T     signaling_NaN() throw() { return 0; }
    static        T     denorm_min()    throw() { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
};//template class numeric_limits<unsigned char> 

template<> class numeric_limits<unsigned short>
{
    typedef unsigned short T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)()         throw() { return 0; }
    static        T     (max)()         throw() { return USHRT_MAX; }
    static const  int   digits            = sizeof(T) * CHAR_BIT;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = false;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       throw() { return 0; }
    static        T     round_error()   throw() { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      throw() { return 0; }
    static        T     quiet_NaN()     throw() { return 0; }
    static        T     signaling_NaN() throw() { return 0; }
    static        T     denorm_min()    throw() { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
};//template class numeric_limits<unsigned shrt> 

template<> class numeric_limits<unsigned int>
{
    typedef unsigned int T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)()         throw() { return 0; }
    static        T     (max)()         throw() { return UINT_MAX; }
    static const  int   digits            = sizeof(T) * CHAR_BIT;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = false;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       throw() { return 0; }
    static        T     round_error()   throw() { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      throw() { return 0; }
    static        T     quiet_NaN()     throw() { return 0; }
    static        T     signaling_NaN() throw() { return 0; }
    static        T     denorm_min()    throw() { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
};//template class numeric_limits<unsigned int> 

template<> class numeric_limits<unsigned long>
{
    typedef unsigned long T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)()         throw() { return 0; }
    static        T     (max)()         throw() { return ULONG_MAX; }
    static const  int   digits            = sizeof(T) * CHAR_BIT;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = false;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       throw() { return 0; }
    static        T     round_error()   throw() { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      throw() { return 0; }
    static        T     quiet_NaN()     throw() { return 0; }
    static        T     signaling_NaN() throw() { return 0; }
    static        T     denorm_min()    throw() { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
};//template class numeric_limits<unsigned long> 

template<> class numeric_limits<unsigned long long>
{
    typedef unsigned long long T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)()         throw() { return 0; }
    static        T     (max)()         throw() { return ULLONG_MAX; }
    static const  int   digits            = sizeof(T) * CHAR_BIT;
    static const  int   digits10          = digits * 301 / 1000;
    static const  bool  is_signed         = false;
    static const  bool  is_integer        = true;
    static const  bool  is_exact          = true;
    static const  int   radix             = 2;
    static        T     epsilon()       throw() { return 0; }
    static        T     round_error()   throw() { return 0; }
    static const  int   min_exponent      = 0;
    static const  int   min_exponent10    = 0;
    static const  int   max_exponent      = 0;
    static const  int   max_exponent10    = 0;
    static const  bool  has_infinity      = false;
    static const  bool  has_quiet_NaN     = false;
    static const  bool  has_signaling_NaN = false;
    static const  float_denorm_style  has_denorm  = denorm_absent;
    static const  bool  has_denorm_loss   = false;
    static        T     infinity()      throw() { return 0; }
    static        T     quiet_NaN()     throw() { return 0; }
    static        T     signaling_NaN() throw() { return 0; }
    static        T     denorm_min()    throw() { return 0; }
    static const  bool  is_iec559         = false;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = true;
    static const  bool  traps             = false;
    static const  bool  tinyness_before   = false;
    static const  float_round_style round_style = round_toward_zero;
};//template class numeric_limits<unsigned long long> 

template<> class numeric_limits<float>
{
    typedef float T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)() throw() { return FLT_MIN; }
    static        T     (max)() throw() { return FLT_MAX; }
    static const  int   digits            = FLT_MANT_DIG;
    static const  int   digits10          = FLT_DIG;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = false;
    static const  bool  is_exact          = false;
    static const  int   radix             = FLT_RADIX;
    static        T     epsilon()     throw() { return FLT_EPSILON; }
    static        T     round_error() throw() { return 0.5; }
    static const  int   min_exponent      = FLT_MIN_EXP;
    static const  int   min_exponent10    = FLT_MIN_10_EXP;
    static const  int   max_exponent      = FLT_MAX_EXP;
    static const  int   max_exponent10    = FLT_MAX_10_EXP;
    static const  bool  has_infinity      = true;
    static const  bool  has_quiet_NaN     = true;
    static const  bool  has_signaling_NaN = true;
    static const  float_denorm_style  has_denorm  = denorm_present;
    static const  bool  has_denorm_loss   = true;
    static T infinity()  throw()  { extern T __float_infinity;    return __float_infinity; }
    static T quiet_NaN() throw()  { extern T __float_quiet_NaN;   return __float_quiet_NaN; }
    static T signaling_NaN() throw() { extern T __float_signaling_NaN; return __float_signaling_NaN; }
    static T denorm_min() throw() { extern T __float_denorm_min;  return __float_denorm_min; }
    static const  bool  is_iec559         = true;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = false;
    static const  bool  traps             = true;
    static const  bool  tinyness_before   = true;
    static const  float_round_style round_style = round_to_nearest;
};//template class numeric_limits<float> 

template<> class numeric_limits<double>
{
    typedef double T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)() throw() { return DBL_MIN; }
    static        T     (max)() throw() { return DBL_MAX; }
    static const  int   digits            = DBL_MANT_DIG;
    static const  int   digits10          = DBL_DIG;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = false;
    static const  bool  is_exact          = false;
    static const  int   radix             = DBL_RADIX;
    static        T     epsilon()     throw() { return DBL_EPSILON; }
    static        T     round_error() throw() { return 0.5; }
    static const  int   min_exponent      = DBL_MIN_EXP;
    static const  int   min_exponent10    = DBL_MIN_10_EXP;
    static const  int   max_exponent      = DBL_MAX_EXP;
    static const  int   max_exponent10    = DBL_MAX_10_EXP;
    static const  bool  has_infinity      = true;
    static const  bool  has_quiet_NaN     = true;
    static const  bool  has_signaling_NaN = true;
    static const  float_denorm_style  has_denorm  = denorm_present;
    static const  bool  has_denorm_loss   = true;
    static T infinity()  throw()  { extern T __double_infinity;    return __double_infinity; }
    static T quiet_NaN() throw()  { extern T __double_quiet_NaN;   return __double_quiet_NaN; }
    static T signaling_NaN() throw() { extern T __double_signaling_NaN; return __double_signaling_NaN; }
    static T denorm_min() throw() { extern T __double_denorm_min;  return __double_denorm_min; }
    static const  bool  is_iec559         = true;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = false;
    static const  bool  traps             = true;
    static const  bool  tinyness_before   = true;
    static const  float_round_style round_style = round_to_nearest;
};//template class numeric_limits<double> 

template<> class numeric_limits<long double>
{
    typedef long double T;

  public:
    static const  bool  is_specialized    = true;
    static        T     (min)() throw() { return LDBL_MIN; }
    static        T     (max)() throw() { return LDBL_MAX; }
    static const  int   digits            = LDBL_MANT_DIG;
    static const  int   digits10          = LDBL_DIG;
    static const  bool  is_signed         = true;
    static const  bool  is_integer        = false;
    static const  bool  is_exact          = false;
    static const  int   radix             = LDBL_RADIX;
    static        T     epsilon()     throw() { return LDBL_EPSILON; }
    static        T     round_error() throw() { return 0.5; }
    static const  int   min_exponent      = LDBL_MIN_EXP;
    static const  int   min_exponent10    = LDBL_MIN_10_EXP;
    static const  int   max_exponent      = LDBL_MAX_EXP;
    static const  int   max_exponent10    = LDBL_MAX_10_EXP;
    static const  bool  has_infinity      = true;
    static const  bool  has_quiet_NaN     = true;
    static const  bool  has_signaling_NaN = true;
    static const  float_denorm_style  has_denorm  = denorm_present;
    static const  bool  has_denorm_loss   = true;
    static T infinity()  throw()  { extern T __long_double_infinity;    return __long_double_infinity; }
    static T quiet_NaN() throw()  { extern T __long_double_quiet_NaN;   return __long_double_quiet_NaN; }
    static T signaling_NaN() throw() { extern T __long_double_signaling_NaN; return __long_double_signaling_NaN; }
    static T denorm_min() throw() { extern T __long_double_denorm_min;  return __long_double_denorm_min; }
    static const  bool  is_iec559         = true;
    static const  bool  is_bounded        = true;
    static const  bool  is_modulo         = false;
    static const  bool  traps             = true;
    static const  bool  tinyness_before   = true;
    static const  float_round_style round_style = round_to_nearest;
};//template class numeric_limits<long double> 

/**@} lib_support_limits */
/**@} lib_language_support */

}//namespace std

#endif//#ifndef NTL__STLX_LIMITS
