/**\file*********************************************************************
 *                                                                     \brief
 *  20.5 Metaprogramming and type traits [meta]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_TYPE_TRAITS_FWD
#define NTL__STLX_TYPE_TRAITS_FWD

namespace std 
{
 /**\addtogroup  lib_utilities *** 20 General utilities library [utilities]
  *@{
  **/
 /**\addtogroup  lib_typetraits ** 20.5 Metaprogramming and type traits [meta]
  *@{
  **/

// 20.5.3 Helper classes [meta.help]
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
template <class T, class U> struct is_same;
template <class Base, class Derived> struct is_base_of;
template <class From, class To> struct is_convertible;

// 20.5.6 Transformations between types [meta.trans]

// 20.5.6.1 Const-volatile modifications [meta.trans.cv]
template <class T> struct remove_const;
template <class T> struct remove_volatile;
template <class T> struct remove_cv;
template <class T> struct add_const;
template <class T> struct add_volatile;
template <class T> struct add_cv;

// 20.5.6.2 Reference modifications [meta.trans.ref]
template <class T> struct remove_reference;
template <class T> struct add_lvalue_reference;
template <class T> struct add_rvalue_reference;
template <class T> struct add_reference;

// 20.5.6.3 Sign modifications [meta.trans.sign]
template <class T> struct make_signed;
template <class T> struct make_unsigned;

// 20.5.6.4 Array modifications [meta.trans.arr]
template <class T> struct remove_extent;
template <class T> struct remove_all_extents;

// 20.5.6.5 Pointer modifications [meta.trans.ptr]
template <class T> struct remove_pointer;
template <class T> struct add_pointer;

// 20.5.7 Other transformations [meta.trans.other]


namespace aux {
template <std::size_t Align>struct aligner;
} // namespace aux

template <std::size_t Len, std::size_t Align = alignof(void*)>
struct aligned_storage;

#ifdef NTL__CXX_VT
template <std::size_t Len, class... Types> struct aligned_union;
#endif

template <class T> struct decay;
template <bool, class T = void> struct enable_if;
template <bool, class IfTrueType, class IfFalseType> struct conditional;

namespace __
{
  // static logical operators: OR and AND
  // usage:
  // select_or<condition1, condition2, TrueType, FalseType>::type is equal to "if(condition1 || condition2) TrueType; else FalseType;
  // select_and works same, but with the "&&" operation.

  template<bool cond1, bool cond2,
  class IfTrueType, class IfFalseType>
  struct select_or;

}

// 20.5.4 Unary Type Traits [meta.unary]

// 20.5.4.1 Primary Type Categories [meta.unary.cat]
template <class T> struct is_void;
template <class T> struct is_integral;
template <class T> struct is_floating_point;
template <class T> struct is_array;
template <class T> struct is_pointer;
template <class T> struct is_lvalue_reference;
template <class T> struct is_rvalue_reference;
template <class T> struct is_member_object_pointer;
template <class T> struct is_member_function_pointer;
template <class T> struct is_enum;
template <class T> struct is_union;
template <class T> struct is_class;
template <class T> struct is_function;
template <class RT> struct is_function<RT()>;

// 20.5.4.2 Composite type traits [meta.unary.comp]
template <class T> struct is_reference;
template <class T> struct is_arithmetic;
template <class T> struct is_fundamental;
template <class T> struct is_object;
template <class T> struct is_member_pointer;
template <class T> struct is_scalar;
template <class T> struct is_compound;

// 20.5.4.3 Type properties [meta.unary.prop]
template <class T> struct is_const;
template <class T> struct is_volatile;

namespace __ {
  template<class T> struct is_unknown_array;
}

template<class T> struct is_trivial;
template<class T> struct is_standard_layout;

template <class T> struct is_pod;
template <class T> struct is_empty;
template <class T> struct is_polymorphic;
template <class T> struct is_abstract;
template <class T> struct has_trivial_default_constructor;
template <class T> struct has_trivial_copy_constructor;
template <class T> struct has_trivial_assign;
template <class T> struct has_trivial_destructor;
template <class T> struct has_nothrow_default_constructor;
template <class T> struct has_nothrow_copy_constructor;
template <class T> struct has_nothrow_assign;
template <class T> struct has_virtual_destructor;
template <class T> struct is_signed;
template <class T> struct is_unsigned;
template <class T> struct alignment_of;
template <class T> struct rank;
template <class T, unsigned I = 0> struct extent;
template<class T> struct is_trivial;

// 20.5.7.2 Other transformations [meta.trans.other]
#if defined(NTL__CXX_VT) && defined(NTL__CXX_TYPEOF)

template <class ...T> struct common_type;

#else // NTL__CXX

template<class T, class U = void, class V = void, class W = void>
struct common_type;

#endif

/**@} lib_typetraits */
/**@} lib_utilities */
}//namespace std

#undef _CHECK_TRAIT

#endif//#ifndef NTL__STLX_TYPE_TRAITS_FWD
