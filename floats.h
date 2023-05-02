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
#ifndef FLOATS_H
#define FLOATS_H

#include "machina.h"

typedef byte x_float [6];

typedef struct
 {
   small exponent;
   small sign;
   byte digits [4];
 } x_float_impl;

void float_cpy (x_float dest, const x_float src);

void float_neg (x_float arg);
void float_abs (x_float arg);

void float_trunc (x_float arg);
void float_round (x_float arg);

void float_min (x_float dest, const x_float lhs, const x_float rhs);
void float_max (x_float dest, const x_float lhs, const x_float rhs);

void float_add (x_float dest, const x_float lhs, const x_float rhs);
void float_mul (x_float dest, const x_float lhs, const x_float rhs);
void float_div (x_float dest, const x_float lhs, const x_float rhs);

const char* float_from_str (x_float dest, const char* src);
void float_to_str (char dest [15], const x_float src);

#endif /* FLOATS_H */
