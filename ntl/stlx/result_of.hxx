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

#include "type_traits.hxx"
#include "spp/args.hxx"

namespace std
{
 /**\addtogroup  lib_utilities ************ 20 General utilities library [utilities]
  *@{
  **/
 /**\addtogroup  lib_function_objects ***** 20.6 Function objects [function.objects]
  *@{
  **/

/**\defgroup lib_func_ret ***************** 20.6.04 Function object return types [func.ret]
 *  Function return type detector
 *@{
 **/
  template <class> class result_of; // undefined


  namespace __
  {
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
    }; \
    template < \
    class ResultType NTL_SPP_COMMA_IF(n) \
    NTL_SPP_ARGS(1, n, class T) > \
    struct result_of_function_type< \
    ResultType (&)(NTL_SPP_ARGS(1, n, T)) > \
    { \
    enum { arity = n }; \
    typedef ResultType type; \
    }; \
    template < \
    class ResultType NTL_SPP_COMMA_IF(n) \
    NTL_SPP_ARGS(1, n, class T) > \
    struct result_of_function_type< \
    ResultType (*)(NTL_SPP_ARGS(1, n, T)) > \
    { \
    enum { arity = n }; \
    typedef ResultType type; \
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
    }

    NTL_DEFINE_RESULT_OF_FUNCTION_TYPE(0);
    NTL_DEFINE_RESULT_OF_FUNCTION_TYPE(1);
    NTL_DEFINE_RESULT_OF_FUNCTION_TYPE(2);
    NTL_DEFINE_RESULT_OF_FUNCTION_TYPE(3);
    NTL_DEFINE_RESULT_OF_FUNCTION_TYPE(4);
    NTL_DEFINE_RESULT_OF_FUNCTION_TYPE(5);

#undef NTL_DEFINE_RESULT_OF_FUNCTION_TYPE

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

    // note that the FunctionCallType is always in form F(Args...)!
    template <class FunctionCallType>
    struct result_of_impl
    {
      typedef typename result_of_function_type<FunctionCallType>::type F;
      static const size_t N = result_of_function_type<FunctionCallType>::arity;

      static const bool is_func = is_function<typename remove_cv<F>::type>::value 
        || is_function<typename remove_pointer<typename remove_cv<F>::type>::type>::value
        || is_function<typename remove_reference<typename remove_cv<F>::type>::type>::value;

      static const bool is_memfunc = is_member_function_pointer<F>::value; // NOTE: is it can be ref or ptr qualified? Seems no.

      static const bool has_result_type = has_nested_result_type<F>::value;

      // NOTE: weak_type used to eliminate the incorrect instantiations, such as result_of_function_type<F>::type where F is not function ((is_func && is_memfunc) == false)
      struct weak_void { typedef void type; };

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
          typename result_of_function_type<typename remove_cv<F>::type>,
          typename conditional<has_result_type,
            typename result_of_with_nested_result_type<F>,
            //typename conditional<N == 0,
              weak_void//,
              //typename F::template result<FunctionCallType>
              //                  >::type
                              >::type
                   >::type weak_type;
      typedef typename weak_type::type type;

#endif
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
    public __::result_of_impl<FunctionCallType>
  {};

/**@} lib_func_ret         */
/**@} lib_function_objects */
/**@} lib_utilities        */

} // namespace std


#endif // !NTL_RESULT_OF_HXX
