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

#ifndef NTL_PLACEHOLDERS_HXX
#define NTL_PLACEHOLDERS_HXX

#include "type_traits.hxx"
#include "spp/loop.hxx"
#include "spp/utility.hxx"

namespace std
{
/**\addtogroup  lib_utilities ********** 20 General utilities library [utilities]
 *@{*/
/**\addtogroup  lib_function_objects ***** 20.6 Function objects [function.objects]
 *@{*/
/**\addtogroup  lib_bind ***************** 20.6.11 Function template bind [bind]
 *@{*/

// 20.6.11.1.4 Placeholders [func.bind.place]
namespace placeholders
{
namespace __
{

template <size_t N>
struct placeholder_t : integral_constant<size_t, N>
{};

#ifndef NTL_DONT_AVOID_EXTERN

//  !fr3@K!
//  This method avoids defining placeholders in cpp file, i.e. no need to
//  resolve symbols for placeholders from different compilation unit or library.
//  However, there are compatibility issues using this method, in unlikely but
//  legal usages of placeholders. For example:
//
//    template <class PlaceHolderT>
//    bool function_in_one_dll(const PlaceHolderT& ph)
//    {
//      return &ph == &std::_1;
//    }
//    bool function_in_another_dll()
//    {
//      return function_in_one_dll(std::_1);
//    }
//
//  function_in_another_dll() would wrongly returns false.
//

// !fr3@K!  Fixture template to to avoid "extern".
template <size_t N>
struct placeholder_fixture
{
  static placeholder_t<N> instance;
};

template <size_t N>
placeholder_t<N> placeholder_fixture<N>::instance;

#endif // !NTL_DONT_AVOID_EXTERN

} // namespace __

#ifndef NTL_DONT_AVOID_EXTERN

#define NTL_DEFINE_PLACEHOLDER(n, not_used) \
  __::placeholder_t<n>& NTL_SPP_CAT(_, n) = __::placeholder_fixture<n>::instance;

#else

// !fr3@K!  Only has declarations for placeholders.
//          Need to define placeholders else where.
#define NTL_DEFINE_PLACEHOLDER(n, not_used) \
  extern __::placeholder_t<n> NTL_SPP_CAT(_, n);

#endif // !NTL_DONT_AVOID_EXTERN

// !fr3@K!
// When NTL_DONT_AVOID_EXTERN is NOT defined (default):
//   __::placeholder_t<1>& _1 = __::placeholder_fixture<1>::instance;
//   __::placeholder_t<1>& _2 = __::placeholder_fixture<2>::instance;
//   ...
//
// When NTL_DONT_AVOID_EXTERN is defined:
//   extern __::placeholder_t<1> _1;
//   extern __::placeholder_t<2> _2;
//   ...
NTL_SPP_LOOP(1, 5, NTL_DEFINE_PLACEHOLDER, not_used)

#undef NTL_DEFINE_PLACEHOLDER

} // namespace placeholders

/**
 *	20.6.11.1.2 Class template is_placeholder [func.bind.isplace]
 *
 *  is_placeholder can be used to detect the standard placeholders \c _1, \c _2, and so on. 
 *  bind uses is_placeholder to detect placeholders. Users may specialize this template to indicate a placeholder type.
 *
 *  \c value is \e J if T is the type of <tt>std::placeholders::_</tt><i>J</i>, 0 otherwise.
 **/
template <class T>
struct is_placeholder
  : integral_constant<int, 0>
{};

/** is_placeholder template specialization for placeholders */
template <size_t N>
struct is_placeholder<placeholders::__::placeholder_t<N> >
  : integral_constant<int, N>
{};

/**@} lib_bind */
/**@} lib_function_objects */
/**@} lib_utilities */
} // namespace std
#endif // !NTL_PLACEHOLDERS_HXX
