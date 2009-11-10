/**\file*********************************************************************
 *                                                                     \brief
 *  Tuples [20.4 tuple]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TUPLE
#define NTL__STLX_TUPLE
#pragma once

#define TTL_MAX_TEMPLATE_PARAMS 5
#define STLX_ENABLE_TUPLE_RELATIONS

#include "ext/typelist.hxx"

#ifndef NTL__STLX_UTILITY
#include "utility.hxx" // for tuple pair access
#endif

namespace std
{

/**\addtogroup  lib_utilities *** 20 General utilities library [utilities]
 *@{
 */

/**\defgroup  lib_tuple ******* 20.4 Tuples [tuple]
 *
 *   Tuple library that provides a tuple type as the class template tuple that can be instantiated with any number of arguments.
 *@{
 */

  /// 20.4.1 Class template tuple [tuple.tuple]
  /// \note Maximum tuple arguments is 5.
  /// \internal Extensible places marked as TUPLE_EXT
  template<typename T1 = ttl::meta::empty_type, typename T2 = ttl::meta::empty_type, typename T3 = ttl::meta::empty_type, typename T4 = ttl::meta::empty_type, typename T5 = ttl::meta::empty_type>
  class tuple;

  /// 20.4.1.4 Tuple helper classes [tuple.helper]
  template<typename T1 = ttl::meta::empty_type, typename T2 = ttl::meta::empty_type, typename T3 = ttl::meta::empty_type, typename T4 = ttl::meta::empty_type, typename T5 = ttl::meta::empty_type>
  struct tuple_size;

  /// 20.4.1.4 Tuple helper classes [tuple.helper]
  template<size_t I, typename T1 = ttl::meta::empty_type, typename T2 = ttl::meta::empty_type, typename T3 = ttl::meta::empty_type, typename T4 = ttl::meta::empty_type, typename T5 = ttl::meta::empty_type>
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

    template<typename Typelist, size_t Idx, bool = (Idx < Typelist::length) >
    struct tuple_param
    {
      typedef typename traits<typename meta::get<Typelist,Idx>::type>::param_type type;
    };

    template<typename Typelist, size_t Idx>
    struct tuple_param<Typelist, Idx, false>
    {
      typedef meta::empty_type type;
    };

#ifdef NTL__CXX_RV

    template<class Types, size_t i, bool = (i < Types::length)>
    struct rparam
    {
      typedef typename meta::get<Types,i>::type ft, type;
    };

    template<class Types, size_t i>
    struct rparam<Types, i, false> { typedef meta::empty_type type; };

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

    protected:
      typedef typename tuple_param<types, Idx+0>::type ltype1;
      typedef typename tuple_param<types, Idx+1>::type ltype2;
      typedef typename tuple_param<types, Idx+2>::type ltype3;
      typedef typename tuple_param<types, Idx+3>::type ltype4;
      typedef typename tuple_param<types, Idx+4>::type ltype5;
    public:
      tuples()
        :head(), tail()
      {}

      // TUPLE_EXT:
      explicit tuples(ltype1 p1)
        :head(p1), tail()
      {}
      explicit tuples(ltype1 p1, ltype2 p2)
        :head(p1), tail(p2)
      {}
      explicit tuples(ltype1 p1, ltype2 p2, ltype3 p3)
        :head(p1), tail(p2, p3)
      {}
      explicit tuples(ltype1 p1, ltype2 p2, ltype3 p3, ltype4 p4)
        :head(p1), tail(p2, p3, p4)
      {}
      explicit tuples(ltype1 p1, ltype2 p2, ltype3 p3, ltype4 p4, ltype5 p5)
        :head(p1), tail(p2, p3, p4, p5)
      {}

      // TUPLE_EXT:
      template<typename U1, typename U2, typename U3, typename U4, typename U5>
      tuples(const tuples<meta::typelist<U1,U2,U3,U4,U5>, Idx>& r)
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

      // TUPLE_EXT:
      template<typename U1, typename U2, typename U3, typename U4, typename U5>
      tuples& operator=(const tuples<meta::typelist<U1,U2,U3,U4,U5>, Idx>& r)
      {
        head = r.head;
        tail = r.tail;
        return *this;
      }

#ifdef NTL__CXX_RV
#if 0
      // TUPLE_EXT:
    protected:
      typedef typename rparam<types,Idx+0>::type rtype1;
      typedef typename rparam<types,Idx+1>::type rtype2;
      typedef typename rparam<types,Idx+2>::type rtype3;
      typedef typename rparam<types,Idx+3>::type rtype4;
      typedef typename rparam<types,Idx+4>::type rtype5;

    public:
      tuples(rvtag, rtype1&& p1)
        :head(forward<rtype1>(p1))
      {}
      tuples(rvtag, rtype1&& p1, rtype2&& p2)
        :head(forward<rtype1>(p1)), tail(rvtag(), forward<rtype2>(p2))
      {}
      tuples(rvtag, rtype1&& p1, rtype2&& p2, rtype3&& p3)
        :head(forward<rtype1>(p1)), tail(rvtag(), forward<rtype2>(p2), forward<rtype3>(p3))
      {}
      tuples(rvtag, rtype1&& p1, rtype2&& p2, rtype3&& p3, rtype4&& p4)
        :head(forward<rtype1>(p1)), tail(rvtag(), forward<rtype2>(p2), forward<rtype3>(p3), forward<rtype4>(p4))
      {}
      tuples(rvtag, rtype1&& p1, rtype2&& p2, rtype3&& p3, rtype4&& p4, rtype5&& p5)
        :head(forward<rtype1>(p1)), tail(rvtag(), forward<rtype2>(p2), forward<rtype3>(p3), forward<rtype4>(p4), forward<rtype5>(p5))
      {}
#else
      template<typename U1>
      tuples(rvtag, U1&& p1)
        :head(forward<U1>(p1))
      {}
      template<typename U1, typename U2>
      tuples(rvtag, U1&& p1, U2&& p2)
        :head(forward<U1>(p1)), tail(rvtag(), forward<U2>(p2))
      {}
      template<typename U1, typename U2, typename U3>
      tuples(rvtag, U1&& p1, U2&& p2, U3&& p3)
        :head(forward<U1>(p1)), tail(rvtag(), forward<U2>(p2), forward<U3>(p3))
      {}
      template<typename U1, typename U2, typename U3, typename U4>
      tuples(rvtag, U1&& p1, U2&& p2, U3&& p3, U4&& p4)
        :head(forward<U1>(p1)), tail(rvtag(), forward<U2>(p2), forward<U3>(p3), forward<U4>(p4))
      {}
      template<typename U1, typename U2, typename U3, typename U4, typename U5>
      tuples(rvtag, U1&& p1, U2&& p2, U3&& p3, U4&& p4, U5&& p5)
        :head(forward<U1>(p1)), tail(rvtag(), forward<U2>(p2), forward<U3>(p3), forward<U4>(p4), forward<U5>(p5))
      {}
#endif

      template<typename U1, typename U2, typename U3, typename U4, typename U5>
      tuples(tuples<meta::typelist<U1,U2,U3,U4,U5>, Idx>&& r)
        :head(forward<head_t>(r.head)), tail(forward<tail_t>(r.tail))
      {}

      tuples(tuples&& r)
        :head(forward<head_t>(r.head)), tail(forward<tail_t>(r.tail))
      {}

      tuples& operator=(tuples&& r)
      {
        if(this == &r) return *this;
        head = std::move(r.head);
        tail = std::move(r.tail);
        return *this;
      }

      // TUPLE_EXT:
      template<typename U1, typename U2, typename U3, typename U4, typename U5>
      tuples& operator=(tuples<meta::typelist<U1,U2,U3,U4,U5>, Idx>&& r)
      {
        head = std::move(r.head);
        tail = std::move(r.tail);
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

      // TUPLE_EXT:
      template<typename U1, typename U2, typename U3, typename U4, typename U5>
      tuples(const tuples<meta::typelist<U1,U2,U3,U4,U5>, Idx>&)
      {}

      tuples(const tuples&)
      {}

      tuples& operator=(const tuples&)
      { return *this; }

      template<typename U1, typename U2, typename U3, typename U4, typename U5>
      tuples& operator=(const tuples<meta::typelist<U1,U2,U3,U4,U5>, Idx>&)
      { return *this; }

#ifdef NTL__CXX_RV
      template<typename U1, typename U2, typename U3, typename U4, typename U5>
      tuples(tuples<meta::typelist<U1,U2,U3,U4,U5>, Idx>&&)
      {}

      tuples(tuples&&)
      {}

      tuples& operator=(tuples&&)
      { return *this; }

      template<typename U1, typename U2, typename U3, typename U4, typename U5>
      tuples& operator=(tuples<meta::typelist<U1,U2,U3,U4,U5>, Idx>&&)
      { return *this; }
#endif

      // TUPLE_EXT:
      explicit tuples(meta::empty_type)
      {}
      explicit tuples(meta::empty_type, meta::empty_type)
      {}
      explicit tuples(meta::empty_type, meta::empty_type, meta::empty_type)
      {}
      explicit tuples(meta::empty_type, meta::empty_type, meta::empty_type, meta::empty_type)
      {}
      explicit tuples(meta::empty_type, meta::empty_type, meta::empty_type, meta::empty_type, meta::empty_type)
      {}
#ifdef NTL__CXX_RV
      explicit tuples(rvtag, meta::empty_type)
      {}
      explicit tuples(rvtag, meta::empty_type, meta::empty_type)
      {}
      explicit tuples(rvtag, meta::empty_type, meta::empty_type, meta::empty_type)
      {}
      explicit tuples(rvtag, meta::empty_type, meta::empty_type, meta::empty_type, meta::empty_type)
      {}
      explicit tuples(rvtag, meta::empty_type, meta::empty_type, meta::empty_type, meta::empty_type, meta::empty_type)
      {}
#endif
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

#define NTL_TUPLE_GET(n,aux) tail.

#ifndef NTL__CXX_RV_DEPRECATED__

    template<typename R, size_t N>
    struct field2
    {
      // strict get
      template<class T>
      static __forceinline R get(const T& t) { return getter<T>::get(t, int2type<N>()); }
      template<class T>
      static __forceinline R get(T& t) { return getter<T>::get(t, int2type<N>()); }

      // non-strict get
      template<class T>
      static __forceinline R sget(const T& t) { return sget(t, bool_type<(N < T::types::length)>()); }
      template<class T>
      static __forceinline R sget(const T& t, true_type) { return getter<T>::get(t, int2type<N>()); }
      template<class T>
      static __forceinline R sget(const T&, false_type) { static const meta::empty_type et; return et; }
      template<class T>
      static __forceinline R sget(T& t) { return sget(t, bool_type<(N < T::types::length)>()); }
      template<class T>
      static __forceinline R sget(T& t, true_type) { return getter<T>::get(t, int2type<N>()); }
      template<class T>
      static __forceinline R sget(T&, false_type) { static const meta::empty_type et; return et; }
    private:
      template<class T>
      struct getter
      {
#define NTL_TUPLE_MAKEGET(N,cv) static __forceinline R get(cv T& t, int2type<N>) { return t.\
  TTL_REPEAT(N, NTL_TUPLE_GET, NTL_TUPLE_GET, 0)\
  head; }

        static __forceinline R get(const T& t, int2type<0>) { return t.head; }
        TTL_REPEAT_NEST(TTL_MAX_TEMPLATE_PARAMS,NTL_TUPLE_MAKEGET,NTL_TUPLE_MAKEGET,const);

        static __forceinline R get(T& t, int2type<0>) { return t.head; }
        TTL_REPEAT_NEST(TTL_MAX_TEMPLATE_PARAMS,NTL_TUPLE_MAKEGET,NTL_TUPLE_MAKEGET,);
      };
    };
#else
    template<typename R, size_t N>
    struct field2
    {
      template<class T>
      static __forceinline R get(T&& t) { return getter<T>::get(forward<T>(t), int2type<N>()); }

      template<class T>
      static __forceinline R sget(T&& t) { return sget(forward<T>(t), bool_type<(N < T::types::length)>()); }
      template<class T>
      static __forceinline R sget(T&& t, true_type) { return getter<T>::get(forward<T>(t), int2type<N>()); }
      template<class T>
      static __forceinline R sget(T&&, false_type) { static const meta::empty_type et; return et; }
    private:
      template<class T>
      struct getter
      {
        static __forceinline R get(T&& t, int2type<0>) { return t.head; }

#define NTL_TUPLE_MAKEGET(N,cv) static __forceinline R get(T&& t, int2type<N>) { return t.\
  TTL_REPEAT(N, NTL_TUPLE_GET, NTL_TUPLE_GET, 0)\
  head; }
        TTL_REPEAT_NEST(TTL_MAX_TEMPLATE_PARAMS,NTL_TUPLE_MAKEGET,NTL_TUPLE_MAKEGET,const);
      };
    };
#endif
#undef NTL_TUPLE_GET
#undef NTL_TUPLE_MAKEGET

    template<typename T>
    struct is_tuple: false_type{};
    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    struct is_tuple<tuple<T1,T2,T3,T4,T5> >: true_type{};
  }

  template<typename T1, typename T2, typename T3, typename T4, typename T5>
  class tuple:
    public __::tuples<ttl::meta::typelist<T1,T2,T3,T4,T5> >
  {
  public:
    typedef ttl::meta::typelist<T1,T2,T3,T4,T5> types;
  public:
    typedef __::rvtag rvtag;
    typedef __::tuples<types> base;
    typedef tuple<T1,T2,T3,T4,T5>   this_t;

    typedef typename __::tuple_param<types,0>::type ltype1;
    typedef typename __::tuple_param<types,1>::type ltype2;
    typedef typename __::tuple_param<types,2>::type ltype3;
    typedef typename __::tuple_param<types,3>::type ltype4;
    typedef typename __::tuple_param<types,4>::type ltype5;
    
#ifdef NTL__CXX_RV
    typedef typename __::rparam<types,0>::type rtype1;
    typedef typename __::rparam<types,1>::type rtype2;
    typedef typename __::rparam<types,2>::type rtype3;
    typedef typename __::rparam<types,3>::type rtype4;
    typedef typename __::rparam<types,4>::type rtype5;
#endif
  public:
    tuple()
    {}

    // explicit tuple(const Types&...);
    explicit tuple(ltype1 p1)
      :base(p1)
    {}
    explicit tuple(ltype1 p1, ltype2 p2)
      :base(p1, p2)
    {}
    explicit tuple(ltype1 p1, ltype2 p2, ltype3 p3)
      :base(p1, p2, p3)
    {}
    explicit tuple(ltype1 p1, ltype2 p2, ltype3 p3, ltype4 p4)
      :base(p1, p2, p3, p4)
    {}
    explicit tuple(ltype1 p1, ltype2 p2, ltype3 p3, ltype4 p4, ltype5 p5)
      :base(p1, p2, p3, p4, p5)
    {}

    tuple(const tuple& r)
      :base(static_cast<const base&>(r))
    {}

    //tuple(tuple& r)
    //  :base(static_cast<base&>(r))
    //{}

    // template <class... UTypes> tuple(const tuple<UTypes...>&);
    template<typename U1, typename U2, typename U3, typename U4, typename U5>
    tuple(const tuple<U1,U2,U3,U4,U5>& r)
      :base(r)
    {
      static_assert((types::length == tuple<U1,U2,U3,U4,U5>::types::length), "incompatible number of types");
    }

    tuple& operator=(const tuple& r)
    {
      base::operator=(r);
      return *this;
    }

    // template <class... UTypes> tuple& operator=(const tuple<UTypes...>&);
    template<typename U1, typename U2, typename U3, typename U4, typename U5>
    tuple& operator=(const tuple<U1,U2,U3,U4,U5>& r)
    {
      static_assert((types::length == tuple<U1,U2,U3,U4,U5>::types::length), "incompatible number of types");
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

    template<typename U1, typename U2, typename U3, typename U4>
    explicit tuple(U1&& p1, U2&& p2, U3&& p3, U4&& p4)
      :base(rvtag(), forward<U1>(p1), forward<U2>(p2), forward<U3>(p3), forward<U4>(p4))
    {}

    template<typename U1, typename U2, typename U3, typename U4, typename U5>
    explicit tuple(U1&& p1, U2&& p2, U3&& p3, U4&& p4, U5&& p5)
      :base(rvtag(), forward<U1>(p1), forward<U2>(p2), forward<U3>(p3), forward<U4>(p4), forward<U5>(p5))
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
    template<typename U1, typename U2, typename U3, typename U4, typename U5>
    tuple(tuple<U1,U2,U3,U4,U5>&& r)
      :base(move(r))
    {
      static_assert((types::length == tuple<U1,U2,U3,U4,U5>::types::length), "incompatible number of types");
    }

    // template <class... UTypes> tuple& operator=(tuple<UTypes...>&&);
    template<typename U1, typename U2, typename U3, typename U4, typename U5>
    tuple& operator=(tuple<U1,U2,U3,U4,U5>&& r)
    {
      static_assert((types::length == tuple<U1,U2,U3,U4,U5>::types::length), "incompatible number of types");
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
    template<typename T1 = meta::empty_type, typename T2 = meta::empty_type, typename T3 = meta::empty_type, typename T4 = meta::empty_type, typename T5 = meta::empty_type>
    struct tmap
    {
      typedef tuple<typename mktraits<T1>::type, typename mktraits<T2>::type, typename mktraits<T3>::type, typename mktraits<T4>::type, typename mktraits<T5>::type> type;
    };

  }

  /// "ignore" allows tuple positions to be ignored when using "tie".
  static __::swallow_assign const ignore = {};

  inline tuple<> make_tuple()
  {
    return tuple<>();
  }

  // TUPLE_EXT:
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
  template<typename T1, typename T2, typename T3, typename T4>
  inline typename __::tmap<T1,T2,T3,T4>::type make_tuple(T1&& p1, T2&& p2, T3&& p3, T4&& p4)
  {
    return __::tmap<T1,T2,T3,T4>::type (forward<T1>(p1),forward<T2>(p2),forward<T3>(p3),forward<T4>(p4));
  }
  template<typename T1, typename T2, typename T3, typename T4, typename T5>
  inline typename __::tmap<T1,T2,T3,T4,T5>::type make_tuple(T1&& p1, T2&& p2, T3&& p3, T4&& p4, T5&& p5)
  {
    return __::tmap<T1,T2,T3,T4,T5>::type (forward<T1>(p1),forward<T2>(p2),forward<T3>(p3),forward<T4>(p4),forward<T5>(p5));
  }
#else
  template<typename T1>
  inline typename __::tmap<const T1&>::type make_tuple(const T1& p1)
  {
    return __::tmap<const T1&>::type (p1);
  }
  template<typename T1, typename T2>
  inline typename __::tmap<const T1&,const T2&>::type make_tuple(const T1& p1, const T2& p2)
  {
    return __::tmap<const T1&,const T2&>::type (p1,p2);
  }
  template<typename T1, typename T2, typename T3>
  inline typename __::tmap<const T1&,const T2&,const T3&>::type make_tuple(const T1& p1, const T2& p2, const T3& p3)
  {
    return __::tmap<const T1&,const T2&,const T3&>::type (p1,p2,p3);
  }
  template<typename T1, typename T2, typename T3, typename T4>
  inline typename __::tmap<const T1&,const T2&,const T3&,const T4&>::type make_tuple(const T1& p1, const T2& p2, const T3& p3, const T4& p4)
  {
    return __::tmap<const T1&,const T2&,const T3&,const T4&>::type (p1,p2,p3,p4);
  }
  template<typename T1, typename T2, typename T3, typename T4, typename T5>
  inline typename __::tmap<const T1&,const T2&,const T3&,const T5&>::type make_tuple(const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5)
  {
    return __::tmap<const T1&,const T2&,const T3&,const T4&,const T5&>::type (p1,p2,p3,p4,p5);
  }
#endif

  // tie
  // TUPLE_EXT:
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
  template<typename T1, typename T2, typename T3, typename T4>
  inline tuple<T1&,T2&,T3&,T4&> tie(T1& p1, T2& p2, T3& p3, T4& p4)
  {
    return tuple<T1&,T2&,T3&,T4&> (p1,p2,p3,p4);
  }
  template<typename T1, typename T2, typename T3, typename T4, typename T5>
  inline tuple<T1&,T2&,T3&,T4&,T5&> tie(T1& p1, T2& p2, T3& p3, T4& p4, T5& p5)
  {
    return tuple<T1&,T2&,T3&,T4&,T5&> (p1,p2,p3,p4,p5);
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
  // TUPLE_EXT:
  template<typename T1, typename T2, typename T3, typename T4, typename T5>
  struct tuple_size<tuple<T1,T2,T3,T4,T5> >:
    integral_constant<size_t, ttl::meta::length<typename tuple<T1,T2,T3,T4,T5>::types>::value>
  {};

  template<size_t I, typename T1, typename T2, typename T3, typename T4, typename T5>
  struct tuple_element<I, tuple<T1,T2,T3,T4,T5> >
  {
    typedef tuple<T1,T2,T3,T4,T5> tuple_t;
    static_assert(I < tuple_size<tuple_t>::value, "I is out of bounds");

    typedef typename ttl::meta::get<typename tuple_t::types, I>::type type;
  };

  ///\name 20.4.1.5, element access:
  // get(tuple<>)
  namespace __
  {
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
    template<size_t I, class Tuple>
    struct tuple_return
    {
      typedef typename ttl::meta::get<typename Tuple::types, I>::type found_type;
      typedef typename traits<found_type>::return_type type;
      typedef typename traits<typename add_const<found_type>::type>::return_type ctype;
    };

    template<size_t I, class Tuple>
    struct tuple_sreturn
    {
    private:
      struct empty { typedef meta::empty_type& type; typedef const meta::empty_type& ctype; };
      static const bool ok = (I < Tuple::types::length);
      typedef typename conditional<ok, tuple_return<I,Tuple>, empty>::type rt;
    public:
      typedef typename rt::type  type;
      typedef typename rt::ctype ctype;
    };

    template<class Tuple> struct sret
    {
      template<size_t I> 
      static typename tuple_sreturn<I,Tuple>::ctype get(const Tuple& t) { return field2<typename tuple_sreturn<I,Tuple>::ctype, I>::sget(t); }
      template<size_t I> 
      static typename tuple_sreturn<I,Tuple>::ctype get(Tuple& t) { return field2<typename tuple_sreturn<I,Tuple>::ctype, I>::sget(t); }
    };
  }

  // TUPLE_EXT:
  template<size_t I, typename T1, typename T2, typename T3, typename T4, typename T5>
  inline typename __::tuple_return<I, tuple<T1,T2,T3,T4,T5> >::ctype get(const tuple<T1,T2,T3,T4,T5>& t)
  {
    static_assert((I < tuple_size<tuple<T1,T2,T3,T4,T5> >::value), "I is out of bounds");
    typedef typename __::tuple_return<I, tuple<T1,T2,T3,T4,T5> >::ctype RT;
    return __::field2<RT, I>::get(t);
  }

  template<size_t I, typename T1, typename T2, typename T3, typename T4, typename T5>
  inline typename __::tuple_return<I, tuple<T1,T2,T3,T4,T5> >::type get(tuple<T1,T2,T3,T4,T5>& t)
  {
    static_assert((I < tuple_size<tuple<T1,T2,T3,T4,T5> >::value), "I is out of bounds");
    typedef typename __::tuple_return<I, tuple<T1,T2,T3,T4,T5> >::type RT;
    return __::field2<RT, I>::get(t);
  }


#ifdef STLX_ENABLE_TUPLE_RELATIONS
  ///\name 20.4.1.6, relational operators:
  inline bool operator== (const tuple<>&, const tuple<>&) {  return true; }
  inline bool operator!= (const tuple<>&, const tuple<>&) {  return false; }
  inline bool operator<  (const tuple<>&, const tuple<>&) {  return false; }
  inline bool operator>  (const tuple<>&, const tuple<>&) {  return false; }
  inline bool operator<= (const tuple<>&, const tuple<>&) {  return true; }
  inline bool operator>= (const tuple<>&, const tuple<>&) {  return true; }

  template<
    typename T1, typename T2, typename T3, typename T4, typename T5,
    typename U1, typename U2, typename U3, typename U4, typename U5
          >
  inline bool operator== (const tuple<T1,T2,T3,T4,T5>& x, const tuple<U1,U2,U3,U4,U5>& y)
  {
    typedef __::sret<tuple<T1,T2,T3,T4,T5> > ret1;
    typedef __::sret<tuple<U1,U2,U3,U4,U5> > ret2;
    typedef __::tuple_sreturn<1, tuple<T1,T2,T3,T4,T5> > rt;
    return ret1::get<0>(x) == ret2::get<0>(y) && ret1::get<1>(x) == ret2::get<1>(y) && ret1::get<2>(x) == ret2::get<2>(y) && ret1::get<3>(x) == ret2::get<3>(y) && ret1::get<4>(x) == ret2::get<4>(y);
  }

  template<
    typename T1, typename T2, typename T3, typename T4, typename T5,
    typename U1, typename U2, typename U3, typename U4, typename U5
          >
  inline bool operator!= (const tuple<T1,T2,T3,T4,T5>& x, const tuple<U1,U2,U3,U4,U5>& y)
  {
    return rel_ops::operator!= (x,y);
  }

  template<
    typename T1, typename T2, typename T3, typename T4, typename T5,
    typename U1, typename U2, typename U3, typename U4, typename U5
          >
  inline bool operator< (const tuple<T1,T2,T3,T4,T5>& x, const tuple<U1,U2,U3,U4,U5>& y)
  {
    typedef __::sret<tuple<T1,T2,T3,T4,T5> > ret1;
    typedef __::sret<tuple<U1,U2,U3,U4,U5> > ret2;
    return ret1::get<0>(x) < ret2::get<0>(y) && ret1::get<1>(x) < ret2::get<1>(y) && ret1::get<2>(x) < ret2::get<2>(y) && ret1::get<3>(x) < ret2::get<3>(y) && ret1::get<4>(x) < ret2::get<4>(y);
  }

  template<
    typename T1, typename T2, typename T3, typename T4, typename T5,
    typename U1, typename U2, typename U3, typename U4, typename U5
          >
  inline bool operator> (const tuple<T1,T2,T3,T4,T5>& x, const tuple<U1,U2,U3,U4,U5>& y)
  {
    return rel_ops::operator>(x,y);
  }

  template<
    typename T1, typename T2, typename T3, typename T4, typename T5,
    typename U1, typename U2, typename U3, typename U4, typename U5
          >
  inline bool operator<= (const tuple<T1,T2,T3,T4,T5>& x, const tuple<U1,U2,U3,U4,U5>& y)
  {
    return rel_ops::operator<=(x,y);
  }

  template<
    typename T1, typename T2, typename T3, typename T4, typename T5,
    typename U1, typename U2, typename U3, typename U4, typename U5
          >
  inline bool operator>= (const tuple<T1,T2,T3,T4,T5>& x, const tuple<U1,U2,U3,U4,U5>& y)
  {
    return rel_ops::operator>=(x,y);
  }

#endif
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

  ///\name 20.5.2.10 tuple range access [tuple.range]
  /** @note Is only two-types tuple can be here? */
  template <class InputIterator>
  inline InputIterator begin(const std::tuple<InputIterator, InputIterator>& t) { return get<0>(t); }
  template <class InputIterator>
  inline InputIterator end(const std::tuple<InputIterator, InputIterator>& t)   { return get<1>(t); }
  ///\}

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
