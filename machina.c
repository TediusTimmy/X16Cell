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
#include "machina.h"

#ifdef __GNUC__

static int storePage;
static char store [8192 * 63]; // The whole 512K available to the X16.

void setStorePage(byte pageNo)
 {
   storePage = pageNo;
   storePage <<= 13;
 }

char* getStore(byte indexInCurrentPage)
 {
   int location = indexInCurrentPage;
   location <<= 7;
   location |= storePage;
   return &store[location];
 }

#ifndef __MINGW32__

#include <curses.h>

void platformInitializeScreen (void)
 {
   initscr();
   start_color();

   cbreak();
   keypad(stdscr, TRUE);
   noecho();
   nonl();

   init_pair(COLOR_HEADER,    COLOR_BLACK, COLOR_WHITE);
   init_pair(COLOR_NORMAL,    COLOR_WHITE, COLOR_BLUE);
   init_pair(COLOR_HIGHLIGHT, COLOR_BLUE,  COLOR_BLACK);
 }

void platformScreensize (byte* x, byte* y)
 {
   int X, Y;
   getmaxyx(stdscr, Y, X);
   *x = X;
   *y = Y;
 }

void platformGotoxy (byte x, byte y)
 {
   move(y, x);
 }

void platformColors (byte pair)
 {
   attron(COLOR_PAIR(pair));
 }

byte platformGetch (void)
 {
      // I just noticed that I never call refresh in the main UpdateScreen loop.
      // It gets called implicitly here, because the window has been modified. The more you know.
      // I don't particularly like this, but I don't want to add another phony function to waste
      // RAM on the CX16.
   int ch = getch();
   switch (ch)
    {
   case KEY_BACKSPACE:
      ch = '\b';
      break;
   case KEY_UP:
      ch = 145;
      break;
   case KEY_DOWN:
      ch = 17;
      break;
   case KEY_LEFT:
      ch = 157;
      break;
   case KEY_RIGHT:
      ch = 29;
      break;
    }
   return ch;
 }

void platformPutch (byte ch)
 {
   addch(ch);
 }

void platformPuts (char* s)
 {
   printw(s);
 }

void platformCloseScreen (void)
 {
   endwin();
 }

#else /* __MINGW32__ */

#include "external/conio2.h"

void platformInitializeScreen (void)
 {
 }

void platformScreensize (byte* x, byte* y)
 {
   struct text_info info;
   gettextinfo(&info);
   *x = info.screenwidth;
   *y = info.screenheight;
 }

void platformGotoxy (byte x, byte y)
 {
   gotoxy(x + 1, y + 1);
 }

void platformColors (byte pair)
 {
   switch (pair)
    {
   case COLOR_HEADER:
      textcolor(BLACK);
      textbackground(LIGHTGRAY);
      break;
   case COLOR_NORMAL:
      textcolor(LIGHTGRAY);
      textbackground(BLUE);
      break;
   case COLOR_HIGHLIGHT:
      textcolor(BLUE);
      textbackground(BLACK);
      break;
    }
 }

byte platformGetch (void)
 {
   int ch = getch();
   if ((0 == ch) || (224 == ch))
    {
      switch (getch())
       {
      case 72:
         ch = 145;
         break;
      case 80:
         ch = 17;
         break;
      case 75:
         ch = 157;
         break;
      case 77:
         ch = 29;
         break;
       }
    }
   return ch;
 }

void platformPutch (byte ch)
 {
   struct text_info info;
   gettextinfo(&info);
   if ((info.curx != info.screenwidth) || (info.cury != info.screenheight))
    {
      putch(ch);
    }
 }

void platformPuts (char* s)
 {
   cputs(s);
 }

void platformCloseScreen (void)
 {
 }

#endif /* ! __MINGW32__ */

#endif /* __GNUC__ */


#ifdef __CC65__

#define BASE_PAGE 8;

void setStorePage(byte pageNo)
 {
   *((byte*)0) = pageNo + BASE_PAGE;
 }

char* getStore(byte indexInCurrentPage)
 {
   int location = indexInCurrentPage;
   location <<= 7;
   location += 0xA000;
   return (char*)location;
 }

#include <conio.h>

void platformInitializeScreen (void)
 {
      // This is graphically glitchy, but I rather it is on.
   cursor(1U);
 }

void platformScreensize (byte* x, byte* y)
 {
   screensize(x, y);
 }

void platformGotoxy (byte x, byte y)
 {
   gotoxy(x, y);
 }

void platformColors (byte pair)
 {
   switch (pair)
    {
   case COLOR_HEADER:
      textcolor(COLOR_BLACK);
      bgcolor(COLOR_GRAY3);
      break;
   case COLOR_NORMAL:
      textcolor(COLOR_GRAY3);
      bgcolor(COLOR_BLUE);
      break;
   case COLOR_HIGHLIGHT:
      textcolor(COLOR_BLUE);
      bgcolor(COLOR_BLACK);
      break;
    }
 }

byte platformGetch (void)
 {
   return cgetc();
 }

void platformPutch (byte ch)
 {
   cputc(ch);
 }

void platformPuts (char* s)
 {
   cputs(s);
 }

void platformCloseScreen (void)
 {
 }

#endif /* __CC65__ */
