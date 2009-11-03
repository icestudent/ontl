/**\file*********************************************************************
 *                                                                     \brief
 *  26.8 C Library [c.math]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_CMATH
#define NTL__STLX_CMATH
#pragma once

#include "cfloat.hxx"
#include "type_traits.hxx"

namespace std
{
 /**\addtogroup  lib_numeric ************ 26 Numerics library [numerics]
  *@{
 /**\addtogroup  lib_cmath   ************ 26.8 C Library [c.math]
  *@{*/

#ifdef FLT_EVAL_METHOD
# if (FLT_EVAL_METHOD == 0)
  typedef float float_t;
  typedef double double_t;
# elif (FLT_EVAL_METHOD == 1)
  typedef double float_t;
  typedef double double_t;
# elif (FLT_EVAL_METHOD == 2)
  typedef long double float_t;
  typedef long double double_t;
#endif
#else
  typedef long double float_t;
  typedef long double double_t;
#endif

#define  FP_ILOGB0
#define  FP_ILOGBNAN

#define  MATH_ERRNO     1
#define  MATH_ERREXCEPT 2

  // no error handling
  const int math_errhandling = 0;


  ///\name 7.12.3 Classification
#define HUGE_VAL (*(double*)&std::fpclass::_hex_infinityd)
#define HUGE_VALF (*(float*)&std::fpclass::_hex_infinityf)
#define HUGE_VALL HUGE_VALF

#define INFINITY HUGE_VALF

#define  NAN      (*(float*)&std::fpclass::_hex_qnanf)

#define  FP_INFINITE  std::fpclass::infinity
#define  FP_NAN       std::fpclass::nan
#define  FP_NORMAL    std::fpclass::normal
#define  FP_SUBNORMAL std::fpclass::subnormal
#define  FP_ZERO      std::fpclass::zero


  namespace fpclass 
  {
    enum type {
      zero      = 0,
      denormal  = 1,
      normal    = 2,
      finite    = denormal|normal,
      infinity  = 4,
      snan      = 8,
      qnan      = 10,
      nan       = qnan|snan,

      negative  = 1 << 7,
      sign  = negative,
      subnormal = denormal,

      negative_zero = zero|sign,
      negative_denormal = denormal|sign,
      negative_normal = normal|sign,
      negative_finite = finite|sign,
      negative_infinity = infinity|sign,
      negative_snan = snan|sign,
      negative_qnan = qnan|sign,
      negative_nan  = nan|sign
    };

    static const uint32_t _hex_infinityf = 0x7F800000, _hex_qnanf = 0x7FC00000;
    static const uint64_t _hex_infinityd = 0x7FF0000000000000, _hex_qnand = 0x7FF8000000000000;
  }
  typedef fpclass::type fpclass_t;

  template<typename T> typename enable_if<is_floating_point<T>::value, 
    fpclass_t>::type fpclassify(T x);

  template<typename T> typename enable_if<is_floating_point<T>::value, 
    bool>::type isfinite(T x);
  template<typename T> typename enable_if<is_floating_point<T>::value, 
    bool>::type isinf(T x);
  template<typename T> typename enable_if<is_floating_point<T>::value, 
    bool>::type isnan(T x);
  template<typename T> typename enable_if<is_floating_point<T>::value, 
    bool>::type isnormal(T x);
  template<typename T> typename enable_if<is_floating_point<T>::value, 
    bool>::type signbit(T x);

  template<typename T> inline typename enable_if<is_integral<T>::value, 
    fpclass_t>::type fpclassify(T x) { return fpclassify<double>(double(x)); }

  template<typename T> inline typename enable_if<is_integral<T>::value, 
    bool>::type isfinite(T) { return true; }
  template<typename T> inline typename enable_if<is_integral<T>::value, 
    bool>::type isinf(T) { return false; }
  template<typename T> inline typename enable_if<is_integral<T>::value, 
    bool>::type isnan(T) { return false; }
  template<typename T> inline typename enable_if<is_integral<T>::value, 
    bool>::type isnormal(T) { return x != 0; }
  template<typename T> inline typename enable_if<is_integral<T>::value, 
    bool>::type signbit(T x)  { return is_signed<T>::value && x < 0; }



  ///\name 7.12.4 Trigonometric functions
  double acos(double x);
  float acosf(float x);
  long double acosl(long double x);

  double asin(double x);
  float asinf(float x);
  long double asinl(long double x);

  double atan(double x);
  float atanf(float x);
  long double atanl(long double x);

  double atan2(double y, double x);
  float atan2f(float y, float x);
  long double atan2l(long double y, long double x);

  double cos(double x);
  float cosf(float x);
  long double cosl(long double x);

  double sin(double x);
  float sinf(float x);
  long double sinl(long double x);

  double tan(double x);
  float tanf(float x);
  long double tanl(long double x);


  ///\name 7.12.5 Hyperbolic functions
  double acosh(double x);
  float acoshf(float x);
  long double acoshl(long double x);

  double asinh(double x);
  float asinhf(float x);
  long double asinhl(long double x);

  double atanh(double x);
  float atanhf(float x);
  long double atanhl(long double x);

  double cosh(double x);
  float coshf(float x);
  long double coshl(long double x);

  double sinh(double x);
  float sinhf(float x);
  long double sinhl(long double x);

  double tanh(double x);
  float tanhf(float x);
  long double tanhl(long double x);


  ///\name 7.12.6  Exponential and logarithmic functions
  double exp(double x);
  float expf(float x);
  long double expl(long double x);

  double exp2(double x);
  float exp2f(float x);
  long double exp2l(long double x);

  double expm1(double x);
  float expm1f(float x);
  long double expm1l(long double x);

  double frexp(double value, int *exp);
  float frexpf(float value, int *exp);
  long double frexpl(long double value, int *exp);
  
  int ilogb(double x);
  int ilogbf(float x);
  int ilogbl(long double x);

  double ldexp(double x, int exp);
  float ldexpf(float x, int exp);
  long double ldexpl(long double x, int exp);

  double log(double x);
  float logf(float x);
  long double logl(long double x);

  double log10(double x);
  float log10f(float x);
  long double log10l(long double x);

  double log1p(double x);
  float log1pf(float x);
  long double log1pl(long double x);

  double log2(double x);
  float log2f(float x);
  long double log2l(long double x);

  double logb(double x);
  float logbf(float x);
  long double logbl(long double x);

  double modf(double value, double *iptr);
  float modff(float value, float *iptr);
  long double modfl(long double value, long double *iptr);

  double scalbn(double x, int n);
  float scalbnf(float x, int n);
  long double scalbnl(long double x, int n);
  double scalbln(double x, long int n);
  float scalblnf(float x, long int n);
  long double scalblnl(long double x, long int n);

  
  ///\name 7.12.7  Power and absolute-value functions
  double cbrt(double x);
  float cbrtf(float x);
  long double cbrtl(long double x);

  double fabs(double x);
  float fabsf(float x);
  long double fabsl(long double x);

  double hypot(double x, double y);
  float hypotf(float x, float y);
  long double hypotl(long double x, long double y);

  double pow(double x, double y);
  float powf(float x, float y);
  long double powl(long double x, long double y);

  double sqrt(double x);
  float sqrtf(float x);
  long double sqrtl(long double x);

  
  ///\name 7.12.8  Error and gamma functions
  double erf(double x);
  float erff(float x);
  long double erfl(long double x);

  double erfc(double x);
  float erfcf(float x);
  long double erfcl(long double x);

  double lgamma(double x);
  float lgammaf(float x);
  long double lgammal(long double x);

  double tgamma(double x);
  float tgammaf(float x);
  long double tgammal(long double x);


  ///\name 7.12.9  Nearest integer functions
  double ceil(double x);
  float ceilf(float x);
  long double ceill(long double x);

  double floor(double x);
  float floorf(float x);
  long double floorl(long double x);

  double nearbyint(double x);
  float nearbyintf(float x);
  long double nearbyintl(long double x);

  double rint(double x);
  float rintf(float x);
  long double rintl(long double x);

  long int lrint(double x);
  long int lrintf(float x);
  long int lrintl(long double x);
  long long int llrint(double x);
  long long int llrintf(float x);
  long long int llrintl(long double x);

  double round(double x);
  float roundf(float x);
  long double roundl(long double x);

  long int lround(double x);
  long int lroundf(float x);
  long int lroundl(long double x);
  long long int llround(double x);
  long long int llroundf(float x);
  long long int llroundl(long double x);

  double trunc(double x);
  float truncf(float x);
  long double truncl(long double x);


  ///\name 7.12.10 Remainder functions
  double fmod(double x, double y);
  float fmodf(float x, float y);
  long double fmodl(long double x, long double y);

  double remainder(double x, double y);
  float remainderf(float x, float y);
  long double remainderl(long double x, long double y);

  double remquo(double x, double y, int *quo);
  float remquof(float x, float y, int *quo);
  long double remquol(long double x, long double y, int *quo);


  ///\name 7.12.11 Manipulation functions
  double copysign(double x, double y);
  float copysignf(float x, float y);
  long double copysignl(long double x, long double y);

  double nan(const char *tagp);
  float nanf(const char *tagp);
  long double nanl(const char *tagp);

  double nextafter(double x, double y);
  float nextafterf(float x, float y);
  long double nextafterl(long double x, long double y);

  double nexttoward(double x, long double y);
  float nexttowardf(float x, long double y);
  long double nexttowardl(long double x, long double y);


  ///\name 7.12.12  Maximum, minimum, and positive difference functions
  double fdim(double x, double y);
  float fdimf(float x, float y);
  long double fdiml(long double x, long double y);

  double fmax(double x, double y);
  float fmaxf(float x, float y);
  long double fmaxl(long double x, long double y);

  double fmin(double x, double y);
  float fminf(float x, float y);
  long double fminl(long double x, long double y);

  double fma(double x, double y, double z);
  float fmaf(float x, float y, float z);
  long double fmal(long double x, long double y, long double z);


  ///\name 7.12.14 Comparison macros
  namespace __ {
    template<typename T1, typename T2>
    struct float_promote
    {
    private:
      static sfinae_passed_tag test(T1);
      static sfinae_failed_tag test(T2);
      static T1 __t1();
      static T2 __t2();

      typedef conditional<sizeof(test(__t1()+__t2())) == sizeof(sfinae_passed_tag), T1, T2> select;
    public:
      typedef typename select::type type;
    };

    template<typename T> struct float_promote<T,T>: float_promote<T,float> {};
    template<> struct float_promote<float,float> { typedef float type; };

    template<typename T> bool isgreater(T x, T y);
    template<typename T> bool isgreaterequal(T x, T y);
    template<typename T> bool isless(T x, T y);
    template<typename T> bool islessequal(T x, T y);
    template<typename T> bool islessgreater(T x, T y);
    template<typename T> bool isunordered(T x, T y);
  }

  template<typename U, typename V>
  inline typename enable_if<is_arithmetic<U>::value && is_arithmetic<V>::value, bool>::type
    isgreater(U x, V y)
  {
    typedef typename __::float_promote<U,V>::type T;
    return __::isgreater<T>(T(x), T(y));
  }
  template<typename U, typename V>
  inline typename enable_if<is_arithmetic<U>::value && is_arithmetic<V>::value, bool>::type
    isgreaterequal(U x, V y)
  {
    typedef typename __::float_promote<U,V>::type T;
    return __::isgreaterequal<T>(T(x), T(y));
  }
  template<typename U, typename V>
  inline typename enable_if<is_arithmetic<U>::value && is_arithmetic<V>::value, bool>::type
    isless(U x, V y)
  {
    typedef typename __::float_promote<U,V>::type T;
    return __::isless<T>(T(x), T(y));
  }
  template<typename U, typename V>
  inline typename enable_if<is_arithmetic<U>::value && is_arithmetic<V>::value, bool>::type
    islessequal(U x, V y)
  {
    typedef typename __::float_promote<U,V>::type T;
    return __::islessequal<T>(T(x), T(y));
  }
  template<typename U, typename V>
  inline typename enable_if<is_arithmetic<U>::value && is_arithmetic<V>::value, bool>::type
    islessgreater(U x, V y)
  {
    typedef typename __::float_promote<U,V>::type T;
    return __::islessgreater<T>(T(x), T(y));
  }
  template<typename U, typename V>
  inline typename enable_if<is_arithmetic<U>::value && is_arithmetic<V>::value, bool>::type
    isunordered(U x, V y)
  {
    typedef typename __::float_promote<U,V>::type T;
    return __::isunordered<T>(T(x), T(y));
  }

  ///\name 26.8/10 Added signatures
  float abs(float);
  float acos(float);
  float acosh(float);
  float asin(float);
  float asinh(float);
  float atan(float);
  float atan2(float, float);
  float atanh(float);
  float cbrt(float);
  float ceil(float);
  float copysign(float, float);
  float cos(float);
  float cosh(float);
  float erf(float);
  float erfc(float);
  float exp(float);
  float exp2(float);
  float expm1(float);
  float fabs(float);
  float fdim(float, float);
  float floor(float);
  float fma(float, float, float);
  float fmax(float, float);
  float fmin(float, float);

  float fmod(float, float);
  float frexp(float, int*);
  float hypot(float, float);
  int ilogb(float);
  float ldexp(float, int);
  float lgamma(float);
  long long llrint(float);
  long long llround(float);
  float log(float);
  float log10(float);
  float log1p(float);
  float log2(float);
  float logb(float);
  long lrint(float);
  long lround(float);
  float modf(float, float*);
  float nearbyint(float);
  float nextafter(float, float);
  float nexttoward(float, long double);
  float pow(float, float);
  float remainder(float, float);
  float remquo(float, float, int *);
  float rint(float);
  float round(float);
  float scalbln(float, long);
  float scalbn(float, int);
  float sin(float);
  float sinh(float);
  float sqrt(float);
  float tan(float);
  float tanh(float);
  float tgamma(float);
  float trunc(float);
  double abs(double);
  ///\}


  /**@} lib_cmath */
  /**@} lib_numeric */
} // std
#endif // NTL__STLX_CMATH
