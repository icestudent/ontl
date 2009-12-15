/**\file*********************************************************************
 *                                                                     \brief
 *  20.6 Metaprogramming and type traits [meta]
 *  MS VC version
 ****************************************************************************
 */
#ifndef NTL__STLX_TYPE_TRAITS_MSVC
#define NTL__STLX_TYPE_TRAITS_MSVC
#pragma once

#ifndef NTL__IN_STLX_TYPE_TRAITS
  #error Always include this file through <type_traits>.
#endif

namespace std {

// 20.6.5 Relationships between types [meta.rel]

template <class T, class U> struct is_same : public false_type {};
template <class T> struct is_same<T, T> : public true_type {};

template <class Base, class Derived> struct is_base_of
: public integral_constant<bool, __is_base_of(Base, Derived)> {};

template <class From, class To> struct is_convertible
: public integral_constant<bool, __is_convertible_to(From, To)> {};


// 20.6.6 Transformations between types [meta.trans]

// 20.6.6.1 Const-volatile modifications [meta.trans.cv]

template <class T> struct remove_const              { typedef T type; };
template <class T> struct remove_const<const T>     { typedef T type; };

template <class T> struct remove_volatile             { typedef T type; };
template <class T> struct remove_volatile<volatile T> { typedef T type; };

template <class T> struct remove_cv                   { typedef T type; };
template <class T> struct remove_cv<const T>          { typedef T type; };
template <class T> struct remove_cv<volatile T>       { typedef T type; };
template <class T> struct remove_cv<volatile const T> { typedef T type; };

template <class T> struct add_const           { typedef const T  type; };
template <class T> struct add_const<T&>       { typedef const T& type; };

template <class T> struct add_volatile  { typedef volatile T type; };

template <class T> struct add_cv { typedef const volatile T type; };

// 20.6.6.2 Reference modifications [meta.trans.ref]

template <class T> struct remove_reference     { typedef T type; };
template <class T> struct remove_reference<T&> { typedef T type; };
# ifdef NTL__CXX_RV
template <class T> struct remove_reference<T&&> { typedef T type; };
# else
template <class T> struct remove_reference<_rvalue<T> > { typedef T type; };
# endif

template <class T> struct add_lvalue_reference     { typedef T& type; };
template <class T> struct add_lvalue_reference<T&> { typedef T& type; };
#ifdef NTL__CXX_RV
template <class T> struct add_rvalue_reference     { typedef T&& type; };
template <class T> struct add_rvalue_reference<T&> { typedef T&& type; };
template <class T> struct add_rvalue_reference<T&&>{ typedef T&& type; };
#else
template <class T> struct add_rvalue_reference     { typedef _rvalue<T> type; };
template <class T> struct add_rvalue_reference<T&> { typedef _rvalue<T> type; };
template <class T> struct add_rvalue_reference<_rvalue<T> > { typedef _rvalue<T> type; };
#endif

template <class T> struct add_reference : public add_lvalue_reference<T> {};

// 20.6.6.3 Sign modifications [meta.trans.sign]
namespace __
{
  template<typename T> struct change_sign;

#define _change_signs(funda) \
  template<> struct change_sign<unsigned funda> { typedef   signed funda type; }; \
  template<> struct change_sign<  signed funda> { typedef unsigned funda type; };

  _change_signs(char)
  _change_signs(short int)
  _change_signs(int)
  _change_signs(long int)
  _change_signs(long long int)
#undef _change_signs

  // signed type by default
  template<size_t SizeOfT> struct map_to_funda;
  template<> struct map_to_funda<0> { typedef void    type; };
  template<> struct map_to_funda<1> { typedef int8_t  type; };
  template<> struct map_to_funda<2> { typedef int16_t type; };
  template<> struct map_to_funda<4> { typedef int32_t type; };
  template<> struct map_to_funda<8> { typedef int64_t type; };

  template<typename enumT>
  struct enum_base
  {
    typedef typename map_to_funda<sizeof(enumT)>::type type;
  };

  template <class T, bool isenum> struct make_signed2
  {
  private:
    static const bool isconst = is_const<T>::value, isvolatile = is_volatile<T>::value;
    typedef typename remove_cv<T>::type U;
    typedef typename conditional<is_signed<U>::value, U, typename __::change_sign<U>::type>::type V;
    typedef typename conditional<isvolatile, typename add_volatile<V>::type, V>::type W;
    typedef typename conditional<isconst, typename add_const<W>::type, W>::type X;
  public:
    typedef X type;
  };

  template <class T, bool isenum> struct make_unsigned2
  {
    static_assert(is_integral<T>::value, "T shall be integral type");
    static_assert((!is_same<T, bool>::value), "T shall not be a bool type");
  private:
    static const bool isconst = is_const<T>::value, isvolatile = is_volatile<T>::value;
    typedef typename remove_cv<T>::type U;
    typedef typename conditional<is_unsigned<U>::value, U, typename __::change_sign<U>::type>::type V;
    typedef typename conditional<isvolatile, typename add_volatile<V>::type, V>::type W;
    typedef typename conditional<isconst, typename add_const<W>::type, W>::type X;
  public:
    typedef X type;
  };

  template<class T> struct make_signed2<T,true>
  {
    typedef typename enum_base<T>::type type;
  };

  template<class T> struct make_unsigned2<T,true>
  {
    typedef typename make_unsigned2<typename enum_base<T>::type, false>::type type;
  };
}

template<class T>
struct make_signed
{
  static_assert(is_integral<T>::value||is_enum<T>::value, "T shall be an integral type or enumeration");
  static_assert((!is_same<T, bool>::value), "T shall not be a bool type");
  
  typedef typename __::make_signed2<T, is_enum<T>::value>::type type;
};

template<class T>
struct make_unsigned
{
  static_assert(is_integral<T>::value||is_enum<T>::value, "T shall be an integral type or enumeration");
  static_assert((!is_same<T, bool>::value), "T shall not be a bool type");

  typedef typename __::make_unsigned2<T, is_enum<T>::value>::type type;
};

template<> struct make_signed  <char> { typedef   signed char type; };
template<> struct make_unsigned<char> { typedef unsigned char type; };

// 20.6.6.4 Array modifications [meta.trans.arr]

template <class T> struct remove_extent { typedef T type; };
template <class T> struct remove_extent<T[]> { typedef T type; };
template <class T, size_t Size> struct remove_extent<T[Size]> { typedef T type; };

template <class T> struct remove_all_extents { typedef T type; };
template <class T> struct remove_all_extents<T[]>
  { typedef typename remove_all_extents<T>::type type; };
template <class T, size_t Size> struct remove_all_extents<T[Size]>
  { typedef typename remove_all_extents<T>::type type; };

// 20.6.6.5 Pointer modifications [meta.trans.ptr]

template <class T> struct remove_pointer     { typedef T type; };
template <class T> struct remove_pointer<T*> { typedef T type; };

template <class T> struct add_pointer     { typedef T* type; };
template <class T> struct add_pointer<T&> { typedef typename remove_reference<T>::type* type; };

// 20.6.7 Other transformations [meta.trans.other]

#pragma warning(push)
#pragma warning(disable:4324) // structure was padded due to __declspec(align())
#pragma warning(disable:4820) // 'X' bytes padding added after data member 'std::aligned_storage<Len,Align>::type::data'

namespace aux {
template <std::size_t Align>struct aligner;
// MSDN: Valid alignment values are integer powers of two from 1 to 8192 (bytes)
template <> struct aligner<1> { __declspec(align(1)) class type {}; };
template <> struct aligner<2> { __declspec(align(2)) class type {}; };
template <> struct aligner<4> { __declspec(align(4)) class type {}; };
template <> struct aligner<8> { __declspec(align(8)) class type {}; };
template <> struct aligner<16> { __declspec(align(16)) class type {}; };
template <> struct aligner<32> { __declspec(align(32)) class type {}; };
template <> struct aligner<64> { __declspec(align(64)) class type {}; };
template <> struct aligner<128> { __declspec(align(128)) class type {}; };
template <> struct aligner<256> { __declspec(align(256)) class type {}; };
template <> struct aligner<512> { __declspec(align(512)) class type {}; };
template <> struct aligner<1024> { __declspec(align(1024)) class type {}; };
template <> struct aligner<4096> { __declspec(align(4096)) class type {}; };
template <> struct aligner<sizeof(max_align_t)> { __declspec(align(8192)) class type {}; };
} // namespace aux

template <std::size_t Len, std::size_t Align>
struct aligned_storage
{
  union type
  {
    private: unsigned char __data[Len];
    typename aux::aligner<1 + ( Align - 1
                             | (Align - 1) >> 1
                             | (Align - 1) >> 2
                             | (Align - 1) >> 3
                             | (Align - 1) >> 4
                             | (Align - 1) >> 5
                             | (Align - 1) >> 6
                             | (Align - 1) >> 7
                             | (Align - 1) >> 8
                             | (Align - 1) >> 9
                             | (Align - 1) >> 10
                             | (Align - 1) >> 11
                             | (Align - 1) >> 12
                             | (Align - 1) >> 13 )
                          >::type __align;
  };
};

#pragma warning(pop)

//
#ifdef NTL__CXX_VT
template <std::size_t Len, class... Types> struct aligned_union;
#endif

template <class T> struct decay;

template <bool Condition, class T> struct enable_if {};
template <class T> struct enable_if<true, T> { typedef T type; };

template <bool, class IfTrueType, class IfFalseType> struct conditional;
template <class T, class F> struct conditional<true, T, F>  { typedef T type; };
template <class T, class F> struct conditional<false, T, F> { typedef F type; };

namespace __
{
  // static logical operators: OR and AND
  // usage:
  // select_or<condition1, condition2, TrueType, FalseType>::type is equal to "if(condition1 || condition2) TrueType; else FalseType;
  // select_and works same, but with the "&&" operation.

  template<bool cond1, bool cond2,
  class IfTrueType, class IfFalseType>
  struct select_or
  { typedef IfFalseType type; };

  template<bool cond2, class T, class F>
  struct select_or<true, cond2, T, F>
  { typedef T type; };

  template<bool cond1, class T, class F>
  struct select_or<cond1, true, T, F>
  { typedef T type; };

  template<class T, class F>
  struct select_or<true, true, T, F>
  { typedef T type; };

  template<bool cond1, bool cond2,
  class IfTrueType, class IfFalseType>
  struct select_and
  { typedef IfFalseType type; };

  template<class T, class F>
  struct select_and<true, true, T, F>
  { typedef T type; };
}

// 20.6.4 Unary Type Traits [meta.unary]

#define NTL__STLX_DEF_TRAIT(trait)\
  template <class T> struct trait : public integral_constant<bool, __##trait(T)> {};
#define NTL__STLX_DEF_TRAIT2(trait,builtin_trait)\
  template <class T> struct trait : public integral_constant<bool, __##builtin_trait(T)> {};

// 20.6.4.1 Primary Type Categories [meta.unary.cat]

template <class T> struct is_void                      : public false_type {};
template <>        struct is_void<void>                : public true_type {};
template <>        struct is_void<const void>          : public true_type {};
template <>        struct is_void<volatile void>       : public true_type {};
template <>        struct is_void<const volatile void> : public true_type {};


template <class T> struct is_array      : public false_type {};
template <class T> struct is_array<T[]> : public true_type  {};
template <class T, size_t N>
                   struct is_array<T[N]>: public true_type  {};

template <class T> struct is_pointer                    : public false_type {};
template <class T> struct is_pointer<T*>                : public true_type {};
template <class T> struct is_pointer<T* const>          : public true_type {};
template <class T> struct is_pointer<T* volatile>       : public true_type {};
template <class T> struct is_pointer<T* const volatile> : public true_type {};

template <class T> struct is_lvalue_reference     : public false_type {};
template <class T> struct is_lvalue_reference<T&> : public true_type {};

template <class T> struct is_rvalue_reference     : public false_type {};
#ifdef NTL__CXX_RV
template <class T> struct is_rvalue_reference<T&&>: public true_type {};
#else
template <class T> struct is_rvalue_reference<_rvalue<T> >: public true_type {};
#endif

template <class T>
struct is_member_object_pointer : public false_type {};

template <class T>
struct is_member_function_pointer : public false_type {};

#undef NTL_TT_TCV

#undef NTL_TT_PCV
#define NTL_TT_PCV
#include "tt_ismemptr.inl"
#undef NTL_TT_PCV
#define NTL_TT_PCV const
#include "tt_ismemptr.inl"
#undef NTL_TT_PCV
#define NTL_TT_PCV volatile
#include "tt_ismemptr.inl"
#undef NTL_TT_PCV
#define NTL_TT_PCV const volatile
#include "tt_ismemptr.inl"

#undef NTL_TT_TCV
#define NTL_TT_TCV const

#undef NTL_TT_PCV
#define NTL_TT_PCV
#include "tt_ismemptr.inl"
#undef NTL_TT_PCV
#define NTL_TT_PCV const
#include "tt_ismemptr.inl"
#undef NTL_TT_PCV
#define NTL_TT_PCV volatile
#include "tt_ismemptr.inl"
#undef NTL_TT_PCV
#define NTL_TT_PCV const volatile
#include "tt_ismemptr.inl"

#undef NTL_TT_TCV
#define NTL_TT_TCV volatile

#undef NTL_TT_PCV
#define NTL_TT_PCV
#include "tt_ismemptr.inl"
#undef NTL_TT_PCV
#define NTL_TT_PCV const
#include "tt_ismemptr.inl"
#undef NTL_TT_PCV
#define NTL_TT_PCV volatile
#include "tt_ismemptr.inl"
#undef NTL_TT_PCV
#define NTL_TT_PCV const volatile
#include "tt_ismemptr.inl"

#undef NTL_TT_TCV
#define NTL_TT_TCV const volatile

#undef NTL_TT_PCV
#define NTL_TT_PCV
#include "tt_ismemptr.inl"
#undef NTL_TT_PCV
#define NTL_TT_PCV const
#include "tt_ismemptr.inl"
#undef NTL_TT_PCV
#define NTL_TT_PCV volatile
#include "tt_ismemptr.inl"
#undef NTL_TT_PCV
#define NTL_TT_PCV const volatile
#include "tt_ismemptr.inl"

#undef NTL_TT_PCV
#undef NTL_TT_TCV

NTL__STLX_DEF_TRAIT(is_enum)

NTL__STLX_DEF_TRAIT(is_union)

NTL__STLX_DEF_TRAIT(is_class)

template <class T>
struct is_function : public false_type {};

#include "tt_isfuncc.inl"

#ifndef _M_X64
#define NTL_TT_CC __cdecl
#include "tt_isfunc.inl"
#undef  NTL_TT_CC
#define NTL_TT_CC __stdcall
#include "tt_isfunc.inl"
#undef  NTL_TT_CC
#define NTL_TT_CC __fastcall
#include "tt_isfunc.inl"
#undef  NTL_TT_CC
#else
#define NTL_TT_CC
#include "tt_isfunc.inl"
#undef  NTL_TT_CC
#endif

namespace __
{
  template<class T> struct is_int_helper
  {
    template<class > struct func_limits { static const bool is_integer = false, is_iec559 = false; };

    typedef typename remove_cv<T>::type U;
    typedef typename conditional<is_function<U>::value, func_limits<U>, numeric_limits<U> >::type type;
  };
}

template <class T> struct is_integral
: public integral_constant<bool, __::is_int_helper<T>::type::is_integer> {};

template <class T> struct is_floating_point
: public integral_constant<bool, __::is_int_helper<T>::type::is_iec559> {};

template <class T> struct decay
{
  private: typedef typename remove_reference<T>::type U;
  public:  typedef
    typename conditional<is_array<U>::value,
                          typename remove_extent<U>::type*,
                          typename conditional<is_function<U>::value,
                                                typename add_pointer<U>::type,
                                                typename remove_cv<U>::type
                                              >::type
                        >::type   type;
};


// 20.6.4.2 Composite type traits [meta.unary.comp]

template <class T> struct is_reference
: public integral_constant<
    bool, is_lvalue_reference<T>::value || is_rvalue_reference<T>::value
    > {};

template <class T> struct is_arithmetic
: public integral_constant<
    bool, is_integral<T>::value || numeric_limits<T>::is_iec559
    > {};

template <class T> struct is_fundamental
: public integral_constant<
    bool, is_arithmetic<T>::value || is_void<T>::value
    > {};

template <class T> struct is_object
: public integral_constant<
    bool, !is_void<T>::value
       && !is_reference<T>::value
       && !is_function<T>::value
       > {};

template <class T> struct is_member_pointer
: public integral_constant<
    bool, is_member_object_pointer<T>::value
       || is_member_function_pointer<T>::value
       > {};

template <class T> struct is_scalar
: public integral_constant<
    bool, is_enum<T>::value
       || is_integral<T>::value
       || is_pointer<T>::value
       || is_member_pointer<T>::value
       > {};

template <class T> struct is_compound
: public integral_constant<
    bool, is_array<T>::value
       || is_function<T>::value
       || is_pointer<T>::value
       || is_reference<T>::value
       || is_class<T>::value
       || is_union<T>::value
       || is_enum<T>::value
       || is_member_pointer<T>::value
       > {};

// 20.6.4.3 Type properties [meta.unary.prop]

template <class T> struct is_const          : public false_type {};
template <class T> struct is_const<const T> : public true_type {};

template <class T> struct is_volatile             : public false_type {};
template <class T> struct is_volatile<volatile T> : public true_type {};

namespace __ {
  template<class T> struct is_unknown_array: public false_type {};
  template<class T> struct is_unknown_array<T[]>: public true_type {};
  template<class T, size_t Size> struct is_unknown_array<T[Size]>: public false_type {};
}

template<class T> struct is_trivial;
template<class T> struct is_standard_layout;

///\warning what about std::pair<int, int> ?
template <class T> struct is_pod
: public integral_constant<
bool, is_scalar<typename remove_extent<T>::type>::value ||
      __is_pod(typename remove_extent<T>::type) // __is_pod will return false on fundamental types
    >
{};

NTL__STLX_DEF_TRAIT(is_empty)

NTL__STLX_DEF_TRAIT(is_polymorphic)

NTL__STLX_DEF_TRAIT(is_abstract)

NTL__STLX_DEF_TRAIT2(has_trivial_default_constructor, has_trivial_constructor)

NTL__STLX_DEF_TRAIT2(has_trivial_copy_constructor, has_trivial_copy)

NTL__STLX_DEF_TRAIT(has_trivial_assign)

NTL__STLX_DEF_TRAIT(has_trivial_destructor)

NTL__STLX_DEF_TRAIT2(has_nothrow_default_constructor, has_nothrow_constructor)

NTL__STLX_DEF_TRAIT2(has_nothrow_copy_constructor, has_nothrow_copy)

NTL__STLX_DEF_TRAIT(has_nothrow_assign)

NTL__STLX_DEF_TRAIT(has_virtual_destructor)

template <class T> struct is_signed
//: public integral_constant<bool, (is_arithmetic<T>::value && T(-1) < T(0))> {};
: public integral_constant<bool, (static_cast<T>(-1) < 0)> {};

#ifndef __ICL
  template <class T> struct is_unsigned
  //: public integral_constant<bool, (is_arithmetic<T>::value && T(-1) > T(0))> {};
  : public integral_constant<bool, (static_cast<T>(-1) > 0)> {};
#else
  template <class T> struct is_unsigned
    : public integral_constant<bool, (is_arithmetic<T>::value && !numeric_limits<T>::is_signed)> {};
  //: public integral_constant<bool, (static_cast<T>(-1) > 0)> {};
#endif

template <class T> struct alignment_of
: public integral_constant<size_t, alignof(T)> {};
template <>        struct alignment_of<void>
: public integral_constant<size_t, 0> {};
template <>        struct alignment_of<const void>
: public integral_constant<size_t, 0> {};
template <>        struct alignment_of<volatile void>
: public integral_constant<size_t, 0> {};
template <>        struct alignment_of<const volatile void>
: public integral_constant<size_t, 0> {};

template <class T> struct rank : public integral_constant<size_t, 0> {};
template <class T> struct rank<T[]>
: public integral_constant<size_t, rank<T>::value + 1> {};
template <class T, size_t Size> struct rank<T[Size]>
: public integral_constant<size_t, rank<T>::value + 1> {};

template <class T, unsigned I> struct extent
: public integral_constant<size_t, 0> {};
template <class T, unsigned I> struct extent<T[], I>
: public integral_constant<size_t, extent<T, I-1>::value> {};
template <class T, unsigned I, size_t Size> struct extent<T[Size], I>
: public integral_constant<size_t, extent<T, I-1>::value> {};
template <class T, size_t Size> struct extent<T[Size], 0>
: public integral_constant<size_t, Size> {};

template<class T> struct is_trivial
: public integral_constant<
bool,( has_trivial_default_constructor<T>::value &&
       has_trivial_copy_constructor<T>::value &&
       has_trivial_assign<T>::value &&
       has_trivial_destructor<T>::value
     )                    >
{};


// 20.6.7.2 Other transformations [meta.trans.other]
#if defined(NTL__CXX_VT) && defined(NTL__CXX_TYPEOF)

template <class ...T> struct common_type;

template <class T>
struct common_type<T> { typedef T type; };

template <class T, class U>
struct common_type<T, U>
{
private:
  static T&& __t();
  static U&& __u();
public:
  typedef decltype(true ? __t() : __u()) type;
};

template <class T, class U, class... V>
struct common_type<T, U, V...>
{
  typedef typename common_type<typename common_type<T, U>::type, V...>::type type;
};

#else // NTL__CXX_VT

template<class T, class U, class V, class W>
struct common_type;

template <class T>
struct common_type<T, void, void, void>
{
  static_assert(sizeof(T) > 0, "T shall be complete");
  typedef T type;
};

namespace __
{
  template<typename T1, typename T2> struct select_type
  {
  private:
  #ifdef NTL__CXX_RV
    static T1&& __t();
    static T2&& __u();
  #else
    static T1 __t();
    static T2 __u();
  #endif

    static char test(T1);
    static int  test(T2);
  public:
    typedef typename conditional<sizeof(test(true ? __t() : __u())) == sizeof(char), T1, T2>::type type;
  };
  template<typename T1> struct select_type<T1,T1> { typedef T1 type; };

}

template<class T, class U>
struct common_type<T, U, void, void>
{
  static_assert(sizeof(T) > 0, "T shall be complete");
  static_assert(sizeof(T) > 0, "U shall be complete");

private:
#ifdef NTL__CXX_RV
  static T&& __t();
  static U&& __u();
#else
  static T __t();
  static U __u();
#endif

public:
#ifdef NTL__CXX_TYPEOF
  typedef decltype(true ? __t() : __u()) type;
#else
  typedef typename __::select_type<T,U>::type type;
#endif // typeof

};

template<class T, class U, class V>
struct common_type<T, U, V, void>
{
  typedef typename common_type<typename common_type<T, U>::type, V>::type type;
};

template<class T, class U, class V, class W>
struct common_type
{
  typedef typename common_type<typename common_type<T, U, V>::type, W>::type type;
};

#endif // NTL__CXX_VT

// N2947
// NOTE: there are few limitations: enum_base determines the base type calculating size of enumeration type, 
// so it cannot determine the char type (it reports [un]signed char) and the int type (it reports [un]signed long int).
template<typename T> 
struct enum_base
{
  static_assert(is_enum<T>::value, "T shall be an enumeration type");
private:
  typedef typename __::map_to_funda<sizeof(T)>::type U;
  typedef conditional<is_unsigned<T>::value, typename make_unsigned<U>::type, U> V;
public:
  typedef typename V::type type;
};

}//namespace std

#endif//#ifndef NTL__STLX_TYPE_TRAITS_MSVC
