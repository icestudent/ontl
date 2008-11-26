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

#ifndef NTL_SPP_MATH_HXX
#define NTL_SPP_MATH_HXX

// !fr3@K!
// === implementation detail ===
#define NTL_SPP_DETAIL_ADD_0_0 0
#define NTL_SPP_DETAIL_ADD_0_1 1
#define NTL_SPP_DETAIL_ADD_0_2 2
#define NTL_SPP_DETAIL_ADD_0_3 3
#define NTL_SPP_DETAIL_ADD_0_4 4
#define NTL_SPP_DETAIL_ADD_0_5 5
#define NTL_SPP_DETAIL_ADD_0(n) NTL_SPP_DETAIL_ADD_0_##n

#define NTL_SPP_DETAIL_ADD_1_0 1
#define NTL_SPP_DETAIL_ADD_1_1 2
#define NTL_SPP_DETAIL_ADD_1_2 3
#define NTL_SPP_DETAIL_ADD_1_3 4
#define NTL_SPP_DETAIL_ADD_1_4 5
#define NTL_SPP_DETAIL_ADD_1(n) NTL_SPP_DETAIL_ADD_1_##n

#define NTL_SPP_DETAIL_ADD_2_0 2
#define NTL_SPP_DETAIL_ADD_2_1 3
#define NTL_SPP_DETAIL_ADD_2_2 4
#define NTL_SPP_DETAIL_ADD_2_3 5
//#define NTL_SPP_DETAIL_ADD_2_4 6
//#define NTL_SPP_DETAIL_ADD_2_5 7
#define NTL_SPP_DETAIL_ADD_2(n) NTL_SPP_DETAIL_ADD_2_##n

#define NTL_SPP_DETAIL_ADD_3_0 3
#define NTL_SPP_DETAIL_ADD_3_1 4
#define NTL_SPP_DETAIL_ADD_3_2 5
//#define NTL_SPP_DETAIL_ADD_3_3 6
//#define NTL_SPP_DETAIL_ADD_3_4 7
//#define NTL_SPP_DETAIL_ADD_3_5 8
#define NTL_SPP_DETAIL_ADD_3(n) NTL_SPP_DETAIL_ADD_3_##n

#define NTL_SPP_DETAIL_ADD_4_0 4
#define NTL_SPP_DETAIL_ADD_4_1 5
//#define NTL_SPP_DETAIL_ADD_4_2 6
//#define NTL_SPP_DETAIL_ADD_4_3 7
//#define NTL_SPP_DETAIL_ADD_4_4 8
//#define NTL_SPP_DETAIL_ADD_4_5 9
#define NTL_SPP_DETAIL_ADD_4(n) NTL_SPP_DETAIL_ADD_4_##n

#define NTL_SPP_DETAIL_ADD_5_0 5
//#define NTL_SPP_DETAIL_ADD_5_1 6
//#define NTL_SPP_DETAIL_ADD_5_2 7
//#define NTL_SPP_DETAIL_ADD_5_3 8
//#define NTL_SPP_DETAIL_ADD_5_4 9
//#define NTL_SPP_DETAIL_ADD_5_5 10
#define NTL_SPP_DETAIL_ADD_5(n) NTL_SPP_DETAIL_ADD_5_##n


#define NTL_SPP_DETAIL_SUB_1_0 1
#define NTL_SPP_DETAIL_SUB_1_1 0
#define NTL_SPP_DETAIL_SUB_1(n) NTL_SPP_DETAIL_SUB_1_##n

#define NTL_SPP_DETAIL_SUB_2_0 2
#define NTL_SPP_DETAIL_SUB_2_1 1
#define NTL_SPP_DETAIL_SUB_2_2 0
#define NTL_SPP_DETAIL_SUB_2(n) NTL_SPP_DETAIL_SUB_2_##n

#define NTL_SPP_DETAIL_SUB_3_0 3
#define NTL_SPP_DETAIL_SUB_3_1 2
#define NTL_SPP_DETAIL_SUB_3_2 1
#define NTL_SPP_DETAIL_SUB_3_3 0
#define NTL_SPP_DETAIL_SUB_3(n) NTL_SPP_DETAIL_SUB_3_##n

#define NTL_SPP_DETAIL_SUB_4_0 4
#define NTL_SPP_DETAIL_SUB_4_1 3
#define NTL_SPP_DETAIL_SUB_4_2 2
#define NTL_SPP_DETAIL_SUB_4_3 1
#define NTL_SPP_DETAIL_SUB_4_4 0
#define NTL_SPP_DETAIL_SUB_4(n) NTL_SPP_DETAIL_SUB_4_##n

#define NTL_SPP_DETAIL_SUB_5_0 5
#define NTL_SPP_DETAIL_SUB_5_1 4
#define NTL_SPP_DETAIL_SUB_5_2 3
#define NTL_SPP_DETAIL_SUB_5_3 2
#define NTL_SPP_DETAIL_SUB_5_4 1
#define NTL_SPP_DETAIL_SUB_5_5 0
#define NTL_SPP_DETAIL_SUB_5(n) NTL_SPP_DETAIL_SUB_5_##n




// !fr3@K!
// === public interface ===

#define NTL_SPP_ADD(n, m) \
  NTL_SPP_DETAIL_ADD_##n##_##m

#define NTL_SPP_SUB(n, m) \
  NTL_SPP_DETAIL_SUB_##n##_##m

#define NTL_SPP_DEC(n) \
  NTL_SPP_DETAIL_SUB_##n(1)

#define NTL_SPP_INC(n) \
  NTL_SPP_DETAIL_ADD_##n(1)


#endif // !NTL_SPP_MATH_HXX
