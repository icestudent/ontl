/**\file*********************************************************************
 *                                                                     \brief
 *  20.5 Metaprogramming and type traits [meta]
 *
 ****************************************************************************
 */

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
 /**\defgroup  lib_utilities *** 20 General utilities library [utilities]
  *
  *    Components used by other elements of the Standard C + + library.
  *@{
  **/

 /**\defgroup  lib_typetraits ** 20.5 Metaprogramming and type traits [meta]
  *
  * This subclause describes components used by C++ programs, particularly in templates, to support the
  * widest possible range of types, optimise template code usage, detect type related user errors, and perform
  * type inference and transformation at compile time.
  *
  * It includes type classification traits, type property inspection traits, and type transformations.\n
  * The type classification traits describe a complete taxonomy of all possible C++ types, and state 
  * where in that taxonomy a given type belongs.
  * The type property inspection traits allow important characteristics of types or of combinations of types to be inspected.
  * The type transformations allow certain properties of types to be manipulated.
  *@{
  */

namespace __
{
  // sfinae support

  template<typename T>
  struct type2type
  {
    typedef T type;
  };

  template<size_t v>
  struct int2type
  {
    enum { value = v };
  };

  struct sfinae_small { char x; };
  struct sfinae_big   { char x[2]; };

  typedef sfinae_small  sfinae_passed_tag;
  typedef sfinae_big    sfinae_failed_tag;

#define NTL_SFINAE_EVAL(test) (sizeof(test) == sizeof(std::__::sfinae_passed_tag))
#define NTL__EMPTY0
#define NTL__EMPTY NTL__EMPTY0

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

/**@} lib_typetraits */
/**@} lib_utilities */
} // namespace std



#undef _CHECK_TRAIT

#endif//#ifndef NTL__STLX_TYPE_TRAITS
