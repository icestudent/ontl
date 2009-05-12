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

#ifndef NTL_SPP_CONTROL_HXX
#define NTL_SPP_CONTROL_HXX

#ifndef NTL_SPP_UTILITY_HXX
#include "utility.hxx"
#endif

// !fr3@K!
// === implementation detail ===
#define NTL_SPP_DETAIL_IF_EQ_0_0(x) x
#define NTL_SPP_DETAIL_IF_EQ_0_1(x)
#define NTL_SPP_DETAIL_IF_EQ_0_2(x)
#define NTL_SPP_DETAIL_IF_EQ_0_3(x)
#define NTL_SPP_DETAIL_IF_EQ_0_4(x)
#define NTL_SPP_DETAIL_IF_EQ_0_5(x)

#define NTL_SPP_DETAIL_IF_EQ_1_0(x)
#define NTL_SPP_DETAIL_IF_EQ_1_1(x) x
#define NTL_SPP_DETAIL_IF_EQ_1_2(x)
#define NTL_SPP_DETAIL_IF_EQ_1_3(x)
#define NTL_SPP_DETAIL_IF_EQ_1_4(x)
#define NTL_SPP_DETAIL_IF_EQ_1_5(x)

#define NTL_SPP_DETAIL_IF_EQ_2_0(x)
#define NTL_SPP_DETAIL_IF_EQ_2_1(x)
#define NTL_SPP_DETAIL_IF_EQ_2_2(x) x
#define NTL_SPP_DETAIL_IF_EQ_2_3(x)
#define NTL_SPP_DETAIL_IF_EQ_2_4(x)
#define NTL_SPP_DETAIL_IF_EQ_2_5(x)

#define NTL_SPP_DETAIL_IF_EQ_3_0(x)
#define NTL_SPP_DETAIL_IF_EQ_3_1(x)
#define NTL_SPP_DETAIL_IF_EQ_3_2(x)
#define NTL_SPP_DETAIL_IF_EQ_3_3(x) x
#define NTL_SPP_DETAIL_IF_EQ_3_4(x)
#define NTL_SPP_DETAIL_IF_EQ_3_5(x)

#define NTL_SPP_DETAIL_IF_EQ_4_0(x)
#define NTL_SPP_DETAIL_IF_EQ_4_1(x)
#define NTL_SPP_DETAIL_IF_EQ_4_2(x)
#define NTL_SPP_DETAIL_IF_EQ_4_3(x)
#define NTL_SPP_DETAIL_IF_EQ_4_4(x) x
#define NTL_SPP_DETAIL_IF_EQ_4_5(x)

#define NTL_SPP_DETAIL_IF_EQ_5_0(x)
#define NTL_SPP_DETAIL_IF_EQ_5_1(x)
#define NTL_SPP_DETAIL_IF_EQ_5_2(x)
#define NTL_SPP_DETAIL_IF_EQ_5_3(x)
#define NTL_SPP_DETAIL_IF_EQ_5_4(x)
#define NTL_SPP_DETAIL_IF_EQ_5_5(x) x

#define NTL_SPP_DETAIL_IF_EQ_0(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_EQ_0_, n)
#define NTL_SPP_DETAIL_IF_EQ_1(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_EQ_1_, n)
#define NTL_SPP_DETAIL_IF_EQ_2(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_EQ_2_, n)
#define NTL_SPP_DETAIL_IF_EQ_3(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_EQ_3_, n)
#define NTL_SPP_DETAIL_IF_EQ_4(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_EQ_4_, n)
#define NTL_SPP_DETAIL_IF_EQ_5(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_EQ_5_, n)




#define NTL_SPP_DETAIL_IF_NOT_EQ_0_0(x)
#define NTL_SPP_DETAIL_IF_NOT_EQ_0_1(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_0_2(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_0_3(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_0_4(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_0_5(x) x

#define NTL_SPP_DETAIL_IF_NOT_EQ_1_0(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_1_1(x)
#define NTL_SPP_DETAIL_IF_NOT_EQ_1_2(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_1_3(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_1_4(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_1_5(x) x

#define NTL_SPP_DETAIL_IF_NOT_EQ_2_0(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_2_1(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_2_2(x)
#define NTL_SPP_DETAIL_IF_NOT_EQ_2_3(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_2_4(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_2_5(x) x

#define NTL_SPP_DETAIL_IF_NOT_EQ_3_0(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_3_1(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_3_2(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_3_3(x)
#define NTL_SPP_DETAIL_IF_NOT_EQ_3_4(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_3_5(x) x

#define NTL_SPP_DETAIL_IF_NOT_EQ_4_0(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_4_1(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_4_2(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_4_3(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_4_4(x)
#define NTL_SPP_DETAIL_IF_NOT_EQ_4_5(x) x

#define NTL_SPP_DETAIL_IF_NOT_EQ_5_0(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_5_1(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_5_2(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_5_3(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_5_4(x) x
#define NTL_SPP_DETAIL_IF_NOT_EQ_5_5(x)

#define NTL_SPP_DETAIL_IF_NOT_EQ_0(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_NOT_EQ_0_, n)
#define NTL_SPP_DETAIL_IF_NOT_EQ_1(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_NOT_EQ_1_, n)
#define NTL_SPP_DETAIL_IF_NOT_EQ_2(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_NOT_EQ_2_, n)
#define NTL_SPP_DETAIL_IF_NOT_EQ_3(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_NOT_EQ_3_, n)
#define NTL_SPP_DETAIL_IF_NOT_EQ_4(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_NOT_EQ_4_, n)
#define NTL_SPP_DETAIL_IF_NOT_EQ_5(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_NOT_EQ_5_, n)



#define NTL_SPP_DETAIL_IF_LT_0_0(x)
#define NTL_SPP_DETAIL_IF_LT_0_1(x) x
#define NTL_SPP_DETAIL_IF_LT_0_2(x) x
#define NTL_SPP_DETAIL_IF_LT_0_3(x) x
#define NTL_SPP_DETAIL_IF_LT_0_4(x) x
#define NTL_SPP_DETAIL_IF_LT_0_5(x) x

#define NTL_SPP_DETAIL_IF_LT_1_0(x)
#define NTL_SPP_DETAIL_IF_LT_1_1(x)
#define NTL_SPP_DETAIL_IF_LT_1_2(x) x
#define NTL_SPP_DETAIL_IF_LT_1_3(x) x
#define NTL_SPP_DETAIL_IF_LT_1_4(x) x
#define NTL_SPP_DETAIL_IF_LT_1_5(x) x

#define NTL_SPP_DETAIL_IF_LT_2_0(x)
#define NTL_SPP_DETAIL_IF_LT_2_1(x)
#define NTL_SPP_DETAIL_IF_LT_2_2(x)
#define NTL_SPP_DETAIL_IF_LT_2_3(x) x
#define NTL_SPP_DETAIL_IF_LT_2_4(x) x
#define NTL_SPP_DETAIL_IF_LT_2_5(x) x

#define NTL_SPP_DETAIL_IF_LT_3_0(x)
#define NTL_SPP_DETAIL_IF_LT_3_1(x)
#define NTL_SPP_DETAIL_IF_LT_3_2(x)
#define NTL_SPP_DETAIL_IF_LT_3_3(x)
#define NTL_SPP_DETAIL_IF_LT_3_4(x) x
#define NTL_SPP_DETAIL_IF_LT_3_5(x) x

#define NTL_SPP_DETAIL_IF_LT_4_0(x)
#define NTL_SPP_DETAIL_IF_LT_4_1(x)
#define NTL_SPP_DETAIL_IF_LT_4_2(x)
#define NTL_SPP_DETAIL_IF_LT_4_3(x)
#define NTL_SPP_DETAIL_IF_LT_4_4(x)
#define NTL_SPP_DETAIL_IF_LT_4_5(x) x

#define NTL_SPP_DETAIL_IF_LT_5_0(x)
#define NTL_SPP_DETAIL_IF_LT_5_1(x)
#define NTL_SPP_DETAIL_IF_LT_5_2(x)
#define NTL_SPP_DETAIL_IF_LT_5_3(x)
#define NTL_SPP_DETAIL_IF_LT_5_4(x)
#define NTL_SPP_DETAIL_IF_LT_5_5(x)

#define NTL_SPP_DETAIL_IF_LT_0(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_LT_0_, n)
#define NTL_SPP_DETAIL_IF_LT_1(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_LT_1_, n)
#define NTL_SPP_DETAIL_IF_LT_2(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_LT_2_, n)
#define NTL_SPP_DETAIL_IF_LT_3(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_LT_3_, n)
#define NTL_SPP_DETAIL_IF_LT_4(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_LT_4_, n)
#define NTL_SPP_DETAIL_IF_LT_5(n) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_LT_5_, n)



// !fr3@K!
// === public interface ===

//  NTL_SPP_IF_LT
//  NTL_SPP_IF_EQ
//  NTL_SPP_IF_NOT_EQ
//  NTL_SPP_IF
//    If-condition-than logical control.
//
//  NTL_SPP_COMMA_IF
//    Conditional comma.
//
//  Example:
//    // example.cpp
//    #define TEST(n) \.
//      NTL_SPP_COMMA_IF(n) \.
//      NTL_SPP_IF_EQ(0, n) \.
//      ( \.
//        "ZERO" \.
//      ) \.
//      NTL_SPP_IF_EQ(1, n) \.
//      ( \.
//        "ONE" \.
//      ) \.
//      NTL_SPP_IF_LT(1, n) \.
//      ( \.
//        "GREATER THAN ONE" \.
//      )
//
//    TEST(0) TEST(1) TEST(2)
//
//    #if(0)
//
//    // GCC: g++ -E example.cpp
//    // MSVC: cl /E example.cpp
//    // Output:
//     "ZERO" , "ONE" , "GREATER THAN ONE"
//
//    #endif
#define NTL_SPP_IF_LT(lhs, rhs) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_LT_, lhs)(rhs)

#define NTL_SPP_IF_EQ(lhs, rhs) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_EQ_, lhs)(rhs)

#define NTL_SPP_IF_NOT_EQ(lhs, rhs) \
  NTL_SPP_CAT(NTL_SPP_DETAIL_IF_NOT_EQ_, lhs)(rhs)

#define NTL_SPP_IF(value) \
  NTL_SPP_IF_LT(0, value)

#define NTL_SPP_COMMA_IF(value) \
  NTL_SPP_IF(value)(NTL_SPP_COMMA)

#endif // !NTL_SPP_CONTROL_HXX
