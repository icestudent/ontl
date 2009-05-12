/**\file*********************************************************************
 *                                                                     \brief
 *  20.6 Function objects [function.objects]
 *
 ****************************************************************************
 */
/*
 * Copyright (c) 2008 fr3@K <freak@fsfoundry.org>
 *
 * When this software is distributed and/or used with Open NTL, the authors
 * grant users of this software the choice to either accept the succeeding
 * license or the license Open NTL is licensed under.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef NTL_RESULT_OF_HXX
#define NTL_RESULT_OF_HXX

#ifndef NTL__STLX_TYPE_TRAITS
#include "type_traits.hxx"
#endif

#include "spp/args.hxx"
#include "tuple.hxx"

namespace std
{
 /**\addtogroup  lib_utilities ************ 20 General utilities library [utilities]
  *@{*/
 /**\addtogroup  lib_function_objects ***** 20.6 Function objects [function.objects]
  *@{*/

/**\defgroup lib_func_ret ***************** 20.6.04 Function object return types [func.ret]
 *  Function return type detector
 *@{
 **/
  template <class> class result_of; // undefined


  namespace __
  {
    // TODO: inherit result_of_function_type from *nary_function<> when arguments count are matched
    template <class FunctionCallType>
    class result_of_function_type;

#define NTL_DEFINE_RESULT_OF_FUNCTION_TYPE(n) \
  template < \
    class ResultType NTL_SPP_COMMA_IF(n) \
    NTL_SPP_ARGS(1, n, class T) > \
    struct result_of_function_type< \
    ResultType (NTL_SPP_ARGS(1, n, T)) > \
    { \
    enum { arity = n }; \
    typedef ResultType type; \
    NTL_SPP_IF(n)(typedef T1 first_argument_type;)\
    }; \
    template < \
    class ResultType NTL_SPP_COMMA_IF(n) \
    NTL_SPP_ARGS(1, n, class T) > \
    struct result_of_function_type< \
    ResultType (&)(NTL_SPP_ARGS(1, n, T)) > \
    { \
    enum { arity = n }; \
    typedef ResultType type; \
    NTL_SPP_IF(n)(typedef T1 first_argument_type;)\
    }; \
    template < \
    class ResultType NTL_SPP_COMMA_IF(n) \
    NTL_SPP_ARGS(1, n, class T) > \
    struct result_of_function_type< \
    ResultType (*)(NTL_SPP_ARGS(1, n, T)) > \
    { \
    enum { arity = n }; \
    typedef ResultType type; \
    NTL_SPP_IF(n)(typedef T1 first_argument_type;)\
    }; \
    template < \
    class ResultType, \
    class ClassType NTL_SPP_COMMA_IF(n) \
    NTL_SPP_ARGS(1, n, class T) > \
    struct result_of_function_type< \
    ResultType (ClassType::*)(NTL_SPP_ARGS(1, n, T)) > \
    { \
    enum { arity = n }; \
    typedef ResultType type; \
    typedef ClassType object_type; \
    NTL_SPP_IF(n)(typedef ClassType first_argument_type;)\
    }; \
    template < \
    class ResultType, \
    class ClassType NTL_SPP_COMMA_IF(n) \
    NTL_SPP_ARGS(1, n, class T) > \
    struct result_of_function_type< \
    ResultType (ClassType::*)(NTL_SPP_ARGS(1, n, T)) const > \
    { \
    enum { arity = n }; \
    typedef ResultType type; \
    typedef ClassType object_type; \
    NTL_SPP_IF(n)(typedef ClassType first_argument_type;)\
    }

    NTL_DEFINE_RESULT_OF_FUNCTION_TYPE(0);
    NTL_DEFINE_RESULT_OF_FUNCTION_TYPE(1);
    NTL_DEFINE_RESULT_OF_FUNCTION_TYPE(2);
    NTL_DEFINE_RESULT_OF_FUNCTION_TYPE(3);
    NTL_DEFINE_RESULT_OF_FUNCTION_TYPE(4);
    NTL_DEFINE_RESULT_OF_FUNCTION_TYPE(5);

#undef NTL_DEFINE_RESULT_OF_FUNCTION_TYPE

    // copy cv from T to U
    template<class T, class U>
    struct copy_cv
    {
      typedef typename remove_cv<U>::type rawU;
      typedef typename conditional<is_const<T>::value,
        typename add_const<rawU>::type,
        rawU>::type cU;
      typedef typename conditional<is_volatile<T>::value,
        typename add_volatile<cU>::type,
        cU>::type cvU;
      typedef cvU type;
    };


    template<class A, class F> struct result_of_memptr;

    template<class WeakA, typename R, class T>
    struct result_of_memptr<WeakA, R T::*>
    {
      // If the type F is a pointer to data member of a class A, the nested type type is a synonym for cv R&, 
      // where R is the declared type of F, 
      // and cv represents the const and volatile qualifiers of the A object referred to by a1

      // NOTE: Но непонятно, в каких случаях должны проявляться эти cv. 
      // Если это cv a1, но a1 не ссылка, то cv теряется! Тогда во избежание надо писать result_of<F(A&)>, но так не пишут. 
      // Хз в общем.
      typedef R result_type;
      typedef T object_type;

      // weak type A
      typedef typename WeakA::type A;

      // extract cv from real object type
      typedef typename conditional<is_pointer<A>::value, typename remove_pointer<A>::type, A>::type cvA;
      // copy cv to R and add reference
      //typedef A type;
      typedef typename add_reference<typename copy_cv<cvA, R>::type>::type type;
    };

    template<class F> struct extract_first_arg;
    template<class R, class A> struct extract_first_arg<R(A)> { typedef A type; };

    template <class FunctionCallType>
    struct result_of_with_nested_result_type
    {
      typedef typename FunctionCallType::result_type type;
    };

    template <class T>
    struct has_nested_result_type_helper
    {
    private:
      template <class U>
      static sfinae_passed_tag test(type2type<typename U::result_type>*);

      template <class U>
      static sfinae_failed_tag test(...);

    public:
      static const bool value = sizeof( test<typename remove_cv<T>::type>(0) ) == sizeof(sfinae_passed_tag);
    };


    template <class T>
    struct has_nested_result_type:
      std::integral_constant<bool, has_nested_result_type_helper<T>::value>
    {};


    // NOTE: weak_type used to eliminate the incorrect instantiations, such as result_of_function_type<F>::type where F is not function ((is_func && is_memfunc) == false)
    typedef type2type<void> weak_void;
    /**
     *  @brief result_of implementation
     *
     *	For a callable type \c F and a set of argument types <tt>T1, T2, ..., TN</tt>, the type <tt>result_of<F(T1, T2, ..., TN)>::type</tt> is determined as follows:
     *  -# If the type \c F is a function object defined in the standard library, the nested type \c type is a synonym for the return type of the call <tt>f(t1, t2, ..., tN)</tt>.
     *  -# If the type \c F is a pointer to function or a function type, the nested type \c type is a synonym for its return type.
     *  -# If the type \c F is a pointer to member function, the nested type \c type is a synonym for its return type.
     *  -# If the type \c F is a pointer to data member of a class \c Ty, the nested type \c type is a synonym for <tt>cv R&</tt>, where \c R 
     *  is the declared type of \c F, and \c cv represents the const and volatile qualifiers of the \c Ty object referred to by \c t1.
     *  -# If the type \c F is a class that has a member named \c result_type that names a type, the nested type \c type is a synonym for \c F::result_type.[11]
     *   <sub>[11] This requirement is not in TR1, so an implementation that conforms to the TR1 specification does not have to satisfy it. It was accidentally left out and will be added in the future.</sub>
     *  -# If the type \c F is a class that does not have a member named \c result_type or that has a member named \c result_type that does not name a type:
     *     -# If the argument list is empty (N is 0) the nested type \c type is a synonym for \c void.
     *     -# Otherwise, the nested type \c type is a synonym for <tt>typename F::result<F(T1, T2, ..., TN)>::type</tt>.[12]
     *   <sub>[12] That is, if \c F defines a nested template named \c result, \c result_of uses that template; if \c F doesn't define that template, it's an error.</sub>
     *  -# Otherwise, the program is ill-formed.
     **/
    template <class F, size_t N = 0, class T1 = void>
    struct result_of_impl
    {
      static const bool is_func = is_function<typename remove_cv<F>::type>::value 
        || is_function<typename remove_pointer<typename remove_cv<F>::type>::type>::value
        || is_function<typename remove_reference<typename remove_cv<F>::type>::type>::value;

      static const bool is_memfunc = is_member_function_pointer<F>::value; // NOTE: is it can be ref or ptr qualified? Seems no.

      static const bool is_memptr = is_member_object_pointer<F>::value && !is_member_function_pointer<F>::value;

      static const bool has_result_type = has_nested_result_type<F>::value;


      // TODO: weak the "typename F::template result<FunctionCallType>" type (e.g. put it into separate template with conditional instantiation)
#if 0
      typedef typename 
        conditional<is_func || is_memfunc,
          typename result_of_function_type<F>::type,
          typename conditional<has_result_type,
            typename result_of_with_nested_result_type<F>::type,
            typename conditional<N == 0,
              void,
              typename F::template result<FunctionCallType>::type
                                >::type
                              >::type
                   >::type type;
#else
      typedef typename 
        conditional<is_func || is_memfunc,
          result_of_function_type<typename remove_cv<F>::type>,
          typename conditional<is_memptr && N == 1,
          result_of_memptr<type2type<T1>,F>,
            typename conditional<has_result_type,
              result_of_with_nested_result_type<F>,
              weak_void
                                >::type
                              >::type
                   >::type weak_type;

      typedef typename weak_type::type type;
#endif
    };

    template <class FunctionCallType>
    class result_of
    {
      typedef __::result_of_function_type<FunctionCallType> F;
      typedef typename conditional<F::arity == 1, __::extract_first_arg<FunctionCallType>, __::weak_void>::type weak_T1;
      typedef __::result_of_impl<typename F::type, F::arity, typename weak_T1::type> result;
    public:
      static const size_t arity = F::arity;
      typedef typename result::type type;
    };

    template<class F, class Args>
    struct result_of_va
    {
      template<class Args, size_t Length> struct result;
      template<class Args> struct result<Args,0> { typedef result_of<F()> type; };
      template<class Args> struct result<Args,1> { typedef result_of<F(typename tuple_element<0,Args>::type)> type; };
      template<class Args> struct result<Args,2> { typedef result_of<F(typename tuple_element<0,Args>::type, typename tuple_element<1,Args>::type)> type; };
      template<class Args> struct result<Args,3> { typedef result_of<F(typename tuple_element<0,Args>::type, typename tuple_element<1,Args>::type, typename tuple_element<2,Args>::type)> type; };
      typedef typename result<Args, Args::types::length>::type weak_type;
      typedef typename weak_type::type type;
      static const size_t arity = weak_type::arity;
    };

  } // namespace __


  /**
   *	@brief 20.6.4 Function return types [func.ret]
   *  
   *  \c FunctionCallType is a <tt> F(T1, T2, ..., TN) </tt>
   *
   *  If \c F is not a function object defined by the standard library, and if either the implementation cannot determine the type
   *  of the expression <tt> f(t1, t2, ..., tN) </tt> or the expression is ill-formed, the implementation shall use the following
   *  process to determine the \c type member:
   *  -# If \c F is a function pointer or function reference type, \c type shall be the return type of the function type.
   *  -# If \c F is a member function pointer type, \c type shall be the return type of the member function type.
   *  -# If \c F is a possibly cv-qualified class type with a member type \c result_type, \c type shall be <tt>typename F::result_type</tt>.
   *  -# If \c F is a possibly cv-qualified class type with no member named \c result_type or if <tt>typename F::result_type</tt> is not a type:
   *    -# If N=0 (no arguments), \c type shall be \c void.
   *    -# If N>0, \c type shall be \code typename F::template result<F(T1, T2,..., TN)>::type. \endcode
   *  -# Otherwise, the program is ill-formed.
   *
   *  @note 
   *  - I can't understand what "If F is not a function object defined by the standard library" means
   *  - I don't know what <tt>typename F::template result<F(T1, T2,..., TN)>::type</tt> means
   *  - I'm not sure about remove cv from the \c F, but some examples requires it
   *
   *  @todo weak the <tt>typename F::template result<FunctionCallType></tt> type
   **/
  template <class FunctionCallType>
  class result_of: 
    public __::result_of<FunctionCallType>
  {};

/**@} lib_func_ret         */
/**@} lib_function_objects */
/**@} lib_utilities        */

} // namespace std

#endif // !NTL_RESULT_OF_HXX
