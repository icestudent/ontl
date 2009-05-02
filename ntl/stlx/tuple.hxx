/**\file*********************************************************************
 *                                                                     \brief
 *  Tuples [20.4 tuple]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TUPLE
#define NTL__STLX_TUPLE

#define TTL_MAX_TEMPLATE_PARAMS 5

//#include "array.hxx" // for tuple array access
#include "ext/ttl/typelist.hxx"
#include "utility.hxx" // for tuple pair access

namespace std
{

/**\addtogroup  lib_utilities *** 20 General utilities library [utilities]
 *@{
 */

/**\defgroup  lib_tuple ******* 20.4 Tuples [tuple]
 *
 *   Tuple library that provides a tuple type as the class template tuple that can
 *   be instantiated with any number of arguments.
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

      // TUPLE_EXT:
      explicit tuples(typename tuple_param<types, Idx+1>::type p1)
        :head(p1), tail()
      {}
      explicit tuples(typename tuple_param<types, Idx+1>::type p1, typename tuple_param<types, Idx+2>::type p2)
        :head(p1), tail(p2)
      {}
      explicit tuples(typename tuple_param<types, Idx+1>::type p1, typename tuple_param<types, Idx+2>::type p2, typename tuple_param<types, Idx+3>::type p3)
        :head(p1), tail(p2, p3)
      {}
      explicit tuples(typename tuple_param<types, Idx+1>::type p1, typename tuple_param<types, Idx+2>::type p2, typename tuple_param<types, Idx+3>::type p3, typename tuple_param<types, Idx+4>::type p4)
        :head(p1), tail(p2, p3, p4)
      {}
      explicit tuples(typename tuple_param<types, Idx+1>::type p1, typename tuple_param<types, Idx+2>::type p2, typename tuple_param<types, Idx+3>::type p3, typename tuple_param<types, Idx+4>::type p4, typename tuple_param<types, Idx+5>::type p5)
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
      // TUPLE_EXT:
      typedef typename rparam<types,Idx+0>::type type1;
      typedef typename rparam<types,Idx+1>::type type2;
      typedef typename rparam<types,Idx+2>::type type3;
      typedef typename rparam<types,Idx+3>::type type4;
      typedef typename rparam<types,Idx+4>::type type5;

      tuples(rvtag, type1&& p1)
        :head(forward<type1>(p1))
      {}
      tuples(rvtag, type1&& p1, type2&& p2)
        :head(forward<type1>(p1)), tail(rvtag(), p2)
      {}
      tuples(rvtag, type1&& p1, type2&& p2, type3&& p3)
        :head(forward<type1>(p1)), tail(rvtag(), p2, p3)
      {}
      tuples(rvtag, type1&& p1, type2&& p2, type3&& p3, type4&& p4)
        :head(forward<type1>(p1)), tail(rvtag(), p2, p3, p4)
      {}
      tuples(rvtag, type1&& p1, type2&& p2, type3&& p3, type4&& p4, type5&& p5)
        :head(forward<type1>(p1)), tail(rvtag(), p2, p3, p4, p5)
      {}

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
        head = move(r.head);
        tail = move(r.tail);
        return *this;
      }

      // TUPLE_EXT:
      template<typename U1, typename U2, typename U3, typename U4, typename U5>
      tuples& operator=(tuples<meta::typelist<U1,U2,U3,U4,U5>, Idx>&& r)
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
#ifndef NTL__CXX_RV
#define NTL_TUPLE_MAKEGET(N,cv) static __forceinline R get(cv T& t, int2type<N>) { return t.\
  TTL_REPEAT(N, NTL_TUPLE_GET, NTL_TUPLE_GET, 0)\
  head; }

    template<typename R, size_t N>
    struct field2
    {
      template<class T>
      static __forceinline R get(const T& t) { return getter<T>::get(t, int2type<N>()); }
      template<class T>
      static __forceinline R get(T& t) { return getter<T>::get(t, int2type<N>()); }
    private:
      template<class T>
      struct getter
      {
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
  private:
    typedef __::rvtag rvtag;
    typedef __::tuples<types> base;
    typedef tuple<T1,T2,T3,T4,T5>   this_t;

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
    explicit tuple(typename __::tuple_param<types,1>::type p1, typename __::tuple_param<types,2>::type p2, typename __::tuple_param<types,3>::type p3, typename __::tuple_param<types,4>::type p4)
      :base(p1, p2, p3, p4)
    {}
    explicit tuple(typename __::tuple_param<types,1>::type p1, typename __::tuple_param<types,2>::type p2, typename __::tuple_param<types,3>::type p3, typename __::tuple_param<types,4>::type p4, typename __::tuple_param<types,5>::type p5)
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

  // make tuple
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
    template<class Tuple1, class Tuple2> struct tuple_glue;

    template<class H = meta::empty_type, class T = meta::empty_type> struct tlist { typedef H head; typedef T tail; };
    
    template<class TL, class A>
    struct tappend { typedef tlist<TL,A> type; };

    template<class A>
    struct tappend<tlist<meta::empty_type,meta::empty_type>, A> { typedef tlist<A> type; };

    template<class H, class A>
    struct tappend<tlist<H,meta::empty_type>, A> { typedef tlist<H, A> type; };

    template<class H, class T>
    struct tappend<tlist<H,T>, meta::empty_type> { typedef tlist<H,T> type; };

    template<class TL> struct tlength { enum { value = 1 + tlength<typename TL::tail>::value }; };
    template<> struct tlength<meta::empty_type> { enum { value = 0 }; };

    template<size_t I, class TL> struct tget { typedef typename tget<I-1,typename TL::tail>::type type; };
    template<class TL> struct tget<0,TL> { typedef typename TL::head type; };


    template<class types, size_t i, size_t len> struct flt 
    {
      typedef typename ttl::meta::get<types, i>::type ct;
      static const bool stop = is_same<ct,meta::empty_type>::value;
      typedef typename tappend<ct, typename conditional<stop, meta::empty_type, typename flt<types,i+1,len>::type>::type>::type type;
    };
    template<class types, size_t len> struct flt<types,len,len> { typedef meta::empty_type type; };

    template<class types, size_t len = ttl::meta::length<types>::value> struct filter 
    {
      typedef typename flt<types, 0, len>::type result;
    };

    template<class TL, size_t len = tlength<TL>::value> struct tl2tup;
    template<class TL> struct tl2tup<TL,0> { typedef tuple<> type; };
    template<class TL> struct tl2tup<TL,1> { typedef tuple<typename tget<0,TL>::type> type; };
    template<class TL> struct tl2tup<TL,2> { typedef tuple<typename tget<0,TL>::type, typename tget<1,TL>::type> type; };
    template<class TL> struct tl2tup<TL,3> { typedef tuple<typename tget<0,TL>::type, typename tget<1,TL>::type, typename tget<2,TL>::type> type; };
    template<class TL> struct tl2tup<TL,4> { typedef tuple<typename tget<0,TL>::type, typename tget<1,TL>::type, typename tget<2,TL>::type, typename tget<3,TL>::type> type; };
    template<class TL> struct tl2tup<TL,5> { typedef tuple<typename tget<0,TL>::type, typename tget<1,TL>::type, typename tget<2,TL>::type, typename tget<3,TL>::type, typename tget<4,TL>::type> type; };

    template<
      typename T1, typename T2, typename T3, typename T4, typename T5,
      typename U1, typename U2, typename U3, typename U4, typename U5
            >
    struct tuple_glue<tuple<T1,T2,T3,T4,T5>, tuple<U1,U2,U3,U4,U5> >
    {
      typedef tuple<T1,T2,T3,T4,T5> tup1;
      typedef tuple<U1,U2,U3,U4,U5> tup2;

      typedef typename filter<typename tup1::types>::result filtered1;
      typedef typename filter<typename tup2::types>::result filtered2;
      typedef typename tappend<filtered1,filtered2>::type filtered;
      typedef typename tl2tup<filtered>::type result;
    };

    template<class Tuple>
    struct sget
    {
      template<size_t I>
      struct rt
      {
        typedef typename ttl::meta::get<typename Tuple::types, I>::type found_type;
        typedef typename mktraits<found_type>::type type;
      };

      template<size_t I, class T1, class T2>
      static typename rt<I>::type v(const T1& t1, const T2& t2)
      {
        static const bool e1 = I >= T1::types::length;
        static const size_t J = e1 ? (I - T1::types::length) : I;
        static const bool e2 = J >= T2::types::length;
        static const bool none = e1 && e2;

        typedef meta::int2type<none ? 0 : !e1 ? 1 : 2> w; // 0 - out of range, 1 - t1, 2 - t2
        return get<J, typename rt<I>::type> (t1,t2,w());
      }

    private:
      typedef meta::int2type<0> ignore;
      typedef meta::int2type<1> left;
      typedef meta::int2type<2> right;

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
    using __::sget;
    typedef typename __::tuple_glue<Tuple1,Tuple2>::result R;
    return R(sget<R>::v<0>(t1,t2), sget<R>::v<1>(t1,t2));
  }


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
  }

  // TUPLE_EXT:
  template<size_t I, typename T1>
  inline typename __::tuple_return<I, tuple<T1> >::type get(tuple<T1>& t)
  {
    typedef typename __::tuple_return<I, tuple<T1> >::type RT;
    return __::field2<RT, I>::get(t);
  }

  template<size_t I, typename T1>
  inline typename __::tuple_return<I, tuple<T1> >::ctype get(const tuple<T1>& t)
  {
    typedef typename __::tuple_return<I, tuple<T1> >::ctype RT;
    return __::field2<RT, I>::get(t);
  }

  template<size_t I, typename T1, typename T2>
  inline typename __::tuple_return<I, tuple<T1,T2> >::type get(tuple<T1,T2>& t)
  {
    typedef typename __::tuple_return<I, tuple<T1,T2> >::type RT;
    return __::field2<RT, I>::get(t);
  }

  template<size_t I, typename T1, typename T2>
  inline typename __::tuple_return<I, tuple<T1,T2> >::ctype get(const tuple<T1,T2>& t)
  {
    typedef typename __::tuple_return<I, tuple<T1,T2> >::ctype RT;
    return __::field2<RT, I>::get(t);
  }

  template<size_t I, typename T1, typename T2, typename T3>
  inline typename __::tuple_return<I, tuple<T1,T2,T3> >::type get(tuple<T1,T2,T3>& t)
  {
    typedef typename __::tuple_return<I, tuple<T1,T2,T3> >::type RT;
    return __::field2<RT, I>::get(t);
  }

  template<size_t I, typename T1, typename T2, typename T3>
  inline typename __::tuple_return<I, tuple<T1,T2,T3> >::ctype get(const tuple<T1,T2,T3>& t)
  {
    typedef typename __::tuple_return<I, tuple<T1,T2,T3> >::ctype RT;
    return __::field2<RT, I>::get(t);
  }

  template<size_t I, typename T1, typename T2, typename T3, typename T4>
  inline typename __::tuple_return<I, tuple<T1,T2,T3,T4> >::ctype get(const tuple<T1,T2,T3,T4>& t)
  {
    typedef typename __::tuple_return<I, tuple<T1,T2,T3,T4> >::ctype RT;
    return __::field2<RT, I>::get(t);
  }

  template<size_t I, typename T1, typename T2, typename T3, typename T4, typename T5>
  inline typename __::tuple_return<I, tuple<T1,T2,T3,T4,T5> >::ctype get(const tuple<T1,T2,T3,T4,T5>& t)
  {
    typedef typename __::tuple_return<I, tuple<T1,T2,T3,T4,T5> >::ctype RT;
    return __::field2<RT, I>::get(t);
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
