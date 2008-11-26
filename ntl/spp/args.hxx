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

#ifndef NTL_SPP_ARGS_HXX
#define NTL_SPP_ARGS_HXX

#include "control.hxx"
#include "loop.hxx"
#include "math.hxx"

// !fr3@K!
// === implementation detail ===
#define NTL_SPP_DETAIL_ARG_FIRST(n, prefix) \
  prefix##n
#define NTL_SPP_DETAIL_ARG_OTHER(n, prefix) \
  NTL_SPP_CAT(NTL_SPP_COMMA prefix, n)


// !fr3@K!
// === public interface ===

// NTL_SPP_ARGS
//    Output comma-separated symbols, as in argument-list format.
//
//    NTL_SPP_ARGS outputs symbols prefixed with `prefix` and postfixed
//    with a index value, which starts at `init` and incremented by 1,
//    `count` times.
//
// Example:
//    // example.cpp
//
//    NTL_SPP_ARGS(0, 3, foo)
//    NTL_SPP_ARGS(3, 2, bar)
//    NTL_SPP_ARGS(5, 1, faux)
//
//    #if(0)
//
//    // GCC: g++ -E example.cpp
//    // MSVC: cl /E example.cpp
//    // Output:
//    foo0, foo1, foo2
//    bar3, bar4
//    faux5
//
//    #endif

#define NTL_SPP_ARGS(init, count, prefix) \
  NTL_SPP_IF(count) \
  ( \
    NTL_SPP_DETAIL_ARG_FIRST(init, prefix) \
  ) \
  NTL_SPP_IF_LT(1, count) \
  ( \
    NTL_SPP_LOOP(NTL_SPP_INC(init), NTL_SPP_DEC(count), NTL_SPP_DETAIL_ARG_OTHER, prefix) \
  )

#endif // !NTL_SPP_ARGS_HXX
