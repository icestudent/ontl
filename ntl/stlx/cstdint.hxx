/**\file*********************************************************************
 *                                                                     \brief
 *  Integer types ISO9899: 7.18
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CSTDINT
#define NTL__STLX_CSTDINT

#include "climits.hxx"

namespace std {

/**\addtogroup  lib_language_support *** 18 Language support library [language.support] ******
 *@{*/
/**\addtogroup  lib_support_types ****** 18.1 Types [support.types] ***********************
 *@{*/

///\name  Exact-width integer types 7.18.1.1
typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef short               int16_t;
typedef unsigned short      uint16_t;
typedef long                int32_t;
typedef unsigned long       uint32_t;
typedef long long           int64_t;
typedef unsigned long long  uint64_t;

///\name  Minimum-width integer types 7.18.1.2
typedef int8_t    int_least8_t;
typedef uint8_t   uint_least8_t;
typedef int16_t   int_least16_t;
typedef uint16_t  uint_least16_t;
typedef int32_t   int_least32_t;
typedef uint32_t  uint_least32_t;
typedef int64_t   int_least64_t;
typedef uint64_t  uint_least64_t;

///\name  Fastest minimum-width integer types 7.18.1.3
typedef int8_t    int_fast8_t;
typedef uint8_t   uint_fast8_t;
typedef int16_t   int_fast16_t;
typedef uint16_t  uint_fast16_t;
typedef int32_t   int_fast32_t;
typedef uint32_t  uint_fast32_t;
typedef int64_t   int_fast64_t;
typedef uint64_t  uint_fast64_t;

///\name  Integer types capable of holding object pointers 7.18.1.4
#ifdef __GNUC__
  typedef __PTRDIFF_TYPE__  intptr_t;
  typedef __SIZE_TYPE__     uintptr_t;
#else
  #ifdef _M_X64
  typedef          long long  intptr_t;
  typedef unsigned long long  uintptr_t;
  #else
  typedef __w64          int  intptr_t;
  typedef __w64 unsigned int  uintptr_t;
  #endif
#endif

///\name  Greatest-width integer types 7.18.1.5
typedef int64_t   intmax_t;
typedef uint64_t  uintmax_t;

///@}

/**@} lib_support_types
 */

/**\addtogroup  lib_support_limits ***** 18.2 Implementation properties [support.limits] ***
 *@{*/

///\name  Limits of exact-width integer types 7.18.2.1
#define INT8_MIN          SCHAR_MIN
#define INT8_MAX          SCHAR_MAX
#define UINT8_MAX         UCHAR_MAX

#define INT16_MIN         SHRT_MIN
#define INT16_MAX         SHRT_MAX
#define UINT16_MAX        USHRT_MAX

#define INT32_MIN         INT_MIN
#define INT32_MAX         INT_MAX
#define UINT32_MAX        UINT_MAX

#define INT64_MIN         LLONG_MIN
#define INT64_MAX         LLONG_MAX
#define UINT64_MAX        ULLONG_MAX

///\name  Limits of minimum-width integer types 7.18.2.2
#define INT_LEAST8_MIN    INT8_MIN
#define INT_LEAST8_MAX    INT8_MAX
#define UINT_LEAST8_MAX   UINT8_MAX

#define INT_LEAST16_MIN   INT16_MIN
#define INT_LEAST16_MAX   INT16_MAX
#define UINT_LEAST16_MAX  UINT16_MAX

#define INT_LEAST32_MIN   INT32_MIN
#define INT_LEAST32_MAX   INT32_MAX
#define UINT_LEAST32_MAX  UINT32_MAX

#define INT_LEAST64_MIN   INT64_MIN
#define INT_LEAST64_MAX   INT64_MAX
#define UINT_LEAST64_MAX  UINT64_MAX

///\name  Limits of fastest minimum-width integer types 7.18.2.3
#define INT_FAST8_MIN     INT8_MIN
#define INT_FAST8_MAX     INT8_MAX
#define UINT_FAST8_MAX    UINT8_MAX

#define INT_FAST16_MIN    INT16_MIN
#define INT_FAST16_MAX    INT16_MAX
#define UINT_FAST16_MAX   UINT16_MAX

#define INT_FAST32_MIN    INT32_MIN
#define INT_FAST32_MAX    INT32_MAX
#define UINT_FAST32_MAX   UINT32_MAX

#define INT_FAST64_MIN    INT64_MIN
#define INT_FAST64_MAX    INT64_MAX
#define UINT_FAST64_MAX   UINT64_MAX

///\name  Limits of integer types capable of holding object pointers 7.18.2.4

#ifdef __GNUC__
  #if (__SIZEOF_POINTER__ == 4)
    #define INTPTR_MIN        INT32_MIN
    #define INTPTR_MAX        INT32_MAX
    #define UINTPTR_MAX       UINT32_MAX
  #elif (__SIZEOF_POINTER__ == 8)
    #define INTPTR_MIN        INT64_MIN
    #define INTPTR_MAX        INT64_MAX
    #define UINTPTR_MAX       UINT64_MAX
  #endif
#else
  #ifdef _M_X64
    #define INTPTR_MIN        INT64_MIN
    #define INTPTR_MAX        INT64_MAX
    #define UINTPTR_MAX       UINT64_MAX
  #else
    #define INTPTR_MIN        INT32_MIN
    #define INTPTR_MAX        INT32_MAX
    #define UINTPTR_MAX       UINT32_MAX
  #endif
#endif

///\name  Limits of greatest-width integer types 7.18.2.5
#define INTMAX_MIN        INT64_MIN
#define INTMAX_MAX        INT64_MAX
#define UINTMAX_MAX       UINT64_MAX

///\name  Limits of other integer types 7.18.3
#ifdef __STDC_LIMIT_MACROS

#endif//#ifdef __STDC_LIMIT_MACROS

/**@} lib_support_limits */
/**@} lib_language_support */

}//namespace std

#endif//#ifndef NTL__STLX_CSTDINT
