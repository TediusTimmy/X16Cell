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

/*
   This is just me trying to get a handle on the algorithm for doing division on the
   Felt & Tarrant Comptometer. There's a step that I don't quite understand.

   From Wikipedia:
      The Comptometer was the first commercially successful key-driven mechanical
      calculator, patented in the United States by Dorr Felt in 1887. A key-driven
      calculator is extremely fast because each key adds or subtracts its value to
      the accumulator as soon as it is pressed and a skilled operator can enter all
      of the digits of a number simultaneously, using as many fingers as required,
      making them sometimes faster to use than electronic calculators. Consequently,
      in specialized applications, comptometers remained in use in limited numbers
      into the early 1990s
         https://en.wikipedia.org/w/index.php?title=Comptometer&oldid=1132136730

   The interesting thing about how to do division on the comptometer is that it is
   a shift-add algorithm. While you repeatedly add the ten's complement negation of
   the subtrahend (which is subtraction), it is still a shift-add algorithm.
*/
#include <stdint.h>
#include <string.h>
#include <stdio.h>

static void add(char lhd [16], const char rhd [16])
 {
   int carry = 0;
   for (int current = 15; current >= 0; --current)
    {
      lhd[current] += rhd[current] + carry;

      if (lhd[current] > 9)
       {
         carry = 1;
         lhd[current] -= 10;
       }
      else
       {
         carry = 0;
       }
    }
 }

static void inc(char lhd [8])
 {
   int carry = 1;
   for (int current = 7; (current >= 0) && carry; --current)
    {
      lhd[current] += carry;

      if (lhd[current] > 9)
       {
         carry = 1;
         lhd[current] -= 10;
       }
      else
       {
         carry = 0;
       }
    }
 }

void comp_divide (char x [8], char y [8])
 {
   char a [16], b [16];
   int count = 1, times = 0;
   memset(a, '\0', 16);
   memset(b, '\0', 16);
   memcpy(b + 1, y, 8);
   if (memcmp(x, y, 8) < 0)
    {
      memcpy(a, x, 8);
    }
   else
    {
      memcpy(a + 1, x, 8);
    }
   for (int i = 1; i < 9; ++i) b[i] = 9 - b[i];
   inc(b + 1);
   for (; count < 9; ++count)
    {
      if (a[count - 1] != 0) // I don't get this step.
       {
         times = 0;
         while (times < a[count - 1])
          {
            add(a, b);
            ++times;
          }
       }
      while (memcmp(a + count, y, 8) >= 0)
       {
         add(a, b);
       }
      memmove(b + 1, b, 15);
      b[0] = 0;
    }
   memcpy(x, a, 8);
 }

int main (void)
 {
   char x[8], y[8];

   for (int32_t dd = 10000000; dd < 100000000; dd += 1000000)
    {
      for (int32_t dr = 99000000; dr > 9000000; dr -= 1000000)
       {
//int32_t dd = 11000000;
//int32_t dr = 11000000;
         int32_t temp1 = dd;
         int32_t temp2 = dr;
         for (int i = 0; i < 8; ++i)
          {
            x[7 - i] = temp1 % 10;
            temp1 /= 10;
            y[7 - i] = temp2 % 10;
            temp2 /= 10;
          }
         //printf("%d%d%d%d%d%d%d%d\t", x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7]);
         //printf("%d%d%d%d%d%d%d%d\t", y[0], y[1], y[2], y[3], y[4], y[5], y[6], y[7]);
         comp_divide(x, y);
         //printf("%d%d%d%d%d%d%d%d\t", x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7]);
         int64_t temp3 = dd;
         if (dd < dr)
            temp3 *= 100000000;
         else
            temp3 *= 10000000;
         temp3 /= dr;
         //printf("%08ld\n", temp3);
         int32_t temp4 = 0;
         for (int i = 0; i < 8; ++i)
          {
            temp4 = temp4 * 10 + x[i];
          }
         if (temp3 != temp4) printf("%08d\t%08d\t%08d\t%08ld\n", temp1, temp2, temp4, temp3);
       }
    }

   return 0;
 }
