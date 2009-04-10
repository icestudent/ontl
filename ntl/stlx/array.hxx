/**\file*********************************************************************
 *                                                                     \brief
 *  Fixed size array [6.2 tr.array]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_ARRAY
#define NTL__STLX_ARRAY

#include "iterator.hxx"
#include "stdexcept.hxx"

namespace std {

/**\addtogroup  lib_containers ********* 23 Containers library [containers] ************
 *@{*/

/**\addtogroup  lib_sequence *********** 23.2 Sequence containers [sequences] *******************
 *@{*/

  /**
   *	@brief 23.2.1 Class template array [%array]
   *
   *  The header \<array\> defines a class template for storing fixed-size sequences of objects. An array supports
   *  random access iterators. An instance of <tt>array<T, N></tt> stores \e N elements of type \e T, 
   *  so that <tt>size() == N</tt> is an invariant.
   *  The elements of an array are stored contiguously, meaning that if \c a is an <tt>array<T, N></tt> then
   *  it obeys the %identity <tt>&a[n] == &a[0] + n</tt> for all <tt>0 <= n < N</tt>.
   *
   *  An array is an aggregate (8.5.1) that can be initialized with the syntax: <tt>array a<T, N> = { initializer-list };</tt>
   *  where <i>initializer-list</i> is a comma separated %list of up to \e N elements whose types are convertible to \e T.
   *
   *  Unless otherwise specified, all array operations are as described in 23.1. Descriptions are provided here only
   *  for operations on array that are not described in that Clause or for operations where there is additional
   *  semantic information.
   **/
template <class T, size_t N>
struct array
{
    // types:

    typedef T &                                   reference;
    typedef const T &                             const_reference;
    typedef T *                                   iterator;
    typedef const T *                             const_iterator;
    typedef size_t                                size_type;
    typedef ptrdiff_t                             difference_type;
    typedef T                                     value_type;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    void fill(const T& u) 
    {
      fill_n(begin(), N, u);
    }

    void swap(array<T, N> & y)
    {
      //swap_ranges(x.begin(), x.end(), y.begin() );
      for ( size_t i = 0; i != size(); ++i )
        std::swap(__elems[i], y.__elems[i]);
    }

    ///\name  iterators

    iterator                begin()       { return &__elems[0]; }
    const_iterator          begin() const { return &__elems[0]; }
    iterator                end()         { return &__elems[N]; }
    const_iterator          end()   const { return &__elems[N]; }

    reverse_iterator        rbegin()      { return reverse_iterator(end()); }
    const_reverse_iterator  rbegin() const
      { return const_reverse_iterator(end()); }
    reverse_iterator        rend()        { return reverse_iterator(begin()); }
    const_reverse_iterator  rend() const
      { return const_reverse_iterator(begin()); }

    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const   { return end(); }
    const_reverse_iterator crbegin() const { return rbegin(); }
    const_reverse_iterator crend() const   { return rend(); }

    ///\name  capacity

    size_type size() const      { return N; }
    size_type max_size() const  { return N; }
    bool empty() const          { return N != 0; }

    ///\name  element access

    reference       operator[](size_type n)       { return __elems[n]; }
    const_reference operator[](size_type n) const { return __elems[n]; }

    const_reference at(size_type n) const
    {
      __check_bounds(n);
      return operator[](n);
    }

    reference at(size_type n)
    {
      __check_bounds(n);
      return operator[](n);
    }

    reference       front()       { return *begin(); }
    const_reference front() const { return *begin(); }
    reference       back()        { return __elems[N-1]; }
    const_reference back()  const { return __elems[N-1]; }

          T * data()        { return __elems; }
    const T * data() const  { return __elems; }

    ///@}

  // Types with private or protected data members are not aggregate
  //private:

    /* vs2k5 debugger bug: it doesn't recognize `static const type`, we must use enum instead */
    //static const size_t __actual_size = N ? N : 1;
    enum { __actual_size = N ? N : 1 };

    T __elems[__actual_size];

    void __check_bounds(size_type n) const __ntl_throws(out_of_range)
    {
      if ( n > size() ) __ntl_throw (out_of_range());
    }

};//struct array

///\name  Array comparisons
template <class T, size_t N> bool operator== (const array<T,N>& x, const array<T,N>& y);
template <class T, size_t N> bool operator!= (const array<T,N>& x, const array<T,N>& y)
{
  return rel_ops::operator !=(x, y);
}

template <class T, size_t N> bool operator< (const array<T,N>& x, const array<T,N>& y);
template <class T, size_t N> bool operator> (const array<T,N>& x, const array<T,N>& y)
{
  return rel_ops::operator >(x, y);
}

template <class T, size_t N> bool operator<= (const array<T,N>& x, const array<T,N>& y)
{
  return rel_ops::operator <=(x, y);
}

template <class T, size_t N> bool operator>= (const array<T,N>& x, const array<T,N>& y)
{
  return rel_ops::operator >=(x, y);
}

///\name  Array specialized algorithms [6.2.2.2]
template <class T, size_t N > void swap(array<T,N>& x, array<T,N>& y)
{
  x.swap(y);
}

///@}
/**@} lib_sequence */
/**@} lib_containers */

}//namespace std

#endif//#ifndef NTL__STLX_ARRAY
