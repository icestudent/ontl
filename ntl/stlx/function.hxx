/**\file*********************************************************************
 *                                                                     \brief
 *  20.7 Function objects [function.objects]
 *  Implementation of polymorphic function wrappers for legacy compilers
 ****************************************************************************
 */
#ifndef NTL__STLX_FUNCTION
#define NTL__STLX_FUNCTION
#pragma once

#include "stdexception.hxx"


#ifdef NTL_CXX_VT
# include "function_vt.hxx"
#else

#include "mem_fn.hxx"
#ifndef NTL__STLX_FNCALLER
#include "fn_caller.hxx"
#endif

namespace std
{
 /**\addtogroup  lib_utilities ************ 20 General utilities library [utilities]
  *@{*/
 /**\addtogroup  lib_function_objects ***** 20.7 Function objects [function.objects]
  *@{*/
 /**\defgroup lib_func_wrap *************** 20.7.15 Polymorphic function wrappers [func.wrap]
  *@{*/

  /**
   *	An exception of type \c bad_function_call is thrown by \c function::operator() (20.7.15.2.4) when the function wrapper object has no target.
   **/
  class bad_function_call:
    public exception
  {
  public:
    bad_function_call() __ntl_nothrow
    {}
    const char* what() const __ntl_nothrow { return "bad_function_call"; }
  };

  namespace __ { namespace func {

    /// function<> implementation
    /// \version 1
    /// \internal
    namespace v1 
    {
      //template<class> struct function;

      namespace impl
      {
        using ttl::meta::empty_type;

        template<typename R, class Args, size_t Argc = tuple_size<Args>::value>
        struct fun_arity { typedef R result_type; };

        template<typename R, class Args>
        struct fun_arity<R,Args,1>: unary_function<typename tuple_element<0, Args>::type, R>{};

        template<typename R, class Args>
        struct fun_arity<R,Args,2>: binary_function<typename tuple_element<0, Args>::type, typename tuple_element<1, Args>::type, R>{};

        //////////////////////////////////////////////////////////////////////////
        /************************************************************************/
        /* Base Caller                                                          */
        /************************************************************************/
        template<typename R, class Args>
        struct caller
        {
          virtual R operator()(const Args&) const = 0;
          virtual caller* clone() const = 0;
          virtual ~caller(){}
          virtual const type_info& target_type() const = 0;
          virtual void* target() = 0;
        };

        /************************************************************************/
        /* Caller                                                               */
        /************************************************************************/
        template<typename R, class F, class Args>
        struct fun_caller:
          caller<R,Args>,
          noncopyable
        {
          explicit fun_caller(const F& f)
            :f(f)
          {}

          R operator()(const Args& args) const
          {
            return fn_caller<F,Args,R>::call(f, args);
          }
          caller<R,Args>* clone() const
          {
            return new fun_caller(f);
          }
          const type_info& target_type() const { return target_type<false>(__::is_refwrap<F>()); }
          void* target() { return target<false>(__::is_refwrap<F>()); }
        protected:
          template<bool> const type_info& target_type(false_type) const { return __ntl_typeid(f); }
          template<bool> const type_info& target_type(true_type)  const { return __ntl_typeid(f.get()); }
          template<bool> void* target(false_type) { return reinterpret_cast<void*>(&f); }
          template<bool> void* target(true_type)  { return reinterpret_cast<void*>(&f.get()); }
        private:
          F f;
        };
      }


      /************************************************************************/
      /* Function implementation                                              */
      /************************************************************************/

      /**
       *	function<> implementation
       **/
      template<typename R, class Args = tuple<> >
      struct function:
        impl::fun_arity<R, Args>
      {
      protected:
        typedef int nullptr_t;
      public:
        enum { 
          /** arguments count */
          arity = tuple_size<Args>::value
        };

        using impl::fun_arity<R, Args>::result_type;

        ///\name 20.7.15.2.1, construct/copy/destroy:
        // allocator-aware:
        //template<class A> function(allocator_arg_t, const A&);
        //template<class A> function(allocator_arg_t, const A&, unspecified-null-pointer-type );
        //template<class A> function(allocator_arg_t, const A&, const function&);
        //template<class A> function(allocator_arg_t, const A&, function&&);
        //template<class F, class A> function(allocator_arg_t, const A&, F);
        //template<class F, class A> function(allocator_arg_t, const A&, F&&);

        /** default ctor */
        explicit function() __ntl_nothrow
          :caller()
        {}

        /** Creates copy of \c r target */
        function(const function& r)
        {
          caller = r.caller ? r.caller->clone() : 0;
        }

        /** dtor */
        ~function()
        {
          clear();
        }

        /** Constructs function wrapper from reference to callable object */
        template<typename F>
        explicit function(reference_wrapper<F> rf)
        {
          caller = new impl::fun_caller<R, reference_wrapper<F>, Args>(rf);
        }

        /** Copies \c r target */
        function& operator=(const function& r)
        {
          clear();
          if(r.caller)
            caller = r.caller->clone();
          return *this;
        }

    #ifdef NTL_CXX_RV
        ///** Constructs function wrapper from copy of callable \c f */
        //template<typename F>
        //explicit function(const F& f)
        //  :caller()
        //{
        //  assign_impl(f, is_member_function_pointer<F>());
        //}

        /** Constructs function wrapper from callable \c f */
        template<typename F>
        explicit function(F&& f)
          :caller()
        {
          assign_impl<F>(forward<F>(f));
        }

        /** Moves \c f to this wrapper */
        template<class F> function& operator=(F&& f)
        {
          function(forward<F>(f)).swap(*this);
          return *this;
        }

        /** Constructs function wrapper from target of \c r */
        function(function&& r)
          :caller()
        {
          if(r.caller)
            std::swap(caller, r.caller);
        }

        /** Replaces the target of this wrapper with the target of \c r */
        function& operator=(function&& r)
        {
          clear();
          if(r.caller)
            std::swap(caller, r.caller);
        }

    #else
        /** Constructs function wrapper from copy of callable \c f */
        template<typename F>
        explicit function(const F& f)
          :caller()
        {
          assign_impl<F>(f);
        }
        template<typename F>
        explicit function(_rvalue<F> f)
          :caller()
        {
          assign_impl<F>(f);
        }

        ///** Constructs function wrapper from copy of callable \c f */
        //template<typename F>
        //explicit function(F& f)
        //  :caller()
        //{
        //  assign_impl(f, is_member_function_pointer<F>());
        //}

        /** Copies \c f to this wrapper */
        template<class F> function& operator=(const F& f) 
        {
          clear();
          assign_impl<F>(f);
          return *this;
        }
        template<class F> function& operator=(_rvalue<F> f) 
        {
          clear();
          assign_impl<F>(f);
          return *this;
        }

        ///** Copies \c f to this wrapper */
        //template<class F> function& operator=(F& f)
        //{
        //  clear();
        //  assign_impl(f, is_member_function_pointer<F>());
        //  return *this;
        //}
    #endif

        /** Takes referenced callable to this wrapper */
        template<class F> function& operator=(reference_wrapper<F> rf) __ntl_nothrow
        {
          function(rf).swap(*this);
          return *this;
        }

        ///\name 20.7.15.2.4, function invocation:
        result_type operator()(const Args& args) const __ntl_throws(bad_function_call)
        {
          if(!caller) __ntl_throw(bad_function_call());
          return (*caller)(args);
        }

        result_type operator()() const __ntl_throws(bad_function_call)
        { if(!caller) __ntl_throw(bad_function_call()); return (*caller)(Args()); }
        result_type operator()(typename __::arg_t<0, Args>::type a1) const __ntl_throws(bad_function_call)
        { if(!caller) __ntl_throw(bad_function_call()); return (*caller)(Args(a1)); }
        result_type operator()(typename __::arg_t<0, Args>::type a1, typename __::arg_t<1, Args>::type a2) const __ntl_throws(bad_function_call)
        { if(!caller) __ntl_throw(bad_function_call()); return (*caller)(Args(a1,a2)); }
        result_type operator()(typename __::arg_t<0, Args>::type a1, typename __::arg_t<1, Args>::type a2, typename __::arg_t<2, Args>::type a3) const __ntl_throws(bad_function_call)
        { if(!caller) __ntl_throw(bad_function_call()); return (*caller)(Args(a1,a2,a3)); }


        ///\name 20.7.15.2.3 function capacity

        /** Returns true if this has target */
        __explicit_operator_bool() const __ntl_nothrow { return __explicit_bool(caller); }


        ///\name 20.7.15.2.2, function modifiers:

        /** Swaps this target with the target of \c r */
        void swap(function&  r) __ntl_nothrow
        {
          std::swap(caller, r.caller);
        }

        /** Assigns this object with callable \c f */
        template<class F, class A>
        void assign(F f, const A& a)
        {
          function(allocator_arg, f, a).swap(*this);
        }

    #if STLX_USE_RTTI
        ///\name 20.7.15.2.5, function target access:

        /** Returns type info of the target if exists; otherwise returns <tt>typeid(void)</tt> */
        const std::type_info& target_type() const __ntl_nothrow
        {
          return caller ? caller->target_type() : typeid(void);
        }

        /** Returns pointer to target if T is type of the target or null pointer otherwise */
        template <typename T> T* target() __ntl_nothrow
        {
          return caller && typeid(T) == target_type() ? reinterpret_cast<T*>(caller->target()) : nullptr;
        }

        /** Returns pointer to constant target if T is type of the target or null pointer otherwise */
        template <typename T> const T* target() const __ntl_nothrow
        {
          return caller && typeid(T) == target_type() ? reinterpret_cast<const T*>(caller->target()) : nullptr;
        }
    #endif
        ///\}
      protected:
        ///\cond __
        inline void clear()
        {
          if(caller){
            delete caller;
            caller = nullptr;
          }
        }
        ///\endcond

    #ifndef NTL_CXX_RV
        template<class Fn> inline void assign_impl(const Fn& f)
        {
          if(check_ptr(f, is_pointer<Fn>()))
            caller = new impl::fun_caller<result_type, Fn, Args>(f);
        }
        template<class Fn> inline void assign_impl(_rvalue<Fn> f)
        {
          if(check_ptr(f, is_pointer<Fn>()))
            caller = new impl::fun_caller<result_type, Fn, Args>(f);
        }
        //template<class Fn> inline void assign_impl(Fn& f)
        //{
        //  caller = new impl::functor_caller<Fn, result_type, Args>(f);
        //}
    #else
        template<class Fn> inline void assign_impl(Fn&& f)
        {
          static_assert(!is_reference<Fn>::value, "reference to reference isn't allowed");
          if(check_ptr(f, is_pointer<typename remove_reference<Fn>::type>()))
            caller = new impl::fun_caller<result_type, typename remove_reference<Fn>::type, Args>(forward<Fn>(f));
        }
    #endif

        /** Checks pointer if it is */
        template<class F> inline bool check_ptr(const F& f, true_type){ return f != nullptr; }
        /** Check pointer stub for nonpointer callables */
        template<class F> inline bool check_ptr(const F&,  false_type){ return true; }

      private:
        impl::caller<result_type, Args>* caller;
      };
    } // namespace v1
    namespace detail = v1;
    } // func
  } // __

    /************************************************************************/
    /* Function interface wrapper                                           */
    /************************************************************************/
    template<class> class function;

    /** function<> specialization for 0 arguments. \sa v1::function */
    template<class R>
    class function< R()>: 
      public __::func::detail::function<R>
    {
      typedef __::func::detail::function<R> base;
    public:
      template<typename F>
      explicit function(F f)
        :base(forward<F>(f))
      {}
      explicit function() __ntl_nothrow {}
      function(nullptr_t) __ntl_nothrow {}
      function(const function& r)
        :base(static_cast<const base&>(r)){}
      function& operator=(const function& r) { base::operator=(static_cast<const base&>(r)); return *this; }
      function& operator=(nullptr_t) { clear(); return *this; }
      template<class F> function& operator=(F f) { base::operator=(forward<F>(f)); return *this; }
    };

    /** function<> specialization for 1 argument */
    template<class R, class A1>
    class function< R(A1)>:
      public __::func::detail::function<R, NTL_FUNARGS(A1)>
    {
      typedef __::func::detail::function<R, NTL_FUNARGS(A1)> base;
    public:
      template<typename F>
      explicit function(F f)
        :base(forward<F>(f))
      {}
      explicit function() __ntl_nothrow {}
      function(nullptr_t) __ntl_nothrow {}
      function(const function& r)
        :base(static_cast<const base&>(r)){}
      function& operator=(const function& r) { base::operator=(static_cast<const base&>(r)); return *this; }
      function& operator=(nullptr_t) { clear(); return *this; }
      template<class F> function& operator=(F f) { base::operator=(forward<F>(f)); return *this; }
    };

    /** function<> specialization for 2 arguments */
    template<class R, class A1, class A2>
    class function< R(A1, A2)>: 
      public __::func::detail::function<R, NTL_FUNARGS(A1,A2)>
    {
      typedef __::func::detail::function<R, NTL_FUNARGS(A1,A2)> base;
    public:
      template<typename F>
      explicit function(F f)
        :base(forward<F>(f))
      {}
      explicit function() __ntl_nothrow {}
      function(nullptr_t) __ntl_nothrow {}
      function(const function& r)
        :base(static_cast<const base&>(r)){}
      function& operator=(const function& r) { base::operator=(static_cast<const base&>(r)); return *this; }
      function& operator=(nullptr_t) { clear(); return *this; }
      template<class F> function& operator=(F f) { base::operator=(forward<F>(f)); return *this; }
    };

    /** function<> specialization for 3 arguments */
    template<class R, class A1, class A2, class A3>
    class function< R(A1, A2, A3)>: 
      public __::func::detail::function<R, NTL_FUNARGS(A1,A2,A3)>
    {
      typedef __::func::detail::function<R, NTL_FUNARGS(A1,A2,A3)> base;
    public:
      template<typename F>
      explicit function(F f)
        :base(forward<F>(f))
      {}
      explicit function() __ntl_nothrow {}
      function(nullptr_t) __ntl_nothrow {}
      function(const function& r)
        :base(static_cast<const base&>(r)){}
      function& operator=(const function& r) { base::operator=(static_cast<const base&>(r)); return *this; }
      function& operator=(nullptr_t) { clear(); return *this; }
      template<class F> function& operator=(F f) { base::operator=(forward<F>(f)); return *this; }
    };

  /**@} lib_func_wrap        */
  /**@} lib_function_objects */
  /**@} lib_utilities        */

} // namespace std
#endif // NTL_CXX_VT
#endif // NTL__STLX_FUNCTION
