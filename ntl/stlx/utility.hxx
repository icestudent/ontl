/**\file*********************************************************************
 *                                                                     \brief
 *  Utility components [20.2 lib.utility]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_UTILITY
#define NTL__STLX_UTILITY

#include "type_traits.hxx"
#include "functional.hxx"

/// Standard Template Library
namespace std {

/**\defgroup  lib_utilities *** 20 General utilities library [utilities] *****
 *
 *    Components used by other elements of the Standard C + + library.
 *@{
 */

/**\defgroup  lib_utility ************** Utility components [20.2] **********
 *
 *    basic function and class templates that are used throughout
 *    the rest of the library.
 *@{
 */

#pragma region operators
/// 20.2.1 Operators [operators]
/// 1 To avoid redundant definitions of operator!= out of operator== and
///   operators >, <=, and >= out of operator<, the library provides the following:
namespace rel_ops {

template<class T> inline
bool operator!=(const T & x, const T & y) { return !(x == y); }

template<class T> inline
bool operator> (const T & x, const T & y) { return y < x; }

template<class T> inline
bool operator<=(const T & x, const T & y) { return !(y < x); }

template<class T> inline
bool operator>=(const T & x, const T & y) { return !(x < y); }

}//namespace rel_ops
#pragma endregion

#pragma region forward
/// 20.2.2 forward/move helpers [forward]
#ifdef NTL__CXX_RV
template <class T>
struct identity
{
  typedef T type;

  const T& operator()(const T& x) const { return x; }
};

template <class T>
inline
T&& forward(typename identity<T>::type&& t)
{
  return t;
}

template <class T> 
inline
typename remove_reference<T>::type&& move(T&& t)
{
  return t;
}
#endif
#pragma endregion

#pragma region pairs
/// Pairs [20.2.3 pairs]
#pragma warning(push)
// assignment operator could not be generated if either T is const
#pragma warning(disable:4512)
template<class T1, class T2>
struct pair
{
    typedef T1  first_type;
    typedef T2  second_type;

    T1  first;
    T2  second;


    pair() : first(T1()), second(T2()) 
    {}

    pair(const T1 & x, const T2 & y) : first(x), second(y) 
    {}

    template<class U, class V>
    pair(const pair<U, V>& p)
      :first(p.first), second(p.second)
    {}

#ifdef NTL__CXX_RV
    template<class U, class V> 
    pair(U&& x, V&& y)
      :first(forward<U>(x)), second(forward<V>(y))
    {}

    pair(pair&& p)
      :first(move(p.first)), second(move(p.second))
    {}

    template<class U, class V>
    pair(pair<U, V>&& p)
      :first(move(p.first)), second(move(p.second))
    {}
#endif

#ifdef NTL__CXX_VT
    template<class U, class... Args>
    pair(U&& x, Args&&... args)
      :first(forward<U>(x), second(forward<Args>(args))
    {}
#endif

    // allocator-extended constructors
    template <class Alloc> pair(allocator_arg_t, const Alloc& a);
    template <class Alloc> pair(allocator_arg_t, const Alloc& a, const T1& x, const T2& y);

#ifdef NTL__CXX_RV
    template <class U, class V, class Alloc>
    pair(allocator_arg_t, const Alloc& a, U&& x, V&& y);

    template <class Alloc> pair(allocator_arg_t, const Alloc&, pair&& p);
#endif

    template <class U, class V, class Alloc>
    pair(allocator_arg_t, const Alloc& a, const pair<U, V>& p);

#ifdef NTL__CXX_RV
    template <class U, class V, class Alloc>
    pair(allocator_arg_t, const Alloc& a, pair<U, V>&& p);
#endif

#ifdef NTL__CXX_VT
    template <class U, class... Args, class Alloc>
    pair(allocator_arg_t, const Alloc& a, U&& x, Args&&... args);
#endif

#ifdef NTL__CXX_RV
    pair& operator=(pair&& p)
    {
      first = move(p.first);
      second = move(p.second);
      return *this;
    }

    template<class U, class V>
    pair& operator=(pair<U, V>&& p)
    {
      first = move(p.first);
      second = move(p.second);
      return *this;
    }

    void swap(pair&& p)
    {
      std::swap(first, p.first);
      std::swap(second, p.second);
    }
#else
    void swap(pair& p)
    {
      std::swap(first, p.first);
      std::swap(second, p.second);
    }
#endif
};
#pragma warning(pop)

// forward declarations
template<class T, class Alloc> struct uses_allocator;
template <class T> struct constructible_with_allocator_suffix;
template <class T> struct constructible_with_allocator_prefix;

// 20.2.3.2
template <class T1, class T2, class Alloc>
struct uses_allocator<pair<T1, T2>, Alloc>: true_type {};

// 20.2.3.3
template <class T1, class T2>
struct constructible_with_allocator_prefix<pair<T1, T2> >: true_type {};

///\name  Comparisons
///@{

template<class T1, class T2>
inline
bool
  operator==(const pair<T1, T2> & x, const pair<T1, T2> & y)
{
  return x.first == y.first && x.second == y.second;
}

template<class T1, class T2>
inline
bool
  operator<(const pair<T1, T2> & x, const pair<T1, T2> & y)
{
  const bool less = x.first < y.first;
  return less || (!less && x.second < y.second);
}

template<class T1, class T2>
inline
bool
  operator!=(const pair<T1, T2> & x, const pair<T1, T2> & y)
{
  return !(x == y);
}

template<class T1, class T2>
inline
bool
  operator> (const pair<T1, T2> & x, const pair<T1, T2> & y)
{
  return y < x;
}

template<class T1, class T2>
inline
bool
  operator>=(const pair<T1, T2> & x, const pair<T1, T2> & y)
{
  return !(y < x);
}

template<class T1, class T2>
inline
bool
  operator<=(const pair<T1, T2> & x, const pair<T1, T2> & y)
{
  return !(x < y);
}

template<class T1, class T2> void swap(pair<T1, T2>& x, pair<T1, T2>& y)
{
  x.swap(y);
}
#ifdef NTL__CXX_RV
template<class T1, class T2> void swap(pair<T1, T2>&& x, pair<T1, T2>& y)
{
  x.swap(y);
}
template<class T1, class T2> void swap(pair<T1, T2>& x, pair<T1, T2>&& y)
{
  x.swap(y);
}

#endif
///@}

#ifdef NTL__CXX_RV
namespace __
{
  template<class T>
  struct pair_type
  {
    typedef typename decay<T>::type U;
    
    typedef typename conditional<is_same<U, reference_wrapper<T> >::value, 
                                 T&, 
                                 U
                                >::type type;
  };
  template<class T1, class T2>
  struct pair_result
  {
    typedef typename pair_type<T1>::type V1;
    typedef typename pair_type<T2>::type V2;
    typedef typename pair<V1, V2> type;
  };
}

template<class T1, class T2>
inline
typename __::pair_result<T1, T2>::type
make_pair(T1&& x, T2&& y)
{
  return __::pair_result<T1, T2>::type( forward<T1>(x), forward<T2>(y) );
}
#else
template<class T1, class T2>
inline
pair<T1, T2>
  make_pair(T1 x, T2 y)
{
  return pair<T1, T2>( x, y );
}
#endif
#pragma endregion

/**@} lib_utility */
/**@} lib_utilities */

}//namespace std

#endif//#ifndef NTL__STLX_UTILITY
