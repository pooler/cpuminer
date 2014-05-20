/*
 * file        : vperm.h
 * version     : 1.0.208
 * date        : 14.12.2010
 * 
 * vperm implementation of AES s-box 
 *
 * Credits: Adapted from Mike Hamburg's AES implementation, http://crypto.stanford.edu/vpaes/
 *
 * Cagdas Calik
 * ccalik@metu.edu.tr
 * Institute of Applied Mathematics, Middle East Technical University, Turkey.
 *
 */

#ifndef VPERM_H
#define VPERM_H

#include "sha3_common.h"
#include <tmmintrin.h>

/*
extern const unsigned int _k_s0F[];
extern const unsigned int _k_ipt[];
extern const unsigned int _k_opt[];
extern const unsigned int _k_inv[];
extern const unsigned int _k_sb1[];
extern const unsigned int _k_sb2[];
extern const unsigned int _k_sb3[];
extern const unsigned int _k_sb4[];
extern const unsigned int _k_sb5[];
extern const unsigned int _k_sb7[];
extern const unsigned int _k_sbo[];
extern const unsigned int _k_h63[];
extern const unsigned int _k_hc6[];
extern const unsigned int _k_h5b[];
extern const unsigned int _k_h4e[];
extern const unsigned int _k_h0e[];
extern const unsigned int _k_h15[];
extern const unsigned int _k_aesmix1[];
extern const unsigned int _k_aesmix2[];
extern const unsigned int _k_aesmix3[];
extern const unsigned int _k_aesmix4[];
*/

// input: x, table
// output: x
#define TRANSFORM(x, table, t1, t2)\
	t1 = _mm_andnot_si128(M128(_k_s0F), x);\
	t1 = _mm_srli_epi32(t1, 4);\
	x  = _mm_and_si128(x, M128(_k_s0F));\
	t1 = _mm_shuffle_epi8(*((__m128i*)table + 1), t1);\
	x  = _mm_shuffle_epi8(*((__m128i*)table + 0), x);\
	x  = _mm_xor_si128(x, t1)

// compiled erroneously with 32-bit msc compiler
	//t2 = _mm_shuffle_epi8(table[0], x);\
	//x  = _mm_shuffle_epi8(table[1], t1);\
	//x  = _mm_xor_si128(x, t2)


// input: x
// output: t2, t3
#define SUBSTITUTE_VPERM_CORE(x, t1, t2, t3, t4)\
	t1 = _mm_andnot_si128(M128(_k_s0F), x);\
	t1 = _mm_srli_epi32(t1, 4);\
	x  = _mm_and_si128(x, M128(_k_s0F));\
	t2 = _mm_shuffle_epi8(*((__m128i*)_k_inv + 1), x);\
	x  = _mm_xor_si128(x, t1);\
	t3 = _mm_shuffle_epi8(*((__m128i*)_k_inv + 0), t1);\
	t3 = _mm_xor_si128(t3, t2);\
	t4 = _mm_shuffle_epi8(*((__m128i*)_k_inv + 0), x);\
	t4 = _mm_xor_si128(t4, t2);\
	t2 = _mm_shuffle_epi8(*((__m128i*)_k_inv + 0), t3);\
	t2 = _mm_xor_si128(t2, x);\
	t3 = _mm_shuffle_epi8(*((__m128i*)_k_inv + 0), t4);\
	t3 = _mm_xor_si128(t3, t1);\


// input: x1, x2, table
// output: y
#define VPERM_LOOKUP(x1, x2, table, y, t)\
	t = _mm_shuffle_epi8(*((__m128i*)table + 0), x1);\
	y = _mm_shuffle_epi8(*((__m128i*)table + 1), x2);\
	y = _mm_xor_si128(y, t)


// input: x
// output: x
#define SUBSTITUTE_VPERM(x, t1, t2, t3, t4)  \
	TRANSFORM(x, _k_ipt, t1, t2);\
	SUBSTITUTE_VPERM_CORE(x, t1, t2, t3, t4);\
	VPERM_LOOKUP(t2, t3, _k_sbo, x, t1);\
	x = _mm_xor_si128(x, M128(_k_h63))


// input: x
// output: x
#define AES_ROUND_VPERM_CORE(x, t1, t2, t3, t4, s1, s2, s3) \
	SUBSTITUTE_VPERM_CORE(x, t1, t2, t3, t4);\
	VPERM_LOOKUP(t2, t3, _k_sb1, s1, t1);\
	VPERM_LOOKUP(t2, t3, _k_sb2, s2, t1);\
	s3 = _mm_xor_si128(s1, s2);\
	x = _mm_shuffle_epi8(s2, M128(_k_aesmix1));\
	x = _mm_xor_si128(x, _mm_shuffle_epi8(s3, M128(_k_aesmix2)));\
	x = _mm_xor_si128(x, _mm_shuffle_epi8(s1, M128(_k_aesmix3)));\
	x = _mm_xor_si128(x, _mm_shuffle_epi8(s1, M128(_k_aesmix4)));\
	x = _mm_xor_si128(x, M128(_k_h5b))


// input: x
// output: x
#define AES_ROUND_VPERM(x, t1, t2, t3, t4, s1, s2, s3) \
	TRANSFORM(x, _k_ipt, t1, t2);\
	AES_ROUND_VPERM_CORE(x, t1, t2, t3, t4, s1, s2, s3);\
	TRANSFORM(x, _k_opt, t1, t2)

#endif // VPERM_H

