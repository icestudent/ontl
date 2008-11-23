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
#if 0
 // 20.4.1, class template tuple:
 template <class... Types> class tuple;

 // 20.4.1.3, tuple creation functions:
 const unspecified ignore;
 template <class... Types>
 tuple<VTypes...> make_tuple(Types&&...);
 template<class... Types>
 tuple<Types&...> tie(Types&...);
 template <class... TTypes, class... UTypes>
 tuple<TTypes..., UTypes...> tuple_cat(const tuple<TTypes...>&, const tuple<UTypes...>&);
 template <class... TTypes, class... UTypes>
 tuple<TTypes..., UTypes...> tuple_cat(tuple<TTypes...>&&, const tuple<UTypes...>&);
 template <class... TTypes, class... UTypes>
 tuple<TTypes..., UTypes...> tuple_cat(const tuple<TTypes...>&, tuple<UTypes...>&&);
 template <class... TTypes, class... UTypes>
 tuple<TTypes..., UTypes...> tuple_cat(tuple<TTypes...>&&, tuple<UTypes...>&&);
 
 // 20.4.1.4, tuple helper classes:
 template <class T> class tuple_size; // undefined
 template <class... Types> class tuple_size<tuple<Types...> >;
 template <size_t I, class T> class tuple_element; // undefined
 template <size_t I, class... Types> class tuple_element<I, tuple<Types...> >;
 
 // 20.4.1.5, element access:
 template <size_t I, class... Types>
 typename tuple_element<I, tuple<Types...> >::type& get(tuple<Types...>&);
 template <size_t I, class ... types>
 typename tuple_element<I, tuple<Types...> >::type const& get(const tuple<Types...>&);
 
 // 20.4.1.6, relational operators:
 template<class... TTypes, class... UTypes>
 bool operator==(const tuple<TTypes...>&, const tuple<UTypes...>&);
 template<class... TTypes, class... UTypes>
 bool operator<(const tuple<TTypes...>&, const tuple<UTypes...>&);
 template<class... TTypes, class... UTypes>
 bool operator!=(const tuple<TTypes...>&, const tuple<UTypes...>&);
 template<class... TTypes, class... UTypes>
 bool operator>(const tuple<TTypes...>&, const tuple<UTypes...>&);
 template<class... TTypes, class... UTypes>
 bool operator<=(const tuple<TTypes...>&, const tuple<UTypes...>&);
 template<class... TTypes, class... UTypes>
 bool operator>=(const tuple<TTypes...>&, const tuple<UTypes...>&);
#endif

 namespace __
 {
   namespace meta = ttl::meta;

#define TTL_TUPLE_CALL_PARAM(n,p) typename tuple_param<types,N+n>::type p##n,
#define TTL_TUPLE_CALL_PARAM_END(n,p) typename tuple_param<types,N+n>::type p##n
#define TTL_TUPLE_CALL_PARAMS(n,p) TTL_REPEAT(n, TTL_TUPLE_CALL_PARAM, TTL_TUPLE_CALL_PARAM_END, p)

#define TTL_TUPLE_CTOR(n,p) tuple_data(TTL_TUPLE_CALL_PARAMS(n,p)) : \
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
     typedef tuple_data this_t;
     typedef Types types;
     typedef typename ttl::meta::get<types, N>::type head_t;
     typedef tuple_data<types, N+1> tail_t;
     head_t head_;
     tail_t tail_;

     tuple_data() 
     {}
     TTL_REPEAT_NEST(TTL_MAX_TUPLE_PARAMS, TTL_TUPLE_CTOR, TTL_TUPLE_CTOR, p)
     
     tuple_data(const this_t& r) : head_(r.head_), tail_(r.tail_) {}

     this_t& operator=(const this_t& r)
     {
       if(&r == this) return *this;
       head_ = r.head_;
       tail_ = r.tail_;
       return *this;
     }
     
     #ifdef NTL__CXX_RV
     tuple_data(this_t&& r)
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
     {
       return *this;
     }
     #ifdef NTL__CXX_RV
     tuple_data(const this_t&& r) {}

     this_t& operator=(const this_t&& r)
     {
       return *this;
     }
     #endif
   };

#undef TTL_TUPLE_CALL_PARAM
#undef TTL_TUPLE_CALL_PARAM_END
#undef TTL_TUPLE_CALL_PARAMS
#undef TTL_TUPLE_CTOR

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

 };  // implementation


#define TTL_TUPLE_CALL_PARAM(n,p) typename __::tuple_param<types,n>::type p##n,
#define TTL_TUPLE_CALL_PARAM_END(n,p) typename __::tuple_param<types,n>::type p##n
#define TTL_TUPLE_CALL_PARAMS(n,p) TTL_REPEAT(n, TTL_TUPLE_CALL_PARAM, TTL_TUPLE_CALL_PARAM_END, p)

#define TTL_TUPLE_CTOR(n,p) explicit tuple(TTL_TUPLE_CALL_PARAMS(n,p)): base(TTL_ARGSX(n,p)) {}


 /// Class template tuple [20.4.1 tuple.tuple]
 template<TTL_TPARAMS_DEF(TTL_MAX_TUPLE_PARAMS, ttl::meta::empty_type) >
 class tuple: __::tuple_data<ttl::meta::typelist<TTL_ARGS(TTL_MAX_TUPLE_PARAMS) > >
 {
   typedef __::tuple_data<types> base;

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
  typedef ttl::meta::typelist<TTL_ARGS(TTL_MAX_TUPLE_PARAMS) > types;

  /// 20.4.1.2 Construction [tuple.cnstr]
  tuple() {}

  TTL_REPEAT_NEST(TTL_MAX_TUPLE_PARAMS, TTL_TUPLE_CTOR, TTL_TUPLE_CTOR, p)
  
  //template <class... UTypes>
  //explicit tuple(UTypes&&...);

  //tuple(const base& r) : base(r) {}
  tuple(const tuple& u)
  :base(u)
  {}

  //template <class... UTypes>
  //tuple(const tuple<UTypes...>&);
  //template <class... UTypes>
  //tuple(tuple<UTypes...>&&);

  //template <class U1, class U2>
  //tuple(const pair<U1, U2>& u);
  //template <class U1, class U2>
  //tuple(pair<U1, U2>&&); // if sizeof...(Types) == 2

  // allocator-extended constructors
  template <class Alloc>
  tuple(allocator_arg_t, const Alloc& a);
  //template <class Alloc>
  //tuple(allocator_arg_t, const Alloc& a, const Types&...);
  //template <class Alloc, class... UTypes>
  //tuple(allocator_arg_t, const Alloc& a, const UTypes&&...);
  //template <class Alloc>
  //tuple(allocator_arg_t, const Alloc& a, const tuple&);
  template <class Alloc>
  tuple(allocator_arg_t, const Alloc& a, tuple&&);
  //template <class Alloc, class... UTypes>
  //tuple(allocator_arg_t, const Alloc& a, const tuple<UTypes...>&);
  //template <class Alloc, class... UTypes>
  //tuple(allocator_arg_t, const Alloc& a, tuple<UTypes...>&&);
  template <class Alloc, class U1, class U2>
  tuple(allocator_arg_t, const Alloc& a, const pair<U1, U2>&);
  template <class Alloc, class U1, class U2>
  tuple(allocator_arg_t, const Alloc& a, pair<U1, U2>&&);

  #ifdef NTL__CXX_RV
  tuple(tuple&& u)
    :base(forward<base>(u))
  {}
  
  tuple& operator=(tuple&& r)
  { 
    base::operator=(forward<base>(r));
    return *this;
  }
  #endif

  tuple& operator=(const tuple& r)
  { 
   base::operator=(r);
   return *this;
  }

  //template <class... UTypes>
  //tuple& operator=(const tuple<UTypes...>&);
  //template <class... UTypes>
  //tuple& operator=(tuple<UTypes...>&&);
  //template <class U1, class U2>
  //tuple& operator=(const pair<U1, U2>&); // if sizeof...(Types) == 2
  //template <class U1, class U2>
  //tuple& operator=(pair<U1, U2>&&); // if sizeof...(Types) == 2

};

#undef TTL_TUPLE_CALL_PARAM
#undef TTL_TUPLE_CALL_PARAM_END
#undef TTL_TUPLE_CALL_PARAMS
#undef TTL_TUPLE_CTOR

 //////////////////////////////////////////////////////////////////////////
 // 20.4.1.3 Tuple creation functions [tuple.creation]

 // const unspecified ignore; // wtf?

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
 }
 
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

TTL_REPEAT_NEST(TTL_MAX_TUPLE_PARAMS, TTL_MAKE_TUPLE, TTL_MAKE_TUPLE, p)
#undef TTL_CV
#undef TTL_REF
#undef TTL_MK_P
#undef TTL_MK_P2
#undef TTL_MAKE_TUPLE

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

   template<TTL_TPARAMS_DEF(TTL_MAX_TUPLE_PARAMS, ttl::meta::empty_type)>
   struct tuple_length
   { static_assert(1, "tuple_length must be a applied only on tuple type"); };

   template<TTL_TPARAMS(TTL_MAX_TUPLE_PARAMS)>
   struct tuple_length<tuple<TTL_ARGS(TTL_MAX_TUPLE_PARAMS)> >
   {
     typedef typename tuple<TTL_ARGS(TTL_MAX_TUPLE_PARAMS)> tuple;
     enum { value = ttl::meta::length<typename tuple::types>::value };
   };

   template<typename T>
   struct tuple_size_helper { enum { size = sizeof(T) }; };
   template<>
   struct tuple_size_helper<ttl::meta::empty_type> { enum { size = 0 }; };

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
#define TTL_SIZEOF_TUPLE(n, p) __::tuple_size_helper<p##n>::size +
#define TTL_SIZEOF_TUPLE_END(n, p) __::tuple_size_helper<p##n>::size
 /// tuple_size specialization for tuple
 template<TTL_TPARAMS(TTL_MAX_TUPLE_PARAMS)>
 struct tuple_size<tuple<TTL_ARGS(TTL_MAX_TUPLE_PARAMS)> >
   :integral_constant<size_t, TTL_REPEAT(TTL_MAX_TUPLE_PARAMS, TTL_SIZEOF_TUPLE, TTL_SIZEOF_TUPLE_END, T) >
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
   static_assert(I < __::tuple_length<tuple_t>::value, "I is out of bounds");

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
   typedef __::tuple_return<T, N>::type return_type;
   __::get_field<return_type, N > g;
   return g(t);
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
