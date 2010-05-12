/**\file*********************************************************************
 *                                                                     \brief
 *  18.2 Types [lib.support.types]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_CSTDDEF
#define NTL__STLX_CSTDDEF
#pragma once

#include "../baseconf.hxx"
#include "../baseconf_0x.hxx"
#include "cpp0x_mode.hxx"

// NTL LIB signature
#define NTL__CPPLIB

namespace std {

// Boost support
#ifndef __STDC_VERSION__
#define __STDC_VERSION__ 199901 
#endif

/**\addtogroup  lib_language_support *** 18 Language support library [language.support]
 *@{*/

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable:4324)
  #ifndef __ICL
  typedef alignas(8192)  struct {} max_align_t;
  #else
  typedef alignas(8192)  struct {void*_;} max_align_t;
  #endif
  #pragma warning(pop)
#else
  typedef struct {} max_align_t;
#endif

/**\addtogroup  lib_support_types ****** 18.2 Types [support.types]
 *@{*/

#if defined(NTL__CXX_NULLPTR)
  typedef decltype(nullptr) nullptr_t;
#elif defined(__DOXYGEN__)
  struct nullptr_t {};
#endif

// favour for habit
#ifndef NULL
#define NULL nullptr
#endif

#ifdef __GNUC__

  typedef __PTRDIFF_TYPE__  ptrdiff_t;
  typedef __SIZE_TYPE__     size_t;
  typedef __PTRDIFF_TYPE__  ssize_t;

#else

  #ifdef _M_X64
      typedef          long long  ptrdiff_t;
  # ifdef _MSC_VER
      using ::size_t;
  # else
      typedef unsigned long long  size_t;
  # endif
      typedef          long long  ssize_t;
  #else
      typedef __w64    int  ptrdiff_t;
  # ifdef _MSC_VER
      using ::size_t;
  # else
      typedef unsigned int  size_t;
  # endif
      typedef __w64    int  ssize_t;
  #endif

#endif

#ifndef offsetof
  #define offsetof(s,m) (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))
  #define offsetof_ptr(s,memptr) (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->*memptr))
  // from bcb #define offsetof( s_name, m_name )  (size_t)&(((s_name*)0)->m_name)
#endif

#define __ntl_bitmask_type(bitmask, _func_spec)\
  _func_spec bitmask operator&(bitmask x, bitmask y) { return ntl::bitwise_and(x, y); }\
  _func_spec bitmask operator|(bitmask x, bitmask y) { return ntl::bitwise_or (x, y); }\
  _func_spec bitmask operator^(bitmask x, bitmask y) { return ntl::bitwise_xor(x, y); }\
  _func_spec bitmask operator~(bitmask x) { return static_cast<bitmask>(~static_cast<unsigned>(x)); }\
  _func_spec bitmask& operator&=(bitmask& x, bitmask y) { x = x&y ; return x ; }\
  _func_spec bitmask& operator|=(bitmask& x, bitmask y) { x = x|y ; return x ; }\
  _func_spec bitmask& operator^=(bitmask& x, bitmask y) { x = x^y ; return x ; }

#ifdef _M_X64
# define __ptr_align alignas(8)
#else
# define __ptr_align alignas(4)
#endif
/**@} lib_support_types */
/**@} lib_language_support */


  /**\addtogroup  lib_utilities ********** 20 General utilities library [utilities]
  *@{*/
  /**\addtogroup  lib_memory ************* 20.8 Memory [memory]
  *@{*/

  /// 20.8.1 Allocator argument tag [allocator.tag]
  struct allocator_arg_t {/**/};
#ifdef _MSC_VER
  const allocator_arg_t allocator_arg;// = allocator_arg_t();
#else
  const allocator_arg_t allocator_arg = allocator_arg_t();
#endif
  /**@} lib_memory */
  /**@} lib_utilities */


} //namespace std

#ifndef _MSC_VER
  using std::ptrdiff_t;
  using std::size_t;
#endif


/// _countof macro to calculate array length in compile time
template <typename T, size_t N>
char (*__countof_helper(T(&array)[N]))[N];
#define _countof(array) sizeof(*__countof_helper(array))
#define _endof(array) (array+_countof(array))

namespace std { namespace __{ using ntl::explicit_bool; using ntl::explicit_bool_type; } }

namespace stlx = std;

#endif //#ifndef NTL__STLX_CSTDDEF
