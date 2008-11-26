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

#include <type_traits>
#include "spp/args.hxx"

namespace std
{

namespace detail
{
template <class FunctionCallType >
class result_of_function_type;

#define NTL_DEFINE_NTL_RESULT_OF_FUNCTION_TYPE(n) \
  template < \
    class ResultType NTL_SPP_COMMA_IF(n) \
    NTL_SPP_ARGS(1, n, class T) > \
  struct result_of_function_type<ResultType (&)(NTL_SPP_ARGS(1, n, T)) > \
  { \
    typedef ResultType type; \
  }; \
  template < \
    class ResultType NTL_SPP_COMMA_IF(n) \
    NTL_SPP_ARGS(1, n, class T) > \
  struct result_of_function_type< \
    ResultType (*)(NTL_SPP_ARGS(1, n, T)) > \
  { \
    typedef ResultType type; \
  }; \
  template < \
    class ResultType, \
    class ClassType NTL_SPP_COMMA_IF(n) \
    NTL_SPP_ARGS(1, n, class T) > \
  struct result_of_function_type< \
    ResultType (ClassType::*)(NTL_SPP_ARGS(1, n, T)) > \
  { \
    typedef ResultType type; \
  }; \
  template < \
    class ResultType, \
    class ClassType NTL_SPP_COMMA_IF(n) \
    NTL_SPP_ARGS(1, n, class T) > \
  struct result_of_function_type< \
    ResultType (ClassType::*)(NTL_SPP_ARGS(1, n, T)) const > \
  { \
    typedef ResultType type; \
}

NTL_DEFINE_NTL_RESULT_OF_FUNCTION_TYPE(0);
NTL_DEFINE_NTL_RESULT_OF_FUNCTION_TYPE(1);
NTL_DEFINE_NTL_RESULT_OF_FUNCTION_TYPE(2);
NTL_DEFINE_NTL_RESULT_OF_FUNCTION_TYPE(3);
NTL_DEFINE_NTL_RESULT_OF_FUNCTION_TYPE(4);
NTL_DEFINE_NTL_RESULT_OF_FUNCTION_TYPE(5);

#undef NTL_DEFINE_NTL_RESULT_OF_FUNCTION_TYPE

template <class FunctionCallType >
struct result_of_with_nested_result_type
{
  typedef typename FunctionCallType::result_type type;
};


template <class T >
struct has_nested_result_type_helper
{
private:
  typedef char passed[1];
  typedef char failed[2];

  template <class U >
  static passed* test(typename U::result_type*);

  template <class U >
  static failed* test(...);

public:
  static const bool value = sizeof(*(test<T>(0))) == sizeof(passed);
};


template <class T >
struct has_nested_result_type :
  std::conditional<
    has_nested_result_type_helper<T >::value,
    std::true_type,
    std::false_type >::type
{};


template <class FunctionCallType >
struct result_of_impl :
  std::conditional<
    has_nested_result_type_helper<FunctionCallType>::value,
    result_of_with_nested_result_type<FunctionCallType >,
    result_of_function_type<FunctionCallType > >::type
{
};

} // namespace detail



template <class FunctionCallType >
class result_of :
  public detail::result_of_impl<FunctionCallType >
{};


} // namespace std


#endif // !NTL_RESULT_OF_HXX
