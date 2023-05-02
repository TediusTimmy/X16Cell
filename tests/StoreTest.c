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
#include "store.h"

#include <stdio.h>
#include <string.h>


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


void testConversions (void)
 {
   byte row, col;
   const char* res;
   x_float a;

   ASSERT_TRUE(8U == sizeof(struct CELL_ENTRY));

   res = convertName("A0 ", &col, &row);
   ASSERT_TRUE(res != NULL);
   ASSERT_TRUE(*res == ' ');
   ASSERT_TRUE(0U == col);
   ASSERT_TRUE(0U == row);

   res = convertName("B5+", &col, &row);
   ASSERT_TRUE(res != NULL);
   ASSERT_TRUE(*res == '+');
   ASSERT_TRUE(1U == col);
   ASSERT_TRUE(5U == row);

   res = convertName("c5;", &col, &row);
   ASSERT_TRUE(res != NULL);
   ASSERT_TRUE(*res == ';');
   ASSERT_TRUE(2U == col);
   ASSERT_TRUE(5U == row);

   res = convertName("d67d", &col, &row);
   ASSERT_TRUE(res != NULL);
   ASSERT_TRUE(*res == 'd');
   ASSERT_TRUE(3U == col);
   ASSERT_TRUE(67U == row);

   res = convertName("T99", &col, &row);
   ASSERT_TRUE(res != NULL);
   ASSERT_TRUE(*res == '\0');
   ASSERT_TRUE(19U == col);
   ASSERT_TRUE(99U == row);

   res = convertName("A100", &col, &row);
   ASSERT_TRUE(res == NULL);

   res = convertName("U6", &col, &row);
   ASSERT_TRUE(res == NULL);

   res = convertName("A-1", &col, &row);
   ASSERT_TRUE(res == NULL);

   res = convertName("-1", &col, &row);
   ASSERT_TRUE(res == NULL);

   res = convertName("_1", &col, &row);
   ASSERT_TRUE(res == NULL);

   res = convertName("~1", &col, &row);
   ASSERT_TRUE(res == NULL);

   res = convertName("dd6", &col, &row);
   ASSERT_TRUE(res == NULL);


      // Make sure that these don't crash the machine.
   initStore();
   ASSERT_TRUE(NULL != lookupCell(6U, 6U));
   ASSERT_TRUE(NULL == lookupCellValue(6U, 66U));
   lookupCell(6U, 66U)->use = CELL_USE_VALUE;
   ASSERT_TRUE(NULL != lookupCellValue(6U, 66U));
   lookupCell(6U, 66U)->use = CELL_UNUSED;

      // There is a chance that these will hang the machine.
      // Run them to make sure they don't.
   recalculate(0, 0, 0);
   recalculate(1, 0, 0);
   recalculate(0, 1, 0);
   recalculate(1, 1, 0);
   recalculate(0, 0, 1);
   recalculate(1, 0, 1);
   recalculate(0, 1, 1); // This is the default one.
   recalculate(1, 1, 1);

      // This test knows too much.
   lookupCell(0U, 0U)->use = CELL_USE_VALUE;
   lookupCell(1U, 0U)->use = CELL_USE_VALUE;
   setStorePage(0);
   strcpy(getStore(0), "5");
   strcpy(getStore(1), "A0");

   recalculate(0, 1, 1);

   setFloat(a, 0, 0, 0x50, 0, 0, 0);
   ASSERT_EQ(a, lookupCell(0U, 0U)->prev);
   ASSERT_EQ(a, lookupCell(1U, 0U)->prev);
 }


int main (void)
 {
   testConversions();

   printf("DONE\n");

   return 0;
 }
