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

#include <string.h>

static x_float sy_stack [16];
static byte sy_op_stack [32];
static x_float* sy_dsp = sy_stack;
static byte sy_osp = 0;

#define NO_OP  0
#define ADD_OP 1
#define SUB_OP 2
#define MUL_OP 3
#define DIV_OP 4
#define NEG_OP 5
#define PARENS 6
static const byte PREC [] = "\1\2\2\3\3\4\1";

static byte pushOp (byte op)
 {
   if (31 != sy_osp)
    {
      ++sy_osp;
      sy_op_stack[sy_osp] = op;
    }
   else
    {
      return 1U;
    }
   return 0U;
 }

static byte pushData (void)
 {
   if (sy_dsp != (sy_stack + 15))
    {
      ++sy_dsp;
    }
   else
    {
      return 1;
    }
   return 0;
 }

static byte popData (void)
 {
   if (sy_dsp != sy_stack)
    {
      --sy_dsp;
    }
   else
    {
      return 1;
    }
   return 0;
 }

static byte doAnOp (void)
 {
   byte fail = 0;
   switch(sy_op_stack[sy_osp])
    {
   case ADD_OP:
      fail = popData();
      if (0 == fail)
       {
         float_add(*sy_dsp, *sy_dsp, sy_dsp[1]);
       }
      break;
   case SUB_OP:
      fail = popData();
      if (0 == fail)
       {
         float_neg(sy_dsp[1]);
         float_add(*sy_dsp, *sy_dsp, sy_dsp[1]);
       }
      break;
   case MUL_OP:
      fail = popData();
      if (0 == fail)
       {
         float_mul(*sy_dsp, *sy_dsp, sy_dsp[1]);
       }
      break;
   case DIV_OP:
      fail = popData();
      if (0 == fail)
       {
         float_div(*sy_dsp, *sy_dsp, sy_dsp[1]);
       }
      break;
   case NEG_OP:
      float_neg(*sy_dsp);
      break;
    }
   --sy_osp;
   return fail;
 }

struct FUNTABLE
 {
   const char* name;
   byte length;
   byte number;
 };

   // Either convert them to an enum or this.
#define FUN_BAD 0U
#define FUN_MIN 1U
#define FUN_MAX 2U
#define FUN_SUM 3U
#define FUN_AVG 4U
#define FUN_CNT 5U
#define FUN_TRU 6U
#define FUN_ROU 7U
#define FUN_ABS 8U

static const struct FUNTABLE funTable [] =
 {
   {"min",     3U, FUN_MIN},
   {"max",     3U, FUN_MAX},
   {"sum",     3U, FUN_SUM},
   {"average", 7U, FUN_AVG},
   {"count",   5U, FUN_CNT},
   {"trunc",   5U, FUN_TRU},
   {"round",   5U, FUN_ROU},
   {"abs",     3U, FUN_ABS},
   {NULL,      0U, FUN_BAD}
 };

byte function_min (x_float dest, const char** src);
byte function_max (x_float dest, const char** src);
byte function_sum (x_float dest, const char** src);
byte function_avg (x_float dest, const char** src);
byte function_cnt (x_float dest, const char** src);
byte function_trunc (x_float dest, const char** src);
byte function_round (x_float dest, const char** src);
byte function_abs (x_float dest, const char** src);

static const char* eatWhitespace(const char* src)
 {
   while (' ' == *src)
    {
      ++src;
    }
   return src;
 }

static byte getFunction(const char** src)
 {
   byte iter;
   byte toReturn = FUN_BAD;
   const char* temp = *src;
   temp = eatWhitespace(temp);
   for (iter = 0; NULL != funTable[iter].name; ++iter)
    {
      if (0 == strncasecmp(temp, funTable[iter].name, funTable[iter].length))
       {
         temp += funTable[iter].length;
         toReturn = funTable[iter].number;

         temp = eatWhitespace(temp);
         break;
       }
    }
   *src = temp;
   return toReturn;
 }

static byte doFunction(byte next_op, const char** src)
 {
   byte result = 0;
   switch(next_op)
    {
   case FUN_MIN:
      result = function_min(*sy_dsp, src);
      break;
   case FUN_MAX:
      result = function_max(*sy_dsp, src);
      break;
   case FUN_SUM:
      result = function_sum(*sy_dsp, src);
      break;
   case FUN_CNT:
      result = function_cnt(*sy_dsp, src);
      break;
   case FUN_AVG:
      result = function_avg(*sy_dsp, src);
      break;
   case FUN_TRU:
      result = function_trunc(*sy_dsp, src);
      break;
   case FUN_ROU:
      result = function_round(*sy_dsp, src);
      break;
   case FUN_ABS:
      result = function_abs(*sy_dsp, src);
      break;
   default:
      result = 1;
      break;
    }
   return result;
 }

const char* shuntingYard (x_float dest, const char* src, byte inAFunk)
 {
   x_float* d_entered = sy_dsp;
   byte o_entered = sy_osp;
   byte last = 0;
   byte done = 0;
   byte fail = 0;
   byte next_op;
   byte col, row;
   x_float* stored;

   for (; (0 == done) && (0 == fail);)
    {
      next_op = NO_OP;
      switch (*src)
       {
      case '+':
         next_op = ADD_OP;
         break;
      case '-':
         if (0 == last) // if the last thing was not number-like, then it was operator-like and this is negation
          {
            next_op = NEG_OP;
          }
         else // else it was number-like and this is subtraction
          {
            next_op = SUB_OP;
          }
         break;
      case '*':
         next_op = MUL_OP;
         break;
      case '/':
         next_op = DIV_OP;
         break;
      case '(':
         if (0 == last) // if the last thing was operator-like
          {
            ++src;
            fail |= pushOp(PARENS);
          }
         else // else we have a number after number
          {
            fail = 1;
          }
         break;
      case ')':
         last = 1;
         while ((sy_osp != o_entered) && (sy_op_stack[sy_osp] != PARENS) && (0 == fail))
          {
            fail |= doAnOp();
          }
         if ((sy_osp != o_entered) && (0 == fail) && (sy_op_stack[sy_osp] == PARENS))
          {
            --sy_osp;
            ++src; // inAFunk needs to return the parens
          }
         else
          {
            if (0 == inAFunk) // If we are in a function, an unmatched parenthesis means we need to leave the function.
             {
               fail = 1;
             }
            else
             {
               done = 1;
             }
          }
         break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '.':
      case ',':
         fail |= pushData(); // Just increments sy_dsp and checks for error.
         if (0 == fail)
          {
            src = float_from_str(*sy_dsp, src);
            if (1 == last)
             {
               fail = 1; // Number after number
             }
            else
             {
               last = 1;
             }
          }
         break;
      case ' ':
      case '\n':
         ++src;
         break;
      case '\0':
         done = 1;
         break;
      case ';':
         if (0 != inAFunk)
          {
            done = 1;
          }
         else
          {
            fail = 1;
          }
         break;
      case '@':
         fail |= pushData(); // Just increments sy_dsp and checks for error.
         if (0 == fail)
          {
            ++src;
            last = 1;
            next_op = getFunction(&src);
            fail |= doFunction(next_op, &src);
            next_op = NO_OP;
          }
         break;
      default:
         fail |= pushData(); // Just increments sy_dsp and checks for error.
         if (0 == fail)
          {
            last = 1;
            src = convertName(src, &col, &row);
            if (NULL != src)
             {
               stored = lookupCellValue(col, row);
               if (NULL != stored)
                {
                  float_cpy(*sy_dsp, *stored);
                }
               else
                {
                  memset(*sy_dsp, '\0', 6);
                  *sy_dsp[0] = 0x80;
                }
             }
            else
             {
               fail = 1;
             }
          }
         break;
       }

      if (NO_OP != next_op)
       {
         ++src;
         last = 0;

         while (sy_osp != o_entered)
          {
            if (NEG_OP != next_op) // Only right-to-left associative operator
             {
               if (PREC[next_op] > PREC[sy_op_stack[sy_osp]])
                {
                  break;
                }
             }
            else
             {
               if (PREC[next_op] >= PREC[sy_op_stack[sy_osp]])
                {
                  break;
                }
             }

            fail |= doAnOp();
          }
         fail |= pushOp(next_op);
       }
    }

   while ((sy_osp != o_entered) && (0 == fail))
    {
      fail |= doAnOp();
    }

   if (sy_dsp == d_entered)
    {
      memset(dest, '\0', 6);
      dest[0] = 0x80;
      dest[1] = 0x80;
    }
   else
    {
      float_cpy(dest, *sy_dsp);
      --sy_dsp;
    }

      // Check for a bad parse.
   if ((sy_dsp != d_entered) || (sy_osp != o_entered) || (0 != fail))
    {
      sy_dsp = d_entered;
      sy_osp = o_entered;
      memset(dest, '\0', 6);
      dest[0] = 0x80;
      dest[1] = 0x80;
      src = NULL;
    }

   return src;
 }

byte generic_one_arg (x_float dest, const char** src, void(*function)(x_float))
 {
   byte fail = 0;
   const char* temp = *src;
   if ('(' == *temp)
    {
      ++temp;
      temp = shuntingYard(dest, temp, 1);
      if ((NULL != temp) && (')' == *temp))
       {
         ++temp;
         *src = temp;
         function(dest);
       }
      else
       {
         fail = 1;
       }
    }
   else
    {
      fail = 1;
    }
   return fail;
 }

byte function_trunc (x_float dest, const char** src)
 {
   return generic_one_arg(dest, src, float_trunc);
 }

byte function_round (x_float dest, const char** src)
 {
   return generic_one_arg(dest, src, float_round);
 }

byte function_abs (x_float dest, const char** src)
 {
   return generic_one_arg(dest, src, float_abs);
 }

static byte isARange (const char* src)
 {
   byte hasColon = 0;
   byte count = 0;
   byte end = 0;
   for (; 0 == end; ++src)
    {
      switch (*src)
       {
      case '(':
         ++count;
         break;
      case ')':
         if (0 == count)
          {
            end = 1;
          }
         else
          {
            --count;
          }
         break;
      case ';':
         if (0 == count)
          {
            end = 1;
          }
         break;
      case ':':
         if (0 == count)
          {
            hasColon = 1;
          }
         break;
      case '\0':
         end = 1;
         hasColon = 0;
         break;
       }
    }
   return hasColon;
 }

static byte col1, row1, col2, row2, swap;
static byte getRange(const char** src)
 {
   const char* temp = *src;
   temp = eatWhitespace(temp);
   temp = convertName(temp, &col1, &row1);
   if (NULL == temp)
    {
      return 1;
    }
   temp = eatWhitespace(temp);
   if (':' == *temp)
    {
      ++temp;
    }
   else
    {
      return 1;
    }
   temp = eatWhitespace(temp);
   temp = convertName(temp, &col2, &row2);
   if (NULL == temp)
    {
      return 1;
    }
   temp = eatWhitespace(temp);
   *src = temp;
   if (col1 > col2)
    {
      swap = col2;
      col2 = col1;
      col1 = swap;
    }
   if (row1 > row2)
    {
      swap = row2;
      row2 = row1;
      row1 = swap;
    }
   return 0;
 }

static x_float s_temp;
static x_float* lookupResult;
byte generic_min_max (x_float dest, const char** src, void(*function)(x_float, const x_float, const x_float))
 {
   byte first = 0;
   byte fail = 0;
   const char* temp = *src;
   byte c, r;
   if ('(' == *temp)
    {
      do
       {
         ++temp;
         if (0 == isARange(temp))
          {
            temp = shuntingYard(s_temp, temp, 1);
            if (NULL != temp)
             {
               if (0 == first)
                {
                  float_cpy(dest, s_temp);
                  first = 1;
                }
               else
                {
                  function(dest, s_temp, dest);
                }
             }
            else
             {
               fail = 1;
             }
          }
         else
          {
            fail |= getRange(&temp);
            if (0 == fail)
             {
               for (c = col1; c <= col2; ++c)
                {
                  for (r = row1; r <= row2; ++r)
                   {
                     lookupResult = lookupCellValue(c, r);
                     if (NULL != lookupResult)
                      {
                        if (0 == first)
                         {
                           float_cpy(dest, *lookupResult);
                           first = 1;
                         }
                        else
                         {
                           function(dest, *lookupResult, dest);
                         }
                      }
                   }
                }
             }
          }
       }
      while ((0 == fail) && (*temp == ';'));

      if ((0 == fail) && (*temp == ')'))
       {
         ++temp;
         *src = temp;
       }
      else
       {
         fail = 1;
       }
    }
   else
    {
      fail = 1;
    }
   return fail;
 }

byte function_min (x_float dest, const char** src)
 {
   return generic_min_max(dest, src, float_min);
 }

byte function_max (x_float dest, const char** src)
 {
   return generic_min_max(dest, src, float_max);
 }

static const byte* ONE = (const byte*)"\0\0\20\0\0\0\0";
byte generic_average (x_float sum, x_float count, const char** src)
 {
   byte fail = 0;
   const char* temp = *src;
   byte c, r;
   memset(sum, '\0', 6);
   sum[0] = 0x80;
   float_cpy(count, sum);
   if ('(' == *temp)
    {
      do
       {
         ++temp;
         if (0 == isARange(temp))
          {
            temp = shuntingYard(s_temp, temp, 1);
            if (NULL != temp)
             {
               float_add(sum, s_temp, sum);
               float_add(count, count, ONE);
             }
            else
             {
               fail = 1;
             }
          }
         else
          {
            fail |= getRange(&temp);
            if (0 == fail)
             {
               for (c = col1; c <= col2; ++c)
                {
                  for (r = row1; r <= row2; ++r)
                   {
                     lookupResult = lookupCellValue(c, r);
                     if (NULL != lookupResult)
                      {
                        float_add(sum, *lookupResult, sum);
                        float_add(count, count, ONE);
                      }
                   }
                }
             }
          }
       }
      while ((0 == fail) && (*temp == ';'));

      if ((0 == fail) && (*temp == ')'))
       {
         ++temp;
         *src = temp;
       }
      else
       {
         fail = 1;
       }
    }
   else
    {
      fail = 1;
    }
   return fail;
 }

byte function_sum (x_float dest, const char** src)
 {
   byte fail = 0;
   fail |= pushData();
   if (0 == fail)
    {
      fail |= generic_average(dest, *sy_dsp, src);
      fail |= popData();
    }
   return fail;
 }

byte function_avg (x_float dest, const char** src)
 {
   byte fail = 0;
   fail |= pushData();
   if (0 == fail)
    {
      fail |= generic_average(dest, *sy_dsp, src);
      if (0 == fail)
       {
         float_div(dest, dest, *sy_dsp);
       }
      fail |= popData();
    }
   return fail;
 }

byte function_cnt (x_float dest, const char** src)
 {
   byte fail = 0;
   fail |= pushData();
   if (0 == fail)
    {
      fail |= generic_average(dest, *sy_dsp, src);
      if (0 == fail)
       {
         float_cpy(dest, *sy_dsp);
       }
      fail |= popData();
    }
   return fail;
 }
