/**\file*********************************************************************
 *                                                                     \brief
 *  Utility components [20.3 lib.utility]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_UTILITY
#define NTL__STLX_UTILITY
#pragma once

#ifndef NTL__STLX_TYPE_TRAITS
#include "type_traits.hxx"
#endif

#ifndef NTL__BASEDEF
#include "../basedef.hxx" // for noncopyable
#endif

/// Standard Template Library
namespace std 
{

/**\defgroup  std_depr **************** C++ Standard Compatibility features [depr]
 *
 * This Clause describes features of the C++ Standard that are specified for compatibility with existing implementations.
 * These are deprecated features, where deprecated is defined as: Normative for the current edition of the
 * Standard, but not guaranteed to be part of the Standard in future revisions.
 **/

/**\addtogroup  lib_utilities *** 20 General utilities library [utilities]
 *@{
 **/

/**\addtogroup  lib_utility ***** 20.3 Utility components [utility]
 *
 *    basic function and class templates that are used throughout
 *    the rest of the library.
 *@{
 */

#pragma region operators
  /**
   *  @brief Relation operators
   *
   *	20.3.1 Operators [operators]\n
   *	To avoid redundant definitions of <tt>operator!=</tt> out of <tt>operator==</tt> and
   *	<tt>operators ></tt>, <tt><=</tt>, and <tt>>=</tt> out of <tt>operator<</tt>, the library provides the following:
   **/
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
///\name 20.2.3 forward/move helpers [forward]
#ifdef NTL__CXX_RV
#ifdef NTL__CXX_RVFIX

  // rvalue ref v2
  template <class T> inline T&& forward(typename std::remove_reference<T>::type& t) { return static_cast<T&&>(t); }
  template <class T> inline T&& forward(typename std::remove_reference<T>::type&& t){ return static_cast<T&&>(t); }
  template <class T> inline typename std::remove_reference<T>::type&& move(T&& t) { return static_cast<typename std::remove_reference<T>::type&&>(t); }
#else
  template <class T>
  struct identity
  {
    typedef T type;
    const T& operator()(const T& x) const { return x; }
  };
  template <class T>
  inline T&& forward(typename identity<T>::type&& t) { return t; }

  template <class T> 
  inline typename remove_reference<T>::type&& move(T&& t) { return t; }
# endif // rvalue ref v2

  template <class T> typename conditional<
    !has_nothrow_move_constructor<T>::value && has_copy_constructor<T>::value,
    const T&, T&&>::type move_if_noexcept(T& x)
  {
    return std::move(x);
  }
#ifdef NTL__DOC   // declaration in <type_traits_fwd.hxx>
  /** 20.2.4 Function template declval */
  template <class T>
  typename add_rvalue_reference<T>::type declval() __ntl_nothrow;
#endif

#else
  template <class T>
  struct identity
  {
    typedef T type;
    const T& operator()(const T& x) const { return x; }
  };

# if 0
  ///\name move emulation

  template<typename T>
  inline typename enable_if<std::is_convertible<T&, _rvalue<T> >::value, _rvalue<T> >::type move(const T& t)
  {
    return _rvalue<T>(t);
  }

  template <class T>
  inline _rvalue<T> forward(const typename identity<T>::type& t) { return t; }

  template <class T>
  inline _rvalue<T> forward(_rvalue<T> t) { return t; }

  template<typename T>
  inline _rvalue<T> move(_rvalue<T> t) { return t; }
  ///\}

# else

  template <class T>
  inline const T& forward(const typename identity<T>::type& t) { return t; }

  template <class T>
  inline T& move(T& t) { return t; }

  template <class T>
  inline const T& move(const T& t) { return t; }
# endif

  template <class T> typename conditional<
    !has_nothrow_move_constructor<T>::value && has_copy_constructor<T>::value,
    const T&, T&>::type move_if_noexcept(T& x)
  {
    return std::move(x);
  }

  // declaration in <type_traits_fwd.hxx>
  //template <class T>
  //T declval() __ntl_nothrow;

#endif


#pragma endregion

#pragma region pairs

  template<class T>
  inline void swap(T& a, T& b);

/**\addtogroup  lib_pairs *****  20.3 Pairs [pairs]
 *@{*/

  ///\name 20.3.5 Piecewise construction [pair.piecewise]
  struct piecewise_construct_t { };

#ifdef NTL__CXX_CONSTEXPR
  constexpr piecewise_construct_t piecewise_construct = piecewise_construct_t();
#else
  extern __declspec(selectany) piecewise_construct_t piecewise_construct = {};
#endif
  ///\}



  // forward
#ifdef NTL__CXX_VT_DONE
  // will be enabled when new tuple implemented
  template<typename...>
  class tuple;
#endif



#pragma warning(push)
#pragma warning(disable:4512) // assignment operator could not be generated if either T is const

  ///\name 20.3.2 Class template pair [pairs.pair]
template<class T1, class T2>
struct pair
{
    typedef T1  first_type;
    typedef T2  second_type;

    T1  first;
    T2  second;


    pair()
      :first(), second() 
    {}

    pair(const T1 & x, const T2 & y)
      :first(x), second(y) 
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
      :first(forward<U>(p.first)), second(forward<V>(p.second))
    {}
#endif

#ifdef NTL__CXX_VT_DONE
    template<class... Args1, class... Args2>
    pair(piecewise_construct_t, tuple<Args1...> a, tuple<Args2...> b);
#endif

    // allocator-extended constructors
    template <class Alloc> pair(allocator_arg_t, const Alloc& a);
    template <class Alloc> pair(allocator_arg_t, const Alloc& a, const T1& x, const T2& y);

    pair& operator=(const pair& p)
    {
      first = p.first, second = p.second;
      return *this;
    }

    template<class U, class V>
    pair& operator =(const pair<U,V>& p)
    {
      first = p.first, second = p.second;
      return *this;
    }

#ifdef NTL__CXX_RV
    pair& operator=(pair&& p)
    {
      first = forward<first_type>(p.first);
      second = forward<second_type>(p.second);
      return *this;
    }

    template<class U, class V>
    pair& operator=(pair<U, V>&& p)
    {
      first = forward<U>(p.first);
      second = forward<V>(p.second);
      return *this;
    }
#endif
    void swap(pair& p)
    {
      using std::swap;
      swap(first, p.first);
      swap(second, p.second);
    }
};
#pragma warning(pop)

///\name  Comparisons
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

template<class T1, class T2> inline void swap(pair<T1, T2>&  x, pair<T1, T2>&  y) { x.swap(y); }

///\name Make pair
#ifdef NTL__CXX_RV
template<typename T>
class reference_wrapper;

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
inline typename __::pair_result<T1, T2>::type make_pair(T1&& x, T2&& y)
{
  return __::pair_result<T1, T2>::type(forward<T1>(x), forward<T2>(y));
}

#else

template<class T1, class T2>
inline pair<T1, T2> make_pair(T1 x, T2 y)
{
  return pair<T1, T2>( x, y );
}

#endif // RV

///\name 20.3.6 pair range access [pair.range]
template <class InputIterator>
inline InputIterator begin(const std::pair<InputIterator, InputIterator>& p) { return p.first; }
template <class InputIterator>
inline InputIterator end(const std::pair<InputIterator, InputIterator>& p)   { return p.second;}



/**@} lib_pairs */
#pragma endregion

///\name 20.2.x.1 class noncopyable [utility.noncopyable] 
/**
 *	@brief 20.2.x.1 class noncopyable [utility.noncopyable] 
 *  @details Class noncopyable is provided to simplify creation of classes that inhibit copy semantics.
 **/
using ntl::noncopyable;

///\}
/**@} lib_utility */
/**@} lib_utilities */

}//namespace std

#endif//#ifndef NTL__STLX_UTILITY
