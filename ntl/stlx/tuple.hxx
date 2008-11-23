/**\file*********************************************************************
 *                                                                     \brief
 *  Tuples [20.4 tuple]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TUPLE
#define NTL__STLX_TUPLE

#include "ext/ttl/typelist.hxx"
#include "ext/ttl/data_holder.hxx"

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
 namespace __
 {
   namespace meta = ttl::meta;

#define TTL_TUPLE_CALL_PARAM(n,p) typename tuple_param<types,N+n>::type p##n,
#define TTL_TUPLE_CALL_PARAM_END(n,p) typename tuple_param<types,N+n>::type p##n
#define TTL_TUPLE_CALL_PARAMS(n,p) TTL_REPEAT(n, TTL_TUPLE_CALL_PARAM, TTL_TUPLE_CALL_PARAM_END, p)

#define TTL_TUPLE_CALL_PARAM2(n,p) U##n&& p##n,
#define TTL_TUPLE_CALL_PARAM_END2(n,p) U##n&& p##n
#define TTL_TUPLE_CALL_PARAMS2(n,p) TTL_REPEAT(n, TTL_TUPLE_CALL_PARAM2, TTL_TUPLE_CALL_PARAM_END2, p)

#define TTL_TUPLE_CTOR(n,p) explicit tuple_data(TTL_TUPLE_CALL_PARAMS(n,p)) : \
  head_(p1), \
  tail_(TTL_ARGS_SX(TTL_DEC(n),p)) {}

#define TTL_TUPLE_UCTOR(n,p) template<TTL_TPARAMSX(n, U)> \
  explicit tuple_data(TTL_TUPLE_CALL_PARAMS2(n,p)) : \
    head_(p1), \
    tail_(TTL_ARGS_SX(TTL_DEC(n),p)) {}

   template<typename Types, size_t N, bool Stop = (N > meta::length<Types>::value) >
   struct tuple_param
   {
     typedef typename ttl::data_holder<typename meta::get<Types,N-1>::type >::param_type type;
   };

   template<typename Types, size_t N >
   struct tuple_param<Types, N, true >
   {
     typedef meta::empty_type type;
   };

   template<typename Types, size_t N = 0, bool Stop = (N >= meta::length<Types>::value) >
   struct tuple_data
   {
     typedef Types types;
     typedef tuple_data this_t;
     typedef typename ttl::meta::get<types, N>::type head_t;
     typedef tuple_data<types, N+1> tail_t;
     head_t head_;
     tail_t tail_;

   public:
     tuple_data()
       :head_(), tail_()
     {}

     TTL_REPEAT_NEST(TTL_MAX_TUPLE_PARAMS, TTL_TUPLE_CTOR, TTL_TUPLE_CTOR, p)
     //TTL_REPEAT_NEST(TTL_MAX_TUPLE_PARAMS, TTL_TUPLE_UCTOR, TTL_TUPLE_UCTOR, p)
     
     template<TTL_TPARAMSX(TTL_MAX_TUPLE_PARAMS, U)>
     tuple_data(const tuple_data<ttl::meta::typelist<TTL_ARGSX(TTL_MAX_TUPLE_PARAMS, U)>, N>& r)
       :head_(r.head_), tail_(r.tail_)
     {}
     
     tuple_data(const this_t& r) : head_(r.head_), tail_(r.tail_) {}

     this_t& operator=(const this_t& r)
     {
       if(&r == this) return *this;
       head_ = r.head_;
       tail_ = r.tail_;
       return *this;
     }

     template<TTL_TPARAMSX(TTL_MAX_TUPLE_PARAMS, U)>
     tuple_data& operator= (const tuple_data<ttl::meta::typelist<TTL_ARGSX(TTL_MAX_TUPLE_PARAMS, U)>, N>& r)
     {
       //if(&r == this) return *this;
       head_ = r.head_;
       tail_ = r.tail_;
       return *this;
     }

     #ifdef NTL__CXX_RV
     tuple_data(this_t&& r)
       :head_(forward<head_t>(r.head_)), 
       tail_(forward<tail_t>(r.tail_))
     {}
     template<TTL_TPARAMSX(TTL_MAX_TUPLE_PARAMS, U)>
     tuple_data(tuple_data<ttl::meta::typelist<TTL_ARGSX(TTL_MAX_TUPLE_PARAMS, U)>, N>&& r)
       :head_(forward<head_t>(r.head_)), 
       tail_(forward<tail_t>(r.tail_))
     {}

     this_t& operator=(this_t&& r)
     {
       if(&r == this) return *this;
       head_ = move(r.head_);
       tail_ = move(r.tail_);
       return *this;
     }
     template<TTL_TPARAMSX(TTL_MAX_TUPLE_PARAMS, U)>
     tuple_data& operator=(tuple_data<ttl::meta::typelist<TTL_ARGSX(TTL_MAX_TUPLE_PARAMS, U)>, N>&& r)
     {
       if(&r == this) return *this;
       head_ = move(r.head_);
       tail_ = move(r.tail_);
       return *this;
     }
     #endif
   };	

   template<typename Types, size_t N >
   struct tuple_data<Types, N, true>
   {
     typedef Types types;
     typedef tuple_data this_t;

     tuple_data() {}
     tuple_data(const this_t& r) {}
     this_t& operator=(const this_t& r)
     { return *this; }

     template<TTL_TPARAMSX(TTL_MAX_TUPLE_PARAMS, U)>
     tuple_data(const tuple_data<ttl::meta::typelist<TTL_ARGSX(TTL_MAX_TUPLE_PARAMS, U)>, N>& r)
     {}
     template<TTL_TPARAMSX(TTL_MAX_TUPLE_PARAMS, U)>
     tuple_data& operator=(const tuple_data<ttl::meta::typelist<TTL_ARGSX(TTL_MAX_TUPLE_PARAMS, U)>, N>&& r)
     { return *this; }

     #ifdef NTL__CXX_RV
     tuple_data(this_t&& r) {}
     this_t& operator=(this_t&& r)
     { return *this; }
     template<TTL_TPARAMSX(TTL_MAX_TUPLE_PARAMS, U)>
     tuple_data(tuple_data<ttl::meta::typelist<TTL_ARGSX(TTL_MAX_TUPLE_PARAMS, U)>, N>&& r)
     {}
     template<TTL_TPARAMSX(TTL_MAX_TUPLE_PARAMS, U)>
     tuple_data& operator=(tuple_data<ttl::meta::typelist<TTL_ARGSX(TTL_MAX_TUPLE_PARAMS, U)>, N>&& r)
     { return *this; }

     #endif
   };

#undef TTL_TUPLE_CALL_PARAM2
#undef TTL_TUPLE_CALL_PARAM_END2
#undef TTL_TUPLE_CALL_PARAMS2
#undef TTL_TUPLE_CALL_PARAM
#undef TTL_TUPLE_CALL_PARAM_END
#undef TTL_TUPLE_CALL_PARAMS
#undef TTL_TUPLE_CTOR
#undef TTL_TUPLE_UCTOR

   template<typename R, size_t N >
   struct get_field
   {
     template<typename T >
     R operator()(T& t)
     {
       get_field<R,N-1> g;
       return g(t.tail_);
     }
   };

   template<typename R >
   struct get_field<R, 0>
   {
     template<typename T >
     R operator()(T& t)
     {
       return t.head_ ;
     }
   };

   template<typename R, size_t N>
   struct field
   {
     template<typename T>
     static R get(T& t)
     {
       return field<R, N-1>::get(t.tail_);
     }
     template<typename T>
     static const R get(const T& t)
     {
       return field<R, N-1>::get(t.tail_);
     }
   };

   template<typename R>
   struct field<R, 0>
   {
     template<typename T>
     static R get(T& t)
     {
       return t.head_;
     }
     template<typename T>
     static const R get(const T& t)
     {
       return t.head_;
     }
   };


 };  // implementation


#define TTL_TUPLE_CALL_PARAM(n,p) typename __::tuple_param<types,n>::type p##n,
#define TTL_TUPLE_CALL_PARAM_END(n,p) typename __::tuple_param<types,n>::type p##n
#define TTL_TUPLE_CALL_PARAMS(n,p) TTL_REPEAT(n, TTL_TUPLE_CALL_PARAM, TTL_TUPLE_CALL_PARAM_END, p)

#define TTL_MK_U(n,p) forward<U##n>(p##n),
#define TTL_MK_U2(n,p) forward<U##n>(p##n)

#define TTL_TUPLE_CTOR(n,p)  explicit tuple(TTL_FUNC_CVPARAMS(n,p)): base(TTL_ARGSX(n,p)) {}
#define TTL_TUPLE_UCTOR(n,p) template<TTL_TPARAMSX(n, U)> explicit tuple(TTL_FUNC_UPARAMS(n,p)): base(TTL_REPEAT(n, TTL_MK_U, TTL_MK_U2, p)) {}
#define TTL_TUPLE_CCTOR(n,p) template<TTL_TPARAMSX(n, U)> tuple(const tuple<TTL_TPARAMSX(n,U)>& u): base(u) {}
#define TTL_TUPLE_CCTOR2(n,p)template<TTL_TPARAMSX(n, U)> tuple(tuple<TTL_TPARAMSX(n,U)>&& u): base(move(u)) {}

#define TTL_TUPLE_ASSIGN(n,p) template<TTL_TPARAMSX(n, U)> tuple& operator=(const tuple<TTL_ARGSX(n,U)>& u) { base::operator=(u); return *this; }
#define TTL_TUPLE_ASSIGN2(n,p)template<TTL_TPARAMSX(n, U)> tuple& operator=(tuple<TTL_ARGSX(n,U)>&& u) { base::operator=(u); return *this; }
// { base::operator=(move(u)); }

 /// Class template tuple [20.4.1 tuple.tuple]
 template<TTL_TPARAMS_DEF(TTL_MAX_TUPLE_PARAMS, ttl::meta::empty_type) >
 class tuple: 
   public __::tuple_data<ttl::meta::typelist<TTL_ARGS(TTL_MAX_TUPLE_PARAMS) > >
 {
   typedef __::tuple_data<types> base;

 public:
   typedef ttl::meta::typelist<TTL_ARGS(TTL_MAX_TUPLE_PARAMS) > types;

   template<size_t N>
   struct return_type
   {
     typedef typename ttl::data_holder<typename ttl::meta::get<types, N>::type>::return_type type;
   };

   template<size_t N>
   struct const_return_type
   {
     typedef typename ttl::data_holder<typename ttl::meta::get<types, N>::type>::const_return_type type;
   };

 public:
  /// 20.4.1.2 Construction [tuple.cnstr]
  tuple() {}

  // explicit tuple(const Types&...);
#define TTL_CV const
#define TTL_REF &
  //TTL_REPEAT_NEST(TTL_MAX_TUPLE_PARAMS, TTL_TUPLE_CTOR, TTL_TUPLE_CTOR, p)

  // template <class... UTypes> explicit tuple(UTypes&&... u);
#undef TTL_CV
#undef TTL_REF
#define TTL_CV
#define TTL_REF &&

  TTL_REPEAT_NEST(TTL_MAX_TUPLE_PARAMS, TTL_TUPLE_UCTOR, TTL_TUPLE_UCTOR, u)
#undef TTL_CV
#undef TTL_REF
  
  tuple(const tuple& u)
  :base(u)
  {}


  //template <class... UTypes> tuple(const tuple<UTypes...>&);

  TTL_REPEAT_NEST(TTL_MAX_TUPLE_PARAMS, TTL_TUPLE_CCTOR, TTL_TUPLE_CCTOR, p)

  //template <class... UTypes> tuple(tuple<UTypes...>&&);
  TTL_REPEAT_NEST(TTL_MAX_TUPLE_PARAMS, TTL_TUPLE_CCTOR2, TTL_TUPLE_CCTOR2, p)

#ifdef NO_TUPLE2
  //tuple(const pair<T1, T2>& u);
  template <class U1, class U2>
  tuple(const pair<U1, U2>& u);

  //tuple(pair<T1, T2>&& u);
  template <class U1, class U2>
  tuple(pair<U1, U2>&& u);
#endif
  // TODO: allocator-extended constructors
  //template <class Alloc>
  //tuple(allocator_arg_t, const Alloc& a);
  //template <class Alloc>
  //tuple(allocator_arg_t, const Alloc& a, const Types&...);
  //template <class Alloc, class... UTypes>
  //tuple(allocator_arg_t, const Alloc& a, const UTypes&&...);
  //template <class Alloc>
  //tuple(allocator_arg_t, const Alloc& a, const tuple&);
  //template <class Alloc>
  //tuple(allocator_arg_t, const Alloc& a, tuple&&);
  //template <class Alloc, class... UTypes>
  //tuple(allocator_arg_t, const Alloc& a, const tuple<UTypes...>&);
  //template <class Alloc, class... UTypes>
  //tuple(allocator_arg_t, const Alloc& a, tuple<UTypes...>&&);
  //template <class Alloc, class U1, class U2>
  //tuple(allocator_arg_t, const Alloc& a, const pair<U1, U2>&);
  //template <class Alloc, class U1, class U2>
  //tuple(allocator_arg_t, const Alloc& a, pair<U1, U2>&&);

  #ifdef NTL__CXX_RV
  tuple(tuple&& u)
    :base(forward<base>(u))
  {}
  
  tuple& operator=(tuple&& r)
  { 
    base::operator=(forward<base>(r));
    return *this;
  }
  
  //template <class... UTypes> tuple& operator=(tuple<UTypes...>&&);
  TTL_REPEAT_NEST(TTL_MAX_TUPLE_PARAMS, TTL_TUPLE_ASSIGN2, TTL_TUPLE_ASSIGN2, p)
  #endif

  tuple& operator=(const tuple& r)
  { 
   base::operator=(r);
   return *this;
  }

  //template <class... UTypes> tuple& operator=(const tuple<UTypes...>&);
  TTL_REPEAT_NEST(TTL_MAX_TUPLE_PARAMS, TTL_TUPLE_ASSIGN, TTL_TUPLE_ASSIGN, p)

#ifdef NO_TUPLE2
  template <class U1, class U2>
  tuple& operator=(const pair<U1, U2>& u);
  template <class U1, class U2>
  tuple& operator=(pair<U1, U2>&& u);
#endif
};

#undef TTL_TUPLE_CALL_PARAM
#undef TTL_TUPLE_CALL_PARAM_END
#undef TTL_TUPLE_CALL_PARAMS
#undef TTL_TUPLE_CTOR

#ifndef NO_TUPLE2
template<typename T1, typename T2>
class tuple<T1,T2>: 
  public __::tuple_data<ttl::meta::typelist<T1,T2> >
{
  typedef __::tuple_data<types> base;

public:
  typedef ttl::meta::typelist<T1,T2> types;

  template<size_t N>
  struct return_type
  {
    typedef typename ttl::data_holder<typename ttl::meta::get<types, N>::type>::return_type type;
  };

  template<size_t N>
  struct const_return_type
  {
    typedef typename ttl::data_holder<typename ttl::meta::get<types, N>::type>::const_return_type type;
  };

public:
  /// 20.4.1.2 Construction [tuple.cnstr]
  tuple() {}

  tuple(const T1& t1, const T2& t2);
  //tuple(T1&& t1, T2&& t2);

  //template <class U1, class U2>
  //tuple(const U1& u1, const U2& u2);

  template <class U1, class U2>
  tuple(U1&& u1, U2&& u2);

  tuple(const tuple& u)
    :base(u)
  {}

  tuple(tuple&&);

  template <class U1, class U2>
  tuple(const tuple<U1,U2>&);

  template <class U1, class U2>
  tuple(tuple<U1,U2>&&);

  template <class U1, class U2>
  tuple(const pair<U1, U2>& u);

  template <class U1, class U2>
  tuple(pair<U1, U2>&& u);

  // TODO: allocator-extended constructors
  template <class Alloc>
  tuple(allocator_arg_t, const Alloc& a);
  //template <class Alloc>
  //tuple(allocator_arg_t, const Alloc& a, const Types&...);
  //template <class Alloc, class... UTypes>
  //tuple(allocator_arg_t, const Alloc& a, const UTypes&&...);
  //template <class Alloc>
  //tuple(allocator_arg_t, const Alloc& a, const tuple&);
  //template <class Alloc>
  //tuple(allocator_arg_t, const Alloc& a, tuple&&);
  //template <class Alloc, class... UTypes>
  //tuple(allocator_arg_t, const Alloc& a, const tuple<UTypes...>&);
  //template <class Alloc, class... UTypes>
  //tuple(allocator_arg_t, const Alloc& a, tuple<UTypes...>&&);
  template <class Alloc, class U1, class U2>
  tuple(allocator_arg_t, const Alloc& a, const pair<U1, U2>&);
  template <class Alloc, class U1, class U2>
  tuple(allocator_arg_t, const Alloc& a, pair<U1, U2>&&);

  tuple& operator=(const tuple& r)
  { 
    base::operator=(r);
    return *this;
  }

  tuple& operator=(tuple&& r)
  { 
    base::operator=(forward<base>(r));
    return *this;
  }

  template <class U1, class U2>
  tuple& operator=(const tuple<U1,U2>&);

  template <class U1, class U2>
  tuple& operator=(tuple<U1,U2>&&);


  template <class U1, class U2>
  tuple& operator=(const pair<U1, U2>& u)
  {
    *this = tuple<U1,U2>(u);
    return *this;
  }
  template <class U1, class U2>
  tuple& operator=(pair<U1, U2>&& u)
  {
    *this = move(tuple<U1,U2>(forward<pair<U1,U2>(u)));
    return *this;
  }
};
#endif

 //////////////////////////////////////////////////////////////////////////
 // 20.4.1.3 Tuple creation functions [tuple.creation]

 namespace __
 {
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
   struct ttraits
   {
     typedef decay<T> U;
     typedef typename conditional<is_refwrap<U>::value, typename is_refwrap<U>::type&, U>::type type;
   };

   template<TTL_TPARAMS_DEF(TTL_MAX_TUPLE_PARAMS, ttl::meta::empty_type)>
   struct tmap
   {
     typedef tuple<TTL_ARGS(TTL_MAX_TUPLE_PARAMS)> type;
   };

   struct swallow_assign
   {
     template<typename T>
     const swallow_assign& operator=(const T&) const
     { return *this; }
   };
 }

 // "ignore" allows tuple positions to be ignored when using "tie".
 __::swallow_assign const ignore = __::swallow_assign();

 
 tuple<> make_tuple()
 {
   return tuple<>();
 }

#ifdef NTL__CXX_RV
#define TTL_CV
#define TTL_REF &&
#else
#define TTL_CV const
#define TTL_REF &
#endif

#define TTL_MK_P(n,p) forward<T##n>(p##n),
#define TTL_MK_P2(n,p) forward<T##n>(p##n)

#define TTL_MAKE_TUPLE(N,p) template<TTL_TPARAMS(N)> \
  typename __::tmap<TTL_ARGS(N)>::type \
  make_tuple(TTL_FUNC_CVPARAMS(N, p)) \
  { \
    return __::tmap<TTL_ARGS(N)>::type(TTL_REPEAT(N, TTL_MK_P, TTL_MK_P2, p)); \
  } 

#define TTL_TIE_TUPLE(N,p) template<TTL_TPARAMS(N)> \
  typename __::tmap<TTL_ARGSCV(N)>::type \
  tie(TTL_FUNC_CVPARAMS(N, p)) \
 { \
   return typename __::tmap<TTL_ARGSCV(N)>::type( TTL_ARGSX(N, p) ); \
 } 

 TTL_REPEAT_NEST(TTL_MAX_TUPLE_PARAMS, TTL_MAKE_TUPLE, TTL_MAKE_TUPLE, p)

#undef TTL_CV
#undef TTL_REF
#define TTL_CV
#define TTL_REF &

 TTL_REPEAT_NEST(TTL_MAX_TUPLE_PARAMS, TTL_TIE_TUPLE, TTL_TIE_TUPLE, p)

#undef TTL_CV
#undef TTL_REF
#undef TTL_MK_P
#undef TTL_MK_P2
#undef TTL_MAKE_TUPLE
#undef TTL_TIE_TUPLE

//////////////////////////////////////////////////////////////////////////
 // 20.4.1.4, tuple helper classes:
 
 /// Tuple helper class that calculates a summary size of the types which tuple<> contains
 template<TTL_TPARAMS_DEF(TTL_MAX_TUPLE_PARAMS, ttl::meta::empty_type)>
 struct tuple_size; // undefined

 /// Tuple helper class that returns a type at tuple in position I
 template<size_t I, TTL_TPARAMS_DEF(TTL_MAX_TUPLE_PARAMS, ttl::meta::empty_type)>
 struct tuple_element; // undefined

 namespace __
 {
   namespace meta = ttl::meta;

   template<class Tuple, size_t N>
   struct tuple_return
   {
     typedef typename conditional
       <
       is_const<Tuple&>::value, 
       typename Tuple::template const_return_type<N>::type, 
       typename Tuple::template return_type<N>::type
       >::type type;
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

 /// tuple_size specialization for tuple
 template<TTL_TPARAMS(TTL_MAX_TUPLE_PARAMS)>
 struct tuple_size<tuple<TTL_ARGS(TTL_MAX_TUPLE_PARAMS)> >
   :integral_constant<size_t, ttl::meta::length<typename tuple<TTL_ARGS(TTL_MAX_TUPLE_PARAMS)>::types>::value>
 {};
 
 /// tuple-like access to pair
 template <class T1, class T2> 
 struct tuple_size<std::pair<T1, T2> >
   :integral_constant<size_t, 2>
 {};

#undef TTL_SIZEOF_TUPLE
#undef TTL_SIZEOF_TUPLE_END

 /// tuple_element specialization for tuple
 template<size_t I, TTL_TPARAMS(TTL_MAX_TUPLE_PARAMS)>
 struct tuple_element<I, tuple<TTL_ARGS(TTL_MAX_TUPLE_PARAMS)> >
 {
   typedef tuple<TTL_ARGS(TTL_MAX_TUPLE_PARAMS)> tuple_t;
   static_assert(I < tuple_size<tuple_t>::value, "I is out of bounds");

   typedef typename ttl::meta::get<typename tuple_t::types, I>::type type;
 };

 template <size_t I, class T1, class T2>
 struct tuple_element<I, std::pair<T1, T2> >; // undefined
 template <class T1, class T2>
 struct tuple_element<0, std::pair<T1, T2> > { typedef T1 type; };
 template <class T1, class T2> 
 struct tuple_element<1, std::pair<T1, T2> > { typedef T2 type; };

 //////////////////////////////////////////////////////////////////////////
 // 20.4.1.5, element access:
 // TODO: tuple specialization
 template<size_t N, typename T>
 typename __::tuple_return<T, N>::type get(T& t)
 {
   static_assert(N < tuple_size<T>::value, "N out of bounds");
   typedef typename __::tuple_return<T, N>::type return_type;
   return __::field<return_type, N>::get(t);
 }

 template<size_t N, typename T>
 typename tuple_element<N, T>::type get(const T& t)
 {
   static_assert(N < tuple_size<T>::value, "N out of bounds");
   return __::field<tuple_element<N, T>::type, N>::get(t);
 }

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
 ///\name  Tuple interface to class template array [6.2.2.5]

 template <class T, size_t N> 
 struct tuple_size<array<T, N> >: integral_constant<size_t, N>
 {};

 template <size_t I, class T, size_t N>
 struct tuple_element<I, array<T, N> >
 {
   static_assert(I < N, "I out of bounds");
   typedef T type;
 };

 template <int I, class T, size_t N>
 T&
   get(array<T, N>& a)
 {
   return a[I];
 }

 template <int I, class T, size_t N>
 const T&
   get(const array<T, N>& a)
 {
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
