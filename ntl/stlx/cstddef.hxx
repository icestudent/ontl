/**\file*********************************************************************
 *                                                                     \brief
 *  18.1 Types [lib.support.types]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CSTDDEF
#define NTL__STLX_CSTDDEF

#ifdef _MSC_VER
#pragma warning(disable:4514)// unreferenced inline function has been removed
//#define __forceinline __forceinline
#else
#define __forceinline inline
#endif

#define __optional

#if defined(_MSC_VER) && _MSC_VER >= 1400
//#define __thiscall __thiscall
//#define __restrict __restrict
#else
#define __restrict
#define __thiscall
#endif

#define _PasteToken(x,y) x##y
#define _Join(x,y) _PasteToken(x,y)
#ifndef STATIC_ASSERT
#define STATIC_ASSERT(e) typedef char _Join(_STATIC_ASSERT_, __COUNTER__) [(e)?1:-1]
#endif

#ifndef NTL__CRTCALL
  #ifdef NTL__STLX_FORCE_CDECL
  #ifdef _MSC_VER
    #define NTL__CRTCALL __cdecl
  #else
    #error unsupported compiler
  #endif
#else
  #define NTL__CRTCALL
#endif
#endif

#ifndef NTL__EXTERNAPI
  #ifdef  _MSC_VER
    #define NTL__EXTERNAPI extern "C" __declspec(dllimport) __declspec(nothrow)
    #define NTL__EXTERNVAR extern "C" __declspec(dllimport)
  #else
    #error usupported compiler
  #endif
#endif

#if __cplusplus > 199711L
  /// new C++ Standard
  #define NTL__CXX
#endif

#if __cplusplus <= 199711L

#ifdef _MSC_VER

#if _MSC_VER >= 1600
 
/** partial C++0x support */

// keywords:
// align (N2798+)
//#define NTL__CXX_ALIGN
// alignas( <= N2723)
//#define NTL__CXX_ALIGNAS
// alignof
//#define NTL__CXX_ALIGNOF
// auto
#define NTL__CXX_AUTO
// char16_t, char32_t
//#define NTL__CXX_CHART
// concepts, concept_map, requires
//#define NTL__CXX_CONCEPT
// constexpr
//#define NTL__CXX_CONSTEXPR
// decltype (typeof)
//#define NTL__CXX_TYPEOF
// class enum
//#define NTL__CXX_ENUM
// nullptr
//#define NTL__CXX_NULLPTR
// static assert
#define NTL__CXX_ASSERT
// thread_local
//#define NTL__CXX_THREADL

// syntax:
// explicit delete/default function definition
//#define NTL__CXX_EF
// explicit conversion operators
//#define NTL__CXX_EXPLICITOP
// initializer lists
//#define NTL__CXX_IL
// lambda
#define NTL__CXX_LAMBDA
// rvalues
#define NTL__CXX_RV
// template typedef
//#define NTL__CXX_TT
// variadic templates (implies rvalue references support)
//#define NTL__CXX_VT

#endif // _MSC_VER >= 1600

  #ifndef NTL__CXX_ASSERT
    #define static_assert(e, Msg) STATIC_ASSERT(e)
  #endif

  #ifndef NTL__CXX_ALIGNAS
    #ifdef _MSC_VER
      #define alignas(X) __declspec(align(X))
    #else
      #error unsupported compiler
    #endif
  #endif

  #ifndef NTL__CXX_ALIGNOF
    #if _MSC_VER <= 1600
      #define alignof(X) __alignof(X)
    #endif
  #endif
  static_assert(alignof(int)==alignof(unsigned int), "wierd platform");

    //static const char __func__[];
    #ifndef __func__
      #ifdef _MSC_VER
        //#define __func__ __FUNCDNAME__
        #define __func__ __FUNCSIG__
        //#define __func__ __FUNCTION__
      #else
        #error unsupported compiler
      #endif
    #endif

  #ifndef __align
    #define __align(X) __declspec(align(X))
  #endif

  #ifndef NTL_CXX_CONSTEXPR
    #define constexpr
  #endif
#else
  #error unsupported compiler
#endif // _MSC_VER

#endif//__cplusplus <= 199711L

namespace std {

/**\defgroup  lib_language_support ***** Language support library [18] ******
 *@{*/

#pragma warning(push)
#pragma warning(disable:4324)
typedef alignas(8192) struct {} max_align_t;
#pragma warning(pop)

/**\defgroup  lib_support_types ******** Types [18.1] ***********************
 *@{*/

#ifdef NTL__CXX
  typedef decltype(nullptr) nullptr_t;
#else

//based on SC22/WG21/N2431 = J16/07-0301
struct nullptr_t
{
    template<typename any> operator any * () const { return 0; }
    template<class any, typename T> operator T any:: * () const { return 0; }
    struct pad {};
    pad __[sizeof(void*)/sizeof(pad)];
  private:
  //  nullptr_t();// {}
  //  nullptr_t(const nullptr_t&);
  //  void operator = (const nullptr_t&);
    void operator &() const;
    template<typename any> void operator +(any) const { I Love MSVC 2005! }
    template<typename any> void operator -(any) const { I Love MSVC 2005! }

};
static const nullptr_t __nullptr = {};

  #ifndef nullptr
    #define nullptr std::__nullptr
  #endif
#endif // NTL__CXX
STATIC_ASSERT(sizeof(nullptr)==sizeof(void*));

#ifndef NULL
#define NULL 0
#endif

#ifdef _M_X64
  typedef          long long  ptrdiff_t;
#ifdef _MSC_VER
  using ::size_t;
#else
  typedef unsigned long long  size_t;
#endif
  typedef          long long  ssize_t;
#else
  typedef __w64    int  ptrdiff_t;
#ifdef _MSC_VER
  using ::size_t;
#else
  typedef unsigned int  size_t;
#endif
  typedef __w64    int  ssize_t;
#endif

#ifndef offsetof
  #define offsetof(s,m) (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))
#endif

#define __ntl_bitmask_type(bitmask, _friend)\
  _friend bitmask operator&(bitmask x, bitmask y) { return bitwise_and(x, y); }\
  _friend bitmask operator|(bitmask x, bitmask y) { return bitwise_or (x, y); }\
  _friend bitmask operator^(bitmask x, bitmask y) { return bitwise_xor(x, y); }\
  _friend bitmask operator~(bitmask x) { return static_cast<bitmask>(~static_cast<unsigned>(x)); }\
  _friend bitmask& operator&=(bitmask& x, bitmask y) { x = x&y ; return x ; }\
  _friend bitmask& operator|=(bitmask& x, bitmask y) { x = x|y ; return x ; }\
  _friend bitmask& operator^=(bitmask& x, bitmask y) { x = x^y ; return x ; }

/**@} lib_support_types */
/**@} lib_language_support */


  /**\addtogroup  lib_utilities ********** [20] General utilities library *****
  *@{*/
  /**\addtogroup  lib_memory *************** [20.7] Memory **********************
  *@{*/

  /// 20.7.1 Allocator argument tag [allocator.tag]
  struct allocator_arg_t {/**/};
  const allocator_arg_t allocator_arg;// = allocator_arg_t();
  /**@} lib_memory */
  /**@} lib_utilities */


} //namespace std

#endif //#ifndef NTL__STLX_CSTDDEF
