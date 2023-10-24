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
#include "shunting.h"

#include <stddef.h>
#include <string.h>

#define NUM_ROWS 100U
#define NUM_COLS  26U

static centry BEGIN;
static centry END;

void initStore (void)
 {
   centry iter;
   int loc;
   setStorePageReserved(0U);
   BEGIN = (centry) getStore(0U);
   END = BEGIN + NUM_ROWS * NUM_COLS;
   loc = 0;
   for (iter = BEGIN; iter != END; ++iter)
    {
      iter->use = CELL_UNUSED;
      iter->loc = loc;
      ++loc;
    }
 }

const char* convertName (const char* src, byte* col, byte* row)
 {
   if ((*src >= 'A') && (*src <= 'Z'))
    {
      *col = *src - 'A';
    }
   else if ((*src >= 'a') && (*src <= 'z'))
    {
      *col = *src - 'a';
    }
   else
    {
      return NULL;
    }
   ++src;
   if ((*src >= '0') && (*src <= '9'))
    {
      *row = *src - '0';
    }
   else
    {
      return NULL;
    }
   ++src;
   if ((*src >= '0') && (*src <= '9'))
    {
      *row = (((*row << 2U) + *row) << 1U) + *src - '0';
    }
   else
    {
      return src;
    }
   ++src;
   if ((*src >= '0') && (*src <= '9'))
    {
      return NULL;
    }
   return src;
 }

centry lookupCell (byte col, byte row)
 {
   setStorePageReserved(0U);
   return &BEGIN[ ((int)row) * NUM_COLS + ((int)col) ];
 }

x_float* lookupCellValue (byte col, byte row)
 {
   centry temp = lookupCell(col, row);
   word loc = temp->loc;
   if (CELL_USE_VALUE == temp->use)
    {
      setStorePage(loc >> 6);
      return (x_float*)(getStore(loc & 63) + CELL_STR_LEN);
    }
   else
    {
      return NULL;
    }
 }

char* getCellString (byte col, byte row)
 {
   centry temp = lookupCell(col, row);
   word loc = temp->loc;
   setStorePage(loc >> 6);
   return getStore(loc & 63);
 }

char max_entry [CELL_STR_LEN];
x_float ret_val;
void interlinked (byte col, byte row)
 {
   centry cell; // Do they keep you in a cell?
   cell = lookupCell(col, row); // Cells
   if (CELL_USE_VALUE == cell->use) // What's it like to hold the hand of someone you love?
    {
      strcpy(max_entry, getCellString(col, row));
      shuntingYard(ret_val, max_entry, 0); // Interlinked
      float_cpy(*lookupCellValue(col, row), ret_val); // Should NEVER be NULL.
    }
 }

void recalculate (byte major, byte td, byte lr)
 {
   byte row, col;
   if (major) // Going in column-major order
    {
      if (lr) // Going from left-to-right
       {
         if (td) // Going from top-to-bottom
          {
            for (col = 0U; col < NUM_COLS; ++col)
             {
               for (row = 0U; row < NUM_ROWS; ++row)
                {
                  interlinked(col, row);
                }
             }
          }
         else // Going from bottom-to-top
          {
            for (col = 0U; col < NUM_COLS; ++col)
             {
               for (row = NUM_ROWS - 1U; row != (byte)(0U - 1U); --row)
                {
                  interlinked(col, row);
                }
             }
          }
       }
      else // Going from right-to-left
       {
         if (td) // Going from top-to-bottom
          {
            for (col = NUM_COLS - 1U; col != (byte)(0U - 1U); --col)
             {
               for (row = 0U; row < NUM_ROWS; ++row)
                {
                  interlinked(col, row);
                }
             }
          }
         else // Going from bottom-to-top
          {
            for (col = NUM_COLS - 1U; col != (byte)(0U - 1U); --col)
             {
               for (row = NUM_ROWS - 1U; row != (byte)(0U - 1U); --row)
                {
                  interlinked(col, row);
                }
             }
          }
       }
    }
   else // Going in row major order
    {
      if (td) // Going from top-to-bottom
       {
         if (lr) // Going from left-to-right
          {
            for (row = 0U; row < NUM_ROWS; ++row)
             {
               for (col = 0U; col < NUM_COLS; ++col)
                {
                  interlinked(col, row);
                }
             }
          }
         else // Going from right-to-left
          {
            for (row = NUM_ROWS - 1U; row != (byte)(0U - 1U); --row)
             {
               for (col = 0U; col < NUM_COLS; ++col)
                {
                  interlinked(col, row);
                }
             }
          }
       }
      else // Going from bottom-to-top
       {
         if (lr) // Going from left-to-right
          {
            for (row = 0U; row < NUM_ROWS; ++row)
             {
               for (col = NUM_COLS - 1U; col != (byte)(0U - 1U); --col)
                {
                  interlinked(col, row);
                }
             }
          }
         else // Going from right-to-left
          {
            for (row = NUM_ROWS - 1U; row != (byte)(0U - 1U); --row)
             {
               for (col = NUM_COLS - 1U; col != (byte)(0U - 1U); --col)
                {
                  interlinked(col, row);
                }
             }
          }
       }
    }
 }
