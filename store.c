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

#define NUM_ROWS 100U
#define NUM_COLS  20U

   // This isn't even my final form!
static struct CELL_ENTRY store [ NUM_ROWS * NUM_COLS ];
static const centry BEGIN = store;
static const centry END = store + NUM_ROWS * NUM_COLS;

void initStore (void)
 {
   centry iter;
   for (iter = BEGIN; iter != END; ++iter)
    {
      iter->use = CELL_UNUSED;
    }
 }

const char* convertName (const char* src, byte* col, byte* row)
 {
   if ((*src >= 'A') && (*src <= 'T'))
    {
      *col = *src - 'A';
    }
   else if ((*src >= 'a') && (*src <= 't'))
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
//   int temp = (int)row;
//   temp = ((temp << 2U) + temp) << 2U;
//   return &store[ temp + ((int)col) ];
   return &store[ ((int)row) * NUM_COLS + ((int)col) ];
 }

x_float* lookupCellValue (byte col, byte row)
 {
   centry temp = lookupCell(col, row);
   if (CELL_USE_VALUE == temp->use)
    {
      return &(temp->prev);
    }
   else
    {
      return NULL;
    }
 }

char* getCellString (byte col, byte row)
 {
   int location;
   location = ((int)row) * NUM_COLS + ((int)col);
   setStorePage(location >> 6);
   return getStore(location & 63);
 }

void interlinked (byte col, byte row)
 {
   centry cell; // Do they keep you in a cell?
   cell = lookupCell(col, row); // Cells
   if (CELL_USE_VALUE == cell->use) // What's it like to hold the hand of someone you love?
    {
      shuntingYard(cell->prev, getCellString(col, row), 0); // Interlinked
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
