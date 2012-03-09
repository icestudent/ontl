/**\file*********************************************************************
 *                                                                     \brief
 *  20.7 Function objects [function.objects]
 *
 ****************************************************************************
 */
#ifndef NTL_MEMFN_RV
#define NTL_MEMFN_RV
#pragma once

#ifdef NTL_CXX_RV__WORKS
# include "fn_caller_rv.hxx"
#else
# include "fn_caller.hxx"
#endif

namespace std
{
 /**\addtogroup  lib_utilities ************ 20 General utilities library [utilities]
  *@{*/
 /**\addtogroup  lib_function_objects ***** 20.7 Function objects [function.objects]
  *@{*/
  /**\defgroup  lib_member_function_adaptors 20.7.14 Member function adaptors [func.memfn]
  *@{*/


  template<typename R, class T>
  class mem_pm_t:
    public unary_function<T*, R>
  {
    R T::*pm;
  public:

    typedef typename unary_function::result_type    result_type;
    typedef typename unary_function::argument_type  argument_type;
    
    explicit mem_pm_t(R T::*pm)
      :pm(pm)
    {}

    template<class U>
    result_type& operator()(U&& obj) const
    {
      return call(forward<U>(obj),&obj);
    }
  private:
    template<class U>
    result_type& call(U&& obj, const volatile T*) const { return obj.*pm; }
    template<class U>
    result_type& call(U&& obj, const volatile void*) const { return (*obj).*pm; }
  };

  template<typename R, class T>
  mem_pm_t<R,T> mem_fn(R T::*pm)
  {
    return mem_pm_t<R,T> (pm);
  }

  namespace __
  {
    template<bool f, size_t, class>  struct get_arg_t
    { typedef ttl::meta::empty_type type; };
    template<size_t I, class Args> struct get_arg_t<true, I, Args> 
    { typedef typename tuple_element<I, Args>::type type; };

    template<size_t I, class Args> struct arg_t: get_arg_t<(tuple_size<Args>::value > I), I, Args>
    {};

    template<typename R, class T, class Args, size_t Argc = tuple_size<Args>::value>
    struct memfun_arity { typedef R result_type; };

    template<typename R, class T, class Args>
    struct memfun_arity<R,T,Args,0>: unary_function<T*, R>{};

    template<typename R, class T, class Args>
    struct memfun_arity<R,T,Args,1>: binary_function<T*, typename tuple_element<0, Args>::type, R>{};
  }
#ifndef NTL_DOC
#define NTL_DEFINE_MEMFNARG(n,aux), typename __::arg_t<NTL_SPP_DEC(n), Args>::type a ## n
#define NTL_DEFINE_MEMFNARGT(n,aux), typename __::arg_t<NTL_SPP_DEC(n), Args>::type 

#define NTL_DEFINE_MEMFNOP(n, aux) \
  template<typename U> \
  result_type operator()(U&& obj TTL_REPEAT(n, NTL_DEFINE_MEMFNARG, NTL_DEFINE_MEMFNARG, aux)) const \
  { \
    static_assert(tuple_size<Args>::value == n, "wrong count of arguments"); \
    return __::func::invoke<result_type>(pmf, typename __::tmap<U&& TTL_REPEAT(n, NTL_DEFINE_MEMFNARGT, NTL_DEFINE_MEMFNARGT, aux)>::type(forward<U>(obj), NTL_SPP_NEST( NTL_SPP_ARGS(1,n,a) ) )); \
  } 
#endif

  template<typename R, class T, class Functor, class Args = tuple<> >
  class mem_fn_t:
    public __::memfun_arity<R, T, Args>
  {
    Functor pmf;
  public:

    using __::memfun_arity<R, T, Args>::result_type;

    explicit mem_fn_t(Functor f)
      :pmf(f)
    {}

    template<typename U>
    result_type operator()(U&& obj) const
    {
      static_assert(tuple_size<Args>::value == 0, "wrong count of arguments");
      return func::invoke<result_type>(pmf, typename __::tmap<U&&>::type(forward<U>(obj)));
    }
#if 1
    template<typename U>
    result_type operator()(U&& obj, typename __::arg_t<0, Args>::type a1) const
    {
      static_assert(tuple_size<Args>::value == 1, "wrong count of arguments");
      return __::func::invoke<result_type>(pmf, typename __::tmap<U&&, typename __::arg_t<0, Args>::type>::type(forward<U>(obj), a1));
    }
#else
    TTL_REPEAT_NEST(TTL_MAX_TEMPLATE_PARAMS,NTL_DEFINE_MEMFNOP,NTL_DEFINE_MEMFNOP,)
#endif
  };

#if 0 && !defined(NTL_DOC)

  // ARGC == 0
#define NTL_DEFINE_MEMFN0(cv) \
  template<typename R, class T> \
  mem_fn_t<cv R, cv T, R(T::*)() cv> mem_fn( R (T::* pm)()cv ) __ntl_nothrow \
  { \
    return mem_fn_t<cv R, cv T, R(T::*)() cv>(pm); \
  }

  // ARGC == n
#define NTL_DEFINE_MEMFN(cv, n) \
  template<typename R, class T, NTL_SPP_ARGS(1, n, typename A)> \
  mem_fn_t<R, cv T, R(T::*)(NTL_SPP_ARGS(1,n,A)) cv, typename __::tmap<NTL_SPP_ARGS(1,n,A)>::type> mem_fn( R (T::* pm)(NTL_SPP_ARGS(1,n,A)) cv ) __ntl_nothrow \
  { \
    return mem_fn_t<R, cv T, R(T::*)(NTL_SPP_ARGS(1,n,A)) cv, typename __::tmap<NTL_SPP_ARGS(1,n,A)>::type> (pm); \
  }

  NTL_DEFINE_MEMFN0(NTL__EMPTY);
  NTL_DEFINE_MEMFN0(const);
  NTL_DEFINE_MEMFN0(volatile);
  NTL_DEFINE_MEMFN0(const volatile);

  NTL_DEFINE_MEMFN(,1);
  NTL_DEFINE_MEMFN(const,1);
  NTL_DEFINE_MEMFN(volatile,1);
  NTL_DEFINE_MEMFN(const volatile,1);

  NTL_DEFINE_MEMFN(,2);
  NTL_DEFINE_MEMFN(const,2);
  NTL_DEFINE_MEMFN(volatile,2);
  NTL_DEFINE_MEMFN(const volatile,2);

  NTL_DEFINE_MEMFN(,3);
  NTL_DEFINE_MEMFN(const,3);
  NTL_DEFINE_MEMFN(volatile,3);
  NTL_DEFINE_MEMFN(const volatile,3);

  NTL_DEFINE_MEMFN(,4);
  NTL_DEFINE_MEMFN(const,4);
  NTL_DEFINE_MEMFN(volatile,4);
  NTL_DEFINE_MEMFN(const volatile,4);

  NTL_DEFINE_MEMFN(,5);
  NTL_DEFINE_MEMFN(const,5);
  NTL_DEFINE_MEMFN(volatile,5);
  NTL_DEFINE_MEMFN(const volatile,5);

#undef NTL_DEFINE_MEMFN
#undef NTL_DEFINE_MEMFN0
#undef NTL_DEFINE_MEMFNOP
#undef NTL_DEFINE_MEMFNARG
#undef NTL_DEFINE_MEMFNARGT
#else
  template<typename R, class T>
  mem_fn_t<R, T, R(T::*)()> mem_fn( R (T::* pm)() ) __ntl_nothrow
  {
    return mem_fn_t<R, T, R(T::*)()>(pm);
  }

  template<typename R, class T>
  mem_fn_t<R, const T, R(T::*)()const> mem_fn( R (T::* pm)() const) __ntl_nothrow;
  template<typename R, class T>
  mem_fn_t<R, volatile T, R(T::*)()volatile> mem_fn( R (T::* pm)() volatile) __ntl_nothrow;
  template<typename R, class T>
  mem_fn_t<R, const volatile T, R(T::*)()const volatile> mem_fn( R (T::* pm)() const volatile) __ntl_nothrow;

  template<typename R, class T, class A1>
  mem_fn_t<R, T, R(T::*)(A1), typename __::tmap<A1>::type> mem_fn( R (T::* pm)(A1) ) __ntl_nothrow
  {
    return mem_fn_t<R, T, R(T::*)(A1), typename __::tmap<A1>::type>(pm);
  }

#endif

  /**@} lib_member_function_adaptors */
  /**@} lib_function_objects */
  /**@} lib_utilities */
}

#endif // NTL_MEMFN_RV
