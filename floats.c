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

#include <string.h>


#define CUTOFF   8
#define MAX_EXP 50

typedef byte add_buffer [5]; // One more than in float.
typedef byte mul_buffer [8]; // Twice the size of a float.

static void add_bufcpy(add_buffer dest, const x_float src)
 {
   dest[0] = 0;
   memmove(&dest[1], &src[2], 4U);
 }

#define add_lshift(x) gen_shl((x), 1U, sizeof(add_buffer))
#define mul_shl(x, y) gen_shl((x), (y), sizeof(mul_buffer))
static void gen_shl(byte* arg, byte amount, byte bytes)
 {
   if (amount > 1U)
    {
      memmove(arg, arg + (amount >> 1), bytes - (amount >> 1));
      memset(arg + bytes - (amount >> 1), '\0', (amount >> 1));
    }
   if (amount & 1U)
    {
      byte carryin = 0;
      byte carryout = 0;
      small count = bytes - 1;
      for (; count >= 0; --count)
       {
         carryout = arg[count] >> 4;
         arg[count] = arg[count] << 4 | carryin;
         carryin = carryout;
       }
    }
 }

#define add_rshift(x, y) gen_shr((x), (y), sizeof(add_buffer))
#define div_shr(x)       gen_shr((x), 1U, sizeof(mul_buffer))
static void gen_shr(byte* arg, byte amount, byte bytes)
 {
   if (amount > 1U)
    {
      memmove(arg + (amount >> 1), arg, bytes - (amount >> 1));
      memset(arg, '\0', (amount >> 1));
    }
   if (amount & 1U)
    {
      byte carryin = 0;
      byte carryout = 0;
      small count = 0;
      for (; count < bytes; ++count)
       {
         carryout = arg[count];
         arg[count] = (arg[count] >> 4) | (carryin << 4);
         carryin = carryout;
       }
    }
 }

#define add_doadd(x, y, z, c) gen_doadd((x), (y), (z), sizeof(add_buffer) - 1, (c))
#define mul_doadd(x, y)       gen_doadd((x), (x), (y), sizeof(mul_buffer) - 1, 0)
static void gen_doadd(add_buffer dest, const add_buffer lhd, add_buffer rhd, small start, byte carry)
 {
   for (; start >= 0; --start)
    {
      byte lb = lhd[start];
      byte rb = rhd[start];
      byte lhn = lb >> 4;
      byte lln = lb & 15;
      byte rhn = rb >> 4;
      byte rln = rb & 15;

      lln = lln + rln + carry;
      if (lln > 9)
       {
         carry = 1;
         lln -= 10;
       }
      else
       {
         carry = 0;
       }

      lhn = lhn + rhn + carry;
      if (lhn > 9)
       {
         carry = 1;
         lhn -= 10;
       }
      else
       {
         carry = 0;
       }

      dest[start] = lln | (lhn << 4);
    }
 }

static void add_dosub(add_buffer dest, const add_buffer lhd, add_buffer rhd)
 {
   rhd[0] = 0x99 - rhd[0];
   rhd[1] = 0x99 - rhd[1];
   rhd[2] = 0x99 - rhd[2];
   rhd[3] = 0x99 - rhd[3];
   rhd[4] = 0x99 - rhd[4];
   add_doadd(dest, lhd, rhd, 1);
 }

static int add_iszero(const add_buffer lhd)
 {
   static const char * const test = "\0\0\0\0\0";
   return memcmp(lhd, test, sizeof(add_buffer)) == 0;
 }

static void set_to_special(x_float dest, byte special)
 {
   memset(dest, '\0', 6);
   dest[0] = 0x80;
   dest[1] = special;
 }

static void set_digits_to_zero(x_float dest, small digits)
 {
   small start = 1 + 4 - (digits >> 1);
   if (digits & 1)
    {
      dest[start] = dest[start] & 0xF0;
    }
   ++start;
   while (start < 6)
    {
      dest[start] = 0;
      ++start;
    }
 }

static byte placeDigits (char dest [15], const x_float src, byte digits, byte place, byte put_separator)
 {
   byte digit = 3;
   dest[place] = (src[2] >> 4) + '0';
   ++place;
   --digits;
   if (digits)
    {
      if (put_separator)
       {
         dest[place] = '.';
         ++place;
       }
      dest[place] = (src[2] & 0xF) + '0';
      ++place;
      --digits;
    }
   while (digits)
    {
      dest[place] = (src[digit] >> 4) + '0';
      ++place;
      --digits;
      if (digits)
       {
         dest[place] = (src[digit] & 0xF) + '0';
         ++place;
         --digits;
       }
      ++digit;
    }
   return place;
 }

static void placeExponent (char dest [15], small exponent, byte place)
 {
   small digit = 0;
   small check = 0;
   dest[place] = 'e';
   ++place;
   if (exponent < 0)
    {
      exponent = -exponent;
      dest[place] = '-';
    }
   else
    {
      dest[place] = '+';
    }
   ++place;
   while (check <= exponent)
    {
      check += 10;
      ++digit;
    }
   check -= 10;
   --digit;
   if (digit)
    {
      dest[place] = digit + '0';
      ++place;
    }
   dest[place] = (exponent - check) + '0';
 }

static void mul_mulByDigit(add_buffer lhd, byte digit)
 {
   add_buffer temp;
   memcpy(temp, lhd, sizeof(add_buffer));
   switch(digit)
    {
   case 2:
      add_doadd(lhd, lhd, lhd, 0);
      break;
   case 3:
      add_doadd(lhd, lhd, lhd, 0);
      add_doadd(lhd, lhd, temp, 0);
      break;
   case 4:
      add_doadd(lhd, lhd, lhd, 0);
      add_doadd(lhd, lhd, lhd, 0);
      break;
   case 5:
      add_doadd(lhd, lhd, lhd, 0);
      add_doadd(lhd, lhd, lhd, 0);
      add_doadd(lhd, lhd, temp, 0);
      break;
   case 6:
      add_doadd(lhd, lhd, lhd, 0);
      add_doadd(lhd, lhd, temp, 0);
      add_doadd(lhd, lhd, lhd, 0);
      break;
   case 7:
      add_doadd(lhd, lhd, lhd, 0);
      add_doadd(lhd, lhd, temp, 0);
      add_doadd(lhd, lhd, lhd, 0);
      add_doadd(lhd, lhd, temp, 0);
      break;
   case 8:
      add_doadd(lhd, lhd, lhd, 0);
      add_doadd(lhd, lhd, lhd, 0);
      add_doadd(lhd, lhd, lhd, 0);
      break;
   case 9:
      add_doadd(lhd, lhd, lhd, 0);
      add_doadd(lhd, lhd, lhd, 0);
      add_doadd(lhd, lhd, lhd, 0);
      add_doadd(lhd, lhd, temp, 0);
      break;
    }
 }

static small div_cmp(const byte* lhs, const byte* rhs)
 {
   small temp = 0;
   for (; temp < 3; ++temp)
    {
      small ld = lhs[temp] & 0xF;
      small rd = rhs[temp] >> 4;
      if (ld != rd)
       {
         return ld - rd;
       }
      ld = lhs[temp + 1] >> 4;
      rd = rhs[temp] & 0xF;
      if (ld != rd)
       {
         return ld - rd;
       }
    }
   return 0;
 }



void float_cpy (x_float dest, const x_float src)
 {
   if (src != dest) // They COULD be the same.
    {
      memcpy(dest, src, sizeof(x_float));
    }
 }

void float_neg (x_float arg)
 {
   if (arg[0] == 0x80) // There is no negative zero.
    {
      return;
    }
   arg[1] ^= -128;
 }

void float_abs (x_float arg)
 {
   if ((arg[0] == 0x80) && (arg[1] == 0x80)) // Don't change an error condition.
    {
      return;
    }
   arg[1] = 0;
 }

void float_min (x_float dest, const x_float lhs, const x_float rhs)
 {
   x_float_impl* LHS = (x_float_impl*)lhs;
   x_float_impl* RHS = (x_float_impl*)rhs;
   small temp;
   if (lhs[0] == 0x80)
    {
      if (lhs[1] == 0x80) // Err
       {
         float_cpy(dest, lhs);
       }
      else if (rhs[1] == 0x80) // RHS is - or Err
       {
         float_cpy(dest, rhs);
       }
      else
       {
         float_cpy(dest, lhs);
       }
      return;
    }
   if (rhs[0] == 0x80)
    {
      if (rhs[1] == 0x80) // Err
       {
         float_cpy(dest, rhs);
       }
      else if (lhs[1] == 0x80) // LHS is - or Err
       {
         float_cpy(dest, lhs);
       }
      else
       {
         float_cpy(dest, rhs);
       }
      return;
    }
   if (LHS->sign != RHS->sign)
    {
      if (LHS->sign < 0)
       {
         float_cpy(dest, lhs);
       }
      else
       {
         float_cpy(dest, rhs);
       }
    }
   else if (LHS->exponent != RHS->exponent)
    {
      temp = LHS->exponent > RHS->exponent;
      if (LHS->sign)
       {
         temp = !temp;
       }
      if (temp)
       {
         float_cpy(dest, rhs);
       }
      else
       {
         float_cpy(dest, lhs);
       }
    }
   else
    {
      temp = memcmp(&lhs[2], &rhs[2], sizeof(x_float) - 2U) > 0;
      if (LHS->sign)
       {
         temp = !temp;
       }
      if (temp)
       {
         float_cpy(dest, rhs);
       }
      else
       {
         float_cpy(dest, lhs);
       }
    }
 }

void float_max (x_float dest, const x_float lhs, const x_float rhs)
 {
   x_float_impl* LHS = (x_float_impl*)lhs;
   x_float_impl* RHS = (x_float_impl*)rhs;
   small temp;
   if (lhs[0] == 0x80)
    {
      if (lhs[1] == 0x80) // Err
       {
         float_cpy(dest, lhs);
       }
      else if ((rhs[1] != 0x80) || (rhs[0] == 0x80)) // RHS is + or Err
       {
         float_cpy(dest, rhs);
       }
      else
       {
         float_cpy(dest, lhs);
       }
      return;
    }
   if (rhs[0] == 0x80)
    {
      if (rhs[1] == 0x80) // Err
       {
         float_cpy(dest, rhs);
       }
      else if (lhs[1] != 0x80) // LHS is + (Err case handled above)
       {
         float_cpy(dest, lhs);
       }
      else
       {
         float_cpy(dest, rhs);
       }
      return;
    }
   if (LHS->sign != RHS->sign)
    {
      if (LHS->sign < 0)
       {
         float_cpy(dest, rhs);
       }
      else
       {
         float_cpy(dest, lhs);
       }
    }
   else if (LHS->exponent != RHS->exponent)
    {
      temp = LHS->exponent > RHS->exponent;
      if (LHS->sign)
       {
         temp = !temp;
       }
      if (temp)
       {
         float_cpy(dest, lhs);
       }
      else
       {
         float_cpy(dest, rhs);
       }
    }
   else
    {
      temp = memcmp(&lhs[2], &rhs[2], sizeof(x_float) - 2U) > 0;
      if (LHS->sign)
       {
         temp = !temp;
       }
      if (temp)
       {
         float_cpy(dest, lhs);
       }
      else
       {
         float_cpy(dest, rhs);
       }
    }
 }

void float_trunc (x_float arg)
 {
   x_float_impl* ARG = (x_float_impl*)arg;
   small digits;
   if (arg[0] == 0x80) // Don't change zero or error.
    {
      return;
    }
   if (ARG->exponent < 0) // Go to zero.
    {
      set_to_special(arg, 0);
      return;
    }
   if (ARG->exponent > 6) // Nothing to change.
    {
      return;
    }
   digits = 7 - ARG->exponent; // Digits to remove.
   set_digits_to_zero(arg, digits);
 }

void float_round (x_float arg)
 {
   x_float_impl* ARG = (x_float_impl*)arg;
   small digits, digit;
   byte carryin, temp1, temp2;
   if (arg[0] == 0x80) // Don't change zero or error.
    {
      return;
    }
   if (ARG->exponent < -1)
    {
      set_to_special(arg, 0);
      return;
    }
   if (ARG->exponent > 6) // Nothing to change.
    {
      return;
    }
   if (-1 == ARG->exponent)
    {
      if ((arg[2] & 0xF0) > 0x40)
       {
         arg[0] = 0;
         arg[2] = 0x10;
         arg[3] = 0;
         arg[4] = 0;
         arg[5] = 0;
       }
      else
       {
         set_to_special(arg, 0);
       }
      return;
    }
   digits = 7 - ARG->exponent; // Digits to remove.
   carryin = 0;
   digit = 1 + 4 - (digits >> 1);
   if (digits & 1)
    {
      carryin = (arg[digit] & 0xF) > 4;
    }
   else
    {
      carryin = (arg[digit + 1] & 0xF0) > 0x40;
    }
   set_digits_to_zero(arg, digits);
   if (0 != carryin)
    {
      if (digits & 1)
       {
         arg[digit] = arg[digit] + 0x10;
         if (arg[digit] > 0x90)
          {
            arg[digit] = 0;
          }
         else
          {
            digit = 2;
            carryin = 0;
          }
         --digit;
       }
      while (digit > 1)
       {
         temp1 = arg[digit] & 0xF;
         temp2 = (arg[digit] & 0xF0) >> 4;
         temp1 = temp1 + 1;
         if (temp1 > 9)
          {
            temp1 = 0;
            temp2 = temp2 + 1;
            if (temp2 > 9)
             {
               temp2 = 0;
             }
            else
             {
               carryin = 0;
             }
            arg[digit] = temp1 | (temp2 << 4);
            if (0 == carryin)
             {
               digit = 2;
             }
          }
         else
          {
            arg[digit] = temp1 | (temp2 << 4);
            digit = 2;
            carryin = 0;
          }
         --digit;
       }
      if (1 == carryin)
       {
         ARG->exponent = ARG->exponent + 1;
         arg[2] = 0x10;
       }
    }
 }

void float_add (x_float dest, const x_float lhs, const x_float rhs)
 {
   x_float_impl* DEST = (x_float_impl*)dest;
   x_float_impl* LHS = (x_float_impl*)lhs;
   x_float_impl* RHS = (x_float_impl*)rhs;
   small resExp, resSign, expDiff;
   add_buffer lhd, rhd; // Declare temp buffers.

   if (LHS->exponent == -128) // Is the lhs zero?
    {
      if (LHS->sign == -128) // Is it Err?
       {
         float_cpy(dest, lhs); // Retain Err
       }
      else
       {
         float_cpy(dest, rhs); // Just return RHS
       }
      return;
    }

   else if (RHS->exponent == -128) // Is the rhs zero?
    {
      if (RHS->sign == -128) // Is it Err?
       {
         float_cpy(dest, rhs); // Retain Err
       }
      else
       {
         float_cpy(dest, lhs); // Just return LHS
       }
      return;
    }

   resExp = LHS->exponent;
   expDiff = 0;
   if (LHS->exponent > RHS->exponent)
    {
      expDiff = LHS->exponent - RHS->exponent;
      if (expDiff <= CUTOFF)
       {
         add_bufcpy(lhd, lhs);
         add_bufcpy(rhd, rhs);
         if (expDiff > 1)
          {
            add_rshift(rhd, expDiff - 1);
            expDiff = 1;
          }
         add_lshift(lhd);
       }
      else
       {
         float_cpy(dest, lhs); // Result won't change
         return;
       }
    }
   else if (LHS->exponent < RHS->exponent)
    {
      resExp = RHS->exponent;
      expDiff = RHS->exponent - LHS->exponent;
      if (expDiff <= CUTOFF)
       {
         add_bufcpy(lhd, lhs);
         add_bufcpy(rhd, rhs);
         if (expDiff > 1)
          {
            add_rshift(lhd, expDiff - 1);
            expDiff = 1;
          }
         add_lshift(rhd);
       }
      else
       {
         float_cpy(dest, rhs); // Result won't change
         return;
       }
    }
   else
    {
      add_bufcpy(lhd, lhs);
      add_bufcpy(rhd, rhs);
    }

   resSign = LHS->sign;
   if (LHS->sign == RHS->sign)
    {
      add_doadd(lhd, lhd, rhd, 0);
    }
   else
    {
      if (memcmp(lhd, rhd, sizeof(add_buffer)) >= 0)
       {
         add_dosub(lhd, lhd, rhd);
       }
      else
       {
         add_dosub(lhd, rhd, lhd);
         resSign ^= -128;
       }
    }

   if (add_iszero(lhd))
    {
      resExp = -128;
      resSign = 0;
    }
   else if (0 != lhd[0])
    {
      if (0 == expDiff)
       {
         ++resExp;
       }
      add_rshift(lhd, 1U);
      if (0 != lhd[0])
       {
         ++resExp;
         add_rshift(lhd, 1U);
       }
      if (resExp >= MAX_EXP)
       {
         resExp = -128;
         resSign = -128;
         memset(&lhd[1], '\0', 4U);
       }
    }
   else
    {
      if (1 == expDiff)
       {
         --resExp;
       }
      while (0 == (lhd[1] & 0xF0))
       {
         add_lshift(lhd);
         --resExp;
       }
      if (resExp <= -MAX_EXP)
       {
         resExp = -128;
         resSign = 0;
         memset(&lhd[1], '\0', 4U);
       }
    }

   memmove(&dest[2], &lhd[1], 4U);
   DEST->sign = resSign;
   DEST->exponent = resExp;
 }

void float_mul (x_float dest, const x_float lhs, const x_float rhs)
 {
   x_float_impl* LHS = (x_float_impl*)lhs;
   x_float_impl* RHS = (x_float_impl*)rhs;
   byte resultSign = lhs[1] ^ rhs[1]; // Sign is computed here.
   byte digit, curr;
   small resultExponent;
   mul_buffer sum, addend;

   if (lhs[0] == 0x80) // LHS zero?
    {
      if ((rhs[0] == 0x80) && (rhs[1] == 0x80)) // RHS is Err, keep that
       {
         float_cpy(dest, rhs);
       }
      else // LHS is either zero or Err: 0 * x is 0 and Err * x is Err
       {
         float_cpy(dest, lhs);
       }
      return;
    }
   if (rhs[0] == 0x80) // RHS is either zero or Err, either way return that instead.
    {
      float_cpy(dest, rhs);
      return;
    }

   resultExponent = LHS->exponent + RHS->exponent; // Compute the exponent
      // Can we quit early?
   if (resultExponent >= MAX_EXP) // Overflow to Err
    {
      set_to_special(dest, -128);
      return;
    }
   else if (resultExponent <= (-MAX_EXP - 1)) // Underflow to zero
    {
      set_to_special(dest, 0);
      return;
    }

   // Perform a multiply
   memset(sum, '\0', 8);
   curr = 0;
   for (digit = 5; digit > 1; --digit)
    {
      byte mb = rhs[digit];
      byte mhn = mb >> 4;
      byte mln = mb & 15;

      if (0U != mln)
       {
         memset(addend, '\0', 4);
         memcpy(&addend[4], &lhs[2], sizeof(x_float) - 2U);
         mul_mulByDigit(addend + 3, mln);
         mul_shl(addend, curr);
         mul_doadd(sum, addend);
       }
      ++curr;
      if (0U != mhn)
       {
         memset(addend, '\0', 4);
         memcpy(&addend[4], &lhs[2], sizeof(x_float) - 2U);
         mul_mulByDigit(addend + 3, mhn);
         mul_shl(addend, curr);
         mul_doadd(sum, addend);
       }
      ++curr;
    }
   if (0 == (sum[0] & 0xF0))
    {
      mul_shl(sum, 1U);
    }
   else
    {
      ++resultExponent;
    }

   if (resultExponent >= MAX_EXP) // Overflow to Err
    {
      set_to_special(dest, -128);
    }
   else if (resultExponent <= -MAX_EXP) // Underflow to zero : unable to save a result
    {
      set_to_special(dest, 0);
    }
   else
    {
      // set result
      dest[0] = (byte)resultExponent;
      dest[1] = resultSign;
      memcpy(&dest[2], sum, sizeof(x_float) - 2U);
    }
 }

void float_div (x_float dest, const x_float lhs, const x_float rhs)
 {
   x_float_impl* LHS = (x_float_impl*)lhs;
   x_float_impl* RHS = (x_float_impl*)rhs;
   byte resultSign = lhs[1] ^ rhs[1]; // Sign is computed here.
   small resultExponent;
   mul_buffer sum, addend;
   byte count, times;

   if (rhs[0] == 0x80) // RHS is either zero or Err, either way return Err.
    {
      float_cpy(dest, rhs);
      dest[1] = 0x80;
      return;
    }
   if (lhs[0] == 0x80) // LHS is either zero or Err: 0 / x is 0 and Err / x is Err
    {
      float_cpy(dest, lhs);
      return;
    }

   resultExponent = LHS->exponent - RHS->exponent; // Compute the exponent
      // If this is true, then the exponent will be one less.
   times = memcmp(&lhs[2], &rhs[2], sizeof(x_float) - 2U) < 0;
   resultExponent -= times;
      // Can we quit early?
   if (resultExponent >= MAX_EXP) // Overflow to Err
    {
      set_to_special(dest, -128);
      return;
    }
   else if (resultExponent <= -MAX_EXP) // Underflow to zero
    {
      set_to_special(dest, 0);
      return;
    }

   // Perform a division
   /*
      If this were a standard shift-subtract algorithm, I wouldn't feel the need to attribute it.
      The algorithm here is from "Methods of Operating the Comptometer" from Felt and Tarrant Mfg. Co.
      I was looking at the 1921 edition for the Model H, but any edition that includes division will work.
      This is the algorithm utilized by a century-old adding machine.

      The quotient is developed by repeated addition of the ten's complement of the divisor.
      The dividend is transformed into the quotient: only one register (variable) is used.
   */
   memset(sum, '\0', 8);
   memset(addend, '\0', 8);

   memcpy(&addend[4], &rhs[2], sizeof(x_float) - 2U);
   for (count = 4; count < 8; ++count)
    {
      addend[count] = 0x99 - addend[count];
    }
   sum[7] = 1;
   mul_doadd(addend, sum);
   memcpy(&sum[4], &lhs[2], sizeof(x_float) - 2U);
   
   mul_shl(addend, 7);
   mul_shl(sum, 7 + times);

   for (count = 0; count < 4; ++count)
    {
      if (sum[count] >> 4) // I don't get this step.
       {
         times = 0;
         while (times < (sum[count] >> 4))
          {
            mul_doadd(sum, addend);
            ++times;
          }
       }
      while (div_cmp(sum + count, rhs + 2) >= 0)
       {
         mul_doadd(sum, addend);
       }
      div_shr(addend);

      if (sum[count] & 0xF) // I don't get this step.
       {
         times = 0;
         while (times < (sum[count] & 0xF))
          {
            mul_doadd(sum, addend);
            ++times;
          }
       }
      while (memcmp(sum + count + 1, &rhs[2], sizeof(x_float) - 2U) >= 0)
       {
         mul_doadd(sum, addend);
       }
      div_shr(addend);
    }

   // set result
   dest[0] = (byte)resultExponent;
   dest[1] = resultSign;
   memcpy(&dest[2], sum, sizeof(x_float) - 2U);
 }

const char* float_from_str (x_float dest, const char* src)
 {
   byte resultSign = 0;
   byte digits = 0;
   byte realDigit = 0;
   byte exponentSign = 0;
   small resultExponent = -1;
   small exponentValue = 0;

   set_to_special(dest, 0);
   if (*src == '-')
    {
      resultSign = 0x80;
      ++src;
    }
   while ((*src >= '0') && (*src <= '9'))
    {
      if (digits < CUTOFF)
       {
         if (realDigit || ('0' != *src))
          {
            add_lshift(&dest[1]);
            dest[5] |= *src - '0';
            realDigit = 1;
            ++digits;
          }
       }
      if (realDigit)
       {
         ++resultExponent;
       }
      ++src;
    }
   if (('.' == *src) || (',' == *src))
    {
      ++src;
    }
   while ((*src >= '0') && (*src <= '9'))
    {
      if (digits < CUTOFF)
       {
         if (realDigit || ('0' != *src))
          {
            add_lshift(&dest[1]);
            dest[5] |= *src - '0';
            realDigit = 1;
            ++digits;
          }
       }
      if (!realDigit)
       {
         --resultExponent;
       }
      ++src;
    }
   while (digits < CUTOFF)
    {
      add_lshift(&dest[1]);
      ++digits;
    }
   if (('e' == *src) || ('E' == *src))
    {
      ++src;
      if ('-' == *src)
       {
         exponentSign = 1;
         ++src;
       }
      if ('+' == *src)
       {
         ++src;
       }
      if ((*src >= '0') && (*src <= '9'))
       {
         exponentValue = *src - '0';
         ++src;
       }
      if ((*src >= '0') && (*src <= '9'))
       {
         exponentValue = (((exponentValue << 2) + exponentValue) << 1) + *src - '0';
         ++src;
       }
      if ((*src >= '0') && (*src <= '9'))
       {
         exponentValue = MAX_EXP + (MAX_EXP >> 1);
       }
      if (exponentSign)
       {
         resultExponent = resultExponent - exponentValue;
       }
      else
       {
         resultExponent = resultExponent + exponentValue;
       }
    }

   if (add_iszero(&dest[1]))
    {
      dest[0] = 0x80;
    }
   else if (resultExponent < -MAX_EXP)
    {
      set_to_special(dest, 0);
    }
   else if (resultExponent > MAX_EXP)
    {
      set_to_special(dest, 0x80);
    }
   else
    {
      dest[0] = (byte)resultExponent;
      dest[1] = resultSign;
    }

   return src;
 }

void float_to_str (char dest [15], const x_float src)
 {
   x_float_impl* SRC = (x_float_impl*)src;
   byte trailing_zeros = 0;
   byte place = 5;
   small mover;
   memset(dest, '\0', 15U);
      // Remove special cases.
   if (src[0] == 0x80)
    {
      if (src[1] == 0x80)
       {
         strcpy(dest, "Err");
       }
      else
       {
         strcpy(dest, "0.00");
       }
      return;
    }
   if (0U == src[2]) // Prevent a hang by erroring out here.
    {
      strcpy(dest, "Bug");
      return;
    }
      // Determine the number of trailing zeros to remove.
   for (;;)
    {
      if (0 == (src[place] & 0xF))
       {
         ++trailing_zeros;
         if (0 == (src[place] & 0xF0))
          {
            ++trailing_zeros;
          }
         else
          {
            break;
          }
       }
      else
       {
         break;
       }
      --place;
    }
   place = 0;
   if (src[1] == 0x80)
    {
      dest[0] = '-';
      ++place;
    }
   if (SRC->exponent > 7)
    {
      place = placeDigits(dest, src, CUTOFF - trailing_zeros, place, 1);
      placeExponent(dest, SRC->exponent, place);
    }
   else if (SRC->exponent == 7)
    {
      placeDigits(dest, src, CUTOFF, place, 0);
    }
   else if (SRC->exponent == 6)
    {
      place = placeDigits(dest, src, 7, place, 0);
      if (0 == trailing_zeros)
       {
         dest[place] = '.';
         ++place;
         dest[place] = (src[5] & 0xF) + '0';
       }
    }
   else if (SRC->exponent > -1)
    {
      trailing_zeros = CUTOFF - trailing_zeros;
      if (trailing_zeros < (SRC->exponent + 3))
       {
         trailing_zeros = SRC->exponent + 3;
       }
      placeDigits(dest, src, trailing_zeros, place, 1);
      ++place;
      mover = SRC->exponent;
      while (mover > 0)
       {
         dest[place] = dest[place + 1];
         ++place;
         --mover;
       }
      dest[place] = '.';
    }
   else if (SRC->exponent == -1)
    {
      dest[place] = '0';
      ++place;
      dest[place] = '.';
      ++place;
      trailing_zeros = CUTOFF - trailing_zeros;
      if (trailing_zeros < 2)
       {
         trailing_zeros = 2;
       }
      placeDigits(dest, src, trailing_zeros, place, 0);
    }
   else if ((SRC->exponent + trailing_zeros) > -5)
    {
      dest[place] = '0';
      ++place;
      dest[place] = '.';
      ++place;
      mover = SRC->exponent + 1;
      while (mover < 0)
       {
         dest[place] = '0';
         ++place;
         ++mover;
       }
      placeDigits(dest, src, CUTOFF - trailing_zeros, place, 0);
    }
   else
    {
      place = placeDigits(dest, src, CUTOFF - trailing_zeros, place, 1);
      placeExponent(dest, SRC->exponent, place);
    }
 }
