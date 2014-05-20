/*
 * file        : bitslice.h
 * version     : 1.0.208
 * date        : 14.12.2010
 * 
 * Bitsliced implementation of AES s-box
 *
 * Credits: Adapted from Emilia Kasper's bitsliced AES implementation, http://homes.esat.kuleuven.be/~ekasper/code/aes-ctr.tar.bz2
 *
 * Cagdas Calik
 * ccalik@metu.edu.tr
 * Institute of Applied Mathematics, Middle East Technical University, Turkey.
 *
 */

#ifndef BITSLICE_H
#define BITSLICE_H

#include "sha3_common.h"
#include <emmintrin.h>

extern const unsigned int _BS0[], _BS1[], _BS2[], _ONE[];



#define SWAPMOVE(a, b, n, m, t) t = b;\
								t = _mm_srli_epi64(t, n);\
								t = _mm_xor_si128(t, a);\
								t = _mm_and_si128(t, m);\
								a = _mm_xor_si128(a, t);\
								t = _mm_slli_epi64(t, n);\
								b = _mm_xor_si128(b, t)


#define BITSLICE(x0, x1, x2, x3, x4, x5, x6, x7, t) SWAPMOVE(x0, x1, 1, M128(_BS0), t);\
													SWAPMOVE(x2, x3, 1, M128(_BS0), t);\
													SWAPMOVE(x4, x5, 1, M128(_BS0), t);\
													SWAPMOVE(x6, x7, 1, M128(_BS0), t);\
													SWAPMOVE(x0, x2, 2, M128(_BS1), t);\
													SWAPMOVE(x1, x3, 2, M128(_BS1), t);\
													SWAPMOVE(x4, x6, 2, M128(_BS1), t);\
													SWAPMOVE(x5, x7, 2, M128(_BS1), t);\
													SWAPMOVE(x0, x4, 4, M128(_BS2), t);\
													SWAPMOVE(x1, x5, 4, M128(_BS2), t);\
													SWAPMOVE(x2, x6, 4, M128(_BS2), t);\
													SWAPMOVE(x3, x7, 4, M128(_BS2), t)



#define SUBSTITUTE_BITSLICE(b0, b1, b2, b3, b4, b5, b6, b7, t0, t1, t2, t3, s0, s1, s2, s3) \
	INBASISCHANGE(b0, b1, b2, b3, b4, b5, b6, b7);\
	INV_GF256(b6, b5, b0, b3, b7, b1, b4, b2, t0, t1, t2, t3, s0, s1, s2, s3);\
	OUTBASISCHANGE(b7, b1, b4, b2, b6, b5, b0, b3);\
	t0 = b0;\
	t1 = b1;\
	t2 = b2;\
	t3 = b3;\
	s0 = b4;\
	s1 = b5;\
	s2 = b6;\
	s3 = b7;\
	b2 = s0;\
	b3 = s2;\
	b4 = t3;\
	b5 = s3;\
	b6 = t2;\
	b7 = s1


#define INBASISCHANGE(b0, b1, b2, b3, b4, b5, b6, b7) \
 b5 = _mm_xor_si128(b5, b6);\
 b2 = _mm_xor_si128(b2, b1);\
 b5 = _mm_xor_si128(b5, b0);\
 b6 = _mm_xor_si128(b6, b2);\
 b3 = _mm_xor_si128(b3, b0);\
 b6 = _mm_xor_si128(b6, b3);\
 b3 = _mm_xor_si128(b3, b7);\
 b3 = _mm_xor_si128(b3, b4);\
 b7 = _mm_xor_si128(b7, b5);\
 b3 = _mm_xor_si128(b3, b1);\
 b4 = _mm_xor_si128(b4, b5);\
 b2 = _mm_xor_si128(b2, b7);\
 b1 = _mm_xor_si128(b1, b5)

#define OUTBASISCHANGE(b0, b1, b2, b3, b4, b5, b6, b7) \
 b0 = _mm_xor_si128(b0, b6);\
 b1 = _mm_xor_si128(b1, b4);\
 b2 = _mm_xor_si128(b2, b0);\
 b4 = _mm_xor_si128(b4, b6);\
 b6 = _mm_xor_si128(b6, b1);\
 b1 = _mm_xor_si128(b1, b5);\
 b5 = _mm_xor_si128(b5, b3);\
 b2 = _mm_xor_si128(b2, b5);\
 b3 = _mm_xor_si128(b3, b7);\
 b7 = _mm_xor_si128(b7, b5);\
 b4 = _mm_xor_si128(b4, b7);\
 b3 = _mm_xor_si128(b3, M128(_ONE));\
 b0 = _mm_xor_si128(b0, M128(_ONE));\
 b1 = _mm_xor_si128(b1, M128(_ONE));\
 b6 = _mm_xor_si128(b6, M128(_ONE))

#define MUL_GF4(x0, x1, y0, y1, t0) \
t0 = y0;\
t0 = _mm_xor_si128(t0, y1);\
t0 = _mm_and_si128(t0, x0);\
x0 = _mm_xor_si128(x0, x1);\
x0 = _mm_and_si128(x0, y1);\
x1 = _mm_and_si128(x1, y0);\
x0 = _mm_xor_si128(x0, x1);\
x1 = _mm_xor_si128(x1, t0)

#define MUL_GF4_N(x0, x1, y0, y1, t0) \
t0 = y0;\
t0 = _mm_xor_si128(t0, y1);\
t0 = _mm_and_si128(t0, x0);\
x0 = _mm_xor_si128(x0, x1);\
x0 = _mm_and_si128(x0, y1);\
x1 = _mm_and_si128(x1, y0);\
x1 = _mm_xor_si128(x1, x0);\
x0 = _mm_xor_si128(x0, t0)

#define MUL_GF4_2(x0, x1, x2, x3, y0, y1, t0, t1) \
t0 = y0;\
t0 = _mm_xor_si128(t0, y1);\
t1 = t0;\
t0 = _mm_and_si128(t0, x0);\
t1 = _mm_and_si128(t1, x2);\
x0 = _mm_xor_si128(x0, x1);\
x2 = _mm_xor_si128(x2, x3);\
x0 = _mm_and_si128(x0, y1);\
x2 = _mm_and_si128(x2, y1);\
x1 = _mm_and_si128(x1, y0);\
x3 = _mm_and_si128(x3, y0);\
x0 = _mm_xor_si128(x0, x1);\
x2 = _mm_xor_si128(x2, x3);\
x1 = _mm_xor_si128(x1, t0);\
x3 = _mm_xor_si128(x3, t1)

#define MUL_GF16(x0, x1, x2, x3, y0, y1, y2, y3, t0, t1, t2, t3) \
t0 = x0;\
t1 = x1;\
MUL_GF4(x0, x1, y0, y1, t2);\
t0 = _mm_xor_si128(t0, x2);\
t1 = _mm_xor_si128(t1, x3);\
y0 = _mm_xor_si128(y0, y2);\
y1 = _mm_xor_si128(y1, y3);\
MUL_GF4_N(t0, t1, y0, y1, t2);\
MUL_GF4(x2, x3, y2, y3, t3);\
x0 = _mm_xor_si128(x0, t0);\
x2 = _mm_xor_si128(x2, t0);\
x1 = _mm_xor_si128(x1, t1);\
x3 = _mm_xor_si128(x3, t1)

#define MUL_GF16_2(x0, x1, x2, x3, x4, x5, x6, x7, y0, y1, y2, y3, t0, t1, t2, t3) \
t0 = x0;\
t1 = x1;\
MUL_GF4(x0, x1, y0, y1, t2);\
t0 = _mm_xor_si128(t0, x2);\
t1 = _mm_xor_si128(t1, x3);\
y0 = _mm_xor_si128(y0, y2);\
y1 = _mm_xor_si128(y1, y3);\
MUL_GF4_N(t0, t1, y0, y1, t3);\
MUL_GF4(x2, x3, y2, y3, t2);\
x0 = _mm_xor_si128(x0, t0);\
x2 = _mm_xor_si128(x2, t0);\
x1 = _mm_xor_si128(x1, t1);\
x3 = _mm_xor_si128(x3, t1);\
t0 = x4;\
t1 = x5;\
t0 = _mm_xor_si128(t0, x6);\
t1 = _mm_xor_si128(t1, x7);\
MUL_GF4_N(t0, t1, y0, y1, t3);\
MUL_GF4(x6, x7, y2, y3, t2);\
y0 = _mm_xor_si128(y0, y2);\
y1 = _mm_xor_si128(y1, y3);\
MUL_GF4(x4, x5, y0, y1, t3);\
x4 = _mm_xor_si128(x4, t0);\
x6 = _mm_xor_si128(x6, t0);\
x5 = _mm_xor_si128(x5, t1);\
x7 = _mm_xor_si128(x7, t1)


#define INV_GF16(x0, x1, x2, x3, t0, t1, t2, t3) \
t0 = x1;\
t1 = x0;\
t0 = _mm_and_si128(t0, x3);\
t1 = _mm_or_si128(t1, x2);\
t2 = x1;\
t3 = x0;\
t2 = _mm_or_si128(t2, x2);\
t3 = _mm_or_si128(t3, x3);\
t2 = _mm_xor_si128(t2, t3);\
t0 = _mm_xor_si128(t0, t2);\
t1 = _mm_xor_si128(t1, t2);\
MUL_GF4_2(x0, x1, x2, x3, t1, t0, t2, t3)

#define INV_GF256(x0,x1,x2,x3,x4,x5,x6,x7,t0,t1,t2,t3,s0,s1,s2,s3) \
t3 = x4;\
t2 = x5;\
t1 = x1;\
s1 = x7;\
s0 = x0;\
t3 = _mm_xor_si128(t3, x6);\
t2 = _mm_xor_si128(t2, x7);\
t1 = _mm_xor_si128(t1, x3);\
s1 = _mm_xor_si128(s1, x6);\
s0 = _mm_xor_si128(s0, x2);\
s2 = t3;\
t0 = t2;\
s3 = t3;\
t2 = _mm_or_si128(t2, t1);\
t3 = _mm_or_si128(t3, s0);\
s3 = _mm_xor_si128(s3, t0);\
s2 = _mm_and_si128(s2, s0);\
t0 = _mm_and_si128(t0, t1);\
s0 = _mm_xor_si128(s0, t1);\
s3 = _mm_and_si128(s3, s0);\
s0 = x3;\
s0 = _mm_xor_si128(s0, x2);\
s1 = _mm_and_si128(s1, s0);\
t3 = _mm_xor_si128(t3, s1);\
t2 = _mm_xor_si128(t2, s1);\
s1 = x4;\
s1 = _mm_xor_si128(s1, x5);\
s0 = x1;\
t1 = s1;\
s0 = _mm_xor_si128(s0, x0);\
t1 = _mm_or_si128(t1, s0);\
s1 = _mm_and_si128(s1, s0);\
t0 = _mm_xor_si128(t0, s1);\
t3 = _mm_xor_si128(t3, s3);\
t2 = _mm_xor_si128(t2, s2);\
t1 = _mm_xor_si128(t1, s3);\
t0 = _mm_xor_si128(t0, s2);\
t1 = _mm_xor_si128(t1, s2);\
s0 = x7;\
s1 = x6;\
s2 = x5;\
s3 = x4;\
s0 = _mm_and_si128(s0, x3);\
s1 = _mm_and_si128(s1, x2);\
s2 = _mm_and_si128(s2, x1);\
s3 = _mm_or_si128(s3, x0);\
t3 = _mm_xor_si128(t3, s0);\
t2 = _mm_xor_si128(t2, s1);\
t1 = _mm_xor_si128(t1, s2);\
t0 = _mm_xor_si128(t0, s3);\
INV_GF16(t0, t1, t2, t3, s0, s1, s2, s3);\
MUL_GF16_2(x0, x1, x2, x3, x4, x5, x6, x7, t2, t3, t0, t1, s0, s1, s2, s3)

#endif // BITSLICE_H
