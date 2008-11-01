/**\file*********************************************************************
 *                                                                     \brief
 *  20.5 Metaprogramming and type traits [meta]
 *  C++ builder version
 ****************************************************************************
 */

#include "cstddef.hxx"
#include "limits.hxx"

#ifndef NTL__STLX_TYPE_TRAITS_BCB
#define NTL__STLX_TYPE_TRAITS_BCB

namespace std {

/// 20.5.3 Helper classes [meta.help]
template <class T, T v>
struct integral_constant
{
  static const T                  value = v;
  typedef T                       value_type;
  typedef integral_constant<T, v> type;
};

typedef integral_constant<bool, true>   true_type;
typedef integral_constant<bool, false>  false_type;


// 20.5.5 Relationships between types [meta.rel]

template <class T, class U> struct is_same : false_type {};
template <class T> struct is_same<T, T> : true_type {};
_CHECK_TRAIT((is_same<volatile int, volatile int>::value));

template <class Base, class Derived> struct is_base_of
: public integral_constant<bool, __is_base_of(Base, Derived)> {};

template <class From, class To> struct is_convertible
: public integral_constant<bool, __is_convertible(From, To)> {};


// 20.5.6 Transformations between types [meta.trans]

// 20.5.6.1 Const-volatile modifications [meta.trans.cv]

template <class T> struct remove_const              { typedef T type; };
template <class T> struct remove_const<const T>     { typedef T type; };
_CHECK_TRAIT((is_same<remove_const<volatile const int>::type, volatile int>::value));
_CHECK_TRAIT((is_same<remove_const<const int>::type, const int>::value) == 0);

template <class T> struct remove_volatile             { typedef T type; };
template <class T> struct remove_volatile<volatile T> { typedef T type; };
_CHECK_TRAIT((is_same<remove_volatile<volatile int>::type, volatile int>::value) == 0);
_CHECK_TRAIT((is_same<remove_volatile<volatile const int>::type, const int>::value));

template <class T> struct remove_cv                   { typedef T type; };
template <class T> struct remove_cv<const T>          { typedef T type; };
template <class T> struct remove_cv<volatile T>       { typedef T type; };
template <class T> struct remove_cv<volatile const T> { typedef T type; };
_CHECK_TRAIT((is_same<remove_cv<const int>::type, const int>::value) == 0);
_CHECK_TRAIT((is_same<remove_cv<volatile int>::type, volatile int>::value) == 0);
_CHECK_TRAIT((is_same<remove_cv<volatile const int>::type, int>::value));

template <class T> struct add_const           { typedef const T type; };
_CHECK_TRAIT((is_same<add_const<volatile int>::type, volatile const int>::value));
_CHECK_TRAIT((is_same<add_const<int>::type, const int>::value));

template <class T> struct add_volatile  { typedef volatile T type; };
_CHECK_TRAIT((is_same<add_volatile<const int>::type, volatile const int>::value));
_CHECK_TRAIT((is_same<add_volatile<int>::type, volatile int>::value));

template <class T> struct add_cv { typedef const volatile T type; };
_CHECK_TRAIT((is_same<add_cv<int>::type, volatile const int>::value));

// 20.5.6.2 Reference modifications [meta.trans.ref]

template <class T> struct remove_reference     { typedef T type; };
template <class T> struct remove_reference<T&> { typedef T type; };
#ifdef NTL__CXX_RV
template <class T> struct remove_reference<T&&> { typedef T type; };
_CHECK_TRAIT((is_same<remove_reference<int&>::type, int&>::value) == 0);
_CHECK_TRAIT((is_same<remove_reference<int&>::type, int>::value));
#endif

template <class T> struct add_lvalue_reference     { typedef T& type; };
template <class T> struct add_lvalue_reference<T&> { typedef T& type; };
_CHECK_TRAIT((is_same<add_lvalue_reference<int&>::type, int&>::value));
_CHECK_TRAIT((is_same<add_lvalue_reference<int>::type, int&>::value));

template <class T> struct add_reference : add_lvalue_reference<T> {};

// 20.5.6.3 Sign modifications [meta.trans.sign]
template <class T> struct make_signed;
template <class T> struct make_unsigned;

// 20.5.6.4 Array modifications [meta.trans.arr]

template <class T> struct remove_extent { typedef T type; };
template <class T, size_t Size> struct remove_extent<T[Size]> { typedef T type; };
template <class T, size_t Size> struct remove_extent<const T[Size]> { typedef const T type; };
template <class T, size_t Size> struct remove_extent<volatile T[Size]> { typedef volatile T type; };
template <class T, size_t Size> struct remove_extent<const volatile T[Size]> { typedef const volatile T type; };
//_CHECK_TRAIT((is_same<remove_extent<int[][3]>::type, int[3]>::value));
_CHECK_TRAIT((is_same<remove_extent<const int>::type, const int>::value));
//_CHECK_TRAIT((is_same<remove_extent<volatile int[]>::type, volatile int>::value));
_CHECK_TRAIT((is_same<remove_extent<const volatile int[2]>::type, const volatile int>::value));

template <class T> struct remove_all_extents { typedef T type; };
//template <class T> struct remove_all_extents<T[]>
//  { typedef typename remove_all_extents<T>::type type; };
template <class T, size_t Size> struct remove_all_extents<T[Size]>
  { typedef typename remove_all_extents<T>::type type; };

_CHECK_TRAIT((is_same<remove_all_extents<int>::type, int>::value));
_CHECK_TRAIT((is_same<remove_all_extents<int[2]>::type, int>::value));
_CHECK_TRAIT((is_same<remove_all_extents<int[2][3]>::type, int>::value));
//_CHECK_TRAIT((is_same<remove_all_extents<int[][3][6]>::type, int>::value));

// 20.5.6.5 Pointer modifications [meta.trans.ptr]

template <class T> struct remove_pointer     { typedef T type; };
template <class T> struct remove_pointer<T*> { typedef T type; };
_CHECK_TRAIT((is_same<remove_pointer<const int*>::type, const int>::value));
_CHECK_TRAIT((is_same<remove_pointer<int*>::type, int*>::value) == 0);

template <class T> struct add_pointer     { typedef T* type; };
template <class T> struct add_pointer<T&> { typedef typename remove_reference<T>::type* type; };
_CHECK_TRAIT((is_same<add_pointer<volatile int>::type, volatile int*>::value));
_CHECK_TRAIT((is_same<add_pointer<int*&>::type, int**>::value));

// 20.5.7 Other transformations [meta.trans.other]

namespace __ { namespace aux {
#define _FITS(ty) Align == alignof(ty)
#define _NEXT_ALIGN(ty) typedef typename _Aligned<_Len, Align, ty, _FITS(ty)>::_Type _Type

template<class _Ty, size_t _Len> union _Align_type
	{	// union with size _Len bytes and alignment of _Ty
	_Ty _Val;
	char _Pad[_Len];
	};

template<size_t _Len, size_t Align, class _Ty, bool _Ok>
	struct _Aligned;
template<size_t _Len, size_t Align, class _Ty>
	struct _Aligned<_Len, Align, _Ty, true>
	{	// define type with size _Len and alignment _Ty
	typedef _Align_type<_Ty, _Len> _Type;
	};
template<size_t _Len, size_t Align>
	struct _Aligned<_Len, Align, long, false>
	{	// define type with size _Len and alignment _Ty
	typedef _Align_type<double, _Len> _Type;
	};
template<size_t _Len, size_t Align>
	struct _Aligned<_Len, Align, int, false>
	{	// define type with size _Len and alignment _Ty
	_NEXT_ALIGN(long);
	};
template<size_t _Len, size_t Align>
	struct _Aligned<_Len, Align, short, false>
	{	// define type with size _Len and alignment _Ty
	_NEXT_ALIGN(int);
	};
template<size_t _Len, size_t Align>
	struct _Aligned<_Len, Align, char, false>
	{	// define type with size _Len and alignment _Ty
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

//_CHECK_TRAIT(alignof(aligned_storage<5, 8000>::type) == 8192);
_CHECK_TRAIT(sizeof(aligned_storage<2, 4>::type) == 4);
//_CHECK_TRAIT(sizeof(aligned_storage<2, 4>::type) == 2);

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

// 20.5.4 Unary Type Traits [meta.unary]

#define NTL__STLX_DEF_TRAIT(X)\
  template <class T> struct X : public integral_constant<bool, __##X(T)> {};
#define NTL__STLX_DEF_TRAIT2(X,Y)\
  template <class T> struct X : public integral_constant<bool, __##Y(T)> {};

// 20.5.4.1 Primary Type Categories [meta.unary.cat]
NTL__STLX_DEF_TRAIT(is_void)
_CHECK_TRAIT(is_void<void>::value);
_CHECK_TRAIT(is_void<const void>::value);
_CHECK_TRAIT(is_void<volatile void>::value);
_CHECK_TRAIT(is_void<const volatile void>::value);

NTL__STLX_DEF_TRAIT(is_integral)
_CHECK_TRAIT(is_integral<volatile const int>::value);
_CHECK_TRAIT(is_integral<void>::value == false);

NTL__STLX_DEF_TRAIT(is_floating_point)
_CHECK_TRAIT(is_floating_point<double>::value);
_CHECK_TRAIT(is_floating_point<int>::value == false);

NTL__STLX_DEF_TRAIT(is_array)
_CHECK_TRAIT(is_array<int>::value == false);
_CHECK_TRAIT(is_array<int[][2]>::value);

NTL__STLX_DEF_TRAIT(is_pointer)
_CHECK_TRAIT(is_pointer<void*>::value);
_CHECK_TRAIT(is_pointer<void* const>::value);
_CHECK_TRAIT(is_pointer<void* volatile>::value);
_CHECK_TRAIT(is_pointer<void* const volatile>::value);

NTL__STLX_DEF_TRAIT(is_lvalue_reference)
_CHECK_TRAIT(is_lvalue_reference<volatile int&>::value);

NTL__STLX_DEF_TRAIT(is_rvalue_reference)
_CHECK_TRAIT(is_rvalue_reference<volatile int&&>::value);

NTL__STLX_DEF_TRAIT(is_member_object_pointer)
NTL__STLX_DEF_TRAIT(is_member_function_pointer)

namespace test__impl {
struct has_members
{
  //int m;
  //void f(int);
  _CHECK_TRAIT(is_member_object_pointer<int has_members::* const>::value);
  _CHECK_TRAIT(is_member_object_pointer<int *>::value == 0);
  _CHECK_TRAIT(is_member_function_pointer<void (has_members::* volatile)()volatile>::value);
  _CHECK_TRAIT(is_member_function_pointer<void (has_members::*)(int ...)const volatile>::value);
  _CHECK_TRAIT(is_member_function_pointer<int (*)()>::value == 0);
};
}//namespace test__impl

NTL__STLX_DEF_TRAIT(is_enum)

NTL__STLX_DEF_TRAIT(is_union)

NTL__STLX_DEF_TRAIT(is_class)

NTL__STLX_DEF_TRAIT(is_function)
_CHECK_TRAIT(is_function<void()>::value);
_CHECK_TRAIT(is_function<void(int, int, ...)>::value);

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


// 20.5.4.2 Composite type traits [meta.unary.comp]

NTL__STLX_DEF_TRAIT(is_reference)

NTL__STLX_DEF_TRAIT(is_arithmetic)

NTL__STLX_DEF_TRAIT(is_fundamental)

NTL__STLX_DEF_TRAIT(is_object)

NTL__STLX_DEF_TRAIT(is_member_pointer)

NTL__STLX_DEF_TRAIT(is_scalar)

NTL__STLX_DEF_TRAIT(is_compound)

// 20.5.4.3 Type properties [meta.unary.prop]

NTL__STLX_DEF_TRAIT(is_const)
_CHECK_TRAIT(is_const<volatile int>::value == false);
_CHECK_TRAIT(is_const<const int>::value);

NTL__STLX_DEF_TRAIT(is_volatile)
_CHECK_TRAIT(is_volatile<const int>::value == false);
_CHECK_TRAIT(is_volatile<volatile int>::value);

NTL__STLX_DEF_TRAIT(is_trivial);
NTL__STLX_DEF_TRAIT(is_standard_layout);

NTL__STLX_DEF_TRAIT(is_pod)
_CHECK_TRAIT(is_pod<int>::value);

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
_CHECK_TRAIT(is_signed<int>::value);

NTL__STLX_DEF_TRAIT(is_unsigned)
_CHECK_TRAIT(is_unsigned<unsigned>::value);
_CHECK_TRAIT(is_unsigned<float>::value == 0);

namespace __ { namespace aux {
  template<typename T> struct Alignof_wrap { static const size_t value = alignof(T); };

  template<size_t N> struct Alignof_wrap2 {
    typedef integral_constant<size_t, N> type;
  };
}}
template <class T> struct alignment_of: __::aux::Alignof_wrap2<__::aux::Alignof_wrap<T>::value>::type {};
template<> struct alignment_of<void>: integral_constant<size_t, 0>{};
_CHECK_TRAIT(alignment_of<volatile int>::value == sizeof(int));
_CHECK_TRAIT(alignment_of<void>::value == 0);

NTL__STLX_DEF_TRAIT2(rank, array_rank)
_CHECK_TRAIT(rank<int>::value == 0);
_CHECK_TRAIT(rank<int[2]>::value == 1);
//_CHECK_TRAIT(rank<int[][4]>::value == 2);

template <class T, unsigned I = 0> struct extent: public integral_constant<size_t, __array_extent(T, I)> {};
_CHECK_TRAIT(extent<int>::value == 0);
_CHECK_TRAIT(extent<int[2]>::value == 2);
_CHECK_TRAIT(extent<int[2][4]>::value == 2);
_CHECK_TRAIT(extent<int[][4]>::value == 0);
_CHECK_TRAIT((extent<int, 1>::value) == 0);
_CHECK_TRAIT((extent<int[2], 1>::value) == 0);
_CHECK_TRAIT((extent<int[2][4], 1>::value) == 4);
_CHECK_TRAIT((extent<int[][4], 1>::value) == 4);


// 20.5.7.2 Other transformations [meta.trans.other]
#ifdef NTL__CXX_VT

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

template<class T, class U = void, class V = void, class W = void>
struct common_type;

template <class T>
struct common_type<T, void, void, void>
{
  static_assert(sizeof(T) > 0, "T shall be complete");
  typedef T type;
};

template<class T, class U>
struct common_type<T, U, void, void>
{
  static_assert(sizeof(T) > 0, "T shall be complete");
  static_assert(sizeof(T) > 0, "U shall be complete");

private:
  typedef typename remove_cv<typename remove_reference<T>::type>::type rawT;
  typedef typename remove_cv<typename remove_reference<U>::type>::type rawU;

public:
  /**
   *	@brief rules of selecting the "common" type
   *  1) if raw types (without qualificators) are equal, then select one of them, also raw (don't sure here)
   *  2) if one of types is convertible to other, select other type
   *  3) else place the void
   **/
  typedef
    typename conditional<is_same<rawT, rawU>::value, rawT,
      typename conditional<is_arithmetic<rawT>::value && is_arithmetic<rawU>::value, typename conditional<(sizeof(T) < sizeof(U)), rawU, rawT>::type,
        typename conditional<is_convertible<rawT,rawU>::value, rawU,
          typename conditional<is_convertible<rawU,rawT>::value, rawT, void>::type
                            >::type>::type>::type type;
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

#undef _CHECK_TRAIT

#endif//#ifndef NTL__STLX_TYPE_TRAITS_BCB
