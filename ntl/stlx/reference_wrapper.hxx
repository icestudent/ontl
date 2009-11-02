/**\file*********************************************************************
 *                                                                     \brief
 *  20.6 Function objects [function.objects]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_REFWRAP
#define NTL__STLX_REFWRAP
#pragma once

#include "fn_caller.hxx"
#include "functional.hxx"

namespace std
{
  /**\addtogroup  lib_utilities ********** 20 General utilities library [utilities]
   *@{*/
  /**\addtogroup  lib_function_objects *** 20.6 Function objects [function.objects]
   *@{*/

 /**\defgroup  lib_refwrap *************** 20.6.05 Class template reference_wrapper [refwrap]
  *    reference_wrapper<T> is a CopyConstructible and Assignable wrapper around a reference to an object of type T.
  *@{*/
  namespace __
  {
    struct empty_base
    {};

    namespace refwrap_from
    {
      enum type {
        none,
        unary,
        binary,
        other
      };
    }

    template<typename T>
    class rfw_derives_from
    {
      template<typename T1, typename R>
      static sfinae_passed_tag  probe_unary(const volatile unary_function<T1,R>*);
      static sfinae_failed_tag  probe_unary(...);

      template<typename T1, typename T2, typename R>
      static sfinae_passed_tag  probe_binary(const volatile binary_function<T1,T2,R>*);
      static sfinae_failed_tag  probe_binary(...);

      template<typename U>
      static sfinae_passed_tag  probe_result(type2type<typename U::result_type>*);
      template<typename U>
      static sfinae_failed_tag  probe_result(...);
    public:
      static const bool  unary = sizeof( probe_unary ((T*)0) ) == sizeof(sfinae_passed_tag);
      static const bool binary = sizeof( probe_binary((T*)0) ) == sizeof(sfinae_passed_tag);
      static const bool has_result_type = sizeof( probe_result<T>(0) ) == sizeof(sfinae_passed_tag);

      static const refwrap_from::type value = 
        unary ? refwrap_from::unary :
        binary ? refwrap_from::binary :
        has_result_type ? refwrap_from::other :
        refwrap_from::none;
    };

    template<class T, refwrap_from::type derives_from>
    struct refwrap_derived;

    template<class T>
    struct refwrap_derived<T, refwrap_from::none>: empty_base
    {};

    template<class T>
    struct refwrap_derived<T, refwrap_from::unary>: 
      unary_function<typename T::argument_type, typename T::result_type>
    {};

    template<class T>
    struct refwrap_derived<T, refwrap_from::binary>: 
      binary_function<typename T::first_argument_type, typename T::second_argument_type, typename T::result_type>
    {};

    template<class T>
    struct refwrap_derived<T, refwrap_from::other>
    {
      typedef typename T::result_type result_type;
    };


    /************************************************************************/
    /* Reference wrapper base specializations                               */
    /************************************************************************/
    template<class T>
    struct refwrap_base:
      refwrap_derived<T, rfw_derives_from<typename remove_cv<T>::type>::value>
    {};

    // functions
    // TODO: take the result_of_function_type
    template<typename R>
    struct refwrap_base<R()>
    {
      typedef R result_type;
    };

    template<typename R, typename T1>
    struct refwrap_base<R(T1)>:
      unary_function<T1,R>
    {};

    template<typename R, typename T1, typename T2>
    struct refwrap_base<R(T1,T2)>:
      binary_function<T1,T2,R>
    {};

    // function pointers
    template<typename R>
    struct refwrap_base<R(*)()>
    {
      typedef R result_type;
    };

    template<typename R, typename T1>
    struct refwrap_base<R(*)(T1)>:
      unary_function<T1,R>
    {};

    template<typename R, typename T1, typename T2>
    struct refwrap_base<R(*)(T1,T2)>:
      binary_function<T1,T2,R>
    {};

    // member functions
#define NTL_RF_MAKEBASE(_cv) \
  template<typename R, class T> \
    struct refwrap_base<R(T::*)() _cv>: \
    unary_function<_cv T*,R> \
    {}; \
    template<typename R, class T, typename T1> \
    struct refwrap_base<R(T::*)(T1) _cv>: \
    binary_function<_cv T*,T1,R> \
    {};

#define _NEST(x) x
#define _EMPTY
#ifdef __ICL
#pragma warning(disable:424)
#endif
    NTL_RF_MAKEBASE(_NEST(_EMPTY));
    NTL_RF_MAKEBASE(const);
    NTL_RF_MAKEBASE(volatile);
    NTL_RF_MAKEBASE(const volatile);
#ifdef __ICL
#pragma warning(default:424)
#endif
#undef _EMPTY
#undef _NEST
#undef NTL_RF_MAKEBASE
  }

  /**
  *	@brief 20.6.05 Class template reference_wrapper [refwrap]
  *
  *  \c reference_wrapper<T> is a CopyConstructible and Assignable wrapper around a reference to an object of type \c T.
  *
  *  @note implements 20.6.5/3, 20.6.5/4 (N2723); has \c result_type if type \c T has it.
  *  @sa result_of notes
  **/
  template <class T>
  class reference_wrapper:
    public __::refwrap_base<typename remove_cv<T>::type>
  {
  public:
    // types
    typedef T type;

    // construct/copy/destroy
    /** Constructs a reference_wrapper object that stores a reference to \c t */
    explicit reference_wrapper(T& t) __ntl_nothrow
      : ptr(&t)
    {}

    /** Constructs a reference_wrapper object that stores a reference to \c x.get() */
    reference_wrapper(const reference_wrapper<T>& x) __ntl_nothrow
      : ptr(x.ptr)
    {}

    // assignment
    /** Stores a reference to \c x.get() */
    reference_wrapper& operator=(const reference_wrapper<T>& x) __ntl_nothrow
    {
      ptr = x.ptr;
      return *this;
    }

    // access
    /** Returns the stored reference */
    operator T& () const __ntl_nothrow
    {
      return this->get();
    }
    /** Returns the stored reference */
    T& get() const __ntl_nothrow
    {
      return *ptr;
    }

    typename result_of<T()>::type operator()() const { return __::func::invoke<typename result_of<T()>::type>(get(), make_tuple()); }
    template<class A1>
    typename result_of<T(A1)>::type operator()(A1& a1) const { return __::func::invoke<typename result_of<T(A1)>::type>(get(), make_tuple(a1)); }
    template<class A1, class A2>
    typename result_of<T(A1,A2)>::type operator()(A1& a1, A2& a2) const { return __::func::invoke<typename result_of<T(A1,A2)>::type>(get(), make_tuple(a1, a2)); }
    template<class A1, class A2, class A3>
    typename result_of<T(A1,A2,A3)>::type operator()(A1& a1, A2& a2, A3& a3) const { return __::func::invoke<typename result_of<T(A1,A2,A3)>::type>(get(), make_tuple(a1, a2, a3)); }
    template<class A1, class A2, class A3, class A4>
    typename result_of<T(A1,A2,A3,A4)>::type operator()(A1& a1, A2& a2, A3& a3, A4& a4) const { return __::func::invoke<typename result_of<T(A1,A2,A3,A4)>::type>(get(), make_tuple(a1, a2, a3, a4)); }
    template<class A1, class A2, class A3, class A4, class A5>
    typename result_of<T(A1,A2,A3,A4,A5)>::type operator()(A1& a1, A2& a2, A3& a3, A4& a4, A5& a5) const { return __::func::invoke<typename result_of<T(A1,A2,A3,A4,A5)>::type>(get(), make_tuple(a1, a2, a3, a4, a5)); }

    ///////////////////////////////////////////////////////////////////////////
  private:
    T* ptr;

    // do not bind to temporary objects
#if defined(NTL__CXX_RV)
    explicit reference_wrapper(T&&) __deleted;
#endif
  };

  /** Returns a reference wrapper object which holds a reference to \c t */
  template <class T>
  inline
    reference_wrapper<T> ref(T& t) __ntl_nothrow
  {
    return reference_wrapper<T>(t);
  }

  /** Returns a constant reference wrapper object which holds a reference to const \c t */
  template <class T>
  inline reference_wrapper<const T> cref(const T& t) __ntl_nothrow
  {
    return reference_wrapper<const T>(t);
  }

  /** Returns a reference wrapper object which holds a reference to \c x.get() */
  template <class T>
  inline reference_wrapper<T> ref(reference_wrapper<T> x) __ntl_nothrow
  {
    return x;
  }

  /** Returns a constant reference wrapper object which holds a constant reference to \c x.get() */
  template <class T>
  inline reference_wrapper<const T> cref(reference_wrapper<const T> x) __ntl_nothrow
  {
    return x;
  }

  /** Returns a constant reference wrapper object which holds a reference to \c x.get() */
  template <class T>
  inline reference_wrapper<const T> cref(reference_wrapper<T> x) __ntl_nothrow
  {
    return reference_wrapper<const T>(x.get());
  }

  /**@} lib_refwrap */
  /**@} lib_function_objects */
  /**@} lib_utilities */
} // std
#endif // NTL__STLX_REFWRAP
