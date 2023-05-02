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
#include "shunting.h"
#include "store.h"

#include <stdio.h>
#include <string.h>


#define ASSERT_EQ(x, y) ASSERT_1(x, y, __LINE__)
void ASSERT_1(x_float x, x_float y, int lineNo)
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


void testYard (void)
 {
   x_float a, b;
   const char* c;
   setFloat(a, 0, 0, 0, 0, 0, 0);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   shuntingYard(a, "", 0); // Empty string is Err
   ASSERT_EQ(b, a);

   setFloat(b, -128, 0, 0, 0, 0, 0);
   shuntingYard(a, "0", 0); // zero
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x50, 0, 0, 0);
   shuntingYard(a, "5", 0); // five
   ASSERT_EQ(b, a);

   setFloat(b, 0, -128, 0x50, 0, 0, 0);
   shuntingYard(a, "-5", 0); // negative five
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x50, 0, 0, 0);
   shuntingYard(a, "2+3", 0);
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x60, 0, 0, 0);
   shuntingYard(a, "2*3", 0);
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x70, 0, 0, 0);
   shuntingYard(a, "1+2*3", 0);
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x80, 0, 0, 0);
   shuntingYard(a, "2*(3+1)", 0);
   ASSERT_EQ(b, a);

   setFloat(b, 0, -128, 0x10, 0, 0, 0);
   shuntingYard(a, "2-3", 0);
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x50, 0, 0, 0);
   shuntingYard(a, "2--3", 0);
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x40, 0, 0, 0);
   shuntingYard(a, "8/2", 0);
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x20, 0, 0, 0);
   shuntingYard(a, "6-8/2", 0);
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x40, 0, 0, 0);
   shuntingYard(a, "@abs(-8/2)", 0);
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   shuntingYard(a, "@Abs(-8/2;", 0);
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   shuntingYard(a, "@Abs", 0);
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   shuntingYard(a, "@abS(2 2)", 0);
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x50, 0, 0, 0);
   shuntingYard(a, "@Trunc(5.4)", 0);
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x60, 0, 0, 0);
   shuntingYard(a, "@ROUND(5.6)", 0);
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x30, 0, 0, 0);
   shuntingYard(a, "3*(-4+5)", 0);
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x70, 0, 0, 0);
   shuntingYard(a, "4+--3", 0);
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   shuntingYard(a, "2 3", 0);
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   shuntingYard(a, "2 (3+5)", 0);
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   c = shuntingYard(a, "2*(3+5))", 0);
   ASSERT_EQ(b, a);
   if (NULL != c) printf("Assertion failed at %d\n", __LINE__);

   setFloat(b, 1, 0, 0x16, 0, 0, 0);
   shuntingYard(a, "2*(3+5)\n", 0);
   ASSERT_EQ(b, a);

   setFloat(b, 1, 0, 0x18, 0, 0, 0);
   c = shuntingYard(a, "2*(4+5)) ", 1);
   ASSERT_EQ(b, a);
   if ((NULL == c) || (*c != ')') || (c[1] != ' ')) printf("Assertion failed at %d\n", __LINE__);

   setFloat(b, 1, 0, 0x18, 0, 0, 0);
   c = shuntingYard(a, "2*(4+5);3", 1);
   ASSERT_EQ(b, a);
   if ((NULL == c) || (*c != ';')) printf("Assertion failed at %d\n", __LINE__);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   c = shuntingYard(a, "2*(3+5);", 0);
   ASSERT_EQ(b, a);
   if (NULL != c) printf("Assertion failed at %d\n", __LINE__);
 }


int main (void)
 {
   testYard();

   printf("DONE\n");

   return 0;
 }
