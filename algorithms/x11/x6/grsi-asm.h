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
#include "grsi.h"

/*define data alignment for different C compilers*/
#if defined(__GNUC__)
      #define DATA_ALIGN16(x) x __attribute__ ((aligned(16)))
#else
      #define DATA_ALIGN16(x) __declspec(align(16)) x
#endif

//#if defined(DECLARE_GLOBAL)
#if 1
#define GLOBAL
#else
#define GLOBAL extern
#endif

//#if defined(DECLARE_IFUN)
#if 1
#define IFUN
#else
#define IFUN extern
#endif

/* global constants  */
//GLOBAL __m128i grsiROUND_CONST_Lx;
//GLOBAL __m128i grsiROUND_CONST_L0[grsiROUNDS512];
//GLOBAL __m128i grsiROUND_CONST_L7[grsiROUNDS512];
DATA_ALIGN16(int32_t grsiSUBSH_MASK_short[8*4]) = {
    0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,
    0x04030201, 0x08070605, 0x0c0b0a09, 0x000f0e0d,
    0x05040302, 0x09080706, 0x0d0c0b0a, 0x01000f0e,
    0x06050403, 0x0a090807, 0x0e0d0c0b, 0x0201000f,
    0x07060504, 0x0b0a0908, 0x0f0e0d0c, 0x03020100,
    0x08070605, 0x0c0b0a09, 0x000f0e0d, 0x04030201,
    0x09080706, 0x0d0c0b0a, 0x01000f0e, 0x05040302,
    0x0e0d0c0b, 0x0201000f, 0x06050403, 0x0a090807
};
GLOBAL __m128i *grsiSUBSH_MASK = grsiSUBSH_MASK_short;
GLOBAL __m128i grsiALL_0F = {0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f};
GLOBAL __m128i grsiALL_1B = {0x1b1b1b1b, 0x1b1b1b1b, 0x1b1b1b1b, 0x1b1b1b1b};
GLOBAL __m128i grsiALL_FF = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};

/* global unsknown */


GLOBAL __m128i grsiVPERM_OPT[2];
GLOBAL __m128i grsiVPERM_INV[2];
GLOBAL __m128i grsiVPERM_SB1[2];
GLOBAL __m128i grsiVPERM_SB2[2];
GLOBAL __m128i grsiVPERM_SB4[2];
GLOBAL __m128i grsiVPERM_SBO[2];

/* state vars */
GLOBAL __m128i grsiTRANSP_MASK;
GLOBAL __m128i grsiVPERM_IPT[2];
GLOBAL __m128i grsiALL_15;
GLOBAL __m128i grsiALL_63;
GLOBAL __m128i grsiROUND_CONST_P[grsiROUNDS1024];
GLOBAL __m128i grsiROUND_CONST_Q[grsiROUNDS1024];

#define grsitos(a)    #a
#define grsitostr(a)  grsitos(a)

/*
  grsiALL_1B = _mm_set_epi32(0x1b1b1b1b, 0x1b1b1b1b, 0x1b1b1b1b, 0x1b1b1b1b);\
  grsiALL_63 = _mm_set_epi32(0x63636363, 0x63636363, 0x63636363, 0x63636363);\
*/

#define grsiSET_SHARED_CONSTANTS(){\
  grsiTRANSP_MASK = _mm_set_epi32(0x0f070b03, 0x0e060a02, 0x0d050901, 0x0c040800);\
  grsiALL_0F = _mm_set_epi32(0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f);\
  grsiALL_15 = _mm_set_epi32(0x15151515, 0x15151515, 0x15151515, 0x15151515);\
\
  grsiVPERM_IPT[0] = _mm_set_epi32(0xCD80B1FC, 0xB0FDCC81, 0x4C01307D, 0x317C4D00);\
  grsiVPERM_IPT[1] = _mm_set_epi32(0xCABAE090, 0x52227808, 0xC2B2E898, 0x5A2A7000);\
  grsiVPERM_OPT[0] = _mm_set_epi32(0xE10D5DB1, 0xB05C0CE0, 0x01EDBD51, 0x50BCEC00);\
  grsiVPERM_OPT[1] = _mm_set_epi32(0xF7974121, 0xDEBE6808, 0xFF9F4929, 0xD6B66000);\
  grsiVPERM_INV[0] = _mm_set_epi32(0x030D0E0C, 0x02050809, 0x01040A06, 0x0F0B0780);\
  grsiVPERM_INV[1] = _mm_set_epi32(0x04070309, 0x0A0B0C02, 0x0E05060F, 0x0D080180);\
  grsiVPERM_SB1[0] = _mm_set_epi32(0x3BF7CCC1, 0x0D2ED9EF, 0x3618D415, 0xFAE22300);\
  grsiVPERM_SB1[1] = _mm_set_epi32(0xA5DF7A6E, 0x142AF544, 0xB19BE18F, 0xCB503E00);\
  grsiVPERM_SB2[0] = _mm_set_epi32(0xC2A163C8, 0xAB82234A, 0x69EB8840, 0x0AE12900);\
  grsiVPERM_SB2[1] = _mm_set_epi32(0x5EB7E955, 0xBC982FCD, 0xE27A93C6, 0x0B712400);\
  grsiVPERM_SB4[0] = _mm_set_epi32(0xBA44FE79, 0x876D2914, 0x3D50AED7, 0xC393EA00);\
  grsiVPERM_SB4[1] = _mm_set_epi32(0xA876DE97, 0x49087E9F, 0xE1E937A0, 0x3FD64100);\
}/**/

/* grsiVPERM
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
#define grsiVPERM_Transform_No_Const(a0, a1, t0, t1, t2, t3, c0, c1, c2){\
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

#define grsiVPERM_Transform_Set_Const(table, c0, c1, c2){\
  c0 = grsiALL_0F;\
  c1 = ((__m128i*) table )[0];\
  c2 = ((__m128i*) table )[1];\
}/**/

/* grsiVPERM
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
#define grsiVPERM_Transform(a0, a1, table, t0, t1, t2, t3, c0, c1, c2){\
  grsiVPERM_Transform_Set_Const(table, c0, c1, c2);\
  grsiVPERM_Transform_No_Const(a0, a1, t0, t1, t2, t3, c0, c1, c2);\
}/**/

/* grsiVPERM
 * Transform State
 * inputs:
 * a0-a3 = state
 * table = transformation table to use
 * t* = clobbers
 * outputs:
 * a0-a3 = transformed state
 * */
#define grsiVPERM_Transform_State(a0, a1, a2, a3, table, t0, t1, t2, t3, c0, c1, c2){\
  grsiVPERM_Transform_Set_Const(table, c0, c1, c2);\
  grsiVPERM_Transform_No_Const(a0, a1, t0, t1, t2, t3, c0, c1, c2);\
  grsiVPERM_Transform_No_Const(a2, a3, t0, t1, t2, t3, c0, c1, c2);\
}/**/

/* grsiVPERM
 * Add Constant to State
 * inputs:
 * a0-a7 = state
 * constant = constant to add
 * t0 = clobber
 * outputs:
 * a0-a7 = state + constant
 * */
#define grsiVPERM_Add_Constant(a0, a1, a2, a3, a4, a5, a6, a7, constant, t0){\
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

/* grsiVPERM
 * Set Substitute Core Constants
 * */
#define grsiVPERM_Substitute_Core_Set_Const(c0, c1, c2){\
  grsiVPERM_Transform_Set_Const(grsiVPERM_INV, c0, c1, c2);\
}/**/

/* grsiVPERM
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
#define grsiVPERM_Substitute_Core(a0, b0a, b0b, t0, t1, c0, c1, c2){\
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

/* grsiVPERM
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
#define grsiVPERM_Lookup(a0a, a0b, table, b0, t0){\
  b0 = ((__m128i*) table )[0];\
  t0 = ((__m128i*) table )[1];\
  b0 = _mm_shuffle_epi8(b0, a0b);\
  t0 = _mm_shuffle_epi8(t0, a0a);\
  b0 = _mm_xor_si128(b0, t0);\
}/**/

/* grsiVPERM
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
 * call:grsiVPERM_SUB_MULTIPLY(a0, a1, a2, a3, a4, a5, a6, a7, b1, b2, b5, b6, b0, b3, b4, b7) */
#define grsiVPERM_SUB_MULTIPLY(a0, a1, a2, a3, a4, a5, a6, a7, t0, t1, t3, t4, c2, c1, c0, t2){\
  /* set Constants */\
  grsiVPERM_Substitute_Core_Set_Const(c0, c1, c2);\
  /* row 1 */\
  grsiVPERM_Substitute_Core(a1, t0, t1, t3, t4, c0, c1, c2);\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB1, t2, t4);\
  TEMP_MUL1[1] = t2;\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB2, t3, t4);\
  TEMP_MUL2[1] = t3;\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB4, a1, t4);\
  /* --- */\
  /* row 2 */\
  grsiVPERM_Substitute_Core(a2, t0, t1, t3, t4, c0, c1, c2);\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB1, t2, t4);\
  TEMP_MUL1[2] = t2;\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB2, t3, t4);\
  TEMP_MUL2[2] = t3;\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB4, a2, t4);\
  /* --- */\
  /* row 3 */\
  grsiVPERM_Substitute_Core(a3, t0, t1, t3, t4, c0, c1, c2);\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB1, t2, t4);\
  TEMP_MUL1[3] = t2;\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB2, t3, t4);\
  TEMP_MUL2[3] = t3;\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB4, a3, t4);\
  /* --- */\
  /* row 5 */\
  grsiVPERM_Substitute_Core(a5, t0, t1, t3, t4, c0, c1, c2);\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB1, t2, t4);\
  TEMP_MUL1[5] = t2;\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB2, t3, t4);\
  TEMP_MUL2[5] = t3;\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB4, a5, t4);\
  /* --- */\
  /* row 6 */\
  grsiVPERM_Substitute_Core(a6, t0, t1, t3, t4, c0, c1, c2);\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB1, t2, t4);\
  TEMP_MUL1[6] = t2;\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB2, t3, t4);\
  TEMP_MUL2[6] = t3;\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB4, a6, t4);\
  /* --- */\
  /* row 7 */\
  grsiVPERM_Substitute_Core(a7, t0, t1, t3, t4, c0, c1, c2);\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB1, t2, t4);\
  TEMP_MUL1[7] = t2;\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB2, c1, t4); /*c1 -> b3*/\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB4, a7, t4);\
  /* --- */\
  /* row 4 */\
  grsiVPERM_Substitute_Core(a4, t0, t1, t3, t4, c0, (grsiVPERM_INV[0]), c2);\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB1, t2, t4); /*t2 -> b7*/\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB2, t3, t4);\
  TEMP_MUL2[4] = t3;\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB4, a4, t4);\
  /* --- */\
  /* row 0 */\
  grsiVPERM_Substitute_Core(a0, t0, t1, t3, t4, c0, (grsiVPERM_INV[0]), c2);\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB1, c0, t4); /*c0 -> b4*/\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB2, c2, t4); /*c2 -> b0*/\
  TEMP_MUL2[0] = c2;\
  grsiVPERM_Lookup(t0, t1, grsiVPERM_SB4, a0, t4);\
  /* --- */\
}/**/


/* Optimized grsiMixBytes
 * inputs:
 * a0-a7 = (row0-row7) * 4
 * b0 = row0 * 2
 * b3 = row7 * 2
 * b4 = row7 * 1
 * b7 = row4 * 1
 * all *1 and *2 values must also be in TEMP_MUL1, TEMP_MUL2
 * output: b0-b7
 * */
#define grsiMixBytes(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7){\
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

/*
  grsiSUBSH_MASK[0] = _mm_set_epi32(0x0f0e0d0c, 0x0b0a0908, 0x07060504, 0x03020100);\
  grsiSUBSH_MASK[1] = _mm_set_epi32(0x000f0e0d, 0x0c0b0a09, 0x08070605, 0x04030201);\
  grsiSUBSH_MASK[2] = _mm_set_epi32(0x01000f0e, 0x0d0c0b0a, 0x09080706, 0x05040302);\
  grsiSUBSH_MASK[3] = _mm_set_epi32(0x0201000f, 0x0e0d0c0b, 0x0a090807, 0x06050403);\
  grsiSUBSH_MASK[4] = _mm_set_epi32(0x03020100, 0x0f0e0d0c, 0x0b0a0908, 0x07060504);\
  grsiSUBSH_MASK[5] = _mm_set_epi32(0x04030201, 0x000f0e0d, 0x0c0b0a09, 0x08070605);\
  grsiSUBSH_MASK[6] = _mm_set_epi32(0x05040302, 0x01000f0e, 0x0d0c0b0a, 0x09080706);\
  grsiSUBSH_MASK[7] = _mm_set_epi32(0x0a090807, 0x06050403, 0x0201000f, 0x0e0d0c0b);\
*/

#define grsiSET_CONSTANTS(){\
  grsiSET_SHARED_CONSTANTS();\
  grsiALL_FF = _mm_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);\
  for(i = 0; i < grsiROUNDS1024; i++)\
  {\
    grsiROUND_CONST_P[i] = _mm_set_epi32(0xf0e0d0c0 ^ (i * 0x01010101), 0xb0a09080 ^ (i * 0x01010101), 0x70605040 ^ (i * 0x01010101), 0x30201000 ^ (i * 0x01010101));\
    grsiROUND_CONST_Q[i] = _mm_set_epi32(0x0f1f2f3f ^ (i * 0x01010101), 0x4f5f6f7f ^ (i * 0x01010101), 0x8f9fafbf ^ (i * 0x01010101), 0xcfdfefff ^ (i * 0x01010101));\
  }\
}/**/

/* one round
 * a0-a7 = input rows
 * b0-b7 = output rows
 */
#define grsiSUBMIX(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7){\
  /* SubBytes + Multiplication */\
  grsiVPERM_SUB_MULTIPLY(a0, a1, a2, a3, a4, a5, a6, a7, b1, b2, b5, b6, b0, b3, b4, b7);\
  /* grsiMixBytes */\
  grsiMixBytes(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7);\
}/**/

#define grsiROUNDS_P(){\
  u32 round_counter;\
  for(round_counter = 0; round_counter < 14; round_counter+=2) {\
    /* AddRoundConstant P1024 */\
    xmm8 = _mm_xor_si128(xmm8, (grsiROUND_CONST_P[round_counter]));\
    /* ShiftBytes P1024 + pre-AESENCLAST */\
    xmm8 = _mm_shuffle_epi8(xmm8,  (grsiSUBSH_MASK[0]));\
    xmm9 = _mm_shuffle_epi8(xmm9,  (grsiSUBSH_MASK[1]));\
    xmm10 = _mm_shuffle_epi8(xmm10, (grsiSUBSH_MASK[2]));\
    xmm11 = _mm_shuffle_epi8(xmm11, (grsiSUBSH_MASK[3]));\
    xmm12 = _mm_shuffle_epi8(xmm12, (grsiSUBSH_MASK[4]));\
    xmm13 = _mm_shuffle_epi8(xmm13, (grsiSUBSH_MASK[5]));\
    xmm14 = _mm_shuffle_epi8(xmm14, (grsiSUBSH_MASK[6]));\
    xmm15 = _mm_shuffle_epi8(xmm15, (grsiSUBSH_MASK[7]));\
    /* SubBytes + grsiMixBytes */\
    grsiSUBMIX(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);\
    grsiVPERM_Add_Constant(xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, grsiALL_15, xmm8);\
    \
    /* AddRoundConstant P1024 */\
    xmm0 = _mm_xor_si128(xmm0, (grsiROUND_CONST_P[round_counter+1]));\
    /* ShiftBytes P1024 + pre-AESENCLAST */\
    xmm0 = _mm_shuffle_epi8(xmm0, (grsiSUBSH_MASK[0]));\
    xmm1 = _mm_shuffle_epi8(xmm1, (grsiSUBSH_MASK[1]));\
    xmm2 = _mm_shuffle_epi8(xmm2, (grsiSUBSH_MASK[2]));\
    xmm3 = _mm_shuffle_epi8(xmm3, (grsiSUBSH_MASK[3]));\
    xmm4 = _mm_shuffle_epi8(xmm4, (grsiSUBSH_MASK[4]));\
    xmm5 = _mm_shuffle_epi8(xmm5, (grsiSUBSH_MASK[5]));\
    xmm6 = _mm_shuffle_epi8(xmm6, (grsiSUBSH_MASK[6]));\
    xmm7 = _mm_shuffle_epi8(xmm7, (grsiSUBSH_MASK[7]));\
    /* SubBytes + grsiMixBytes */\
    grsiSUBMIX(xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15);\
    grsiVPERM_Add_Constant(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, grsiALL_15, xmm0);\
  }\
}/**/

#define grsiROUNDS_Q(){\
  grsiVPERM_Add_Constant(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, grsiALL_15, xmm1);\
  u32 round_counter = 0;\
  for(round_counter = 0; round_counter < 14; round_counter+=2) {\
    /* AddRoundConstant Q1024 */\
    xmm1 = grsiALL_FF;\
    xmm8 = _mm_xor_si128(xmm8, xmm1);\
    xmm9 = _mm_xor_si128(xmm9, xmm1);\
    xmm10 = _mm_xor_si128(xmm10, xmm1);\
    xmm11 = _mm_xor_si128(xmm11, xmm1);\
    xmm12 = _mm_xor_si128(xmm12, xmm1);\
    xmm13 = _mm_xor_si128(xmm13, xmm1);\
    xmm14 = _mm_xor_si128(xmm14, xmm1);\
    xmm15 = _mm_xor_si128(xmm15, (grsiROUND_CONST_Q[round_counter]));\
    /* ShiftBytes Q1024 + pre-AESENCLAST */\
    xmm8 = _mm_shuffle_epi8(xmm8, (grsiSUBSH_MASK[1]));\
    xmm9 = _mm_shuffle_epi8(xmm9, (grsiSUBSH_MASK[3]));\
    xmm10 = _mm_shuffle_epi8(xmm10, (grsiSUBSH_MASK[5]));\
    xmm11 = _mm_shuffle_epi8(xmm11, (grsiSUBSH_MASK[7]));\
    xmm12 = _mm_shuffle_epi8(xmm12, (grsiSUBSH_MASK[0]));\
    xmm13 = _mm_shuffle_epi8(xmm13, (grsiSUBSH_MASK[2]));\
    xmm14 = _mm_shuffle_epi8(xmm14, (grsiSUBSH_MASK[4]));\
    xmm15 = _mm_shuffle_epi8(xmm15, (grsiSUBSH_MASK[6]));\
    /* SubBytes + grsiMixBytes */\
    grsiSUBMIX(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);\
    \
    /* AddRoundConstant Q1024 */\
    xmm9 = grsiALL_FF;\
    xmm0 = _mm_xor_si128(xmm0, xmm9);\
    xmm1 = _mm_xor_si128(xmm1, xmm9);\
    xmm2 = _mm_xor_si128(xmm2, xmm9);\
    xmm3 = _mm_xor_si128(xmm3, xmm9);\
    xmm4 = _mm_xor_si128(xmm4, xmm9);\
    xmm5 = _mm_xor_si128(xmm5, xmm9);\
    xmm6 = _mm_xor_si128(xmm6, xmm9);\
    xmm7 = _mm_xor_si128(xmm7, (grsiROUND_CONST_Q[round_counter+1]));\
    /* ShiftBytes Q1024 + pre-AESENCLAST */\
    xmm0 = _mm_shuffle_epi8(xmm0, (grsiSUBSH_MASK[1]));\
    xmm1 = _mm_shuffle_epi8(xmm1, (grsiSUBSH_MASK[3]));\
    xmm2 = _mm_shuffle_epi8(xmm2, (grsiSUBSH_MASK[5]));\
    xmm3 = _mm_shuffle_epi8(xmm3, (grsiSUBSH_MASK[7]));\
    xmm4 = _mm_shuffle_epi8(xmm4, (grsiSUBSH_MASK[0]));\
    xmm5 = _mm_shuffle_epi8(xmm5, (grsiSUBSH_MASK[2]));\
    xmm6 = _mm_shuffle_epi8(xmm6, (grsiSUBSH_MASK[4]));\
    xmm7 = _mm_shuffle_epi8(xmm7, (grsiSUBSH_MASK[6]));\
    /* SubBytes + grsiMixBytes*/ \
    grsiSUBMIX(xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15);\
  }\
  grsiVPERM_Add_Constant(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, grsiALL_15, xmm1);\
}/**/


/* Matrix Transpose
 * input is a 1024-bit state with two columns in one xmm
 * output is a 1024-bit state with two rows in one xmm
 * inputs: i0-i7
 * outputs: i0-i7
 * clobbers: t0-t7
 */
#define grsiMatrix_Transpose(i0, i1, i2, i3, i4, i5, i6, i7, t0, t1, t2, t3, t4, t5, t6, t7){\
  t0 = grsiTRANSP_MASK;\
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
#define grsiMatrix_Transpose_INV(i0, i1, i2, i3, i4, i5, i6, i7, o0, o1, o2, t0, t1, t2, t3, t4){\
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
  o0 = grsiTRANSP_MASK;\
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

/* transform round constants into grsiVPERM mode */
#define grsiVPERM_Transform_RoundConst_CNT2(i, j){\
  xmm0 = grsiROUND_CONST_P[i];\
  xmm1 = grsiROUND_CONST_P[j];\
  xmm2 = grsiROUND_CONST_Q[i];\
  xmm3 = grsiROUND_CONST_Q[j];\
  grsiVPERM_Transform_State(xmm0, xmm1, xmm2, xmm3, grsiVPERM_IPT, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10);\
  xmm2 = _mm_xor_si128(xmm2, (grsiALL_15));\
  xmm3 = _mm_xor_si128(xmm3, (grsiALL_15));\
  grsiROUND_CONST_P[i] = xmm0;\
  grsiROUND_CONST_P[j] = xmm1;\
  grsiROUND_CONST_Q[i] = xmm2;\
  grsiROUND_CONST_Q[j] = xmm3;\
}/**/

/* transform round constants into grsiVPERM mode */
#define grsiVPERM_Transform_RoundConst(){\
  grsiVPERM_Transform_RoundConst_CNT2(0, 1);\
  grsiVPERM_Transform_RoundConst_CNT2(2, 3);\
  grsiVPERM_Transform_RoundConst_CNT2(4, 5);\
  grsiVPERM_Transform_RoundConst_CNT2(6, 7);\
  grsiVPERM_Transform_RoundConst_CNT2(8, 9);\
  grsiVPERM_Transform_RoundConst_CNT2(10, 11);\
  grsiVPERM_Transform_RoundConst_CNT2(12, 13);\
  xmm0 = grsiALL_FF;\
  grsiVPERM_Transform(xmm0, xmm1, grsiVPERM_IPT, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10);\
  xmm0 = _mm_xor_si128(xmm0, (grsiALL_15));\
  grsiALL_FF = xmm0;\
}/**/


IFUN void grsiINIT(u64* h)
#if !defined(DECLARE_IFUN)
;
#else
{
   __m128i* const chaining = (__m128i*) h;
  static __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
  static __m128i xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;

  /* transform round constants into grsiVPERM mode */
  grsiVPERM_Transform_RoundConst();

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
  grsiVPERM_Transform_State(xmm8, xmm9, xmm10, xmm11, grsiVPERM_IPT, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);
  grsiVPERM_Transform_State(xmm12, xmm13, xmm14, xmm15, grsiVPERM_IPT, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);
  grsiMatrix_Transpose(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);

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
#endif

IFUN void grsiTF1024(u64* h, u64* m)
#if !defined(DECLARE_IFUN)
;
#else
{
  __m128i* const chaining = (__m128i*) h;
  __m128i* const message = (__m128i*) m;
  static __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
  static __m128i xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;
  static __m128i TEMP_MUL1[8];
  static __m128i TEMP_MUL2[8];
  static __m128i TEMP_MUL4;
  static __m128i QTEMP[8];

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
  grsiVPERM_Transform_State(xmm8, xmm9, xmm10, xmm11, grsiVPERM_IPT, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);
  grsiVPERM_Transform_State(xmm12, xmm13, xmm14, xmm15, grsiVPERM_IPT, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);
  grsiMatrix_Transpose(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7);

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
  grsiROUNDS_P();

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
  grsiROUNDS_Q();

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

  return;
}
#endif

IFUN void grsiOF1024(u64* h)
#if !defined(DECLARE_IFUN)
;
#else
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
  grsiROUNDS_P();

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
  grsiMatrix_Transpose_INV(xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, xmm4, xmm0, xmm6, xmm1, xmm2, xmm3, xmm5, xmm7);
  grsiVPERM_Transform_State(xmm0, xmm6, xmm13, xmm15, grsiVPERM_OPT, xmm1, xmm2, xmm3, xmm5, xmm7, xmm10, xmm12);

  /* we only need to return the truncated half of the state */
  chaining[4] = xmm0;
  chaining[5] = xmm6;
  chaining[6] = xmm13;
  chaining[7] = xmm15;

  return;
}
#endif

