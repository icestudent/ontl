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

#ifndef STATIC_ASSERT
#define STATIC_ASSERT(e) typedef char _STATIC_ASSERT_ ## __COUNTER__ [(e)?1:-1]
#endif

#ifndef static_assert
#define static_assert(e, Msg) STATIC_ASSERT(e)
#endif

#if defined(NTL__STLX_FORCE_CDECL) && !defined(NTL__CRTCALL)
  #ifdef _MSC_VER
    #define NTL__CRTCALL __cdecl
  #else
    #error unsupported compiler
  #endif
#else
  #define NTL__CRTCALL
#endif

#ifndef NTL__EXTERNAPI
  #ifdef  _MSC_VER
    #define NTL__EXTERNAPI extern "C" __declspec(dllimport)
  #else
    #error usupported compiler
  #endif
#endif

#ifndef alignas
  #ifdef _MSC_VER
    #define alignas(X) __declspec(align(X))
  #else
    #error unsupported compiler
  #endif
#endif

#ifndef alignof
  #ifdef _MSC_VER
  #if _MSC_VER <= 1500
    #define alignof(X) __alignof(X)
  #endif
  #else
    #error unsupported compiler
  #endif
#endif
static_assert(alignof(int)==alignof(unsigned int), "wierd platform");

#ifndef __align
  #define __align(X) __declspec(align(X))
#endif

#if __cplusplus <= 199711L
  #define constexpr
#endif

namespace std {

/**\defgroup  lib_language_support ***** Language support library [18] ******
 *@{*/

#pragma warning(push)
#pragma warning(disable:4324)
typedef alignas(8192) struct {} max_align_t;
#pragma warning(pop)

/**\defgroup  lib_support_types ******** Types [18.1] ***********************
 *@{*/

//based on SC22/WG21/N1601 J16/04-0041
typedef struct 
{
    template<typename any> operator any * () const { return 0; }
    template<class any, typename T> operator T any:: * () const { return 0; }
  private:
  //  nullptr_t();// {}
  //  nullptr_t(const nullptr_t&);
  //  void operator = (const nullptr_t&);
    void operator &() const;
    template<typename any> void operator +(any) const { I Love MSVC 2005! }
    template<typename any> void operator -(any) const { I Love MSVC 2005! }
//    void * _;
} nullptr_t;
//STATIC_ASSERT(sizeof(nullptr_t)==sizeof(void*));
static const nullptr_t nullptr = {};

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
  typedef          int  ptrdiff_t;
#ifdef _MSC_VER
  using ::size_t;
#else
  typedef unsigned int  size_t;
#endif
  typedef          int  ssize_t;
#endif

#ifndef offsetof
  #define offsetof(s,m) (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))
#endif

#define __ntl_bitmask_type(bitmask, _F)\
  _F bitmask operator&(bitmask x, bitmask y) { return static_cast<bitmask>(x&y); }\
  _F bitmask operator|(bitmask x, bitmask y) { return static_cast<bitmask>(x|y); }\
  _F bitmask operator^(bitmask x, bitmask y) { return static_cast<bitmask>(x^y); }\
  _F bitmask operator~(bitmask x) { return static_cast<bitmask >(~x); }\
  _F bitmask& operator&=(bitmask& x, bitmask y) { x = x&y ; return x ; }\
  _F bitmask& operator|=(bitmask& x, bitmask y) { x = x|y ; return x ; }\
  _F bitmask& operator^=(bitmask& x, bitmask y) { x = x^y ; return x ; }


/**@} lib_support_types */
/**@} lib_language_support */


} //namespace std

#endif //#ifndef NTL__STLX_CSTDDEF