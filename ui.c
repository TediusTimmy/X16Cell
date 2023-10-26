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
#include "ui.h"
#include "store.h"

#include <string.h>
#include <stdio.h>

#define NUM_ROWS 100U
#define NUM_COLS  26U

static byte c_col;
static byte c_row;
static byte tr_col;
static byte tr_row;
static byte widths[NUM_COLS];

static byte c_major;
static byte top_down;
static byte left_right;

static byte inputMode;
static byte useComma;

static char tempFloat [15];
static char working [CELL_STR_LEN];
static byte lChar, inLoc, totLen;

static byte redrawEverything;
static byte l_col;
static byte l_row;

static byte supressRecalc;

static void setComma(char* str)
 {
   char* c;
   if (0U != useComma)
    {
      c = strchr(str, '.');
      while (NULL != c)
       {
         *c = ',';
         c = strchr(c, '.');
       }
    }
 }

void initializeScreen (void)
 {
   byte i;

   c_col = 0U;
   c_row = 0U;
   tr_col = 0U;
   tr_row = 0U;

   c_major = 0U;
   top_down = 1U;
   left_right = 1U;

   inputMode = 0U;
   useComma = 0U;

   redrawEverything = 1U;
   supressRecalc = 0U;

   for (i = 0; i < NUM_COLS; ++i)
    {
      widths[i] = 9U;
    }

   platformInitializeScreen();
   initStore();
 }

void updateScreen (void)
 {
   byte x, y, mx, my, q, t, r, i, c, j;
   centry cell;

   platformScreensize(&x, &y);
   mx = 0U;
   my = 1U;

   platformGotoxy(0, 0);
   cell = lookupCell(c_col, c_row);

      // Line 1 : Info and current cell's contents.
   platformColors(COLOR_HEADER);
   platformPutch('A' + c_col);
   t = 0U;
   q = 0U;
   while (t <= c_row)
    {
      t += 10;
      ++q;
    }
   --q;
   r = c_row - t + 10;
   if (0 != q) platformPutch('0' + q);
   platformPutch('0' + r);
   if (0 == q) platformPutch(' ');
   platformPutch(' ');
   if (CELL_UNUSED != cell->use)
    {
      if (CELL_USE_LABEL == cell->use)
       {
         platformPuts("LABEL ");
         strcpy(working, getCellString(c_col, c_row));
         if (strlen(working) > x - 13U) working[x - 13U] = '\0';
         platformPuts(working);
         for (q = x - 13U - strlen(working); q != (byte)(0U - 1U); --q) platformPutch(' ');
       }
      else
       {
         platformPuts("VALUE ");
         float_to_str(tempFloat, *lookupCellValue(c_col, c_row));
         setComma(tempFloat);
         platformPuts(tempFloat);
         for (q = x - 13U - strlen(tempFloat); q != (byte)(0U - 1U); --q) platformPutch(' ');
       }
    }
   else
    {
      for (q = x - 7U; q != (byte)(0U - 1U); --q) platformPutch(' ');
    }
   if (c_major)
    {
      platformPutch(top_down ? 'T' : 'B');
      platformPutch(left_right ? 'L' : 'R');
    }
   else
    {
      platformPutch(left_right ? 'L' : 'R');
      platformPutch(top_down ? 'T' : 'B');
    }

      // Line 2 : Edit line
   cell = lookupCell(c_col, c_row);
   platformColors(COLOR_NORMAL);
   if (CELL_UNUSED != cell->use)
    {
      strcpy(working, getCellString(c_col, c_row) + lChar);
      mx = strlen(working);
      if (mx > x) working[x] = '\0';
      platformPuts(working);
      mx = strlen(working);
      for (q = x - mx - 1U; q != (byte)(0U - 1U); --q) platformPutch(' ');
      mx = inLoc - lChar;
    }
   else
    {
      for (i = 0; i < x; ++i) platformPutch(' ');
    }

      // Line 3 : Column Headers
   platformColors(COLOR_HEADER);
   platformPutch(' ');
   platformPutch(' ');
   q = 2U;
   r = tr_col;
   for (;r < NUM_COLS;)
    {
      t = widths[r];
      if (q + t <= x)
       {
         if ((1U == redrawEverything) || (r == c_col) || (r == l_col))
          {
            if (0U == redrawEverything) platformGotoxy(q, 2);
            if (r == c_col) platformColors(COLOR_HIGHLIGHT);
            q += t;
            --t;
            for (i = 0; i < (t >> 1); ++i) platformPutch(' ');
            platformPutch('A' + r);
            for (i = 0; i < (t >> 1); ++i) platformPutch(' ');
            if (t & 1) platformPutch(' ');
            if (r == c_col) platformColors(COLOR_HEADER);
          }
         else
          {
            q += t;
          }
       }
      else
       {
         break;
       }
      ++r;
    }
   if (1U == redrawEverything)
    {
      platformColors(COLOR_HIGHLIGHT);
      for (; q < x; ++q) platformPutch(' ');
    }

      // All other lines:
   for (i = 3U; i < y; ++i)
    {
      r = tr_row + i - 3U;
      if ((1U == redrawEverything) || (r == c_row) || (r == l_row))
       {
         if (0U == redrawEverything) platformGotoxy(0, i);
         if (r == c_row) platformColors(COLOR_HIGHLIGHT);
         else platformColors(COLOR_HEADER);
         t = 0U;
         q = 0U;
         while (t <= r)
          {
            t += 10;
            ++q;
          }
         --q;
         if (0U == q) platformPutch(' ');
         else platformPutch('0' + q);
         platformPutch('0' + r - t + 10U);
       }

      q = 2U;
      c = tr_col;
      for (;c < NUM_COLS;)
       {
         t = widths[c];
         if (q + t <= x)
          {
            if ((1U == redrawEverything) || ((c == c_col) && (r == c_row)) || ((c == l_col) && (r == l_row)))
             {
               if (0U == redrawEverything) platformGotoxy(q, i);
               if ((c == c_col) && (r == c_row))
                {
                  platformColors(COLOR_HEADER);
                  if (0 == inputMode)
                   {
                     mx = ((q << 1U) + t) >> 1;
                     my = i;
                   }
                }
               else
                {
                  platformColors(COLOR_NORMAL);
                }
               q += t;
               cell = lookupCell(c, r);
               if (CELL_UNUSED != cell->use)
                {
                  if (CELL_USE_LABEL == cell->use)
                   {
                     strcpy(working, getCellString(c, r));
                     if (strlen(working) > t) working[t] = '\0';
                     else
                      {
                        j = strlen(working);
                        memset(working + j, ' ', t - j);
                        working[t] = '\0';
                      }
                   }
                  else
                   {
                     float_to_str(tempFloat, *lookupCellValue(c, r));
                     setComma(tempFloat);
                     strcpy(working, tempFloat);
                     if (strlen(working) > t)
                      {
                        working[t] = '\0';
                        working[t - 1U] = '#';
                      }
                     else
                      {
                        j = strlen(working);
                        memmove(working + t - j, working, j + 1);
                        memset(working, ' ', t - j);
                      }
                   }
                  platformPuts(working);
                  for (j = t - strlen(working) - 1; j != (byte)(0U - 1U); --j) platformPutch(' ');
                }
               else
                {
                  for (j = 0; j < t; ++j) platformPutch(' ');
                }
             }
            else
             {
               q += t;
             }
          }
         else
          {
            break;
          }
         ++c;
       }
      if (1U == redrawEverything)
       {
         platformColors(COLOR_HIGHLIGHT);
         for (; q < x; ++q) platformPutch(' ');
       }
    }

   redrawEverything = 0U;
   platformGotoxy(mx, my);
 }

byte getNextCommand (void)
 {
   return platformGetch();
 }

static centry cell;
static word toSave;
static void swapAndClear(byte col, byte row)
 {
   cell = lookupCell(col, row);
   toSave = cell->loc;
   memmove(cell, cell + 1, sizeof(struct CELL_ENTRY) * (NUM_COLS - col - 1));
   cell += (NUM_COLS - col - 1);
   cell->loc = toSave;
   cell->use = CELL_UNUSED;
 }

void doSave (void);
void doLoad (void);

static char* string;
byte interpretCommand (byte command)
 {
   byte x, y, c, mx, mc, t;

   platformScreensize(&x, &y);

   if (1 == inputMode)
    {
      string = getCellString(c_col, c_row);
      if (((' ' <= command) && (']' >= command)) ||
#if 'z' > 'Z'
          (('a' <= command) && ('z' >= command)))
#else
          (('A' <= command) && ('Z' >= command)))
#endif
       {
            // A normal character to be inserted into the cell.
         if (totLen != CELL_MAX_LEN)
          {
            if (inLoc == totLen)
             {
               string[inLoc] = command;
               string[inLoc + 1U] = '\0';
             }
            else
             {
               memmove(&string[inLoc + 1], &string[inLoc], totLen - inLoc + 1U);
               string[inLoc] = command;
             }
            ++inLoc;
            ++totLen;

            if (inLoc > (x - 5U + lChar))
             {
               ++lChar;
             }
          }
       }
      else if (('\b' == command) || (0177 == command)) // Backspace
       {
         if (0U != inLoc)
          {
            if (inLoc == totLen)
             {
               string[inLoc - 1U] = '\0';
             }
            else
             {
               memmove(&string[inLoc - 1], &string[inLoc], totLen - inLoc + 1U);
             }
            --inLoc;
            --totLen;

            if ((inLoc + lChar) > totLen)
             {
               --lChar;
             }
          }
       }
      else if ((command == '\n') || (command == '\r')) // Enter
       {
         inputMode = 0;
         redrawEverything = 1U;
         if (0U == supressRecalc) recalculate(c_major, top_down, left_right);
       }
      else if (command == 157) // Cursor Left
       {
         if (0U != inLoc)
          {
            --inLoc;

            if ((inLoc == (lChar + 5U)) && (0U != lChar))
             {
               --lChar;
             }
          }
       }
      else if (command == 29) // Cursor Right
       {
         if (inLoc != totLen)
          {
            ++inLoc;

            if (inLoc > (x - 5U + lChar))
             {
               ++lChar;
             }
          }
       }
      else if (command == 19) // Home
       {
         inLoc = 0U;
         lChar = 0U;
       }
      else if ((command == 4) || (command == 148)) // Insert / End
       { // Insert on Commodore because the C64 keyboard doesn't have End
         inLoc = totLen;
         if (inLoc > (x - 5U))
          {
            lChar = inLoc - x + 5U;
          }
         else
          {
            lChar = 0U;
          }
       }
      else if (command == 25) // Delete
       {
         if (inLoc != totLen)
          {
            memmove(&string[inLoc], &string[inLoc + 1], totLen - inLoc + 1U);
            --totLen;

            if ((inLoc + lChar) > totLen)
             {
               --lChar;
             }
          }
       }
      return 0;
    }

   c = tr_col;
   mx = 2U;
   mc = 0;
   for (;c < NUM_COLS;)
    {
      t = widths[c];
      if (mx + t <= x)
       {
         mx += t;
         ++mc;
       }
      else
       {
         break;
       }
      ++c;
    }
   cell = lookupCell(c_col, c_row);

   switch (command)
    {
   case '\'':
   case '"':
      cell->use = CELL_USE_LABEL;
      *getCellString(c_col, c_row) = '\0';
      lChar = 0;
      inLoc = 0;
      totLen = 0;
      inputMode = 1;
      break;
   case '=':
   case '+':
      cell->use = CELL_USE_VALUE;
      string = getCellString(c_col, c_row);
      *string = '\0';
      lChar = 0;
      inLoc = 0;
      totLen = 0;
      inputMode = 1;
      string += CELL_STR_LEN;
      memset(string, '\0', 6);
      *string = -128;
      break;
   case 'E':
   case 'e':
      if (CELL_UNUSED != cell->use)
       {
         inputMode = 1;
         inLoc = strlen(getCellString(c_col, c_row));
         totLen = inLoc;
         if (inLoc > (x - 5U))
          {
            lChar = inLoc - x + 5U;
          }
         else
          {
            lChar = 0U;
          }
       }
      break;
   case 'W':
   case 'w':
   case 145:
      if (0U != c_row)
       {
         l_row = c_row;
         l_col = c_col;
         --c_row;
         if (c_row < tr_row)
          {
            redrawEverything = 1U;
            --tr_row;
          }
       }
      break;
   case 'S':
   case 's':
   case 17:
      if ((NUM_ROWS - 1U) != c_row)
       {
         l_row = c_row;
         l_col = c_col;
         ++c_row;
         if ((c_row - tr_row) >= (y - 3))
          {
            redrawEverything = 1U;
            ++tr_row;
          }
       }
      break;
   case 'A':
   case 'a':
   case 157:
      if (0U != c_col)
       {
         l_row = c_row;
         l_col = c_col;
         --c_col;
         if (c_col < tr_col)
          {
            redrawEverything = 1U;
            --tr_col;
          }
       }
      break;
   case 'D':
   case 'd':
   case 29:
      if ((NUM_COLS - 1U) != c_col)
       {
         l_row = c_row;
         l_col = c_col;
         ++c_col;
         if ((c_col - tr_col) >= mc)
          {
            redrawEverything = 1U;
            ++tr_col;
          }
       }
      break;
   case ']':
      if (widths[c_col] != 30)
       {
         redrawEverything = 1U;
         ++widths[c_col];
         if (((mx + 1) > x) && (c_col == (tr_col + mc - 1U))) ++tr_col;
       }
      break;
   case '[':
      if (widths[c_col] != 1)
       {
         redrawEverything = 1U;
         --widths[c_col];
       }
      break;
   case '!':
      redrawEverything = 1U;
      recalculate(c_major, top_down, left_right);
      break;
   case 'J':
   case 'j':
      c_major ^= 1U;
      break;
   case 'K':
   case 'k':
      top_down ^= 1U;
      break;
   case 'L':
   case 'l':
      left_right ^= 1U;
      break;
   case 'X':
   case 'x':
      t = platformGetch();
      switch (t)
       {
      case 'X':
      case 'x':
         cell->use = CELL_UNUSED;
         break;
      case 'R':
      case 'r':
         for (c = 0; c < NUM_COLS; ++c)
          {
            cell = lookupCell(c, c_row);
            cell->use = CELL_UNUSED;
          }
         redrawEverything = 1U;
         break;
      case 'C':
      case 'c':
         for (c = 0; c < NUM_ROWS; ++c)
          {
            cell = lookupCell(c_col, c);
            cell->use = CELL_UNUSED;
          }
         redrawEverything = 1U;
         break;
       }
      recalculate(c_major, top_down, left_right);
      break;
   case ',':
      useComma ^= 1U;
      break;
   case 'Z':
   case 'z':
   case 19:
      redrawEverything = 1U;
      c_col = 0U;
      c_row = 0U;
      tr_col = 0U;
      tr_row = 0U;
      break;
   case 'Q':
   case 'q':
      t = platformGetch();
      if (('y' == t) || ('Y' == t))
       {
         return 1;
       }
      break;
   case 'n':
   case 'N':
      string = getCellString(c_col, c_row);
      strcpy(working, string);
      doSave();
      string = getCellString(c_col, c_row);
      if ('\0' != *working)
       {
         strcpy(string, working);
       }
      break;
   case 'm':
   case 'M':
      string = getCellString(c_col, c_row);
      strcpy(working, string);
      doLoad();
      string = getCellString(c_col, c_row);
      if ('\0' != *working)
       {
         strcpy(string, working);
       }
      recalculate(c_major, top_down, left_right);
      break;
   case 'U':
   case 'u':
      t = platformGetch();
      switch (t)
       {
      case 'U':
      case 'u':
         swapAndClear(c_col, c_row);
         break;
      case 'O':
      case 'o':
         toSave = cell->loc;
         for (c = c_row; c < (NUM_ROWS - 1); ++c)
          {
            cell->use = (cell + NUM_COLS)->use;
            cell->loc = (cell + NUM_COLS)->loc;
            cell += NUM_COLS;
          }
         cell->loc = toSave;
         cell->use = CELL_UNUSED;
         break;
      case 'R':
      case 'r':
         cell = lookupCell(0U, c_row);
         memcpy(cell + (NUM_COLS * (NUM_ROWS - c_row)), cell, sizeof(struct CELL_ENTRY) * NUM_COLS);
         memmove(cell, cell + NUM_COLS, sizeof(struct CELL_ENTRY) * NUM_COLS * (NUM_ROWS - c_row));
         cell += (NUM_COLS * (NUM_ROWS - c_row - 1));
         for (c = 0; c < NUM_COLS; ++c)
          {
            cell->use = CELL_UNUSED;
            ++cell;
          }
         break;
      case 'C':
      case 'c':
         for (c = 0; c < NUM_ROWS; ++c)
          {
            swapAndClear(c_col, c);
          }
         break;
       }
      redrawEverything = 1U;
      recalculate(c_major, top_down, left_right);
      break;
    }
   return 0;
 }

void closeScreen (void)
 {
   platformCloseScreen();
 }

FILE* sl_file;
void doSave (void)
 {
   byte row, col;
   sl_file = fopen(working, "wb");
   if (NULL == sl_file)
    {
      strcpy(working, "Failed to open file.");
      return;
    }
   fputc('z', sl_file); // Go to top-left.
   for (row = 0; row < NUM_ROWS;)
    {
      for (col = 0; col < NUM_COLS; ++col)
       {
         cell = lookupCell(col, row);
         if (CELL_UNUSED != cell->use)
          {
            if (CELL_USE_LABEL == cell->use)
             {
               fputc('"', sl_file);
             }
            else if (CELL_USE_VALUE == cell->use)
             {
               fputc('=', sl_file);
             }
            fputs(getCellString(col, row), sl_file);
            fputc('\n', sl_file);
          }
         fputc('d', sl_file);
       }
      fputc('s', sl_file);
      ++row;
      for (col = NUM_COLS - 1U; col != (byte)(0U - 1U); --col)
       {
         cell = lookupCell(col, row);
         if (CELL_UNUSED != cell->use)
          {
            if (CELL_USE_LABEL == cell->use)
             {
               fputc('"', sl_file);
             }
            else if (CELL_USE_VALUE == cell->use)
             {
               fputc('=', sl_file);
             }
            fputs(getCellString(col, row), sl_file);
            fputc('\n', sl_file);
          }
         fputc('a', sl_file);
       }
      fputc('s', sl_file);
      ++row;
    }
   fputc('z', sl_file);
   if (ferror(sl_file))
    {
      strcpy(working, "Error during file load.");
    }
   else
    {
      *working = '\0';
    }
   fclose(sl_file);
 }

void doLoad (void)
 {
   byte ch;
   sl_file = fopen(working, "rb");
   if (NULL == sl_file)
    {
      strcpy(working, "Failed to open file.");
      return;
    }
   supressRecalc = 1U;
   ch = fgetc(sl_file);
   while (!feof(sl_file))
    {
      interpretCommand(ch);
      ch = fgetc(sl_file);
    }
   supressRecalc = 0U;
   if (ferror(sl_file))
    {
      strcpy(working, "Error during file load.");
    }
   else
    {
      *working = '\0';
    }
   fclose(sl_file);
 }
