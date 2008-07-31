/**\file*********************************************************************
 *                                                                     \brief
 *  Utility components [20.2 lib.utility]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_UTILITY
#define NTL__STLX_UTILITY

#include "type_traits.hxx"

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
/// Operators [20.2.1 operators]
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
#ifdef NTL__CXX
template <class T> struct identity;
template <class T> T&& forward(typename identity<T>::type&&);
template <class T> typename remove_reference<T>::type&& move(T&&);
#endif
#pragma endregion

#pragma region pairs
/// Pairs [20.2.3 pairs]
template<class T1, class T2>
struct pair
{
    typedef T1  first_type;
    typedef T2  second_type;

    T1  first;
    T2  second;

    pair() : first(T1()), second(T2()) {}
    pair(const T1 & x, const T2 & y) : first(x), second(y) {}

    template<class U, class V>
    pair(const pair<U, V> & p) : first(p.first), second(p.second) {}

#ifdef NTL__CXX
    pair(pair&& p);
    pair(const T1& x, const T2& y);
    template<class U, class V> pair(U&& x, V&& y);
    template<class U, class V> pair(pair<U, V>&& p);
    template<class U, class... Args> pair(U&& x, Args&&... args);

    // allocator-extended constructors
    template <class Alloc> pair(allocator_arg_t, const Alloc& a);
    template <class Alloc> pair(allocator_arg_t, const Alloc& a, const T1& x, const T2& y);
    
    template <class U, class V, class Alloc>
    pair(allocator_arg_t, const Alloc& a, U&& x, V&& y);

    template <class Alloc> pair(allocator_arg_t, const Alloc&, pair&& p);

    template <class U, class V, class Alloc>
    pair(allocator_arg_t, const Alloc& a, const pair<U, V>& p);

    template <class U, class V, class Alloc>
    pair(allocator_arg_t, const Alloc& a, pair<U, V>&& p);

    template <class U, class... Args, class Alloc>
    pair(allocator_arg_t, const Alloc& a, U&& x, Args&&... args);

    pair& operator=(pair&& p);

    template<class U, class V> pair& operator=(pair<U, V>&& p);

    void swap(pair&& p);
#endif
};

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

///@}

template<class T1, class T2>
inline
pair<T1, T2>
  make_pair(T1 x, T2 y)
{ 
  return pair<T1, T2>( x, y );
}
#pragma endregion

/**@} lib_utility */
/**@} lib_utilities */

}//namespace std

#endif//#ifndef NTL__STLX_UTILITY
