/*
 * file        : hash_api.h
 * version     : 1.0.208
 * date        : 14.12.2010
 * 
 * Grostl multi-stream bitsliced implementation Hash API
 *
 * Cagdas Calik
 * ccalik@metu.edu.tr
 * Institute of Applied Mathematics, Middle East Technical University, Turkey.
 *
 */

#ifndef GRSS_API_H
#define GRSS_API_H

#include "sha3_common.h"
#include <tmmintrin.h>

typedef struct
{
	__m128i state1[8];
	__m128i state2[8];
	__m128i state3[8];
	__m128i state4[8];

	__m128i _Pconst[14][8];
	__m128i	_Qconst[14][8];
	__m128i	_shiftconst[8];

	unsigned int uHashLength;
	unsigned int uBlockLength;

	BitSequence buffer[128];

} grssState;

void grssInit(grssState *state, int grssbitlen);

void grssUpdate(grssState *state, const BitSequence *data, DataLength databitlen);

void grssFinal(grssState *state, BitSequence *grssval);

#endif // HASH_API_H

