/* groestl-intr-avx.h     Aug 2011
 *
 * Groestl implementation with intrinsics using ssse3, sse4.1, aes and avx
 * instructions.
 * Author: Günther A. Roland, Martin Schläffer, Krystian Matusiewicz
 *
 * This code is placed in the public domain
 */

#include <smmintrin.h>
#include <wmmintrin.h>
#include <immintrin.h>
#include "hash-groestl.h"

/* global constants  */
__m128i ROUND_CONST_Lx;
__m128i ROUND_CONST_L0[ROUNDS512];
__m128i ROUND_CONST_L7[ROUNDS512];
__m128i ROUND_CONST_P[ROUNDS1024];
__m128i ROUND_CONST_Q[ROUNDS1024];
__m128i TRANSP_MASK;
__m128i SUBSH_MASK[8];
__m128i ALL_FF;
#if LENGTH <= 256
__m128i ALL_1B;
#else
__m256d ALL_1B;
#endif

#define tos(a)    #a
#define tostr(a)  tos(a)

#define insert_m128i_in_m256d(ymm, xmm, pos) (_mm256_castsi256_pd(_mm256_insertf128_si256(_mm256_castpd_si256(ymm), xmm, pos)))
#define extract_m128i_from_m256d(ymm, pos) (_mm256_extractf128_si256(_mm256_castpd_si256(ymm), pos))

#if (LENGTH <= 256)

#define SET_CONSTANTS(){\
  ALL_1B = _mm_set_epi32(0x1b1b1b1b, 0x1b1b1b1b, 0x1b1b1b1b, 0x1b1b1b1b);\
  ALL_FF = _mm_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);\
  TRANSP_MASK = _mm_set_epi32(0x0f070b03, 0x0e060a02, 0x0d050901, 0x0c040800);\
  SUBSH_MASK[0] = _mm_set_epi32(0x03060a0d, 0x08020509, 0x0c0f0104, 0x070b0e00);\
  SUBSH_MASK[1] = _mm_set_epi32(0x04070c0f, 0x0a03060b, 0x0e090205, 0x000d0801);\
  SUBSH_MASK[2] = _mm_set_epi32(0x05000e09, 0x0c04070d, 0x080b0306, 0x010f0a02);\
  SUBSH_MASK[3] = _mm_set_epi32(0x0601080b, 0x0e05000f, 0x0a0d0407, 0x02090c03);\
  SUBSH_MASK[4] = _mm_set_epi32(0x0702090c, 0x0f060108, 0x0b0e0500, 0x030a0d04);\
  SUBSH_MASK[5] = _mm_set_epi32(0x00030b0e, 0x0907020a, 0x0d080601, 0x040c0f05);\
  SUBSH_MASK[6] = _mm_set_epi32(0x01040d08, 0x0b00030c, 0x0f0a0702, 0x050e0906);\
  SUBSH_MASK[7] = _mm_set_epi32(0x02050f0a, 0x0d01040e, 0x090c0003, 0x06080b07);\
  for(i = 0; i < ROUNDS512; i++)\
  {\
    ROUND_CONST_L0[i] = _mm_set_epi32(0xffffffff, 0xffffffff, 0x70605040 ^ (i * 0x01010101), 0x30201000 ^ (i * 0x01010101));\
    ROUND_CONST_L7[i] = _mm_set_epi32(0x8f9fafbf ^ (i * 0x01010101), 0xcfdfefff ^ (i * 0x01010101), 0x00000000, 0x00000000);\
  }\
  ROUND_CONST_Lx = _mm_set_epi32(0xffffffff, 0xffffffff, 0x00000000, 0x00000000);\
}while(0);

/* xmm[i] will be multiplied by 2
 * xmm[j] will be lost
 * xmm[k] has to be all 0x1b
 * xmm[z] has to be zero */
#define VMUL2(i, j, k, z){\
  j = _mm_cmpgt_epi8(z, i);\
  i = _mm_add_epi8(i, i);\
  j = _mm_and_si128(j, k);\
  i = _mm_xor_si128(i, j);\
}/**/

/* Yet another implementation of MixBytes.
   This time we use the formulae (3) from the paper "Byte Slicing Groestl".
   Input: a0, ..., a7
   Output: b0, ..., b7 = MixBytes(a0,...,a7).
   but we use the relations:
   t_i = a_i + a_{i+3}
   x_i = t_i + t_{i+3}
   y_i = t_i + t+{i+2} + a_{i+6}
   z_i = 2*x_i
   w_i = z_i + y_{i+4}
   v_i = 2*w_i
   b_i = v_{i+3} + y_{i+4}
   We keep building b_i in registers xmm8..xmm15 by first building y_{i+4} there
   and then adding v_i computed in the meantime in registers xmm0..xmm7.
   We almost fit into 16 registers, need only 3 spills to memory.
   This implementation costs 7.7 c/b giving total speed on SNB: 10.7c/b.
   K. Matusiewicz, 2011/05/29 */
#define MixBytes(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7){\
  /* xmm"tostr(8..xmm"tostr(15 = a2 a3... a0 a1 */\
  b0 = a2;\
  b1 = a3;\
  b2 = a4;\
  b3 = a5;\
  b4 = a6;\
  b5 = a7;\
  b6 = a0;\
  b7 = a1;\
  \
  /* t_i = a_i + a_{i+1} */\
  a0 = _mm_xor_si128(a0, a1);\
  a1 = _mm_xor_si128(a1, a2);\
  a2 = _mm_xor_si128(a2, a3);\
  a3 = _mm_xor_si128(a3, a4);\
  a4 = _mm_xor_si128(a4, a5);\
  a5 = _mm_xor_si128(a5, a6);\
  a6 = _mm_xor_si128(a6, a7);\
  a7 = _mm_xor_si128(a7, b6);\
  \
  /* build y4 y5 y6 ... in regs xmm8, xmm9, xmm10 by adding t_i*/\
  b0 = _mm_xor_si128(b0, a4);\
  b1 = _mm_xor_si128(b1, a5);\
  b2 = _mm_xor_si128(b2, a6);\
  b3 = _mm_xor_si128(b3, a7);\
  b4 = _mm_xor_si128(b4, a0);\
  b5 = _mm_xor_si128(b5, a1);\
  b6 = _mm_xor_si128(b6, a2);\
  b7 = _mm_xor_si128(b7, a3);\
  \
  b0 = _mm_xor_si128(b0, a6);\
  b1 = _mm_xor_si128(b1, a7);\
  b2 = _mm_xor_si128(b2, a0);\
  b3 = _mm_xor_si128(b3, a1);\
  b4 = _mm_xor_si128(b4, a2);\
  b5 = _mm_xor_si128(b5, a3);\
  b6 = _mm_xor_si128(b6, a4);\
  b7 = _mm_xor_si128(b7, a5);\
  \
  /* spill values y_4, y_5 to memory */\
  TEMP0 = b0;\
  TEMP1 = b1;\
  TEMP2 = b2;\
  \
  /* save values t0, t1, t2 to xmm8, xmm9 and memory */\
  b0 = a0;\
  b1 = a1;\
  TEMP3 = a2;\
  \
  /* compute x_i = t_i + t_{i+3} */\
  a0 = _mm_xor_si128(a0, a3);\
  a1 = _mm_xor_si128(a1, a4);\
  a2 = _mm_xor_si128(a2, a5);\
  a3 = _mm_xor_si128(a3, a6);\
  a4 = _mm_xor_si128(a4, a7);\
  a5 = _mm_xor_si128(a5, b0);\
  a6 = _mm_xor_si128(a6, b1);\
  a7 = _mm_xor_si128(a7, TEMP3);\
  \
  /*compute z_i : double x_i using temp xmm8 and 1B xmm9 */\
  b1 = ALL_1B;\
  b2 = _mm_xor_si128(b2, b2);\
  VMUL2(a7, b0, b1, b2);\
  VMUL2(a6, b0, b1, b2);\
  VMUL2(a5, b0, b1, b2);\
  VMUL2(a4, b0, b1, b2);\
  VMUL2(a3, b0, b1, b2);\
  VMUL2(a2, b0, b1, b2);\
  VMUL2(a1, b0, b1, b2);\
  VMUL2(a0, b0, b1, b2);\
  \
  /* compute w_i :  add y_{i+4} */\
  a0 = _mm_xor_si128(a0, TEMP0);\
  a1 = _mm_xor_si128(a1, TEMP1);\
  a2 = _mm_xor_si128(a2, TEMP2);\
  a3 = _mm_xor_si128(a3, b3);\
  a4 = _mm_xor_si128(a4, b4);\
  a5 = _mm_xor_si128(a5, b5);\
  a6 = _mm_xor_si128(a6, b6);\
  a7 = _mm_xor_si128(a7, b7);\
  \
  /*compute v_i: double w_i */\
  VMUL2(a0, b0, b1, b2);\
  VMUL2(a1, b0, b1, b2);\
  VMUL2(a2, b0, b1, b2);\
  VMUL2(a3, b0, b1, b2);\
  VMUL2(a4, b0, b1, b2);\
  VMUL2(a5, b0, b1, b2);\
  VMUL2(a6, b0, b1, b2);\
  VMUL2(a7, b0, b1, b2);\
  \
  /* add to y_4 y_5 .. v3, v4, ... */\
  b0 = _mm_xor_si128(a3, TEMP0);\
  b1 = _mm_xor_si128(a4, TEMP1);\
  b2 = _mm_xor_si128(a5, TEMP2);\
  b3 = _mm_xor_si128(b3, a6);\
  b4 = _mm_xor_si128(b4, a7);\
  b5 = _mm_xor_si128(b5, a0);\
  b6 = _mm_xor_si128(b6, a1);\
  b7 = _mm_xor_si128(b7, a2);\
}/*MixBytes*/

/* one round
 * i = round number
 * a0-a7 = input rows
 * b0-b7 = output rows
 */
#define ROUND(i, a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7){\
  /* Add Round Constant */\
  b1 = ROUND_CONST_Lx;\
  a0 = _mm_xor_si128(a0, (ROUND_CONST_L0[i]));\
  a1 = _mm_xor_si128(a1, b1);\
  a2 = _mm_xor_si128(a2, b1);\
  a3 = _mm_xor_si128(a3, b1);\
  a4 = _mm_xor_si128(a4, b1);\
  a5 = _mm_xor_si128(a5, b1);\
  a6 = _mm_xor_si128(a6, b1);\
  a7 = _mm_xor_si128(a7, (ROUND_CONST_L7[i]));\
  \
  /* ShiftBytes + SubBytes (interleaved) */\
  b0 = _mm_xor_si128(b0,  b0);\
  a0 = _mm_shuffle_epi8(a0, (SUBSH_MASK[0]));\
  a0 = _mm_aesenclast_si128(a0, b0);\
  a1 = _mm_shuffle_epi8(a1, (SUBSH_MASK[1]));\
  a1 = _mm_aesenclast_si128(a1, b0);\
  a2 = _mm_shuffle_epi8(a2, (SUBSH_MASK[2]));\
  a2 = _mm_aesenclast_si128(a2, b0);\
  a3 = _mm_shuffle_epi8(a3, (SUBSH_MASK[3]));\
  a3 = _mm_aesenclast_si128(a3, b0);\
  a4 = _mm_shuffle_epi8(a4, (SUBSH_MASK[4]));\
  a4 = _mm_aesenclast_si128(a4, b0);\
  a5 = _mm_shuffle_epi8(a5, (SUBSH_MASK[5]));\
  a5 = _mm_aesenclast_si128(a5, b0);\
  a6 = _mm_shuffle_epi8(a6, (SUBSH_MASK[6]));\
  a6 = _mm_aesenclast_si128(a6, b0);\
  a7 = _mm_shuffle_epi8(a7, (SUBSH_MASK[7]));\
  a7 = _mm_aesenclast_si128(a7, b0);\
  \
  /* MixBytes */\
  MixBytes(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7);\
}

/* 10 rounds, P and Q in parallel */
#define ROUNDS_P_Q(){\
  ROUND(0, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);\
  ROUND(1, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15);\
  ROUND(2, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);\
  ROUND(3, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15);\
  ROUND(4, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);\
  ROUND(5, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15);\
  ROUND(6, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);\
  ROUND(7, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15);\
  ROUND(8, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);\
  ROUND(9, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15);\
}

/* Matrix Transpose Step 1
 * input is a 512-bit state with two columns in one xmm
 * output is a 512-bit state with two rows in one xmm
 * inputs: i0-i3
 * outputs: i0, o1-o3
 * clobbers: t0
 */
#define Matrix_Transpose_A(i0, i1, i2, i3, o1, o2, o3, t0){\
  t0 = TRANSP_MASK;\
  \
  i0 = _mm_shuffle_epi8(i0, t0);\
  i1 = _mm_shuffle_epi8(i1, t0);\
  i2 = _mm_shuffle_epi8(i2, t0);\
  i3 = _mm_shuffle_epi8(i3, t0);\
  \
  o1 = _mm_unpackhi_epi16(i0, i1);\
  i0 = _mm_unpacklo_epi16(i0, i1);\
  t0 = _mm_unpackhi_epi16(i2, i3);\
  i2 = _mm_unpacklo_epi16(i2, i3);\
  \
  i0 = _mm_shuffle_epi32(i0, 216);\
  o1 = _mm_shuffle_epi32(o1, 216);\
  i2 = _mm_shuffle_epi32(i2, 216);\
  t0 = _mm_shuffle_epi32(t0, 216);\
  \
  o2 = _mm_unpackhi_epi32(i0, i2);\
  o3 = _mm_unpackhi_epi32(o1, t0);\
  i0 = _mm_unpacklo_epi32(i0, i2);\
  o1 = _mm_unpacklo_epi32(o1, t0);\
}/**/

/* Matrix Transpose Step 2
 * input are two 512-bit states with two rows in one xmm
 * output are two 512-bit states with one row of each state in one xmm
 * inputs: i0-i3 = P, i4-i7 = Q
 * outputs: (i0, o1-o7) = (P|Q)
 * possible reassignments: (output reg = input reg)
 * * i1 -> o3-7
 * * i2 -> o5-7
 * * i3 -> o7
 * * i4 -> o3-7
 * * i5 -> o6-7
 */
#define Matrix_Transpose_B(i0, i1, i2, i3, i4, i5, i6, i7, o1, o2, o3, o4, o5, o6, o7){\
  o1 = _mm_unpackhi_epi64(i0, i4);\
  i0 = _mm_unpacklo_epi64(i0, i4);\
  o2 = _mm_unpacklo_epi64(i1, i5);\
  o3 = _mm_unpackhi_epi64(i1, i5);\
  o4 = _mm_unpacklo_epi64(i2, i6);\
  o5 = _mm_unpackhi_epi64(i2, i6);\
  o6 = _mm_unpacklo_epi64(i3, i7);\
  o7 = _mm_unpackhi_epi64(i3, i7);\
}/**/

/* Matrix Transpose Inverse Step 2
 * input are two 512-bit states with one row of each state in one xmm
 * output are two 512-bit states with two rows in one xmm
 * inputs: i0-i7 = (P|Q)
 * outputs: (i0, i2, i4, i6) = P, (o0-o3) = Q
 */
#define Matrix_Transpose_B_INV(i0, i1, i2, i3, i4, i5, i6, i7, o0, o1, o2, o3){\
  o0 = _mm_unpackhi_epi64(i0, i1);\
  i0 = _mm_unpacklo_epi64(i0, i1);\
  o1 = _mm_unpackhi_epi64(i2, i3);\
  i2 = _mm_unpacklo_epi64(i2, i3);\
  o2 = _mm_unpackhi_epi64(i4, i5);\
  i4 = _mm_unpacklo_epi64(i4, i5);\
  o3 = _mm_unpackhi_epi64(i6, i7);\
  i6 = _mm_unpacklo_epi64(i6, i7);\
}/**/

/* Matrix Transpose Output Step 2
 * input is one 512-bit state with two rows in one xmm
 * output is one 512-bit state with one row in the low 64-bits of one xmm
 * inputs: i0,i2,i4,i6 = S
 * outputs: (i0-7) = (0|S)
 */
#define Matrix_Transpose_O_B(i0, i1, i2, i3, i4, i5, i6, i7, t0){\
  t0 = _mm_xor_si128(t0, t0);\
  i1 = _mm_unpackhi_epi64(i0, t0);\
  i0 = _mm_unpacklo_epi64(i0, t0);\
  i3 = _mm_unpackhi_epi64(i2, t0);\
  i2 = _mm_unpacklo_epi64(i2, t0);\
  i5 = _mm_unpackhi_epi64(i4, t0);\
  i4 = _mm_unpacklo_epi64(i4, t0);\
  i7 = _mm_unpackhi_epi64(i6, t0);\
  i6 = _mm_unpacklo_epi64(i6, t0);\
}/**/

/* Matrix Transpose Output Inverse Step 2
 * input is one 512-bit state with one row in the low 64-bits of one xmm
 * output is one 512-bit state with two rows in one xmm
 * inputs: i0-i7 = (0|S)
 * outputs: (i0, i2, i4, i6) = S
 */
#define Matrix_Transpose_O_B_INV(i0, i1, i2, i3, i4, i5, i6, i7){\
  i0 = _mm_unpacklo_epi64(i0, i1);\
  i2 = _mm_unpacklo_epi64(i2, i3);\
  i4 = _mm_unpacklo_epi64(i4, i5);\
  i6 = _mm_unpacklo_epi64(i6, i7);\
}/**/


void INIT(u64* h)
{
  __m128i* const chaining = (__m128i*) h;
  static __m128i xmm0, /*xmm1,*/ xmm2, /*xmm3, xmm4, xmm5,*/ xmm6, xmm7;
  static __m128i /*xmm8, xmm9, xmm10, xmm11,*/ xmm12, xmm13, xmm14, xmm15;

  /* load IV into registers xmm12 - xmm15 */
  xmm12 = chaining[0];
  xmm13 = chaining[1];
  xmm14 = chaining[2];
  xmm15 = chaining[3];

  /* transform chaining value from column ordering into row ordering */
  /* we put two rows (64 bit) of the IV into one 128-bit XMM register */
  Matrix_Transpose_A(xmm12, xmm13, xmm14, xmm15, xmm2, xmm6, xmm7, xmm0);

  /* store transposed IV */
  chaining[0] = xmm12;
  chaining[1] = xmm2;
  chaining[2] = xmm6;
  chaining[3] = xmm7;
}

void TF512(u64* h, u64* m)
{
  __m128i* const chaining = (__m128i*) h;
  __m128i* const message = (__m128i*) m;
  static __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
  static __m128i xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;
  static __m128i TEMP0;
  static __m128i TEMP1;
  static __m128i TEMP2;
  static __m128i TEMP3;

#ifdef IACA_TRACE
  IACA_START;
#endif

  /* load message into registers xmm12 - xmm15 */
  xmm12 = message[0];
  xmm13 = message[1];
  xmm14 = message[2];
  xmm15 = message[3];

  /* transform message M from column ordering into row ordering */
  /* we first put two rows (64 bit) of the message into one 128-bit xmm register */
  Matrix_Transpose_A(xmm12, xmm13, xmm14, xmm15, xmm2, xmm6, xmm7, xmm0);

  /* load previous chaining value and xor message to CV to get input of P */
  /* we first put two rows (2x64 bit) of the CV into one 128-bit xmm register */
  /* result: CV+M in xmm8, xmm0, xmm4, xmm5 */
  xmm8 = _mm_xor_si128(xmm12, chaining[0]);
  xmm0 = _mm_xor_si128(xmm2,  chaining[1]);
  xmm4 = _mm_xor_si128(xmm6,  chaining[2]);
  xmm5 = _mm_xor_si128(xmm7,  chaining[3]);

  /* there are now 2 rows of the Groestl state (P and Q) in each xmm register */
  /* unpack to get 1 row of P (64 bit) and Q (64 bit) into one xmm register */
  /* result: the 8 rows of P and Q in xmm8 - xmm12 */
  Matrix_Transpose_B(xmm8, xmm0, xmm4, xmm5, xmm12, xmm2, xmm6, xmm7, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15);

  /* compute the two permutations P and Q in parallel */
  ROUNDS_P_Q();

  /* unpack again to get two rows of P or two rows of Q in one xmm register */
  Matrix_Transpose_B_INV(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3);

  /* xor output of P and Q */
  /* result: P(CV+M)+Q(M) in xmm0...xmm3 */
  xmm0 = _mm_xor_si128(xmm0, xmm8);
  xmm1 = _mm_xor_si128(xmm1, xmm10);
  xmm2 = _mm_xor_si128(xmm2, xmm12);
  xmm3 = _mm_xor_si128(xmm3, xmm14);

  /* xor CV (feed-forward) */
  /* result: P(CV+M)+Q(M)+CV in xmm0...xmm3 */
  xmm0 = _mm_xor_si128(xmm0, chaining[0]);
  xmm1 = _mm_xor_si128(xmm1, chaining[1]);
  xmm2 = _mm_xor_si128(xmm2, chaining[2]);
  xmm3 = _mm_xor_si128(xmm3, chaining[3]);

  /* store CV */
  chaining[0] = xmm0;
  chaining[1] = xmm1;
  chaining[2] = xmm2;
  chaining[3] = xmm3;

#ifdef IACA_TRACE
  IACA_END;
#endif
  return;
}

void OF512(u64* h)
{
  __m128i* const chaining = (__m128i*) h;
  static __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
  static __m128i xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;
  static __m128i TEMP0;
  static __m128i TEMP1;
  static __m128i TEMP2;
  static __m128i TEMP3;

  /* load CV into registers xmm8, xmm10, xmm12, xmm14 */
  xmm8 = chaining[0];
  xmm10 = chaining[1];
  xmm12 = chaining[2];
  xmm14 = chaining[3];

  /* there are now 2 rows of the CV in one xmm register */
  /* unpack to get 1 row of P (64 bit) into one half of an xmm register */
  /* result: the 8 input rows of P in xmm8 - xmm15 */
  Matrix_Transpose_O_B(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0);

  /* compute the permutation P */
  /* result: the output of P(CV) in xmm8 - xmm15 */
  ROUNDS_P_Q();

  /* unpack again to get two rows of P in one xmm register */
  /* result: P(CV) in xmm8, xmm10, xmm12, xmm14 */
  Matrix_Transpose_O_B_INV(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15);

  /* xor CV to P output (feed-forward) */
  /* result: P(CV)+CV in xmm8, xmm10, xmm12, xmm14 */
  xmm8 = _mm_xor_si128(xmm8,  (chaining[0]));
  xmm10 = _mm_xor_si128(xmm10, (chaining[1]));
  xmm12 = _mm_xor_si128(xmm12, (chaining[2]));
  xmm14 = _mm_xor_si128(xmm14, (chaining[3]));

  /* transform state back from row ordering into column ordering */
  /* result: final hash value in xmm9, xmm11 */
  Matrix_Transpose_A(xmm8, xmm10, xmm12, xmm14, xmm4, xmm9, xmm11, xmm0);

  /* we only need to return the truncated half of the state */
  chaining[2] = xmm9;
  chaining[3] = xmm11;
}

#endif

#if (LENGTH > 256)

#define SET_CONSTANTS(){\
  __m128i xmm0, xmm1;\
  __m256d ymm0;\
  xmm0 = _mm_set_epi32(0x1b1b1b1b, 0x1b1b1b1b, 0x1b1b1b1b, 0x1b1b1b1b);\
  xmm1 = _mm_set_epi32(0x1b1b1b1b, 0x1b1b1b1b, 0x1b1b1b1b, 0x1b1b1b1b);\
  ymm0 = insert_m128i_in_m256d(ymm0, xmm0, 0);\
  ymm0 = insert_m128i_in_m256d(ymm0, xmm1, 1);\
  ALL_1B = ymm0;\
  ALL_FF = _mm_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);\
  TRANSP_MASK = _mm_set_epi32(0x0f070b03, 0x0e060a02, 0x0d050901, 0x0c040800);\
  SUBSH_MASK[0] = _mm_set_epi32(0x0306090c, 0x0f020508, 0x0b0e0104, 0x070a0d00);\
  SUBSH_MASK[1] = _mm_set_epi32(0x04070a0d, 0x00030609, 0x0c0f0205, 0x080b0e01);\
  SUBSH_MASK[2] = _mm_set_epi32(0x05080b0e, 0x0104070a, 0x0d000306, 0x090c0f02);\
  SUBSH_MASK[3] = _mm_set_epi32(0x06090c0f, 0x0205080b, 0x0e010407, 0x0a0d0003);\
  SUBSH_MASK[4] = _mm_set_epi32(0x070a0d00, 0x0306090c, 0x0f020508, 0x0b0e0104);\
  SUBSH_MASK[5] = _mm_set_epi32(0x080b0e01, 0x04070a0d, 0x00030609, 0x0c0f0205);\
  SUBSH_MASK[6] = _mm_set_epi32(0x090c0f02, 0x05080b0e, 0x0104070a, 0x0d000306);\
  SUBSH_MASK[7] = _mm_set_epi32(0x0e010407, 0x0a0d0003, 0x06090c0f, 0x0205080b);\
  for(i = 0; i < ROUNDS1024; i++)\
  {\
    ROUND_CONST_P[i] = _mm_set_epi32(0xf0e0d0c0 ^ (i * 0x01010101), 0xb0a09080 ^ (i * 0x01010101), 0x70605040 ^ (i * 0x01010101), 0x30201000 ^ (i * 0x01010101));\
    ROUND_CONST_Q[i] = _mm_set_epi32(0x0f1f2f3f ^ (i * 0x01010101), 0x4f5f6f7f ^ (i * 0x01010101), 0x8f9fafbf ^ (i * 0x01010101), 0xcfdfefff ^ (i * 0x01010101));\
  }\
}while(0);

/* AVX MUL2
 * input: i
 * output i = 2 * i
 * */
#define VMUL2(i){\
    xmmZERO = _mm_xor_si128(xmmZERO, xmmZERO);\
    xmmIL = extract_m128i_from_m256d(i, 0);\
    xmmIH = extract_m128i_from_m256d(i, 1);\
    xmmJL = _mm_cmpgt_epi8(xmmZERO, xmmIL);\
    xmmJH = _mm_cmpgt_epi8(xmmZERO, xmmIH);\
    xmmIL = _mm_add_epi8(xmmIL, xmmIL);\
    xmmIH = _mm_add_epi8(xmmIH, xmmIH);\
    ymmJ = insert_m128i_in_m256d(ymmJ, xmmJL, 0);\
    ymmJ = insert_m128i_in_m256d(ymmJ, xmmJH, 1);\
    ymmJ = _mm256_and_pd(ymmJ, ALL_1B);\
    i = insert_m128i_in_m256d(i, xmmIL, 0);\
    i = insert_m128i_in_m256d(i, xmmIH, 1);\
    i = _mm256_xor_pd(i, ymmJ);\
}/**/

/* AVX SubShift
 * inputs:
 * * i
 * * c0 (must be 0)
 * * ShiftP
 * * ShiftQ
 * output i = S(Shift(i_1, ShiftQ)|Shift(i_0, ShiftP))
 * clobbers: t0
 * */
#define SubShift(i, ShiftP, ShiftQ){\
  xmmZERO = _mm_xor_si128(xmmZERO, xmmZERO);\
  xmmIL = extract_m128i_from_m256d(i, 0);\
  xmmIH = extract_m128i_from_m256d(i, 1);\
  xmmIL = _mm_shuffle_epi8(xmmIL, SUBSH_MASK[ShiftP]);\
  xmmIH = _mm_shuffle_epi8(xmmIH, SUBSH_MASK[ShiftQ]);\
  xmmIL = _mm_aesenclast_si128(xmmIL, xmmZERO);\
  xmmIH = _mm_aesenclast_si128(xmmIH, xmmZERO);\
  i = insert_m128i_in_m256d(i, xmmIL, 0);\
  i = insert_m128i_in_m256d(i, xmmIH, 1);\
}/**/

/* Yet another implementation of MixBytes.
   This time we use the formulae (3) from the paper "Byte Slicing Groestl".
   Input: a0, ..., a7
   Output: b0, ..., b7 = MixBytes(a0,...,a7).
   but we use the relations:
   t_i = a_i + a_{i+3}
   x_i = t_i + t_{i+3}
   y_i = t_i + t+{i+2} + a_{i+6}
   z_i = 2*x_i
   w_i = z_i + y_{i+4}
   v_i = 2*w_i
   b_i = v_{i+3} + y_{i+4}
   We keep building b_i in registers xmm8..xmm15 by first building y_{i+4} there
   and then adding v_i computed in the meantime in registers xmm0..xmm7.
   We almost fit into 16 registers, need only 3 spills to memory.
   This implementation costs 7.7 c/b giving total speed on SNB: 10.7c/b.
   K. Matusiewicz, 2011/05/29 */
#define MixBytes(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7){\
  /* xmm"tostr(8..xmm"tostr(15 = a2 a3... a0 a1 */\
  b0 = a2;\
  b1 = a3;\
  b2 = a4;\
  b3 = a5;\
  b4 = a6;\
  b5 = a7;\
  b6 = a0;\
  b7 = a1;\
  \
  /* t_i = a_i + a_{i+1} */\
  a0 = _mm256_xor_pd(a0, a1);\
  a1 = _mm256_xor_pd(a1, a2);\
  a2 = _mm256_xor_pd(a2, a3);\
  a3 = _mm256_xor_pd(a3, a4);\
  a4 = _mm256_xor_pd(a4, a5);\
  a5 = _mm256_xor_pd(a5, a6);\
  a6 = _mm256_xor_pd(a6, a7);\
  a7 = _mm256_xor_pd(a7, b6);\
  \
  /* build y4 y5 y6 ... in regs xmm8, xmm9, xmm10 by adding t_i*/\
  b0 = _mm256_xor_pd(b0, a4);\
  b1 = _mm256_xor_pd(b1, a5);\
  b2 = _mm256_xor_pd(b2, a6);\
  b3 = _mm256_xor_pd(b3, a7);\
  b4 = _mm256_xor_pd(b4, a0);\
  b5 = _mm256_xor_pd(b5, a1);\
  b6 = _mm256_xor_pd(b6, a2);\
  b7 = _mm256_xor_pd(b7, a3);\
  \
  b0 = _mm256_xor_pd(b0, a6);\
  b1 = _mm256_xor_pd(b1, a7);\
  b2 = _mm256_xor_pd(b2, a0);\
  b3 = _mm256_xor_pd(b3, a1);\
  b4 = _mm256_xor_pd(b4, a2);\
  b5 = _mm256_xor_pd(b5, a3);\
  b6 = _mm256_xor_pd(b6, a4);\
  b7 = _mm256_xor_pd(b7, a5);\
  \
  /* spill values y_4, y_5 to memory */\
  TEMP0 = b0;\
  TEMP1 = b1;\
  TEMP2 = b2;\
  \
  /* save values t0, t1, t2 to xmm8, xmm9 and memory */\
  b0 = a0;\
  b1 = a1;\
  TEMP3 = a2;\
  \
  /* compute x_i = t_i + t_{i+3} */\
  a0 = _mm256_xor_pd(a0, a3);\
  a1 = _mm256_xor_pd(a1, a4);\
  a2 = _mm256_xor_pd(a2, a5);\
  a3 = _mm256_xor_pd(a3, a6);\
  a4 = _mm256_xor_pd(a4, a7);\
  a5 = _mm256_xor_pd(a5, b0);\
  a6 = _mm256_xor_pd(a6, b1);\
  a7 = _mm256_xor_pd(a7, TEMP3);\
  \
  /*compute z_i : double x_i using temp xmm8 and 1B xmm9 */\
  b1 = ALL_1B;\
  b2 = _mm256_xor_pd(b2, b2);\
  VMUL2(a7);\
  VMUL2(a6);\
  VMUL2(a5);\
  VMUL2(a4);\
  VMUL2(a3);\
  VMUL2(a2);\
  VMUL2(a1);\
  VMUL2(a0);\
  \
  /* compute w_i :  add y_{i+4} */\
  a0 = _mm256_xor_pd(a0, TEMP0);\
  a1 = _mm256_xor_pd(a1, TEMP1);\
  a2 = _mm256_xor_pd(a2, TEMP2);\
  a3 = _mm256_xor_pd(a3, b3);\
  a4 = _mm256_xor_pd(a4, b4);\
  a5 = _mm256_xor_pd(a5, b5);\
  a6 = _mm256_xor_pd(a6, b6);\
  a7 = _mm256_xor_pd(a7, b7);\
  \
  /*compute v_i: double w_i */\
  VMUL2(a0);\
  VMUL2(a1);\
  VMUL2(a2);\
  VMUL2(a3);\
  VMUL2(a4);\
  VMUL2(a5);\
  VMUL2(a6);\
  VMUL2(a7);\
  \
  /* add to y_4 y_5 .. v3, v4, ... */\
  b0 = _mm256_xor_pd(a3, TEMP0);\
  b1 = _mm256_xor_pd(a4, TEMP1);\
  b2 = _mm256_xor_pd(a5, TEMP2);\
  b3 = _mm256_xor_pd(b3, a6);\
  b4 = _mm256_xor_pd(b4, a7);\
  b5 = _mm256_xor_pd(b5, a0);\
  b6 = _mm256_xor_pd(b6, a1);\
  b7 = _mm256_xor_pd(b7, a2);\
}/*MixBytes*/

/* one round
 * a0-a7 = input rows
 * b0-b7 = output rows
 */
#define SUBSHIFTMIX(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7){\
  /* ShiftBytes + SubBytes */\
  SubShift(a0, 0, 1);\
  SubShift(a1, 1, 3);\
  SubShift(a2, 2, 5);\
  SubShift(a3, 3, 7);\
  SubShift(a4, 4, 0);\
  SubShift(a5, 5, 2);\
  SubShift(a6, 6, 4);\
  SubShift(a7, 7, 6);\
  MixBytes(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7);\
}

#define ROUNDS_P_Q(){\
  u8 round_counter = 0;\
  for(round_counter = 0; round_counter < 14; round_counter++) {\
    /* AddRoundConstant */\
    ymm6 = _mm256_xor_pd(ymm6, ymm6);\
    ymm7 = insert_m128i_in_m256d(ymm6, ROUND_CONST_Q[round_counter], 1);\
    ymm6 = insert_m128i_in_m256d(ymm6, ALL_FF, 1);\
    ymm0 = insert_m128i_in_m256d(ymm6, ROUND_CONST_P[round_counter], 0);\
    ymm0 = _mm256_xor_pd(ymm8, ymm0);\
    ymm1 = _mm256_xor_pd(ymm9, ymm6);\
    ymm2 = _mm256_xor_pd(ymm10, ymm6);\
    ymm3 = _mm256_xor_pd(ymm11, ymm6);\
    ymm4 = _mm256_xor_pd(ymm12, ymm6);\
    ymm5 = _mm256_xor_pd(ymm13, ymm6);\
    ymm6 = _mm256_xor_pd(ymm14, ymm6);\
    ymm7 = _mm256_xor_pd(ymm15, ymm7);\
    /* SubBytes + ShiftBytes + MixBytes */\
    SUBSHIFTMIX(ymm0, ymm1, ymm2, ymm3, ymm4, ymm5, ymm6, ymm7, ymm8, ymm9, ymm10, ymm11, ymm12, ymm13, ymm14, ymm15);\
  }\
}

/* Matrix Transpose
 * input is a 1024-bit state with two columns in one xmm
 * output is a 1024-bit state with two rows in one xmm
 * inputs: i0-i7
 * outputs: i0-i7
 * clobbers: t0-t7
 */
#define Matrix_Transpose(i0, i1, i2, i3, i4, i5, i6, i7, t0, t1, t2, t3, t4, t5, t6, t7){\
  t0 = TRANSP_MASK;\
  \
  i6 = _mm_shuffle_epi8(i6, t0);\
  i0 = _mm_shuffle_epi8(i0, t0);\
  i1 = _mm_shuffle_epi8(i1, t0);\
  i2 = _mm_shuffle_epi8(i2, t0);\
  i3 = _mm_shuffle_epi8(i3, t0);\
  i4 = _mm_shuffle_epi8(i4, t0);\
  i5 = _mm_shuffle_epi8(i5, t0);\
  i7 = _mm_shuffle_epi8(i7, t0);\
  \
  /* continue with unpack */\
  t0 = _mm_unpackhi_epi16(i0, i1);\
  t1 = _mm_unpackhi_epi16(i2, i3);\
  t2 = _mm_unpackhi_epi16(i4, i5);\
  t3 = _mm_unpackhi_epi16(i6, i7);\
  i0 = _mm_unpacklo_epi16(i0, i1);\
  i2 = _mm_unpacklo_epi16(i2, i3);\
  i4 = _mm_unpacklo_epi16(i4, i5);\
  i6 = _mm_unpacklo_epi16(i6, i7);\
  \
  /* shuffle with immediate */\
  t0 = _mm_shuffle_epi32(t0, 216);\
  t1 = _mm_shuffle_epi32(t1, 216);\
  t2 = _mm_shuffle_epi32(t2, 216);\
  t3 = _mm_shuffle_epi32(t3, 216);\
  i0 = _mm_shuffle_epi32(i0, 216);\
  i2 = _mm_shuffle_epi32(i2, 216);\
  i4 = _mm_shuffle_epi32(i4, 216);\
  i6 = _mm_shuffle_epi32(i6, 216);\
  \
  /* continue with unpack */\
  t4 = _mm_unpackhi_epi32(i0,  i2);\
  i0 = _mm_unpacklo_epi32(i0,  i2);\
  t5 = _mm_unpackhi_epi32(t0,  t1);\
  t0 = _mm_unpacklo_epi32(t0,  t1);\
  t6 = _mm_unpackhi_epi32(i4,  i6);\
  i4 = _mm_unpacklo_epi32(i4, i6);\
  t7 = _mm_unpackhi_epi32(t2,  t3);\
  t2 = _mm_unpacklo_epi32(t2,  t3);\
  \
  /* there are now 2 rows in each xmm */\
  /* unpack to get 1 row of CV in each xmm */\
  i1 = _mm_unpackhi_epi64(i0, i4);\
  i0 = _mm_unpacklo_epi64(i0, i4);\
  i2 = _mm_unpacklo_epi64(t0, t2);\
  i3 = _mm_unpackhi_epi64(t0, t2);\
  i4 = _mm_unpacklo_epi64(t4, t6);\
  i5 = _mm_unpackhi_epi64(t4, t6);\
  i6 = _mm_unpacklo_epi64(t5, t7);\
  i7 = _mm_unpackhi_epi64(t5, t7);\
  /* transpose done */\
}/**/

/* Matrix Transpose Inverse
 * input is a 1024-bit state with two rows in one xmm
 * output is a 1024-bit state with two columns in one xmm
 * inputs: i0-i7
 * outputs: (i0, o0, i1, i3, o1, o2, i5, i7)
 * clobbers: t0-t4
 */
#define Matrix_Transpose_INV(i0, i1, i2, i3, i4, i5, i6, i7, o0, o1, o2, t0, t1, t2, t3, t4){\
  o0 = TRANSP_MASK;\
  /*  transpose matrix to get output format */\
  o1 = _mm_unpackhi_epi64(i0, i1);\
  i0 = _mm_unpacklo_epi64(i0, i1);\
  t0 = _mm_unpackhi_epi64(i2, i3);\
  i2 = _mm_unpacklo_epi64(i2, i3);\
  t1 = _mm_unpackhi_epi64(i4, i5);\
  i4 = _mm_unpacklo_epi64(i4, i5);\
  t2 = _mm_unpackhi_epi64(i6, i7);\
  i6 = _mm_unpacklo_epi64(i6, i7);\
  /* load transpose mask into a register, because it will be used 8 times */\
  i0 = _mm_shuffle_epi8(i0, o0);\
  i2 = _mm_shuffle_epi8(i2, o0);\
  i4 = _mm_shuffle_epi8(i4, o0);\
  i6 = _mm_shuffle_epi8(i6, o0);\
  o1 = _mm_shuffle_epi8(o1, o0);\
  t0 = _mm_shuffle_epi8(t0, o0);\
  t1 = _mm_shuffle_epi8(t1, o0);\
  t2 = _mm_shuffle_epi8(t2, o0);\
  /* continue with unpack */\
  t3 = _mm_unpackhi_epi16(i4, i6);\
  i4 = _mm_unpacklo_epi16(i4, i6);\
  o0 = _mm_unpackhi_epi16(i0, i2);\
  i0 = _mm_unpacklo_epi16(i0, i2);\
  o2 = _mm_unpackhi_epi16(o1, t0);\
  o1 = _mm_unpacklo_epi16(o1, t0);\
  t4 = _mm_unpackhi_epi16(t1, t2);\
  t1 = _mm_unpacklo_epi16(t1, t2);\
  /* shuffle with immediate */\
  i4 = _mm_shuffle_epi32(i4, 216);\
  t3 = _mm_shuffle_epi32(t3, 216);\
  o1 = _mm_shuffle_epi32(o1, 216);\
  o2 = _mm_shuffle_epi32(o2, 216);\
  i0 = _mm_shuffle_epi32(i0, 216);\
  o0 = _mm_shuffle_epi32(o0, 216);\
  t1 = _mm_shuffle_epi32(t1, 216);\
  t4 = _mm_shuffle_epi32(t4, 216);\
  /* continue with unpack */\
  i1 = _mm_unpackhi_epi32(i0, i4);\
  i0 = _mm_unpacklo_epi32(i0, i4);\
  i3 = _mm_unpackhi_epi32(o0, t3);\
  o0 = _mm_unpacklo_epi32(o0, t3);\
  i5 = _mm_unpackhi_epi32(o1, t1);\
  o1 = _mm_unpacklo_epi32(o1, t1);\
  i7 = _mm_unpackhi_epi32(o2, t4);\
  o2 = _mm_unpacklo_epi32(o2, t4);\
  /* transpose done */\
}/**/

void INIT(u64* h)
{
   __m128i* const chaining = (__m128i*) h;
  static __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
  static __m128i xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;

  /* load IV into registers xmm8 - xmm15 */
  xmm8 = chaining[0];
  xmm9 = chaining[1];
  xmm10 = chaining[2];
  xmm11 = chaining[3];
  xmm12 = chaining[4];
  xmm13 = chaining[5];
  xmm14 = chaining[6];
  xmm15 = chaining[7];

  /* transform chaining value from column ordering into row ordering */
  Matrix_Transpose(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);

  /* store transposed IV */
  chaining[0] = xmm8;
  chaining[1] = xmm9;
  chaining[2] = xmm10;
  chaining[3] = xmm11;
  chaining[4] = xmm12;
  chaining[5] = xmm13;
  chaining[6] = xmm14;
  chaining[7] = xmm15;
}

void TF1024(u64* h, u64* m)
{
  __m128i* const chaining = (__m128i*) h;
  __m128i* const message = (__m128i*) m;
  static __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
  static __m128i xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;
  static __m128i xmmIL, xmmIH, xmmJL, xmmJH, xmmZERO;
  static __m256d ymm0, ymm1, ymm2, ymm3, ymm4, ymm5, ymm6, ymm7;
  static __m256d ymm8, ymm9, ymm10, ymm11, ymm12, ymm13, ymm14, ymm15;
  static __m256d ymmJ;
  static __m256d TEMP0;
  static __m256d TEMP1;
  static __m256d TEMP2;
  static __m256d TEMP3;

#ifdef IACA_TRACE
  IACA_START;
#endif

  /* load message into registers xmm8 - xmm15 (Q = message) */
  xmm0 = message[0];
  xmm1 = message[1];
  xmm2 = message[2];
  xmm3 = message[3];
  xmm4 = message[4];
  xmm5 = message[5];
  xmm6 = message[6];
  xmm7 = message[7];

  /* transform message M from column ordering into row ordering */
  Matrix_Transpose(xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15);

  /* load previous chaining value and xor message to CV to get input of P */
   /* we put two rows (2x64 bit) of the CV into one 128-bit xmm register */
   /* result: CV+M in xmm8...xmm15 */
  xmm8 = _mm_xor_si128(xmm0, chaining[0]);
  xmm9 = _mm_xor_si128(xmm1, chaining[1]);
  xmm10 = _mm_xor_si128(xmm2, chaining[2]);
  xmm11 = _mm_xor_si128(xmm3, chaining[3]);
  xmm12 = _mm_xor_si128(xmm4, chaining[4]);
  xmm13 = _mm_xor_si128(xmm5, chaining[5]);
  xmm14 = _mm_xor_si128(xmm6, chaining[6]);
  xmm15 = _mm_xor_si128(xmm7, chaining[7]);

  /* generate AVX registers with Q in high and P in low 128 bits */
  ymm8 =  insert_m128i_in_m256d(ymm8,  xmm8,  0);
  ymm9 =  insert_m128i_in_m256d(ymm9,  xmm9,  0);
  ymm10 = insert_m128i_in_m256d(ymm10, xmm10, 0);
  ymm11 = insert_m128i_in_m256d(ymm11, xmm11, 0);
  ymm12 = insert_m128i_in_m256d(ymm12, xmm12, 0);
  ymm13 = insert_m128i_in_m256d(ymm13, xmm13, 0);
  ymm14 = insert_m128i_in_m256d(ymm14, xmm14, 0);
  ymm15 = insert_m128i_in_m256d(ymm15, xmm15, 0);

  ymm8 =  insert_m128i_in_m256d(ymm8,  xmm0, 1);
  ymm9 =  insert_m128i_in_m256d(ymm9,  xmm1, 1);
  ymm10 = insert_m128i_in_m256d(ymm10, xmm2, 1);
  ymm11 = insert_m128i_in_m256d(ymm11, xmm3, 1);
  ymm12 = insert_m128i_in_m256d(ymm12, xmm4, 1);
  ymm13 = insert_m128i_in_m256d(ymm13, xmm5, 1);
  ymm14 = insert_m128i_in_m256d(ymm14, xmm6, 1);
  ymm15 = insert_m128i_in_m256d(ymm15, xmm7, 1);

  /* compute the two permutations P and Q in parallel */
  ROUNDS_P_Q();

  /* extract Q to xmm */
  xmm0 = extract_m128i_from_m256d(ymm8, 1);
  xmm1 = extract_m128i_from_m256d(ymm9, 1);
  xmm2 = extract_m128i_from_m256d(ymm10, 1);
  xmm3 = extract_m128i_from_m256d(ymm11, 1);
  xmm4 = extract_m128i_from_m256d(ymm12, 1);
  xmm5 = extract_m128i_from_m256d(ymm13, 1);
  xmm6 = extract_m128i_from_m256d(ymm14, 1);
  xmm7 = extract_m128i_from_m256d(ymm15, 1);

  /* extract P to xmm */
  xmm8  =  extract_m128i_from_m256d(ymm8, 0);
  xmm9  =  extract_m128i_from_m256d(ymm9, 0);
  xmm10 =  extract_m128i_from_m256d(ymm10, 0);
  xmm11 =  extract_m128i_from_m256d(ymm11, 0);
  xmm12 =  extract_m128i_from_m256d(ymm12, 0);
  xmm13 =  extract_m128i_from_m256d(ymm13, 0);
  xmm14 =  extract_m128i_from_m256d(ymm14, 0);
  xmm15 =  extract_m128i_from_m256d(ymm15, 0);

  /* xor output of P and Q */
  /* result: P(CV+M)+Q(M) in xmm8...xmm15 */
  xmm8 = _mm_xor_si128(xmm8,  xmm0);
  xmm9 = _mm_xor_si128(xmm9,  xmm1);
  xmm10 = _mm_xor_si128(xmm10, xmm2);
  xmm11 = _mm_xor_si128(xmm11, xmm3);
  xmm12 = _mm_xor_si128(xmm12, xmm4);
  xmm13 = _mm_xor_si128(xmm13, xmm5);
  xmm14 = _mm_xor_si128(xmm14, xmm6);
  xmm15 = _mm_xor_si128(xmm15, xmm7);

  /* xor CV (feed-forward) */
  /* result: P(CV+M)+Q(M)+CV in xmm8...xmm15 */
  xmm8 = _mm_xor_si128(xmm8,  chaining[0]);
  xmm9 = _mm_xor_si128(xmm9,  chaining[1]);
  xmm10 = _mm_xor_si128(xmm10, chaining[2]);
  xmm11 = _mm_xor_si128(xmm11, chaining[3]);
  xmm12 = _mm_xor_si128(xmm12, chaining[4]);
  xmm13 = _mm_xor_si128(xmm13, chaining[5]);
  xmm14 = _mm_xor_si128(xmm14, chaining[6]);
  xmm15 = _mm_xor_si128(xmm15, chaining[7]);

  /* store CV */
  chaining[0] = xmm8;
  chaining[1] = xmm9;
  chaining[2] = xmm10;
  chaining[3] = xmm11;
  chaining[4] = xmm12;
  chaining[5] = xmm13;
  chaining[6] = xmm14;
  chaining[7] = xmm15;

#ifdef IACA_TRACE
  IACA_END;
#endif
  return;
}

void OF1024(u64* h)
{
  __m128i* const chaining = (__m128i*) h;
  static __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
  static __m128i xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;
  static __m128i xmmIL, xmmIH, xmmJL, xmmJH, xmmZERO;
  static __m256d ymm0, ymm1, ymm2, ymm3, ymm4, ymm5, ymm6, ymm7;
  static __m256d ymm8, ymm9, ymm10, ymm11, ymm12, ymm13, ymm14, ymm15;
  static __m256d ymmJ;
  static __m256d TEMP0;
  static __m256d TEMP1;
  static __m256d TEMP2;
  static __m256d TEMP3;

  /* load CV into registers xmm8...xmm15 */
  xmm8 = chaining[0];
  xmm9 = chaining[1];
  xmm10 = chaining[2];
  xmm11 = chaining[3];
  xmm12 = chaining[4];
  xmm13 = chaining[5];
  xmm14 = chaining[6];
  xmm15 = chaining[7];
  
  xmm0 = _mm_xor_si128(xmm0, xmm0);

  /* generate AVX registers with Q in high and P in low 128 bits */
  ymm8 =  insert_m128i_in_m256d(ymm8,  xmm8,  0);
  ymm9 =  insert_m128i_in_m256d(ymm9,  xmm9,  0);
  ymm10 = insert_m128i_in_m256d(ymm10, xmm10, 0);
  ymm11 = insert_m128i_in_m256d(ymm11, xmm11, 0);
  ymm12 = insert_m128i_in_m256d(ymm12, xmm12, 0);
  ymm13 = insert_m128i_in_m256d(ymm13, xmm13, 0);
  ymm14 = insert_m128i_in_m256d(ymm14, xmm14, 0);
  ymm15 = insert_m128i_in_m256d(ymm15, xmm15, 0);

  ymm8 =  insert_m128i_in_m256d(ymm8,  xmm0, 1);
  ymm9 =  insert_m128i_in_m256d(ymm9,  xmm0, 1);
  ymm10 = insert_m128i_in_m256d(ymm10, xmm0, 1);
  ymm11 = insert_m128i_in_m256d(ymm11, xmm0, 1);
  ymm12 = insert_m128i_in_m256d(ymm12, xmm0, 1);
  ymm13 = insert_m128i_in_m256d(ymm13, xmm0, 1);
  ymm14 = insert_m128i_in_m256d(ymm14, xmm0, 1);
  ymm15 = insert_m128i_in_m256d(ymm15, xmm0, 1);

  /* compute the permutation P */
  /* result: the output of P(CV) in xmm8...xmm15 */
  ROUNDS_P_Q();

  xmm8  =  extract_m128i_from_m256d(ymm8, 0);
  xmm9  =  extract_m128i_from_m256d(ymm9, 0);
  xmm10 =  extract_m128i_from_m256d(ymm10, 0);
  xmm11 =  extract_m128i_from_m256d(ymm11, 0);
  xmm12 =  extract_m128i_from_m256d(ymm12, 0);
  xmm13 =  extract_m128i_from_m256d(ymm13, 0);
  xmm14 =  extract_m128i_from_m256d(ymm14, 0);
  xmm15 =  extract_m128i_from_m256d(ymm15, 0);

  /* xor CV to P output (feed-forward) */
  /* result: P(CV)+CV in xmm8...xmm15 */
  xmm8 = _mm_xor_si128(xmm8,  chaining[0]);
  xmm9 = _mm_xor_si128(xmm9,  chaining[1]);
  xmm10 = _mm_xor_si128(xmm10, chaining[2]);
  xmm11 = _mm_xor_si128(xmm11, chaining[3]);
  xmm12 = _mm_xor_si128(xmm12, chaining[4]);
  xmm13 = _mm_xor_si128(xmm13, chaining[5]);
  xmm14 = _mm_xor_si128(xmm14, chaining[6]);
  xmm15 = _mm_xor_si128(xmm15, chaining[7]);

  /* transpose CV back from row ordering to column ordering */
  /* result: final hash value in xmm0, xmm6, xmm13, xmm15 */
  Matrix_Transpose_INV(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm4, xmm0, xmm6, xmm1, xmm2, xmm3, xmm5, xmm7);

  /* we only need to return the truncated half of the state */
  chaining[0] = xmm8;
  chaining[1] = xmm4;
  chaining[2] = xmm9;
  chaining[3] = xmm11;
  chaining[4] = xmm0;
  chaining[5] = xmm6;
  chaining[6] = xmm13;
  chaining[7] = xmm15;

  return;
}//OF1024()

#endif

