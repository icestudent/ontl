/**\file*********************************************************************
 *                                                                     \brief
 *  20.6 Metaprogramming and type traits [meta]
 *  C++ builder version
 ****************************************************************************
 */
#ifndef NTL__STLX_TYPE_TRAITS_BCB
#define NTL__STLX_TYPE_TRAITS_BCB
#pragma once

#ifndef NTL__IN_STLX_TYPE_TRAITS
  #error Always include this file through <type_traits>.
#endif

namespace std {

// 20.6.5 Relationships between types [meta.rel]

template <class T, class U> struct is_same : false_type {};
template <class T> struct is_same<T, T> : true_type {};

template <class Base, class Derived> struct is_base_of
: public integral_constant<bool, __is_base_of(Base, Derived)> {};

template <class From, class To> struct is_convertible
: public integral_constant<bool, __is_convertible(From, To)> {};


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

template <class T> struct add_const           { typedef const T type; };

template <class T> struct add_volatile  { typedef volatile T type; };

template <class T> struct add_cv { typedef const volatile T type; };

// 20.6.6.2 Reference modifications [meta.trans.ref]

template <class T> struct remove_reference     { typedef T type; };
template <class T> struct remove_reference<T&> { typedef T type; };
#ifdef NTL_CXX_RV
template <class T> struct remove_reference<T&&> { typedef T type; };
#endif

template <class T> struct add_lvalue_reference     { typedef T& type; };
template <class T> struct add_lvalue_reference<T&> { typedef T& type; };

template <class T> struct add_reference : add_lvalue_reference<T> {};

// 20.6.6.3 Sign modifications [meta.trans.sign]
template <class T> struct make_signed;
template <class T> struct make_unsigned;

// 20.6.6.4 Array modifications [meta.trans.arr]

template <class T> struct remove_extent { typedef T type; };
template <class T, size_t Size> struct remove_extent<T[Size]> { typedef T type; };
template <class T, size_t Size> struct remove_extent<const T[Size]> { typedef const T type; };
template <class T, size_t Size> struct remove_extent<volatile T[Size]> { typedef volatile T type; };
template <class T, size_t Size> struct remove_extent<const volatile T[Size]> { typedef const volatile T type; };
////
template <class T> struct remove_all_extents { typedef T type; };
//template <class T> struct remove_all_extents<T[]>
//  { typedef typename remove_all_extents<T>::type type; };
template <class T, size_t Size> struct remove_all_extents<T[Size]>
  { typedef typename remove_all_extents<T>::type type; };

//
// 20.6.6.5 Pointer modifications [meta.trans.ptr]

template <class T> struct remove_pointer     { typedef T type; };
template <class T> struct remove_pointer<T*> { typedef T type; };

template <class T> struct add_pointer     { typedef T* type; };
template <class T> struct add_pointer<T&> { typedef typename remove_reference<T>::type* type; };

// 20.6.7 Other transformations [meta.trans.other]

namespace __ { namespace aux {
#define _FITS(ty) Align == alignof(ty)
#define _NEXT_ALIGN(ty) typedef typename _Aligned<_Len, Align, ty, _FITS(ty)>::_Type _Type

template<class _Ty, size_t _Len> union _Align_type
  { // union with size _Len bytes and alignment of _Ty
  _Ty _Val;
  char _Pad[_Len];
  };

template<size_t _Len, size_t Align, class _Ty, bool _Ok>
  struct _Aligned;
template<size_t _Len, size_t Align, class _Ty>
  struct _Aligned<_Len, Align, _Ty, true>
  { // define type with size _Len and alignment _Ty
  typedef _Align_type<_Ty, _Len> _Type;
  };
template<size_t _Len, size_t Align>
  struct _Aligned<_Len, Align, long, false>
  { // define type with size _Len and alignment _Ty
  typedef _Align_type<double, _Len> _Type;
  };
template<size_t _Len, size_t Align>
  struct _Aligned<_Len, Align, int, false>
  { // define type with size _Len and alignment _Ty
  _NEXT_ALIGN(long);
  };
template<size_t _Len, size_t Align>
  struct _Aligned<_Len, Align, short, false>
  { // define type with size _Len and alignment _Ty
  _NEXT_ALIGN(int);
  };
template<size_t _Len, size_t Align>
  struct _Aligned<_Len, Align, char, false>
  { // define type with size _Len and alignment _Ty
  _NEXT_ALIGN(short);
  };
#undef _NEXT_ALIGN
}} // __

template <std::size_t Len, std::size_t Align>
struct aligned_storage
{
  typedef typename __::aux::_Aligned<Len, Align, char, _FITS(char)>::_Type type;
};
#undef _FITS

////
template <class T> struct decay;

template <bool, class T = void> struct enable_if {};
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
NTL__STLX_DEF_TRAIT(is_void)

NTL__STLX_DEF_TRAIT(is_integral)

NTL__STLX_DEF_TRAIT(is_floating_point)

NTL__STLX_DEF_TRAIT(is_array)

NTL__STLX_DEF_TRAIT(is_pointer)

NTL__STLX_DEF_TRAIT(is_lvalue_reference)

NTL__STLX_DEF_TRAIT(is_rvalue_reference)

NTL__STLX_DEF_TRAIT(is_member_object_pointer)
NTL__STLX_DEF_TRAIT(is_member_function_pointer)

namespace test__impl {
struct has_members
{
  //int m;
  //void f(int);
          };
}//namespace test__impl

NTL__STLX_DEF_TRAIT(is_enum)

NTL__STLX_DEF_TRAIT(is_union)

NTL__STLX_DEF_TRAIT(is_class)

NTL__STLX_DEF_TRAIT(is_function)

// decay implementation
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

NTL__STLX_DEF_TRAIT(is_reference)

NTL__STLX_DEF_TRAIT(is_arithmetic)

NTL__STLX_DEF_TRAIT(is_fundamental)

NTL__STLX_DEF_TRAIT(is_object)

NTL__STLX_DEF_TRAIT(is_member_pointer)

NTL__STLX_DEF_TRAIT(is_scalar)

NTL__STLX_DEF_TRAIT(is_compound)

// 20.6.4.3 Type properties [meta.unary.prop]

NTL__STLX_DEF_TRAIT(is_const)

NTL__STLX_DEF_TRAIT(is_volatile)

NTL__STLX_DEF_TRAIT(is_trivial);
NTL__STLX_DEF_TRAIT(is_standard_layout);

NTL__STLX_DEF_TRAIT(is_pod)

NTL__STLX_DEF_TRAIT(is_empty)

NTL__STLX_DEF_TRAIT(is_polymorphic)

NTL__STLX_DEF_TRAIT(is_abstract)

NTL__STLX_DEF_TRAIT(has_trivial_default_constructor)

NTL__STLX_DEF_TRAIT(has_trivial_copy_constructor)

NTL__STLX_DEF_TRAIT(has_trivial_assign)

NTL__STLX_DEF_TRAIT(has_trivial_destructor)

NTL__STLX_DEF_TRAIT(has_nothrow_default_constructor)

NTL__STLX_DEF_TRAIT(has_nothrow_copy_constructor)

NTL__STLX_DEF_TRAIT(has_nothrow_assign)

NTL__STLX_DEF_TRAIT(has_virtual_destructor)

NTL__STLX_DEF_TRAIT(is_signed)

NTL__STLX_DEF_TRAIT(is_unsigned)

namespace __ { namespace aux {
  template<typename T> struct Alignof_wrap { static const size_t value = alignof(T); };

  template<size_t N> struct Alignof_wrap2 {
    typedef integral_constant<size_t, N> type;
  };
}}
template <class T> struct alignment_of: __::aux::Alignof_wrap2<__::aux::Alignof_wrap<T>::value>::type {};
template<> struct alignment_of<void>: integral_constant<size_t, 0>{};

NTL__STLX_DEF_TRAIT2(rank, array_rank)
//
template <class T, unsigned I = 0> struct extent: public integral_constant<size_t, __array_extent(T, I)> {};


// 20.6.7.2 Other transformations [meta.trans.other]
#ifdef NTL_CXX_VT

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

#else // NTL_CXX_VT

template<class T, class U = void, class V = void, class W = void>
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
#ifdef NTL_CXX_RV
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
#ifdef NTL_CXX_RV
  static T&& __t();
  static U&& __u();
#else
  static T __t();
  static U __u();
#endif

public:
#ifdef NTL_CXX_TYPEOF
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

#endif

}//namespace std

#endif//#ifndef NTL__STLX_TYPE_TRAITS_BCB
