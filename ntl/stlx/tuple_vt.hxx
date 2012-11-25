/**\file*********************************************************************
 *                                                                     \brief
 *  Tuples [20.4 tuple]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TUPLE_VT
#define NTL__STLX_TUPLE_VT
#pragma once

#define TTL_MAX_TEMPLATE_PARAMS 5
#define STLX_ENABLE_TUPLE_RELATIONS

#include "ext/typelist.hxx"

#ifndef NTL__STLX_UTILITY
#include "utility.hxx" // for tuple pair access
#endif

namespace std
{
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER <= 170051025)
# pragma warning(push)
# pragma warning(disable:4520)
#endif

/**\addtogroup  lib_utilities *** 20 General utilities library [utilities]
 *@{
 */

/**\addtogroup  lib_tuple ******* 20.4 Tuples [tuple]
 *
 *   Tuple library that provides a tuple type as the class template tuple that can be instantiated with any number of arguments.
 *@{
 */

  /// 20.4.2 Class template tuple [tuple.tuple]
  template<typename... Types>
  class tuple;

  /// 20.4.2.5 Tuple helper classes [tuple.helper]
  template <class T> struct tuple_size;
  //template <class T> struct tuple_size<const T>;
  //template <class T> struct tuple_size<volatile T>;
  //template <class T> struct tuple_size<const volatile T>;
  //template <class... Types> struct tuple_size<tuple<Types...> >;

  /// 20.4.2.5 Tuple helper classes [tuple.helper]
  template <size_t I, class T> struct tuple_element; // undefined
  //template <size_t I, class T> struct tuple_element<I, const T>;
  //template <size_t I, class T> struct tuple_element<I, volatile T>;
  //template <size_t I, class T> struct tuple_element<I, const volatile T>;
  //template <size_t I, class... Types> struct tuple_element<I, tuple<Types...> >;


  namespace __
  {
    namespace meta = ttl::meta;

    struct rvtag {};

    //////////////////////////////////////////////////////////////////////////
    template<size_t I, class... Types> class tuples;

    template<size_t I> class tuples<I>{};

    template<size_t I, class Head, class... Tail>
    class tuples<I, Head, Tail...>:
      public tuples<I+1, Tail...>
    {
      typedef Head head_t;
      typedef tuples<I+1, Tail...> tail_t;

    public:
      tuples()
        :value()
      {}

      explicit tuples(const Head& h, const Tail&... t)
        :tail_t(t...), value(h)
      {}

      template<typename UH, typename... UT,
        typename = enable_if<(sizeof...(UT) == sizeof...(Tail))>::type>
      explicit tuples(UH&& h, UT&&... t)
        :tail_t(forward<UT>(t)...), value(forward<UH>(h))
      {}

      tuples(tuples&& r)
        :tail_t(move(tail(r))), value(forward<Head>(head(r)))
      {}

      static const  head_t& head(const  tuples& h) { return h.value; }
      static head_t& head(tuples& h) { return h.value; }
      static tail_t& tail(tuples& h) { return h; }
    private:
      head_t value;
    };
  }

  template<typename... Types>
  class tuple:
    public __::tuples<0, Types...>
  {
    enum { length = sizeof...(Types) };

    typedef __::rvtag rvtag;
    typedef __::tuples<0, Types...> base;

  public:
    ///\name 20.4.2.1, tuple construction
    tuple()
    {}

    explicit tuple(const Types&... args)
      :base(args...)
    {
      static_assert((length == sizeof...(Types)), "incompatible number of types");
    }

    template <class... UTypes>
    explicit tuple(UTypes&&... r)
      :base(forward<UTypes>(r)...)
    {
      static_assert((length == sizeof...(Types)), "incompatible number of types");
    }

#ifdef NTL_CXX_EF
    tuple(const tuple&) = default;
    tuple(tuple&&) = default;
#endif

    template <class... UTypes>
    tuple(const tuple<UTypes...>& r)
      :base(r)
    {
      static_assert((length == tuple<UTypes...>::length), "incompatible number of types");
    }

    template <class... UTypes>
    tuple(tuple<UTypes...>&& r)
      :base(r)
    {
      static_assert((length == tuple<UTypes...>::length), "incompatible number of types");
    }

    template <class U1, class U2, typename = enable_if<sizeof...(Types) == 2>::type>
    tuple(const pair<U1, U2>& r)
      :base(r.first, r.second)
    {}

    template <class U1, class U2, typename = enable_if<sizeof...(Types) == 2>::type>
    tuple(pair<U1, U2>&& r)
      :base(forward<U1>(r.first), forward<U2>(r.second))
    {}

    ///\name allocator-extended constructors

    ///\name 20.4.2.2, tuple assignment
    tuple& operator=(const tuple&);

    tuple& operator=(tuple&&)
#ifdef NTL_CXX_NOEXCEPT
      noexcept(see below )
#endif
      ;
    
    template <class... UTypes>
    tuple& operator=(const tuple<UTypes...>&);
    
    template <class... UTypes>
    tuple& operator=(tuple<UTypes...>&&);
    
    template <class U1, class U2, typename = enable_if<sizeof...(Types) == 2>::type>
    tuple& operator=(const pair<U1, U2>&);
    
    template <class U1, class U2, typename = enable_if<sizeof...(Types) == 2>::type>
    tuple& operator=(pair<U1, U2>&&)
#ifdef NTL_CXX_NOEXCEPT
      noexcept(see below )
#endif
      ;

    // 20.4.2.3, tuple swap
    void swap(tuple&)
#ifdef NTL_CXX_NOEXCEPT
      noexcept(see below )
#endif
      ;
    

    // template <class... UTypes> tuple(const tuple<UTypes...>&);
    //template<typename U1, typename U2, typename U3, typename U4, typename U5>
    //tuple(const tuple<U1,U2,U3,U4,U5>& r)
    //  :base(r)
    //{
    //  static_assert((types::length == tuple<U1,U2,U3,U4,U5>::types::length), "incompatible number of types");
    //}

    //tuple& operator=(const tuple& r)
    //{
    //  base::operator=(r);
    //  return *this;
    //}

    //// template <class... UTypes> tuple& operator=(const tuple<UTypes...>&);
    //template<typename U1, typename U2, typename U3, typename U4, typename U5>
    //tuple& operator=(const tuple<U1,U2,U3,U4,U5>& r)
    //{
    //  static_assert((types::length == tuple<U1,U2,U3,U4,U5>::types::length), "incompatible number of types");
    //  base::operator=(r);
    //  return *this;
    //}


    // template <class... UTypes> explicit tuple(UTypes&&... u);
    // BUG: compiler selects 'tuple(U1&&) instead of 'tuple(const tuple&)', e.g. tuple<int> x1, x2(x1);
    //template<typename U1>
    //explicit tuple(U1&& p1, typename enable_if<is_convertible<U1,typename __::rparam<types,0>::type>::value>::type* = 0)
    //  :base(rvtag(), forward<U1>(p1))
    //{}


    //tuple(tuple&& r)
    //  :base(forward<base>(r))
    //{}

    //tuple& operator=(tuple&& r)
    //{
    //  base::operator=(forward<base>(r));
    //  return *this;
    //}


    // template <class... UTypes> tuple(tuple<UTypes...>&&);
    //template<typename U1, typename U2, typename U3, typename U4, typename U5>
    //tuple(tuple<U1,U2,U3,U4,U5>&& r)
    //  :base(move(r))
    //{
    //  static_assert((types::length == tuple<U1,U2,U3,U4,U5>::types::length), "incompatible number of types");
    //}

    //// template <class... UTypes> tuple& operator=(tuple<UTypes...>&&);
    //template<typename U1, typename U2, typename U3, typename U4, typename U5>
    //tuple& operator=(tuple<U1,U2,U3,U4,U5>&& r)
    //{
    //  static_assert((types::length == tuple<U1,U2,U3,U4,U5>::types::length), "incompatible number of types");
    //  base::operator=(r);
    //  return *this;
    //}

    // pair conversions
//    template <class U1, class U2>
//    tuple(const pair<U1, U2>& p)
//      :base(p.first, p.second)
//    {
//      static_assert(types::length == 2, "invalid conversion");
//    }
//
//    template <class U1, class U2>
//    tuple& operator=(const pair<U1, U2>& p)
//    {
//      static_assert(types::length == 2, "invalid conversion");
//      return *this = tuple<U1,U2>(p);
//    }
//
//#ifdef NTL_CXX_RV
//    template <class U1, class U2>
//    tuple(pair<U1, U2>&& p)
//      :base(forward<U1>(p.first), forward<U2>(p.second))
//    {
//      static_assert(types::length == 2, "invalid conversion");
//    }
//
//    template <class U1, class U2>
//    tuple& operator=(pair<U1, U2>&& p)
//    {
//      static_assert(types::length == 2, "invalid conversion");
//      return *this = tuple<U1,U2>(forward<pair<U1,U2> >(p));
//    }
//#endif
  }; // class tuple

  template<> class tuple<>
  {
  public:
    void swap(tuple&)
    {}
  };



  ///\name 20.4.1.3, tuple creation functions:
  template <class T> class reference_wrapper;

  namespace __
  {
    struct swallow_assign
    {
      template<typename T>
      const swallow_assign& operator=(const T&) const
      { return *this; }
    };

    template<typename T>
    struct is_refwrap: 
      integral_constant<bool, false>
    {
      typedef T type;
    };
    template<typename T>
    struct is_refwrap<reference_wrapper<T> >:
      integral_constant<bool, true>
    {
      typedef T type;
    };

    template<typename T>
    struct mktraits
    {
      typedef typename decay<T>::type U;
      typedef typename conditional<is_refwrap<U>::value, typename is_refwrap<U>::type&, U>::type type;
    };

    // TUPLE_EXT:
    template<typename... Types>
    struct tmap
    {
      typedef tuple<typename mktraits<Types>::type...> type;
    };

  }

  /// "ignore" allows tuple positions to be ignored when using "tie".
  static __::swallow_assign const ignore = {};

  inline tuple<> make_tuple()
  {
    return tuple<>();
  }


  ///\name 20.4.2.4 Tuple creation functions [tuple.creation]
  template<typename... Types>
  inline typename __::tmap<Types...>::type make_tuple(Types... p)
  {
    return __::tmap<Types...>::type (forward<Types>(p)...);
  }

  //
  template<typename... Types>
  inline tuple<Types&&...> forward_as_tuple(Types&&... t)
  {
    return tuple<Types&&...>(forward<Types>(t)...);
  }

  // tie
  template<typename... Types>
  inline tuple<Types&...> tie(Types&... t)
  {
    return tuple<Types&...> (t);
  }


  // tuple_cat
  namespace __
  {
    namespace nmeta = ntl::meta;
    template<class Tuple1, class Tuple2> struct tuple_glue;

    template<template<size_t,typename> class handler, class types, size_t len = types::length, size_t i = 0> struct tup2tl_do
    {
      typedef typename ttl::meta::get<types, i>::type ct;
      static const bool stop = is_same<ct,meta::empty_type>::value || handler<i,ct>::stop;

      typedef typename conditional<stop, meta::empty_type, typename tup2tl_do<handler,types,len,i+1>::type>::type next;
      typedef typename conditional<stop, meta::empty_type, nmeta::tlist<typename handler<i,ct>::type, next> >::type type;
    };
    template<template<size_t,typename> class handler, class types, size_t len> struct tup2tl_do<handler,types,len,len> { typedef meta::empty_type type; };

    template<template<size_t,typename> class handler, class types, size_t len = types::length> 
    struct tup2tl
    {
      typedef typename tup2tl_do<handler,types,len>::type result;
    };
    template<template<size_t,typename> class handler, class types> struct tup2tl<handler,types,0> { typedef nmeta::tlist<> result; };

    // tlist 2 tuple
    template<class TL, size_t len = nmeta::tl::length<TL>::value> struct tl2tup;
    template<class TL> struct tl2tup<TL,0> { typedef tuple<> type; };
    template<class TL> struct tl2tup<TL,1> { typedef tuple<typename nmeta::tl::get<0,TL>::type> type; };
    template<class TL> struct tl2tup<TL,2> { typedef tuple<typename nmeta::tl::get<0,TL>::type, typename nmeta::tl::get<1,TL>::type> type; };
    template<class TL> struct tl2tup<TL,3> { typedef tuple<typename nmeta::tl::get<0,TL>::type, typename nmeta::tl::get<1,TL>::type, typename nmeta::tl::get<2,TL>::type> type; };
    template<class TL> struct tl2tup<TL,4> { typedef tuple<typename nmeta::tl::get<0,TL>::type, typename nmeta::tl::get<1,TL>::type, typename nmeta::tl::get<2,TL>::type, typename nmeta::tl::get<3,TL>::type> type; };
    template<class TL> struct tl2tup<TL,5> { typedef tuple<typename nmeta::tl::get<0,TL>::type, typename nmeta::tl::get<1,TL>::type, typename nmeta::tl::get<2,TL>::type, typename nmeta::tl::get<3,TL>::type, typename nmeta::tl::get<4,TL>::type> type; };

    template<
      typename T1, typename T2, typename T3, typename T4, typename T5,
      typename U1, typename U2, typename U3, typename U4, typename U5
            >
    struct tuple_glue<tuple<T1,T2,T3,T4,T5>, tuple<U1,U2,U3,U4,U5> >
    {
      typedef tuple<T1,T2,T3,T4,T5> tup1;
      typedef tuple<U1,U2,U3,U4,U5> tup2;

      typedef typename tup2tl<nmeta::tl::skip_handler, typename tup1::types>::result filtered1;
      typedef typename tup2tl<nmeta::tl::skip_handler, typename tup2::types>::result filtered2;
      typedef typename nmeta::tl::append<filtered1,filtered2>::type filtered;
      typedef typename tl2tup<filtered>::type result;
      //SHOWT(result);
    };

    struct weak_empty { typedef meta::empty_type type; };
    template<class Tuple>
    struct sget
    {
      template<size_t I, bool = (I < Tuple::types::length)> struct rt2 { typedef typename mktraits<typename ttl::meta::get<typename Tuple::types, I>::type>::type type; };
      template<size_t I> struct rt2<I,false> { typedef meta::empty_type type; };
      template<size_t I>
      struct rt
      {
        typedef typename rt2<I>::type type;
      };

      template<size_t I, class T1, class T2>
      static typename rt<I>::type v(const T1& t1, const T2& t2)
      {
        static const bool e1 = I >= T1::types::length;
        static const size_t J = e1 ? (I - T1::types::length) : I;
        static const bool e2 = J >= T2::types::length;
        static const bool none = e1 && e2;

        typedef int2type<none ? 0 : !e1 ? 1 : 2> w; // 0 - out of range, 1 - t1, 2 - t2
        return get<J, typename rt<I>::type> (t1,t2,w());
      }

    private:
      typedef int2type<0> ignore;
      typedef int2type<1> left;
      typedef int2type<2> right;

      template<size_t I, class R, class T1, class T2>
      static R get(const T1&, const T2&, ignore) { return meta::empty_type(); }

      template<size_t I, class R, class T1, class T2>
      static R get(const T1& t1, const T2&, left){ return field2<R,I>::get(t1); }

      template<size_t I, class R, class T1, class T2>
      static R get(const T1&, const T2& t2, right){return field2<R,I>::get(t2); }
    };
  }

  template<class Tuple1, class Tuple2>
  typename __::tuple_glue<Tuple1,Tuple2>::result tuple_cat(const Tuple1& t1, const Tuple2& t2)
  {
    typedef typename __::tuple_glue<Tuple1,Tuple2>::result R;
    typedef __::sget<R> sget;
    R z(sget::v<0>(t1,t2), sget::v<1>(t1,t2), sget::v<2>(t1,t2), sget::v<3>(t1,t2), sget::v<4>(t1,t2));
    return R();
  }


  ///\name 20.4.1.4, tuple helper classes:
  // tuple size
  template<typename... Types>
  struct tuple_size<tuple<Types...> >:
    integral_constant<size_t, sizeof...(Types)>
  {};
  template<typename... Types>
  struct tuple_size<const tuple<Types...> >:
    tuple_size<tuple<Types...> >
  {};
  template<typename... Types>
  struct tuple_size<volatile tuple<Types...> >:
    tuple_size<tuple<Types...> >
  {};
  template<typename... Types>
  struct tuple_size<const volatile tuple<Types...> >:
    tuple_size<tuple<Types...> >
  {};


  // tuple_element
  template<size_t I, typename H, typename... T>
  struct tuple_element<I, tuple<H, T...> >
  {
    static_assert(I < tuple_size<tuple<H,T...> >::value, "I out of bounds" );
    typedef typename tuple_element<I-1, tuple<T...> >::type type;
  };
  template<typename H, typename... T>
  struct tuple_element<0, tuple<H, T...> >
  {
    typedef H type;
  };
  template<size_t I, class Tuple>
  struct tuple_element<I, const Tuple>
  {
    typedef const typename tuple_element<I, Tuple>::type type;
  };
  template<size_t I, class Tuple>
  struct tuple_element<I, volatile Tuple>
  {
    typedef volatile typename tuple_element<I, Tuple>::type type;
  };
  template<size_t I, class Tuple>
  struct tuple_element<I, const volatile Tuple>
  {
    typedef const volatile typename tuple_element<I, Tuple>::type type;
  };



  ///\name 20.4.1.5, element access:
  // get(tuple<>)
  namespace __
  {
    template<size_t I, typename H, typename... T>
    inline auto get_impl(const tuples<I,H,T...>& t) ->
      typename add_reference<typename add_const<H>::type>::type
    {
      return t.head(t);
    }
    template<size_t I, typename H, typename... T>
    inline auto get_impl(tuples<I,H,T...>& t) ->
      typename add_reference<H>::type
    {
      return t.head(t);
    }
  }

  template<size_t I, typename... Types>
  inline auto get(const tuple<Types...>& t) -> 
    typename add_reference<typename add_const<
      typename tuple_element<I, tuple<Types...> >::type
    >::type>::type
  {
    static_assert(I < tuple_size<tuple<Types...> >::value, "I out of bounds" );
    return __::get_impl<I>(t);
  }
  template<size_t I, typename... Types>
  inline auto get(tuple<Types...>& t) -> 
    typename add_reference<
      typename tuple_element<I, tuple<Types...> >::type
    >::type
  {
    static_assert(I < tuple_size<tuple<Types...> >::value, "I out of bounds" );
    return __::get_impl<I>(t);
  }
  template<size_t I, typename... Types>
  inline auto get(tuple<Types...>&& t) -> 
    typename add_rvalue_reference<
      typename tuple_element<I, tuple<Types...> >::type
    >::type
  {
    typedef tuple<Types...> tuple_t;
    static_assert(I < tuple_size<tuple_t>::value, "I out of bounds" );
    return forward<typename tuple_element<I, tuple_t>::type&&> ( get<I>(t) );
  }


  ///\name 20.4.1.6, relational operators:
  inline bool operator== (const tuple<>&, const tuple<>&) {  return true; }
  inline bool operator!= (const tuple<>&, const tuple<>&) {  return false; }
  inline bool operator<  (const tuple<>&, const tuple<>&) {  return false; }
  inline bool operator>  (const tuple<>&, const tuple<>&) {  return false; }
  inline bool operator<= (const tuple<>&, const tuple<>&) {  return true; }
  inline bool operator>= (const tuple<>&, const tuple<>&) {  return true; }

  template<typename... Types, typename... UTypes>
  inline bool operator == (const tuple<Types...>& x, const tuple<UTypes...>& y);

  template<typename... Types, typename... UTypes>
  inline bool operator != (const tuple<Types...>& x, const tuple<UTypes...>& y)
  {
    return rel_ops::operator != (x, y);
  }

  template<typename... Types, typename... UTypes>
  inline bool operator < (const tuple<Types...>& x, const tuple<UTypes...>& y);

  template<typename... Types, typename... UTypes>
  inline bool operator > (const tuple<Types...>& x, const tuple<UTypes...>& y)
  {
    return rel_ops::operator > (x, y);
  }

  template<typename... Types, typename... UTypes>
  inline bool operator <= (const tuple<Types...>& x, const tuple<UTypes...>& y)
  {
    return rel_ops::operator <= (x, y);
  }

  template<typename... Types, typename... UTypes>
  inline bool operator< (const tuple<Types...>& x, const tuple<UTypes...>& y)
  {
    return rel_ops::operator >= (x,y);
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
  inline typename tuple_element<I, std::pair<T1, T2> >::type& get(std::pair<T1, T2>& p)
  {
    static_assert(I < 2, "I out of bounds");
    return __::tuple_pair<T1, T2>::get(p, ttl::meta::int2type<I>());
  }

  template<size_t I, class T1, class T2> 
  inline const typename tuple_element<I, std::pair<T1, T2> >::type& get(const std::pair<T1, T2>& p)
  {
    static_assert(I < 2, "I out of bounds");
    return __::tuple_pair<T1, T2>::get(p, ttl::meta::int2type<I>());
  }
  ///\}

  ///\name 20.4.2.9, specialized algorithms
  template <typename... Types>
  inline void swap(tuple<Types...>& x, tuple<Types...>& y)
#ifdef NTL_CXX_NOEXCEPT
    noexcept(x.swap(y))
#endif
  {
    x.swap(y);
  }

  ///\name 20.4.2.10 tuple range access [tuple.range]
  /** @note Is only two-types tuple can be here? */
  template <class InputIterator>
  inline InputIterator begin(const std::tuple<InputIterator, InputIterator>& t) { return get<0>(t); }

  template <class InputIterator>
  inline InputIterator end(const std::tuple<InputIterator, InputIterator>& t)   { return get<1>(t); }
  ///\}

 /**@} lib_tuple */
 /**@} lib_utilities */
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER <= 170051025)
# pragma warning(pop)
#endif // _MSC
}//namespace std
#endif//#ifndef NTL__STLX_TUPLE_VT
