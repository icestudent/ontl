/**\file*********************************************************************
 *                                                                     \brief
 *  18.1 Types [lib.support.types]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CSTDDEF
#define NTL__STLX_CSTDDEF

// supported compilers
#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
// MS VC++
  #if _MSC_VER < 1400
  # error upgrade your compiler at least to VC8 (14.00)
  #endif
#elif defined(__BCPLUSPLUS__)
// Borland C++
  #if __BCPLUSPLUS__ < 0x600
  # error upgrade your compiler at least to BCB 2009 (12.00)
  #endif
#elif defined(__INTEL_COMPILER)
  #ifndef __ICL
  # define __ICL __INTEL_COMPILER
  #endif
#elif defined(__GNUC__)
// GCC
  #if !(__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 2)
  # error upgrade GCC at least to the GCC 4.2
  #endif
#else
# pragma message("Unknown compiler, it is unsupported probably. Sorry, mate")
#endif


#if defined(_MSC_VER)
# pragma warning(disable:4514)// unreferenced inline function has been removed
# pragma warning(disable:4480)// nonstandard extension used: specifying underlying type for enum 'enum'
# pragma runtime_checks( "", off )
#elif defined(__GNUC__)
# define __forceinline __attribute__((always_inline))
# define __assume(X)
#else
# define __forceinline inline
# define __assume(X)
#endif

#define __optional

#if defined(_MSC_VER) && _MSC_VER >= 1400
//#define __thiscall __thiscall
//#define __restrict __restrict
#else
#define __restrict
#define __thiscall
#endif

#if defined(_MSC_VER)
# define __noalias __declspec(noalias)
#elif defined(__BCPLUSPLUS__) || defined(__GNUC__)
# define __noalias
#endif

#define _PasteToken(x,y) x##y
#define _Join(x,y) _PasteToken(x,y)

#ifndef STATIC_ASSERT
  #if defined(__GNUC__)
    #if (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 3)
      // !fr3@K!
      // GCC specific note: __COUNTER__ is not supported prior to 4.3
      #define STATIC_ASSERT(e) typedef char _Join(_STATIC_ASSERT_, __COUNTER__) [(e)?1:-1]
    #else
      #define STATIC_ASSERT(e) typedef char _Join(_STATIC_ASSERT_, __LINE__) [(e)?1:-1]
    #endif
  #else
    #define STATIC_ASSERT(e) typedef char _Join(_STATIC_ASSERT_, __COUNTER__) [(e)?1:-1]
  #endif
#endif

#if defined(__BCPLUSPLUS__) || defined(__GNUC__)
# define __w64
#endif

#ifndef __GNUC__
  #ifdef _M_X64
  # define __SIZEOF_POINTER__ 8
  #else
  # define __SIZEOF_POINTER__ 4
  #endif // x64
#endif // __GNUC__

#ifdef __GNUC__
 #define _cdecl    __attribute__((cdecl))
 #define _stdcall  __attribute__((stdcall))
 #define _fastcall __attribute__((fastcall))
 #define __cdecl    _cdecl
 #define __stdcall  _stdcall
 #define __fastcall _fastcall
#endif

#ifndef __GNUC__
 #define NTL__NORETURN __declspec(noreturn)
#else
 #define NTL__NORETURN __attribute__((noreturn))
#endif

#ifndef NTL__CRTCALL
# ifdef NTL__STLX_FORCE_CDECL
  #ifdef _MSC_VER
    #define NTL__CRTCALL __cdecl
  #elif defined(__BCPLUSPLUS__)
    #define NTL__CRTCALL __cdecl
  #endif
# else
  #define NTL__CRTCALL
# endif
#endif

#define NTL__CRTIMP __cdecl

#ifndef NTL__EXTERNAPI
  #ifdef  _MSC_VER
    #define NTL__EXTERNAPI extern "C" __declspec(dllimport) __declspec(nothrow)
    #define NTL__EXTERNVAR extern "C" __declspec(dllimport)
  #elif defined(__BCPLUSPLUS__)
    #define NTL__EXTERNAPI extern "C" __declspec(dllimport) __declspec(nothrow)
    #define NTL__EXTERNVAR extern "C" __declspec(dllimport)
  #elif defined(__GNUC__)
    #define NTL__EXTERNAPI extern "C"
    #define NTL__EXTERNVAR extern "C"
  #endif
#endif

#if __cplusplus > 199711L
  /// new C++ Standard
  #define NTL__CXX
#endif

#if __cplusplus <= 199711L

#if defined(_MSC_VER) && !defined(__ICL)

#if _MSC_VER >= 1600

/** VC10's partial C++0x support */

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
//#define NTL__CXX_CHARS
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
#elif defined(__BCPLUSPLUS__)

/** BCB's partial C++0x support */

// keywords:
// align (N2798+)
//#define NTL__CXX_ALIGN
// alignas( <= N2723)
//#define NTL__CXX_ALIGNAS
// alignof
#define NTL__CXX_ALIGNOF
// auto
//#define NTL__CXX_AUTO
// char16_t, char32_t
#define NTL__CXX_CHARS
// concepts, concept_map, requires
//#define NTL__CXX_CONCEPT
// constexpr
//#define NTL__CXX_CONSTEXPR
// decltype (typeof)
#define NTL__CXX_TYPEOF
// class enum
#define NTL__CXX_ENUM
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
// extern templates
#define NTL__CXX_EXTPL
// initializer lists
//#define NTL__CXX_IL
// lambda
//#define NTL__CXX_LAMBDA
// rvalues
#define NTL__CXX_RV
// template typedef
//#define NTL__CXX_TT
// variadic templates (implies rvalue references support)
//#define NTL__CXX_VT
#elif defined(__ICL)

# if __ICL > 1100

// TODO: detect -std=C++0x presence

/** ICL's (East Mill) partial C++0x support */

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
//#define NTL__CXX_CHARS
// concepts, concept_map, requires
//#define NTL__CXX_CONCEPT
// constexpr
//#define NTL__CXX_CONSTEXPR
// decltype (typeof)
#define NTL__CXX_TYPEOF
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
// extern templates
#define NTL__CXX_EXTPL
// initializer lists
//#define NTL__CXX_IL
// lambda
#define NTL__CXX_LAMBDA
// rvalues
//#define NTL__CXX_RV
// template typedef
//#define NTL__CXX_TT
// variadic templates (implies rvalue references support)
//#define NTL__CXX_VT


# endif // __ICL

#elif defined(__GNUC__) && defined(__GXX_EXPERIMENTAL_CXX0X__)

/** GCC's partial C++0x support */

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
//#define NTL__CXX_CHARS
// concepts, concept_map, requires
//#define NTL__CXX_CONCEPT
// constexpr
//#define NTL__CXX_CONSTEXPR
// decltype (typeof)
#define NTL__CXX_TYPEOF
// class enum
#define NTL__CXX_ENUM
// nullptr
//#define NTL__CXX_NULLPTR
// static assert
#define NTL__CXX_ASSERT
// thread_local
//#define NTL__CXX_THREADL

// syntax:
// explicit delete/default function definition
#define NTL__CXX_EF
// explicit conversion operators
//#define NTL__CXX_EXPLICITOP
// extern templates
#define NTL__CXX_EXTPL
// initializer lists
#define NTL__CXX_IL
// lambda
//#define NTL__CXX_LAMBDA
// rvalues
#define NTL__CXX_RV
// template typedef
//#define NTL__CXX_TT
// variadic templates (implies rvalue references support)
#define NTL__CXX_VT

#endif // _MSC_VER

  #ifndef NTL__CXX_ASSERT
    #define static_assert(e, Msg) STATIC_ASSERT(e)
  #else
    #undef STATIC_ASSERT
    #define STATIC_ASSERT(e) static_assert(e, #e)
  #endif

  #ifndef NTL__CXX_ALIGNAS
    #ifdef _MSC_VER
      #define alignas(X) __declspec(align(X))
    #else
      #define alignas(X)
    #endif
  #endif

  #ifndef NTL__CXX_ALIGNOF
    #if defined(__ICL)
      #define alignof(X) __ALIGNOF__(X)
    #elif _MSC_VER <= 1600
      #define alignof(X) __alignof(X)
    #endif
  #endif
  static_assert(alignof(int)==alignof(unsigned int), "wierd platform");

  #ifndef __func__
    #ifdef _MSC_VER
      #define __func__ __FUNCSIG__
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

#endif//__cplusplus <= 199711L


// explicit function definition
#ifdef NTL__CXX_EF
  #define __deleted = delete
  #define __default = default
#else
  #define __deleted
  #define __default
#endif

namespace std {

/**\defgroup  lib_language_support ***** 18 Language support library [language.support] ******
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

/**\defgroup  lib_support_types ******** 18.1 Types [support.types] ***********************
 *@{*/

#ifdef NTL__CXX_NULLPTR
  typedef decltype(nullptr) nullptr_t;
#else

//based on SC22/WG21/N2431 = J16/07-0301
struct nullptr_t
{
    template<typename any> operator any * () const { return 0; }
    template<class any, typename T> operator T any:: * () const { return 0; }

    #ifdef _MSC_VER
    struct pad {};
    pad __[sizeof(void*)/sizeof(pad)];
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
#endif // NTL__CXX
static_assert(sizeof(nullptr)==sizeof(void*), "3.9.1.10: sizeof(std::nullptr_t) shall be equal to sizeof(void*)");

#ifndef NULL
#define NULL 0
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
  // from bcb #define offsetof( s_name, m_name )  (size_t)&(((s_name*)0)->m_name)
#endif

#define __ntl_bitmask_type(bitmask, _func_spec)\
  _func_spec bitmask operator&(bitmask x, bitmask y) { return bitwise_and(x, y); }\
  _func_spec bitmask operator|(bitmask x, bitmask y) { return bitwise_or (x, y); }\
  _func_spec bitmask operator^(bitmask x, bitmask y) { return bitwise_xor(x, y); }\
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
  /**\addtogroup  lib_memory ************* 20.7 Memory [memory]
  *@{*/

  /// 20.7.1 Allocator argument tag [allocator.tag]
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

namespace ntl
{
  struct explicit_bool_t { int _; };
  typedef int explicit_bool_t::*  explicit_bool_type;

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

namespace std { namespace __{ using ntl::explicit_bool; using ntl::explicit_bool_type; } }

#endif //#ifndef NTL__STLX_CSTDDEF
