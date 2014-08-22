/**\file*********************************************************************
 *                                                                     \brief
 *  20.6 Metaprogramming and type traits [meta]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TYPE_TRAITS_ALIASES
#define NTL__STLX_TYPE_TRAITS_ALIASES
#pragma once

namespace std 
{
 /**\addtogroup  lib_utilities *** 20 General utilities library [utilities]
  *@{
  **/
 /**\addtogroup  lib_typetraits ** 20.6 Metaprogramming and type traits [meta]
  *@{
  **/

  template <class T> using remove_const_t                 = typename remove_const<T>::type;
  template <class T> using remove_volatile_t              = typename remove_volatile<T>::type;
  template <class T> using remove_cv_t                    = typename remove_cv<T>::type;
  template <class T> using add_const_t                    = typename add_const<T>::type;
  template <class T> using add_volatile_t                 = typename add_volatile<T>::type;
  template <class T> using add_cv_t                       = typename add_cv<T>::type;

  template <class T> using remove_reference_t             = typename remove_reference<T>::type;
  template <class T> using add_lvalue_reference_t         = typename add_lvalue_reference<T>::type;
  template <class T> using add_rvalue_reference_t         = typename add_rvalue_reference<T>::type;

  template <class T> using make_signed_t                  = typename make_signed<T>::type;
  template <class T> using make_unsigned_t                = typename make_unsigned<T>::type;

  template <class T> using remove_extent_t                = typename remove_extent<T>::type;
  template <class T> using remove_all_extents_t           = typename remove_all_extents<T>::type;

  template <class T> using remove_pointer_t               = typename remove_pointer<T>::type;
  template <class T> using add_pointer_t                  = typename add_pointer<T>::type;


  template <std::size_t Len, std::size_t Align = alignof(void*)>
  using aligned_storage_t = typename aligned_storage<Len,Align>::type;

  template <std::size_t Len, class... Types> 
  using aligned_union_t = typename aligned_union<Len,Types...>::type;


  template <class T> using decay_t                        = typename decay<T>::type;
  template <bool b, class T = void> using enable_if_t     = typename enable_if<b,T>::type;
  template <bool b, class T, class F> using conditional_t = typename conditional<b,T,F>::type;

  template <class... T> using common_type_t               = typename common_type<T...>::type;
  template <class T> using underlying_type_t              = typename underlying_type<T>::type;

  /**@} lib_typetraits */
  /**@} lib_utilities */
}//namespace std


#endif//#ifndef NTL__STLX_TYPE_TRAITS_ALIASES
