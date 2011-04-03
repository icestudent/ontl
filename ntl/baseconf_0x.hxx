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
  #define NTL__CXX

  // keywords:
  // align (N2798+)
  #define NTL__CXX_ALIGN
  // alignas( <= N2723)
  #define NTL__CXX_ALIGNAS
  // alignof
  #define NTL__CXX_ALIGNOF
  // auto
  #define NTL__CXX_AUTO
  // auto f() -> typeid (requires auto & decltype)
  # define NTL__CXX_AUTORET
  // char16_t, char32_t (as builtin types)
  #define NTL__CXX_CHARS_TYPES
  // unicode string literals (u'' / U"")
  #define NTL__CXX_CHARS
  // concepts, concept_map, requires (removed from c++0x)
  // #define NTL__CXX_CONCEPT
  // constexpr
  #define NTL__CXX_CONSTEXPR
  // decltype (typeof)
  #define NTL__CXX_TYPEOF
  // class enum
  #define NTL__CXX_ENUM
  // nullptr
  #define NTL__CXX_NULLPTR
  // static assert
  #define NTL__CXX_ASSERT
  // thread_local
  #define NTL__CXX_THREADL
  // __func__
  #define NTL__CXX_FUNC

  // C++0x attributes
  #define NTL__CXX_ATTRIBUTES

  // language features:
  // explicit delete/default function definition
  #define NTL__CXX_EF
  // explicit conversion operators
  #define NTL__CXX_EXPLICITOP
  // initializer lists
  #define NTL__CXX_IL
  // lambda
  #define NTL__CXX_LAMBDA
  // rvalue references
  #define NTL__CXX_RV
  // new rvalue reference behaviour
  #define NTL__CXX_RVFIX
  // template typedef
  #define NTL__CXX_TT
  // variadic templates (implies rvalue references support)
  #define NTL__CXX_VT

#endif // pure c++0x



#if __cplusplus <= 199711L // legacy compilers

#if defined(_MSC_VER) && !defined(__ICL)
#if _MSC_VER >= 1600

/** VC10's partial C++0x support */
// _MSC_FULL_VER: 160011001 (CTP), 160020506 (beta1), 160021003 (beta2), 160030128 (rc), 160040219 (sp1)

#define NTL__CXX_AUTO
#if _MSC_FULL_VER >= 160020506 // beta1
# define NTL__CXX_AUTORET
# define NTL__CXX_TYPEOF
#endif
#if _MSC_FULL_VER >= 160021003 // beta2
# define NTL__CXX_NULLPTR
# define NTL__CXX_RVFIX
#endif

#define NTL__CXX_ASSERT
#define NTL__CXX_LAMBDA

#define NTL__CXX_RV
#endif // _MSC_VER >= 1600

#elif defined(__BCPLUSPLUS__)

/** BCB's partial C++0x support */
#define NTL__CXX_ATTRIBUTES
#define NTL__CXX_ALIGNOF
#define NTL__CXX_CHARS
#define NTL__CXX_TYPEOF
#define NTL__CXX_DECLTYPE
#define NTL__CXX_ENUM
#define NTL__CXX_ASSERT

#define NTL__CXX_EXTPL
#define NTL__CXX_RV

#elif defined(__ICL)

#if __ICL >= 1100 && !defined(_MSC_EXTENSIONS)
// detecting -std=C++0x presence^^^

/** ICL's partial C++0x support */

#define NTL__CXX_AUTO
#define NTL__CXX_TYPEOF
#define NTL__CXX_ASSERT
#define NTL__CXX_EXTPL
#define NTL__CXX_LAMBDA

#if __ICL >= 1200
#define NTL__CXX_FUNC
#define NTL__CXX_EF
#define NTL__CXX_ENUM
#define NTL__CXX_TYPEOF
#define NTL__CXX_RV
#define NTL__CXX_RVFIX
#endif

#endif // __ICL

#elif defined(__GNUC__) && defined(__GXX_EXPERIMENTAL_CXX0X__)

/** GCC's partial C++0x support */

#define NTL__CXX_AUTO
#define NTL__CXX_AUTORET
#define NTL__CXX_TYPEOF
#define NTL__CXX_ENUM
#define NTL__CXX_ASSERT
#define NTL__CXX_EF
#define NTL__CXX_EXTPL
#define NTL__CXX_IL
#define NTL__CXX_RV
#define NTL__CXX_VT
#define NTL__CXX_FUNC

#if((__GNUC__*10+__GNUC_MINOR__) >= 45)
# define NTL__CXX_EXPLICITOP	// 4.5
# define NTL__CXX_LAMBDA		// 4.5
#endif // gcc 4.5+

#endif // compilers set
#endif // legacy compilers

#endif // NTL__BASECONF_CPP0X
