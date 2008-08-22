/**\file*********************************************************************
 *                                                                     \brief
 *  Numerics library [25 lib.numerics]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_NUMERICS
#define NTL__STLX_NUMERICS

#include "iterator.hxx"


namespace std {

// 26.6 Generalized numeric operations [numeric.ops]

  // 26.6.1 Accumulate [accumulate]
  template <class InputIterator, class T>
  __forceinline
  T 
    accumulate(InputIterator first, InputIterator last, T init)
  {
    while(first != last){
      init = init + *first;
      ++first;
    }
    return init;
  }

  // 26.6.1 Accumulate [accumulate]
  template <class InputIterator, class T, class BinaryOperation>
  __forceinline
  T 
    accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op)
  {
    while(first != last){
      init = binary_op(init, *first);
      ++first;
    }
    return init;
  }

} // namespace std
#endif // NTL__STLX_NUMERICS
