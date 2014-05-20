/* $Id: sph_skein.h 253 2011-06-07 18:33:10Z tp $ */
/**
 * Skein interface. The Skein specification defines three main
 * functions, called Skein-256, Skein-512 and Skein-1024, which can be
 * further parameterized with an output length. For the SHA-3
 * competition, Skein-512 is used for output sizes of 224, 256, 384 and
 * 512 bits; this is what this code implements. Thus, we hereafter call
 * Skein-224, Skein-256, Skein-384 and Skein-512 what the Skein
 * specification defines as Skein-512-224, Skein-512-256, Skein-512-384
 * and Skein-512-512, respectively.
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
 * @file     sph_skein.h
 * @author   Thomas Pornin <thomas.pornin@cryptolog.com>
 */

#ifndef SPH_SKEIN_H__
#define SPH_SKEIN_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <stddef.h>
#include "sph_types.h"

#define SPH_SIZE_skein512   512

typedef struct {
#ifndef DOXYGEN_IGNORE
	sph_u64 sknh0, sknh1, sknh2, sknh3, sknh4, sknh5, sknh6, sknh7;
#endif
} sph_skein_big_context;

typedef sph_skein_big_context sph_skein512_context;


#ifdef __cplusplus
}
#endif

#endif
