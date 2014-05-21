/*
 * file        : grostl_bitsliced_mm.c
 * version     : 1.0.208
 * date        : 14.12.2010
 * 
 * - multi-stream bitsliced implementation of hash function Grostl 
 * - implements NIST hash api
 * - assumes that message lenght is multiple of 8-bits
 * - _GROSTL_BITSLICED_MM_ must be defined if compiling with ../main.c
 *
 * Cagdas Calik
 * ccalik@metu.edu.tr
 * Institute of Applied Mathematics, Middle East Technical University, Turkey.
 *
 */

#include "grss_api.h"
#include "bitsliceaes.h"


MYALIGN const unsigned int _transpose1[] = {0x060e070f, 0x040c050d, 0x020a030b, 0x00080109};
MYALIGN const unsigned int _hiqmask[]	 = {0x00000000, 0x00000000, 0xffffffff, 0xffffffff};
MYALIGN const unsigned int _loqmask[]	 = {0xffffffff, 0xffffffff, 0x00000000, 0x00000000};
MYALIGN const unsigned int _invmask[]	 = {0x0c0d0e0f, 0x08090a0b, 0x04050607, 0x00010203};

		


#define TRANSPOSE(m, u, v)\
		u[0]  = _mm_shuffle_epi8(m[0], M128(_transpose1));\
		u[1]  = _mm_shuffle_epi8(m[1], M128(_transpose1));\
		u[2]  = _mm_shuffle_epi8(m[2], M128(_transpose1));\
		u[3]  = _mm_shuffle_epi8(m[3], M128(_transpose1));\
		v[0] = _mm_unpacklo_epi16(u[3], u[2]);\
		v[1] = _mm_unpacklo_epi16(u[1], u[0]);\
		v[2] = _mm_unpackhi_epi16(u[3], u[2]);\
		v[3] = _mm_unpackhi_epi16(u[1], u[0]);\
		m[0]  = _mm_unpackhi_epi32(v[2], v[3]);\
		m[1]  = _mm_unpacklo_epi32(v[2], v[3]);\
		m[2]  = _mm_unpackhi_epi32(v[0], v[1]);\
		m[3]  = _mm_unpacklo_epi32(v[0], v[1])

#define TRANSPOSE_BACK(m, u, v)\
	u[0] = _mm_shuffle_epi8(m[0], M128(_transpose1));\
	u[1] = _mm_shuffle_epi8(m[1], M128(_transpose1));\
	u[2] = _mm_shuffle_epi8(m[2], M128(_transpose1));\
	u[3] = _mm_shuffle_epi8(m[3], M128(_transpose1));\
	v[0] = _mm_unpacklo_epi16(u[0], u[1]);\
	v[1] = _mm_unpacklo_epi16(u[2], u[3]);\
	v[2] = _mm_unpackhi_epi16(u[0], u[1]);\
	v[3] = _mm_unpackhi_epi16(u[2], u[3]);\
	m[0] = _mm_unpacklo_epi32(v[0], v[1]);\
	m[1] = _mm_unpackhi_epi32(v[0], v[1]);\
	m[2] = _mm_unpacklo_epi32(v[2], v[3]);\
	m[3] = _mm_unpackhi_epi32(v[2], v[3])


void Init256(grssState *pctx)
{
	unsigned int i;
	__m128i t;

	pctx->state1[0] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state1[1] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state1[2] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state1[3] = _mm_set_epi32(0x00010000, 0, 0, 0);

	pctx->state2[0] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state2[1] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state2[2] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state2[3] = _mm_set_epi32(0x00010000, 0, 0, 0);

	pctx->state3[0] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state3[1] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state3[2] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state3[3] = _mm_set_epi32(0x00010000, 0, 0, 0);

	pctx->state4[0] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state4[1] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state4[2] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state4[3] = _mm_set_epi32(0x00010000, 0, 0, 0);


	for(i = 0; i < 10; i++)
	{
		pctx->_Pconst[i][0] = _mm_set_epi32(i << 24, 0, 0, 0);
		pctx->_Pconst[i][1] = _mm_set_epi32(i << 24, 0, 0, 0);
		pctx->_Pconst[i][2] = _mm_set_epi32(i << 24, 0, 0, 0);
		pctx->_Pconst[i][3] = _mm_set_epi32(i << 24, 0, 0, 0);
		pctx->_Pconst[i][4] = _mm_set_epi32(0, 0, 0, 0);
		pctx->_Pconst[i][5] = _mm_set_epi32(0, 0, 0, 0);
		pctx->_Pconst[i][6] = _mm_set_epi32(0, 0, 0, 0);
		pctx->_Pconst[i][7] = _mm_set_epi32(0, 0, 0, 0);


		pctx->_Qconst[i][0] = _mm_set_epi32(0, 0, 0, 0);
		pctx->_Qconst[i][1] = _mm_set_epi32(0, 0, 0, 0);
		pctx->_Qconst[i][2] = _mm_set_epi32(0, 0, 0, 0);
		pctx->_Qconst[i][3] = _mm_set_epi32(0, 0, 0, 0);
		pctx->_Qconst[i][4] = _mm_set_epi32(0, 0, (~i) << 24, 0);
		pctx->_Qconst[i][5] = _mm_set_epi32(0, 0, (~i) << 24, 0);
		pctx->_Qconst[i][6] = _mm_set_epi32(0, 0, (~i) << 24, 0);
		pctx->_Qconst[i][7] = _mm_set_epi32(0, 0, (~i) << 24, 0);

		BITSLICE(pctx->_Pconst[i][0], pctx->_Pconst[i][1], pctx->_Pconst[i][2], pctx->_Pconst[i][3], pctx->_Pconst[i][4], pctx->_Pconst[i][5], pctx->_Pconst[i][6], pctx->_Pconst[i][7], t);
		BITSLICE(pctx->_Qconst[i][0], pctx->_Qconst[i][1], pctx->_Qconst[i][2], pctx->_Qconst[i][3], pctx->_Qconst[i][4], pctx->_Qconst[i][5], pctx->_Qconst[i][6], pctx->_Qconst[i][7], t);
	}

	pctx->_shiftconst[0] = _mm_set_epi32(0x0f0e0d0c, 0x0b0a0908, 0x06050403, 0x02010007);
	pctx->_shiftconst[1] = _mm_set_epi32(0x0d0c0b0a, 0x09080f0e, 0x04030201, 0x00070605);
	pctx->_shiftconst[2] = _mm_set_epi32(0x0b0a0908, 0x0f0e0d0c, 0x02010007, 0x06050403);
	pctx->_shiftconst[3] = _mm_set_epi32(0x09080f0e, 0x0d0c0b0a, 0x00070605, 0x04030201);
}


void Init512(grssState *pctx)
{
	unsigned int i;
	__m128i t;

	pctx->state1[0] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state1[1] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state1[2] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state1[3] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state1[4] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state1[5] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state1[6] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state1[7] = _mm_set_epi32(0x00020000, 0, 0, 0);

	pctx->state2[0] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state2[1] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state2[2] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state2[3] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state2[4] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state2[5] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state2[6] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state2[7] = _mm_set_epi32(0x00020000, 0, 0, 0);

	pctx->state3[0] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state3[1] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state3[2] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state3[3] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state3[4] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state3[5] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state3[6] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state3[7] = _mm_set_epi32(0x00020000, 0, 0, 0);

	pctx->state4[0] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state4[1] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state4[2] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state4[3] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state4[4] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state4[5] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state4[6] = _mm_set_epi32(0, 0, 0, 0);
	pctx->state4[7] = _mm_set_epi32(0x00020000, 0, 0, 0);

	for(i = 0; i < 14; i++)
	{
		pctx->_Pconst[i][0] = _mm_set_epi32(i << 24, 0, 0, 0);
		pctx->_Pconst[i][1] = _mm_set_epi32(i << 24, 0, 0, 0);
		pctx->_Pconst[i][2] = _mm_set_epi32(i << 24, 0, 0, 0);
		pctx->_Pconst[i][3] = _mm_set_epi32(i << 24, 0, 0, 0);
		pctx->_Pconst[i][4] = _mm_set_epi32(0, 0, 0, 0);
		pctx->_Pconst[i][5] = _mm_set_epi32(0, 0, 0, 0);
		pctx->_Pconst[i][6] = _mm_set_epi32(0, 0, 0, 0);
		pctx->_Pconst[i][7] = _mm_set_epi32(0, 0, 0, 0);


		pctx->_Qconst[i][4] = _mm_set_epi32((~i) << 24, 0, 0, 0);
		pctx->_Qconst[i][5] = _mm_set_epi32((~i) << 24, 0, 0, 0);
		pctx->_Qconst[i][6] = _mm_set_epi32((~i) << 24, 0, 0, 0);
		pctx->_Qconst[i][7] = _mm_set_epi32((~i) << 24, 0, 0, 0);
		pctx->_Qconst[i][0] = _mm_set_epi32(0, 0, 0, 0);
		pctx->_Qconst[i][1] = _mm_set_epi32(0, 0, 0, 0);
		pctx->_Qconst[i][2] = _mm_set_epi32(0, 0, 0, 0);
		pctx->_Qconst[i][3] = _mm_set_epi32(0, 0, 0, 0);

		BITSLICE(pctx->_Pconst[i][0], pctx->_Pconst[i][1], pctx->_Pconst[i][2], pctx->_Pconst[i][3], pctx->_Pconst[i][4], pctx->_Pconst[i][5], pctx->_Pconst[i][6], pctx->_Pconst[i][7], t);
		BITSLICE(pctx->_Qconst[i][0], pctx->_Qconst[i][1], pctx->_Qconst[i][2], pctx->_Qconst[i][3], pctx->_Qconst[i][4], pctx->_Qconst[i][5], pctx->_Qconst[i][6], pctx->_Qconst[i][7], t);
	}

	pctx->_shiftconst[1] = _mm_set_epi32(0x0e0d0c0b, 0x0a090807, 0x06050403, 0x0201000f);
	pctx->_shiftconst[2] = _mm_set_epi32(0x0d0c0b0a, 0x09080706, 0x05040302, 0x01000f0e);
	pctx->_shiftconst[3] = _mm_set_epi32(0x0c0b0a09, 0x08070605, 0x04030201, 0x000f0e0d);
	pctx->_shiftconst[4] = _mm_set_epi32(0x0b0a0908, 0x07060504, 0x03020100, 0x0f0e0d0c);
	pctx->_shiftconst[5] = _mm_set_epi32(0x0a090807, 0x06050403, 0x0201000f, 0x0e0d0c0b);
	pctx->_shiftconst[6] = _mm_set_epi32(0x09080706, 0x05040302, 0x01000f0e, 0x0d0c0b0a);
	pctx->_shiftconst[7] = _mm_set_epi32(0x04030201, 0x000f0e0d, 0x0c0b0a09, 0x08070605);
}


#define MUL_BITSLICE_2(x, i, b7, b6, b5, b4, b3, b2, b1, b0)\
			x[7] = _mm_xor_si128(x[7], b6[i]);\
			x[6] = _mm_xor_si128(x[6], b5[i]);\
			x[5] = _mm_xor_si128(x[5], b4[i]);\
			x[4] = _mm_xor_si128(x[4], b3[i]);\
			x[4] = _mm_xor_si128(x[4], b7[i]);\
			x[3] = _mm_xor_si128(x[3], b2[i]);\
			x[3] = _mm_xor_si128(x[3], b7[i]);\
			x[2] = _mm_xor_si128(x[2], b1[i]);\
			x[1] = _mm_xor_si128(x[1], b0[i]);\
			x[1] = _mm_xor_si128(x[1], b7[i]);\
			x[0] = _mm_xor_si128(x[0], b7[i])

#define MUL_BITSLICE_3(x, i, b7, b6, b5, b4, b3, b2, b1, b0)\
			x[7] = _mm_xor_si128(x[7], b6[i]);\
			x[7] = _mm_xor_si128(x[7], b7[i]);\
			x[6] = _mm_xor_si128(x[6], b5[i]);\
			x[6] = _mm_xor_si128(x[6], b6[i]);\
			x[5] = _mm_xor_si128(x[5], b4[i]);\
			x[5] = _mm_xor_si128(x[5], b5[i]);\
			x[4] = _mm_xor_si128(x[4], b3[i]);\
			x[4] = _mm_xor_si128(x[4], b4[i]);\
			x[4] = _mm_xor_si128(x[4], b7[i]);\
			x[3] = _mm_xor_si128(x[3], b2[i]);\
			x[3] = _mm_xor_si128(x[3], b3[i]);\
			x[3] = _mm_xor_si128(x[3], b7[i]);\
			x[2] = _mm_xor_si128(x[2], b1[i]);\
			x[2] = _mm_xor_si128(x[2], b2[i]);\
			x[1] = _mm_xor_si128(x[1], b0[i]);\
			x[1] = _mm_xor_si128(x[1], b1[i]);\
			x[1] = _mm_xor_si128(x[1], b7[i]);\
			x[0] = _mm_xor_si128(x[0], b0[i]);\
			x[0] = _mm_xor_si128(x[0], b7[i])

#define MUL_BITSLICE_4(x, i, b7, b6, b5, b4, b3, b2, b1, b0)\
			x[7] = _mm_xor_si128(x[7], b5[i]);\
			x[6] = _mm_xor_si128(x[6], b4[i]);\
			x[5] = _mm_xor_si128(x[5], b3[i]);\
			x[5] = _mm_xor_si128(x[5], b7[i]);\
			x[4] = _mm_xor_si128(x[4], b2[i]);\
			x[4] = _mm_xor_si128(x[4], b6[i]);\
			x[4] = _mm_xor_si128(x[4], b7[i]);\
			x[3] = _mm_xor_si128(x[3], b1[i]);\
			x[3] = _mm_xor_si128(x[3], b6[i]);\
			x[2] = _mm_xor_si128(x[2], b0[i]);\
			x[2] = _mm_xor_si128(x[2], b7[i]);\
			x[1] = _mm_xor_si128(x[1], b6[i]);\
			x[1] = _mm_xor_si128(x[1], b7[i]);\
			x[0] = _mm_xor_si128(x[0], b6[i])


#define MUL_BITSLICE_5(x, i, b7, b6, b5, b4, b3, b2, b1, b0)\
			x[7] = _mm_xor_si128(x[7], b5[i]);\
			x[7] = _mm_xor_si128(x[7], b7[i]);\
			x[6] = _mm_xor_si128(x[6], b4[i]);\
			x[6] = _mm_xor_si128(x[6], b6[i]);\
			x[5] = _mm_xor_si128(x[5], b3[i]);\
			x[5] = _mm_xor_si128(x[5], b5[i]);\
			x[5] = _mm_xor_si128(x[5], b7[i]);\
			x[4] = _mm_xor_si128(x[4], b2[i]);\
			x[4] = _mm_xor_si128(x[4], b4[i]);\
			x[4] = _mm_xor_si128(x[4], b6[i]);\
			x[4] = _mm_xor_si128(x[4], b7[i]);\
			x[3] = _mm_xor_si128(x[3], b1[i]);\
			x[3] = _mm_xor_si128(x[3], b3[i]);\
			x[3] = _mm_xor_si128(x[3], b6[i]);\
			x[2] = _mm_xor_si128(x[2], b0[i]);\
			x[2] = _mm_xor_si128(x[2], b2[i]);\
			x[2] = _mm_xor_si128(x[2], b7[i]);\
			x[1] = _mm_xor_si128(x[1], b1[i]);\
			x[1] = _mm_xor_si128(x[1], b6[i]);\
			x[1] = _mm_xor_si128(x[1], b7[i]);\
			x[0] = _mm_xor_si128(x[0], b0[i]);\
			x[0] = _mm_xor_si128(x[0], b6[i])

#define MUL_BITSLICE_7(x, i, b7, b6, b5, b4, b3, b2, b1, b0)\
			x[7] = _mm_xor_si128(x[7], b5[i]);\
			x[7] = _mm_xor_si128(x[7], b6[i]);\
			x[7] = _mm_xor_si128(x[7], b7[i]);\
			x[6] = _mm_xor_si128(x[6], b4[i]);\
			x[6] = _mm_xor_si128(x[6], b5[i]);\
			x[6] = _mm_xor_si128(x[6], b6[i]);\
			x[5] = _mm_xor_si128(x[5], b3[i]);\
			x[5] = _mm_xor_si128(x[5], b4[i]);\
			x[5] = _mm_xor_si128(x[5], b5[i]);\
			x[5] = _mm_xor_si128(x[5], b7[i]);\
			x[4] = _mm_xor_si128(x[4], b2[i]);\
			x[4] = _mm_xor_si128(x[4], b3[i]);\
			x[4] = _mm_xor_si128(x[4], b4[i]);\
			x[4] = _mm_xor_si128(x[4], b6[i]);\
			x[3] = _mm_xor_si128(x[3], b1[i]);\
			x[3] = _mm_xor_si128(x[3], b2[i]);\
			x[3] = _mm_xor_si128(x[3], b3[i]);\
			x[3] = _mm_xor_si128(x[3], b6[i]);\
			x[3] = _mm_xor_si128(x[3], b7[i]);\
			x[2] = _mm_xor_si128(x[2], b0[i]);\
			x[2] = _mm_xor_si128(x[2], b1[i]);\
			x[2] = _mm_xor_si128(x[2], b2[i]);\
			x[2] = _mm_xor_si128(x[2], b7[i]);\
			x[1] = _mm_xor_si128(x[1], b0[i]);\
			x[1] = _mm_xor_si128(x[1], b1[i]);\
			x[1] = _mm_xor_si128(x[1], b6[i]);\
			x[0] = _mm_xor_si128(x[0], b0[i]);\
			x[0] = _mm_xor_si128(x[0], b6[i]);\
			x[0] = _mm_xor_si128(x[0], b7[i])




#define ROW_L2L(x) _mm_and_si128(x, M128(_hiqmask))
#define ROW_L2R(x) _mm_srli_si128(x, 8)
#define ROW_R2L(x) _mm_slli_si128(x, 8)
#define ROW_R2R(x) _mm_and_si128(x, M128(_loqmask))

#define ROW_MOV_EO ROW_L2R
#define ROW_MOV_EE ROW_L2L
#define ROW_MOV_OE ROW_R2L
#define ROW_MOV_OO ROW_R2R

#define MUL_BITSLICE256_2(x, rm, i)\
			x[7] = _mm_xor_si128(x[7], rm(p2[i]));\
			x[6] = _mm_xor_si128(x[6], rm(p3[i]));\
			x[5] = _mm_xor_si128(x[5], rm(p4[i]));\
			x[4] = _mm_xor_si128(x[4], rm(q1[i]));\
			x[4] = _mm_xor_si128(x[4], rm(p1[i]));\
			x[3] = _mm_xor_si128(x[3], rm(q2[i]));\
			x[3] = _mm_xor_si128(x[3], rm(p1[i]));\
			x[2] = _mm_xor_si128(x[2], rm(q3[i]));\
			x[1] = _mm_xor_si128(x[1], rm(q4[i]));\
			x[1] = _mm_xor_si128(x[1], rm(p1[i]));\
			x[0] = _mm_xor_si128(x[0], rm(p1[i]))

#define MUL_BITSLICE256_3(x, rm, i)\
			x[7] = _mm_xor_si128(x[7], rm(p2[i]));\
			x[7] = _mm_xor_si128(x[7], rm(p1[i]));\
			x[6] = _mm_xor_si128(x[6], rm(p3[i]));\
			x[6] = _mm_xor_si128(x[6], rm(p2[i]));\
			x[5] = _mm_xor_si128(x[5], rm(p4[i]));\
			x[5] = _mm_xor_si128(x[5], rm(p3[i]));\
			x[4] = _mm_xor_si128(x[4], rm(q1[i]));\
			x[4] = _mm_xor_si128(x[4], rm(p4[i]));\
			x[4] = _mm_xor_si128(x[4], rm(p1[i]));\
			x[3] = _mm_xor_si128(x[3], rm(q2[i]));\
			x[3] = _mm_xor_si128(x[3], rm(q1[i]));\
			x[3] = _mm_xor_si128(x[3], rm(p1[i]));\
			x[2] = _mm_xor_si128(x[2], rm(q2[i]));\
			x[2] = _mm_xor_si128(x[2], rm(q3[i]));\
			x[1] = _mm_xor_si128(x[1], rm(q4[i]));\
			x[1] = _mm_xor_si128(x[1], rm(q3[i]));\
			x[1] = _mm_xor_si128(x[1], rm(p1[i]));\
			x[0] = _mm_xor_si128(x[0], rm(q4[i]));\
			x[0] = _mm_xor_si128(x[0], rm(p1[i]))


#define MUL_BITSLICE256_4(x, rm, i)\
			x[7] = _mm_xor_si128(x[7], rm(p3[i]));\
			x[6] = _mm_xor_si128(x[6], rm(p4[i]));\
			x[5] = _mm_xor_si128(x[5], rm(q1[i]));\
			x[5] = _mm_xor_si128(x[5], rm(p1[i]));\
			x[4] = _mm_xor_si128(x[4], rm(q2[i]));\
			x[4] = _mm_xor_si128(x[4], rm(p2[i]));\
			x[4] = _mm_xor_si128(x[4], rm(p1[i]));\
			x[3] = _mm_xor_si128(x[3], rm(q3[i]));\
			x[3] = _mm_xor_si128(x[3], rm(p2[i]));\
			x[2] = _mm_xor_si128(x[2], rm(q4[i]));\
			x[2] = _mm_xor_si128(x[2], rm(p1[i]));\
			x[1] = _mm_xor_si128(x[1], rm(p2[i]));\
			x[1] = _mm_xor_si128(x[1], rm(p1[i]));\
			x[0] = _mm_xor_si128(x[0], rm(p2[i]))

#define MUL_BITSLICE256_5(x, rm, i)\
			x[7] = _mm_xor_si128(x[7], rm(p3[i]));\
			x[7] = _mm_xor_si128(x[7], rm(p1[i]));\
			x[6] = _mm_xor_si128(x[6], rm(p4[i]));\
			x[6] = _mm_xor_si128(x[6], rm(p2[i]));\
			x[5] = _mm_xor_si128(x[5], rm(q1[i]));\
			x[5] = _mm_xor_si128(x[5], rm(p3[i]));\
			x[5] = _mm_xor_si128(x[5], rm(p1[i]));\
			x[4] = _mm_xor_si128(x[4], rm(q2[i]));\
			x[4] = _mm_xor_si128(x[4], rm(p4[i]));\
			x[4] = _mm_xor_si128(x[4], rm(p2[i]));\
			x[4] = _mm_xor_si128(x[4], rm(p1[i]));\
			x[3] = _mm_xor_si128(x[3], rm(q3[i]));\
			x[3] = _mm_xor_si128(x[3], rm(q1[i]));\
			x[3] = _mm_xor_si128(x[3], rm(p2[i]));\
			x[2] = _mm_xor_si128(x[2], rm(q4[i]));\
			x[2] = _mm_xor_si128(x[2], rm(q2[i]));\
			x[2] = _mm_xor_si128(x[2], rm(p1[i]));\
			x[1] = _mm_xor_si128(x[1], rm(q3[i]));\
			x[1] = _mm_xor_si128(x[1], rm(p2[i]));\
			x[1] = _mm_xor_si128(x[1], rm(p1[i]));\
			x[0] = _mm_xor_si128(x[0], rm(q4[i]));\
			x[0] = _mm_xor_si128(x[0], rm(p2[i]))


#define MUL_BITSLICE256_7(x, rm, i)\
			x[7] = _mm_xor_si128(x[7], rm(p3[i]));\
			x[7] = _mm_xor_si128(x[7], rm(p2[i]));\
			x[7] = _mm_xor_si128(x[7], rm(p1[i]));\
			x[6] = _mm_xor_si128(x[6], rm(p4[i]));\
			x[6] = _mm_xor_si128(x[6], rm(p3[i]));\
			x[6] = _mm_xor_si128(x[6], rm(p2[i]));\
			x[5] = _mm_xor_si128(x[5], rm(q1[i]));\
			x[5] = _mm_xor_si128(x[5], rm(p4[i]));\
			x[5] = _mm_xor_si128(x[5], rm(p3[i]));\
			x[5] = _mm_xor_si128(x[5], rm(p1[i]));\
			x[4] = _mm_xor_si128(x[4], rm(q2[i]));\
			x[4] = _mm_xor_si128(x[4], rm(q1[i]));\
			x[4] = _mm_xor_si128(x[4], rm(p4[i]));\
			x[4] = _mm_xor_si128(x[4], rm(p2[i]));\
			x[3] = _mm_xor_si128(x[3], rm(q3[i]));\
			x[3] = _mm_xor_si128(x[3], rm(q2[i]));\
			x[3] = _mm_xor_si128(x[3], rm(q1[i]));\
			x[3] = _mm_xor_si128(x[3], rm(p2[i]));\
			x[3] = _mm_xor_si128(x[3], rm(p1[i]));\
			x[2] = _mm_xor_si128(x[2], rm(q4[i]));\
			x[2] = _mm_xor_si128(x[2], rm(q3[i]));\
			x[2] = _mm_xor_si128(x[2], rm(q2[i]));\
			x[2] = _mm_xor_si128(x[2], rm(p1[i]));\
			x[1] = _mm_xor_si128(x[1], rm(q4[i]));\
			x[1] = _mm_xor_si128(x[1], rm(q3[i]));\
			x[1] = _mm_xor_si128(x[1], rm(p2[i]));\
			x[0] = _mm_xor_si128(x[0], rm(q4[i]));\
			x[0] = _mm_xor_si128(x[0], rm(p2[i]));\
			x[0] = _mm_xor_si128(x[0], rm(p1[i]))


void Compress256(grssState *ctx,
				 const unsigned char *pmsg1, const unsigned char *pmsg2, const unsigned char *pmsg3, const unsigned char *pmsg4,
				 DataLength uBlockCount)
{

	DataLength b;
	unsigned int i, r;
	__m128i x[8], t0, t1, t2, t3, t4, t5, t6, t7, u[4], u2[4];
	__m128i p1[4], p2[4], p3[4], p4[4], q1[4], q2[4], q3[4], q4[4];
	__m128i r1[8], r2[8], r3[8], r4[8], s1[8], s2[8], s3[8], s4[8];
	__m128i x01[8], x23[8], x45[8], x67[8];
	__m128i x0[8], x1[8], x2[8], x3[8], x4[8], x5[8], x6[8], x7[8];

	for(i = 0; i < 8; i++)
		x[i] = _mm_xor_si128(x[i], x[i]);

	// transpose cv
	TRANSPOSE(ctx->state1, u, u2);
	TRANSPOSE(ctx->state2, u, u2);
	TRANSPOSE(ctx->state3, u, u2);
	TRANSPOSE(ctx->state4, u, u2);

	for(b = 0; b < uBlockCount; b++)
	{
		q1[0] = _mm_loadu_si128((__m128i*)pmsg1 + 0);
		q1[1] = _mm_loadu_si128((__m128i*)pmsg1 + 1);
		q1[2] = _mm_loadu_si128((__m128i*)pmsg1 + 2);
		q1[3] = _mm_loadu_si128((__m128i*)pmsg1 + 3);
		q2[0] = _mm_loadu_si128((__m128i*)pmsg2 + 0);
		q2[1] = _mm_loadu_si128((__m128i*)pmsg2 + 1);
		q2[2] = _mm_loadu_si128((__m128i*)pmsg2 + 2);
		q2[3] = _mm_loadu_si128((__m128i*)pmsg2 + 3);
		q3[0] = _mm_loadu_si128((__m128i*)pmsg3 + 0);
		q3[1] = _mm_loadu_si128((__m128i*)pmsg3 + 1);
		q3[2] = _mm_loadu_si128((__m128i*)pmsg3 + 2);
		q3[3] = _mm_loadu_si128((__m128i*)pmsg3 + 3);
		q4[0] = _mm_loadu_si128((__m128i*)pmsg4 + 0);
		q4[1] = _mm_loadu_si128((__m128i*)pmsg4 + 1);
		q4[2] = _mm_loadu_si128((__m128i*)pmsg4 + 2);
		q4[3] = _mm_loadu_si128((__m128i*)pmsg4 + 3);

		// transpose message
		TRANSPOSE(q1, u, u2);
		TRANSPOSE(q2, u, u2);
		TRANSPOSE(q3, u, u2);
		TRANSPOSE(q4, u, u2);

		// xor cv and message
		for(i = 0; i < 4; i++)
		{
			p1[i] = _mm_xor_si128(ctx->state1[i], q1[i]);
			p2[i] = _mm_xor_si128(ctx->state2[i], q2[i]);
			p3[i] = _mm_xor_si128(ctx->state3[i], q3[i]);
			p4[i] = _mm_xor_si128(ctx->state4[i], q4[i]);
		}


		BITSLICE(p1[0], p2[0], p3[0], p4[0], q1[0], q2[0], q3[0], q4[0], t0);
		BITSLICE(p1[1], p2[1], p3[1], p4[1], q1[1], q2[1], q3[1], q4[1], t0);
		BITSLICE(p1[2], p2[2], p3[2], p4[2], q1[2], q2[2], q3[2], q4[2], t0);
		BITSLICE(p1[3], p2[3], p3[3], p4[3], q1[3], q2[3], q3[3], q4[3], t0);

		for(r = 0; r < 10; r++)
		{
			// Add const
			p1[0] = _mm_xor_si128(p1[0], ctx->_Pconst[r][0]);
			p2[0] = _mm_xor_si128(p2[0], ctx->_Pconst[r][1]);
			p3[0] = _mm_xor_si128(p3[0], ctx->_Pconst[r][2]);
			p4[0] = _mm_xor_si128(p4[0], ctx->_Pconst[r][3]);
			q1[0] = _mm_xor_si128(q1[0], ctx->_Pconst[r][4]);
			q2[0] = _mm_xor_si128(q2[0], ctx->_Pconst[r][5]);
			q3[0] = _mm_xor_si128(q3[0], ctx->_Pconst[r][6]);
			q4[0] = _mm_xor_si128(q4[0], ctx->_Pconst[r][7]);

			p1[3] = _mm_xor_si128(p1[3], ctx->_Qconst[r][0]);
			p2[3] = _mm_xor_si128(p2[3], ctx->_Qconst[r][1]);
			p3[3] = _mm_xor_si128(p3[3], ctx->_Qconst[r][2]);
			p4[3] = _mm_xor_si128(p4[3], ctx->_Qconst[r][3]);
			q1[3] = _mm_xor_si128(q1[3], ctx->_Qconst[r][4]);
			q2[3] = _mm_xor_si128(q2[3], ctx->_Qconst[r][5]);
			q3[3] = _mm_xor_si128(q3[3], ctx->_Qconst[r][6]);
			q4[3] = _mm_xor_si128(q4[3], ctx->_Qconst[r][7]);

			// Sub bytes
			SUBSTITUTE_BITSLICE(q4[0], q3[0], q2[0], q1[0], p4[0], p3[0], p2[0], p1[0], t0, t1, t2, t3, t4, t5, t6, t7);
			SUBSTITUTE_BITSLICE(q4[1], q3[1], q2[1], q1[1], p4[1], p3[1], p2[1], p1[1], t0, t1, t2, t3, t4, t5, t6, t7);
			SUBSTITUTE_BITSLICE(q4[2], q3[2], q2[2], q1[2], p4[2], p3[2], p2[2], p1[2], t0, t1, t2, t3, t4, t5, t6, t7);
			SUBSTITUTE_BITSLICE(q4[3], q3[3], q2[3], q1[3], p4[3], p3[3], p2[3], p1[3], t0, t1, t2, t3, t4, t5, t6, t7);

			// Shift bytes
			p1[0] = _mm_shuffle_epi8(p1[0], ctx->_shiftconst[0]);
			p2[0] = _mm_shuffle_epi8(p2[0], ctx->_shiftconst[0]);
			p3[0] = _mm_shuffle_epi8(p3[0], ctx->_shiftconst[0]);
			p4[0] = _mm_shuffle_epi8(p4[0], ctx->_shiftconst[0]);
			q1[0] = _mm_shuffle_epi8(q1[0], ctx->_shiftconst[0]);
			q2[0] = _mm_shuffle_epi8(q2[0], ctx->_shiftconst[0]);
			q3[0] = _mm_shuffle_epi8(q3[0], ctx->_shiftconst[0]);
			q4[0] = _mm_shuffle_epi8(q4[0], ctx->_shiftconst[0]);

			p1[1] = _mm_shuffle_epi8(p1[1], ctx->_shiftconst[1]);
			p2[1] = _mm_shuffle_epi8(p2[1], ctx->_shiftconst[1]);
			p3[1] = _mm_shuffle_epi8(p3[1], ctx->_shiftconst[1]);
			p4[1] = _mm_shuffle_epi8(p4[1], ctx->_shiftconst[1]);
			q1[1] = _mm_shuffle_epi8(q1[1], ctx->_shiftconst[1]);
			q2[1] = _mm_shuffle_epi8(q2[1], ctx->_shiftconst[1]);
			q3[1] = _mm_shuffle_epi8(q3[1], ctx->_shiftconst[1]);
			q4[1] = _mm_shuffle_epi8(q4[1], ctx->_shiftconst[1]);

			p1[2] = _mm_shuffle_epi8(p1[2], ctx->_shiftconst[2]);
			p2[2] = _mm_shuffle_epi8(p2[2], ctx->_shiftconst[2]);
			p3[2] = _mm_shuffle_epi8(p3[2], ctx->_shiftconst[2]);
			p4[2] = _mm_shuffle_epi8(p4[2], ctx->_shiftconst[2]);
			q1[2] = _mm_shuffle_epi8(q1[2], ctx->_shiftconst[2]);
			q2[2] = _mm_shuffle_epi8(q2[2], ctx->_shiftconst[2]);
			q3[2] = _mm_shuffle_epi8(q3[2], ctx->_shiftconst[2]);
			q4[2] = _mm_shuffle_epi8(q4[2], ctx->_shiftconst[2]);

			p1[3] = _mm_shuffle_epi8(p1[3], ctx->_shiftconst[3]);
			p2[3] = _mm_shuffle_epi8(p2[3], ctx->_shiftconst[3]);
			p3[3] = _mm_shuffle_epi8(p3[3], ctx->_shiftconst[3]);
			p4[3] = _mm_shuffle_epi8(p4[3], ctx->_shiftconst[3]);
			q1[3] = _mm_shuffle_epi8(q1[3], ctx->_shiftconst[3]);
			q2[3] = _mm_shuffle_epi8(q2[3], ctx->_shiftconst[3]);
			q3[3] = _mm_shuffle_epi8(q3[3], ctx->_shiftconst[3]);
			q4[3] = _mm_shuffle_epi8(q4[3], ctx->_shiftconst[3]);

			// Mix bytes
#if 0
			for(i = 0; i < 4; i++)
			{
				r1[2 * i + 0] = _mm_srli_si128(p1[i], 8);
				r1[2 * i + 1] = _mm_and_si128(p1[i], M128(_loqmask));
				r2[2 * i + 0] = _mm_srli_si128(p2[i], 8);
				r2[2 * i + 1] = _mm_and_si128(p2[i], M128(_loqmask));
				r3[2 * i + 0] = _mm_srli_si128(p3[i], 8);
				r3[2 * i + 1] = _mm_and_si128(p3[i], M128(_loqmask));
				r4[2 * i + 0] = _mm_srli_si128(p4[i], 8);
				r4[2 * i + 1] = _mm_and_si128(p4[i], M128(_loqmask));

				s1[2 * i + 0] = _mm_srli_si128(q1[i], 8);
				s1[2 * i + 1] = _mm_and_si128(q1[i], M128(_loqmask));
				s2[2 * i + 0] = _mm_srli_si128(q2[i], 8);
				s2[2 * i + 1] = _mm_and_si128(q2[i], M128(_loqmask));
				s3[2 * i + 0] = _mm_srli_si128(q3[i], 8);
				s3[2 * i + 1] = _mm_and_si128(q3[i], M128(_loqmask));
				s4[2 * i + 0] = _mm_srli_si128(q4[i], 8);
				s4[2 * i + 1] = _mm_and_si128(q4[i], M128(_loqmask));

			}

			for(i = 0; i < 8; i++)
			{
				x0[i] = _mm_xor_si128(x0[i], x0[i]);
				x1[i] = _mm_xor_si128(x1[i], x1[i]);
				x2[i] = _mm_xor_si128(x2[i], x2[i]);
				x3[i] = _mm_xor_si128(x3[i], x3[i]);
				x4[i] = _mm_xor_si128(x4[i], x4[i]);
				x5[i] = _mm_xor_si128(x5[i], x5[i]);
				x6[i] = _mm_xor_si128(x6[i], x6[i]);
				x7[i] = _mm_xor_si128(x7[i], x7[i]);
			}

			MUL_BITSLICE_2(x0, 0, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_2(x0, 1, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x0, 2, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_4(x0, 3, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x0, 4, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x0, 5, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x0, 6, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_7(x0, 7, r1, r2, r3, r4, s1, s2, s3, s4);

			MUL_BITSLICE_2(x1, 1, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_2(x1, 2, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x1, 3, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_4(x1, 4, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x1, 5, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x1, 6, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x1, 7, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_7(x1, 0, r1, r2, r3, r4, s1, s2, s3, s4);

			MUL_BITSLICE_2(x2, 2, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_2(x2, 3, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x2, 4, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_4(x2, 5, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x2, 6, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x2, 7, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x2, 0, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_7(x2, 1, r1, r2, r3, r4, s1, s2, s3, s4);

			MUL_BITSLICE_2(x3, 3, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_2(x3, 4, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x3, 5, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_4(x3, 6, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x3, 7, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x3, 0, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x3, 1, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_7(x3, 2, r1, r2, r3, r4, s1, s2, s3, s4);

			MUL_BITSLICE_2(x4, 4, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_2(x4, 5, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x4, 6, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_4(x4, 7, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x4, 0, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x4, 1, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x4, 2, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_7(x4, 3, r1, r2, r3, r4, s1, s2, s3, s4);

			MUL_BITSLICE_2(x5, 5, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_2(x5, 6, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x5, 7, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_4(x5, 0, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x5, 1, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x5, 2, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x5, 3, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_7(x5, 4, r1, r2, r3, r4, s1, s2, s3, s4);

			MUL_BITSLICE_2(x6, 6, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_2(x6, 7, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x6, 0, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_4(x6, 1, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x6, 2, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x6, 3, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x6, 4, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_7(x6, 5, r1, r2, r3, r4, s1, s2, s3, s4);

			MUL_BITSLICE_2(x7, 7, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_2(x7, 0, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x7, 1, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_4(x7, 2, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x7, 3, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_3(x7, 4, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_5(x7, 5, r1, r2, r3, r4, s1, s2, s3, s4);
			MUL_BITSLICE_7(x7, 6, r1, r2, r3, r4, s1, s2, s3, s4);


			p1[0] = _mm_unpacklo_epi64(x1[7], x0[7]);
			p2[0] = _mm_unpacklo_epi64(x1[6], x0[6]);
			p3[0] = _mm_unpacklo_epi64(x1[5], x0[5]);
			p4[0] = _mm_unpacklo_epi64(x1[4], x0[4]);
			q1[0] = _mm_unpacklo_epi64(x1[3], x0[3]);
			q2[0] = _mm_unpacklo_epi64(x1[2], x0[2]);
			q3[0] = _mm_unpacklo_epi64(x1[1], x0[1]);
			q4[0] = _mm_unpacklo_epi64(x1[0], x0[0]);

			p1[1] = _mm_unpacklo_epi64(x3[7], x2[7]);
			p2[1] = _mm_unpacklo_epi64(x3[6], x2[6]);
			p3[1] = _mm_unpacklo_epi64(x3[5], x2[5]);
			p4[1] = _mm_unpacklo_epi64(x3[4], x2[4]);
			q1[1] = _mm_unpacklo_epi64(x3[3], x2[3]);
			q2[1] = _mm_unpacklo_epi64(x3[2], x2[2]);
			q3[1] = _mm_unpacklo_epi64(x3[1], x2[1]);
			q4[1] = _mm_unpacklo_epi64(x3[0], x2[0]);

			p1[2] = _mm_unpacklo_epi64(x5[7], x4[7]);
			p2[2] = _mm_unpacklo_epi64(x5[6], x4[6]);
			p3[2] = _mm_unpacklo_epi64(x5[5], x4[5]);
			p4[2] = _mm_unpacklo_epi64(x5[4], x4[4]);
			q1[2] = _mm_unpacklo_epi64(x5[3], x4[3]);
			q2[2] = _mm_unpacklo_epi64(x5[2], x4[2]);
			q3[2] = _mm_unpacklo_epi64(x5[1], x4[1]);
			q4[2] = _mm_unpacklo_epi64(x5[0], x4[0]);

			p1[3] = _mm_unpacklo_epi64(x7[7], x6[7]);
			p2[3] = _mm_unpacklo_epi64(x7[6], x6[6]);
			p3[3] = _mm_unpacklo_epi64(x7[5], x6[5]);
			p4[3] = _mm_unpacklo_epi64(x7[4], x6[4]);
			q1[3] = _mm_unpacklo_epi64(x7[3], x6[3]);
			q2[3] = _mm_unpacklo_epi64(x7[2], x6[2]);
			q3[3] = _mm_unpacklo_epi64(x7[1], x6[1]);
			q4[3] = _mm_unpacklo_epi64(x7[0], x6[0]);

#else

			for(i = 0; i < 8; i ++)
			{
				x01[i] = _mm_xor_si128(x01[i], x01[i]);
				x23[i] = _mm_xor_si128(x23[i], x23[i]);
				x45[i] = _mm_xor_si128(x45[i], x45[i]);
				x67[i] = _mm_xor_si128(x67[i], x67[i]);
			}

			// row 1
			MUL_BITSLICE256_2(x01, ROW_MOV_EE, 0);
			MUL_BITSLICE256_2(x01, ROW_MOV_OE, 0);
			MUL_BITSLICE256_3(x01, ROW_MOV_EE, 1);
			MUL_BITSLICE256_4(x01, ROW_MOV_OE, 1);
			MUL_BITSLICE256_5(x01, ROW_MOV_EE, 2);
			MUL_BITSLICE256_3(x01, ROW_MOV_OE, 2);
			MUL_BITSLICE256_5(x01, ROW_MOV_EE, 3);
			MUL_BITSLICE256_7(x01, ROW_MOV_OE, 3);

			// row2
			MUL_BITSLICE256_7(x01, ROW_MOV_EO, 0);
			MUL_BITSLICE256_2(x01, ROW_MOV_OO, 0);
			MUL_BITSLICE256_2(x01, ROW_MOV_EO, 1);
			MUL_BITSLICE256_3(x01, ROW_MOV_OO, 1);
			MUL_BITSLICE256_4(x01, ROW_MOV_EO, 2);
			MUL_BITSLICE256_5(x01, ROW_MOV_OO, 2);
			MUL_BITSLICE256_3(x01, ROW_MOV_EO, 3);
			MUL_BITSLICE256_5(x01, ROW_MOV_OO, 3);

			// row 3
			MUL_BITSLICE256_5(x23, ROW_MOV_EE, 0);
			MUL_BITSLICE256_7(x23, ROW_MOV_OE, 0);
			MUL_BITSLICE256_2(x23, ROW_MOV_EE, 1);
			MUL_BITSLICE256_2(x23, ROW_MOV_OE, 1);
			MUL_BITSLICE256_3(x23, ROW_MOV_EE, 2);
			MUL_BITSLICE256_4(x23, ROW_MOV_OE, 2);
			MUL_BITSLICE256_5(x23, ROW_MOV_EE, 3);
			MUL_BITSLICE256_3(x23, ROW_MOV_OE, 3);

			// row 4
			MUL_BITSLICE256_3(x23, ROW_MOV_EO, 0);
			MUL_BITSLICE256_5(x23, ROW_MOV_OO, 0);
			MUL_BITSLICE256_7(x23, ROW_MOV_EO, 1);
			MUL_BITSLICE256_2(x23, ROW_MOV_OO, 1);
			MUL_BITSLICE256_2(x23, ROW_MOV_EO, 2);
			MUL_BITSLICE256_3(x23, ROW_MOV_OO, 2);
			MUL_BITSLICE256_4(x23, ROW_MOV_EO, 3);
			MUL_BITSLICE256_5(x23, ROW_MOV_OO, 3);

			// row 5
			MUL_BITSLICE256_5(x45, ROW_MOV_EE, 0);
			MUL_BITSLICE256_3(x45, ROW_MOV_OE, 0);
			MUL_BITSLICE256_5(x45, ROW_MOV_EE, 1);
			MUL_BITSLICE256_7(x45, ROW_MOV_OE, 1);
			MUL_BITSLICE256_2(x45, ROW_MOV_EE, 2);
			MUL_BITSLICE256_2(x45, ROW_MOV_OE, 2);
			MUL_BITSLICE256_3(x45, ROW_MOV_EE, 3);
			MUL_BITSLICE256_4(x45, ROW_MOV_OE, 3);

			// row 6
			MUL_BITSLICE256_4(x45, ROW_MOV_EO, 0);
			MUL_BITSLICE256_5(x45, ROW_MOV_OO, 0);
			MUL_BITSLICE256_3(x45, ROW_MOV_EO, 1);
			MUL_BITSLICE256_5(x45, ROW_MOV_OO, 1);
			MUL_BITSLICE256_7(x45, ROW_MOV_EO, 2);
			MUL_BITSLICE256_2(x45, ROW_MOV_OO, 2);
			MUL_BITSLICE256_2(x45, ROW_MOV_EO, 3);
			MUL_BITSLICE256_3(x45, ROW_MOV_OO, 3);

			// row 7
			MUL_BITSLICE256_3(x67, ROW_MOV_EE, 0);
			MUL_BITSLICE256_4(x67, ROW_MOV_OE, 0);
			MUL_BITSLICE256_5(x67, ROW_MOV_EE, 1);
			MUL_BITSLICE256_3(x67, ROW_MOV_OE, 1);
			MUL_BITSLICE256_5(x67, ROW_MOV_EE, 2);
			MUL_BITSLICE256_7(x67, ROW_MOV_OE, 2);
			MUL_BITSLICE256_2(x67, ROW_MOV_EE, 3);
			MUL_BITSLICE256_2(x67, ROW_MOV_OE, 3);

			// row 8
			MUL_BITSLICE256_2(x67, ROW_MOV_EO, 0);
			MUL_BITSLICE256_3(x67, ROW_MOV_OO, 0);
			MUL_BITSLICE256_4(x67, ROW_MOV_EO, 1);
			MUL_BITSLICE256_5(x67, ROW_MOV_OO, 1);
			MUL_BITSLICE256_3(x67, ROW_MOV_EO, 2);
			MUL_BITSLICE256_5(x67, ROW_MOV_OO, 2);
			MUL_BITSLICE256_7(x67, ROW_MOV_EO, 3);
			MUL_BITSLICE256_2(x67, ROW_MOV_OO, 3);

			p1[0] = x01[7];
			p2[0] = x01[6];
			p3[0] = x01[5];
			p4[0] = x01[4];
			q1[0] = x01[3];
			q2[0] = x01[2];
			q3[0] = x01[1];
			q4[0] = x01[0];

			p1[1] = x23[7];
			p2[1] = x23[6];
			p3[1] = x23[5];
			p4[1] = x23[4];
			q1[1] = x23[3];
			q2[1] = x23[2];
			q3[1] = x23[1];
			q4[1] = x23[0];

			p1[2] = x45[7];
			p2[2] = x45[6];
			p3[2] = x45[5];
			p4[2] = x45[4];
			q1[2] = x45[3];
			q2[2] = x45[2];
			q3[2] = x45[1];
			q4[2] = x45[0];

			p1[3] = x67[7];
			p2[3] = x67[6];
			p3[3] = x67[5];
			p4[3] = x67[4];
			q1[3] = x67[3];
			q2[3] = x67[2];
			q3[3] = x67[1];
			q4[3] = x67[0];
#endif
		}

		BITSLICE(p1[0], p2[0], p3[0], p4[0], q1[0], q2[0], q3[0], q4[0], t0);
		BITSLICE(p1[1], p2[1], p3[1], p4[1], q1[1], q2[1], q3[1], q4[1], t0);
		BITSLICE(p1[2], p2[2], p3[2], p4[2], q1[2], q2[2], q3[2], q4[2], t0);
		BITSLICE(p1[3], p2[3], p3[3], p4[3], q1[3], q2[3], q3[3], q4[3], t0);

		// P ^ Q
		for(i = 0; i < 4; i++)
		{
			ctx->state1[i] = _mm_xor_si128(ctx->state1[i], _mm_xor_si128(p1[i], q1[i]));
			ctx->state2[i] = _mm_xor_si128(ctx->state2[i], _mm_xor_si128(p2[i], q2[i]));
			ctx->state3[i] = _mm_xor_si128(ctx->state3[i], _mm_xor_si128(p3[i], q3[i]));
			ctx->state4[i] = _mm_xor_si128(ctx->state4[i], _mm_xor_si128(p4[i], q4[i]));
		}

		pmsg1 += 64;
		pmsg2 += 64;
		pmsg3 += 64;
		pmsg4 += 64;
	}

	// transpose state back
	TRANSPOSE_BACK(ctx->state1, u, u2);
	TRANSPOSE_BACK(ctx->state2, u, u2);
	TRANSPOSE_BACK(ctx->state3, u, u2);
	TRANSPOSE_BACK(ctx->state4, u, u2);
}

#define TRANSPOSE512(m, u, v)\
	u[0]  = _mm_shuffle_epi8(m[0], M128(_transpose1));\
	u[1]  = _mm_shuffle_epi8(m[1], M128(_transpose1));\
	u[2]  = _mm_shuffle_epi8(m[2], M128(_transpose1));\
	u[3]  = _mm_shuffle_epi8(m[3], M128(_transpose1));\
	u[4]  = _mm_shuffle_epi8(m[4], M128(_transpose1));\
	u[5]  = _mm_shuffle_epi8(m[5], M128(_transpose1));\
	u[6]  = _mm_shuffle_epi8(m[6], M128(_transpose1));\
	u[7]  = _mm_shuffle_epi8(m[7], M128(_transpose1));\
	v[0] = _mm_unpacklo_epi16(u[7], u[6]);\
	v[1] = _mm_unpacklo_epi16(u[5], u[4]);\
	v[2] = _mm_unpacklo_epi16(u[3], u[2]);\
	v[3] = _mm_unpacklo_epi16(u[1], u[0]);\
	v[4] = _mm_unpackhi_epi16(u[7], u[6]);\
	v[5] = _mm_unpackhi_epi16(u[5], u[4]);\
	v[6] = _mm_unpackhi_epi16(u[3], u[2]);\
	v[7] = _mm_unpackhi_epi16(u[1], u[0]);\
	u[0]  = _mm_unpackhi_epi32(v[6], v[7]);\
	u[1]  = _mm_unpacklo_epi32(v[6], v[7]);\
	u[2]  = _mm_unpackhi_epi32(v[4], v[5]);\
	u[3]  = _mm_unpacklo_epi32(v[4], v[5]);\
	u[4]  = _mm_unpackhi_epi32(v[2], v[3]);\
	u[5]  = _mm_unpacklo_epi32(v[2], v[3]);\
	u[6]  = _mm_unpackhi_epi32(v[0], v[1]);\
	u[7]  = _mm_unpacklo_epi32(v[0], v[1]);\
	m[0] = _mm_unpackhi_epi64(u[2], u[0]);\
	m[1] = _mm_unpacklo_epi64(u[2], u[0]);\
	m[2] = _mm_unpackhi_epi64(u[3], u[1]);\
	m[3] = _mm_unpacklo_epi64(u[3], u[1]);\
	m[4] = _mm_unpackhi_epi64(u[6], u[4]);\
	m[5] = _mm_unpacklo_epi64(u[6], u[4]);\
	m[6] = _mm_unpackhi_epi64(u[7], u[5]);\
	m[7] = _mm_unpacklo_epi64(u[7], u[5])


#define TRANSPOSE512_BACK(m, u, v)\
	u[0] = _mm_shuffle_epi8(m[0], M128(_invmask));\
	u[1] = _mm_shuffle_epi8(m[1], M128(_invmask));\
	u[2] = _mm_shuffle_epi8(m[2], M128(_invmask));\
	u[3] = _mm_shuffle_epi8(m[3], M128(_invmask));\
	u[4] = _mm_shuffle_epi8(m[4], M128(_invmask));\
	u[5] = _mm_shuffle_epi8(m[5], M128(_invmask));\
	u[6] = _mm_shuffle_epi8(m[6], M128(_invmask));\
	u[7] = _mm_shuffle_epi8(m[7], M128(_invmask));\
	v[0] = _mm_unpacklo_epi8(u[0], u[1]);\
	v[1] = _mm_unpacklo_epi8(u[2], u[3]);\
	v[2] = _mm_unpacklo_epi8(u[4], u[5]);\
	v[3] = _mm_unpacklo_epi8(u[6], u[7]);\
	v[4] = _mm_unpackhi_epi8(u[0], u[1]);\
	v[5] = _mm_unpackhi_epi8(u[2], u[3]);\
	v[6] = _mm_unpackhi_epi8(u[4], u[5]);\
	v[7] = _mm_unpackhi_epi8(u[6], u[7]);\
	u[0] = _mm_unpacklo_epi16(v[0], v[1]);\
	u[1] = _mm_unpacklo_epi16(v[2], v[3]);\
	u[2] = _mm_unpacklo_epi16(v[4], v[5]);\
	u[3] = _mm_unpacklo_epi16(v[6], v[7]);\
	u[4] = _mm_unpackhi_epi16(v[0], v[1]);\
	u[5] = _mm_unpackhi_epi16(v[2], v[3]);\
	u[6] = _mm_unpackhi_epi16(v[4], v[5]);\
	u[7] = _mm_unpackhi_epi16(v[6], v[7]);\
	m[0] = _mm_unpacklo_epi32(u[0], u[1]);\
	m[1] = _mm_unpackhi_epi32(u[0], u[1]);\
	m[2] = _mm_unpacklo_epi32(u[4], u[5]);\
	m[3] = _mm_unpackhi_epi32(u[4], u[5]);\
	m[4] = _mm_unpacklo_epi32(u[2], u[3]);\
	m[5] = _mm_unpackhi_epi32(u[2], u[3]);\
	m[6] = _mm_unpacklo_epi32(u[6], u[7]);\
	m[7] = _mm_unpackhi_epi32(u[6], u[7])


void Compress512(grssState *ctx,
				 const unsigned char *pmsg1, const unsigned char *pmsg2, const unsigned char *pmsg3, const unsigned char *pmsg4,
				 DataLength uBlockCount)
{

	__m128i u[8], v[8], p1[8], p2[8], p3[8], p4[8], q1[8], q2[8], q3[8], q4[8], t;
	__m128i t0, t1, t2, t3, s0, s1, s2, s3;
	__m128i x0[8], x1[8], x2[8], x3[8], x4[8], x5[8], x6[8], x7[8];
	DataLength b;
	unsigned int i, r;

	// transpose cv
	TRANSPOSE512(ctx->state1, u, v);
	TRANSPOSE512(ctx->state2, u, v);
	TRANSPOSE512(ctx->state3, u, v);
	TRANSPOSE512(ctx->state4, u, v);

	for(b = 0; b < uBlockCount; b++)
	{
		// load message
		for(i = 0; i < 8; i++)
		{
			q1[i] = _mm_loadu_si128((__m128i*)pmsg1 + i);
			q2[i] = _mm_loadu_si128((__m128i*)pmsg2 + i);
			q3[i] = _mm_loadu_si128((__m128i*)pmsg3 + i);
			q4[i] = _mm_loadu_si128((__m128i*)pmsg4 + i);
		}

		// transpose message
		TRANSPOSE512(q1, u, v);
		TRANSPOSE512(q2, u, v);
		TRANSPOSE512(q3, u, v);
		TRANSPOSE512(q4, u, v);

		// xor cv and message
		for(i = 0; i < 8; i++)
		{
			p1[i] = _mm_xor_si128(ctx->state1[i], q1[i]);
			p2[i] = _mm_xor_si128(ctx->state2[i], q2[i]);
			p3[i] = _mm_xor_si128(ctx->state3[i], q3[i]);
			p4[i] = _mm_xor_si128(ctx->state4[i], q4[i]);
		}

		for(i = 0; i < 8; i++)
		{
			BITSLICE(p1[i], p2[i], p3[i], p4[i], q1[i], q2[i], q3[i], q4[i], t);
		}

		for(r = 0; r < 14; r++)
		{
			// add constant
			p1[0] = _mm_xor_si128(p1[0], ctx->_Pconst[r][0]);
			p2[0] = _mm_xor_si128(p2[0], ctx->_Pconst[r][1]);
			p3[0] = _mm_xor_si128(p3[0], ctx->_Pconst[r][2]);
			p4[0] = _mm_xor_si128(p4[0], ctx->_Pconst[r][3]);
			q1[0] = _mm_xor_si128(q1[0], ctx->_Pconst[r][4]);
			q2[0] = _mm_xor_si128(q2[0], ctx->_Pconst[r][5]);
			q3[0] = _mm_xor_si128(q3[0], ctx->_Pconst[r][6]);
			q4[0] = _mm_xor_si128(q4[0], ctx->_Pconst[r][7]);

			p1[7] = _mm_xor_si128(p1[7], ctx->_Qconst[r][0]);
			p2[7] = _mm_xor_si128(p2[7], ctx->_Qconst[r][1]);
			p3[7] = _mm_xor_si128(p3[7], ctx->_Qconst[r][2]);
			p4[7] = _mm_xor_si128(p4[7], ctx->_Qconst[r][3]);
			q1[7] = _mm_xor_si128(q1[7], ctx->_Qconst[r][4]);
			q2[7] = _mm_xor_si128(q2[7], ctx->_Qconst[r][5]);
			q3[7] = _mm_xor_si128(q3[7], ctx->_Qconst[r][6]);
			q4[7] = _mm_xor_si128(q4[7], ctx->_Qconst[r][7]);

			// sub bytes
			for(i = 0; i < 8; i++)
			{
				SUBSTITUTE_BITSLICE(q4[i], q3[i], q2[i], q1[i], p4[i], p3[i], p2[i], p1[i], t0, t1, t2, t3, s0, s1, s2, s3);
			}

			// shift bytes
			for(i = 1; i < 8; i++)
			{
				p1[i] = _mm_shuffle_epi8(p1[i], ctx->_shiftconst[i]);
				p2[i] = _mm_shuffle_epi8(p2[i], ctx->_shiftconst[i]);
				p3[i] = _mm_shuffle_epi8(p3[i], ctx->_shiftconst[i]);
				p4[i] = _mm_shuffle_epi8(p4[i], ctx->_shiftconst[i]);

				q1[i] = _mm_shuffle_epi8(q1[i], ctx->_shiftconst[i]);
				q2[i] = _mm_shuffle_epi8(q2[i], ctx->_shiftconst[i]);
				q3[i] = _mm_shuffle_epi8(q3[i], ctx->_shiftconst[i]);
				q4[i] = _mm_shuffle_epi8(q4[i], ctx->_shiftconst[i]);
			}

			// mix bytes
			for(i = 0; i < 8; i++)
			{
				x0[i] = _mm_xor_si128(x0[i], x0[i]);
				x1[i] = _mm_xor_si128(x1[i], x1[i]);
				x2[i] = _mm_xor_si128(x2[i], x2[i]);
				x3[i] = _mm_xor_si128(x3[i], x3[i]);
				x4[i] = _mm_xor_si128(x4[i], x4[i]);
				x5[i] = _mm_xor_si128(x5[i], x5[i]);
				x6[i] = _mm_xor_si128(x6[i], x6[i]);
				x7[i] = _mm_xor_si128(x7[i], x7[i]);
			}

			MUL_BITSLICE_2(x0, 0, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_2(x0, 1, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x0, 2, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_4(x0, 3, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x0, 4, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x0, 5, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x0, 6, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_7(x0, 7, p1, p2, p3, p4, q1, q2, q3, q4);

			MUL_BITSLICE_2(x1, 1, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_2(x1, 2, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x1, 3, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_4(x1, 4, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x1, 5, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x1, 6, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x1, 7, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_7(x1, 0, p1, p2, p3, p4, q1, q2, q3, q4);

			MUL_BITSLICE_2(x2, 2, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_2(x2, 3, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x2, 4, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_4(x2, 5, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x2, 6, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x2, 7, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x2, 0, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_7(x2, 1, p1, p2, p3, p4, q1, q2, q3, q4);

			MUL_BITSLICE_2(x3, 3, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_2(x3, 4, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x3, 5, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_4(x3, 6, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x3, 7, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x3, 0, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x3, 1, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_7(x3, 2, p1, p2, p3, p4, q1, q2, q3, q4);

			MUL_BITSLICE_2(x4, 4, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_2(x4, 5, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x4, 6, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_4(x4, 7, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x4, 0, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x4, 1, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x4, 2, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_7(x4, 3, p1, p2, p3, p4, q1, q2, q3, q4);

			MUL_BITSLICE_2(x5, 5, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_2(x5, 6, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x5, 7, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_4(x5, 0, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x5, 1, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x5, 2, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x5, 3, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_7(x5, 4, p1, p2, p3, p4, q1, q2, q3, q4);

			MUL_BITSLICE_2(x6, 6, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_2(x6, 7, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x6, 0, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_4(x6, 1, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x6, 2, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x6, 3, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x6, 4, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_7(x6, 5, p1, p2, p3, p4, q1, q2, q3, q4);

			MUL_BITSLICE_2(x7, 7, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_2(x7, 0, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x7, 1, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_4(x7, 2, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x7, 3, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_3(x7, 4, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_5(x7, 5, p1, p2, p3, p4, q1, q2, q3, q4);
			MUL_BITSLICE_7(x7, 6, p1, p2, p3, p4, q1, q2, q3, q4);


			p1[0] = x0[7];
			p2[0] = x0[6];
			p3[0] = x0[5];
			p4[0] = x0[4];
			q1[0] = x0[3];
			q2[0] = x0[2];
			q3[0] = x0[1];
			q4[0] = x0[0];

			p1[1] = x1[7];
			p2[1] = x1[6];
			p3[1] = x1[5];
			p4[1] = x1[4];
			q1[1] = x1[3];
			q2[1] = x1[2];
			q3[1] = x1[1];
			q4[1] = x1[0];

			p1[2] = x2[7];
			p2[2] = x2[6];
			p3[2] = x2[5];
			p4[2] = x2[4];
			q1[2] = x2[3];
			q2[2] = x2[2];
			q3[2] = x2[1];
			q4[2] = x2[0];

			p1[3] = x3[7];
			p2[3] = x3[6];
			p3[3] = x3[5];
			p4[3] = x3[4];
			q1[3] = x3[3];
			q2[3] = x3[2];
			q3[3] = x3[1];
			q4[3] = x3[0];

			p1[4] = x4[7];
			p2[4] = x4[6];
			p3[4] = x4[5];
			p4[4] = x4[4];
			q1[4] = x4[3];
			q2[4] = x4[2];
			q3[4] = x4[1];
			q4[4] = x4[0];

			p1[5] = x5[7];
			p2[5] = x5[6];
			p3[5] = x5[5];
			p4[5] = x5[4];
			q1[5] = x5[3];
			q2[5] = x5[2];
			q3[5] = x5[1];
			q4[5] = x5[0];

			p1[6] = x6[7];
			p2[6] = x6[6];
			p3[6] = x6[5];
			p4[6] = x6[4];
			q1[6] = x6[3];
			q2[6] = x6[2];
			q3[6] = x6[1];
			q4[6] = x6[0];

			p1[7] = x7[7];
			p2[7] = x7[6];
			p3[7] = x7[5];
			p4[7] = x7[4];
			q1[7] = x7[3];
			q2[7] = x7[2];
			q3[7] = x7[1];
			q4[7] = x7[0];

		}


		for(i = 0; i < 8; i++)
		{
			BITSLICE(p1[i], p2[i], p3[i], p4[i], q1[i], q2[i], q3[i], q4[i], t);
		}


		for(i = 0; i < 8; i++)
		{
			ctx->state1[i] = _mm_xor_si128(ctx->state1[i], _mm_xor_si128(p1[i], q1[i]));
			ctx->state2[i] = _mm_xor_si128(ctx->state2[i], _mm_xor_si128(p2[i], q2[i]));
			ctx->state3[i] = _mm_xor_si128(ctx->state3[i], _mm_xor_si128(p3[i], q3[i]));
			ctx->state4[i] = _mm_xor_si128(ctx->state4[i], _mm_xor_si128(p4[i], q4[i]));
		}
	}

	TRANSPOSE512_BACK(ctx->state1, u, v);
	TRANSPOSE512_BACK(ctx->state2, u, v);
	TRANSPOSE512_BACK(ctx->state3, u, v);
	TRANSPOSE512_BACK(ctx->state4, u, v);
}



void grssInit(grssState *pctx, int grssbitlen)
{
	pctx->uHashLength = grssbitlen;
	
	switch(grssbitlen)
	{
		case 256:
			pctx->uBlockLength = 64;
			Init256(pctx);
			break;

		case 512:
			pctx->uBlockLength = 128;
			Init512(pctx);
			break;
	}

}


void grssUpdate(grssState *state, const BitSequence *data, DataLength databitlen)
{
	DataLength uByteLength, uBlockCount;
	
	uByteLength = databitlen / 8;
		
	uBlockCount = uByteLength / state->uBlockLength;


	if(state->uHashLength == 256)
	{
		Compress256(state,
						data + 0 * (uBlockCount / 4) * state->uBlockLength,
						data + 1 * (uBlockCount / 4) * state->uBlockLength,
						data + 2 * (uBlockCount / 4) * state->uBlockLength,
						data + 3 * (uBlockCount / 4) * state->uBlockLength,
						uBlockCount / 4);
	}
	else
	{
		Compress512(state,
						data + 0 * (uBlockCount / 4) * state->uBlockLength,
						data + 1 * (uBlockCount / 4) * state->uBlockLength,
						data + 2 * (uBlockCount / 4) * state->uBlockLength,
						data + 3 * (uBlockCount / 4) * state->uBlockLength,
						1);
						/*uBlockCount / 4); */
	}

}

void grssFinal(grssState *state, BitSequence *grssval)
{
	if(state->uHashLength == 256)
	{
		_mm_storeu_si128((__m128i*)grssval + 0, state->state1[0]);
		_mm_storeu_si128((__m128i*)grssval + 1, state->state1[1]);
	}
	else
	{
		_mm_storeu_si128((__m128i*)grssval + 0, state->state1[0]);
		_mm_storeu_si128((__m128i*)grssval + 1, state->state1[1]);
		_mm_storeu_si128((__m128i*)grssval + 2, state->state1[2]);
		_mm_storeu_si128((__m128i*)grssval + 3, state->state1[3]);
	}

}

void Hash(int hashbitlen, const BitSequence *data, DataLength databitlen, BitSequence *hashval)
{
	grssState hs;
	grssInit(&hs, hashbitlen);
	grssUpdate(&hs, data, databitlen);
	grssFinal(&hs, hashval);
}

