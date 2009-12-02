/**\file*********************************************************************
 *                                                                     \brief
 *  23.2.1 General container requirements [container.requirements.general]
 *
 ****************************************************************************
 */
#pragma once
#include "ntl-tests-common.hxx"

/** Compile-time tests.
  @note Use as the one template instantiation per translation unit (cpp file). */
template<class Type, class Container>
struct general_container_test
{
  typedef Type      T;
  typedef Container X;

  ///\name X::value_type
  static_assert((std::is_same<typename X::value_type, T>::value), "T");

  ///\name X::reference
  static_assert((std::is_lvalue_reference<typename X::reference>::value), "lvalue of T");
  static_assert((std::is_same<typename std::remove_reference<typename X::reference>::type, T>::value), "lvalue of T");

  ///\name X::const_reference
  static_assert((std::is_lvalue_reference<typename X::const_reference>::value), "const lvalue of T");
  static_assert((std::is_const<typename std::remove_reference<typename X::const_reference>::type>::value), "const lvalue of T");
  static_assert((std::is_same<typename std::remove_reference<typename X::const_reference>::type, const T>::value), "const lvalue of T");


  ///\name X::difference_type
  static_assert((std::is_integral<typename X::difference_type>::value), "signed integral type");
  static_assert((std::  is_signed<typename X::difference_type>::value), "signed integral type");

  ///\name X::size_type
  static_assert((std::is_integral<typename X::size_type>::value), "unsigned integral type");
  static_assert((std::is_unsigned<typename X::size_type>::value), "unsigned integral type");
  ///\}
};

