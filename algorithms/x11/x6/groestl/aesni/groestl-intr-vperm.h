/* groestl-intr-vperm.h     Aug 2011
 *
 * Groestl implementation with intrinsics using ssse3 instructions.
 * Author: Günther A. Roland, Martin Schläffer
 *
 * Based on the vperm and aes_ni implementations of the hash function Groestl
 * by Cagdas Calik <ccalik@metu.edu.tr> http://www.metu.edu.tr/~ccalik/
 * Institute of Applied Mathematics, Middle East Technical University, Turkey
 *
 * This code is placed in the public domain
 */

#include <tmmintrin.h>
#include "hash-groestl.h"

/* global constants  */
__m128i ROUND_CONST_Lx;
__m128i ROUND_CONST_L0[ROUNDS512];
__m128i ROUND_CONST_L7[ROUNDS512];
__m128i ROUND_CONST_P[ROUNDS1024];
__m128i ROUND_CONST_Q[ROUNDS1024];
__m128i TRANSP_MASK;
__m128i SUBSH_MASK[8];
__m128i ALL_0F;
__m128i ALL_15;
__m128i ALL_1B;
__m128i ALL_63;
__m128i ALL_FF;
__m128i VPERM_IPT[2];
__m128i VPERM_OPT[2];
__m128i VPERM_INV[2];
__m128i VPERM_SB1[2];
__m128i VPERM_SB2[2];
__m128i VPERM_SB4[2];
__m128i VPERM_SBO[2];


#define tos(a)    #a
#define tostr(a)  tos(a)

#define SET_SHARED_CONSTANTS(){\
  TRANSP_MASK = _mm_set_epi32(0x0f070b03, 0x0e060a02, 0x0d050901, 0x0c040800);\
  ALL_1B = _mm_set_epi32(0x1b1b1b1b, 0x1b1b1b1b, 0x1b1b1b1b, 0x1b1b1b1b);\
  ALL_63 = _mm_set_epi32(0x63636363, 0x63636363, 0x63636363, 0x63636363);\
  ALL_0F = _mm_set_epi32(0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f);\
  ALL_15 = _mm_set_epi32(0x15151515, 0x15151515, 0x15151515, 0x15151515);\
  VPERM_IPT[0] = _mm_set_epi32(0xCD80B1FC, 0xB0FDCC81, 0x4C01307D, 0x317C4D00);\
  VPERM_IPT[1] = _mm_set_epi32(0xCABAE090, 0x52227808, 0xC2B2E898, 0x5A2A7000);\
  VPERM_OPT[0] = _mm_set_epi32(0xE10D5DB1, 0xB05C0CE0, 0x01EDBD51, 0x50BCEC00);\
  VPERM_OPT[1] = _mm_set_epi32(0xF7974121, 0xDEBE6808, 0xFF9F4929, 0xD6B66000);\
  VPERM_INV[0] = _mm_set_epi32(0x030D0E0C, 0x02050809, 0x01040A06, 0x0F0B0780);\
  VPERM_INV[1] = _mm_set_epi32(0x04070309, 0x0A0B0C02, 0x0E05060F, 0x0D080180);\
  VPERM_SB1[0] = _mm_set_epi32(0x3BF7CCC1, 0x0D2ED9EF, 0x3618D415, 0xFAE22300);\
  VPERM_SB1[1] = _mm_set_epi32(0xA5DF7A6E, 0x142AF544, 0xB19BE18F, 0xCB503E00);\
  VPERM_SB2[0] = _mm_set_epi32(0xC2A163C8, 0xAB82234A, 0x69EB8840, 0x0AE12900);\
  VPERM_SB2[1] = _mm_set_epi32(0x5EB7E955, 0xBC982FCD, 0xE27A93C6, 0x0B712400);\
  VPERM_SB4[0] = _mm_set_epi32(0xBA44FE79, 0x876D2914, 0x3D50AED7, 0xC393EA00);\
  VPERM_SB4[1] = _mm_set_epi32(0xA876DE97, 0x49087E9F, 0xE1E937A0, 0x3FD64100);\
}/**/

/* VPERM
 * Transform w/o settings c*
 * transforms 2 rows to/from "vperm mode"
 * this function is derived from:
 *   vperm and aes_ni implementations of hash function Grostl
 *   by Cagdas CALIK
 * inputs:
 * a0, a1 = 2 rows
 * table = transformation table to use
 * t*, c* = clobbers
 * outputs:
 * a0, a1 = 2 rows transformed with table
 * */
#define VPERM_Transform_No_Const(a0, a1, t0, t1, t2, t3, c0, c1, c2){\
  t0 = c0;\
  t1 = c0;\
  t0 = _mm_andnot_si128(t0, a0);\
  t1 = _mm_andnot_si128(t1, a1);\
  t0 = _mm_srli_epi32(t0, 4);\
  t1 = _mm_srli_epi32(t1, 4);\
  a0 = _mm_and_si128(a0, c0);\
  a1 = _mm_and_si128(a1, c0);\
  t2 = c2;\
  t3 = c2;\
  t2 = _mm_shuffle_epi8(t2, a0);\
  t3 = _mm_shuffle_epi8(t3, a1);\
  a0 = c1;\
  a1 = c1;\
  a0 = _mm_shuffle_epi8(a0, t0);\
  a1 = _mm_shuffle_epi8(a1, t1);\
  a0 = _mm_xor_si128(a0, t2);\
  a1 = _mm_xor_si128(a1, t3);\
}/**/

#define VPERM_Transform_Set_Const(table, c0, c1, c2){\
  c0 = ALL_0F;\
  c1 = ((__m128i*) table )[0];\
  c2 = ((__m128i*) table )[1];\
}/**/

/* VPERM
 * Transform
 * transforms 2 rows to/from "vperm mode"
 * this function is derived from:
 *   vperm and aes_ni implementations of hash function Grostl
 *   by Cagdas CALIK
 * inputs:
 * a0, a1 = 2 rows
 * table = transformation table to use
 * t*, c* = clobbers
 * outputs:
 * a0, a1 = 2 rows transformed with table
 * */
#define VPERM_Transform(a0, a1, table, t0, t1, t2, t3, c0, c1, c2){\
  VPERM_Transform_Set_Const(table, c0, c1, c2);\
  VPERM_Transform_No_Const(a0, a1, t0, t1, t2, t3, c0, c1, c2);\
}/**/

/* VPERM
 * Transform State
 * inputs:
 * a0-a3 = state
 * table = transformation table to use
 * t* = clobbers
 * outputs:
 * a0-a3 = transformed state
 * */
#define VPERM_Transform_State(a0, a1, a2, a3, table, t0, t1, t2, t3, c0, c1, c2){\
  VPERM_Transform_Set_Const(table, c0, c1, c2);\
  VPERM_Transform_No_Const(a0, a1, t0, t1, t2, t3, c0, c1, c2);\
  VPERM_Transform_No_Const(a2, a3, t0, t1, t2, t3, c0, c1, c2);\
}/**/

/* VPERM
 * Add Constant to State
 * inputs:
 * a0-a7 = state
 * constant = constant to add
 * t0 = clobber
 * outputs:
 * a0-a7 = state + constant
 * */
#define VPERM_Add_Constant(a0, a1, a2, a3, a4, a5, a6, a7, constant, t0){\
  t0 = constant;\
  a0 = _mm_xor_si128(a0,  t0);\
  a1 = _mm_xor_si128(a1,  t0);\
  a2 = _mm_xor_si128(a2,  t0);\
  a3 = _mm_xor_si128(a3,  t0);\
  a4 = _mm_xor_si128(a4,  t0);\
  a5 = _mm_xor_si128(a5,  t0);\
  a6 = _mm_xor_si128(a6,  t0);\
  a7 = _mm_xor_si128(a7,  t0);\
}/**/

/* VPERM
 * Set Substitute Core Constants
 * */
#define VPERM_Substitute_Core_Set_Const(c0, c1, c2){\
  VPERM_Transform_Set_Const(VPERM_INV, c0, c1, c2);\
}/**/

/* VPERM
 * Substitute Core
 * first part of sbox inverse computation
 * this function is derived from:
 *   vperm and aes_ni implementations of hash function Grostl
 *   by Cagdas CALIK
 * inputs:
 * a0 = 1 row
 * t*, c* = clobbers
 * outputs:
 * b0a, b0b = inputs for lookup step
 * */
#define VPERM_Substitute_Core(a0, b0a, b0b, t0, t1, c0, c1, c2){\
  t0 = c0;\
  t0 = _mm_andnot_si128(t0, a0);\
  t0 = _mm_srli_epi32(t0, 4);\
  a0 = _mm_and_si128(a0,  c0);\
  b0a = c1;\
  b0a = _mm_shuffle_epi8(b0a, a0);\
  a0 = _mm_xor_si128(a0,  t0);\
  b0b = c2;\
  b0b = _mm_shuffle_epi8(b0b, t0);\
  b0b = _mm_xor_si128(b0b, b0a);\
  t1 = c2;\
  t1 = _mm_shuffle_epi8(t1,  a0);\
  t1 = _mm_xor_si128(t1,  b0a);\
  b0a = c2;\
  b0a = _mm_shuffle_epi8(b0a, b0b);\
  b0a = _mm_xor_si128(b0a, a0);\
  b0b = c2;\
  b0b = _mm_shuffle_epi8(b0b, t1);\
  b0b = _mm_xor_si128(b0b, t0);\
}/**/

/* VPERM
 * Lookup
 * second part of sbox inverse computation
 * this function is derived from:
 *   vperm and aes_ni implementations of hash function Grostl
 *   by Cagdas CALIK
 * inputs:
 * a0a, a0b = output of Substitution Core
 * table = lookup table to use (*1 / *2 / *4)
 * t0 = clobber
 * outputs:
 * b0 = output of sbox + multiplication
 * */
#define VPERM_Lookup(a0a, a0b, table, b0, t0){\
  b0 = ((__m128i*) table )[0];\
  t0 = ((__m128i*) table )[1];\
  b0 = _mm_shuffle_epi8(b0, a0b);\
  t0 = _mm_shuffle_epi8(t0, a0a);\
  b0 = _mm_xor_si128(b0, t0);\
}/**/

/* VPERM
 * SubBytes and *2 / *4
 * this function is derived from:
 *   Constant-time SSSE3 AES core implementation
 *   by Mike Hamburg
 * and
 *   vperm and aes_ni implementations of hash function Grostl
 *   by Cagdas CALIK
 * inputs:
 * a0-a7 = state
 * t*, c* = clobbers
 * outputs:
 * a0-a7 = state * 4
 * c2 = row0 * 2 -> b0
 * c1 = row7 * 2 -> b3
 * c0 = row7 * 1 -> b4
 * t2 = row4 * 1 -> b7
 * TEMP_MUL1 = row(i) * 1
 * TEMP_MUL2 = row(i) * 2
 *
 * call:VPERM_SUB_MULTIPLY(a0, a1, a2, a3, a4, a5, a6, a7, b1, b2, b5, b6, b0, b3, b4, b7) */
#define VPERM_SUB_MULTIPLY(a0, a1, a2, a3, a4, a5, a6, a7, t0, t1, t3, t4, c2, c1, c0, t2){\
  /* set Constants */\
  VPERM_Substitute_Core_Set_Const(c0, c1, c2);\
  /* row 1 */\
  VPERM_Substitute_Core(a1, t0, t1, t3, t4, c0, c1, c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, t2, t4);\
  TEMP_MUL1[1] = t2;\
  VPERM_Lookup(t0, t1, VPERM_SB2, t3, t4);\
  TEMP_MUL2[1] = t3;\
  VPERM_Lookup(t0, t1, VPERM_SB4, a1, t4);\
  /* --- */\
  /* row 2 */\
  VPERM_Substitute_Core(a2, t0, t1, t3, t4, c0, c1, c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, t2, t4);\
  TEMP_MUL1[2] = t2;\
  VPERM_Lookup(t0, t1, VPERM_SB2, t3, t4);\
  TEMP_MUL2[2] = t3;\
  VPERM_Lookup(t0, t1, VPERM_SB4, a2, t4);\
  /* --- */\
  /* row 3 */\
  VPERM_Substitute_Core(a3, t0, t1, t3, t4, c0, c1, c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, t2, t4);\
  TEMP_MUL1[3] = t2;\
  VPERM_Lookup(t0, t1, VPERM_SB2, t3, t4);\
  TEMP_MUL2[3] = t3;\
  VPERM_Lookup(t0, t1, VPERM_SB4, a3, t4);\
  /* --- */\
  /* row 5 */\
  VPERM_Substitute_Core(a5, t0, t1, t3, t4, c0, c1, c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, t2, t4);\
  TEMP_MUL1[5] = t2;\
  VPERM_Lookup(t0, t1, VPERM_SB2, t3, t4);\
  TEMP_MUL2[5] = t3;\
  VPERM_Lookup(t0, t1, VPERM_SB4, a5, t4);\
  /* --- */\
  /* row 6 */\
  VPERM_Substitute_Core(a6, t0, t1, t3, t4, c0, c1, c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, t2, t4);\
  TEMP_MUL1[6] = t2;\
  VPERM_Lookup(t0, t1, VPERM_SB2, t3, t4);\
  TEMP_MUL2[6] = t3;\
  VPERM_Lookup(t0, t1, VPERM_SB4, a6, t4);\
  /* --- */\
  /* row 7 */\
  VPERM_Substitute_Core(a7, t0, t1, t3, t4, c0, c1, c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, t2, t4);\
  TEMP_MUL1[7] = t2;\
  VPERM_Lookup(t0, t1, VPERM_SB2, c1, t4); /*c1 -> b3*/\
  VPERM_Lookup(t0, t1, VPERM_SB4, a7, t4);\
  /* --- */\
  /* row 4 */\
  VPERM_Substitute_Core(a4, t0, t1, t3, t4, c0, (VPERM_INV[0]), c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, t2, t4); /*t2 -> b7*/\
  VPERM_Lookup(t0, t1, VPERM_SB2, t3, t4);\
  TEMP_MUL2[4] = t3;\
  VPERM_Lookup(t0, t1, VPERM_SB4, a4, t4);\
  /* --- */\
  /* row 0 */\
  VPERM_Substitute_Core(a0, t0, t1, t3, t4, c0, (VPERM_INV[0]), c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, c0, t4); /*c0 -> b4*/\
  VPERM_Lookup(t0, t1, VPERM_SB2, c2, t4); /*c2 -> b0*/\
  TEMP_MUL2[0] = c2;\
  VPERM_Lookup(t0, t1, VPERM_SB4, a0, t4);\
  /* --- */\
}/**/


/* Optimized MixBytes
 * inputs:
 * a0-a7 = (row0-row7) * 4
 * b0 = row0 * 2
 * b3 = row7 * 2
 * b4 = row7 * 1
 * b7 = row4 * 1
 * all *1 and *2 values must also be in TEMP_MUL1, TEMP_MUL2
 * output: b0-b7
 * */
#define MixBytes(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7){\
  /* save one value */\
  TEMP_MUL4 = a3;\
  /* 1 */\
  b1 = a0;\
  b1 = _mm_xor_si128(b1, a5);\
  b1 = _mm_xor_si128(b1, b4); /* -> helper! */\
  b1 = _mm_xor_si128(b1, (TEMP_MUL2[3]));\
  b2 = b1;\
  \
  /* 2 */\
  b5 = a1;\
  b5 = _mm_xor_si128(b5, a4);\
  b5 = _mm_xor_si128(b5, b7); /* -> helper! */\
  b5 = _mm_xor_si128(b5, b3); /* -> helper! */\
  b6 = b5;\
  \
  /* 4 */\
  b7 = _mm_xor_si128(b7, a6);\
  /*b7 = _mm_xor_si128(b7, (TEMP_MUL1[4])); -> helper! */\
  b7 = _mm_xor_si128(b7, (TEMP_MUL1[6]));\
  b7 = _mm_xor_si128(b7, (TEMP_MUL2[1]));\
  b7 = _mm_xor_si128(b7, b3); /* -> helper! */\
  b2 = _mm_xor_si128(b2, b7);\
  \
  /* 3 */\
  b0 = _mm_xor_si128(b0, a7);\
  b0 = _mm_xor_si128(b0, (TEMP_MUL1[5]));\
  b0 = _mm_xor_si128(b0, (TEMP_MUL1[7]));\
  /*b0 = _mm_xor_si128(b0, (TEMP_MUL2[0])); -> helper! */\
  b0 = _mm_xor_si128(b0, (TEMP_MUL2[2]));\
  b3 = b0;\
  b1 = _mm_xor_si128(b1, b0);\
  b0 = _mm_xor_si128(b0, b7); /* moved from 4 */\
  \
  /* 5 */\
  b4 = _mm_xor_si128(b4, a2);\
  /*b4 = _mm_xor_si128(b4, (TEMP_MUL1[0])); -> helper! */\
  b4 = _mm_xor_si128(b4, (TEMP_MUL1[2]));\
  b4 = _mm_xor_si128(b4, (TEMP_MUL2[3]));\
  b4 = _mm_xor_si128(b4, (TEMP_MUL2[5]));\
  b3 = _mm_xor_si128(b3, b4);\
  b6 = _mm_xor_si128(b6, b4);\
  \
  /* 6 */\
  a3 = _mm_xor_si128(a3, (TEMP_MUL1[1]));\
  a3 = _mm_xor_si128(a3, (TEMP_MUL1[3]));\
  a3 = _mm_xor_si128(a3, (TEMP_MUL2[4]));\
  a3 = _mm_xor_si128(a3, (TEMP_MUL2[6]));\
  b4 = _mm_xor_si128(b4, a3);\
  b5 = _mm_xor_si128(b5, a3);\
  b7 = _mm_xor_si128(b7, a3);\
  \
  /* 7 */\
  a1 = _mm_xor_si128(a1, (TEMP_MUL1[1]));\
  a1 = _mm_xor_si128(a1, (TEMP_MUL2[4]));\
  b2 = _mm_xor_si128(b2, a1);\
  b3 = _mm_xor_si128(b3, a1);\
  \
  /* 8 */\
  a5 = _mm_xor_si128(a5, (TEMP_MUL1[5]));\
  a5 = _mm_xor_si128(a5, (TEMP_MUL2[0]));\
  b6 = _mm_xor_si128(b6, a5);\
  b7 = _mm_xor_si128(b7, a5);\
  \
  /* 9 */\
  a3 = TEMP_MUL1[2];\
  a3 = _mm_xor_si128(a3, (TEMP_MUL2[5]));\
  b0 = _mm_xor_si128(b0, a3);\
  b5 = _mm_xor_si128(b5, a3);\
  \
  /* 10 */\
  a1 = TEMP_MUL1[6];\
  a1 = _mm_xor_si128(a1, (TEMP_MUL2[1]));\
  b1 = _mm_xor_si128(b1, a1);\
  b4 = _mm_xor_si128(b4, a1);\
  \
  /* 11 */\
  a5 = TEMP_MUL1[3];\
  a5 = _mm_xor_si128(a5, (TEMP_MUL2[6]));\
  b1 = _mm_xor_si128(b1, a5);\
  b6 = _mm_xor_si128(b6, a5);\
  \
  /* 12 */\
  a3 = TEMP_MUL1[7];\
  a3 = _mm_xor_si128(a3, (TEMP_MUL2[2]));\
  b2 = _mm_xor_si128(b2, a3);\
  b5 = _mm_xor_si128(b5, a3);\
  \
  /* 13 */\
  b0 = _mm_xor_si128(b0, (TEMP_MUL4));\
  b0 = _mm_xor_si128(b0, a4);\
  b1 = _mm_xor_si128(b1, a4);\
  b3 = _mm_xor_si128(b3, a6);\
  b4 = _mm_xor_si128(b4, a0);\
  b4 = _mm_xor_si128(b4, a7);\
  b5 = _mm_xor_si128(b5, a0);\
  b7 = _mm_xor_si128(b7, a2);\
}/**/

#if (LENGTH <= 256)

#define SET_CONSTANTS(){\
  SET_SHARED_CONSTANTS();\
  SUBSH_MASK[0] = _mm_set_epi32(0x080f0e0d, 0x0c0b0a09, 0x07060504, 0x03020100);\
  SUBSH_MASK[1] = _mm_set_epi32(0x0a09080f, 0x0e0d0c0b, 0x00070605, 0x04030201);\
  SUBSH_MASK[2] = _mm_set_epi32(0x0c0b0a09, 0x080f0e0d, 0x01000706, 0x05040302);\
  SUBSH_MASK[3] = _mm_set_epi32(0x0e0d0c0b, 0x0a09080f, 0x02010007, 0x06050403);\
  SUBSH_MASK[4] = _mm_set_epi32(0x0f0e0d0c, 0x0b0a0908, 0x03020100, 0x07060504);\
  SUBSH_MASK[5] = _mm_set_epi32(0x09080f0e, 0x0d0c0b0a, 0x04030201, 0x00070605);\
  SUBSH_MASK[6] = _mm_set_epi32(0x0b0a0908, 0x0f0e0d0c, 0x05040302, 0x01000706);\
  SUBSH_MASK[7] = _mm_set_epi32(0x0d0c0b0a, 0x09080f0e, 0x06050403, 0x02010007);\
  for(i = 0; i < ROUNDS512; i++)\
  {\
    ROUND_CONST_L0[i] = _mm_set_epi32(0xffffffff, 0xffffffff, 0x70605040 ^ (i * 0x01010101), 0x30201000 ^ (i * 0x01010101));\
    ROUND_CONST_L7[i] = _mm_set_epi32(0x8f9fafbf ^ (i * 0x01010101), 0xcfdfefff ^ (i * 0x01010101), 0x00000000, 0x00000000);\
  }\
  ROUND_CONST_Lx = _mm_set_epi32(0xffffffff, 0xffffffff, 0x00000000, 0x00000000);\
}/**/

/* vperm:
 * transformation before rounds with ipt
 * first round add transformed constant
 * middle rounds: add constant XOR 0x15...15
 * last round: additionally add 0x15...15 after MB
 * transformation after rounds with opt
 */
/* one round
 * i = round number
 * a0-a7 = input rows
 * b0-b7 = output rows
 */
#define ROUND(i, a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7){\
  /* AddRoundConstant + ShiftBytes (interleaved) */\
  b1 = ROUND_CONST_Lx;\
  a0 = _mm_xor_si128(a0, (ROUND_CONST_L0[i]));\
  a1 = _mm_xor_si128(a1, b1);\
  a2 = _mm_xor_si128(a2, b1);\
  a3 = _mm_xor_si128(a3, b1);\
  a0 = _mm_shuffle_epi8(a0, (SUBSH_MASK[0]));\
  a1 = _mm_shuffle_epi8(a1, (SUBSH_MASK[1]));\
  a4 = _mm_xor_si128(a4, b1);\
  a2 = _mm_shuffle_epi8(a2, (SUBSH_MASK[2]));\
  a3 = _mm_shuffle_epi8(a3, (SUBSH_MASK[3]));\
  a5 = _mm_xor_si128(a5, b1);\
  a6 = _mm_xor_si128(a6, b1);\
  a4 = _mm_shuffle_epi8(a4, (SUBSH_MASK[4]));\
  a5 = _mm_shuffle_epi8(a5, (SUBSH_MASK[5]));\
  a7 = _mm_xor_si128(a7, (ROUND_CONST_L7[i]));\
  a6 = _mm_shuffle_epi8(a6, (SUBSH_MASK[6]));\
  a7 = _mm_shuffle_epi8(a7, (SUBSH_MASK[7]));\
  /* SubBytes + Multiplication by 2 and 4 */\
  VPERM_SUB_MULTIPLY(a0, a1, a2, a3, a4, a5, a6, a7, b1, b2, b5, b6, b0, b3, b4, b7);\
  /* MixBytes */\
  MixBytes(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7);\
}/**/

/* 10 rounds, P and Q in parallel */
#define ROUNDS_P_Q(){\
  VPERM_Add_Constant(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, ALL_15, xmm0);\
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
  VPERM_Add_Constant(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, ALL_15, xmm0);\
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
  o1 = i0;\
  t0 = i2;\
\
  i0 = _mm_unpacklo_epi16(i0, i1);\
  o1 = _mm_unpackhi_epi16(o1, i1);\
  i2 = _mm_unpacklo_epi16(i2, i3);\
  t0 = _mm_unpackhi_epi16(t0, i3);\
\
  i0 = _mm_shuffle_epi32(i0, 216);\
  o1 = _mm_shuffle_epi32(o1, 216);\
  i2 = _mm_shuffle_epi32(i2, 216);\
  t0 = _mm_shuffle_epi32(t0, 216);\
\
  o2 = i0;\
  o3 = o1;\
\
  i0 = _mm_unpacklo_epi32(i0, i2);\
  o1 = _mm_unpacklo_epi32(o1, t0);\
  o2 = _mm_unpackhi_epi32(o2, i2);\
  o3 = _mm_unpackhi_epi32(o3, t0);\
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
  o1 = i0;\
  o2 = i1;\
  i0 = _mm_unpacklo_epi64(i0, i4);\
  o1 = _mm_unpackhi_epi64(o1, i4);\
  o3 = i1;\
  o4 = i2;\
  o2 = _mm_unpacklo_epi64(o2, i5);\
  o3 = _mm_unpackhi_epi64(o3, i5);\
  o5 = i2;\
  o6 = i3;\
  o4 = _mm_unpacklo_epi64(o4, i6);\
  o5 = _mm_unpackhi_epi64(o5, i6);\
  o7 = i3;\
  o6 = _mm_unpacklo_epi64(o6, i7);\
  o7 = _mm_unpackhi_epi64(o7, i7);\
}/**/

/* Matrix Transpose Inverse Step 2
 * input are two 512-bit states with one row of each state in one xmm
 * output are two 512-bit states with two rows in one xmm
 * inputs: i0-i7 = (P|Q)
 * outputs: (i0, i2, i4, i6) = P, (o0-o3) = Q
 */
#define Matrix_Transpose_B_INV(i0, i1, i2, i3, i4, i5, i6, i7, o0, o1, o2, o3){\
  o0 = i0;\
  i0 = _mm_unpacklo_epi64(i0, i1);\
  o0 = _mm_unpackhi_epi64(o0, i1);\
  o1 = i2;\
  i2 = _mm_unpacklo_epi64(i2, i3);\
  o1 = _mm_unpackhi_epi64(o1, i3);\
  o2 = i4;\
  i4 = _mm_unpacklo_epi64(i4, i5);\
  o2 = _mm_unpackhi_epi64(o2, i5);\
  o3 = i6;\
  i6 = _mm_unpacklo_epi64(i6, i7);\
  o3 = _mm_unpackhi_epi64(o3, i7);\
}/**/

/* Matrix Transpose Output Step 2
 * input is one 512-bit state with two rows in one xmm
 * output is one 512-bit state with one row in the low 64-bits of one xmm
 * inputs: i0,i2,i4,i6 = S
 * outputs: (i0-7) = (0|S)
 */
#define Matrix_Transpose_O_B(i0, i1, i2, i3, i4, i5, i6, i7, t0){\
  t0 = _mm_xor_si128(t0, t0);\
  i1 = i0;\
  i3 = i2;\
  i5 = i4;\
  i7 = i6;\
  i0 = _mm_unpacklo_epi64(i0, t0);\
  i1 = _mm_unpackhi_epi64(i1, t0);\
  i2 = _mm_unpacklo_epi64(i2, t0);\
  i3 = _mm_unpackhi_epi64(i3, t0);\
  i4 = _mm_unpacklo_epi64(i4, t0);\
  i5 = _mm_unpackhi_epi64(i5, t0);\
  i6 = _mm_unpacklo_epi64(i6, t0);\
  i7 = _mm_unpackhi_epi64(i7, t0);\
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


/* transform round constants into VPERM mode */
#define VPERM_Transform_RoundConst_CNT2(i, j){\
  xmm0 = ROUND_CONST_L0[i];\
  xmm1 = ROUND_CONST_L7[i];\
  xmm2 = ROUND_CONST_L0[j];\
  xmm3 = ROUND_CONST_L7[j];\
  VPERM_Transform_State(xmm0, xmm1, xmm2, xmm3, VPERM_IPT, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10);\
  xmm0 = _mm_xor_si128(xmm0, (ALL_15));\
  xmm1 = _mm_xor_si128(xmm1, (ALL_15));\
  xmm2 = _mm_xor_si128(xmm2, (ALL_15));\
  xmm3 = _mm_xor_si128(xmm3, (ALL_15));\
  ROUND_CONST_L0[i] = xmm0;\
  ROUND_CONST_L7[i] = xmm1;\
  ROUND_CONST_L0[j] = xmm2;\
  ROUND_CONST_L7[j] = xmm3;\
}/**/

/* transform round constants into VPERM mode */
#define VPERM_Transform_RoundConst(){\
  xmm0 = ROUND_CONST_Lx;\
  VPERM_Transform(xmm0, xmm1, VPERM_IPT, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10);\
  xmm0 = _mm_xor_si128(xmm0, (ALL_15));\
  ROUND_CONST_Lx = xmm0;\
  VPERM_Transform_RoundConst_CNT2(0, 1);\
  VPERM_Transform_RoundConst_CNT2(2, 3);\
  VPERM_Transform_RoundConst_CNT2(4, 5);\
  VPERM_Transform_RoundConst_CNT2(6, 7);\
  VPERM_Transform_RoundConst_CNT2(8, 9);\
}/**/

void INIT(u64* h)
{
  __m128i* const chaining = (__m128i*) h;
  static __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
  static __m128i xmm8, xmm9, xmm10, /*xmm11,*/ xmm12, xmm13, xmm14, xmm15;

  /* transform round constants into VPERM mode */
  VPERM_Transform_RoundConst();

  /* load IV into registers xmm12 - xmm15 */
  xmm12 = chaining[0];
  xmm13 = chaining[1];
  xmm14 = chaining[2];
  xmm15 = chaining[3];

  /* transform chaining value from column ordering into row ordering */
  /* we put two rows (64 bit) of the IV into one 128-bit XMM register */
  VPERM_Transform_State(xmm12, xmm13, xmm14, xmm15, VPERM_IPT, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);
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
  static __m128i TEMP_MUL1[8];
  static __m128i TEMP_MUL2[8];
  static __m128i TEMP_MUL4;

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
  VPERM_Transform_State(xmm12, xmm13, xmm14, xmm15, VPERM_IPT, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);
  Matrix_Transpose_A(xmm12, xmm13, xmm14, xmm15, xmm2, xmm6, xmm7, xmm0);

  /* load previous chaining value */
  /* we first put two rows (64 bit) of the CV into one 128-bit xmm register */
  xmm8 = chaining[0];
  xmm0 = chaining[1];
  xmm4 = chaining[2];
  xmm5 = chaining[3];

  /* xor message to CV get input of P */
  /* result: CV+M in xmm8, xmm0, xmm4, xmm5 */
  xmm8 = _mm_xor_si128(xmm8, xmm12);
  xmm0 = _mm_xor_si128(xmm0, xmm2);
  xmm4 = _mm_xor_si128(xmm4, xmm6);
  xmm5 = _mm_xor_si128(xmm5, xmm7);

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
  xmm0 = _mm_xor_si128(xmm0, (chaining[0]));
  xmm1 = _mm_xor_si128(xmm1, (chaining[1]));
  xmm2 = _mm_xor_si128(xmm2, (chaining[2]));
  xmm3 = _mm_xor_si128(xmm3, (chaining[3]));

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
  static __m128i TEMP_MUL1[8];
  static __m128i TEMP_MUL2[8];
  static __m128i TEMP_MUL4;

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
  VPERM_Transform(xmm9, xmm11, VPERM_OPT, xmm0, xmm1, xmm2, xmm3, xmm5, xmm6, xmm7);

  /* we only need to return the truncated half of the state */
  chaining[2] = xmm9;
  chaining[3] = xmm11;

  return;
}//OF512()

#endif

#if (LENGTH > 256)

#define SET_CONSTANTS(){\
  SET_SHARED_CONSTANTS();\
  ALL_FF = _mm_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);\
  SUBSH_MASK[0] = _mm_set_epi32(0x0f0e0d0c, 0x0b0a0908, 0x07060504, 0x03020100);\
  SUBSH_MASK[1] = _mm_set_epi32(0x000f0e0d, 0x0c0b0a09, 0x08070605, 0x04030201);\
  SUBSH_MASK[2] = _mm_set_epi32(0x01000f0e, 0x0d0c0b0a, 0x09080706, 0x05040302);\
  SUBSH_MASK[3] = _mm_set_epi32(0x0201000f, 0x0e0d0c0b, 0x0a090807, 0x06050403);\
  SUBSH_MASK[4] = _mm_set_epi32(0x03020100, 0x0f0e0d0c, 0x0b0a0908, 0x07060504);\
  SUBSH_MASK[5] = _mm_set_epi32(0x04030201, 0x000f0e0d, 0x0c0b0a09, 0x08070605);\
  SUBSH_MASK[6] = _mm_set_epi32(0x05040302, 0x01000f0e, 0x0d0c0b0a, 0x09080706);\
  SUBSH_MASK[7] = _mm_set_epi32(0x0a090807, 0x06050403, 0x0201000f, 0x0e0d0c0b);\
  for(i = 0; i < ROUNDS1024; i++)\
  {\
    ROUND_CONST_P[i] = _mm_set_epi32(0xf0e0d0c0 ^ (i * 0x01010101), 0xb0a09080 ^ (i * 0x01010101), 0x70605040 ^ (i * 0x01010101), 0x30201000 ^ (i * 0x01010101));\
    ROUND_CONST_Q[i] = _mm_set_epi32(0x0f1f2f3f ^ (i * 0x01010101), 0x4f5f6f7f ^ (i * 0x01010101), 0x8f9fafbf ^ (i * 0x01010101), 0xcfdfefff ^ (i * 0x01010101));\
  }\
}/**/

/* one round
 * a0-a7 = input rows
 * b0-b7 = output rows
 */
#define SUBMIX(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7){\
  /* SubBytes + Multiplication */\
  VPERM_SUB_MULTIPLY(a0, a1, a2, a3, a4, a5, a6, a7, b1, b2, b5, b6, b0, b3, b4, b7);\
  /* MixBytes */\
  MixBytes(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7);\
}/**/

#define ROUNDS_P(){\
  u8 round_counter = 0;\
  for(round_counter = 0; round_counter < 14; round_counter+=2) {\
    /* AddRoundConstant P1024 */\
    xmm8 = _mm_xor_si128(xmm8, (ROUND_CONST_P[round_counter]));\
    /* ShiftBytes P1024 + pre-AESENCLAST */\
    xmm8 = _mm_shuffle_epi8(xmm8,  (SUBSH_MASK[0]));\
    xmm9 = _mm_shuffle_epi8(xmm9,  (SUBSH_MASK[1]));\
    xmm10 = _mm_shuffle_epi8(xmm10, (SUBSH_MASK[2]));\
    xmm11 = _mm_shuffle_epi8(xmm11, (SUBSH_MASK[3]));\
    xmm12 = _mm_shuffle_epi8(xmm12, (SUBSH_MASK[4]));\
    xmm13 = _mm_shuffle_epi8(xmm13, (SUBSH_MASK[5]));\
    xmm14 = _mm_shuffle_epi8(xmm14, (SUBSH_MASK[6]));\
    xmm15 = _mm_shuffle_epi8(xmm15, (SUBSH_MASK[7]));\
    /* SubBytes + MixBytes */\
    SUBMIX(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);\
    VPERM_Add_Constant(xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, ALL_15, xmm8);\
    \
    /* AddRoundConstant P1024 */\
    xmm0 = _mm_xor_si128(xmm0, (ROUND_CONST_P[round_counter+1]));\
    /* ShiftBytes P1024 + pre-AESENCLAST */\
    xmm0 = _mm_shuffle_epi8(xmm0, (SUBSH_MASK[0]));\
    xmm1 = _mm_shuffle_epi8(xmm1, (SUBSH_MASK[1]));\
    xmm2 = _mm_shuffle_epi8(xmm2, (SUBSH_MASK[2]));\
    xmm3 = _mm_shuffle_epi8(xmm3, (SUBSH_MASK[3]));\
    xmm4 = _mm_shuffle_epi8(xmm4, (SUBSH_MASK[4]));\
    xmm5 = _mm_shuffle_epi8(xmm5, (SUBSH_MASK[5]));\
    xmm6 = _mm_shuffle_epi8(xmm6, (SUBSH_MASK[6]));\
    xmm7 = _mm_shuffle_epi8(xmm7, (SUBSH_MASK[7]));\
    /* SubBytes + MixBytes */\
    SUBMIX(xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15);\
    VPERM_Add_Constant(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, ALL_15, xmm0);\
  }\
}/**/

#define ROUNDS_Q(){\
  VPERM_Add_Constant(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, ALL_15, xmm1);\
  u8 round_counter = 0;\
  for(round_counter = 0; round_counter < 14; round_counter+=2) {\
    /* AddRoundConstant Q1024 */\
    xmm1 = ALL_FF;\
    xmm8 = _mm_xor_si128(xmm8, xmm1);\
    xmm9 = _mm_xor_si128(xmm9, xmm1);\
    xmm10 = _mm_xor_si128(xmm10, xmm1);\
    xmm11 = _mm_xor_si128(xmm11, xmm1);\
    xmm12 = _mm_xor_si128(xmm12, xmm1);\
    xmm13 = _mm_xor_si128(xmm13, xmm1);\
    xmm14 = _mm_xor_si128(xmm14, xmm1);\
    xmm15 = _mm_xor_si128(xmm15, (ROUND_CONST_Q[round_counter]));\
    /* ShiftBytes Q1024 + pre-AESENCLAST */\
    xmm8 = _mm_shuffle_epi8(xmm8, (SUBSH_MASK[1]));\
    xmm9 = _mm_shuffle_epi8(xmm9, (SUBSH_MASK[3]));\
    xmm10 = _mm_shuffle_epi8(xmm10, (SUBSH_MASK[5]));\
    xmm11 = _mm_shuffle_epi8(xmm11, (SUBSH_MASK[7]));\
    xmm12 = _mm_shuffle_epi8(xmm12, (SUBSH_MASK[0]));\
    xmm13 = _mm_shuffle_epi8(xmm13, (SUBSH_MASK[2]));\
    xmm14 = _mm_shuffle_epi8(xmm14, (SUBSH_MASK[4]));\
    xmm15 = _mm_shuffle_epi8(xmm15, (SUBSH_MASK[6]));\
    /* SubBytes + MixBytes */\
    SUBMIX(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);\
    \
    /* AddRoundConstant Q1024 */\
    xmm9 = ALL_FF;\
    xmm0 = _mm_xor_si128(xmm0, xmm9);\
    xmm1 = _mm_xor_si128(xmm1, xmm9);\
    xmm2 = _mm_xor_si128(xmm2, xmm9);\
    xmm3 = _mm_xor_si128(xmm3, xmm9);\
    xmm4 = _mm_xor_si128(xmm4, xmm9);\
    xmm5 = _mm_xor_si128(xmm5, xmm9);\
    xmm6 = _mm_xor_si128(xmm6, xmm9);\
    xmm7 = _mm_xor_si128(xmm7, (ROUND_CONST_Q[round_counter+1]));\
    /* ShiftBytes Q1024 + pre-AESENCLAST */\
    xmm0 = _mm_shuffle_epi8(xmm0, (SUBSH_MASK[1]));\
    xmm1 = _mm_shuffle_epi8(xmm1, (SUBSH_MASK[3]));\
    xmm2 = _mm_shuffle_epi8(xmm2, (SUBSH_MASK[5]));\
    xmm3 = _mm_shuffle_epi8(xmm3, (SUBSH_MASK[7]));\
    xmm4 = _mm_shuffle_epi8(xmm4, (SUBSH_MASK[0]));\
    xmm5 = _mm_shuffle_epi8(xmm5, (SUBSH_MASK[2]));\
    xmm6 = _mm_shuffle_epi8(xmm6, (SUBSH_MASK[4]));\
    xmm7 = _mm_shuffle_epi8(xmm7, (SUBSH_MASK[6]));\
    /* SubBytes + MixBytes*/ \
    SUBMIX(xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15);\
  }\
  VPERM_Add_Constant(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, ALL_15, xmm1);\
}/**/


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
  t1 = i2;\
  i4 = _mm_shuffle_epi8(i4, t0);\
  i5 = _mm_shuffle_epi8(i5, t0);\
  t2 = i4;\
  t3 = i6;\
  i7 = _mm_shuffle_epi8(i7, t0);\
\
  /* continue with unpack using 4 temp registers */\
  t0 = i0;\
  t2 = _mm_unpackhi_epi16(t2, i5);\
  i4 = _mm_unpacklo_epi16(i4, i5);\
  t3 = _mm_unpackhi_epi16(t3, i7);\
  i6 = _mm_unpacklo_epi16(i6, i7);\
  t0 = _mm_unpackhi_epi16(t0, i1);\
  t1 = _mm_unpackhi_epi16(t1, i3);\
  i2 = _mm_unpacklo_epi16(i2, i3);\
  i0 = _mm_unpacklo_epi16(i0, i1);\
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
  t4 = i0;\
  i0 = _mm_unpacklo_epi32(i0,  i2);\
  t4 = _mm_unpackhi_epi32(t4,  i2);\
  t5 = t0;\
  t0 = _mm_unpacklo_epi32(t0,  t1);\
  t5 = _mm_unpackhi_epi32(t5,  t1);\
  t6 = i4;\
  i4 = _mm_unpacklo_epi32(i4, i6);\
  t7 = t2;\
  t6 = _mm_unpackhi_epi32(t6,  i6);\
  i2 = t0;\
  t2 = _mm_unpacklo_epi32(t2,  t3);\
  i3 = t0;\
  t7 = _mm_unpackhi_epi32(t7,  t3);\
\
  /* there are now 2 rows in each xmm */\
  /* unpack to get 1 row of CV in each xmm */\
  i1 = i0;\
  i1 = _mm_unpackhi_epi64(i1, i4);\
  i0 = _mm_unpacklo_epi64(i0, i4);\
  i4 = t4;\
  i3 = _mm_unpackhi_epi64(i3, t2);\
  i5 = t4;\
  i2 = _mm_unpacklo_epi64(i2, t2);\
  i6 = t5;\
  i5 = _mm_unpackhi_epi64(i5, t6);\
  i7 = t5;\
  i4 = _mm_unpacklo_epi64(i4, t6);\
  i7 = _mm_unpackhi_epi64(i7, t7);\
  i6 = _mm_unpacklo_epi64(i6, t7);\
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
  /*  transpose matrix to get output format */\
  o1 = i0;\
  i0 = _mm_unpacklo_epi64(i0, i1);\
  o1 = _mm_unpackhi_epi64(o1, i1);\
  t0 = i2;\
  i2 = _mm_unpacklo_epi64(i2, i3);\
  t0 = _mm_unpackhi_epi64(t0, i3);\
  t1 = i4;\
  i4 = _mm_unpacklo_epi64(i4, i5);\
  t1 = _mm_unpackhi_epi64(t1, i5);\
  t2 = i6;\
  o0 = TRANSP_MASK;\
  i6 = _mm_unpacklo_epi64(i6, i7);\
  t2 = _mm_unpackhi_epi64(t2, i7);\
  /* load transpose mask into a register, because it will be used 8 times */\
  i0 = _mm_shuffle_epi8(i0, o0);\
  i2 = _mm_shuffle_epi8(i2, o0);\
  i4 = _mm_shuffle_epi8(i4, o0);\
  i6 = _mm_shuffle_epi8(i6, o0);\
  o1 = _mm_shuffle_epi8(o1, o0);\
  t0 = _mm_shuffle_epi8(t0, o0);\
  t1 = _mm_shuffle_epi8(t1, o0);\
  t2 = _mm_shuffle_epi8(t2, o0);\
  /* continue with unpack using 4 temp registers */\
  t3 = i4;\
  o2 = o1;\
  o0 = i0;\
  t4 = t1;\
  \
  t3 = _mm_unpackhi_epi16(t3, i6);\
  i4 = _mm_unpacklo_epi16(i4, i6);\
  o0 = _mm_unpackhi_epi16(o0, i2);\
  i0 = _mm_unpacklo_epi16(i0, i2);\
  o2 = _mm_unpackhi_epi16(o2, t0);\
  o1 = _mm_unpacklo_epi16(o1, t0);\
  t4 = _mm_unpackhi_epi16(t4, t2);\
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
  i1 = i0;\
  i3 = o0;\
  i5 = o1;\
  i7 = o2;\
  i0 = _mm_unpacklo_epi32(i0, i4);\
  i1 = _mm_unpackhi_epi32(i1, i4);\
  o0 = _mm_unpacklo_epi32(o0, t3);\
  i3 = _mm_unpackhi_epi32(i3, t3);\
  o1 = _mm_unpacklo_epi32(o1, t1);\
  i5 = _mm_unpackhi_epi32(i5, t1);\
  o2 = _mm_unpacklo_epi32(o2, t4);\
  i7 = _mm_unpackhi_epi32(i7, t4);\
  /* transpose done */\
}/**/

/* transform round constants into VPERM mode */
#define VPERM_Transform_RoundConst_CNT2(i, j){\
  xmm0 = ROUND_CONST_P[i];\
  xmm1 = ROUND_CONST_P[j];\
  xmm2 = ROUND_CONST_Q[i];\
  xmm3 = ROUND_CONST_Q[j];\
  VPERM_Transform_State(xmm0, xmm1, xmm2, xmm3, VPERM_IPT, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10);\
  xmm2 = _mm_xor_si128(xmm2, (ALL_15));\
  xmm3 = _mm_xor_si128(xmm3, (ALL_15));\
  ROUND_CONST_P[i] = xmm0;\
  ROUND_CONST_P[j] = xmm1;\
  ROUND_CONST_Q[i] = xmm2;\
  ROUND_CONST_Q[j] = xmm3;\
}/**/

/* transform round constants into VPERM mode */
#define VPERM_Transform_RoundConst(){\
  VPERM_Transform_RoundConst_CNT2(0, 1);\
  VPERM_Transform_RoundConst_CNT2(2, 3);\
  VPERM_Transform_RoundConst_CNT2(4, 5);\
  VPERM_Transform_RoundConst_CNT2(6, 7);\
  VPERM_Transform_RoundConst_CNT2(8, 9);\
  VPERM_Transform_RoundConst_CNT2(10, 11);\
  VPERM_Transform_RoundConst_CNT2(12, 13);\
  xmm0 = ALL_FF;\
  VPERM_Transform(xmm0, xmm1, VPERM_IPT, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10);\
  xmm0 = _mm_xor_si128(xmm0, (ALL_15));\
  ALL_FF = xmm0;\
}/**/


void INIT(u64* h)
{
   __m128i* const chaining = (__m128i*) h;
  static __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
  static __m128i xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;

  /* transform round constants into VPERM mode */
  VPERM_Transform_RoundConst();

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
  VPERM_Transform_State(xmm8, xmm9, xmm10, xmm11, VPERM_IPT, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);
  VPERM_Transform_State(xmm12, xmm13, xmm14, xmm15, VPERM_IPT, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);
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
  static __m128i TEMP_MUL1[8];
  static __m128i TEMP_MUL2[8];
  static __m128i TEMP_MUL4;
  static __m128i QTEMP[8];

#ifdef IACA_TRACE
  IACA_START;
#endif

  /* load message into registers xmm8 - xmm15 (Q = message) */
  xmm8 = message[0];
  xmm9 = message[1];
  xmm10 = message[2];
  xmm11 = message[3];
  xmm12 = message[4];
  xmm13 = message[5];
  xmm14 = message[6];
  xmm15 = message[7];

  /* transform message M from column ordering into row ordering */
  VPERM_Transform_State(xmm8, xmm9, xmm10, xmm11, VPERM_IPT, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);
  VPERM_Transform_State(xmm12, xmm13, xmm14, xmm15, VPERM_IPT, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);
  Matrix_Transpose(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);

  /* store message M (Q input) for later */
  QTEMP[0] = xmm8;
  QTEMP[1] = xmm9;
  QTEMP[2] = xmm10;
  QTEMP[3] = xmm11;
  QTEMP[4] = xmm12;
  QTEMP[5] = xmm13;
  QTEMP[6] = xmm14;
  QTEMP[7] = xmm15;

  /* xor CV to message to get P input */
  /* result: CV+M in xmm8...xmm15 */
  xmm8 = _mm_xor_si128(xmm8,  (chaining[0]));
  xmm9 = _mm_xor_si128(xmm9,  (chaining[1]));
  xmm10 = _mm_xor_si128(xmm10, (chaining[2]));
  xmm11 = _mm_xor_si128(xmm11, (chaining[3]));
  xmm12 = _mm_xor_si128(xmm12, (chaining[4]));
  xmm13 = _mm_xor_si128(xmm13, (chaining[5]));
  xmm14 = _mm_xor_si128(xmm14, (chaining[6]));
  xmm15 = _mm_xor_si128(xmm15, (chaining[7]));

  /* compute permutation P */
  /* result: P(CV+M) in xmm8...xmm15 */
  ROUNDS_P();

  /* xor CV to P output (feed-forward) */
  /* result: P(CV+M)+CV in xmm8...xmm15 */
  xmm8 = _mm_xor_si128(xmm8,  (chaining[0]));
  xmm9 = _mm_xor_si128(xmm9,  (chaining[1]));
  xmm10 = _mm_xor_si128(xmm10, (chaining[2]));
  xmm11 = _mm_xor_si128(xmm11, (chaining[3]));
  xmm12 = _mm_xor_si128(xmm12, (chaining[4]));
  xmm13 = _mm_xor_si128(xmm13, (chaining[5]));
  xmm14 = _mm_xor_si128(xmm14, (chaining[6]));
  xmm15 = _mm_xor_si128(xmm15, (chaining[7]));

  /* store P(CV+M)+CV */
  chaining[0] = xmm8;
  chaining[1] = xmm9;
  chaining[2] = xmm10;
  chaining[3] = xmm11;
  chaining[4] = xmm12;
  chaining[5] = xmm13;
  chaining[6] = xmm14;
  chaining[7] = xmm15;

  /* load message M (Q input) into xmm8-15 */
  xmm8 = QTEMP[0];
  xmm9 = QTEMP[1];
  xmm10 = QTEMP[2];
  xmm11 = QTEMP[3];
  xmm12 = QTEMP[4];
  xmm13 = QTEMP[5];
  xmm14 = QTEMP[6];
  xmm15 = QTEMP[7];

  /* compute permutation Q */
  /* result: Q(M) in xmm8...xmm15 */
  ROUNDS_Q();

  /* xor Q output */
  /* result: P(CV+M)+CV+Q(M) in xmm8...xmm15 */
  xmm8 = _mm_xor_si128(xmm8,  (chaining[0]));
  xmm9 = _mm_xor_si128(xmm9,  (chaining[1]));
  xmm10 = _mm_xor_si128(xmm10, (chaining[2]));
  xmm11 = _mm_xor_si128(xmm11, (chaining[3]));
  xmm12 = _mm_xor_si128(xmm12, (chaining[4]));
  xmm13 = _mm_xor_si128(xmm13, (chaining[5]));
  xmm14 = _mm_xor_si128(xmm14, (chaining[6]));
  xmm15 = _mm_xor_si128(xmm15, (chaining[7]));

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
  static __m128i TEMP_MUL1[8];
  static __m128i TEMP_MUL2[8];
  static __m128i TEMP_MUL4;

  /* load CV into registers xmm8 - xmm15 */
  xmm8 = chaining[0];
  xmm9 = chaining[1];
  xmm10 = chaining[2];
  xmm11 = chaining[3];
  xmm12 = chaining[4];
  xmm13 = chaining[5];
  xmm14 = chaining[6];
  xmm15 = chaining[7];

  /* compute permutation P */
  /* result: P(CV) in xmm8...xmm15 */
  ROUNDS_P();

  /* xor CV to P output (feed-forward) */
  /* result: P(CV)+CV in xmm8...xmm15 */
  xmm8 = _mm_xor_si128(xmm8,  (chaining[0]));
  xmm9 = _mm_xor_si128(xmm9,  (chaining[1]));
  xmm10 = _mm_xor_si128(xmm10, (chaining[2]));
  xmm11 = _mm_xor_si128(xmm11, (chaining[3]));
  xmm12 = _mm_xor_si128(xmm12, (chaining[4]));
  xmm13 = _mm_xor_si128(xmm13, (chaining[5]));
  xmm14 = _mm_xor_si128(xmm14, (chaining[6]));
  xmm15 = _mm_xor_si128(xmm15, (chaining[7]));

  /* transpose CV back from row ordering to column ordering */
  /* result: final hash value in xmm0, xmm6, xmm13, xmm15 */
  Matrix_Transpose_INV(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm4, xmm0, xmm6, xmm1, xmm2, xmm3, xmm5, xmm7);
  VPERM_Transform_State(xmm0, xmm6, xmm13, xmm15, VPERM_OPT, xmm1, xmm2, xmm3, xmm5, xmm7, xmm10, xmm12);

  /* we only need to return the truncated half of the state */
  chaining[4] = xmm0;
  chaining[5] = xmm6;
  chaining[6] = xmm13;
  chaining[7] = xmm15;

  return;
}

#endif

