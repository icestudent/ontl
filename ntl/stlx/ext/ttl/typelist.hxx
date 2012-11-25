//  typelist.hpp
//
//  Copyright (c) 2003 Eugene Gladyshev
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//

#ifndef __ttl_typelist__hpp
#define __ttl_typelist__hpp
#pragma once

#include "../pp/config.hxx"
#include "data_holder.hxx"
//#include "exception.hxx"
#include "../pp/macro_params.hxx"

namespace ttl
{
  namespace meta
  {	
    struct empty_type {};
    class  null_type  {};

    template<size_t v>
    struct int2type
    {
      enum { value = v };
    };

    //namespace meta_rel_ops
    //{
      inline bool operator== (const ttl::meta::empty_type&, const ttl::meta::empty_type&) {  return true; }
      inline bool operator!= (const ttl::meta::empty_type&, const ttl::meta::empty_type&) {  return false; }
      inline bool operator<  (const ttl::meta::empty_type&, const ttl::meta::empty_type&) {  return false; }
      inline bool operator>  (const ttl::meta::empty_type&, const ttl::meta::empty_type&) {  return false; }
      inline bool operator<= (const ttl::meta::empty_type&, const ttl::meta::empty_type&) {  return true; }
      inline bool operator>= (const ttl::meta::empty_type&, const ttl::meta::empty_type&) {  return true; }

      template<typename T> inline bool operator== (const T&, const ttl::meta::empty_type&) {  return false; }
      template<typename T> inline bool operator!= (const T&, const ttl::meta::empty_type&) {  return true; }
      template<typename T> inline bool operator<  (const T&, const ttl::meta::empty_type&) {  return false; }
      template<typename T> inline bool operator>  (const T&, const ttl::meta::empty_type&) {  return false; }
      template<typename T> inline bool operator<= (const T&, const ttl::meta::empty_type&) {  return false; }
      template<typename T> inline bool operator>= (const T&, const ttl::meta::empty_type&) {  return false; }

      template<typename T> inline bool operator== (const ttl::meta::empty_type&, const T&) {  return false; }
      template<typename T> inline bool operator!= (const ttl::meta::empty_type&, const T&) {  return true; }
      template<typename T> inline bool operator<  (const ttl::meta::empty_type&, const T&) {  return false; }
      template<typename T> inline bool operator>  (const ttl::meta::empty_type&, const T&) {  return false; }
      template<typename T> inline bool operator<= (const ttl::meta::empty_type&, const T&) {  return false; }
      template<typename T> inline bool operator>= (const ttl::meta::empty_type&, const T&) {  return false; }
    //}

    //using namespace meta_rel_ops;

    //internal implementation
#ifdef NTL_CXX_VT_TYPELIST
      namespace impl
      {
        struct typelist_traits
        {
          typedef typelist_traits
        };

      }
#else
    namespace impl
    {
      ////////////////////////////////////////////////////////////
      template< TTL_TPARAMS_DEF(TTL_MAX_TYPELIST_PARAMS, empty_type) >
      struct typelist_traits
      {
        typedef typelist_traits< TTL_ARGS_S(TTL_DEC(TTL_MAX_TYPELIST_PARAMS)) > tail;

        enum
        {
          length = 1 + tail::length,
        };

        typedef typename std::conditional< sizeof(ttl::data_holder<T1>) >=sizeof(ttl::data_holder<typename tail::largest_type>),
          T1,
          typename tail::largest_type
        >::type largest_type;
      };

      template<>
      struct typelist_traits< TTL_LIST_ITEMS(TTL_MAX_TYPELIST_PARAMS,empty_type) >
      {
        typedef empty_type tail;
        enum
        {
          length = 0,
        };
        typedef empty_type largest_type;
      };

      ////////////////////////////////////////////////////////////
      //
      //Instantiate TTL_MAX_TYPELIST_PARAMS get<> templates
      //	template<typename T>  struct get<T, 0>
      //	{ 
      //		enum { index = 0 };			
      //		typedef typename T::type1 type; 
      //	};
      //	
      //	template<typename T>  struct get<T, 1>
      //	{ 
      //		enum { index = 1 };			
      //		typedef typename T::type2 type; 
      //	};
      //	...
      //

      template< typename T, int N > struct get;

#define TTL_META_TYPELIST_GET(n, t) template<typename T>  struct get<T, TTL_CNTDEC_##n>  \
      { enum {index = n-1}; typedef typename T::t##n type; };

      TTL_REPEAT( TTL_MAX_TYPELIST_PARAMS, TTL_META_TYPELIST_GET, TTL_META_TYPELIST_GET, type )

#undef TTL_META_TYPELIST_GET
    } // impl

    template < TTL_TPARAMS_DEF(TTL_MAX_TYPELIST_PARAMS, empty_type) >
    struct typelist
    {
      TTL_TYPEDEFS(TTL_MAX_TYPELIST_PARAMS)

        typedef impl::typelist_traits< TTL_ARGS(TTL_MAX_TYPELIST_PARAMS) > list_traits;

      enum { length = list_traits::length };
      typedef typename list_traits::largest_type largest_type;
    };
#endif

    ////////////////////////////////////////////////////////////
    template < typename L >
    struct length
    {
      enum { value = L::length };
    };

    ///////////////////////////////////////////////////////////
    template< typename L, size_t N/*, bool Ok = (N < length<L>::value)*/ >
    struct get
    {
      static_assert(N < length<L>::value, "N out of bounds");
      typedef typename impl::get<L,N>::type type;
      enum{ index = N };
    };

    //////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct equivalent_types: 
      std::integral_constant<bool, std::is_same<typename std::decay<T>::type, typename std::decay<U>::type>::value>
    {};

    template< typename T, typename L, int N = 0, bool Stop=(N>=length<L>::value) >
    struct find_equivalent_type
    {
    private:
      typedef impl::get<L,N> get_type;

      typedef typename std::conditional< equivalent_types<typename get_type::type, T>::value,
        get_type,
        find_equivalent_type<T,L,N+1> 
      >::type found;

    public:
      typedef typename found::type type;
      enum {index = found::index};
    };

    template<typename T, typename L, int N>
    struct find_equivalent_type<T, L, N, true>
    {};


    //////////////////////////////////////////////
    template< typename T, typename TypeList, int N = 0, bool Stop=(N>=length<TypeList>::value) >
    struct find
    {
    private:
      typedef impl::get<TypeList,N> get_type;

      typedef typename std::conditional< std::is_same<typename get_type::type, T>::value,
        get_type,
        find<T,TypeList,N+1> 
      >::type found;

    public:
      typedef typename found::type type;
      enum {index = found::index};
    };

    template<typename T, typename TypeList, int N>
    struct find<T, TypeList, N, true>
    {};

    template<typename T, typename TypeList, int N = 0, bool Stop=(N>=length<TypeList>::value) >
    struct index_of
    {
    private:
      typedef impl::get<TypeList,N> get_type;

      typedef typename std::conditional<std::is_same<typename get_type::type, T>::value,
        get_type,
        index_of<T,TypeList,N+1>
      >::type found;

    public:
      typedef typename found::type type;
      enum {index = found::index};
    };

    template<typename T, typename TypeList, int N>
    struct index_of<T, TypeList, N, true>
    {
      typedef void type;
      enum { index = -1 };
    };

  }
}

#endif //__typelist__hpp
