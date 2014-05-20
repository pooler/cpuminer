/* $Id: sph_bmw.h 216 2010-06-08 09:46:57Z tp $ */
/**
 * BMW interface. BMW (aka "Blue Midnight Wish") is a family of
 * functions which differ by their output size; this implementation
 * defines BMW for output sizes 224, 256, 384 and 512 bits.
 *
 * ==========================(LICENSE BEGIN)============================
 *
 * Copyright (c) 2007-2010  Projet RNRT SAPHIR
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ===========================(LICENSE END)=============================
 *
 * @file     sph_bmw.h
 * @author   Thomas Pornin <thomas.pornin@cryptolog.com>
 */

#ifndef SPH_BMW_H__
#define SPH_BMW_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <stddef.h>
#include "sph_types.h"

#define SPH_SIZE_bmw512   512

typedef struct {
#ifndef DOXYGEN_IGNORE
	sph_u64 bmwH[16];
#endif
} sph_bmw_big_context;

typedef sph_bmw_big_context sph_bmw512_context;


#ifdef __cplusplus
}
#endif

#endif
