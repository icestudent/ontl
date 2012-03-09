/**\file*********************************************************************
 *                                                                     \brief
 *  26 Numerics library [numerics]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_NUMERICS
#define NTL__STLX_NUMERICS
#pragma once

#include "iterator.hxx"

namespace std
{
 /**\defgroup  lib_numeric ************ 26 Numerics library [numerics]
  *@{
  **/

  // 26.7.2 Accumulate [accumulate]
  template <class InputIterator, class T>
  __forceinline
  T accumulate(InputIterator first, InputIterator last, T init)
  {
    while(first != last){
      init = init + *first;
      ++first;
    }
    return init;
  }

  // 26.7.2 Accumulate [accumulate]
  template <class InputIterator, class T, class BinaryOperation>
  __forceinline
  T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op)
  {
    while(first != last){
      init = binary_op(init, *first);
      ++first;
    }
    return init;
  }

  // 26.7.3 Inner product [inner.product]
  template <class InputIterator1, class InputIterator2, class T>
  inline T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init)
  {
    while(first1 != last1){
      init += *first1 * *first2;
      ++first1;
      ++first2;
    }
    return init;
  }
  
  template <class InputIterator1, class InputIterator2, class T, class BinaryOperation1, class BinaryOperation2>
  inline T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init, BinaryOperation1 binary_op1, BinaryOperation2 binary_op2)
  {
    while(first1 != last1){
      init = binary_op1(init, binary_op2(*first1, *first2));
      ++first1;
      ++first2;
    }
    return init;
  }

  // 26.7.6 Iota [numeric.iota]
  template <class ForwardIterator, class T>
  __forceinline
  void iota(ForwardIterator first, ForwardIterator last, T value)
  {
    while(first != last){
      *first = value;
      ++value;
    }
  }
  /**@} lib_numeric */
} // namespace std

#endif // NTL__STLX_NUMERICS
