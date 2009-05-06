/**\file*********************************************************************
 *                                                                     \brief
 *  Type list support
 *
 ****************************************************************************
 */
#ifndef NTL__EXT_TYPELIST
#define NTL__EXT_TYPELIST

#include "ttl/typelist.hxx"

namespace ntl
{
  /// Metaprogramming
  namespace meta
  {
    using ttl::meta::empty_type;
    using ttl::meta::null_type;

    // list
    template<class H = empty_type, class T = empty_type> struct tlist { typedef H head; typedef T tail; };

    /// Type list support
    namespace tl
    {
      template<class T, class A> struct append { typedef typename append<tlist<T>,A>::type type; };

      // null + null = null
      template<> struct append<empty_type,empty_type> { typedef empty_type type; };

      // null + A = tlist<A>
      template<class A> struct append<empty_type,A> { typedef tlist<A> type; };

      // null + tlist<1,2> = tlist<1,2>
      template<class H, class T> struct append<empty_type, tlist<H,T> > { typedef tlist<H,T> type; };

      template<class H, class T, class A> struct append<tlist<H,T>, A> { typedef tlist<H, typename append<T,A>::type> type; };


      //// A + B = tlist<A,tlist<B>>
      //template<class A, class B> struct append/*<A,B>*/ { typedef tlist<A,tlist<B> > type; };

      //// tlist<> + tlist<1,2> = tlist<1,2>
      //template<class H,class T> struct append<tlist<>, tlist<H,T> > { typedef tlist<H,T> type; };

      //// tlist<1,2> + A
      //template<class H, class T, class A>
      //struct append<tlist<H,T>,A> { typedef tlist<tlist<H,T>,A> type; };

      //// tlist<> + A
      //template<class A>
      //struct append<tlist<empty_type,empty_type>, A> { typedef tlist<A> type; };

      //// tlist<1> + A
      //template<class H, class A>
      //struct append<tlist<H,empty_type>, A> { typedef tlist<H, A> type; };

      //// tlist<1,2> + null
      //template<class H, class T>
      //struct append<tlist<H,T>, empty_type> { typedef tlist<H,T> type; };
      //// tlist<1,2> + tlist<>
      //template<class H, class T>
      //struct append<tlist<H,T>, tlist<> > { typedef tlist<H,T> type; };

      // length
      template<class TL> struct length;
      template<class H, class T> struct length<tlist<H,T> > { enum { value = 1 + length<T>::value }; };
      template<> struct length<empty_type> { enum { value = 0 }; };

      // get
      template<size_t I, class TL> struct get;
      template<size_t I, class H, class T> struct get<I, tlist<H,T> > { typedef typename get<I-1,T>::type type; };
      template<class H, class T> struct get<0,tlist<H,T> > { typedef H type; };

      // 
      template<class TL> struct remove_tail;
      template<class H> struct remove_tail<tlist<H,empty_type> > { typedef empty_type type; };
      template<class H, class T> struct remove_tail<tlist<H,T> > { typedef tlist<H, typename remove_tail<T>::type> type; };

      // tuple to tlist
      template<size_t,typename T> struct skip_handler 
      {
        static const bool stop = false;
        typedef T type;
      };

      // transform tlist
      template<template<size_t,typename> class handler, class TL, size_t I = 0>
      struct transform
      {
        typedef typename append<typename handler<I, typename TL::head>::type, typename transform<handler, typename TL::tail, I+1>::type>::type type;
      };
      template<template<size_t,typename> class handler, size_t I>
      struct transform<handler, empty_type, I> { typedef empty_type type; };

    } // tl
  } // meta
} // ntl

#ifdef NTL_TYPELIST_DEBUG
  using ntl::meta::tlist;
  namespace tl = ntl::meta::tl;
#endif

#endif // NTL__EXT_TYPELIST
