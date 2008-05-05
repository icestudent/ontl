/**\file*********************************************************************
 *                                                                     \brief
 *  DTR: 4 Metaprogramming and type traits [tr.meta]
 *
 ****************************************************************************
 */

// sorry, MSVC 2005 only

#include "cstddef.hxx"
#include "limits.hxx"

#ifndef NTL__STLX_TYPE_TRAITS
#define NTL__STLX_TYPE_TRAITS

#if 1
#define _CHECK_TRAIT(X) STATIC_ASSERT(X)
#else
#define _CHECK_TRAIT(X)
#endif

namespace std {

/// 4.3 Helper classes [tr.meta.help]
template <class T, T v>
struct integral_constant
{
  static const T                  value = v;
  typedef T                       value_type;
  typedef integral_constant<T, v> type;
};

typedef integral_constant<bool, true>   true_type;
typedef integral_constant<bool, false>  false_type;


// 4.6 Relationships between types [tr.meta.rel]

template <class T, class U> struct is_same : false_type {};
template <class T> struct is_same<T, T> : true_type {};
_CHECK_TRAIT((is_same<volatile int, volatile int>::value));

template <class Base, class Derived> struct is_base_of
: public integral_constant<bool, __is_base_of(Base, Derived)> {};

template <class From, class To> struct is_convertible
: public integral_constant<bool, __is_convertible_to(From, To)> {};


// 4.7 Transformations between types [tr.meta.trans]

// 4.7.1 Const-volatile modifications [tr.meta.trans.cv]

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

// 4.7.2 Reference modifications [tr.meta.trans.ref]

template <class T> struct remove_reference     { typedef T type; };
template <class T> struct remove_reference<T&> { typedef T type; };
#ifdef NTL__CXX
template <class T> struct remove_reference<T&&> { typedef T type; };
_CHECK_TRAIT((is_same<remove_reference<int&>::type, int&>::value) == 0);
_CHECK_TRAIT((is_same<remove_reference<int&>::type, int>::value));
#endif

template <class T> struct add_lvalue_reference     { typedef T& type; };
template <class T> struct add_lvalue_reference<T&> { typedef T& type; };
_CHECK_TRAIT((is_same<add_lvalue_reference<int&>::type, int&>::value));
_CHECK_TRAIT((is_same<add_lvalue_reference<int>::type, int&>::value));

template <class T> struct add_reference : add_lvalue_reference<T> {};

// 4.7.3 Array modifications [tr.meta.trans.arr]

template <class T> struct remove_extent { typedef T type; };
template <class T> struct remove_extent<T[]> { typedef T type; };
template <class T, size_t Size> struct remove_extent<T[Size]> { typedef T type; };
_CHECK_TRAIT((is_same<remove_extent<int[][3]>::type, int[3]>::value));
_CHECK_TRAIT((is_same<remove_extent<const int>::type, const int>::value));
_CHECK_TRAIT((is_same<remove_extent<volatile int[]>::type, volatile int>::value));
_CHECK_TRAIT((is_same<remove_extent<const volatile int[2]>::type, const volatile int>::value));

template <class T> struct remove_all_extents { typedef T type; };
template <class T> struct remove_all_extents<T[]>
  { typedef typename remove_all_extents<T>::type type; };
template <class T, size_t Size> struct remove_all_extents<T[Size]>
  { typedef typename remove_all_extents<T>::type type; };
_CHECK_TRAIT((is_same<remove_all_extents<int>::type, int>::value));
_CHECK_TRAIT((is_same<remove_all_extents<int[2]>::type, int>::value));
_CHECK_TRAIT((is_same<remove_all_extents<int[2][3]>::type, int>::value));
_CHECK_TRAIT((is_same<remove_all_extents<int[][3][6]>::type, int>::value));

// 4.7.4 Pointer modifications [tr.meta.trans.ptr]

template <class T> struct remove_pointer     { typedef T type; };
template <class T> struct remove_pointer<T*> { typedef T type; };
_CHECK_TRAIT((is_same<remove_pointer<const int*>::type, const int>::value));
_CHECK_TRAIT((is_same<remove_pointer<int*>::type, int*>::value) == 0);

template <class T> struct add_pointer     { typedef T* type; };
template <class T> struct add_pointer<T&> { typedef typename remove_reference<T>::type* type; };
_CHECK_TRAIT((is_same<add_pointer<volatile int>::type, volatile int*>::value));
_CHECK_TRAIT((is_same<add_pointer<int*&>::type, int**>::value));

// 4.8 Other transformations [tr.meta.trans.other]

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
}

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

_CHECK_TRAIT(__alignof(aligned_storage<5, 8000>::type) == 8192);
_CHECK_TRAIT(sizeof(aligned_storage<2, 4>::type) == 4);
//_CHECK_TRAIT(sizeof(aligned_storage<2, 4>::type) == 2);

template <class T> struct decay;

template <bool, class T = void> struct enable_if {};
template <class T> struct enable_if<true, T> { typedef T type; };

template <bool, class T, class F> struct conditional;
template <class T, class F> struct conditional<true, T, F>  { typedef T type; }; 
template <class T, class F> struct conditional<false, T, F> { typedef F type; }; 

// 4.5 Unary Type Traits [tr.meta.unary]

#define NTL__STLX_DEF_TRAIT(X)\
  template <class T> struct X : public integral_constant<bool, __##X(T)> {};

// 4.5.1 Primary Type Categories [tr.meta.unary.cat]

template <class T> struct is_void                      : public false_type {};
template <>        struct is_void<void>                : public true_type {};
template <>        struct is_void<const void>          : public true_type {};
template <>        struct is_void<volatile void>       : public true_type {};
template <>        struct is_void<const volatile void> : public true_type {};
_CHECK_TRAIT(is_void<void>::value);
_CHECK_TRAIT(is_void<const void>::value);
_CHECK_TRAIT(is_void<volatile void>::value);
_CHECK_TRAIT(is_void<const volatile void>::value);

template <class T> struct is_integral
: public integral_constant<bool, numeric_limits<typename remove_cv<T>::type>::is_integer> {};
_CHECK_TRAIT(is_integral<volatile const int>::value);
_CHECK_TRAIT(is_integral<void>::value == false);

template <class T> struct is_floating_point
: public integral_constant<bool, numeric_limits<typename remove_cv<T>::type>::is_iec559> {};
_CHECK_TRAIT(is_floating_point<double>::value);
_CHECK_TRAIT(is_floating_point<int>::value == false);

template <class T> struct is_array      : public false_type {};
template <class T> struct is_array<T[]> : public true_type {};
_CHECK_TRAIT(is_array<int>::value == false);
_CHECK_TRAIT(is_array<int[][2]>::value);

template <class T> struct is_pointer                    : public false_type {};
template <class T> struct is_pointer<T*>                : public true_type {};
template <class T> struct is_pointer<T* const>          : public true_type {};
template <class T> struct is_pointer<T* volatile>       : public true_type {};
template <class T> struct is_pointer<T* const volatile> : public true_type {};
_CHECK_TRAIT(is_pointer<void*>::value);
_CHECK_TRAIT(is_pointer<void* const>::value);
_CHECK_TRAIT(is_pointer<void* volatile>::value);
_CHECK_TRAIT(is_pointer<void* const volatile>::value);

template <class T> struct is_lvalue_reference     : public false_type {};
template <class T> struct is_lvalue_reference<T&> : public true_type {};
_CHECK_TRAIT(is_lvalue_reference<volatile int&>::value);

template <class T> struct is_rvalue_reference     : public false_type {};

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

template <class T>
struct is_function : public false_type {};
template <class RT>
struct is_function<RT()> : public true_type {};
template <class RT>
struct is_function<RT(...)> : public true_type {};
template <class RT, class A1>
struct is_function<RT(A1)> : public true_type {};
template <class RT, class A1>
struct is_function<RT(A1, ...)> : public true_type {};
template <class RT, class A1, class A2>
struct is_function<RT(A1, A2)> : public true_type {};
template <class RT, class A1, class A2>
struct is_function<RT(A1, A2, ...)> : public true_type {};
template <class RT, class A1, class A2, class A3>
struct is_function<RT(A1, A2, A3)> : public true_type {};
template <class RT, class A1, class A2, class A3>
struct is_function<RT(A1, A2, A3, ...)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4>
struct is_function<RT(A1, A2, A3, A4)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4>
struct is_function<RT(A1, A2, A3, A4, ...)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5>
struct is_function<RT(A1, A2, A3, A4, A5)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5>
struct is_function<RT(A1, A2, A3, A4, A5, ...)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6>
struct is_function<RT(A1, A2, A3, A4, A5, A6)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6>
struct is_function<RT(A1, A2, A3, A4, A5, A6, ...)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, ...)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, ...)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9, ...)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, ...)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, ...)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, ...)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, ...)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, ...)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, ...)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15, class A16>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16)> : public true_type {};
template <class RT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15, class A16>
struct is_function<RT(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, ...)> : public true_type {};
_CHECK_TRAIT(is_function<void()>::value);
_CHECK_TRAIT(is_function<void(int, int, ...)>::value);


template <class T> struct decay
{
  private: typedef typename remove_reference<T>::type U;
  public:  typedef
    typename conditional<is_array<U>::value,
                          typename remove_extent<U>::type*,
                          typename conditional<is_function<U>::value,
                                                typename add_pointer<U>::type,
                                                typename remove_cv<U>::type
                                              >
                        >::type   type;
};


// 4.5.2 Composite type traits [tr.meta.unary.comp]

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

// 4.5.3 Type properties [tr.meta.unary.prop]

template <class T> struct is_const          : public false_type {};
template <class T> struct is_const<const T> : public true_type {};
_CHECK_TRAIT(is_const<volatile int>::value == 0);
_CHECK_TRAIT(is_const<const int>::value);

template <class T> struct is_volatile             : public false_type {};
template <class T> struct is_volatile<volatile T> : public true_type {};
_CHECK_TRAIT(is_volatile<const int>::value == 0);
_CHECK_TRAIT(is_volatile<volatile int>::value);

template <class T> struct is_pod
: public integral_constant<
    bool, __is_pod(typename remove_extent<T>::type) // __is_pod will return false on fundamental types
      || is_scalar<typename remove_extent<T>::type>::value
    >
{
  static const bool __v = integral_constant::value;
  STATIC_ASSERT(__v == is_pod<typename remove_extent<T>::type>::__v
             && __v == is_pod<T const volatile>::__v
             && __v >= (is_scalar<T>::value || is_void<T>::value)
             );
};
_CHECK_TRAIT(is_pod<int>::value);

NTL__STLX_DEF_TRAIT(is_empty)
  
NTL__STLX_DEF_TRAIT(is_polymorphic)

NTL__STLX_DEF_TRAIT(is_abstract)

NTL__STLX_DEF_TRAIT(has_trivial_constructor)

NTL__STLX_DEF_TRAIT(has_trivial_copy)

NTL__STLX_DEF_TRAIT(has_trivial_assign)

NTL__STLX_DEF_TRAIT(has_trivial_destructor)

NTL__STLX_DEF_TRAIT(has_nothrow_constructor)

NTL__STLX_DEF_TRAIT(has_nothrow_copy)

NTL__STLX_DEF_TRAIT(has_nothrow_assign)

NTL__STLX_DEF_TRAIT(has_virtual_destructor)

template <class T> struct is_signed
//: public integral_constant<bool, (is_arithmetic<T>::value && T(-1) < T(0))> {};
: public integral_constant<bool, (static_cast<T>(-1) < 0)> {};
_CHECK_TRAIT(is_signed<int>::value);

template <class T> struct is_unsigned
//: public integral_constant<bool, (is_arithmetic<T>::value && T(-1) > T(0))> {};
: public integral_constant<bool, (static_cast<T>(-1) > 0)> {};
_CHECK_TRAIT(is_unsigned<unsigned>::value);
_CHECK_TRAIT(is_unsigned<float>::value == 0);

template <class T> struct alignment_of
: public integral_constant<size_t, __alignof(T)> {};
template <>        struct alignment_of<void>
: public integral_constant<size_t, 0> {};
template <>        struct alignment_of<const void>
: public integral_constant<size_t, 0> {};
template <>        struct alignment_of<volatile void>
: public integral_constant<size_t, 0> {};
template <>        struct alignment_of<const volatile void>
: public integral_constant<size_t, 0> {};
_CHECK_TRAIT(alignment_of<volatile int>::value == sizeof(int));
_CHECK_TRAIT(alignment_of<void>::value == 0);

template <class T> struct rank : public integral_constant<size_t, 0> {};
template <class T> struct rank<T[]>
: public integral_constant<size_t, rank<T>::value + 1> {};
template <class T, size_t Size> struct rank<T[Size]>
: public integral_constant<size_t, rank<T>::value + 1> {};
_CHECK_TRAIT(rank<int>::value == 0);
_CHECK_TRAIT(rank<int[2]>::value == 1);
_CHECK_TRAIT(rank<int[][4]>::value == 2);

template <class T, unsigned I = 0> struct extent
: public integral_constant<size_t, 0> {};
template <class T, unsigned I> struct extent<T[], I>
: public integral_constant<size_t, extent<T, I-1>::value> {};
template <class T, unsigned I, size_t Size> struct extent<T[Size], I>
: public integral_constant<size_t, extent<T, I-1>::value> {};
template <class T, size_t Size> struct extent<T[Size], 0>
: public integral_constant<size_t, Size> {};
_CHECK_TRAIT(extent<int>::value == 0);
_CHECK_TRAIT(extent<int[2]>::value == 2);
_CHECK_TRAIT(extent<int[2][4]>::value == 2);
_CHECK_TRAIT(extent<int[][4]>::value == 0);
_CHECK_TRAIT((extent<int, 1>::value) == 0);
_CHECK_TRAIT((extent<int[2], 1>::value) == 0);
_CHECK_TRAIT((extent<int[2][4], 1>::value) == 4);
_CHECK_TRAIT((extent<int[][4], 1>::value) == 4);

}//namespace std

#undef _CHECK_TRAIT

#endif//#ifndef NTL__STLX_TYPE_TRAITS
