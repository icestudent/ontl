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

#ifndef NTL_SPP_LOOP_HPP
#define NTL_SPP_LOOP_HPP

#include "utility.hpp"

// !fr3@K!
// === implementation detail ===
#define NTL_SPP_DETAIL_LOOP_0(init, meta, aux)

#define NTL_SPP_DETAIL_LOOP_1(init, meta, aux) \
  meta(init, aux)

#define NTL_SPP_DETAIL_LOOP_2(init, meta, aux) \
  NTL_SPP_DETAIL_LOOP_1(init, meta, aux) \
  meta(NTL_SPP_ADD(init, 1), aux)

#define NTL_SPP_DETAIL_LOOP_3(init, meta, aux) \
  NTL_SPP_DETAIL_LOOP_2(init, meta, aux) \
  meta(NTL_SPP_ADD(init, 2), aux)

#define NTL_SPP_DETAIL_LOOP_4(init, meta, aux) \
  NTL_SPP_DETAIL_LOOP_3(init, meta, aux) \
  meta(NTL_SPP_ADD(init, 3), aux)

#define NTL_SPP_DETAIL_LOOP_5(init, meta, aux) \
  NTL_SPP_DETAIL_LOOP_4(init, meta, aux) \
  meta(NTL_SPP_ADD(init, 4), aux)



// !fr3@K!
// === public interface ===

// NTL_SPP_LOOP
//    Iterative invocations of user specified meta function.
//
//    NTL_SPP_LOOP invokes user specified meta function iteratively
//    `count` times with two arguments. The first argument is an index,
//    which starts as user specified value - `init`. The index value is
//    incremented by 1 on every invocation of `meta`. The second
//    argument used to invoke `meta` is `aux`, which is specified by
//    user.
//
// Parameters:
//    init:   Initail value of first argument used to invoke `meta`.
//    count:  Number of invocations of `meta`.
//    meta:   A SPP meta-function in the form of:
//              meta(n, aux)
//    aux:    Auxiliary argument for `meta`.
//
// Example:
//    // example.cpp
//    #define META(n, aux) NTL_SPP_CAT(aux, n)
//
//    NTL_SPP_LOOP(0, 3, META, foo)
//    NTL_SPP_LOOP(3, 2, META, bar)
//    NTL_SPP_LOOP(5, 1, META, faux)
//
//    #if(0)
//
//    // GCC: g++ -E example.cpp
//    // MSVC: cl /E example.cpp
//    // Output:
//    foo0 foo1 foo2
//    bar3 bar4
//    faux5
//
//    #endif
#define NTL_SPP_LOOP(init, count, meta, aux) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_LOOP_, count)(init, meta, aux)

#endif // !NTL_SPP_LOOP_HPP
