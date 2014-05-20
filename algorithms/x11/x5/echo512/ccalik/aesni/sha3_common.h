/*
 * file        : sha3_common.h
 * version     : 1.0.208
 * date        : 14.12.2010
 *
 * Common declarations
 *
 * Cagdas Calik
 * ccalik@metu.edu.tr
 * Institute of Applied Mathematics, Middle East Technical University, Turkey.
 *
 */
#include "../../../defs_x5.h"
#ifndef SHA3_COMMON_H
#define SHA3_COMMON_H


#ifdef __GNUC__
#define MYALIGN __attribute__((aligned(16)))
#else
#define MYALIGN __declspec(align(16))
#endif

#define M128(x) *((__m128i*)x)


//typedef unsigned char BitSequence;
//typedef unsigned long long DataLength;
//typedef enum {SUCCESS = 0, FAIL = 1, BAD_HASHBITLEN = 2} HashReturn;

#endif // SHA3_COMMON_H
