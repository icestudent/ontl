/**\file*********************************************************************
 *                                                                     \brief
 *  20.6 Function objects [function.objects]
 *  Implementation of polymorphic function wrappers for legacy compilers
 ****************************************************************************
 */
#ifndef NTL__STLX_FUNCTION
#define NTL__STLX_FUNCTION

#include "exception.hxx"
#include "type_traits.hxx"
#include "tuple.hxx"
#include "mem_fn.hxx"

namespace std
{
 /**\addtogroup  lib_utilities ************ 20 General utilities library [utilities]
  *@{
  **/
 /**\addtogroup  lib_function_objects ***** 20.6 Function objects [function.objects]
  *@{
  **/
 /**\defgroup lib_func_wrap *************** 20.6.15 Polymorphic function wrappers [func.wrap]
  *@{
  **/

  class bad_function_call:
    public exception
  {
  public:
    bad_function_call() __ntl_nothrow
    {}
    const char* what() const __ntl_nothrow { return "bad_function_call"; }
  };

  /// Polymorphic function wrappers
  namespace func
  {
    /// function<> implementation
    /// \version 1
    /// \internal
    namespace v1 
    {
      //template<class> struct function;

      namespace impl
      {
        using ttl::meta::empty_type;

        /************************************************************************/
        /* Base Caller                                                          */
        /************************************************************************/
        template<class R, class Args, size_t Argc = tuple_size<Args>::value>
        struct caller;

        template<class R, class Args>
        struct caller<R,Args,0>
        {
          virtual R operator()() const = 0;
          virtual caller* clone() const = 0;
          virtual ~caller(){}
          virtual const type_info& target_type() const = 0;
          virtual void* target() = 0;
        };

        template<class R, class Args>
        struct caller<R,Args,1>
        {
          virtual R operator()(typename __::arg_t<0, Args>::type a1) const = 0;
          virtual caller* clone() const = 0;
          virtual ~caller(){}
          virtual const type_info& target_type() const = 0;
          virtual void* target() = 0;
        };

        template<class R, class Args>
        struct caller<R,Args,2>
        {
          virtual R operator()(typename __::arg_t<0, Args>::type a1, typename __::arg_t<1, Args>::type a2) const = 0;
          virtual caller* clone() const = 0;
          virtual ~caller(){}
          virtual const type_info& target_type() const = 0;
          virtual void* target() = 0;
        };

        /************************************************************************/
        /* Functor caller                                                       */
        /************************************************************************/
        template<class F, class R, class Args, size_t Argc = tuple_size<Args>::value>
        struct functor_caller;

        template<class F, class R, class Args>
        struct functor_caller<F,R,Args,0>:
          caller<R,Args>
        {
          explicit functor_caller(F f)
            :f(f)
          {}
          virtual caller* clone() const { return new functor_caller(f); }
          virtual R operator()() const
          {
            return f();
          }
          const type_info& target_type() const { return typeid(F); }
          virtual void* target() { return reinterpret_cast<void*>(&f); }
        private:
          F f;
        };

        template<class F, class R, class Args>
        struct functor_caller<F,R,Args,1>
          :caller<R,Args>
        {
          explicit functor_caller(F f)
            :f(f)
          {}
          virtual caller* clone() const { return new functor_caller(f); }
          virtual R operator()(typename __::arg_t<0, Args>::type a1) const
          {
            return f(a1);
          }
          const type_info& target_type() const { return typeid(F); }
          virtual void* target() { return reinterpret_cast<void*>(&f); }
        private:
          F f;
        };

        /************************************************************************/
        /* Member function caller                                               */
        /************************************************************************/
        template<class MemFn, class R, class Args, size_t Argc = tuple_size<Args>::value>
        struct memfun_caller;

        template<class MemFn, class R, class Args>
        struct memfun_caller<MemFn, R, Args, 1>
          :caller<R,Args>
        {
          explicit memfun_caller(MemFn f)
            :pmf(f)
          {}
          virtual caller* clone() const { return new memfun_caller(pmf); }

          virtual R operator()(typename __::arg_t<0, Args>::type obj) const
          {
            typedef typename __::arg_t<0, Args>::type obj_type;
            return call(obj, std::is_pointer<obj_type>());
          }
          const type_info& target_type() const { return typeid(MemFn); }
          virtual void* target() { return reinterpret_cast<void*>(&pmf); }
        protected:
          template<class T>
          R call(T& obj, false_type) const { return (obj.*pmf)(); }
          template<class T>
          R call(T  obj, true_type ) const { return (obj->*pmf)();}
        private:
          MemFn pmf;
        };

        /************************************************************************/
        /* Refwrap caller                                                       */
        /************************************************************************/
        template<class F, class R, class Args, size_t Argc = tuple_size<Args>::value>
        struct refwrap_caller;

        template<class F, class R, class Args>
        struct refwrap_caller<F,R,Args,0>:
          caller<R,Args>
        {
          explicit refwrap_caller(reference_wrapper<F> f)
            :f(f)
          {}
          virtual caller* clone() const { return new refwrap_caller(f); }
          virtual R operator()() const
          {
            return f();
          }
          const type_info& target_type() const { return typeid(F); }
          virtual void* target() { return reinterpret_cast<void*>(& f.get() ); }
        private:
          reference_wrapper<F> f;
        };

        template<class F, class R, class Args>
        struct refwrap_caller<F,R,Args,1>
          :caller<R,Args>
        {
          explicit refwrap_caller(reference_wrapper<F> f)
            :f(f)
          {}
          virtual caller* clone() const { return new refwrap_caller(f); }
          virtual R operator()(typename __::arg_t<0, Args>::type a1) const
          {
            return f(a1);
          }
          const type_info& target_type() const { return typeid(F); }
          virtual void* target() { return reinterpret_cast<void*>(& f.get() ); }
        private:
          reference_wrapper<F> f;
        };

        //////////////////////////////////////////////////////////////////////////
        template<typename R, class Args, size_t Argc = tuple_size<Args>::value>
        struct fun_arity { typedef R result_type; };

        template<typename R, class Args>
        struct fun_arity<R,Args,1>: unary_function<typename tuple_element<0, Args>::type, R>{};

        template<typename R, class Args>
        struct fun_arity<R,Args,2>: binary_function<typename tuple_element<0, Args>::type, typename tuple_element<1, Args>::type, R>{};
      } // namespace impl



      /************************************************************************/
      /* Function implementation                                              */
      /************************************************************************/
      template<typename R, class Args = tuple<> >
      struct function:
        impl::fun_arity<R, Args>
      {
      protected:
        struct explicit_bool { int _; };
        typedef int explicit_bool::*  explicit_bool_type;
        typedef int                   nullptr_t;

      public:
        enum { arity = tuple_size<Args>::value };

        // allocator-aware:
        //template<class A> function(allocator_arg_t, const A&);
        //template<class A> function(allocator_arg_t, const A&, unspecified-null-pointer-type );
        //template<class A> function(allocator_arg_t, const A&, const function&);
        //template<class A> function(allocator_arg_t, const A&, function&&);
        //template<class F, class A> function(allocator_arg_t, const A&, F);
        //template<class F, class A> function(allocator_arg_t, const A&, F&&);

        explicit function() __ntl_nothrow
          :caller()
        {}

        function(const function& r)
        {
          caller = r.caller ? r.caller->clone() : 0;
        }

        ~function()
        {
          clear();
        }

        template<typename Fn>
        explicit function(Fn f, typename enable_if<!is_member_function_pointer<Fn>::value>::type* = 0)
        {
          caller = new impl::functor_caller<Fn, result_type, Args>(f);
        }

        template<typename MemFn>
        explicit function(MemFn f, typename enable_if<is_member_function_pointer<MemFn>::value>::type* = 0)
        {
          // NOTE: targets a copy of mem_fn(f) if f is a pointer to member function
          // hz, what this means
          caller = new impl::memfun_caller<MemFn, R, Args>(f);
        }

        template<typename F>
        explicit function(reference_wrapper<F> rf)
        {
          caller = new impl::refwrap_caller<F, R, Args>(rf);
        }

        function& operator=(const function& r)
        {
          clear();
          if(r.caller)
            caller = r.caller->clone();
          return *this;
        }

#ifdef NTL__CXX_RV
        function(function&&);
        function& operator=(function&&);
        template<class F> function(F&&);
        template<class F> function& operator=(F&&);
#endif

        template<class F> function& operator=(F f) 
        {
          function(f).swap(*this);
          return *this;
        }

        template<class F> function& operator=(reference_wrapper<F> rf)
        {
          function(rf).swap(*this);
          return *this;
        }

        // 20.6.15.2.4, function invocation:
        result_type operator()() const { return (*caller)(); }
        result_type operator()(typename __::arg_t<0, Args>::type a1) const { return (*caller)(a1); }
        result_type operator()(typename __::arg_t<0, Args>::type a1, typename __::arg_t<1, Args>::type a2) const { return (*caller)(a1,a2); }


        // 20.6.15.2.3 function capacity
        operator explicit_bool_type() const { return caller ? &explicit_bool::_ : 0; }


        // 20.6.15.2.2, function modifiers:
        void swap(function& r)
        {
          std::swap(caller, r.caller);
        }

        template<class F, class A>
        void assign(F f, const A&)
        {
          function(f).swap(*this);
        }


#if STLX__USE_RTTI
        // 20.6.15.2.5, function target access:
        const std::type_info& target_type() const
        {
          return caller ? caller->target_type() : typeid(void);
        }
        template <typename T> T* target()
        {
          return caller && typeid(T) == target_type() ? reinterpret_cast<T*>(caller->target()) : nullptr;
        }
        template <typename T> const T* target() const
        {
          return caller && typeid(T) == target_type() ? reinterpret_cast<T*>(caller->target()) : nullptr;
        }
#endif
      protected:
        inline void clear()
        {
          delete caller;
          caller = nullptr;
        }
      private:
        impl::caller<result_type, Args>* caller;
      };
    } // namespace v1

    /************************************************************************/
    /* Function interface wrapper                                           */
    /************************************************************************/
    template<class> class function;

    template<class R>
    class function<R()>: 
      public v1::function<R>
    {
      typedef v1::function<R> base;
    public:
      template<typename F>
      explicit function(F f)
        :base(f)
      {}
      explicit function(){}
      function(nullptr_t){}
      function(function& r)
        :base(static_cast<base&>(r)){}
      function& operator=(function& r) { base::operator=(static_cast<base&>(r)); return *this; }
      function& operator=(nullptr_t) { clear(); return *this; }
      template<class F> function& operator=(F f) { base::operator=(f); return *this; }
    };

    template<class R, class A1>
    class function<R(A1)>:
      public v1::function<R, tuple<A1> >
    {
      typedef v1::function<R, tuple<A1> > base;
    public:
      template<typename F>
      explicit function(F f)
        :base(f)
      {}
      explicit function(){}
      function(nullptr_t){}
      function(function& r)
        :base(static_cast<base&>(r)){}
      function& operator=(function& r) { base::operator=(static_cast<base&>(r)); return *this; }
      function& operator=(nullptr_t) { clear(); return *this; }
      template<class F> function& operator=(F f) { base::operator=(f); return *this; }
    };

    template<class R, class A1, class A2>
    class function<R(A1, A2)>: 
      public v1::function<R, tuple<A1,A2> >
    {
      typedef v1::function<R, tuple<A1, A2> > base;
    public:
      template<typename F>
      explicit function(F f)
        :base(f)
      {}
      explicit function(){}
      function(nullptr_t){}
      function(function& r)
        :base(static_cast<base&>(r)){}
      function& operator=(function& r) { base::operator=(static_cast<base&>(r)); return *this; }
      function& operator=(nullptr_t) { clear(); return *this; }
      template<class F> function& operator=(F f) { base::operator=(f); return *this; }
    };

  } // namespace func

  using namespace func;

  /**@} lib_func_wrap        */
  /**@} lib_function_objects */
  /**@} lib_utilities        */

} // namespace std

#endif // NTL__STLX_FUNCTION
