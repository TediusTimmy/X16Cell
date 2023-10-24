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
#ifndef STORE_H
#define STORE_H

#include "floats.h"

#define CELL_UNUSED    0
#define CELL_USE_LABEL 1
#define CELL_USE_VALUE 2

   // I know the math doesn't add up here.
#define CELL_MAX_LEN 120U
#define CELL_STR_LEN 122U

typedef struct CELL_ENTRY* centry;

struct CELL_ENTRY
 {
   byte use :  3;
   word loc : 13;
 };

void initStore (void);
   // Returns NULL on error. Loads the name location in (col, row) and returns the end of string.
const char* convertName (const char* src, byte* col, byte* row);
   // These calls invalidate the pointer to previous calls of this function.
centry lookupCell (byte col, byte row); // Returns pointer in HiRAM
x_float* lookupCellValue (byte col, byte row); // Returns NULL if not CELL_USE_VALUE. Returns pointer in HiRAM
char* getCellString (byte col, byte row); // Returns pointer in HiRAM

void recalculate (byte major, byte td, byte lr);

#endif /* STORE_H */
