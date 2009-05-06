/**\file*********************************************************************
 *                                                                     \brief
 *  20.6 Function objects [function.objects]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_FNCALLER
#define NTL__STLX_FNCALLER

#include "type_traits.hxx"
#include "tuple.hxx"
#include "result_of.hxx"

namespace std
{
  namespace func
  {
    using ttl::meta::int2type;

    template<bool Cond> struct bool_type: integral_constant<bool, Cond>{};

    template<class F, class Args, typename R>
    struct fn_caller
    {
      enum { fn_unk, fn_memfun, fn_memptr };

      template<class F> struct result_of_memptr: false_type{};
      template<typename R, class T> 
      struct result_of_memptr<R T::*>:
        true_type
      {
        typedef R result_type;
        typedef T object_type;
      };

      template<class F, class Args, bool IsMember>  struct argcheck: false_type{};
      template<class F, class Args>
      struct argcheck<F,Args,true>
      {
        typedef typename tuple_element<0,Args>::type A;

        typedef typename conditional<is_member_object_pointer<F>::value, result_of_memptr<F>, __::result_of_function_type<F> >::type::object_type T;

        template<class U>
        static __::sfinae_passed_tag check_ptr(const U&);
        template<class U>
        static __::sfinae_failed_tag check_ptr(...);

        static const bool is_conv = is_same<T,A>::value || is_convertible<A,T>::value;

        static const bool value = is_conv;
      };

      static R call(F f, const Args& args)
      {
        // (t1.*f)()  if f is memfun and t1 is object or reference or reference to derived (1)
        // ((*t1).*f) if f is memfun and t1 is nothing above (2)
        // t1.*f      if f is memptr and t1 is 1 (3)
        // (*t1).*f   if f is memptr and t1 is nothing above (4)
        // f(t1)      if nothing above

        static const bool is_memfun = is_member_function_pointer<F>::value;
        static const bool is_memptr = !is_memfun && is_member_object_pointer<F>::value;

        typedef argcheck<F,Args,(tuple_size<Args>::value > 0 && (is_memfun|is_memptr))> check;
        static const bool is_ref = check::value;

        //static const bool is_ptr = sizeof(check::check_ptr<check::T>(*get<0>(args))) == sizeof(__::sfinae_passed_tag);

        return callf(f, const_cast<Args&>(args), int2type<is_memfun|is_memptr*2>(), bool_type<is_ref>());
      }
    private:
      static R callf(F f, Args& args, int2type<fn_memptr>, true_type)
      {
        return get<0>(args).*f;
      }
      static R callf(F f, Args& args, int2type<fn_memptr>, false_type)
      {
        return (*get<0>(args)).*f;
      }
      static R callf(F f, Args& args, int2type<fn_memfun>, true_type)
      {
        return call_memfun<__::result_of_function_type<F>::object_type>(f, get<0>(args), args, int2type<tuple_size<Args>::value>());
      }
      static R callf(F f, Args& args, int2type<fn_memfun>, false_type)
      {
        return call_memfun<__::result_of_function_type<F>::object_type>(f, *get<0>(args), args, int2type<tuple_size<Args>::value>());
      }

      template<class Obj>
      static R call_memfun(F f, Obj& obj, Args&     , int2type<1>) { return (obj.*f)(); }
      template<class Obj>
      static R call_memfun(F f, Obj& obj, Args& args, int2type<2>) { return (obj.*f)(get<1>(args)); }
    private:
      template<bool IsRef>
      static R callf(F f, Args& args, int2type<fn_unk>, bool_type<IsRef>)
      {
        return call_fun(f, args, int2type<tuple_size<Args>::value>());
      }

      static R call_fun(F f, Args&     , int2type<0>) { return f(); }
      static R call_fun(F f, Args& args, int2type<1>) { return f(get<0>(args)); }
      static R call_fun(F f, Args& args, int2type<2>) { return f(get<0>(args), get<1>(args)); }
      static R call_fun(F f, Args& args, int2type<3>) { return f(get<0>(args), get<1>(args), get<2>(args)); }
    };


    template<typename R, typename F, class Args>
    inline R invoke(F f, const Args& args)
    {
      return fn_caller<F,Args,R>::call(f,args);
    }
  }
}

#endif // NTL__STLX_FNCALLER
