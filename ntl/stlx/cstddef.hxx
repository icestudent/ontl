/**\file*********************************************************************
 *                                                                     \brief
 *  18.1 Types [lib.support.types]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CSTDDEF
#define NTL__STLX_CSTDDEF

#ifdef _MSC_VER
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
#define STATIC_ASSERT(e) typedef char _STATIC_ASSERT_AT_##line[(e)?1:-1]
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

#ifndef __align
#define __align(X) __declspec(align(X))
#endif

namespace std {

/**\defgroup  lib_language_support ***** Language support library [18] ******
 *@{*/
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
    void operator & ();
    template<typename any> void operator +(any) const { I Love MSVC 2005! }
    template<typename any> void operator -(any) const { I Love MSVC 2005! }
//    void * _;
} nullptr_t;
//STATIC_ASSERT(sizeof(nullptr_t)==sizeof(void*));
static const nullptr_t nullptr = {};

#ifndef NULL
#define NULL 0
#endif

#ifdef _WIN64
typedef          long long  ptrdiff_t;
typedef unsigned long long  size_t;
typedef          long long  ssize_t;
#else
typedef          int  ptrdiff_t;
typedef unsigned int  size_t;
typedef          int  ssize_t;
#endif

#ifndef offsetof
#define offsetof(s,m) ((size_t)&((const char&)(((s *)0)->m)))
#endif

/**@} lib_support_types */
/**@} lib_language_support */

}//namespace std

#endif//#ifndef NTL__STLX_CSTDDEF
