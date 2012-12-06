/**\file*********************************************************************
 *                                                                     \brief
 *  NTL compiler configuration for C++0x features
 *
 ****************************************************************************
 */
#ifndef NTL__BASECONF_CPP0X
#define NTL__BASECONF_CPP0X
#pragma once

#if __cplusplus > 199711L
  /// new C++ Standard
  #define NTL_CXX

  // keywords:
  // align (N2798+)
  #define NTL_CXX_ALIGN
  // alignas( <= N2723)
  #define NTL_CXX_ALIGNAS
  // alignof
  #define NTL_CXX_ALIGNOF
  // auto
  #define NTL_CXX_AUTO
  // auto f() -> typeid (requires auto & decltype)
  # define NTL_CXX_AUTORET
  // char16_t, char32_t (as builtin types)
  #define NTL_CXX_CHARS_TYPES
  // unicode string literals (u'' / U"")
  #define NTL_CXX_CHARS
  // concepts, concept_map, requires (removed from c++0x)
  // #define NTL_CXX_CONCEPT
  // constexpr
  #define NTL_CXX_CONSTEXPR
  // decltype (typeof)
  #define NTL_CXX_TYPEOF   11
  // class enum
  #define NTL_CXX_ENUM
  // nullptr
  #define NTL_CXX_NULLPTR
  // static assert
  #define NTL_CXX_ASSERT
  // thread_local
  #define NTL_CXX_THREADL
  // __func__
  #define NTL_CXX_FUNC

  // C++0x attributes
  #define NTL_CXX_ATTRIBUTES
  #define NTL_CXX_NOEXCEPT

  // language features:
  // explicit delete/default function definition
  #define NTL_CXX_EF
  // explicit conversion operators
  #define NTL_CXX_EXPLICITOP
  // initializer lists
  #define NTL_CXX_IL
  // lambda
  #define NTL_CXX_LAMBDA   11
  // rvalue references v3.0
  #define NTL_CXX_RV       30
  // template typedef
  #define NTL_CXX_TT
  // variadic templates (implies rvalue references support)
  #define NTL_CXX_VT
  // range-based for loop
  #define NTL_CXX_FOR

// compiler-specific parameters
#ifdef __clang__
# undef NTL_CXX_CHARS_TYPES
# undef NTL_CXX_VT // temporary
#endif

#endif // pure c++0x



#if __cplusplus <= 199711L // legacy compilers

#if defined(_MSC_VER) && !defined(__ICL) && !defined(__clang__)
#if _MSC_VER >= 1600

/** VC10's partial C++0x support */
// _MSC_FULL_VER: 160011001 (CTP), 160020506 (beta1), 160021003 (beta2), 160030128 (rc), 160040219 (sp1), 170040825 (ctp), 170050214 (b1), 170050727 (sp0), 170051025

#define NTL_CXX_AUTO
#define NTL_CXX_ASSERT
#define NTL_CXX_LAMBDA

#if _MSC_FULL_VER >= 160020506 // beta1
# define NTL_CXX_AUTORET
# define NTL_CXX_TYPEOF
#endif

#if _MSC_FULL_VER >= 160021003 // beta2
# define NTL_CXX_NULLPTR
#endif

#if _MSC_FULL_VER >= 170040825 // v17 b1
# define NTL_CXX_ENUM
# define NTL_CXX_NULLPTR
# define NTL_CXX_RV 21
#else
# define NTL_CXX_RV 20
#endif

#if _MSC_FULL_VER >= 170050727 // v17
# define NTL_CXX_FOR
#endif

#if _MSC_FULL_VER >= 170051025 // v17 november update
# define NTL_CXX_IL
# define NTL_CXX_EXPLICITOP
//# define NTL_CXX_VT // buggy
#endif

#if _MSC_FULL_VER == 170051106 // v17 update 1
# undef NTL_CXX_IL
# undef NTL_CXX_EXPLICITOP
# undef NTL_CXX_VT
#endif

#endif // _MSC_VER >= 1600

#elif defined(__BCPLUSPLUS__)

/** BCB's partial C++0x support */
#define NTL_CXX_ATTRIBUTES
#define NTL_CXX_ALIGNOF
#define NTL_CXX_CHARS
#define NTL_CXX_TYPEOF
#define NTL_CXX_ENUM
#define NTL_CXX_ASSERT

#define NTL_CXX_EXTPL
#define NTL_CXX_RV 10

#elif defined(__ICL)

#if (__ICL >= 1100 && !defined(_MSC_EXTENSIONS)) || (defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403))
// detecting -std=C++0x presence^^^, works only before 12.1

/** ICL's partial C++0x support */

#define NTL_CXX_AUTO
#define NTL_CXX_TYPEOF
#define NTL_CXX_ASSERT
#define NTL_CXX_EXTPL
#define NTL_CXX_LAMBDA

#if __ICL >= 1200
#define NTL_CXX_FUNC
#define NTL_CXX_EF
#define NTL_CXX_ENUM
#define NTL_CXX_TYPEOF
#define NTL_CXX_RV     20
#endif

#ifdef __EDG_VERSION__
#if __EDG_VERSION__ >= 403
/** 12.1 */
#define NTL_CXX_AUTORET
#define NTL_CXX_CHARS_TYPES
#define NTL_CXX_CHARS
#define NTL_CXX_NULLPTR
#define NTL_CXX_TT
//#define NTL_CXX_VT
#define NTL_CXX_ATTRIBUTES
#undef  NTL_CXX_FUNC
#endif
#endif  // EDG

#if __ICL >= 1300
  /** 13.0 */
#define NTL_CXX_CONSTEXPR
//#define NTL_CXX_NOEXCEPT  // not work
#define NTL_CXX_EXPLICITOP
#define NTL_CXX_IL
#define NTL_CXX_FOR
#undef NTL_CXX_CHARS_TYPES  // undefined even with /Qoption,cpp,"--uliterals"
#endif

#endif // __ICL

#elif defined(__GNUC__) && defined(__GXX_EXPERIMENTAL_CXX0X__)

/** GCC's partial C++0x support */

#define NTL_CXX_AUTO
#define NTL_CXX_AUTORET
#define NTL_CXX_TYPEOF
#define NTL_CXX_ENUM
#define NTL_CXX_ASSERT
#define NTL_CXX_EF
#define NTL_CXX_EXTPL
#define NTL_CXX_IL
#define NTL_CXX_RV       30
#define NTL_CXX_VT
#define NTL_CXX_FUNC

#if((__GNUC__*10+__GNUC_MINOR__) >= 45)
# define NTL_CXX_EXPLICITOP	// 4.5
# define NTL_CXX_LAMBDA		// 4.5
#endif // gcc 4.5+

#elif defined(__clang__) && defined(__GXX_EXPERIMENTAL_CXX0X__)



#endif // compilers set
#endif // legacy compilers

#endif // NTL__BASECONF_CPP0X
