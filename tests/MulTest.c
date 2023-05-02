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


void testMuls (void)
 {
   x_float a, b, c, t;
   setFloat(t, 0, 0, 0, 0, 0, 0);

   setFloat(a, 0, 0, 0x30, 0, 0, 0);
   setFloat(b, -128, 0, 0, 0, 0, 0);

   float_mul(t, a, b); // 3 * 0 is 0
   ASSERT_EQ(b, t);
   float_mul(t, b, a); // 0 * 3 is 0
   ASSERT_EQ(b, t);

   setFloat(a, -128, 0, 0, 0, 0, 0);
   setFloat(b, -128, 0, 0, 0, 0, 0);

   float_mul(t, a, b); // 0 * 0 is 0
   ASSERT_EQ(a, t);


   setFloat(a, 0, 0, 0x30, 0, 0, 0);
   setFloat(b, -128, -128, 0, 0, 0, 0);

   float_mul(t, a, b); // 3 * Err is Err
   ASSERT_EQ(b, t);
   float_mul(t, b, a); // Err * 3 is Err
   ASSERT_EQ(b, t);

   setFloat(a, -128, -128, 0, 0, 0, 0);
   setFloat(b, -128, -128, 0, 0, 0, 0);

   float_mul(t, a, b); // Err * Err is Err
   ASSERT_EQ(a, t);


   setFloat(a, -128, 0, 0, 0, 0, 0);
   setFloat(b, -128, -128, 0, 0, 0, 0);

   float_mul(t, a, b); // 0 * Err is Err
   ASSERT_EQ(b, t);
   float_mul(t, b, a); // Err * 0 is Err
   ASSERT_EQ(b, t);

   setFloat(a, 48, 0, 0x10, 0, 0, 0);
   setFloat(b, 48, 0, 0x30, 0, 0, 0);
   setFloat(c, -128, -128, 0, 0, 0, 0);

   float_mul(t, a, b); // Overflow to Err
   ASSERT_EQ(c, t);

   setFloat(a, -48, 0, 0x10, 0, 0, 0);
   setFloat(b, -48, 0, 0x30, 0, 0, 0);
   setFloat(c, -128, 0, 0, 0, 0, 0);

   float_mul(t, a, b); // Underflow to 0
   ASSERT_EQ(c, t);

   setFloat(a, 24, 0, 0x20, 0, 0, 0);
   setFloat(b, 25, 0, 0x50, 0, 0, 0);
   setFloat(c, -128, -128, 0, 0, 0, 0);

   float_mul(t, a, b); // Overflow to Err by way of incrementing exponent.
   ASSERT_EQ(c, t);

   setFloat(a, -25, 0, 0x20, 0, 0, 0);
   setFloat(b, -25, 0, 0x50, 0, 0, 0);
   setFloat(c, -49, 0, 0x10, 0, 0, 0);

   float_mul(t, a, b); // Save from underflow by way of incrementing exponent.
   ASSERT_EQ(c, t);

   setFloat(a, -25, 0, 0x20, 0, 0, 0);
   setFloat(b, -25, 0, 0x30, 0, 0, 0);
   setFloat(c, -128, 0, 0, 0, 0, 0);

   float_mul(t, a, b); // Don't save from underflow.
   ASSERT_EQ(c, t);

   setFloat(a, 0, 0, 0x20, 0, 0, 0);
   setFloat(b, 0, 0, 0x30, 0, 0, 0);
   setFloat(c, 0, 0, 0x60, 0, 0, 0);

   float_mul(t, a, b); // 2 * 3 is 6
   ASSERT_EQ(c, t);

   setFloat(a, 0, -128, 0x20, 0, 0, 0);
   setFloat(b, 0, 0, 0x30, 0, 0, 0);
   setFloat(c, 0, -128, 0x60, 0, 0, 0);

   float_mul(t, a, b); // -2 * 3 is -6
   ASSERT_EQ(c, t);

   setFloat(a, 0, 0, 0x20, 0, 0, 0);
   setFloat(b, 0, -128, 0x30, 0, 0, 0);
   setFloat(c, 0, -128, 0x60, 0, 0, 0);

   float_mul(t, a, b); // 2 * -3 is -6
   ASSERT_EQ(c, t);

   setFloat(a, 0, -128, 0x20, 0, 0, 0);
   setFloat(b, 0, -128, 0x30, 0, 0, 0);
   setFloat(c, 0, 0, 0x60, 0, 0, 0);

   float_mul(t, a, b); // -2 * -3 is 6
   ASSERT_EQ(c, t);

   setFloat(a, 0, 0, 0x60, 0, 0, 0);
   setFloat(b, 0, 0, 0x30, 0, 0, 0);
   setFloat(c, 1, 0, 0x18, 0, 0, 0);

   float_mul(t, a, b); // 6 * 3 is 18
   ASSERT_EQ(c, t);

   setFloat(a, 0, 0, 0x12, 0x34, 0x56, 0x78);
   setFloat(b, 0, 0, 0x87, 0x65, 0x43, 0x21);
   setFloat(c, 1, 0, 0x10, 0x82, 0x15, 0x20);

   float_mul(t, a, b);
   ASSERT_EQ(c, t);

   setFloat(a, 0, 0, 0x23, 0x45, 0x67, 0x89);
   setFloat(b, 0, 0, 0x98, 0x76, 0x54, 0x32);
   setFloat(c, 1, 0, 0x23, 0x16, 0x71, 0x98);

   float_mul(t, a, b);
   ASSERT_EQ(c, t);
 }

void testDivs (void)
 {
   x_float a, b, c, t;
   setFloat(t, 0, 0, 0, 0, 0, 0);

   setFloat(a, 0, 0, 0x30, 0, 0, 0);
   setFloat(b, -128, 0, 0, 0, 0, 0);
   setFloat(c, -128, -128, 0, 0, 0, 0);

   float_div(t, a, b); // 3 / 0 is Err
   ASSERT_EQ(c, t);
   float_div(t, b, a); // 0 / 3 is 0
   ASSERT_EQ(b, t);

   setFloat(a, -128, 0, 0, 0, 0, 0);
   setFloat(b, -128, 0, 0, 0, 0, 0);

   float_div(t, a, b); // 0 / 0 is Err (MUST FIGHT THE URGE TO MAKE IT ONE)
   ASSERT_EQ(c, t);


   setFloat(a, 0, 0, 0x30, 0, 0, 0);
   setFloat(b, -128, -128, 0, 0, 0, 0);

   float_div(t, a, b); // 3 / Err is Err
   ASSERT_EQ(b, t);
   float_div(t, b, a); // Err / 3 is Err
   ASSERT_EQ(b, t);

   setFloat(a, -128, -128, 0, 0, 0, 0);
   setFloat(b, -128, -128, 0, 0, 0, 0);

   float_div(t, a, b); // Err / Err is Err
   ASSERT_EQ(a, t);


   setFloat(a, -128, 0, 0, 0, 0, 0);
   setFloat(b, -128, -128, 0, 0, 0, 0);

   float_div(t, a, b); // 0 / Err is Err
   ASSERT_EQ(b, t);
   float_div(t, b, a); // Err / 0 is Err
   ASSERT_EQ(b, t);

   setFloat(a, 48, 0, 0x10, 0, 0, 0);
   setFloat(b, -48, 0, 0x30, 0, 0, 0);
   setFloat(c, -128, -128, 0, 0, 0, 0);

   float_div(t, a, b); // Overflow to Err
   ASSERT_EQ(c, t);

   setFloat(a, -48, 0, 0x10, 0, 0, 0);
   setFloat(b, 48, 0, 0x30, 0, 0, 0);
   setFloat(c, -128, 0, 0, 0, 0, 0);

   float_div(t, a, b); // Underflow to 0
   ASSERT_EQ(c, t);

   setFloat(a, 25, 0, 0x10, 0, 0, 0);
   setFloat(b, -25, 0, 0x20, 0, 0, 0);
   setFloat(c, 49, 0, 0x50, 0, 0, 0);

   float_div(t, a, b); // Save from overflow by way of decrementing exponent.
   ASSERT_EQ(c, t);

   setFloat(a, 25, 0, 0x40, 0, 0, 0);
   setFloat(b, -25, 0, 0x20, 0, 0, 0);
   setFloat(c, -128, -128, 0, 0, 0, 0);

   float_div(t, a, b); // Unable to save from overflow by way of decrementing exponent.
   ASSERT_EQ(c, t);

   setFloat(a, -24, 0, 0x10, 0, 0, 0);
   setFloat(b, 25, 0, 0x20, 0, 0, 0);
   setFloat(c, -128, 0, 0, 0, 0, 0);

   float_div(t, a, b); // Underflow to zero by way of decrementing exponent.
   ASSERT_EQ(c, t);

   setFloat(a, 0, 0, 0x60, 0, 0, 0);
   setFloat(b, 0, 0, 0x30, 0, 0, 0);
   setFloat(c, 0, 0, 0x20, 0, 0, 0);

   float_div(t, a, b); // 6 / 3 is 2
   ASSERT_EQ(c, t);

   setFloat(a, 0, -128, 0x60, 0, 0, 0);
   setFloat(b, 0, 0, 0x30, 0, 0, 0);
   setFloat(c, 0, -128, 0x20, 0, 0, 0);

   float_div(t, a, b); // -6 / 3 is -2
   ASSERT_EQ(c, t);

   setFloat(a, 0, 0, 0x60, 0, 0, 0);
   setFloat(b, 0, -128, 0x30, 0, 0, 0);
   setFloat(c, 0, -128, 0x20, 0, 0, 0);

   float_div(t, a, b); // 6 * -3 is -2
   ASSERT_EQ(c, t);

   setFloat(a, 0, -128, 0x60, 0, 0, 0);
   setFloat(b, 0, -128, 0x30, 0, 0, 0);
   setFloat(c, 0, 0, 0x20, 0, 0, 0);

   float_div(t, a, b); // -6 * -3 is 2
   ASSERT_EQ(c, t);

   setFloat(a, 0, 0, 0x10, 0, 0, 0);
   setFloat(b, 0, 0, 0x20, 0, 0, 0);
   setFloat(c, -1, 0, 0x50, 0, 0, 0);

   float_div(t, a, b); // 1 / 2 is .5
   ASSERT_EQ(c, t);

   setFloat(a, 0, 0, 0x20, 0, 0, 0);
   setFloat(b, 0, 0, 0x30, 0, 0, 0);
   setFloat(c, -1, 0, 0x66, 0x66, 0x66, 0x66);

   float_div(t, a, b);
   ASSERT_EQ(c, t);

   setFloat(a, 0, 0, 0x13, 0, 0, 0);
   setFloat(b, 0, 0, 0x12, 0, 0, 0);
   setFloat(c, 0, 0, 0x10, 0x83, 0x33, 0x33);

   float_div(t, a, b);
   ASSERT_EQ(c, t);
 }

void testFromString (void)
 {
   x_float a, b;

   setFloat(b, 0, 0, 0x50, 0, 0, 0);
   float_from_str(a, "5");
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x50, 0, 0, 0);
   float_from_str(a, "5,0");
   ASSERT_EQ(b, a);

   setFloat(b, 0, -128, 0x50, 0, 0, 0);
   float_from_str(a, "-5,0");
   ASSERT_EQ(b, a);

   setFloat(b, -128, 0, 0, 0, 0, 0);
   float_from_str(a, "0");
   ASSERT_EQ(b, a);

   setFloat(b, -128, 0, 0, 0, 0, 0);
   float_from_str(a, "0.0");
   ASSERT_EQ(b, a);

   setFloat(b, 7, 0, 0x12, 0x34, 0x56, 0x78);
   float_from_str(a, "12345678");
   ASSERT_EQ(b, a);

   setFloat(b, 8, 0, 0x12, 0x34, 0x56, 0x78);
   float_from_str(a, "123456789");
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x12, 0x34, 0x56, 0x78);
   float_from_str(a, "1.2345678");
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x12, 0x34, 0x56, 0x78);
   float_from_str(a, "1,23456789");
   ASSERT_EQ(b, a);

   setFloat(b, -10, 0, 0x50, 0, 0, 0);
   float_from_str(a, "5e-10");
   ASSERT_EQ(b, a);

   setFloat(b, 10, 0, 0x50, 0, 0, 0);
   float_from_str(a, "5e10");
   ASSERT_EQ(b, a);

   setFloat(b, 10, 0, 0x50, 0, 0, 0);
   float_from_str(a, "5e+10");
   ASSERT_EQ(b, a);

   setFloat(b, -128, 0, 0, 0, 0, 0);
   float_from_str(a, "5e-60");
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   float_from_str(a, "5e60");
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   float_from_str(a, "5e+60");
   ASSERT_EQ(b, a);

   setFloat(b, -128, 0, 0, 0, 0, 0);
   float_from_str(a, "-5e-60");
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   float_from_str(a, "-5e60");
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   float_from_str(a, "-5e+60");
   ASSERT_EQ(b, a);

   setFloat(b, -128, 0, 0, 0, 0, 0);
   float_from_str(a, "5e-100");
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   float_from_str(a, "5e100");
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   float_from_str(a, "5e+100");
   ASSERT_EQ(b, a);

   setFloat(b, -128, 0, 0, 0, 0, 0);
   float_from_str(a, "-5e-100");
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   float_from_str(a, "-5e100");
   ASSERT_EQ(b, a);

   setFloat(b, -128, -128, 0, 0, 0, 0);
   float_from_str(a, "-5e+100");
   ASSERT_EQ(b, a);

   setFloat(b, -2, 0, 0x10, 0, 0, 0);
   float_from_str(a, ".01");
   ASSERT_EQ(b, a);

   setFloat(b, -3, 0, 0x10, 0, 0, 0);
   float_from_str(a, ".001");
   ASSERT_EQ(b, a);

   setFloat(b, -2, 0, 0x10, 0, 0, 0);
   float_from_str(a, ",01");
   ASSERT_EQ(b, a);

   setFloat(b, -3, 0, 0x10, 0, 0, 0);
   float_from_str(a, ",001");
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x50, 0, 0, 0);
   float_from_str(a, "5e-");
   ASSERT_EQ(b, a);

   setFloat(b, 0, 0, 0x50, 0, 0, 0);
   float_from_str(a, "5E-a");
   ASSERT_EQ(b, a);
 }

// Regimes:
// > 7 : Scientific, remove trailing zeros and separator (if applicable)
// 7 : Full number
// 6 : Full number, remove trailing zero and separator (if applicable)
// 5 - -2 : Full number, remove trailing zeros but keep 2 digits past separator, even if zero
// -3 - -4 : Full number
// -5 - -11 : If we can remove enough trailing zeros so that the negative number is 14 characters, full number.
//    Else, scientific, remove trailing zeros and separator (if applicable)
// < -11 : Scientific, remove trailing zeros and separator (if applicable)
void testToString (void) // -9.9999999e+99 = 15 byte buffer
 {
   char temp [15];
   x_float a;

   setFloat(a, -128, 0, 0, 0, 0, 0);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("0.00", temp));

   setFloat(a, -128, -128, 0, 0, 0, 0);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("Err", temp));

   setFloat(a, 49, 0, 0x12, 0x34, 0x56, 0x78);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("1.2345678e+49", temp));

   setFloat(a, -49, -128, 0x12, 0x34, 0x56, 0x78);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("-1.2345678e-49", temp));

   setFloat(a, 19, -128, 0x12, 0x34, 0x56, 0x78);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("-1.2345678e+19", temp));

   setFloat(a, -19, 0, 0x12, 0x34, 0x56, 0x78);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("1.2345678e-19", temp));

   setFloat(a, 9, -128, 0x12, 0x34, 0x56, 0x78);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("-1.2345678e+9", temp)); // Can do this, but don't want imaginary precision

   setFloat(a, 7, 0, 0x12, 0x34, 0x56, 0x78);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("12345678", temp)); // Cutoff is 7 due to underlying data.

   setFloat(a, -9, 0, 0x12, 0x34, 0x56, 0x78);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("1.2345678e-9", temp));

   setFloat(a, 4, -128, 0x12, 0x34, 0x56, 0x78);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("-12345.678", temp));

   setFloat(a, -4, 0, 0x12, 0x34, 0x56, 0x78);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("0.00012345678", temp));

   setFloat(a, -5, 0, 0x12, 0x34, 0x56, 0x78);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("1.2345678e-5", temp));

   setFloat(a, -4, -128, 0x12, 0x34, 0x56, 0x78);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("-0.00012345678", temp)); // Cutoff is -4 due to this case.

   setFloat(a, -5, -128, 0x12, 0x34, 0x56, 0x78);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("-1.2345678e-5", temp));

   setFloat(a, 3, 0, 0x10, 0x00, 0x00, 0x00);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("1000.00", temp));

   setFloat(a, 9, 0, 0x10, 0x00, 0x00, 0x00);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("1e+9", temp));

   setFloat(a, 3, 0, 0x12, 0x34, 0x56, 0x00);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("1234.56", temp));

   setFloat(a, 3, 0, 0x12, 0x34, 0x50, 0x00);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("1234.50", temp)); // Always leave 2 digits after the decimal place for numbers with exponent between 0 and 5.

   setFloat(a, 0, 0, 0x50, 0x00, 0x00, 0x00);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("5.00", temp));

   setFloat(a, 5, 0, 0x50, 0x00, 0x00, 0x00);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("500000.00", temp));

   setFloat(a, 6, 0, 0x50, 0x00, 0x00, 0x00);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("5000000", temp));

   setFloat(a, 6, 0, 0x50, 0x00, 0x00, 0x01);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("5000000.1", temp));

   setFloat(a, 7, 0, 0x50, 0x00, 0x00, 0x00);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("50000000", temp));

   setFloat(a, -1, 0, 0x50, 0x00, 0x00, 0x00);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("0.50", temp));

   setFloat(a, -1, 0, 0x50, 0x20, 0x00, 0x00);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("0.502", temp));

   setFloat(a, -5, -128, 0x12, 0x34, 0x56, 0x70);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("-0.00001234567", temp));

   setFloat(a, -6, 0, 0x12, 0x34, 0x50, 0x00);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("0.0000012345", temp));

   setFloat(a, -11, -128, 0x10, 0, 0, 0);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("-0.00000000001", temp));

   setFloat(a, 20, 0, 0x10, 0, 0, 0);
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("1e+20", temp)); // Look! A bug that was found by hand testing. placeExponent was "while (check < exponent)"

   setFloat(a, 0, 0, 0, 0, 0, 0); // I'm tired of accidentally causing a lock-up.
   float_to_str(temp, a);
   ASSERT_TRUE(0 == strcmp("Bug", temp));
 }


int main (void)
 {
   testMuls();
   testDivs();
   testFromString();
   testToString();

   printf("DONE\n");

   return 0;
 }
