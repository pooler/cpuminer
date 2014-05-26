/* groestl-asm-vperm.h     Aug 2011
 *
 * Groestl implementation with inline assembly using ssse3 instructions.
 * Author: Günther A. Roland, Martin Schläffer, Krystian Matusiewicz
 *
 * Based on the vperm and aes_ni implementations of the hash function Groestl
 * by Cagdas Calik <ccalik@metu.edu.tr> http://www.metu.edu.tr/~ccalik/
 * Institute of Applied Mathematics, Middle East Technical University, Turkey
 *
 * This code is placed in the public domain
 */

#include "hash-groestl.h"

/* global constants  */
__attribute__ ((aligned (16))) unsigned char ROUND_CONST_Lx[16];
__attribute__ ((aligned (16))) unsigned char ROUND_CONST_L0[ROUNDS512*16];
__attribute__ ((aligned (16))) unsigned char ROUND_CONST_L7[ROUNDS512*16];
__attribute__ ((aligned (16))) unsigned char ROUND_CONST_P[ROUNDS1024*16];
__attribute__ ((aligned (16))) unsigned char ROUND_CONST_Q[ROUNDS1024*16];
__attribute__ ((aligned (16))) unsigned char TRANSP_MASK[16];
__attribute__ ((aligned (16))) unsigned char SUBSH_MASK[8*16];
__attribute__ ((aligned (16))) unsigned char ALL_0F[16];
__attribute__ ((aligned (16))) unsigned char ALL_15[16];
__attribute__ ((aligned (16))) unsigned char ALL_1B[16];
__attribute__ ((aligned (16))) unsigned char ALL_63[16];
__attribute__ ((aligned (16))) unsigned char ALL_FF[16];
__attribute__ ((aligned (16))) unsigned char VPERM_IPT[2*16];
__attribute__ ((aligned (16))) unsigned char VPERM_OPT[2*16];
__attribute__ ((aligned (16))) unsigned char VPERM_INV[2*16];
__attribute__ ((aligned (16))) unsigned char VPERM_SB1[2*16];
__attribute__ ((aligned (16))) unsigned char VPERM_SB2[2*16];
__attribute__ ((aligned (16))) unsigned char VPERM_SB4[2*16];
__attribute__ ((aligned (16))) unsigned char VPERM_SBO[2*16];

/* temporary variables  */
__attribute__ ((aligned (16))) unsigned char TEMP_MUL1[8*16];
__attribute__ ((aligned (16))) unsigned char TEMP_MUL2[8*16];
__attribute__ ((aligned (16))) unsigned char TEMP_MUL4[1*16];
__attribute__ ((aligned (16))) unsigned char QTEMP[8*16];
__attribute__ ((aligned (16))) unsigned char TEMP[8*16];


#define tos(a)    #a
#define tostr(a)  tos(a)

#define SET_SHARED_CONSTANTS(){\
  ((u64*)TRANSP_MASK)[0] = 0x0d0509010c040800ULL;\
  ((u64*)TRANSP_MASK)[1] = 0x0f070b030e060a02ULL;\
  ((u64*)ALL_1B)[0] = 0x1b1b1b1b1b1b1b1bULL;\
  ((u64*)ALL_1B)[1] = 0x1b1b1b1b1b1b1b1bULL;\
  ((u64*)ALL_63)[ 0] = 0x6363636363636363ULL;\
  ((u64*)ALL_63)[ 1] = 0x6363636363636363ULL;\
  ((u64*)ALL_0F)[ 0] = 0x0F0F0F0F0F0F0F0FULL;\
  ((u64*)ALL_0F)[ 1] = 0x0F0F0F0F0F0F0F0FULL;\
  ((u64*)VPERM_IPT)[ 0] = 0x4C01307D317C4D00ULL;\
  ((u64*)VPERM_IPT)[ 1] = 0xCD80B1FCB0FDCC81ULL;\
  ((u64*)VPERM_IPT)[ 2] = 0xC2B2E8985A2A7000ULL;\
  ((u64*)VPERM_IPT)[ 3] = 0xCABAE09052227808ULL;\
  ((u64*)VPERM_OPT)[ 0] = 0x01EDBD5150BCEC00ULL;\
  ((u64*)VPERM_OPT)[ 1] = 0xE10D5DB1B05C0CE0ULL;\
  ((u64*)VPERM_OPT)[ 2] = 0xFF9F4929D6B66000ULL;\
  ((u64*)VPERM_OPT)[ 3] = 0xF7974121DEBE6808ULL;\
  ((u64*)VPERM_INV)[ 0] = 0x01040A060F0B0780ULL;\
  ((u64*)VPERM_INV)[ 1] = 0x030D0E0C02050809ULL;\
  ((u64*)VPERM_INV)[ 2] = 0x0E05060F0D080180ULL;\
  ((u64*)VPERM_INV)[ 3] = 0x040703090A0B0C02ULL;\
  ((u64*)VPERM_SB1)[ 0] = 0x3618D415FAE22300ULL;\
  ((u64*)VPERM_SB1)[ 1] = 0x3BF7CCC10D2ED9EFULL;\
  ((u64*)VPERM_SB1)[ 2] = 0xB19BE18FCB503E00ULL;\
  ((u64*)VPERM_SB1)[ 3] = 0xA5DF7A6E142AF544ULL;\
  ((u64*)VPERM_SB2)[ 0] = 0x69EB88400AE12900ULL;\
  ((u64*)VPERM_SB2)[ 1] = 0xC2A163C8AB82234AULL;\
  ((u64*)VPERM_SB2)[ 2] = 0xE27A93C60B712400ULL;\
  ((u64*)VPERM_SB2)[ 3] = 0x5EB7E955BC982FCDULL;\
  ((u64*)VPERM_SB4)[ 0] = 0x3D50AED7C393EA00ULL;\
  ((u64*)VPERM_SB4)[ 1] = 0xBA44FE79876D2914ULL;\
  ((u64*)VPERM_SB4)[ 2] = 0xE1E937A03FD64100ULL;\
  ((u64*)VPERM_SB4)[ 3] = 0xA876DE9749087E9FULL;\
/*((u64*)VPERM_SBO)[ 0] = 0xCFE474A55FBB6A00ULL;\
  ((u64*)VPERM_SBO)[ 1] = 0x8E1E90D1412B35FAULL;\
  ((u64*)VPERM_SBO)[ 2] = 0xD0D26D176FBDC700ULL;\
  ((u64*)VPERM_SBO)[ 3] = 0x15AABF7AC502A878ULL;*/\
  ((u64*)ALL_15)[ 0] = 0x1515151515151515ULL;\
  ((u64*)ALL_15)[ 1] = 0x1515151515151515ULL;\
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
  asm ("movdqa xmm"tostr(t0)", xmm"tostr(c0)"");\
  asm ("movdqa xmm"tostr(t1)", xmm"tostr(c0)"");\
  asm ("pandn  xmm"tostr(t0)", xmm"tostr(a0)"");\
  asm ("pandn  xmm"tostr(t1)", xmm"tostr(a1)"");\
  asm ("psrld  xmm"tostr(t0)", 4");\
  asm ("psrld  xmm"tostr(t1)", 4");\
  asm ("pand   xmm"tostr(a0)", xmm"tostr(c0)"");\
  asm ("pand   xmm"tostr(a1)", xmm"tostr(c0)"");\
  asm ("movdqa xmm"tostr(t2)", xmm"tostr(c2)"");\
  asm ("movdqa xmm"tostr(t3)", xmm"tostr(c2)"");\
  asm ("pshufb xmm"tostr(t2)", xmm"tostr(a0)"");\
  asm ("pshufb xmm"tostr(t3)", xmm"tostr(a1)"");\
  asm ("movdqa xmm"tostr(a0)", xmm"tostr(c1)"");\
  asm ("movdqa xmm"tostr(a1)", xmm"tostr(c1)"");\
  asm ("pshufb xmm"tostr(a0)", xmm"tostr(t0)"");\
  asm ("pshufb xmm"tostr(a1)", xmm"tostr(t1)"");\
  asm ("pxor   xmm"tostr(a0)", xmm"tostr(t2)"");\
  asm ("pxor   xmm"tostr(a1)", xmm"tostr(t3)"");\
}/**/

#define VPERM_Transform_Set_Const(table, c0, c1, c2){\
  asm ("movaps xmm"tostr(c0)", [ALL_0F]");\
  asm ("movaps xmm"tostr(c1)", ["tostr(table)"+0*16]");\
  asm ("movaps xmm"tostr(c2)", ["tostr(table)"+1*16]");\
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
  asm ("movaps xmm"tostr(t0)", ["tostr(constant)"]");\
  asm ("pxor   xmm"tostr(a0)",  xmm"tostr(t0)"");\
  asm ("pxor   xmm"tostr(a1)",  xmm"tostr(t0)"");\
  asm ("pxor   xmm"tostr(a2)",  xmm"tostr(t0)"");\
  asm ("pxor   xmm"tostr(a3)",  xmm"tostr(t0)"");\
  asm ("pxor   xmm"tostr(a4)",  xmm"tostr(t0)"");\
  asm ("pxor   xmm"tostr(a5)",  xmm"tostr(t0)"");\
  asm ("pxor   xmm"tostr(a6)",  xmm"tostr(t0)"");\
  asm ("pxor   xmm"tostr(a7)",  xmm"tostr(t0)"");\
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
  asm ("movdqa xmm"tostr(t0)",  xmm"tostr(c0)"");\
  asm ("pandn  xmm"tostr(t0)",  xmm"tostr(a0)"");\
  asm ("psrld  xmm"tostr(t0)",  4");\
  asm ("pand   xmm"tostr(a0)",  xmm"tostr(c0)"");\
  asm ("movdqa xmm"tostr(b0a)", "tostr(c1)"");\
  asm ("pshufb xmm"tostr(b0a)", xmm"tostr(a0)"");\
  asm ("pxor   xmm"tostr(a0)",  xmm"tostr(t0)"");\
  asm ("movdqa xmm"tostr(b0b)", xmm"tostr(c2)"");\
  asm ("pshufb xmm"tostr(b0b)", xmm"tostr(t0)"");\
  asm ("pxor   xmm"tostr(b0b)", xmm"tostr(b0a)"");\
  asm ("movdqa xmm"tostr(t1)",  xmm"tostr(c2)"");\
  asm ("pshufb xmm"tostr(t1)",  xmm"tostr(a0)"");\
  asm ("pxor   xmm"tostr(t1)",  xmm"tostr(b0a)"");\
  asm ("movdqa xmm"tostr(b0a)", xmm"tostr(c2)"");\
  asm ("pshufb xmm"tostr(b0a)", xmm"tostr(b0b)"");\
  asm ("pxor   xmm"tostr(b0a)", xmm"tostr(a0)"");\
  asm ("movdqa xmm"tostr(b0b)", xmm"tostr(c2)"");\
  asm ("pshufb xmm"tostr(b0b)", xmm"tostr(t1)"");\
  asm ("pxor   xmm"tostr(b0b)", xmm"tostr(t0)"");\
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
  asm ("movaps xmm"tostr(b0)", ["tostr(table)"+0*16]");\
  asm ("movaps xmm"tostr(t0)", ["tostr(table)"+1*16]");\
  asm ("pshufb xmm"tostr(b0)", xmm"tostr(a0b)"");\
  asm ("pshufb xmm"tostr(t0)", xmm"tostr(a0a)"");\
  asm ("pxor   xmm"tostr(b0)", xmm"tostr(t0)"");\
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
  VPERM_Substitute_Core(a1, t0, t1, t3, t4, c0, xmm##c1, c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, t2, t4);\
  asm ("movaps [TEMP_MUL1+1*16], xmm"tostr(t2)"");\
  VPERM_Lookup(t0, t1, VPERM_SB2, t3, t4);\
  asm ("movaps [TEMP_MUL2+1*16], xmm"tostr(t3)"");\
  VPERM_Lookup(t0, t1, VPERM_SB4, a1, t4);\
  /* --- */\
  /* row 2 */\
  VPERM_Substitute_Core(a2, t0, t1, t3, t4, c0, xmm##c1, c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, t2, t4);\
  asm ("movaps [TEMP_MUL1+2*16], xmm"tostr(t2)"");\
  VPERM_Lookup(t0, t1, VPERM_SB2, t3, t4);\
  asm ("movaps [TEMP_MUL2+2*16], xmm"tostr(t3)"");\
  VPERM_Lookup(t0, t1, VPERM_SB4, a2, t4);\
  /* --- */\
  /* row 3 */\
  VPERM_Substitute_Core(a3, t0, t1, t3, t4, c0, xmm##c1, c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, t2, t4);\
  asm ("movaps [TEMP_MUL1+3*16], xmm"tostr(t2)"");\
  VPERM_Lookup(t0, t1, VPERM_SB2, t3, t4);\
  asm ("movaps [TEMP_MUL2+3*16], xmm"tostr(t3)"");\
  VPERM_Lookup(t0, t1, VPERM_SB4, a3, t4);\
  /* --- */\
  /* row 5 */\
  VPERM_Substitute_Core(a5, t0, t1, t3, t4, c0, xmm##c1, c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, t2, t4);\
  asm ("movaps [TEMP_MUL1+5*16], xmm"tostr(t2)"");\
  VPERM_Lookup(t0, t1, VPERM_SB2, t3, t4);\
  asm ("movaps [TEMP_MUL2+5*16], xmm"tostr(t3)"");\
  VPERM_Lookup(t0, t1, VPERM_SB4, a5, t4);\
  /* --- */\
  /* row 6 */\
  VPERM_Substitute_Core(a6, t0, t1, t3, t4, c0, xmm##c1, c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, t2, t4);\
  asm ("movaps [TEMP_MUL1+6*16], xmm"tostr(t2)"");\
  VPERM_Lookup(t0, t1, VPERM_SB2, t3, t4);\
  asm ("movaps [TEMP_MUL2+6*16], xmm"tostr(t3)"");\
  VPERM_Lookup(t0, t1, VPERM_SB4, a6, t4);\
  /* --- */\
  /* row 7 */\
  VPERM_Substitute_Core(a7, t0, t1, t3, t4, c0, xmm##c1, c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, t2, t4);\
  asm ("movaps [TEMP_MUL1+7*16], xmm"tostr(t2)"");\
  VPERM_Lookup(t0, t1, VPERM_SB2, c1, t4); /*c1 -> b3*/\
  VPERM_Lookup(t0, t1, VPERM_SB4, a7, t4);\
  /* --- */\
  /* row 4 */\
  VPERM_Substitute_Core(a4, t0, t1, t3, t4, c0, [VPERM_INV+0*16], c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, t2, t4); /*t2 -> b7*/\
  VPERM_Lookup(t0, t1, VPERM_SB2, t3, t4);\
  asm ("movaps [TEMP_MUL2+4*16], xmm"tostr(t3)"");\
  VPERM_Lookup(t0, t1, VPERM_SB4, a4, t4);\
  /* --- */\
  /* row 0 */\
  VPERM_Substitute_Core(a0, t0, t1, t3, t4, c0, [VPERM_INV+0*16], c2);\
  VPERM_Lookup(t0, t1, VPERM_SB1, c0, t4); /*c0 -> b4*/\
  VPERM_Lookup(t0, t1, VPERM_SB2, c2, t4); /*c2 -> b0*/\
  asm ("movaps [TEMP_MUL2+0*16], xmm"tostr(c2)"");\
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
  asm ("movaps [TEMP_MUL4], xmm"tostr(a3)"");\
  /* 1 */\
  asm ("movdqa xmm"tostr(b1)", xmm"tostr(a0)"");\
  asm ("pxor   xmm"tostr(b1)", xmm"tostr(a5)"");\
  asm ("pxor   xmm"tostr(b1)", xmm"tostr(b4)""); /* -> helper! */\
  asm ("pxor   xmm"tostr(b1)", [TEMP_MUL2+3*16]");\
  asm ("movdqa xmm"tostr(b2)", xmm"tostr(b1)"");\
  \
  /* 2 */\
  asm ("movdqa xmm"tostr(b5)", xmm"tostr(a1)"");\
  asm ("pxor   xmm"tostr(b5)", xmm"tostr(a4)"");\
  asm ("pxor   xmm"tostr(b5)", xmm"tostr(b7)""); /* -> helper! */\
  asm ("pxor   xmm"tostr(b5)", xmm"tostr(b3)""); /* -> helper! */\
  asm ("movdqa xmm"tostr(b6)", xmm"tostr(b5)"");\
  \
  /* 4 */\
  asm ("pxor   xmm"tostr(b7)", xmm"tostr(a6)"");\
  /*asm ("pxor   xmm"tostr(b7)", [TEMP_MUL1+4*16]"); -> helper! */\
  asm ("pxor   xmm"tostr(b7)", [TEMP_MUL1+6*16]");\
  asm ("pxor   xmm"tostr(b7)", [TEMP_MUL2+1*16]");\
  asm ("pxor   xmm"tostr(b7)", xmm"tostr(b3)""); /* -> helper! */\
  asm ("pxor   xmm"tostr(b2)", xmm"tostr(b7)"");\
  \
  /* 3 */\
  asm ("pxor   xmm"tostr(b0)", xmm"tostr(a7)"");\
  asm ("pxor   xmm"tostr(b0)", [TEMP_MUL1+5*16]");\
  asm ("pxor   xmm"tostr(b0)", [TEMP_MUL1+7*16]");\
  /*asm ("pxor   xmm"tostr(b0)", [TEMP_MUL2+0*16]"); -> helper! */\
  asm ("pxor   xmm"tostr(b0)", [TEMP_MUL2+2*16]");\
  asm ("movdqa xmm"tostr(b3)", xmm"tostr(b0)"");\
  asm ("pxor   xmm"tostr(b1)", xmm"tostr(b0)"");\
  asm ("pxor   xmm"tostr(b0)", xmm"tostr(b7)""); /* moved from 4 */\
  \
  /* 5 */\
  asm ("pxor   xmm"tostr(b4)", xmm"tostr(a2)"");\
  /*asm ("pxor   xmm"tostr(b4)", [TEMP_MUL1+0*16]"); -> helper! */\
  asm ("pxor   xmm"tostr(b4)", [TEMP_MUL1+2*16]");\
  asm ("pxor   xmm"tostr(b4)", [TEMP_MUL2+3*16]");\
  asm ("pxor   xmm"tostr(b4)", [TEMP_MUL2+5*16]");\
  asm ("pxor   xmm"tostr(b3)", xmm"tostr(b4)"");\
  asm ("pxor   xmm"tostr(b6)", xmm"tostr(b4)"");\
  \
  /* 6 */\
  asm ("pxor xmm"tostr(a3)", [TEMP_MUL1+1*16]");\
  asm ("pxor xmm"tostr(a3)", [TEMP_MUL1+3*16]");\
  asm ("pxor xmm"tostr(a3)", [TEMP_MUL2+4*16]");\
  asm ("pxor xmm"tostr(a3)", [TEMP_MUL2+6*16]");\
  asm ("pxor xmm"tostr(b4)", xmm"tostr(a3)"");\
  asm ("pxor xmm"tostr(b5)", xmm"tostr(a3)"");\
  asm ("pxor xmm"tostr(b7)", xmm"tostr(a3)"");\
  \
  /* 7 */\
  asm ("pxor xmm"tostr(a1)", [TEMP_MUL1+1*16]");\
  asm ("pxor xmm"tostr(a1)", [TEMP_MUL2+4*16]");\
  asm ("pxor xmm"tostr(b2)", xmm"tostr(a1)"");\
  asm ("pxor xmm"tostr(b3)", xmm"tostr(a1)"");\
  \
  /* 8 */\
  asm ("pxor xmm"tostr(a5)", [TEMP_MUL1+5*16]");\
  asm ("pxor xmm"tostr(a5)", [TEMP_MUL2+0*16]");\
  asm ("pxor xmm"tostr(b6)", xmm"tostr(a5)"");\
  asm ("pxor xmm"tostr(b7)", xmm"tostr(a5)"");\
  \
  /* 9 */\
  asm ("movaps xmm"tostr(a3)", [TEMP_MUL1+2*16]");\
  asm ("pxor   xmm"tostr(a3)", [TEMP_MUL2+5*16]");\
  asm ("pxor   xmm"tostr(b0)", xmm"tostr(a3)"");\
  asm ("pxor   xmm"tostr(b5)", xmm"tostr(a3)"");\
  \
  /* 10 */\
  asm ("movaps xmm"tostr(a1)", [TEMP_MUL1+6*16]");\
  asm ("pxor   xmm"tostr(a1)", [TEMP_MUL2+1*16]");\
  asm ("pxor   xmm"tostr(b1)", xmm"tostr(a1)"");\
  asm ("pxor   xmm"tostr(b4)", xmm"tostr(a1)"");\
  \
  /* 11 */\
  asm ("movaps xmm"tostr(a5)", [TEMP_MUL1+3*16]");\
  asm ("pxor   xmm"tostr(a5)", [TEMP_MUL2+6*16]");\
  asm ("pxor   xmm"tostr(b1)", xmm"tostr(a5)"");\
  asm ("pxor   xmm"tostr(b6)", xmm"tostr(a5)"");\
  \
  /* 12 */\
  asm ("movaps xmm"tostr(a3)", [TEMP_MUL1+7*16]");\
  asm ("pxor   xmm"tostr(a3)", [TEMP_MUL2+2*16]");\
  asm ("pxor   xmm"tostr(b2)", xmm"tostr(a3)"");\
  asm ("pxor   xmm"tostr(b5)", xmm"tostr(a3)"");\
  \
  /* 13 */\
  asm ("pxor xmm"tostr(b0)", [TEMP_MUL4]");\
  asm ("pxor xmm"tostr(b0)", xmm"tostr(a4)"");\
  asm ("pxor xmm"tostr(b1)", xmm"tostr(a4)"");\
  asm ("pxor xmm"tostr(b3)", xmm"tostr(a6)"");\
  asm ("pxor xmm"tostr(b4)", xmm"tostr(a0)"");\
  asm ("pxor xmm"tostr(b4)", xmm"tostr(a7)"");\
  asm ("pxor xmm"tostr(b5)", xmm"tostr(a0)"");\
  asm ("pxor xmm"tostr(b7)", xmm"tostr(a2)"");\
}/**/

#if (LENGTH <= 256)

#define SET_CONSTANTS(){\
  SET_SHARED_CONSTANTS();\
  ((u64*)SUBSH_MASK)[ 0] = 0x0706050403020100ULL;\
  ((u64*)SUBSH_MASK)[ 1] = 0x080f0e0d0c0b0a09ULL;\
  ((u64*)SUBSH_MASK)[ 2] = 0x0007060504030201ULL;\
  ((u64*)SUBSH_MASK)[ 3] = 0x0a09080f0e0d0c0bULL;\
  ((u64*)SUBSH_MASK)[ 4] = 0x0100070605040302ULL;\
  ((u64*)SUBSH_MASK)[ 5] = 0x0c0b0a09080f0e0dULL;\
  ((u64*)SUBSH_MASK)[ 6] = 0x0201000706050403ULL;\
  ((u64*)SUBSH_MASK)[ 7] = 0x0e0d0c0b0a09080fULL;\
  ((u64*)SUBSH_MASK)[ 8] = 0x0302010007060504ULL;\
  ((u64*)SUBSH_MASK)[ 9] = 0x0f0e0d0c0b0a0908ULL;\
  ((u64*)SUBSH_MASK)[10] = 0x0403020100070605ULL;\
  ((u64*)SUBSH_MASK)[11] = 0x09080f0e0d0c0b0aULL;\
  ((u64*)SUBSH_MASK)[12] = 0x0504030201000706ULL;\
  ((u64*)SUBSH_MASK)[13] = 0x0b0a09080f0e0d0cULL;\
  ((u64*)SUBSH_MASK)[14] = 0x0605040302010007ULL;\
  ((u64*)SUBSH_MASK)[15] = 0x0d0c0b0a09080f0eULL;\
  for(i = 0; i < ROUNDS512; i++)\
  {\
    ((u64*)ROUND_CONST_L0)[i*2+1] = 0xffffffffffffffffULL;\
    ((u64*)ROUND_CONST_L0)[i*2+0] = (i * 0x0101010101010101ULL)  ^ 0x7060504030201000ULL;\
    ((u64*)ROUND_CONST_L7)[i*2+1] = (i * 0x0101010101010101ULL)  ^ 0x8f9fafbfcfdfefffULL;\
    ((u64*)ROUND_CONST_L7)[i*2+0] = 0x0000000000000000ULL;\
  }\
  ((u64*)ROUND_CONST_Lx)[1] = 0xffffffffffffffffULL;\
  ((u64*)ROUND_CONST_Lx)[0] = 0x0000000000000000ULL;\
}/**/

#define Push_All_Regs(){\
/*  not using any...
    asm("push rax");\
    asm("push rbx");\
    asm("push rcx");*/\
}/**/

#define Pop_All_Regs(){\
/*  not using any...
    asm("pop rcx");\
    asm("pop rbx");\
    asm("pop rax");*/\
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
  asm ("movaps xmm"tostr(b1)", [ROUND_CONST_Lx]");\
  asm ("pxor   xmm"tostr(a0)", [ROUND_CONST_L0+"tostr(i)"*16]");\
  asm ("pxor   xmm"tostr(a1)", xmm"tostr(b1)"");\
  asm ("pxor   xmm"tostr(a2)", xmm"tostr(b1)"");\
  asm ("pxor   xmm"tostr(a3)", xmm"tostr(b1)"");\
  asm ("pshufb xmm"tostr(a0)", [SUBSH_MASK+0*16]");\
  asm ("pshufb xmm"tostr(a1)", [SUBSH_MASK+1*16]");\
  asm ("pxor   xmm"tostr(a4)", xmm"tostr(b1)"");\
  asm ("pshufb xmm"tostr(a2)", [SUBSH_MASK+2*16]");\
  asm ("pshufb xmm"tostr(a3)", [SUBSH_MASK+3*16]");\
  asm ("pxor   xmm"tostr(a5)", xmm"tostr(b1)"");\
  asm ("pxor   xmm"tostr(a6)", xmm"tostr(b1)"");\
  asm ("pshufb xmm"tostr(a4)", [SUBSH_MASK+4*16]");\
  asm ("pshufb xmm"tostr(a5)", [SUBSH_MASK+5*16]");\
  asm ("pxor   xmm"tostr(a7)", [ROUND_CONST_L7+"tostr(i)"*16]");\
  asm ("pshufb xmm"tostr(a6)", [SUBSH_MASK+6*16]");\
  asm ("pshufb xmm"tostr(a7)", [SUBSH_MASK+7*16]");\
  /* SubBytes + Multiplication by 2 and 4 */\
  VPERM_SUB_MULTIPLY(a0, a1, a2, a3, a4, a5, a6, a7, b1, b2, b5, b6, b0, b3, b4, b7);\
  /* MixBytes */\
  MixBytes(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7);\
}/**/

/* 10 rounds, P and Q in parallel */
#define ROUNDS_P_Q(){\
  VPERM_Add_Constant(8, 9, 10, 11, 12, 13, 14, 15, ALL_15, 0);\
  ROUND(0, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7);\
  ROUND(1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);\
  ROUND(2, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7);\
  ROUND(3, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);\
  ROUND(4, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7);\
  ROUND(5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);\
  ROUND(6, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7);\
  ROUND(7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);\
  ROUND(8, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7);\
  ROUND(9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);\
  VPERM_Add_Constant(8, 9, 10, 11, 12, 13, 14, 15, ALL_15, 0);\
}


/* Matrix Transpose Step 1
 * input is a 512-bit state with two columns in one xmm
 * output is a 512-bit state with two rows in one xmm
 * inputs: i0-i3
 * outputs: i0, o1-o3
 * clobbers: t0
 */
#define Matrix_Transpose_A(i0, i1, i2, i3, o1, o2, o3, t0){\
  asm ("movaps xmm"tostr(t0)", [TRANSP_MASK]");\
\
  asm ("pshufb xmm"tostr(i0)", xmm"tostr(t0)"");\
  asm ("pshufb xmm"tostr(i1)", xmm"tostr(t0)"");\
  asm ("pshufb xmm"tostr(i2)", xmm"tostr(t0)"");\
  asm ("pshufb xmm"tostr(i3)", xmm"tostr(t0)"");\
\
  asm ("movdqa xmm"tostr(o1)", xmm"tostr(i0)"");\
  asm ("movdqa xmm"tostr(t0)", xmm"tostr(i2)"");\
\
  asm ("punpcklwd xmm"tostr(i0)", xmm"tostr(i1)"");\
  asm ("punpckhwd xmm"tostr(o1)", xmm"tostr(i1)"");\
  asm ("punpcklwd xmm"tostr(i2)", xmm"tostr(i3)"");\
  asm ("punpckhwd xmm"tostr(t0)", xmm"tostr(i3)"");\
\
  asm ("pshufd xmm"tostr(i0)", xmm"tostr(i0)", 216");\
  asm ("pshufd xmm"tostr(o1)", xmm"tostr(o1)", 216");\
  asm ("pshufd xmm"tostr(i2)", xmm"tostr(i2)", 216");\
  asm ("pshufd xmm"tostr(t0)", xmm"tostr(t0)", 216");\
\
  asm ("movdqa xmm"tostr(o2)", xmm"tostr(i0)"");\
  asm ("movdqa xmm"tostr(o3)", xmm"tostr(o1)"");\
\
  asm ("punpckldq xmm"tostr(i0)", xmm"tostr(i2)"");\
  asm ("punpckldq xmm"tostr(o1)", xmm"tostr(t0)"");\
  asm ("punpckhdq xmm"tostr(o2)", xmm"tostr(i2)"");\
  asm ("punpckhdq xmm"tostr(o3)", xmm"tostr(t0)"");\
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
  asm ("movdqa     xmm"tostr(o1)", xmm"tostr(i0)"");\
  asm ("movdqa     xmm"tostr(o2)", xmm"tostr(i1)"");\
  asm ("punpcklqdq xmm"tostr(i0)", xmm"tostr(i4)"");\
  asm ("punpckhqdq xmm"tostr(o1)", xmm"tostr(i4)"");\
  asm ("movdqa     xmm"tostr(o3)", xmm"tostr(i1)"");\
  asm ("movdqa     xmm"tostr(o4)", xmm"tostr(i2)"");\
  asm ("punpcklqdq xmm"tostr(o2)", xmm"tostr(i5)"");\
  asm ("punpckhqdq xmm"tostr(o3)", xmm"tostr(i5)"");\
  asm ("movdqa     xmm"tostr(o5)", xmm"tostr(i2)"");\
  asm ("movdqa     xmm"tostr(o6)", xmm"tostr(i3)"");\
  asm ("punpcklqdq xmm"tostr(o4)", xmm"tostr(i6)"");\
  asm ("punpckhqdq xmm"tostr(o5)", xmm"tostr(i6)"");\
  asm ("movdqa     xmm"tostr(o7)", xmm"tostr(i3)"");\
  asm ("punpcklqdq xmm"tostr(o6)", xmm"tostr(i7)"");\
  asm ("punpckhqdq xmm"tostr(o7)", xmm"tostr(i7)"");\
}/**/

/* Matrix Transpose Inverse Step 2
 * input are two 512-bit states with one row of each state in one xmm
 * output are two 512-bit states with two rows in one xmm
 * inputs: i0-i7 = (P|Q)
 * outputs: (i0, i2, i4, i6) = P, (o0-o3) = Q
 */
#define Matrix_Transpose_B_INV(i0, i1, i2, i3, i4, i5, i6, i7, o0, o1, o2, o3){\
  asm ("movdqa     xmm"tostr(o0)", xmm"tostr(i0)"");\
  asm ("punpcklqdq xmm"tostr(i0)", xmm"tostr(i1)"");\
  asm ("punpckhqdq xmm"tostr(o0)", xmm"tostr(i1)"");\
  asm ("movdqa     xmm"tostr(o1)", xmm"tostr(i2)"");\
  asm ("punpcklqdq xmm"tostr(i2)", xmm"tostr(i3)"");\
  asm ("punpckhqdq xmm"tostr(o1)", xmm"tostr(i3)"");\
  asm ("movdqa     xmm"tostr(o2)", xmm"tostr(i4)"");\
  asm ("punpcklqdq xmm"tostr(i4)", xmm"tostr(i5)"");\
  asm ("punpckhqdq xmm"tostr(o2)", xmm"tostr(i5)"");\
  asm ("movdqa     xmm"tostr(o3)", xmm"tostr(i6)"");\
  asm ("punpcklqdq xmm"tostr(i6)", xmm"tostr(i7)"");\
  asm ("punpckhqdq xmm"tostr(o3)", xmm"tostr(i7)"");\
}/**/

/* Matrix Transpose Output Step 2
 * input is one 512-bit state with two rows in one xmm
 * output is one 512-bit state with one row in the low 64-bits of one xmm
 * inputs: i0,i2,i4,i6 = S
 * outputs: (i0-7) = (0|S)
 */
#define Matrix_Transpose_O_B(i0, i1, i2, i3, i4, i5, i6, i7, t0){\
  asm ("pxor xmm"tostr(t0)", xmm"tostr(t0)"");\
  asm ("movdqa xmm"tostr(i1)", xmm"tostr(i0)"");\
  asm ("movdqa xmm"tostr(i3)", xmm"tostr(i2)"");\
  asm ("movdqa xmm"tostr(i5)", xmm"tostr(i4)"");\
  asm ("movdqa xmm"tostr(i7)", xmm"tostr(i6)"");\
  asm ("punpcklqdq xmm"tostr(i0)", xmm"tostr(t0)"");\
  asm ("punpckhqdq xmm"tostr(i1)", xmm"tostr(t0)"");\
  asm ("punpcklqdq xmm"tostr(i2)", xmm"tostr(t0)"");\
  asm ("punpckhqdq xmm"tostr(i3)", xmm"tostr(t0)"");\
  asm ("punpcklqdq xmm"tostr(i4)", xmm"tostr(t0)"");\
  asm ("punpckhqdq xmm"tostr(i5)", xmm"tostr(t0)"");\
  asm ("punpcklqdq xmm"tostr(i6)", xmm"tostr(t0)"");\
  asm ("punpckhqdq xmm"tostr(i7)", xmm"tostr(t0)"");\
}/**/

/* Matrix Transpose Output Inverse Step 2
 * input is one 512-bit state with one row in the low 64-bits of one xmm
 * output is one 512-bit state with two rows in one xmm
 * inputs: i0-i7 = (0|S)
 * outputs: (i0, i2, i4, i6) = S
 */
#define Matrix_Transpose_O_B_INV(i0, i1, i2, i3, i4, i5, i6, i7){\
  asm ("punpcklqdq xmm"tostr(i0)", xmm"tostr(i1)"");\
  asm ("punpcklqdq xmm"tostr(i2)", xmm"tostr(i3)"");\
  asm ("punpcklqdq xmm"tostr(i4)", xmm"tostr(i5)"");\
  asm ("punpcklqdq xmm"tostr(i6)", xmm"tostr(i7)"");\
}/**/


/* transform round constants into VPERM mode */
#define VPERM_Transform_RoundConst_CNT2(i, j){\
  asm ("movaps xmm0, [ROUND_CONST_L0+"tostr(i)"*16]");\
  asm ("movaps xmm1, [ROUND_CONST_L7+"tostr(i)"*16]");\
  asm ("movaps xmm2, [ROUND_CONST_L0+"tostr(j)"*16]");\
  asm ("movaps xmm3, [ROUND_CONST_L7+"tostr(j)"*16]");\
  VPERM_Transform_State(0, 1, 2, 3, VPERM_IPT, 4, 5, 6, 7, 8, 9, 10);\
  asm ("pxor xmm0, [ALL_15]");\
  asm ("pxor xmm1, [ALL_15]");\
  asm ("pxor xmm2, [ALL_15]");\
  asm ("pxor xmm3, [ALL_15]");\
  asm ("movaps [ROUND_CONST_L0+"tostr(i)"*16], xmm0");\
  asm ("movaps [ROUND_CONST_L7+"tostr(i)"*16], xmm1");\
  asm ("movaps [ROUND_CONST_L0+"tostr(j)"*16], xmm2");\
  asm ("movaps [ROUND_CONST_L7+"tostr(j)"*16], xmm3");\
}/**/

/* transform round constants into VPERM mode */
#define VPERM_Transform_RoundConst(){\
  asm ("movaps xmm0, [ROUND_CONST_Lx]");\
  VPERM_Transform(0, 1, VPERM_IPT, 4, 5, 6, 7, 8, 9, 10);\
  asm ("pxor xmm0, [ALL_15]");\
  asm ("movaps [ROUND_CONST_Lx], xmm0");\
  VPERM_Transform_RoundConst_CNT2(0, 1);\
  VPERM_Transform_RoundConst_CNT2(2, 3);\
  VPERM_Transform_RoundConst_CNT2(4, 5);\
  VPERM_Transform_RoundConst_CNT2(6, 7);\
  VPERM_Transform_RoundConst_CNT2(8, 9);\
}/**/

void INIT(u64* h)
{
  /* __cdecl calling convention: */
  /* chaining value CV in rdi    */

  asm (".intel_syntax noprefix");
  asm volatile ("emms");

  /* transform round constants into VPERM mode */
  VPERM_Transform_RoundConst();

  /* load IV into registers xmm12 - xmm15 */
  asm ("movaps xmm12, [rdi+0*16]");
  asm ("movaps xmm13, [rdi+1*16]");
  asm ("movaps xmm14, [rdi+2*16]");
  asm ("movaps xmm15, [rdi+3*16]");

  /* transform chaining value from column ordering into row ordering */
  /* we put two rows (64 bit) of the IV into one 128-bit XMM register */
  VPERM_Transform_State(12, 13, 14, 15, VPERM_IPT, 1, 2, 3, 4, 5, 6, 7);
  Matrix_Transpose_A(12, 13, 14, 15, 2, 6, 7, 0);

  /* store transposed IV */
  asm ("movaps [rdi+0*16], xmm12");
  asm ("movaps [rdi+1*16], xmm2");
  asm ("movaps [rdi+2*16], xmm6");
  asm ("movaps [rdi+3*16], xmm7");

  asm volatile ("emms");
  asm (".att_syntax noprefix");
}

void TF512(u64* h, u64* m)
{
  /* __cdecl calling convention: */
  /* chaining value CV in rdi    */
  /* message M in rsi            */

#ifdef IACA_TRACE
  IACA_START;
#endif

  asm (".intel_syntax noprefix");
  Push_All_Regs();

  /* load message into registers xmm12 - xmm15 (Q = message) */
  asm ("movaps xmm12, [rsi+0*16]");
  asm ("movaps xmm13, [rsi+1*16]");
  asm ("movaps xmm14, [rsi+2*16]");
  asm ("movaps xmm15, [rsi+3*16]");

  /* transform message M from column ordering into row ordering */
  /* we first put two rows (64 bit) of the message into one 128-bit xmm register */
  VPERM_Transform_State(12, 13, 14, 15, VPERM_IPT, 1, 2, 3, 4, 5, 6, 7);
  Matrix_Transpose_A(12, 13, 14, 15, 2, 6, 7, 0);

  /* load previous chaining value */
  /* we first put two rows (64 bit) of the CV into one 128-bit xmm register */
  asm ("movaps xmm8, [rdi+0*16]");
  asm ("movaps xmm0, [rdi+1*16]");
  asm ("movaps xmm4, [rdi+2*16]");
  asm ("movaps xmm5, [rdi+3*16]");

  /* xor message to CV get input of P */
  /* result: CV+M in xmm8, xmm0, xmm4, xmm5 */
  asm ("pxor xmm8, xmm12");
  asm ("pxor xmm0, xmm2");
  asm ("pxor xmm4, xmm6");
  asm ("pxor xmm5, xmm7");

  /* there are now 2 rows of the Groestl state (P and Q) in each xmm register */
  /* unpack to get 1 row of P (64 bit) and Q (64 bit) into one xmm register */
  /* result: the 8 rows of P and Q in xmm8 - xmm12 */
  Matrix_Transpose_B(8, 0, 4, 5, 12, 2, 6, 7, 9, 10, 11, 12, 13, 14, 15);

  /* compute the two permutations P and Q in parallel */
  ROUNDS_P_Q();

  /* unpack again to get two rows of P or two rows of Q in one xmm register */
  Matrix_Transpose_B_INV(8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3);

  /* xor output of P and Q */
  /* result: P(CV+M)+Q(M) in xmm0...xmm3 */
  asm ("pxor xmm0, xmm8");
  asm ("pxor xmm1, xmm10");
  asm ("pxor xmm2, xmm12");
  asm ("pxor xmm3, xmm14");

  /* xor CV (feed-forward) */
  /* result: P(CV+M)+Q(M)+CV in xmm0...xmm3 */
  asm ("pxor xmm0, [rdi+0*16]");
  asm ("pxor xmm1, [rdi+1*16]");
  asm ("pxor xmm2, [rdi+2*16]");
  asm ("pxor xmm3, [rdi+3*16]");

  /* store CV */
  asm ("movaps [rdi+0*16], xmm0");
  asm ("movaps [rdi+1*16], xmm1");
  asm ("movaps [rdi+2*16], xmm2");
  asm ("movaps [rdi+3*16], xmm3");

  Pop_All_Regs();
  asm (".att_syntax noprefix");

#ifdef IACA_TRACE
  IACA_END;
#endif

  return;
}

void OF512(u64* h)
{
  /* __cdecl calling convention: */
  /* chaining value CV in rdi    */

  asm (".intel_syntax noprefix");
  Push_All_Regs();

  /* load CV into registers xmm8, xmm10, xmm12, xmm14 */
  asm ("movaps xmm8,  [rdi+0*16]");
  asm ("movaps xmm10, [rdi+1*16]");
  asm ("movaps xmm12, [rdi+2*16]");
  asm ("movaps xmm14, [rdi+3*16]");

  /* there are now 2 rows of the CV in one xmm register */
  /* unpack to get 1 row of P (64 bit) into one half of an xmm register */
  /* result: the 8 input rows of P in xmm8 - xmm15 */
  Matrix_Transpose_O_B(8, 9, 10, 11, 12, 13, 14, 15, 0);

  /* compute the permutation P */
  /* result: the output of P(CV) in xmm8 - xmm15 */
  ROUNDS_P_Q();

  /* unpack again to get two rows of P in one xmm register */
  /* result: P(CV) in xmm8, xmm10, xmm12, xmm14 */
  Matrix_Transpose_O_B_INV(8, 9, 10, 11, 12, 13, 14, 15);

  /* xor CV to P output (feed-forward) */
  /* result: P(CV)+CV in xmm8, xmm10, xmm12, xmm14 */
  asm ("pxor xmm8,  [rdi+0*16]");
  asm ("pxor xmm10, [rdi+1*16]");
  asm ("pxor xmm12, [rdi+2*16]");
  asm ("pxor xmm14, [rdi+3*16]");

  /* transform state back from row ordering into column ordering */
  /* result: final hash value in xmm9, xmm11 */
  Matrix_Transpose_A(8, 10, 12, 14, 4, 9, 11, 0);
  VPERM_Transform(9, 11, VPERM_OPT, 0, 1, 2, 3, 5, 6, 7);

  /* we only need to return the truncated half of the state */
  asm ("movaps [rdi+2*16], xmm9");
  asm ("movaps [rdi+3*16], xmm11");

  Pop_All_Regs();
  asm (".att_syntax noprefix");

  return;
}

#endif

#if (LENGTH > 256)

#define SET_CONSTANTS(){\
  SET_SHARED_CONSTANTS();\
  ((u64*)ALL_FF)[0] = 0xffffffffffffffffULL;\
  ((u64*)ALL_FF)[1] = 0xffffffffffffffffULL;\
  ((u64*)SUBSH_MASK)[ 0] = 0x0706050403020100ULL;\
  ((u64*)SUBSH_MASK)[ 1] = 0x0f0e0d0c0b0a0908ULL;\
  ((u64*)SUBSH_MASK)[ 2] = 0x0807060504030201ULL;\
  ((u64*)SUBSH_MASK)[ 3] = 0x000f0e0d0c0b0a09ULL;\
  ((u64*)SUBSH_MASK)[ 4] = 0x0908070605040302ULL;\
  ((u64*)SUBSH_MASK)[ 5] = 0x01000f0e0d0c0b0aULL;\
  ((u64*)SUBSH_MASK)[ 6] = 0x0a09080706050403ULL;\
  ((u64*)SUBSH_MASK)[ 7] = 0x0201000f0e0d0c0bULL;\
  ((u64*)SUBSH_MASK)[ 8] = 0x0b0a090807060504ULL;\
  ((u64*)SUBSH_MASK)[ 9] = 0x030201000f0e0d0cULL;\
  ((u64*)SUBSH_MASK)[10] = 0x0c0b0a0908070605ULL;\
  ((u64*)SUBSH_MASK)[11] = 0x04030201000f0e0dULL;\
  ((u64*)SUBSH_MASK)[12] = 0x0d0c0b0a09080706ULL;\
  ((u64*)SUBSH_MASK)[13] = 0x0504030201000f0eULL;\
  ((u64*)SUBSH_MASK)[14] = 0x0201000f0e0d0c0bULL;\
  ((u64*)SUBSH_MASK)[15] = 0x0a09080706050403ULL;\
  for(i = 0; i < ROUNDS1024; i++)\
  {\
    ((u64*)ROUND_CONST_P)[2*i+1] = (i * 0x0101010101010101ULL)  ^ 0xf0e0d0c0b0a09080ULL;\
    ((u64*)ROUND_CONST_P)[2*i+0] = (i * 0x0101010101010101ULL)  ^ 0x7060504030201000ULL;\
    ((u64*)ROUND_CONST_Q)[2*i+1] = (i * 0x0101010101010101ULL)  ^ 0x0f1f2f3f4f5f6f7fULL;\
    ((u64*)ROUND_CONST_Q)[2*i+0] = (i * 0x0101010101010101ULL)  ^ 0x8f9fafbfcfdfefffULL;\
  }\
}/**/

#define Push_All_Regs(){\
    asm("push rax");\
    asm("push rbx");\
    asm("push rcx");\
}/**/

#define Pop_All_Regs(){\
    asm("pop rcx");\
    asm("pop rbx");\
    asm("pop rax");\
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
  asm ("xor rax, rax");\
  asm ("xor rbx, rbx");\
  asm ("add bl, 2");\
  asm ("1:");\
  /* AddRoundConstant P1024 */\
  asm ("pxor xmm8, [ROUND_CONST_P+eax*8]");\
  /* ShiftBytes P1024 + pre-AESENCLAST */\
  asm ("pshufb xmm8,  [SUBSH_MASK+0*16]");\
  asm ("pshufb xmm9,  [SUBSH_MASK+1*16]");\
  asm ("pshufb xmm10, [SUBSH_MASK+2*16]");\
  asm ("pshufb xmm11, [SUBSH_MASK+3*16]");\
  asm ("pshufb xmm12, [SUBSH_MASK+4*16]");\
  asm ("pshufb xmm13, [SUBSH_MASK+5*16]");\
  asm ("pshufb xmm14, [SUBSH_MASK+6*16]");\
  asm ("pshufb xmm15, [SUBSH_MASK+7*16]");\
  /* SubBytes + MixBytes */\
  SUBMIX(8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7);\
  VPERM_Add_Constant(0, 1, 2, 3, 4, 5, 6, 7, ALL_15, 8);\
  /* AddRoundConstant P1024 */\
  asm ("pxor xmm0, [ROUND_CONST_P+ebx*8]");\
  /* ShiftBytes P1024 + pre-AESENCLAST */\
  asm ("pshufb xmm0, [SUBSH_MASK+0*16]");\
  asm ("pshufb xmm1, [SUBSH_MASK+1*16]");\
  asm ("pshufb xmm2, [SUBSH_MASK+2*16]");\
  asm ("pshufb xmm3, [SUBSH_MASK+3*16]");\
  asm ("pshufb xmm4, [SUBSH_MASK+4*16]");\
  asm ("pshufb xmm5, [SUBSH_MASK+5*16]");\
  asm ("pshufb xmm6, [SUBSH_MASK+6*16]");\
  asm ("pshufb xmm7, [SUBSH_MASK+7*16]");\
  /* SubBytes + MixBytes */\
  SUBMIX(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);\
  VPERM_Add_Constant(8, 9, 10, 11, 12, 13, 14, 15, ALL_15, 0);\
  asm ("add al, 4");\
  asm ("add bl, 4");\
  asm ("mov rcx, rax");\
  asm ("sub cl, 28");\
  asm ("jb 1b");\
}/**/

#define ROUNDS_Q(){\
  VPERM_Add_Constant(8, 9, 10, 11, 12, 13, 14, 15, ALL_15, 1);\
  asm ("xor rax, rax");\
  asm ("xor rbx, rbx");\
  asm ("add bl, 2");\
  asm ("2:");\
  /* AddRoundConstant Q1024 */\
  asm ("movaps xmm1,  [ALL_FF]");\
  asm ("pxor xmm8,  xmm1");\
  asm ("pxor xmm9,  xmm1");\
  asm ("pxor xmm10, xmm1");\
  asm ("pxor xmm11, xmm1");\
  asm ("pxor xmm12, xmm1");\
  asm ("pxor xmm13, xmm1");\
  asm ("pxor xmm14, xmm1");\
  asm ("pxor xmm15, [ROUND_CONST_Q+eax*8]");\
  /* ShiftBytes Q1024 + pre-AESENCLAST */\
  asm ("pshufb xmm8,  [SUBSH_MASK+1*16]");\
  asm ("pshufb xmm9,  [SUBSH_MASK+3*16]");\
  asm ("pshufb xmm10, [SUBSH_MASK+5*16]");\
  asm ("pshufb xmm11, [SUBSH_MASK+7*16]");\
  asm ("pshufb xmm12, [SUBSH_MASK+0*16]");\
  asm ("pshufb xmm13, [SUBSH_MASK+2*16]");\
  asm ("pshufb xmm14, [SUBSH_MASK+4*16]");\
  asm ("pshufb xmm15, [SUBSH_MASK+6*16]");\
  /* SubBytes + MixBytes */\
  SUBMIX(8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7);\
  /* AddRoundConstant Q1024 */\
  asm ("movaps xmm9,  [ALL_FF]");\
  asm ("pxor xmm0,  xmm9");\
  asm ("pxor xmm1,  xmm9");\
  asm ("pxor xmm2,  xmm9");\
  asm ("pxor xmm3,  xmm9");\
  asm ("pxor xmm4,  xmm9");\
  asm ("pxor xmm5,  xmm9");\
  asm ("pxor xmm6,  xmm9");\
  asm ("pxor xmm7,  [ROUND_CONST_Q+ebx*8]");\
  /* ShiftBytes Q1024 + pre-AESENCLAST */\
  asm ("pshufb xmm0, [SUBSH_MASK+1*16]");\
  asm ("pshufb xmm1, [SUBSH_MASK+3*16]");\
  asm ("pshufb xmm2, [SUBSH_MASK+5*16]");\
  asm ("pshufb xmm3, [SUBSH_MASK+7*16]");\
  asm ("pshufb xmm4, [SUBSH_MASK+0*16]");\
  asm ("pshufb xmm5, [SUBSH_MASK+2*16]");\
  asm ("pshufb xmm6, [SUBSH_MASK+4*16]");\
  asm ("pshufb xmm7, [SUBSH_MASK+6*16]");\
  /* SubBytes + MixBytes */\
  SUBMIX(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);\
  asm ("add al, 4");\
  asm ("add bl, 4");\
  asm ("mov rcx, rax");\
  asm ("sub cl, 28");\
  asm ("jb 2b");\
  VPERM_Add_Constant(8, 9, 10, 11, 12, 13, 14, 15, ALL_15, 1);\
}/**/


/* Matrix Transpose
 * input is a 1024-bit state with two columns in one xmm
 * output is a 1024-bit state with two rows in one xmm
 * inputs: i0-i7
 * outputs: i0-i7
 * clobbers: t0-t7
 */
#define Matrix_Transpose(i0, i1, i2, i3, i4, i5, i6, i7, t0, t1, t2, t3, t4, t5, t6, t7){\
  asm ("movaps xmm"tostr(t0)", [TRANSP_MASK]");\
\
  asm ("pshufb xmm"tostr(i6)", xmm"tostr(t0)"");\
  asm ("pshufb xmm"tostr(i0)", xmm"tostr(t0)"");\
  asm ("pshufb xmm"tostr(i1)", xmm"tostr(t0)"");\
  asm ("pshufb xmm"tostr(i2)", xmm"tostr(t0)"");\
  asm ("pshufb xmm"tostr(i3)", xmm"tostr(t0)"");\
  asm ("movdqa xmm"tostr(t1)", xmm"tostr(i2)"");\
  asm ("pshufb xmm"tostr(i4)", xmm"tostr(t0)"");\
  asm ("pshufb xmm"tostr(i5)", xmm"tostr(t0)"");\
  asm ("movdqa xmm"tostr(t2)", xmm"tostr(i4)"");\
  asm ("movdqa xmm"tostr(t3)", xmm"tostr(i6)"");\
  asm ("pshufb xmm"tostr(i7)", xmm"tostr(t0)"");\
\
  /* continue with unpack using 4 temp registers */\
  asm ("movdqa xmm"tostr(t0)", xmm"tostr(i0)"");\
  asm ("punpckhwd xmm"tostr(t2)", xmm"tostr(i5)"");\
  asm ("punpcklwd xmm"tostr(i4)", xmm"tostr(i5)"");\
  asm ("punpckhwd xmm"tostr(t3)", xmm"tostr(i7)"");\
  asm ("punpcklwd xmm"tostr(i6)", xmm"tostr(i7)"");\
  asm ("punpckhwd xmm"tostr(t0)", xmm"tostr(i1)"");\
  asm ("punpckhwd xmm"tostr(t1)", xmm"tostr(i3)"");\
  asm ("punpcklwd xmm"tostr(i2)", xmm"tostr(i3)"");\
  asm ("punpcklwd xmm"tostr(i0)", xmm"tostr(i1)"");\
\
  /* shuffle with immediate */\
  asm ("pshufd xmm"tostr(t0)", xmm"tostr(t0)", 216");\
  asm ("pshufd xmm"tostr(t1)", xmm"tostr(t1)", 216");\
  asm ("pshufd xmm"tostr(t2)", xmm"tostr(t2)", 216");\
  asm ("pshufd xmm"tostr(t3)", xmm"tostr(t3)", 216");\
  asm ("pshufd xmm"tostr(i0)", xmm"tostr(i0)", 216");\
  asm ("pshufd xmm"tostr(i2)", xmm"tostr(i2)", 216");\
  asm ("pshufd xmm"tostr(i4)", xmm"tostr(i4)", 216");\
  asm ("pshufd xmm"tostr(i6)", xmm"tostr(i6)", 216");\
\
  /* continue with unpack */\
  asm ("movdqa xmm"tostr(t4)", xmm"tostr(i0)"");\
  asm ("punpckldq xmm"tostr(i0)",  xmm"tostr(i2)"");\
  asm ("punpckhdq xmm"tostr(t4)",  xmm"tostr(i2)"");\
  asm ("movdqa xmm"tostr(t5)", xmm"tostr(t0)"");\
  asm ("punpckldq xmm"tostr(t0)",  xmm"tostr(t1)"");\
  asm ("punpckhdq xmm"tostr(t5)",  xmm"tostr(t1)"");\
  asm ("movdqa xmm"tostr(t6)", xmm"tostr(i4)"");\
  asm ("punpckldq xmm"tostr(i4)", xmm"tostr(i6)"");\
  asm ("movdqa xmm"tostr(t7)", xmm"tostr(t2)"");\
  asm ("punpckhdq xmm"tostr(t6)",  xmm"tostr(i6)"");\
  asm ("movdqa xmm"tostr(i2)", xmm"tostr(t0)"");\
  asm ("punpckldq xmm"tostr(t2)",  xmm"tostr(t3)"");\
  asm ("movdqa xmm"tostr(i3)", xmm"tostr(t0)"");\
  asm ("punpckhdq xmm"tostr(t7)",  xmm"tostr(t3)"");\
\
  /* there are now 2 rows in each xmm */\
  /* unpack to get 1 row of CV in each xmm */\
  asm ("movdqa xmm"tostr(i1)", xmm"tostr(i0)"");\
  asm ("punpckhqdq xmm"tostr(i1)", xmm"tostr(i4)"");\
  asm ("punpcklqdq xmm"tostr(i0)", xmm"tostr(i4)"");\
  asm ("movdqa xmm"tostr(i4)", xmm"tostr(t4)"");\
  asm ("punpckhqdq xmm"tostr(i3)", xmm"tostr(t2)"");\
  asm ("movdqa xmm"tostr(i5)", xmm"tostr(t4)"");\
  asm ("punpcklqdq xmm"tostr(i2)", xmm"tostr(t2)"");\
  asm ("movdqa xmm"tostr(i6)", xmm"tostr(t5)"");\
  asm ("punpckhqdq xmm"tostr(i5)", xmm"tostr(t6)"");\
  asm ("movdqa xmm"tostr(i7)", xmm"tostr(t5)"");\
  asm ("punpcklqdq xmm"tostr(i4)", xmm"tostr(t6)"");\
  asm ("punpckhqdq xmm"tostr(i7)", xmm"tostr(t7)"");\
  asm ("punpcklqdq xmm"tostr(i6)", xmm"tostr(t7)"");\
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
  asm ("movdqa xmm"tostr(o1)", xmm"tostr(i0)"");\
  asm ("punpcklqdq xmm"tostr(i0)", xmm"tostr(i1)"");\
  asm ("punpckhqdq xmm"tostr(o1)", xmm"tostr(i1)"");\
  asm ("movdqa xmm"tostr(t0)", xmm"tostr(i2)"");\
  asm ("punpcklqdq xmm"tostr(i2)", xmm"tostr(i3)"");\
  asm ("punpckhqdq xmm"tostr(t0)", xmm"tostr(i3)"");\
  asm ("movdqa xmm"tostr(t1)", xmm"tostr(i4)"");\
  asm ("punpcklqdq xmm"tostr(i4)", xmm"tostr(i5)"");\
  asm ("punpckhqdq xmm"tostr(t1)", xmm"tostr(i5)"");\
  asm ("movdqa xmm"tostr(t2)", xmm"tostr(i6)"");\
  asm ("movaps xmm"tostr(o0)", [TRANSP_MASK]");\
  asm ("punpcklqdq xmm"tostr(i6)", xmm"tostr(i7)"");\
  asm ("punpckhqdq xmm"tostr(t2)", xmm"tostr(i7)"");\
  /* load transpose mask into a register, because it will be used 8 times */\
  asm ("pshufb xmm"tostr(i0)", xmm"tostr(o0)"");\
  asm ("pshufb xmm"tostr(i2)", xmm"tostr(o0)"");\
  asm ("pshufb xmm"tostr(i4)", xmm"tostr(o0)"");\
  asm ("pshufb xmm"tostr(i6)", xmm"tostr(o0)"");\
  asm ("pshufb xmm"tostr(o1)", xmm"tostr(o0)"");\
  asm ("pshufb xmm"tostr(t0)", xmm"tostr(o0)"");\
  asm ("pshufb xmm"tostr(t1)", xmm"tostr(o0)"");\
  asm ("pshufb xmm"tostr(t2)", xmm"tostr(o0)"");\
  /* continue with unpack using 4 temp registers */\
  asm ("movdqa xmm"tostr(t3)", xmm"tostr(i4)"");\
  asm ("movdqa xmm"tostr(o2)", xmm"tostr(o1)"");\
  asm ("movdqa xmm"tostr(o0)", xmm"tostr(i0)"");\
  asm ("movdqa xmm"tostr(t4)", xmm"tostr(t1)"");\
  \
  asm ("punpckhwd xmm"tostr(t3)", xmm"tostr(i6)"");\
  asm ("punpcklwd xmm"tostr(i4)", xmm"tostr(i6)"");\
  asm ("punpckhwd xmm"tostr(o0)", xmm"tostr(i2)"");\
  asm ("punpcklwd xmm"tostr(i0)", xmm"tostr(i2)"");\
  asm ("punpckhwd xmm"tostr(o2)", xmm"tostr(t0)"");\
  asm ("punpcklwd xmm"tostr(o1)", xmm"tostr(t0)"");\
  asm ("punpckhwd xmm"tostr(t4)", xmm"tostr(t2)"");\
  asm ("punpcklwd xmm"tostr(t1)", xmm"tostr(t2)"");\
  /* shuffle with immediate */\
  asm ("pshufd xmm"tostr(i4)", xmm"tostr(i4)", 216");\
  asm ("pshufd xmm"tostr(t3)", xmm"tostr(t3)", 216");\
  asm ("pshufd xmm"tostr(o1)", xmm"tostr(o1)", 216");\
  asm ("pshufd xmm"tostr(o2)", xmm"tostr(o2)", 216");\
  asm ("pshufd xmm"tostr(i0)", xmm"tostr(i0)", 216");\
  asm ("pshufd xmm"tostr(o0)", xmm"tostr(o0)", 216");\
  asm ("pshufd xmm"tostr(t1)", xmm"tostr(t1)", 216");\
  asm ("pshufd xmm"tostr(t4)", xmm"tostr(t4)", 216");\
  /* continue with unpack */\
  asm ("movdqa xmm"tostr(i1)", xmm"tostr(i0)"");\
  asm ("movdqa xmm"tostr(i3)", xmm"tostr(o0)"");\
  asm ("movdqa xmm"tostr(i5)", xmm"tostr(o1)"");\
  asm ("movdqa xmm"tostr(i7)", xmm"tostr(o2)"");\
  asm ("punpckldq xmm"tostr(i0)", xmm"tostr(i4)"");\
  asm ("punpckhdq xmm"tostr(i1)", xmm"tostr(i4)"");\
  asm ("punpckldq xmm"tostr(o0)", xmm"tostr(t3)"");\
  asm ("punpckhdq xmm"tostr(i3)", xmm"tostr(t3)"");\
  asm ("punpckldq xmm"tostr(o1)", xmm"tostr(t1)"");\
  asm ("punpckhdq xmm"tostr(i5)", xmm"tostr(t1)"");\
  asm ("punpckldq xmm"tostr(o2)", xmm"tostr(t4)"");\
  asm ("punpckhdq xmm"tostr(i7)", xmm"tostr(t4)"");\
  /* transpose done */\
}/**/

/* transform round constants into VPERM mode */
#define VPERM_Transform_RoundConst_CNT2(i, j){\
  asm ("movaps xmm0, [ROUND_CONST_P+"tostr(i)"*16]");\
  asm ("movaps xmm1, [ROUND_CONST_P+"tostr(j)"*16]");\
  asm ("movaps xmm2, [ROUND_CONST_Q+"tostr(i)"*16]");\
  asm ("movaps xmm3, [ROUND_CONST_Q+"tostr(j)"*16]");\
  VPERM_Transform_State(0, 1, 2, 3, VPERM_IPT, 4, 5, 6, 7, 8, 9, 10);\
  asm ("pxor xmm2, [ALL_15]");\
  asm ("pxor xmm3, [ALL_15]");\
  asm ("movaps [ROUND_CONST_P+"tostr(i)"*16], xmm0");\
  asm ("movaps [ROUND_CONST_P+"tostr(j)"*16], xmm1");\
  asm ("movaps [ROUND_CONST_Q+"tostr(i)"*16], xmm2");\
  asm ("movaps [ROUND_CONST_Q+"tostr(j)"*16], xmm3");\
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
  asm ("movaps xmm0, [ALL_FF]");\
  VPERM_Transform(0, 1, VPERM_IPT, 4, 5, 6, 7, 8, 9, 10);\
  asm ("pxor xmm0, [ALL_15]");\
  asm ("movaps [ALL_FF], xmm0");\
}/**/


void INIT(u64* h)
{
  /* __cdecl calling convention: */
  /* chaining value CV in rdi    */

  asm (".intel_syntax noprefix");
  asm volatile ("emms");

  /* transform round constants into VPERM mode */
  VPERM_Transform_RoundConst();

  /* load IV into registers xmm8 - xmm15 */
  asm ("movaps xmm8,  [rdi+0*16]");
  asm ("movaps xmm9,  [rdi+1*16]");
  asm ("movaps xmm10, [rdi+2*16]");
  asm ("movaps xmm11, [rdi+3*16]");
  asm ("movaps xmm12, [rdi+4*16]");
  asm ("movaps xmm13, [rdi+5*16]");
  asm ("movaps xmm14, [rdi+6*16]");
  asm ("movaps xmm15, [rdi+7*16]");

  /* transform chaining value from column ordering into row ordering */
  VPERM_Transform_State( 8,  9, 10, 11, VPERM_IPT, 1, 2, 3, 4, 5, 6, 7);
  VPERM_Transform_State(12, 13, 14, 15, VPERM_IPT, 1, 2, 3, 4, 5, 6, 7);
  Matrix_Transpose(8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7);

  /* store transposed IV */
  asm ("movaps [rdi+0*16], xmm8");
  asm ("movaps [rdi+1*16], xmm9");
  asm ("movaps [rdi+2*16], xmm10");
  asm ("movaps [rdi+3*16], xmm11");
  asm ("movaps [rdi+4*16], xmm12");
  asm ("movaps [rdi+5*16], xmm13");
  asm ("movaps [rdi+6*16], xmm14");
  asm ("movaps [rdi+7*16], xmm15");

  asm volatile ("emms");
  asm (".att_syntax noprefix");
}

void TF1024(u64* h, u64* m)
{
  /* __cdecl calling convention: */
  /* chaining value CV in rdi    */
  /* message M in rsi            */

#ifdef IACA_TRACE
  IACA_START;
#endif

  asm (".intel_syntax noprefix");
  Push_All_Regs();

  /* load message into registers xmm8 - xmm15 (Q = message) */
  asm ("movaps xmm8,  [rsi+0*16]");
  asm ("movaps xmm9,  [rsi+1*16]");
  asm ("movaps xmm10, [rsi+2*16]");
  asm ("movaps xmm11, [rsi+3*16]");
  asm ("movaps xmm12, [rsi+4*16]");
  asm ("movaps xmm13, [rsi+5*16]");
  asm ("movaps xmm14, [rsi+6*16]");
  asm ("movaps xmm15, [rsi+7*16]");

  /* transform message M from column ordering into row ordering */
  VPERM_Transform_State( 8,  9, 10, 11, VPERM_IPT, 1, 2, 3, 4, 5, 6, 7);
  VPERM_Transform_State(12, 13, 14, 15, VPERM_IPT, 1, 2, 3, 4, 5, 6, 7);
  Matrix_Transpose(8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7);

  /* store message M (Q input) for later */
  asm ("movaps [QTEMP+0*16], xmm8");
  asm ("movaps [QTEMP+1*16], xmm9");
  asm ("movaps [QTEMP+2*16], xmm10");
  asm ("movaps [QTEMP+3*16], xmm11");
  asm ("movaps [QTEMP+4*16], xmm12");
  asm ("movaps [QTEMP+5*16], xmm13");
  asm ("movaps [QTEMP+6*16], xmm14");
  asm ("movaps [QTEMP+7*16], xmm15");

  /* xor CV to message to get P input */
  /* result: CV+M in xmm8...xmm15 */
  asm ("pxor xmm8,  [rdi+0*16]");
  asm ("pxor xmm9,  [rdi+1*16]");
  asm ("pxor xmm10, [rdi+2*16]");
  asm ("pxor xmm11, [rdi+3*16]");
  asm ("pxor xmm12, [rdi+4*16]");
  asm ("pxor xmm13, [rdi+5*16]");
  asm ("pxor xmm14, [rdi+6*16]");
  asm ("pxor xmm15, [rdi+7*16]");

  /* compute permutation P */
  /* result: P(CV+M) in xmm8...xmm15 */
  ROUNDS_P();

  /* xor CV to P output (feed-forward) */
  /* result: P(CV+M)+CV in xmm8...xmm15 */
  asm ("pxor xmm8,  [rdi+0*16]");
  asm ("pxor xmm9,  [rdi+1*16]");
  asm ("pxor xmm10, [rdi+2*16]");
  asm ("pxor xmm11, [rdi+3*16]");
  asm ("pxor xmm12, [rdi+4*16]");
  asm ("pxor xmm13, [rdi+5*16]");
  asm ("pxor xmm14, [rdi+6*16]");
  asm ("pxor xmm15, [rdi+7*16]");

  /* store P(CV+M)+CV */
  asm ("movaps [rdi+0*16], xmm8");
  asm ("movaps [rdi+1*16], xmm9");
  asm ("movaps [rdi+2*16], xmm10");
  asm ("movaps [rdi+3*16], xmm11");
  asm ("movaps [rdi+4*16], xmm12");
  asm ("movaps [rdi+5*16], xmm13");
  asm ("movaps [rdi+6*16], xmm14");
  asm ("movaps [rdi+7*16], xmm15");

  /* load message M (Q input) into xmm8-15 */
  asm ("movaps xmm8,  [QTEMP+0*16]");
  asm ("movaps xmm9,  [QTEMP+1*16]");
  asm ("movaps xmm10, [QTEMP+2*16]");
  asm ("movaps xmm11, [QTEMP+3*16]");
  asm ("movaps xmm12, [QTEMP+4*16]");
  asm ("movaps xmm13, [QTEMP+5*16]");
  asm ("movaps xmm14, [QTEMP+6*16]");
  asm ("movaps xmm15, [QTEMP+7*16]");

  /* compute permutation Q */
  /* result: Q(M) in xmm8...xmm15 */
  ROUNDS_Q();

  /* xor Q output */
  /* result: P(CV+M)+CV+Q(M) in xmm8...xmm15 */
  asm ("pxor xmm8,  [rdi+0*16]");
  asm ("pxor xmm9,  [rdi+1*16]");
  asm ("pxor xmm10, [rdi+2*16]");
  asm ("pxor xmm11, [rdi+3*16]");
  asm ("pxor xmm12, [rdi+4*16]");
  asm ("pxor xmm13, [rdi+5*16]");
  asm ("pxor xmm14, [rdi+6*16]");
  asm ("pxor xmm15, [rdi+7*16]");

  /* store CV */
  asm ("movaps [rdi+0*16], xmm8");
  asm ("movaps [rdi+1*16], xmm9");
  asm ("movaps [rdi+2*16], xmm10");
  asm ("movaps [rdi+3*16], xmm11");
  asm ("movaps [rdi+4*16], xmm12");
  asm ("movaps [rdi+5*16], xmm13");
  asm ("movaps [rdi+6*16], xmm14");
  asm ("movaps [rdi+7*16], xmm15");

  Pop_All_Regs();
  asm (".att_syntax noprefix");

#ifdef IACA_TRACE
  IACA_END;
#endif

  return;
}

void OF1024(u64* h)
{
  /* __cdecl calling convention: */
  /* chaining value CV in rdi    */

  asm (".intel_syntax noprefix");
  Push_All_Regs();

  /* load CV into registers xmm8 - xmm15 */
  asm ("movaps xmm8,  [rdi+0*16]");
  asm ("movaps xmm9,  [rdi+1*16]");
  asm ("movaps xmm10, [rdi+2*16]");
  asm ("movaps xmm11, [rdi+3*16]");
  asm ("movaps xmm12, [rdi+4*16]");
  asm ("movaps xmm13, [rdi+5*16]");
  asm ("movaps xmm14, [rdi+6*16]");
  asm ("movaps xmm15, [rdi+7*16]");

  /* compute permutation P */
  /* result: P(CV) in xmm8...xmm15 */
  ROUNDS_P();

  /* xor CV to P output (feed-forward) */
  /* result: P(CV)+CV in xmm8...xmm15 */
  asm ("pxor xmm8,  [rdi+0*16]");
  asm ("pxor xmm9,  [rdi+1*16]");
  asm ("pxor xmm10, [rdi+2*16]");
  asm ("pxor xmm11, [rdi+3*16]");
  asm ("pxor xmm12, [rdi+4*16]");
  asm ("pxor xmm13, [rdi+5*16]");
  asm ("pxor xmm14, [rdi+6*16]");
  asm ("pxor xmm15, [rdi+7*16]");

  /* transpose CV back from row ordering to column ordering */
  /* result: final hash value in xmm0, xmm6, xmm13, xmm15 */
  Matrix_Transpose_INV(8, 9, 10, 11, 12, 13, 14, 15, 4, 0, 6, 1, 2, 3, 5, 7);
  VPERM_Transform_State( 0, 6, 13, 15, VPERM_OPT, 1, 2, 3, 5, 7, 10, 12);

  /* we only need to return the truncated half of the state */
  asm ("movaps [rdi+4*16], xmm0");
  asm ("movaps [rdi+5*16], xmm6");
  asm ("movaps [rdi+6*16], xmm13");
  asm ("movaps [rdi+7*16], xmm15");

  Pop_All_Regs();
  asm (".att_syntax noprefix");

  return;
}

#endif

