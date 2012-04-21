/**\file*********************************************************************
 *                                                                     \brief
 *  Partial emulation of the C++0x features
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_EXT_CPP0XMODE
#define NTL__STLX_EXT_CPP0XMODE
#pragma once


/************************************************************************/
/* Emulation                                                            */
/************************************************************************/
#ifndef NTL_CXX // __cplusplus <= 199711L

  #ifndef NTL_CXX_ASSERT
    #define static_assert(e, Msg) STATIC_ASSERT(e)
  #else
    #undef STATIC_ASSERT
    #define STATIC_ASSERT(e) static_assert(e, #e)
  #endif

  #ifndef NTL_CXX_ALIGNAS
    #ifdef _MSC_VER
      #define alignas(X) __declspec(align(X))
    #else
      #define alignas(X)
    #endif
  #endif

  #ifndef NTL_CXX_ALIGNOF
    #if defined(__ICL) || _MSC_VER <= 1700
      #define alignof(X) __alignof(X)
    #endif
  #endif
  static_assert(alignof(int)==alignof(unsigned int), "wierd platform");

  #ifndef NTL_CXX_FUNC
    #ifdef _MSC_VER
      #define __func__ __FUNCTION__
    #else
      #define __func__ __FUNC__
    #endif
  #endif

  #ifndef __name__
    #ifdef _MSC_VER
      #define __name__ __FUNCTION__
    #else
      #define __name__ __FUNC__
    #endif
  #endif

  #ifndef NTL_CXX_CONSTEXPR
    #define constexpr
  #endif

#endif //__cplusplus <= 199711L

/************************************************************************/
/* Common declarations                                                  */
/************************************************************************/

#ifdef NTL_CXX_ATTRIBUTES
# define __noreturn [[noreturn]]
#else
  // __declspec-aware compilers only
# define __noreturn __declspec(noreturn)
#endif

// explicit function definition
#ifdef NTL_CXX_EF
  #define __deleted = delete
  #define __default = default
#else
  #define __deleted
  #define __default
#endif

#ifdef NTL_CXX_EXPLICITOP
# define __explicit_operator_bool() explicit operator bool()
# define __explicit_bool(...) __VA_ARGS__
#else
# define __explicit_operator_bool() operator ntl::explicit_bool_type()
# define __explicit_bool(...) ntl::explicit_bool(__VA_ARGS__)
#endif

namespace ntl
{
  struct explicit_bool_t { void _(){} };
  typedef void (explicit_bool_t::* explicit_bool_type)(); // pointer-to-member function instead of pointer-to-member because the pointer to second form have -1 base and 0/NULL is valid value.

  template<typename T>
  __forceinline
  explicit_bool_type explicit_bool(T cond)
  {
    return *reinterpret_cast<explicit_bool_type*>(&cond);
  }

  __forceinline
  explicit_bool_type explicit_bool(bool cond)
  {
    return cond ? &explicit_bool_t::_ : 0;
  }
}

// null pointer constant
namespace std
{
#if !defined(NTL_CXX_NULLPTR) && !defined(NTL_DOC)
  //based on SC22/WG21/N2431 = J16/07-0301
  struct nullptr_t
  {
    template<typename any> operator any * () const { return 0; }
    template<class any, typename T> operator T any:: * () const { return 0; }
#ifdef NTL_CXX_EXPLICITOP
    explicit operator bool() const { return false; }
#else
    operator ntl::explicit_bool_type() const { return ntl::explicit_bool(false); }
#endif

#ifdef _MSC_VER
    struct pad {};
    pad __[sizeof(void*)/sizeof(pad)]; //-V514
#else
    char __[sizeof(void*)];
#endif
  private:
    //  nullptr_t();// {}
    //  nullptr_t(const nullptr_t&);
    //  void operator = (const nullptr_t&);
    void operator &() const;
    template<typename any> void operator +(any) const { /*I Love MSVC 2005!*/ }
    template<typename any> void operator -(any) const { /*I Love MSVC 2005!*/ }

  };
  static const nullptr_t __nullptr = {};

#ifndef nullptr
#define nullptr std::__nullptr
#endif

  static_assert(sizeof(nullptr)==sizeof(void*), "3.9.1.10: sizeof(std::nullptr_t) shall be equal to sizeof(void*)");

#endif // nullptr
}

#include "move.hxx"

#endif // NTL__STLX_EXT_CPP0XMODE
