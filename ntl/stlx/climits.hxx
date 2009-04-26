/**\file*********************************************************************
 *                                                                     \brief
 *  C Library [18.2.2 lib.c.limits]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CLIMITS
#define NTL__STLX_CLIMITS

namespace std {

/**\addtogroup  lib_language_support *** 18 Language support library [language.support] ******
 *@{*/

/**\defgroup  lib_support_limits ******* 18.2 Implementation properties [support.limits] ***
 *
 *    Characteristics of implementation dependent fundamental types
 *@{
 */

///\name  Limits of fundamental integer types

#ifndef CHAR_BIT
#define CHAR_BIT  8
#endif

#ifndef SCHAR_MIN
#define SCHAR_MIN (-128)
#endif

#ifndef SCHAR_MAX
#define SCHAR_MAX 127
#endif

#ifndef UCHAR_MAX
#define UCHAR_MAX 0xff
#endif

#ifndef CHAR_MIN
#if defined(_MSC_VER) || defined(__BCPLUSPLUS__) || defined(__GNUC__)
  #ifndef _CHAR_UNSIGNED
    #define CHAR_MIN    SCHAR_MIN
    #define CHAR_MAX    SCHAR_MAX
  #else
    #define CHAR_MIN    0
    #define CHAR_MAX    UCHAR_MAX
  #endif
#else
  #error do define CHAR_MIN and CHAR_MAX
  //#define CHAR_MIN
  //#define CHAR_MAX
#endif//#ifdef  _MSC_VER
#endif

#ifndef MB_LEN_MAX
#define MB_LEN_MAX  5
#endif

#ifndef WCHAR_MIN
#define WCHAR_MIN   0
#endif

#ifndef WCHAR_MAX
#define WCHAR_MAX   0xffff
#endif

#ifndef SHRT_MIN
#define SHRT_MIN    (-32768)
#endif

#ifndef SHRT_MAX
#define SHRT_MAX    32767
#endif

#ifndef USHRT_MAX
#define USHRT_MAX   0xffff
#endif

#ifndef INT_MIN
#define INT_MIN     (-INT_MAX-1)
#endif

#ifndef INT_MAX
#define INT_MAX     2147483647
#endif

#ifndef UINT_MAX
#define UINT_MAX    0xffffffff
#endif

#ifndef LONG_MIN
#define LONG_MIN    (-LONG_MAX-1L)
#endif

#ifndef LONG_MAX
#define LONG_MAX    2147483647L
#endif

#ifndef ULONG_MAX
#define ULONG_MAX   0xffffffffUL
#endif

#ifndef LLONG_MAX
  #if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
    #define LLONG_MAX   9223372036854775807i64
  #elif defined(__GNUC__)
    #define LLONG_MAX   __LONG_LONG_MAX__
  #endif
#endif

#ifndef LLONG_MIN
  //#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
  //  #define LLONG_MIN   (-9223372036854775808i64)
  //#elif defined(__GNUC__)
    #define LLONG_MIN   (-LLONG_MAX-1)
  //#endif
#endif

#ifndef ULLONG_MAX
  #if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
    #define ULLONG_MAX  0xffffffffffffffffui64
  #elif defined(__GNUC__)
    #define ULLONG_MAX  (LLONG_MAX * 2ULL + 1)
  #endif
#endif

/**@} lib_support_limits */
/**@} lib_language_support */

}//namespace std

#endif//#ifndef NTL__STLX_CLIMITS
