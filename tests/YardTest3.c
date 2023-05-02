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
   setFloat(a, 0, 0, 0, 0, 0, 0);
   initStore();

   lookupCell(3U, 5U)->use = CELL_USE_VALUE;
   setFloat(lookupCell(3U, 5U)->prev, 0, 0, 0x50, 0, 0, 0);
   setFloat(b, 0, 0, 0x50, 0, 0, 0);
   shuntingYard(a, " D5 ", 0);
   ASSERT_EQ(b, a);

   setFloat(b, 1, 0, 0x20, 0, 0, 0);
   shuntingYard(a, " @sum((3+4)*2; 6) ", 0);
   ASSERT_EQ(b, a);

   setFloat(b, -128, 0, 0, 0, 0, 0);
   shuntingYard(a, " @sum(A0:b3) ", 0);
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x50, 0, 0, 0);
   shuntingYard(a, " @sum(@Average(A0:b3 ; 5)) ", 0);
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   shuntingYard(a, " @sum(A100:b3) ", 0);
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   shuntingYard(a, " @sum(A10 b3 :) ", 0);
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   shuntingYard(a, " @sum(A0:b300) ", 0);
   ASSERT_EQ(b, a);

   setFloat(b, -128, 0, 0, 0, 0, 0);
   shuntingYard(a, " @sum(b3:a0) ", 0);
   ASSERT_EQ(b, a);

   lookupCell(0U, 0U)->use = CELL_USE_VALUE;
   lookupCell(0U, 1U)->use = CELL_USE_VALUE;
   lookupCell(1U, 0U)->use = CELL_USE_VALUE;
   lookupCell(1U, 1U)->use = CELL_USE_VALUE;
   strcpy(getCellString(0U, 0U), "5");
   strcpy(getCellString(0U, 1U), "2");
   strcpy(getCellString(1U, 0U), "6");
   strcpy(getCellString(1U, 1U), "3");
   recalculate(0U, 1U, 1U);

   setFloat(b, 1, 0, 0x16, 0, 0, 0);
   shuntingYard(a, " @sum(a0:b1) ", 0U);
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x60, 0, 0, 0);
   shuntingYard(a, " @max(a0:b1) ", 0U);
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x60, 0, 0, 0);
   shuntingYard(a, " @max(a0:c2) ", 0U);
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   shuntingYard(a, " @max(a0 c2 :) ", 0U);
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x60, 0, 0, 0); // Another bug found during manual testing. Space between : and c2 wasn't eaten.
   shuntingYard(a, " @max(a0 :  c2) ", 0U);
   ASSERT_EQ(b, a);
 }


int main (void)
 {
   testYard();

   printf("DONE\n");

   return 0;
 }
