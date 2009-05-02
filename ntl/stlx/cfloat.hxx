/**\file*********************************************************************
 *                                                                     \brief
 *  C Library [18.2.2 lib.c.limits]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CFLOAT
#define NTL__STLX_CFLOAT

namespace std {

/**\addtogroup  lib_language_support *** 18 Language support library [language.support]
 *@{*/
/**\addtogroup  lib_support_limits ***** 18.2 Implementation properties [support.limits]
 *@{*/

#ifdef __GNUC__
  #include <float.h>
  #ifndef DBL_RADIX
    #define DBL_RADIX FLT_RADIX
  #endif
  #ifndef LDBL_RADIX
    #define LDBL_RADIX FLT_RADIX
  #endif
#elif defined(_MSC_VER) || defined(__BCPLUSPLUS__)
  ///\name  float type
  #ifndef FLT_DIG
    #define FLT_DIG         6
    #define FLT_EPSILON     1.192092896e-07F
    #define FLT_GUARD       0
    #define FLT_MANT_DIG    24
    #define FLT_MAX         3.402823466e+38F
    #define FLT_MAX_10_EXP  38
    #define FLT_MAX_EXP     128
    #define FLT_MIN         1.175494351e-38F
    #define FLT_MIN_10_EXP  (-37)
    #define FLT_MIN_EXP     (-125)
    #define FLT_NORMALIZE   0
    #define FLT_RADIX       2
    #define FLT_ROUNDS      1
  #endif

  ///\name  double type
  #ifndef DBL_DIG
    #define DBL_DIG         15
    #define DBL_EPSILON     2.2204460492503131e-016
    #define DBL_MANT_DIG    53
    #define DBL_MAX         1.7976931348623158e+308
    #define DBL_MAX_10_EXP  308
    #define DBL_MAX_EXP     1024
    #define DBL_MIN         2.2250738585072014e-308
    #define DBL_MIN_10_EXP  (-307)
    #define DBL_MIN_EXP     (-1021)
    #define DBL_RADIX       2
    #define DBL_ROUNDS      1
  #endif

  ///\name  long double type
  #ifndef LDBL_DIG
    #define LDBL_DIG        DBL_DIG
    #define LDBL_EPSILON    DBL_EPSILON
    #define LDBL_MANT_DIG   DBL_MANT_DIG
    #define LDBL_MAX        DBL_MAX
    #define LDBL_MAX_10_EXP DBL_MAX_10_EXP
    #define LDBL_MAX_EXP    DBL_MAX_EXP
    #define LDBL_MIN        DBL_MIN
    #define LDBL_MIN_10_EXP DBL_MIN_10_EXP
    #define LDBL_MIN_EXP    DBL_MIN_EXP
    #define LDBL_RADIX      DBL_RADIX
    #define LDBL_ROUNDS     DBL_ROUNDS
  #endif

#else
  #error Compiler not supported
#endif

/**@} lib_support_limits */
/**@} lib_language_support */

}//namespace std

#endif//#ifndef NTL__STLX_CFLOAT
