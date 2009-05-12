/**\file*********************************************************************
 *                                                                     \brief
 *  20.6 Function objects [function.objects]
 *
 ****************************************************************************
 */
#ifndef NTL_MEMFN_HXX
#define NTL_MEMFN_HXX

#ifdef NTL__CXX_RV
# include "mem_fn_rv.hxx"
#else 

#include "fn_caller.hxx"

namespace std
{
 /**\addtogroup  lib_utilities ************ 20 General utilities library [utilities]
  *@{*/
 /**\addtogroup  lib_function_objects ***** 20.6 Function objects [function.objects]
  *@{*/
 /**\defgroup  lib_member_function_adaptors 20.6.14 Member function adaptors [func.memfn]
  *@{*/


  template<typename R, class T>
  class mem_pm_t:
    public unary_function<T*, R>
  {
    R T::*pm;
  public:
    explicit mem_pm_t(R T::*pm)
      :pm(pm)
    {}

    result_type& operator()(T& obj) const
    {
      return obj.*pm;
    }
    const result_type& operator()(const T& obj) const
    {
      return obj.*pm;
    }
    result_type& operator()(T* obj) const
    {
      return obj->*pm;
    }
    const result_type& operator()(const T* obj) const
    {
      return obj->*pm;
    }
    template<class U>
    result_type& operator()(U& obj) const
    {
      return call(obj,&obj);
    }
    template<class U>
    const result_type& operator()(const U& obj) const
    {
      return call(obj,&obj);
    }
  private:
    template<class U>
    result_type& call(U& obj, const volatile T*) const { return obj.*pm; }
    template<class U>
    result_type& call(U& obj, const volatile void*) const { return (*obj).*pm; }
    template<class U>
    const result_type& call(const U& obj, const volatile T*) const { return obj.*pm; }
    template<class U>
    const result_type& call(const U& obj, const volatile void*) const { return (*obj).*pm; }
  };

  template<typename R, class T>
  mem_pm_t<R,T> mem_fn(R T::*pm)
  {
    return mem_pm_t<R,T> (pm);
  }

#ifndef NTL__CXX_VT

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

  template<typename R, class T, class Functor, class Args = tuple<> >
  class mem_fn_t:
    public __::memfun_arity<R, T, Args>
  {
    Functor pmf;
  public:
    explicit mem_fn_t(Functor f)
      :pmf(f)
    {}

#if 0
    result_type operator()(T& obj) const
    {
      static_assert(tuple_size<Args>::value == 0, "wrong count of arguments");
      return (obj.*pmf)();
    }

    result_type operator()(T* obj) const
    {
      static_assert(tuple_size<Args>::value == 0, "wrong count of arguments");
      return (obj->*pmf)();
    }

    result_type operator()(T& obj, typename __::arg_t<0, Args>::type a1) const
    {
      static_assert(tuple_size<Args>::value == 1, "wrong count of arguments");
      return (obj.*pmf)(a1);
    }

    result_type operator()(T& obj, typename __::arg_t<0, Args>::type a1, typename __::arg_t<1, Args>::type a2) const
    {
      static_assert(tuple_size<Args>::value == 2, "wrong count of arguments");
      return (obj.*pmf)(a1,a2);
    }

    result_type operator()(T& obj, typename __::arg_t<0, Args>::type a1, typename __::arg_t<1, Args>::type a2, typename __::arg_t<2, Args>::type a3) const
    {
      static_assert(tuple_size<Args>::value == 3, "wrong count of arguments");
      return (obj.*pmf)(a1,a2,a3);
    }

    result_type operator()(T& obj, typename __::arg_t<0, Args>::type a1, typename __::arg_t<1, Args>::type a2, typename __::arg_t<2, Args>::type a3, typename __::arg_t<3, Args>::type a4) const
    {
      static_assert(tuple_size<Args>::value == 4, "wrong count of arguments");
      return (obj.*pmf)(a1,a2,a3,a4);
    }

    result_type operator()(T& obj, typename __::arg_t<0, Args>::type a1, typename __::arg_t<1, Args>::type a2, typename __::arg_t<2, Args>::type a3, typename __::arg_t<3, Args>::type a4, typename __::arg_t<4, Args>::type a5) const
    {
      static_assert(tuple_size<Args>::value == 5, "wrong count of arguments");
      return (obj.*pmf)(a1,a2,a3,a4,a5);
    }

    result_type operator()(T* obj, typename __::arg_t<0, Args>::type a1) const
    {
      static_assert(tuple_size<Args>::value == 1, "wrong count of arguments");
      return (obj->*pmf)(a1);
    }

    result_type operator()(T* obj, typename __::arg_t<0, Args>::type a1, typename __::arg_t<1, Args>::type a2) const
    {
      static_assert(tuple_size<Args>::value == 2, "wrong count of arguments");
      return (obj->*pmf)(a1,a2);
    }

    result_type operator()(T* obj, typename __::arg_t<0, Args>::type a1, typename __::arg_t<1, Args>::type a2, typename __::arg_t<2, Args>::type a3) const
    {
      static_assert(tuple_size<Args>::value == 3, "wrong count of arguments");
      return (obj->*pmf)(a1,a2,a3);
    }

    result_type operator()(T* obj, typename __::arg_t<0, Args>::type a1, typename __::arg_t<1, Args>::type a2, typename __::arg_t<2, Args>::type a3, typename __::arg_t<3, Args>::type a4) const
    {
      static_assert(tuple_size<Args>::value == 4, "wrong count of arguments");
      return (obj->*pmf)(a1,a2,a3,a4);
    }

    result_type operator()(T* obj, typename __::arg_t<0, Args>::type a1, typename __::arg_t<1, Args>::type a2, typename __::arg_t<2, Args>::type a3, typename __::arg_t<3, Args>::type a4, typename __::arg_t<4, Args>::type a5) const
    {
      static_assert(tuple_size<Args>::value == 5, "wrong count of arguments");
      return (obj->*pmf)(a1,a2,a3,a4,a5);
    }

#else
    template<typename U>
    result_type operator()(U& obj) const
    {
      static_assert(tuple_size<Args>::value == 0, "wrong count of arguments");
      return __::func::invoke<result_type>(pmf, FUNARGS(U&)(obj));
    }
    template<typename U>
    result_type operator()(const U& obj) const
    {
      static_assert(tuple_size<Args>::value == 0, "wrong count of arguments");
      return __::func::invoke<result_type>(pmf, FUNARGS(const U&)(obj));
    }

    template<typename U>
    result_type operator()(U& obj, typename __::arg_t<0, Args>::type a1) const
    {
      static_assert(tuple_size<Args>::value == 1, "wrong count of arguments");
      return __::func::invoke<result_type>(pmf, FUNARGS(U&, typename __::arg_t<0, Args>::type)(obj, a1));
    }
    template<typename U>
    result_type operator()(const U& obj, typename __::arg_t<0, Args>::type a1) const
    {
      static_assert(tuple_size<Args>::value == 1, "wrong count of arguments");
      return __::func::invoke<result_type>(pmf, FUNARGS(const U&, typename __::arg_t<0, Args>::type)(obj, a1));
    }

    template<typename U>
    result_type operator()(U& obj, typename __::arg_t<0, Args>::type a1, typename __::arg_t<1, Args>::type a2) const
    {
      static_assert(tuple_size<Args>::value == 2, "wrong count of arguments");
      return __::func::invoke<result_type>(pmf, FUNARGS(U&, typename __::arg_t<0, Args>::type, typename __::arg_t<1, Args>::type)(obj, a1, a2));
    }
    template<typename U>
    result_type operator()(const U& obj, typename __::arg_t<0, Args>::type a1, typename __::arg_t<1, Args>::type a2) const
    {
      static_assert(tuple_size<Args>::value == 2, "wrong count of arguments");
      return __::func::invoke<result_type>(pmf, FUNARGS(const U&, typename __::arg_t<0, Args>::type, typename __::arg_t<1, Args>::type)(obj, a1, a2));
    }
#endif
  };

#if 1 && !defined __DOXYGEN__

  // ARGC == 0
#define NTL_DEFINE_MEMFN0(cv) \
  template<typename R, class T> \
  mem_fn_t<R, cv T, R(T::*)() cv> mem_fn( R (T::* pm)()cv ) __ntl_nothrow \
  { \
    return mem_fn_t<R, cv T, R(T::*)() cv>(pm); \
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
  mem_fn_t<R, T, R(T::*)(A1), FUNARGS(A1)> mem_fn( R (T::* pm)(A1) ) __ntl_nothrow
  {
    return mem_fn_t<R, T, R(T::*)(A1), FUNARGS(A1)>(pm);
  }

#endif
#endif // NTL__CXX_VT

  /**@} lib_member_function_adaptors */
  /**@} lib_function_objects */
  /**@} lib_utilities */
}
#endif // RV
#endif // NTL_MEMFN_HXX
