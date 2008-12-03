/**\file*********************************************************************
 *                                                                     \brief
 *  Tuples [20.4 tuple]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TUPLE
#define NTL__STLX_TUPLE

#include "ext/ttl/typelist.hxx"

namespace std
{

/**\defgroup  lib_utilities *** 20 General utilities library [utilities] *****
 *
 *    Components used by other elements of the Standard C + + library.
 *@{
 */

/**\defgroup  lib_tuple ************** 20.4 Tuples [tuple] **********
 *
 *   Tuple library that provides a tuple type as the class template tuple that can
 *   be instantiated with any number of arguments.
 *@{
 */
  template<typename T1 = ttl::meta::empty_type, typename T2 = ttl::meta::empty_type, typename T3 = ttl::meta::empty_type>
  class tuple;

  template<typename T1 = ttl::meta::empty_type, typename T2 = ttl::meta::empty_type, typename T3 = ttl::meta::empty_type>
  struct tuple_size;

  template<size_t I, typename T1 = ttl::meta::empty_type, typename T2 = ttl::meta::empty_type, typename T3 = ttl::meta::empty_type>
  struct tuple_element;


  namespace __
  {
    namespace meta = ttl::meta;

    template<typename T>
    struct traits
    {
      typedef T  stored_type;
      typedef T& return_type;
      typedef const typename remove_cv<T>::type&  param_type;
      typedef typename add_rvalue_reference<typename remove_cv<T>::type>::type rparam_type;
    };

    template<typename T>
    struct traits<const T>
    {
      typedef const T  stored_type;
      typedef const T& return_type;
      typedef const T& param_type;
      typedef typename add_rvalue_reference<const T>::type rparam_type;
    };

    template<typename T>
    struct traits<T&>
    {
      typedef T& stored_type;
      typedef T& return_type;
      typedef T& param_type;
      typedef typename add_rvalue_reference<T>::type rparam_type;
    };

    template<typename T>
    struct traits<const T&>
    {
      typedef const T& stored_type;
      typedef const T& return_type;
      typedef const T& param_type;
      typedef typename add_rvalue_reference<const T>::type rparam_type;
    };

#ifdef NTL__CXX_RV
    template<typename T>
    struct traits<T&&>
    {
      typedef T&& stored_type;
      typedef T&& return_type;
      typedef T&& param_type;
      typedef T&& rparam_type;
    };

    template<typename T>
    struct traits<const T&&>
    {
      typedef const T&& stored_type;
      typedef const T&& return_type;
      typedef const T&& param_type;
      typedef const T&& rparam_type;
    };
#endif

    template<typename T>
    struct traits<T*>
    {
      typedef T* stored_type;
      typedef T* param_type;
      typedef typename add_lvalue_reference<T*>::type return_type;
      typedef typename add_rvalue_reference<T*>::type rparam_type;
    };

    template<typename T>
    struct traits<const T*>
    {
      typedef const T* stored_type;
      typedef const T* param_type;
      typedef typename add_lvalue_reference<const T*>::type return_type;
      typedef typename add_rvalue_reference<const T*>::type rparam_type;
    };

    //template<typename T, size_t N>
    //struct traits<T[N]>
    //{
    //  typedef const T(&param_type)[N];
    //  typedef const T* stored_type;
    //  typedef typename add_lvalue_reference<const T*>::type return_type;
    //  typedef typename add_rvalue_reference<const T*>::type rparam_type;
    //};

    template<typename T, size_t N>
    struct traits<const T[N]>
    {
      //typedef const T(&param_type)[N];
      typedef const T* stored_type;
      typedef stored_type param_type;
      typedef typename add_lvalue_reference<const T*>::type return_type;
      typedef typename add_rvalue_reference<const T*>::type rparam_type;
    };


    template<typename T>
    struct data_wrapper
    {};

    template<typename Typelist, size_t Idx, bool Stop = (Idx > meta::length<Typelist>::value) >
    struct tuple_param
    {
      typedef typename traits<typename meta::get<Typelist,Idx-1>::type>::param_type type;
    };

    template<typename Typelist, size_t Idx>
    struct tuple_param<Typelist, Idx, true>
    {
      typedef meta::empty_type type;
    };

#ifdef NTL__CXX_RV

    template<class Types, size_t i, bool = (i >= meta::length<Types>::value)>
    struct rparam
    {
      typedef typename meta::get<Types,i>::type ft, type;
    };

    template<class Types, size_t i>
    struct rparam<Types, i, true> { typedef meta::empty_type type; };

#endif
    struct rvtag {};

    template<class Typelist, size_t Idx = 0, bool empty = (Idx >= meta::length<Typelist>::value)>
    struct tuples
    {
      typedef Typelist types;
      typedef typename meta::get<types, Idx>::type raw_type;
      typedef tuples<types, Idx+1> tail_t;

      typedef typename conditional<is_void<raw_type>::value || is_function<raw_type>::value, data_wrapper<raw_type>, raw_type>::type head_t;

      typename traits<head_t>::stored_type head;
      tail_t tail;

    public:
      tuples()
        :head(), tail()
      {}

      explicit tuples(typename tuple_param<types, Idx+1>::type p1)
        :head(p1), tail()
      {}
      explicit tuples(typename tuple_param<types, Idx+1>::type p1, typename tuple_param<types, Idx+2>::type p2)
        :head(p1), tail(p2)
      {}
      explicit tuples(typename tuple_param<types, Idx+1>::type p1, typename tuple_param<types, Idx+2>::type p2, typename tuple_param<types, Idx+3>::type p3)
        :head(p1), tail(p2, p3)
      {}

      template<typename U1, typename U2, typename U3>
      tuples(const tuples<meta::typelist<U1,U2,U3>, Idx>& r)
        :head(r.head), tail(r.tail)
      {}

      tuples(const tuples& r)
        :head(r.head), tail(r.tail)
      {}

      tuples& operator=(const tuples& r)
      {
        if(this == &r) return *this;
        head = r.head;
        tail = r.tail;
        return *this;
      }

      template<typename U1, typename U2, typename U3>
      tuples& operator=(const tuples<meta::typelist<U1,U2,U3>, Idx>& r)
      {
        head = r.head;
        tail = r.tail;
        return *this;
      }

#ifdef NTL__CXX_RV
      typedef typename rparam<types,Idx+0>::type type1;
      typedef typename rparam<types,Idx+1>::type type2;
      typedef typename rparam<types,Idx+2>::type type3;

      tuples(rvtag, type1&& p1)
        :head(forward<type1>(p1))
      {}
      tuples(rvtag, type1&& p1, type2&& p2)
        :head(forward<type1>(p1)), tail(rvtag(), p2)
      {}
      tuples(rvtag, type1&& p1, type2&& p2, type3&& p3)
        :head(forward<type1>(p1)), tail(rvtag(), p2, p3)
      {}

      template<typename U1, typename U2, typename U3>
      tuples(tuples<meta::typelist<U1,U2,U3>, Idx>&& r)
        :head(forward<head_t>(r.head)), tail(forward<tail_t>(r.tail))
      {}

      tuples(tuples&& r)
        :head(forward<head_t>(r.head)), tail(forward<tail_t>(r.tail))
      {}

      tuples& operator=(tuples&& r)
      {
        if(this == &r) return *this;
        head = move(r.head);
        tail = move(r.tail);
        return *this;
      }

      template<typename U1, typename U2, typename U3>
      tuples& operator=(tuples<meta::typelist<U1,U2,U3>, Idx>&& r)
      {
        head = move(r.head);
        tail = move(r.tail);
        return *this;
      }
#endif
    };

    template<class Typelist, size_t Idx>
    struct tuples<Typelist, Idx, true>
    {
      typedef Typelist types;
    public:
      tuples()
      {}

      template<typename U1, typename U2, typename U3>
      tuples(const tuples<meta::typelist<U1,U2,U3>, Idx>&)
      {}

      tuples(const tuples&)
      {}

      tuples& operator=(const tuples&)
      { return *this; }

      template<typename U1, typename U2, typename U3>
      tuples& operator=(const tuples<meta::typelist<U1,U2,U3>, Idx>&)
      { return *this; }

#ifdef NTL__CXX_RV
      template<typename U1, typename U2, typename U3>
      tuples(tuples<meta::typelist<U1,U2,U3>, Idx>&&)
      {}

      tuples(tuples&&)
      {}

      tuples& operator=(tuples&&)
      { return *this; }

      template<typename U1, typename U2, typename U3>
      tuples& operator=(tuples<meta::typelist<U1,U2,U3>, Idx>&&)
      { return *this; }
#endif

      explicit tuples(meta::empty_type)
      {}
      explicit tuples(meta::empty_type, meta::empty_type)
      {}
      explicit tuples(meta::empty_type, meta::empty_type, meta::empty_type)
      {}
    }; // tuples


    template<typename R, size_t N>
    struct field
    {
      template<typename T>
      static R get(T& t)
      {
        return field<R, N-1>::get(t.tail);
      }
      template<typename T>
      static R get(const T& t)
      {
        return field<R, N-1>::get(t.tail);
      }
    };

    template<typename R>
    struct field<R, 0>
    {
      template<typename T>
      static R get(T& t)
      {
        return t.head;
      }
      template<typename T>
      static R get(const T& t)
      {
        return t.head;
      }
    };

    template<typename T>
    struct is_tuple: false_type{};
    template<typename T1, typename T2, typename T3>
    struct is_tuple<tuple<T1,T2,T3> >: true_type{};
  }


  template<typename T1, typename T2, typename T3>
  class tuple:
    public __::tuples<ttl::meta::typelist<T1,T2,T3> >
  {
  public:
    typedef ttl::meta::typelist<T1,T2,T3> types;
  private:
    typedef __::rvtag rvtag;
    typedef __::tuples<types> base;
    typedef tuple<T1,T2,T3>   this_t;

  public:
    tuple()
    {}

    // explicit tuple(const Types&...);
    explicit tuple(typename __::tuple_param<types,1>::type p1)
      :base(p1)
    {}
    explicit tuple(typename __::tuple_param<types,1>::type p1, typename __::tuple_param<types,2>::type p2)
      :base(p1, p2)
    {}
    explicit tuple(typename __::tuple_param<types,1>::type p1, typename __::tuple_param<types,2>::type p2, typename __::tuple_param<types,3>::type p3)
      :base(p1, p2, p3)
    {}

    tuple(const tuple& r)
      :base(static_cast<const base&>(r))
    {}

    //tuple(tuple& r)
    //  :base(static_cast<base&>(r))
    //{}

    // template <class... UTypes> tuple(const tuple<UTypes...>&);
    template<typename U1, typename U2, typename U3>
    tuple(const tuple<U1,U2,U3>& r)
      :base(r)
    {}

    tuple& operator=(const tuple& r)
    {
      base::operator=(r);
      return *this;
    }

    // template <class... UTypes> tuple& operator=(const tuple<UTypes...>&);
    template<typename U1, typename U2, typename U3>
    tuple& operator=(const tuple<U1,U2,U3>& r)
    {
      base::operator=(r);
      return *this;
    }


#ifdef NTL__CXX_RV

    // template <class... UTypes> explicit tuple(UTypes&&... u);
    // BUG: compiler selects 'tuple(U1&&) instead of 'tuple(const tuple&)', e.g. tuple<int> x1, x2(x1);
    template<typename U1>
    explicit tuple(U1&& p1, typename enable_if<is_convertible<U1,typename __::rparam<types,0>::type>::value>::type* = 0)
      :base(rvtag(), forward<U1>(p1))
    {}

    template<typename U1, typename U2>
    explicit tuple(U1&& p1, U2&& p2)
      :base(rvtag(), forward<U1>(p1), forward<U2>(p2))
    {}

    template<typename U1, typename U2, typename U3>
    explicit tuple(U1&& p1, U2&& p2, U3&& p3)
      :base(rvtag(), forward<U1>(p1), forward<U2>(p2), forward<U3>(p3))
    {}

    tuple(tuple&& r)
      :base(forward<base>(r))
    {}

    tuple& operator=(tuple&& r)
    {
      base::operator=(forward<base>(r));
      return *this;
    }


    // template <class... UTypes> tuple(tuple<UTypes...>&&);
    template<typename U1, typename U2, typename U3>
    tuple(tuple<U1,U2,U3>&& r)
      :base(move(r))
    {}

    // template <class... UTypes> tuple& operator=(tuple<UTypes...>&&);
    template<typename U1, typename U2, typename U3>
    tuple& operator=(tuple<U1,U2,U3>&& r)
    {
      base::operator=(r);
      return *this;
    }
#endif

    // pair conversions
    template <class U1, class U2>
    tuple(const pair<U1, U2>& p)
      :base(p.first, p.second)
    {
      static_assert(types::length == 2, "invalid conversion");
    }

    template <class U1, class U2>
    tuple& operator=(const pair<U1, U2>& p)
    {
      static_assert(types::length == 2, "invalid conversion");
      return *this = tuple<U1,U2>(p);
    }

#ifdef NTL__CXX_RV
    template <class U1, class U2>
    tuple(pair<U1, U2>&& p)
      :base(forward<U1>(p.first), forward<U2>(p.second))
    {
      static_assert(types::length == 2, "invalid conversion");
    }

    template <class U1, class U2>
    tuple& operator=(pair<U1, U2>&& p)
    {
      static_assert(types::length == 2, "invalid conversion");
      return *this = tuple<U1,U2>(forward<pair<U1,U2> >(p));
    }
#endif
  }; // class tuple

  // make tuple
  namespace __
  {
    struct swallow_assign
    {
      template<typename T>
      const swallow_assign& operator=(const T&) const
      { return *this; }
    };

    template<typename T>
    struct is_refwrap
    {
      static const bool value = false;
      typedef T type;
    };
    template<typename T>
    struct is_refwrap<reference_wrapper<T> >
    {
      static const bool value = true;
      typedef T type;
    };

    template<typename T>
    struct mktraits
    {
      typedef typename decay<T>::type U;
      typedef typename conditional<is_refwrap<U>::value, typename is_refwrap<U>::type&, U>::type type;
    };

    template<typename T1 = meta::empty_type, typename T2 = meta::empty_type, typename T3 = meta::empty_type>
    struct tmap
    {
      typedef tuple<typename mktraits<T1>::type, typename mktraits<T2>::type, typename mktraits<T3>::type> type;
    };

  }

  /// "ignore" allows tuple positions to be ignored when using "tie".
  __::swallow_assign const ignore = __::swallow_assign();

  inline tuple<ttl::meta::empty_type,ttl::meta::empty_type,ttl::meta::empty_type> make_tuple()
  {
    return tuple<ttl::meta::empty_type,ttl::meta::empty_type,ttl::meta::empty_type>();
  }

#ifdef NTL__CXX_RV
  template<typename T1>
  inline typename __::tmap<T1>::type make_tuple(T1&& p1)
  {
    return __::tmap<T1>::type (forward<T1>(p1));
  }
  template<typename T1, typename T2>
  inline typename __::tmap<T1,T2>::type make_tuple(T1&& p1, T2&& p2)
  {
    return __::tmap<T1,T2>::type (forward<T1>(p1),forward<T2>(p2));
  }
  template<typename T1, typename T2, typename T3>
  inline typename __::tmap<T1,T2,T3>::type make_tuple(T1&& p1, T2&& p2, T3&& p3)
  {
    return __::tmap<T1,T2,T3>::type (forward<T1>(p1),forward<T2>(p2),forward<T3>(p3));
  }
#else
  template<typename T1>
  inline typename __::tmap<T1>::type make_tuple(const T1& p1)
  {
    return __::tmap<T1>::type (p1);
  }
  template<typename T1, typename T2>
  inline typename __::tmap<T1,T2>::type make_tuple(const T1& p1, const T2& p2)
  {
    return __::tmap<T1,T2>::type (p1,p2);
  }
  template<typename T1, typename T2, typename T3>
  inline typename __::tmap<T1,T2,T3>::type make_tuple(const T1& p1, const T2& p2, const T3& p3)
  {
    return __::tmap<T1,T2,T3>::type (p1,p2,p3);
  }
#endif

  // tie
  template<typename T1>
  inline tuple<T1&> tie(T1& p1)
  {
    return tuple<T1&> (p1);
  }
  template<typename T1, typename T2>
  inline tuple<T1&,T2&> tie(T1& p1, T2& p2)
  {
    return tuple<T1&,T2&> (p1,p2);
  }
  template<typename T1, typename T2, typename T3>
  inline tuple<T1&,T2&,T3&> tie(T1& p1, T2& p2, T3& p3)
  {
    return tuple<T1&,T2&,T3&> (p1,p2,p3);
  }


  // tuple size
  template<typename T1, typename T2, typename T3>
  struct tuple_size<tuple<T1,T2,T3> >:
    integral_constant<size_t, ttl::meta::length<typename tuple<T1,T2,T3>::types>::value>
  {};

  template<size_t I, typename T1, typename T2, typename T3>
  struct tuple_element<I, tuple<T1,T2,T3> >
  {
    typedef tuple<T1,T2,T3> tuple_t;
    static_assert(I < tuple_size<tuple_t>::value, "I is out of bounds");

    typedef typename ttl::meta::get<typename tuple_t::types, I>::type type;
  };

  // get(tuple<>)
  namespace __
  {
    template<size_t I, class Tuple>
    struct tuple_return
    {
      typedef typename ttl::meta::get<typename Tuple::types, I>::type found_type;
      typedef typename traits<found_type>::return_type type;
      typedef typename traits<typename add_const<found_type>::type>::return_type ctype;
    };

    template<class T1, class T2>
    struct tuple_pair
    {
      T1& get(pair<T1, T2>& p, meta::int2type<0>)
      {
        return p.first;
      }
      T2& get(pair<T1, T2>& p, meta::int2type<1>)
      {
        return p.second;
      }
      const T1& get(const pair<T1, T2>& p, meta::int2type<0>)
      {
        return p.first;
      }
      const T2& get(const pair<T1, T2>& p, meta::int2type<1>)
      {
        return p.second;
      }
    };
  }

  template<size_t I, typename T1>
  inline typename __::tuple_return<I, tuple<T1> >::type get(tuple<T1>& t)
  {
    typedef typename __::tuple_return<I, tuple<T1> >::type RT;
    return __::field<RT, I>::get(t);
  }

  template<size_t I, typename T1>
  inline typename __::tuple_return<I, tuple<T1> >::ctype get(const tuple<T1>& t)
  {
    typedef typename __::tuple_return<I, tuple<T1> >::ctype RT;
    return __::field<RT, I>::get(t);
  }

  template<size_t I, typename T1, typename T2>
  inline typename __::tuple_return<I, tuple<T1,T2> >::type get(tuple<T1,T2>& t)
  {
    typedef typename __::tuple_return<I, tuple<T1,T2> >::type RT;
    return __::field<RT, I>::get(t);
  }

  template<size_t I, typename T1, typename T2>
  inline typename __::tuple_return<I, tuple<T1,T2> >::ctype get(const tuple<T1,T2>& t)
  {
    typedef typename __::tuple_return<I, tuple<T1,T2> >::ctype RT;
    return __::field<RT, I>::get(t);
  }

  template<size_t I, typename T1, typename T2, typename T3>
  inline typename __::tuple_return<I, tuple<T1,T2,T3> >::type get(tuple<T1,T2,T3>& t)
  {
    typedef typename __::tuple_return<I, tuple<T1,T2,T3> >::type RT;
    return __::field<RT, I>::get(t);
  }

  template<size_t I, typename T1, typename T2, typename T3>
  inline typename __::tuple_return<I, tuple<T1,T2,T3> >::ctype get(const tuple<T1,T2,T3>& t)
  {
    typedef typename __::tuple_return<I, tuple<T1,T2,T3> >::ctype RT;
    return __::field<RT, I>::get(t);
  }

  //////////////////////////////////////////////////////////////////////////
  ///\name  Tuple interface to Pairs [20.2.3/27]

  template <class T1, class T2> 
  struct tuple_size<std::pair<T1, T2> >
    :integral_constant<size_t, 2>
  {};

  template <size_t I, class T1, class T2>
  struct tuple_element<I, std::pair<T1, T2> >; // undefined
  template <class T1, class T2>
  struct tuple_element<0, std::pair<T1, T2> > { typedef T1 type; };
  template <class T1, class T2> 
  struct tuple_element<1, std::pair<T1, T2> > { typedef T2 type; };

  template<size_t I, class T1, class T2>
  typename tuple_element<I, std::pair<T1, T2> >::type& get(std::pair<T1, T2>& p)
  {
    static_assert(I < 2, "I out of bounds");
    return __::tuple_pair<T1, T2>::get(p, ttl::meta::int2type<I>());
  }

  template<size_t I, class T1, class T2> 
  const typename tuple_element<I, std::pair<T1, T2> >::type& get(const std::pair<T1, T2>& p)
  {
    static_assert(I < 2, "I out of bounds");
    return __::tuple_pair<T1, T2>::get(p, ttl::meta::int2type<I>());
  }


  //////////////////////////////////////////////////////////////////////////
  ///\name  Tuple interface to class template array [23.2.1.6]

  template <class T, size_t N> 
  struct tuple_size<array<T, N> >: integral_constant<size_t, N>
  {};

  template <size_t I, class T, size_t N>
  struct tuple_element<I, array<T, N> >
  {
    static_assert(I < N, "I out of bounds");
    typedef T type;
  };

  template <size_t I, class T, size_t N>
  T& get(array<T, N>& a)
  {
    static_assert(I < N, "I out of bounds");
    return a[I];
  }

  template <size_t I, class T, size_t N>
  const T& get(const array<T, N>& a)
  {
    static_assert(I < N, "I out of bounds");
    return a[I];
  }


 /**@} lib_tuple */
 /**@} lib_utilities */

}//namespace std
//  Copyright (c) 2003 Eugene Gladyshev
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
#endif//#ifndef NTL__STLX_TUPLE
