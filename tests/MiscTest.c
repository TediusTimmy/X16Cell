/*
BSD 3-Clause License

Copyright (c) 2023, Thomas DiModica
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "floats.h"

#include <stdio.h>


#define ASSERT_TRUE(x) ASSERT_1(x, __LINE__)
void ASSERT_1(int condition, int lineNo)
 {
   if (0 == condition) printf("Assertion failed at %d\n", lineNo);
 }

#define ASSERT_EQ(x, y) ASSERT_2(x, y, __LINE__)
void ASSERT_2(x_float x, x_float y, int lineNo)
 {
   int result;
   result = 0;
   if (x[0] != y[0]) result = 1;
   if (x[1] != y[1]) result = 1;
   if (x[2] != y[2]) result = 1;
   if (x[3] != y[3]) result = 1;
   if (x[4] != y[4]) result = 1;
   if (x[5] != y[5]) result = 1;
   if (1 == result) printf("Assertion failed at %d\n", lineNo);
 }

void setFloat (x_float x, small exponent, small sign, byte digit1, byte digit2, byte digit3, byte digit4)
 {
   x[0] = (byte)exponent;
   x[1] = (byte)sign;
   x[2] = digit1;
   x[3] = digit2;
   x[4] = digit3;
   x[5] = digit4;
 }


void testMisc (void)
 {
   x_float a, b;
   setFloat(a, -128, 0, 0, 0, 0, 0);
   setFloat(b, -128, 0, 0, 0, 0, 0);

   float_neg(a); // Negative zero is still zero.

   ASSERT_EQ(b, a);

   setFloat(a, 0, 0, 0x10, 0, 0, 0);
   setFloat(b, 0, -128, 0x10, 0, 0, 0);

   float_neg(a); // Works correctly.

   ASSERT_EQ(b, a);

   setFloat(a, -128, 0, 0x12, 0x34, 0x56, 0x78);
   setFloat(b, 0, -128, 0x98, 0x76, 0x54, 0x32);

   float_cpy(b, a); // Copy works.

   ASSERT_EQ(b, a);

   setFloat(a, 0, -128, 0x10, 0, 0, 0);
   setFloat(b, 0, 0, 0x10, 0, 0, 0);

   float_abs(a); // Negates negative

   ASSERT_EQ(b, a);

   setFloat(a, 0, 0, 0x23, 0, 0, 0);
   setFloat(b, 0, 0, 0x23, 0, 0, 0);

   float_abs(a); // Doesn't negate positive

   ASSERT_EQ(b, a);

   setFloat(a, -128, -128, 0, 0, 0, 0);
   setFloat(b, -128, -128, 0, 0, 0, 0);

   float_abs(a); // Doesn't change Err

   ASSERT_EQ(b, a);

   setFloat(a, -128, 0, 0, 0, 0, 0);
   setFloat(b, -128, 0, 0, 0, 0, 0);

   float_abs(a); // Doesn't change zero. (Well, it does, but you can't tell.)

   ASSERT_EQ(b, a);
 }

void testMinMax (void)
 {
   x_float a, b, c;
   setFloat(a, -128, 0, 0, 0, 0, 0);
   setFloat(b, -128, 0, 0, 0, 0, 0);
   setFloat(c, 0, 0, 0, 0, 0, 0);

   float_min(c, a, b); // Zeros
   ASSERT_EQ(a, c);

   setFloat(a, -128, 0, 0, 0, 0, 0);
   setFloat(b, 0, 0, 0x10, 0, 0, 0);

   float_min(c, a, b); // Zero with positive
   ASSERT_EQ(a, c);
   float_min(c, b, a);
   ASSERT_EQ(a, c);

   setFloat(a, -128, 0, 0, 0, 0, 0);
   setFloat(b, 0, -128, 0x10, 0, 0, 0);

   float_min(c, a, b); // Zero with negative
   ASSERT_EQ(b, c);
   float_min(c, b, a);
   ASSERT_EQ(b, c);

   setFloat(a, -128, 0, 0, 0, 0, 0);
   setFloat(b, -128, -128, 0, 0, 0, 0);

   float_min(c, a, b); // Zero with Err
   ASSERT_EQ(b, c);
   float_min(c, b, a);
   ASSERT_EQ(b, c);

   setFloat(a, 0, 0, 0x20, 0, 0, 0);
   setFloat(b, 0, 0, 0x10, 0, 0, 0);

   float_min(c, a, b); // Same sign and exponent
   ASSERT_EQ(b, c);
   float_min(c, b, a);
   ASSERT_EQ(b, c);

   setFloat(a, 0, -128, 0x20, 0, 0, 0);
   setFloat(b, 0, -128, 0x10, 0, 0, 0);

   float_min(c, a, b); // Same sign and exponent
   ASSERT_EQ(a, c);
   float_min(c, b, a);
   ASSERT_EQ(a, c);

   setFloat(a, 0, 0, 0x20, 0, 0, 0);
   setFloat(b, 1, 0, 0x10, 0, 0, 0);

   float_min(c, a, b); // Same sign, different exponent
   ASSERT_EQ(a, c);
   float_min(c, b, a);
   ASSERT_EQ(a, c);

   setFloat(a, 0, -128, 0x20, 0, 0, 0);
   setFloat(b, 1, -128, 0x10, 0, 0, 0);

   float_min(c, a, b); // Same sign, different exponent
   ASSERT_EQ(b, c);
   float_min(c, b, a);
   ASSERT_EQ(b, c);

   setFloat(a, 0, -128, 0x20, 0, 0, 0);
   setFloat(b, 0, 0, 0x10, 0, 0, 0);

   float_min(c, a, b); // Different sign, same exponent
   ASSERT_EQ(a, c);
   float_min(c, b, a);
   ASSERT_EQ(a, c);

   setFloat(a, 0, -128, 0x20, 0, 0, 0);
   setFloat(b, 1, 0, 0x10, 0, 0, 0);

   float_min(c, a, b); // Different sign, different exponent
   ASSERT_EQ(a, c);
   float_min(c, b, a);
   ASSERT_EQ(a, c);

   setFloat(a, 0, 0, 0x20, 0, 0, 0);
   setFloat(b, -128, -128, 0x10, 0, 0, 0);

   float_min(c, a, b); // Non-zero with Err
   ASSERT_EQ(b, c);
   float_min(c, b, a);
   ASSERT_EQ(b, c);


   setFloat(a, -128, 0, 0, 0, 0, 0);
   setFloat(b, -128, 0, 0, 0, 0, 0);
   setFloat(c, 0, 0, 0, 0, 0, 0);

   float_max(c, a, b); // Zeros
   ASSERT_EQ(a, c);

   setFloat(a, -128, 0, 0, 0, 0, 0);
   setFloat(b, 0, 0, 0x10, 0, 0, 0);

   float_max(c, a, b); // Zero with positive
   ASSERT_EQ(b, c);
   float_max(c, b, a);
   ASSERT_EQ(b, c);

   setFloat(a, -128, 0, 0, 0, 0, 0);
   setFloat(b, 0, -128, 0x10, 0, 0, 0);

   float_max(c, a, b); // Zero with negative
   ASSERT_EQ(a, c);
   float_max(c, b, a);
   ASSERT_EQ(a, c);

   setFloat(a, -128, 0, 0, 0, 0, 0);
   setFloat(b, -128, -128, 0, 0, 0, 0);

   float_max(c, a, b); // Zero with Err
   ASSERT_EQ(b, c);
   float_max(c, b, a);
   ASSERT_EQ(b, c);

   setFloat(a, 0, 0, 0x20, 0, 0, 0);
   setFloat(b, 0, 0, 0x10, 0, 0, 0);

   float_max(c, a, b); // Same sign and exponent
   ASSERT_EQ(a, c);
   float_max(c, b, a);
   ASSERT_EQ(a, c);

   setFloat(a, 0, -128, 0x20, 0, 0, 0);
   setFloat(b, 0, -128, 0x10, 0, 0, 0);

   float_max(c, a, b); // Same sign and exponent
   ASSERT_EQ(b, c);
   float_max(c, b, a);
   ASSERT_EQ(b, c);

   setFloat(a, 0, 0, 0x20, 0, 0, 0);
   setFloat(b, 1, 0, 0x10, 0, 0, 0);

   float_max(c, a, b); // Same sign, different exponent
   ASSERT_EQ(b, c);
   float_max(c, b, a);
   ASSERT_EQ(b, c);

   setFloat(a, 0, -128, 0x20, 0, 0, 0);
   setFloat(b, 1, -128, 0x10, 0, 0, 0);

   float_max(c, a, b); // Same sign, different exponent
   ASSERT_EQ(a, c);
   float_max(c, b, a);
   ASSERT_EQ(a, c);

   setFloat(a, 0, -128, 0x20, 0, 0, 0);
   setFloat(b, 0, 0, 0x10, 0, 0, 0);

   float_max(c, a, b); // Different sign, same exponent
   ASSERT_EQ(b, c);
   float_max(c, b, a);
   ASSERT_EQ(b, c);

   setFloat(a, 0, -128, 0x20, 0, 0, 0);
   setFloat(b, 1, 0, 0x10, 0, 0, 0);

   float_max(c, a, b); // Different sign, different exponent
   ASSERT_EQ(b, c);
   float_max(c, b, a);
   ASSERT_EQ(b, c);

   setFloat(a, 0, 0, 0x20, 0, 0, 0);
   setFloat(b, -128, -128, 0x10, 0, 0, 0);

   float_max(c, a, b); // Non-zero with Err
   ASSERT_EQ(b, c);
   float_max(c, b, a);
   ASSERT_EQ(b, c);
 }

void testTruncRound (void)
 {
   x_float a, b;
   setFloat(a, 0, 0, 0x10, 0, 0, 0);
   setFloat(b, 0, 0, 0x10, 0, 0, 0);

   float_trunc(a);

   ASSERT_EQ(b, a); // Nothing to change.

   setFloat(a, -1, 0, 0x10, 0, 0, 0);
   setFloat(b, -128, 0, 0, 0, 0, 0);

   float_trunc(a);

   ASSERT_EQ(b, a); // Truncate to zero.

   setFloat(a, 0, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 0, 0, 0x10, 0, 0, 0);

   float_trunc(a);

   ASSERT_EQ(b, a); // 1 Digit.

   setFloat(a, 1, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 1, 0, 0x11, 0, 0, 0);

   float_trunc(a);

   ASSERT_EQ(b, a); // 2 Digits.

   setFloat(a, 2, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 2, 0, 0x11, 0x10, 0, 0);

   float_trunc(a);

   ASSERT_EQ(b, a); // 3 Digits.

   setFloat(a, 3, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 3, 0, 0x11, 0x11, 0, 0);

   float_trunc(a);

   ASSERT_EQ(b, a); // 4 Digits.

   setFloat(a, 4, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 4, 0, 0x11, 0x11, 0x10, 0);

   float_trunc(a);

   ASSERT_EQ(b, a); // 5 Digits.

   setFloat(a, 5, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 5, 0, 0x11, 0x11, 0x11, 0);

   float_trunc(a);

   ASSERT_EQ(b, a); // 6 Digits.

   setFloat(a, 6, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 6, 0, 0x11, 0x11, 0x11, 0x10);

   float_trunc(a);

   ASSERT_EQ(b, a); // 7 Digits.

   setFloat(a, 7, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 7, 0, 0x11, 0x11, 0x11, 0x11);

   float_trunc(a);

   ASSERT_EQ(b, a); // No change.

   setFloat(a, -128, -128, 0, 0, 0, 0);
   setFloat(b, -128, -128, 0, 0, 0, 0);

   float_trunc(a);

   ASSERT_EQ(b, a); // Err in, Err out.



   setFloat(a, 0, 0, 0x10, 0, 0, 0);
   setFloat(b, 0, 0, 0x10, 0, 0, 0);

   float_round(a);

   ASSERT_EQ(b, a); // Nothing to change.

   setFloat(a, -1, 0, 0x10, 0, 0, 0);
   setFloat(b, -128, 0, 0, 0, 0, 0);

   float_round(a);

   ASSERT_EQ(b, a); // Truncate to zero.

   setFloat(a, -2, 0, 0x10, 0, 0, 0);
   setFloat(b, -128, 0, 0, 0, 0, 0);

   float_round(a);

   ASSERT_EQ(b, a); // Truncate to zero, unconditional.

   setFloat(a, 0, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 0, 0, 0x10, 0, 0, 0);

   float_round(a);

   ASSERT_EQ(b, a); // 1 Digit.

   setFloat(a, 1, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 1, 0, 0x11, 0, 0, 0);

   float_round(a);

   ASSERT_EQ(b, a); // 2 Digits.

   setFloat(a, 2, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 2, 0, 0x11, 0x10, 0, 0);

   float_round(a);

   ASSERT_EQ(b, a); // 3 Digits.

   setFloat(a, 3, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 3, 0, 0x11, 0x11, 0, 0);

   float_round(a);

   ASSERT_EQ(b, a); // 4 Digits.

   setFloat(a, 4, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 4, 0, 0x11, 0x11, 0x10, 0);

   float_round(a);

   ASSERT_EQ(b, a); // 5 Digits.

   setFloat(a, 5, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 5, 0, 0x11, 0x11, 0x11, 0);

   float_round(a);

   ASSERT_EQ(b, a); // 6 Digits.

   setFloat(a, 6, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 6, 0, 0x11, 0x11, 0x11, 0x10);

   float_round(a);

   ASSERT_EQ(b, a); // 7 Digits.

   setFloat(a, 7, 0, 0x11, 0x11, 0x11, 0x11);
   setFloat(b, 7, 0, 0x11, 0x11, 0x11, 0x11);

   float_round(a);

   ASSERT_EQ(b, a); // No change.

   setFloat(a, -1, 0, 0x90, 0, 0, 0);
   setFloat(b, 0, 0, 0x10, 0, 0, 0);

   float_round(a);

   ASSERT_EQ(b, a); // Round to one.

   setFloat(a, 1, 0, 0x11, 0x51, 0x11, 0x11);
   setFloat(b, 1, 0, 0x12, 0, 0, 0);

   float_round(a);

   ASSERT_EQ(b, a); // Round 2 Digits.

   setFloat(a, 2, 0, 0x11, 0x15, 0x11, 0x11);
   setFloat(b, 2, 0, 0x11, 0x20, 0, 0);

   float_round(a);

   ASSERT_EQ(b, a); // 3 Digits.

   setFloat(a, 2, 0, 0x99, 0x95, 0x11, 0x11);
   setFloat(b, 3, 0, 0x10, 0, 0, 0);

   float_round(a);

   ASSERT_EQ(b, a); // Round with overflow.

   setFloat(a, 3, 0, 0x99, 0x99, 0x51, 0x11);
   setFloat(b, 4, 0, 0x10, 0, 0, 0);

   float_round(a);

   ASSERT_EQ(b, a); // Round with overflow.

   setFloat(a, 3, 0, 0x99, 0x79, 0x51, 0x11);
   setFloat(b, 3, 0, 0x99, 0x80, 0, 0);

   float_round(a);

   ASSERT_EQ(b, a); // Round without overflow.

   setFloat(a, -128, -128, 0, 0, 0, 0);
   setFloat(b, -128, -128, 0, 0, 0, 0);

   float_round(a);

   ASSERT_EQ(b, a); // Err in, Err out.
 }


int main (void)
 {
   testMisc();
   testMinMax();
   testTruncRound();

   printf("DONE\n");

   return 0;
 }
