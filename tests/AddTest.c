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
   ASSERT_1(x[0] == y[0], lineNo);
   ASSERT_1(x[1] == y[1], lineNo);
   ASSERT_1(x[2] == y[2], lineNo);
   ASSERT_1(x[3] == y[3], lineNo);
   ASSERT_1(x[4] == y[4], lineNo);
   ASSERT_1(x[5] == y[5], lineNo);
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


void testLayout (void)
 {
   x_float a;
   x_float_impl* b;
   a[0] = 1;
   a[1] = 2;
   a[2] = 3;
   a[3] = 4;
   a[4] = 5;
   a[5] = 6;
   b = (x_float_impl*)a;
   ASSERT_TRUE(b->exponent == 1);
   ASSERT_TRUE(b->sign == 2);
   ASSERT_TRUE(b->digits[0] == 3);
   ASSERT_TRUE(b->digits[1] == 4);
   ASSERT_TRUE(b->digits[2] == 5);
   ASSERT_TRUE(b->digits[3] == 6);
 }

void testAdds (void)
 {
   x_float a, b, c, t;
   setFloat(t, 0, 0, 0, 0, 0, 0);

   setFloat(a, 0, 0, 0x30, 0, 0, 0);
   setFloat(b, -128, 0, 0, 0, 0, 0);

   float_add(t, a, b); // 3 + 0 is 3
   ASSERT_EQ(a, t);
   float_add(t, b, a); // 0 + 3 is 3
   ASSERT_EQ(a, t);

   setFloat(a, -128, 0, 0, 0, 0, 0);
   setFloat(b, -128, 0, 0, 0, 0, 0);

   float_add(t, a, b); // 0 + 0 is 0
   ASSERT_EQ(a, t);


   setFloat(a, 0, 0, 0x30, 0, 0, 0);
   setFloat(b, -128, -128, 0, 0, 0, 0);

   float_add(t, a, b); // 3 + Err is Err
   ASSERT_EQ(b, t);
   float_add(t, b, a); // Err + 3 is Err
   ASSERT_EQ(b, t);

   setFloat(a, -128, -128, 0, 0, 0, 0);
   setFloat(b, -128, -128, 0, 0, 0, 0);

   float_add(t, a, b); // Err + Err is Err
   ASSERT_EQ(a, t);


   setFloat(a, 0, 0, 0x30, 0, 0, 0);
   setFloat(b, 0, 0, 0x50, 0, 0, 0);
   setFloat(c, 0, 0, 0x80, 0, 0, 0);

   float_add(t, a, b); // 3 + 5 is 8
   ASSERT_EQ(c, t);

   a[0] = 1;
   b[0] = 1;
   c[0] = 1;

   float_add(t, a, b); // 30 + 50 is 80
   ASSERT_EQ(c, t);

   a[1] = -128;
   b[1] = -128;
   c[1] = -128;

   float_add(t, a, b); // -30 + -50 is -80
   ASSERT_EQ(c, t);


   setFloat(a, 0, 0, 0x90, 0, 0, 0);
   setFloat(b, 0, 0, 0x10, 0, 0, 0);
   setFloat(c, 1, 0, 0x10, 0, 0, 0);

   float_add(t, a, b); // 9 + 1 is 10
   ASSERT_EQ(c, t);

   setFloat(a, 1, 0, 0x99, 0, 0, 0);
   setFloat(c, 2, 0, 0x10, 0, 0, 0);

   float_add(t, a, b); // 99 + 1 is 100
   ASSERT_EQ(c, t);

   setFloat(a, 2, 0, 0x99, 0x90, 0, 0);
   setFloat(c, 3, 0, 0x10, 0, 0, 0);

   float_add(t, a, b); // 999 + 1 is 1000
   ASSERT_EQ(c, t);

   setFloat(a, 3, 0, 0x99, 0x99, 0, 0);
   setFloat(c, 4, 0, 0x10, 0, 0, 0);

   float_add(t, a, b); // 9999 + 1 is 10000
   ASSERT_EQ(c, t);

   setFloat(a, 4, 0, 0x99, 0x99, 0x90, 0);
   setFloat(c, 5, 0, 0x10, 0, 0, 0);

   float_add(t, a, b); // 99999 + 1 is 100000
   ASSERT_EQ(c, t);

   setFloat(a, 5, 0, 0x99, 0x99, 0x99, 0);
   setFloat(c, 6, 0, 0x10, 0, 0, 0);

   float_add(t, a, b); // 999999 + 1 is 1000000
   ASSERT_EQ(c, t);

   setFloat(a, 6, 0, 0x99, 0x99, 0x99, 0x90);
   setFloat(c, 7, 0, 0x10, 0, 0, 0);

   float_add(t, a, b); // 9999999 + 1 is 10000000
   ASSERT_EQ(c, t);

   setFloat(a, 7, 0, 0x99, 0x99, 0x99, 0x99);
   setFloat(c, 8, 0, 0x10, 0, 0, 0);

   float_add(t, a, b); // 99999999 + 1 is 100000000
   ASSERT_EQ(c, t);

   setFloat(a, 8, 0, 0x99, 0x99, 0x99, 0x99);

   float_add(t, a, b); // 999999990 + 1 is 999999990
   ASSERT_EQ(a, t);

   setFloat(a, 9, 0, 0x99, 0x99, 0x99, 0x99);

   float_add(t, a, b); // 9999999900 + 1 is 9999999900
   ASSERT_EQ(a, t);


   setFloat(a, 0, -128, 0x90, 0, 0, 0);
   setFloat(b, 0, -128, 0x10, 0, 0, 0);
   setFloat(c, 1, -128, 0x10, 0, 0, 0);

   float_add(t, a, b); // -9 + -1 is -10
   ASSERT_EQ(c, t);
   float_add(t, b, a); // -1 + -9 is -10
   ASSERT_EQ(c, t);

   setFloat(a, 1, -128, 0x99, 0, 0, 0);
   setFloat(c, 2, -128, 0x10, 0, 0, 0);

   float_add(t, a, b); // -99 + -1 is -100
   ASSERT_EQ(c, t);
   float_add(t, b, a); // -1 + -99 is -100
   ASSERT_EQ(c, t);

   setFloat(a, 2, -128, 0x99, 0x90, 0, 0);
   setFloat(c, 3, -128, 0x10, 0, 0, 0);

   float_add(t, a, b); // -999 + -1 is -1000
   ASSERT_EQ(c, t);

   setFloat(a, 7, -128, 0x99, 0x99, 0x99, 0x99);
   setFloat(c, 8, -128, 0x10, 0, 0, 0);

   float_add(t, a, b); // -99999999 + -1 is -100000000
   ASSERT_EQ(c, t);


   setFloat(a, 2, 0, 0x44, 0x44, 0x44, 0x44);
   setFloat(b, -1, 0, 0x55, 0x55, 0x55, 0x55);
   setFloat(c, 2, 0, 0x44, 0x49, 0x99, 0x99);

   float_add(t, a, b); // 444.44444 + .555555555 = 444.99999
   ASSERT_EQ(c, t);


   setFloat(a, 1, 0, 0x10, 0, 0, 0);
   setFloat(b, 0, -128, 0x10, 0, 0, 0);
   setFloat(c, 0, 0, 0x90, 0, 0, 0);

   float_add(t, a, b); // 10 - 1 = 9
   ASSERT_EQ(c, t);
   float_add(t, b, a); // -1 + 10 = 9
   ASSERT_EQ(c, t);


   setFloat(a, 2, 0, 0x10, 0, 0, 0);
   setFloat(c, 1, 0, 0x99, 0, 0, 0);

   float_add(t, a, b); // 100 - 1 = 99
   ASSERT_EQ(c, t);
   float_add(t, b, a); // -1 + 100 = 99
   ASSERT_EQ(c, t);


   setFloat(a, 3, 0, 0x10, 0, 0, 0);
   setFloat(c, 2, 0, 0x99, 0x90, 0, 0);

   float_add(t, a, b); // 1000 - 1 = 999
   ASSERT_EQ(c, t);


   setFloat(a, 8, 0, 0x10, 0, 0, 0);
   setFloat(c, 7, 0, 0x99, 0x99, 0x99, 0x99);

   float_add(t, a, b); // 100000000 - 1 = 99999999
   ASSERT_EQ(c, t);


   setFloat(a, 9, 0, 0x10, 0, 0, 0);

   float_add(t, a, b); // 1000000000 - 1 = 1000000000
   ASSERT_EQ(a, t);
   float_add(t, b, a); // -1 + 1000000000 = 1000000000
   ASSERT_EQ(a, t);


   setFloat(a, 1, -128, 0x10, 0, 0, 0);
   setFloat(b, 0, 0, 0x10, 0, 0, 0);
   setFloat(c, 0, -128, 0x90, 0, 0, 0);

   float_add(t, a, b); // -10 + 1 = -9
   ASSERT_EQ(c, t);


   setFloat(a, 2, -128, 0x10, 0, 0, 0);
   setFloat(c, 1, -128, 0x99, 0, 0, 0);

   float_add(t, a, b); // -100 + 1 = -99
   ASSERT_EQ(c, t);


   setFloat(a, 3, -128, 0x10, 0, 0, 0);
   setFloat(c, 2, -128, 0x99, 0x90, 0, 0);

   float_add(t, a, b); // -1000 + 1 = -999
   ASSERT_EQ(c, t);


   setFloat(a, 8, -128, 0x10, 0, 0, 0);
   setFloat(c, 7, -128, 0x99, 0x99, 0x99, 0x99);

   float_add(t, a, b); // -100000000 + 1 = -99999999
   ASSERT_EQ(c, t);


   setFloat(a, 0, 0, 0x30, 0, 0, 0);
   setFloat(b, 0, -128, 0x30, 0, 0, 0);
   setFloat(c, -128, 0, 0, 0, 0, 0);

   float_add(t, a, b); // 3 + -3 is 0
   ASSERT_EQ(c, t);


   setFloat(a, 2, 0, 0x99, 0x99, 0x99, 0x99);
   setFloat(b, -1, 0, 0x11, 0x11, 0x21, 0x11);
   setFloat(c, 3, 0, 0x10, 0x00, 0x11, 0x11);

   float_add(t, a, b); // 999.99999 + .11112111 = 1000.1111
   ASSERT_EQ(c, t);


   setFloat(a, 1, 0, 0x10, 0, 0, 0);
   setFloat(b, 0, -128, 0x90, 0, 0, 0);
   setFloat(c, 0, 0, 0x10, 0, 0, 0);

   float_add(t, a, b); // 10 - 9 = 1
   ASSERT_EQ(c, t);


   setFloat(a, 2, 0, 0x10, 0, 0, 0);
   setFloat(b, 1, -128, 0x99, 0, 0, 0);

   float_add(t, a, b); // 100 - 99 = 1
   ASSERT_EQ(c, t);


   setFloat(a, 3, 0, 0x10, 0, 0, 0);
   setFloat(b, 2, -128, 0x99, 0x90, 0, 0);

   float_add(t, a, b); // 1000 - 999 = 1
   ASSERT_EQ(c, t);


   setFloat(a, 4, 0, 0x10, 0, 0, 0);
   setFloat(b, 3, -128, 0x99, 0x99, 0, 0);

   float_add(t, a, b); // 10000 - 9999 = 1
   ASSERT_EQ(c, t);


   setFloat(a, 5, 0, 0x10, 0, 0, 0);
   setFloat(b, 4, -128, 0x99, 0x99, 0x90, 0);

   float_add(t, a, b); // 100000 - 99999 = 1
   ASSERT_EQ(c, t);


   setFloat(a, 6, 0, 0x10, 0, 0, 0);
   setFloat(b, 5, -128, 0x99, 0x99, 0x99, 0);

   float_add(t, a, b); // 1000000 - 999999 = 1
   ASSERT_EQ(c, t);


   setFloat(a, 7, 0, 0x10, 0, 0, 0);
   setFloat(b, 6, -128, 0x99, 0x99, 0x99, 0x90);

   float_add(t, a, b); // 10000000 - 9999999 = 1
   ASSERT_EQ(c, t);


   setFloat(a, 8, 0, 0x10, 0, 0, 0);
   setFloat(b, 7, -128, 0x99, 0x99, 0x99, 0x99);

   float_add(t, a, b); // 100000000 - 99999999 = 1
   ASSERT_EQ(c, t);


   setFloat(a, 1, -128, 0x10, 0, 0, 0);
   setFloat(b, 0, 0, 0x90, 0, 0, 0);
   setFloat(c, 0, -128, 0x10, 0, 0, 0);

   float_add(t, a, b); // -10 + 9 = -1
   ASSERT_EQ(c, t);


   setFloat(a, 2, -128, 0x10, 0, 0, 0);
   setFloat(b, 1, 0, 0x99, 0, 0, 0);

   float_add(t, a, b); // -100 + 99 = -1
   ASSERT_EQ(c, t);


   setFloat(a, 3, -128, 0x10, 0, 0, 0);
   setFloat(b, 2, 0, 0x99, 0x90, 0, 0);

   float_add(t, a, b); // -1000 + 999 = -1
   ASSERT_EQ(c, t);


   setFloat(a, 8, -128, 0x10, 0, 0, 0);
   setFloat(b, 7, 0, 0x99, 0x99, 0x99, 0x99);

   float_add(t, a, b); // -100000000 + 99999999 = -1
   ASSERT_EQ(c, t);


   setFloat(a, 49, 0, 0x90, 0, 0, 0);
   setFloat(b, 49, 0, 0x90, 0, 0, 0);
   setFloat(c, -128, -128, 0, 0, 0, 0);

   float_add(t, a, b); // 9e49 + 9e49 is Err
   ASSERT_EQ(c, t);


   setFloat(a, 49, -128, 0x90, 0, 0, 0);
   setFloat(b, 49, -128, 0x90, 0, 0, 0);
   setFloat(c, -128, -128, 0, 0, 0, 0);

   float_add(t, a, b); // -9e49 + -9e49 is Err
   ASSERT_EQ(c, t);


   setFloat(a, -49, 0, 0x20, 0, 0, 0);
   setFloat(b, -49, -128, 0x19, 0, 0, 0);
   setFloat(c, -128, 0, 0, 0, 0, 0);

   float_add(t, a, b); // 2e-49 + -1.9e-49 is 0 (1e-50)
   ASSERT_EQ(c, t);


   setFloat(a, -49, -128, 0x20, 0, 0, 0);
   setFloat(b, -49, 0, 0x19, 0, 0, 0);
   setFloat(c, -128, 0, 0, 0, 0, 0);

   float_add(t, a, b); // -2e-49 + 1.9e-49 is 0 (-1e-50)
   ASSERT_EQ(c, t);


   setFloat(a, 8, 0, 0x10, 0, 0, 0);
   setFloat(b, 4, -128, 0x10, 0, 0, 0);
   setFloat(c, 7, 0, 0x99, 0x99, 0x00, 0x00);

   float_add(t, a, b); // 100000000 - 10000 = 99990000
   ASSERT_EQ(c, t);


   setFloat(a, 8, 0, 0x10, 0, 0, 0);
   setFloat(b, 2, -128, 0x10, 0, 0, 0);
   setFloat(c, 7, 0, 0x99, 0x99, 0x99, 0x00);

   float_add(t, a, b); // 100000000 - 100 = 99999900
   ASSERT_EQ(c, t);


   setFloat(a, 7, 0, 0x10, 2, 0, 0);
   setFloat(b, 7, -128, 0x10, 1, 0, 0);
   setFloat(c, 4, 0, 0x10, 0, 0, 0);

   float_add(t, a, b); // 10020000 - 10010000 = 10000
   ASSERT_EQ(c, t);


   setFloat(a, 7, 0, 0x10, 0, 2, 0);
   setFloat(b, 7, -128, 0x10, 0, 1, 0);
   setFloat(c, 2, 0, 0x10, 0, 0, 0);

   float_add(t, a, b); // 10000200 - 10000100 = 100
   ASSERT_EQ(c, t);
 }


int main (void)
 {
   testLayout();
   testAdds();

   printf("DONE\n");

   return 0;
 }
