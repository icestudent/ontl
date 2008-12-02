/**\file*********************************************************************
 *                                                                     \brief
 *  20.5 Metaprogramming and type traits [meta]
 *
 ****************************************************************************
 */
// sorry, MSVC 2005+ only

#include "cstddef.hxx"
#include "limits.hxx"
#include "type_traits_fwd.hxx"

#ifndef NTL__STLX_TYPE_TRAITS
#define NTL__STLX_TYPE_TRAITS

#if 1
#define _CHECK_TRAIT(X) static_assert((X), "Traits check failed on '" #X "'")
#else
#define _CHECK_TRAIT(X)
#endif

namespace std
{

namespace __
{
  struct sfinae_small { char x; };
  struct sfinae_big   { long x[10]; };

  typedef sfinae_small  sfinae_passed_tag;
  typedef sfinae_big    sfinae_failed_tag;

#define NTL_SFINAE_EVAL(test) (sizeof(test) == sizeof(std::__::sfinae_passed_tag))

} // namespace __

} // namespace std


#define NTL__IN_STLX_TYPE_TRAITS

#if   defined(__GNUC__)
  #include "type_traits_gnuc.hxx"
#elif defined(_MSC_VER)
  #include "type_traits_msvc.hxx"
#elif defined(__BCPLUSPLUS__)
  #include "type_traits_bcb.hxx"
#else
  #error Unspported compiler
#endif

#undef  NTL__IN_STLX_TYPE_TRAITS

namespace std {
namespace __ {

struct check_meta_rel
{
  _CHECK_TRAIT((is_same<volatile int, volatile int>::value));
};

struct check_is_base_of
{
  struct Base {};
  struct Derived : Base {};
  class Foo {};

  _CHECK_TRAIT((is_base_of<Base, Derived>::value));
  _CHECK_TRAIT((is_base_of<Base, Base>::value));
  _CHECK_TRAIT((is_base_of<Derived, Derived>::value));
  _CHECK_TRAIT((is_base_of<Derived, Base>::value == false));

  _CHECK_TRAIT((is_base_of<Base, Foo>::value == false));
  _CHECK_TRAIT((is_base_of<Foo, Derived>::value == false));
  _CHECK_TRAIT((is_base_of<Foo, Foo>::value));

  _CHECK_TRAIT((is_base_of<int, int>::value == false));
  _CHECK_TRAIT((is_base_of<short, int>::value) == false);
};

struct check_meta_trans_cv
{
  _CHECK_TRAIT((is_same<remove_const<volatile const int>::type, volatile int>::value));
  _CHECK_TRAIT((is_same<remove_const<const int>::type, const int>::value) == 0);
  _CHECK_TRAIT((is_same<remove_volatile<volatile int>::type, volatile int>::value) == 0);
  _CHECK_TRAIT((is_same<remove_volatile<volatile const int>::type, const int>::value));
  _CHECK_TRAIT((is_same<remove_cv<const int>::type, const int>::value) == 0);
  _CHECK_TRAIT((is_same<remove_cv<volatile int>::type, volatile int>::value) == 0);
  _CHECK_TRAIT((is_same<remove_cv<volatile const int>::type, int>::value));
  _CHECK_TRAIT((is_same<add_const<volatile int>::type, volatile const int>::value));
  _CHECK_TRAIT((is_same<add_const<int>::type, const int>::value));
  _CHECK_TRAIT((is_same<add_volatile<const int>::type, volatile const int>::value));
  _CHECK_TRAIT((is_same<add_volatile<int>::type, volatile int>::value));
  _CHECK_TRAIT((is_same<add_cv<int>::type, volatile const int>::value));
};

struct check_meta_trans_ref
{
#ifdef NTL__CXX_RV
  _CHECK_TRAIT((is_same<remove_reference<int&>::type, int&>::value) == 0);
  _CHECK_TRAIT((is_same<remove_reference<int&>::type, int>::value));
#endif
  _CHECK_TRAIT((is_same<add_lvalue_reference<int&>::type, int&>::value));
  _CHECK_TRAIT((is_same<add_lvalue_reference<int>::type, int&>::value));
};

struct check_meta_trans_arr
{
  _CHECK_TRAIT((is_same<remove_extent<int[][3]>::type, int[3]>::value));
  _CHECK_TRAIT((is_same<remove_extent<const int>::type, const int>::value));
  _CHECK_TRAIT((is_same<remove_extent<volatile int[]>::type, volatile int>::value));
  _CHECK_TRAIT((is_same<remove_extent<const volatile int[2]>::type, const volatile int>::value));

  _CHECK_TRAIT((is_same<remove_all_extents<int>::type, int>::value));
  _CHECK_TRAIT((is_same<remove_all_extents<int[2]>::type, int>::value));
  _CHECK_TRAIT((is_same<remove_all_extents<int[2][3]>::type, int>::value));
  _CHECK_TRAIT((is_same<remove_all_extents<int[][3][6]>::type, int>::value));
};

struct check_meta_trans_ptr
{
  _CHECK_TRAIT((is_same<remove_pointer<const int*>::type, const int>::value));
  _CHECK_TRAIT((is_same<remove_pointer<int*>::type, int*>::value) == 0);

  _CHECK_TRAIT((is_same<add_pointer<volatile int>::type, volatile int*>::value));
  _CHECK_TRAIT((is_same<add_pointer<int*&>::type, int**>::value));
};

struct check_meta_unary_cat
{
  _CHECK_TRAIT(is_void<void>::value);
  _CHECK_TRAIT(is_void<const void>::value);
  _CHECK_TRAIT(is_void<volatile void>::value);
  _CHECK_TRAIT(is_void<const volatile void>::value);

  _CHECK_TRAIT(is_integral<volatile const int>::value);
  _CHECK_TRAIT(is_integral<void>::value == false);

  _CHECK_TRAIT(is_floating_point<double>::value);
  _CHECK_TRAIT(is_floating_point<int>::value == false);

  _CHECK_TRAIT(is_array<int>::value == false);
  _CHECK_TRAIT(is_array<int[][2]>::value);

  _CHECK_TRAIT(is_pointer<void*>::value);
  _CHECK_TRAIT(is_pointer<void* const>::value);
  _CHECK_TRAIT(is_pointer<void* volatile>::value);
  _CHECK_TRAIT(is_pointer<void* const volatile>::value);

  _CHECK_TRAIT(is_lvalue_reference<volatile int&>::value);
};

} // namespace __
} // namespace std



#undef _CHECK_TRAIT

#endif//#ifndef NTL__STLX_TYPE_TRAITS
