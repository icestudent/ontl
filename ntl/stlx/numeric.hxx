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
#include "utility.hxx"

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

  namespace __
  {
    template<typename T, typename D = typename std::make_signed<T>::type>
    struct xrange
    {
      typedef size_t      size_type;
      typedef D           difference_type;
      typedef       T   * pointer;
      typedef const T   * const_pointer;
      typedef       T   & reference;
      typedef const T   & const_reference;
      typedef       T     value_type;

    protected:
      struct iterator_impl: std::iterator<std::random_access_iterator_tag, value_type, difference_type, const_pointer, const_reference>
      {
        const_reference operator*() const { return val; }
        const_pointer   operator->()const { return &val; }

        value_type operator[](difference_type n) const
        {
          return val + n * step;
        }

        iterator_impl& operator++()     { val += step; return *this; }
        iterator_impl& operator--()     { val -= step; return *this; }
        iterator_impl& operator++(int)  { iterator_impl tmp(*this); ++tmp; return *this; }
        iterator_impl& operator--(int)  { iterator_impl tmp(*this); --tmp; return *this; }

        iterator_impl& operator += (difference_type n)
        {
          val += n * step;
          return *this;
        }

        iterator_impl& operator -= (difference_type n)
        {
          val -= n * step;
          return *this;
        }

        friend bool operator == (const iterator_impl& x, const iterator_impl& y) { return x.val == y.val;  }
        friend bool operator != (const iterator_impl& x, const iterator_impl& y) { return !(x == y);       }
        friend bool operator <  (const iterator_impl& x, const iterator_impl& y) { return x.val < y.val;   }
        friend bool operator >  (const iterator_impl& x, const iterator_impl& y) { return rel_ops::operator >(x, y);   }
        friend bool operator <= (const iterator_impl& x, const iterator_impl& y) { return rel_ops::operator <= (x, y); }
        friend bool operator >= (const iterator_impl& x, const iterator_impl& y) { return rel_ops::operator >= (x, y); }

        friend iterator_impl operator + (const difference_type n, const iterator_impl& x) { return x + n; }
        friend iterator_impl operator - (const difference_type n, const iterator_impl& x) { return x - n; }

        friend iterator_impl operator + (const iterator_impl& x, const difference_type n)
        {
          iterator_impl tmp(x);
          tmp += n * x.step;
          return tmp;
        }

        friend iterator_impl operator - (const iterator_impl& x, const difference_type n)
        {
          iterator_impl tmp(x);
          tmp -= n * x.step;
          return tmp;
        }

        friend difference_type operator - (const iterator_impl& x, const iterator_impl& y) { return (x.val - y.val) / x.step;  }

      protected:
        iterator_impl(value_type val, difference_type step)
          : val(val), step(step)
        {}

      private:
        friend xrange;
        const difference_type step;
        value_type val;
      };

    public:
      explicit xrange(value_type first, value_type last, difference_type step = 1)
        : first(first), last(last), step(step)
      {}

      typedef iterator_impl iterator, const_iterator;

      const_iterator begin() const { return iterator_impl(first, step); }
      const_iterator end()   const { return iterator_impl(last, step);  }

      const_iterator cbegin() const { return iterator_impl(first, step); }
      const_iterator cend()   const { return iterator_impl(last, step);  }

      size_type size()  const { return last -  first; }
      bool      empty() const { return last == first; }

      bool operator==(const xrange& r) const { return first == r.first && last == r.last && step == r.step; }
      bool operator!=(const xrange& r) const { return !(*this == r); }

    private:
      const value_type first, last;
      const difference_type step;
    };
  }

  ///\name Range generators

  /**
   *	@brief This is a versatile function creates an opaque sequence type which yields values of arithmetic progressions.
   *	
   *	The arguments must be plain integers.
   *	If the \c step argument is omitted, it defaults to 1; it must not be zero
   *	If the \c start argument is omitted, it defaults to 0.
   *	
   *	If \c step is positive, the last element is the largest <tt> start + i * step </tt> less than \c stop;
   *	if \c step is negative, the last element is the smallest <tt> start + i * step </tt> greater than \c stop.
   **/
  template<typename T, typename D>
  inline __::xrange<T,D> irange(T start, T stop, D step) { return __::xrange<T,D>(start, stop, step); }

  /** \sa irange() */
  template<typename T>
  inline __::xrange<T> irange(T start, T stop) { return __::xrange<T>(start, stop); }

  /** \sa irange() */
  template<typename T>
  inline __::xrange<T> irange(T stop) { return __::xrange<T>(T(), stop); }
  ///\}


  /**@} lib_numeric */
} // namespace std

#endif // NTL__STLX_NUMERICS
