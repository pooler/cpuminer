/* groestl-asm-avx.h     Aug 2011
 *
 * Groestl implementation with inline assembly using ssse3, sse4.1, aes and avx
 * instructions.
 * Author: Günther A. Roland, Martin Schläffer, Krystian Matusiewicz
 *
 * This code is placed in the public domain
 */

#include "hash-groestl.h"

/* global variables  */
__attribute__ ((aligned (32))) unsigned char ROUND_CONST_Lx[16];
__attribute__ ((aligned (32))) unsigned char ROUND_CONST_L0[ROUNDS512*16];
__attribute__ ((aligned (32))) unsigned char ROUND_CONST_L7[ROUNDS512*16];
__attribute__ ((aligned (32))) unsigned char ROUND_CONST_P[ROUNDS1024*16];
__attribute__ ((aligned (32))) unsigned char ROUND_CONST_Q[ROUNDS1024*16];
__attribute__ ((aligned (32))) unsigned char TRANSP_MASK[16];
__attribute__ ((aligned (32))) unsigned char SUBSH_MASK[8*16];
__attribute__ ((aligned (32))) unsigned char ALL_1B[32];
__attribute__ ((aligned (32))) unsigned char ALL_FF[32];

/* temporary variables  */
__attribute__ ((aligned (32))) unsigned char TEMP[6*32];


#define tos(a)    #a
#define tostr(a)  tos(a)

#if (LENGTH <= 256)

#define SET_CONSTANTS(){\
  ((u64*)TRANSP_MASK)[0] = 0x0d0509010c040800ULL;\
  ((u64*)TRANSP_MASK)[1] = 0x0f070b030e060a02ULL;\
  ((u64*)ALL_1B)[0] = 0x1b1b1b1b1b1b1b1bULL;\
  ((u64*)ALL_1B)[1] = 0x1b1b1b1b1b1b1b1bULL;\
  ((u64*)SUBSH_MASK)[ 0] = 0x0c0f0104070b0e00ULL;\
  ((u64*)SUBSH_MASK)[ 1] = 0x03060a0d08020509ULL;\
  ((u64*)SUBSH_MASK)[ 2] = 0x0e090205000d0801ULL;\
  ((u64*)SUBSH_MASK)[ 3] = 0x04070c0f0a03060bULL;\
  ((u64*)SUBSH_MASK)[ 4] = 0x080b0306010f0a02ULL;\
  ((u64*)SUBSH_MASK)[ 5] = 0x05000e090c04070dULL;\
  ((u64*)SUBSH_MASK)[ 6] = 0x0a0d040702090c03ULL;\
  ((u64*)SUBSH_MASK)[ 7] = 0x0601080b0e05000fULL;\
  ((u64*)SUBSH_MASK)[ 8] = 0x0b0e0500030a0d04ULL;\
  ((u64*)SUBSH_MASK)[ 9] = 0x0702090c0f060108ULL;\
  ((u64*)SUBSH_MASK)[10] = 0x0d080601040c0f05ULL;\
  ((u64*)SUBSH_MASK)[11] = 0x00030b0e0907020aULL;\
  ((u64*)SUBSH_MASK)[12] = 0x0f0a0702050e0906ULL;\
  ((u64*)SUBSH_MASK)[13] = 0x01040d080b00030cULL;\
  ((u64*)SUBSH_MASK)[14] = 0x090c000306080b07ULL;\
  ((u64*)SUBSH_MASK)[15] = 0x02050f0a0d01040eULL;\
  for(i = 0; i < ROUNDS512; i++)\
  {\
    ((u64*)ROUND_CONST_L0)[i*2+1] = 0xffffffffffffffffULL;\
    ((u64*)ROUND_CONST_L0)[i*2+0] = (i * 0x0101010101010101ULL)  ^ 0x7060504030201000ULL;\
    ((u64*)ROUND_CONST_L7)[i*2+1] = (i * 0x0101010101010101ULL)  ^ 0x8f9fafbfcfdfefffULL;\
    ((u64*)ROUND_CONST_L7)[i*2+0] = 0x0000000000000000ULL;\
  }\
  ((u64*)ROUND_CONST_Lx)[1] = 0xffffffffffffffffULL;\
  ((u64*)ROUND_CONST_Lx)[0] = 0x0000000000000000ULL;\
}while(0);

#define Push_All_Regs() do{\
/*  not using any...
    asm("push rax");\
    asm("push rbx");\
    asm("push rcx");*/\
}while(0);

#define Pop_All_Regs() do{\
/*  not using any...
    asm("pop rcx");\
    asm("pop rbx");\
    asm("pop rax");*/\
}while(0);

/* xmm[i] will be multiplied by 2
 * xmm[j] will be lost
 * xmm[k] has to be all 0x1b
 * xmm[z] has to be zero */
#define VMUL2(i, j, k, z){\
  asm("vpcmpgtb xmm"tostr(j)", xmm"tostr(z)", xmm"tostr(i)"");\
  asm("vpaddb xmm"tostr(i)", xmm"tostr(i)", xmm"tostr(i)"");\
  asm("vpand xmm"tostr(j)", xmm"tostr(j)", xmm"tostr(k)"");\
  asm("vpxor xmm"tostr(i)", xmm"tostr(i)", xmm"tostr(j)"");\
}/**/

/* xmm[i] will be multiplied by 2
 * xmm[j] will be lost
 * xmm[k] has to be all 0x1b
 * xmm[z] has to be zero */
#define VMUL2v2(i, j, k, z){\
  asm("vpblendvb xmm"tostr(j)", xmm"tostr(z)", xmm"tostr(k)", xmm"tostr(i)"");\
  asm("vpaddb xmm"tostr(i)", xmm"tostr(i)", xmm"tostr(i)"");\
  asm("vpxor xmm"tostr(i)", xmm"tostr(i)", xmm"tostr(j)"");\
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
  asm("vmovdqa xmm"tostr(b0)", xmm"tostr(a2)"");\
  asm("vmovdqa xmm"tostr(b1)", xmm"tostr(a3)"");\
  asm("vmovdqa xmm"tostr(b2)", xmm"tostr(a4)"");\
  asm("vmovdqa xmm"tostr(b3)", xmm"tostr(a5)"");\
  asm("vmovdqa xmm"tostr(b4)", xmm"tostr(a6)"");\
  asm("vmovdqa xmm"tostr(b5)", xmm"tostr(a7)"");\
  asm("vmovdqa xmm"tostr(b6)", xmm"tostr(a0)"");\
  asm("vmovdqa xmm"tostr(b7)", xmm"tostr(a1)"");\
  \
  /* t_i = a_i + a_{i+1} */\
  asm("vpxor xmm"tostr(a0)", xmm"tostr(a0)", xmm"tostr(a1)"");\
  asm("vpxor xmm"tostr(a1)", xmm"tostr(a1)", xmm"tostr(a2)"");\
  asm("vpxor xmm"tostr(a2)", xmm"tostr(a2)", xmm"tostr(a3)"");\
  asm("vpxor xmm"tostr(a3)", xmm"tostr(a3)", xmm"tostr(a4)"");\
  asm("vpxor xmm"tostr(a4)", xmm"tostr(a4)", xmm"tostr(a5)"");\
  asm("vpxor xmm"tostr(a5)", xmm"tostr(a5)", xmm"tostr(a6)"");\
  asm("vpxor xmm"tostr(a6)", xmm"tostr(a6)", xmm"tostr(a7)"");\
  asm("vpxor xmm"tostr(a7)", xmm"tostr(a7)", xmm"tostr(b6)"");\
  \
  /* build y4 y5 y6 ... in regs xmm8, xmm9, xmm10 by adding t_i*/\
  asm("vpxor xmm"tostr(b0)", xmm"tostr(b0)", xmm"tostr(a4)"");\
  asm("vpxor xmm"tostr(b1)", xmm"tostr(b1)", xmm"tostr(a5)"");\
  asm("vpxor xmm"tostr(b2)", xmm"tostr(b2)", xmm"tostr(a6)"");\
  asm("vpxor xmm"tostr(b3)", xmm"tostr(b3)", xmm"tostr(a7)"");\
  asm("vpxor xmm"tostr(b4)", xmm"tostr(b4)", xmm"tostr(a0)"");\
  asm("vpxor xmm"tostr(b5)", xmm"tostr(b5)", xmm"tostr(a1)"");\
  asm("vpxor xmm"tostr(b6)", xmm"tostr(b6)", xmm"tostr(a2)"");\
  asm("vpxor xmm"tostr(b7)", xmm"tostr(b7)", xmm"tostr(a3)"");\
  \
  asm("vpxor xmm"tostr(b0)", xmm"tostr(b0)", xmm"tostr(a6)"");\
  asm("vpxor xmm"tostr(b1)", xmm"tostr(b1)", xmm"tostr(a7)"");\
  asm("vpxor xmm"tostr(b2)", xmm"tostr(b2)", xmm"tostr(a0)"");\
  asm("vpxor xmm"tostr(b3)", xmm"tostr(b3)", xmm"tostr(a1)"");\
  asm("vpxor xmm"tostr(b4)", xmm"tostr(b4)", xmm"tostr(a2)"");\
  asm("vpxor xmm"tostr(b5)", xmm"tostr(b5)", xmm"tostr(a3)"");\
  asm("vpxor xmm"tostr(b6)", xmm"tostr(b6)", xmm"tostr(a4)"");\
  asm("vpxor xmm"tostr(b7)", xmm"tostr(b7)", xmm"tostr(a5)"");\
  \
  /* spill values y_4, y_5 to memory */\
  asm("vmovaps [TEMP+0*16], xmm"tostr(b0)"");\
  asm("vmovaps [TEMP+1*16], xmm"tostr(b1)"");\
  asm("vmovaps [TEMP+2*16], xmm"tostr(b2)"");\
  \
  /* save values t0, t1, t2 to xmm8, xmm9 and memory */\
  asm("vmovdqa xmm"tostr(b0)", xmm"tostr(a0)"");\
  asm("vmovdqa xmm"tostr(b1)", xmm"tostr(a1)"");\
  asm("vmovaps [TEMP+3*16], xmm"tostr(a2)"");\
  \
  /* compute x_i = t_i + t_{i+3} */\
  asm("vpxor xmm"tostr(a0)", xmm"tostr(a0)", xmm"tostr(a3)"");\
  asm("vpxor xmm"tostr(a1)", xmm"tostr(a1)", xmm"tostr(a4)"");\
  asm("vpxor xmm"tostr(a2)", xmm"tostr(a2)", xmm"tostr(a5)"");\
  asm("vpxor xmm"tostr(a3)", xmm"tostr(a3)", xmm"tostr(a6)"");\
  asm("vpxor xmm"tostr(a4)", xmm"tostr(a4)", xmm"tostr(a7)"");\
  asm("vpxor xmm"tostr(a5)", xmm"tostr(a5)", xmm"tostr(b0)"");\
  asm("vpxor xmm"tostr(a6)", xmm"tostr(a6)", xmm"tostr(b1)"");\
  asm("vpxor xmm"tostr(a7)", xmm"tostr(a7)", [TEMP+3*16]");\
  \
  /*compute z_i : double x_i using temp xmm8 and 1B xmm9 */\
  asm("vmovaps xmm"tostr(b1)", [ALL_1B]");\
  asm("vpxor xmm"tostr(b2)", xmm"tostr(b2)", xmm"tostr(b2)"");\
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
  asm("vpxor xmm"tostr(a0)", xmm"tostr(a0)", [TEMP+0*16]");\
  asm("vpxor xmm"tostr(a1)", xmm"tostr(a1)", [TEMP+1*16]");\
  asm("vpxor xmm"tostr(a2)", xmm"tostr(a2)", [TEMP+2*16]");\
  asm("vpxor xmm"tostr(a3)", xmm"tostr(a3)", xmm"tostr(b3)"");\
  asm("vpxor xmm"tostr(a4)", xmm"tostr(a4)", xmm"tostr(b4)"");\
  asm("vpxor xmm"tostr(a5)", xmm"tostr(a5)", xmm"tostr(b5)"");\
  asm("vpxor xmm"tostr(a6)", xmm"tostr(a6)", xmm"tostr(b6)"");\
  asm("vpxor xmm"tostr(a7)", xmm"tostr(a7)", xmm"tostr(b7)"");\
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
  asm("vpxor xmm"tostr(b0)", xmm"tostr(a3)", [TEMP+0*16]");\
  asm("vpxor xmm"tostr(b1)", xmm"tostr(a4)", [TEMP+1*16]");\
  asm("vpxor xmm"tostr(b2)", xmm"tostr(a5)", [TEMP+2*16]");\
  asm("vpxor xmm"tostr(b3)", xmm"tostr(b3)", xmm"tostr(a6)"");\
  asm("vpxor xmm"tostr(b4)", xmm"tostr(b4)", xmm"tostr(a7)"");\
  asm("vpxor xmm"tostr(b5)", xmm"tostr(b5)", xmm"tostr(a0)"");\
  asm("vpxor xmm"tostr(b6)", xmm"tostr(b6)", xmm"tostr(a1)"");\
  asm("vpxor xmm"tostr(b7)", xmm"tostr(b7)", xmm"tostr(a2)"");\
}/*MixBytes*/

/* one round
 * i = round number
 * a0-a7 = input rows
 * b0-b7 = output rows
 */
#define ROUND(i, a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7){\
  /* AddRoundConstant */\
  asm ("vmovaps xmm"tostr(b1)", [ROUND_CONST_Lx]");\
  asm ("vpxor   xmm"tostr(a0)", xmm"tostr(a0)", [ROUND_CONST_L0+"tostr(i)"*16]");\
  asm ("vpxor   xmm"tostr(a1)", xmm"tostr(a1)", xmm"tostr(b1)"");\
  asm ("vpxor   xmm"tostr(a2)", xmm"tostr(a2)", xmm"tostr(b1)"");\
  asm ("vpxor   xmm"tostr(a3)", xmm"tostr(a3)", xmm"tostr(b1)"");\
  asm ("vpxor   xmm"tostr(a4)", xmm"tostr(a4)", xmm"tostr(b1)"");\
  asm ("vpxor   xmm"tostr(a5)", xmm"tostr(a5)", xmm"tostr(b1)"");\
  asm ("vpxor   xmm"tostr(a6)", xmm"tostr(a6)", xmm"tostr(b1)"");\
  asm ("vpxor   xmm"tostr(a7)", xmm"tostr(a7)", [ROUND_CONST_L7+"tostr(i)"*16]");\
  /* ShiftBytes + SubBytes (interleaved) */\
  asm ("vpxor xmm"tostr(b0)",  xmm"tostr(b0)",  xmm"tostr(b0)"");\
  asm ("vpshufb     xmm"tostr(a0)", xmm"tostr(a0)", [SUBSH_MASK+0*16]");\
  asm ("vaesenclast xmm"tostr(a0)", xmm"tostr(a0)", xmm"tostr(b0)"");\
  asm ("vpshufb     xmm"tostr(a1)", xmm"tostr(a1)", [SUBSH_MASK+1*16]");\
  asm ("vaesenclast xmm"tostr(a1)", xmm"tostr(a1)", xmm"tostr(b0)"");\
  asm ("vpshufb     xmm"tostr(a2)", xmm"tostr(a2)", [SUBSH_MASK+2*16]");\
  asm ("vaesenclast xmm"tostr(a2)", xmm"tostr(a2)", xmm"tostr(b0)"");\
  asm ("vpshufb     xmm"tostr(a3)", xmm"tostr(a3)", [SUBSH_MASK+3*16]");\
  asm ("vaesenclast xmm"tostr(a3)", xmm"tostr(a3)", xmm"tostr(b0)"");\
  asm ("vpshufb     xmm"tostr(a4)", xmm"tostr(a4)", [SUBSH_MASK+4*16]");\
  asm ("vaesenclast xmm"tostr(a4)", xmm"tostr(a4)", xmm"tostr(b0)"");\
  asm ("vpshufb     xmm"tostr(a5)", xmm"tostr(a5)", [SUBSH_MASK+5*16]");\
  asm ("vaesenclast xmm"tostr(a5)", xmm"tostr(a5)", xmm"tostr(b0)"");\
  asm ("vpshufb     xmm"tostr(a6)", xmm"tostr(a6)", [SUBSH_MASK+6*16]");\
  asm ("vaesenclast xmm"tostr(a6)", xmm"tostr(a6)", xmm"tostr(b0)"");\
  asm ("vpshufb     xmm"tostr(a7)", xmm"tostr(a7)", [SUBSH_MASK+7*16]");\
  asm ("vaesenclast xmm"tostr(a7)", xmm"tostr(a7)", xmm"tostr(b0)"");\
  /* MixBytes */\
  MixBytes(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7);\
}

/* 10 rounds, P and Q in parallel */
#define ROUNDS_P_Q(){\
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
}

/* Matrix Transpose Step 1
 * input is a 512-bit state with two columns in one xmm
 * output is a 512-bit state with two rows in one xmm
 * inputs: i0-i3

 * outputs: i0, o1-o3
 * clobbers: t0
 */
#define Matrix_Transpose_A(i0, i1, i2, i3, o1, o2, o3, t0){\
  asm ("vmovaps xmm"tostr(t0)", [TRANSP_MASK]");\
\
  asm ("vpshufb xmm"tostr(i0)", xmm"tostr(i0)", xmm"tostr(t0)"");\
  asm ("vpshufb xmm"tostr(i1)", xmm"tostr(i1)", xmm"tostr(t0)"");\
  asm ("vpshufb xmm"tostr(i2)", xmm"tostr(i2)", xmm"tostr(t0)"");\
  asm ("vpshufb xmm"tostr(i3)", xmm"tostr(i3)", xmm"tostr(t0)"");\
\
  asm ("vpunpckhwd xmm"tostr(o1)", xmm"tostr(i0)", xmm"tostr(i1)"");\
  asm ("vpunpcklwd xmm"tostr(i0)", xmm"tostr(i0)", xmm"tostr(i1)"");\
  asm ("vpunpckhwd xmm"tostr(t0)", xmm"tostr(i2)", xmm"tostr(i3)"");\
  asm ("vpunpcklwd xmm"tostr(i2)", xmm"tostr(i2)", xmm"tostr(i3)"");\
\
  asm ("vpshufd xmm"tostr(i0)", xmm"tostr(i0)", 216");\
  asm ("vpshufd xmm"tostr(o1)", xmm"tostr(o1)", 216");\
  asm ("vpshufd xmm"tostr(i2)", xmm"tostr(i2)", 216");\
  asm ("vpshufd xmm"tostr(t0)", xmm"tostr(t0)", 216");\
\
  asm ("vpunpckhdq xmm"tostr(o2)", xmm"tostr(i0)", xmm"tostr(i2)"");\
  asm ("vpunpckhdq xmm"tostr(o3)", xmm"tostr(o1)", xmm"tostr(t0)"");\
  asm ("vpunpckldq xmm"tostr(i0)", xmm"tostr(i0)", xmm"tostr(i2)"");\
  asm ("vpunpckldq xmm"tostr(o1)", xmm"tostr(o1)", xmm"tostr(t0)"");\
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
  asm ("vpunpckhqdq xmm"tostr(o1)", xmm"tostr(i0)", xmm"tostr(i4)"");\
  asm ("vpunpcklqdq xmm"tostr(i0)", xmm"tostr(i0)", xmm"tostr(i4)"");\
  asm ("vpunpcklqdq xmm"tostr(o2)", xmm"tostr(i1)", xmm"tostr(i5)"");\
  asm ("vpunpckhqdq xmm"tostr(o3)", xmm"tostr(i1)", xmm"tostr(i5)"");\
  asm ("vpunpcklqdq xmm"tostr(o4)", xmm"tostr(i2)", xmm"tostr(i6)"");\
  asm ("vpunpckhqdq xmm"tostr(o5)", xmm"tostr(i2)", xmm"tostr(i6)"");\
  asm ("vpunpcklqdq xmm"tostr(o6)", xmm"tostr(i3)", xmm"tostr(i7)"");\
  asm ("vpunpckhqdq xmm"tostr(o7)", xmm"tostr(i3)", xmm"tostr(i7)"");\
}/**/

/* Matrix Transpose Inverse Step 2
 * input are two 512-bit states with one row of each state in one xmm
 * output are two 512-bit states with two rows in one xmm
 * inputs: i0-i7 = (P|Q)
 * outputs: (i0, i2, i4, i6) = P, (o0-o3) = Q
 */
#define Matrix_Transpose_B_INV(i0, i1, i2, i3, i4, i5, i6, i7, o0, o1, o2, o3){\
  asm ("vpunpckhqdq xmm"tostr(o0)", xmm"tostr(i0)", xmm"tostr(i1)"");\
  asm ("vpunpcklqdq xmm"tostr(i0)", xmm"tostr(i0)", xmm"tostr(i1)"");\
  asm ("vpunpckhqdq xmm"tostr(o1)", xmm"tostr(i2)", xmm"tostr(i3)"");\
  asm ("vpunpcklqdq xmm"tostr(i2)", xmm"tostr(i2)", xmm"tostr(i3)"");\
  asm ("vpunpckhqdq xmm"tostr(o2)", xmm"tostr(i4)", xmm"tostr(i5)"");\
  asm ("vpunpcklqdq xmm"tostr(i4)", xmm"tostr(i4)", xmm"tostr(i5)"");\
  asm ("vpunpckhqdq xmm"tostr(o3)", xmm"tostr(i6)", xmm"tostr(i7)"");\
  asm ("vpunpcklqdq xmm"tostr(i6)", xmm"tostr(i6)", xmm"tostr(i7)"");\
}/**/

/* Matrix Transpose Output Step 2
 * input is one 512-bit state with two rows in one xmm
 * output is one 512-bit state with one row in the low 64-bits of one xmm
 * inputs: i0,i2,i4,i6 = S
 * outputs: (i0-7) = (0|S)
 */
#define Matrix_Transpose_O_B(i0, i1, i2, i3, i4, i5, i6, i7, t0){\
  asm ("vpxor xmm"tostr(t0)", xmm"tostr(t0)", xmm"tostr(t0)"");\
  asm ("vpunpckhqdq xmm"tostr(i1)", xmm"tostr(i0)", xmm"tostr(t0)"");\
  asm ("vpunpcklqdq xmm"tostr(i0)", xmm"tostr(i0)", xmm"tostr(t0)"");\
  asm ("vpunpckhqdq xmm"tostr(i3)", xmm"tostr(i2)", xmm"tostr(t0)"");\
  asm ("vpunpcklqdq xmm"tostr(i2)", xmm"tostr(i2)", xmm"tostr(t0)"");\
  asm ("vpunpckhqdq xmm"tostr(i5)", xmm"tostr(i4)", xmm"tostr(t0)"");\
  asm ("vpunpcklqdq xmm"tostr(i4)", xmm"tostr(i4)", xmm"tostr(t0)"");\
  asm ("vpunpckhqdq xmm"tostr(i7)", xmm"tostr(i6)", xmm"tostr(t0)"");\
  asm ("vpunpcklqdq xmm"tostr(i6)", xmm"tostr(i6)", xmm"tostr(t0)"");\
}/**/

/* Matrix Transpose Output Inverse Step 2
 * input is one 512-bit state with one row in the low 64-bits of one xmm
 * output is one 512-bit state with two rows in one xmm
 * inputs: i0-i7 = (0|S)
 * outputs: (i0, i2, i4, i6) = S
 */
#define Matrix_Transpose_O_B_INV(i0, i1, i2, i3, i4, i5, i6, i7){\
  asm ("vpunpcklqdq xmm"tostr(i0)", xmm"tostr(i0)", xmm"tostr(i1)"");\
  asm ("vpunpcklqdq xmm"tostr(i2)", xmm"tostr(i2)", xmm"tostr(i3)"");\
  asm ("vpunpcklqdq xmm"tostr(i4)", xmm"tostr(i4)", xmm"tostr(i5)"");\
  asm ("vpunpcklqdq xmm"tostr(i6)", xmm"tostr(i6)", xmm"tostr(i7)"");\
}/**/


void INIT(u64* h)
{
  /* __cdecl calling convention: */
  /* chaining value CV in rdi    */

  asm (".intel_syntax noprefix");
  asm volatile ("emms");

  /* load IV into registers xmm12 - xmm15 */
  asm ("vmovaps xmm12, [rdi+0*16]");
  asm ("vmovaps xmm13, [rdi+1*16]");
  asm ("vmovaps xmm14, [rdi+2*16]");
  asm ("vmovaps xmm15, [rdi+3*16]");

  /* transform chaining value from column ordering into row ordering */
  /* we put two rows (64 bit) of the IV into one 128-bit XMM register */
  Matrix_Transpose_A(12, 13, 14, 15, 2, 6, 7, 0);

  /* store transposed IV */
  asm ("vmovaps [rdi+0*16], xmm12");
  asm ("vmovaps [rdi+1*16], xmm2");
  asm ("vmovaps [rdi+2*16], xmm6");
  asm ("vmovaps [rdi+3*16], xmm7");

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
  asm ("vmovaps xmm12, [rsi+0*16]");
  asm ("vmovaps xmm13, [rsi+1*16]");
  asm ("vmovaps xmm14, [rsi+2*16]");
  asm ("vmovaps xmm15, [rsi+3*16]");

  /* transform message M from column ordering into row ordering */
  /* we first put two rows (64 bit) of the message into one 128-bit xmm register */
  Matrix_Transpose_A(12, 13, 14, 15, 2, 6, 7, 0);

  /* load previous chaining value and xor message to CV to get input of P */
  /* we first put two rows (2x64 bit) of the CV into one 128-bit xmm register */
  /* result: CV+M in xmm8, xmm0, xmm4, xmm5 */
  asm ("vpxor xmm8, xmm12, [rdi+0*16]");
  asm ("vpxor xmm0, xmm2,  [rdi+1*16]");
  asm ("vpxor xmm4, xmm6,  [rdi+2*16]");
  asm ("vpxor xmm5, xmm7,  [rdi+3*16]");

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
  asm ("vpxor xmm0, xmm0, xmm8");
  asm ("vpxor xmm1, xmm1, xmm10");
  asm ("vpxor xmm2, xmm2, xmm12");
  asm ("vpxor xmm3, xmm3, xmm14");

  /* xor CV (feed-forward) */
  /* result: P(CV+M)+Q(M)+CV in xmm0...xmm3 */
  asm ("vpxor xmm0, xmm0, [rdi+0*16]");
  asm ("vpxor xmm1, xmm1, [rdi+1*16]");
  asm ("vpxor xmm2, xmm2, [rdi+2*16]");
  asm ("vpxor xmm3, xmm3, [rdi+3*16]");

  /* store CV */
  asm ("vmovaps [rdi+0*16], xmm0");
  asm ("vmovaps [rdi+1*16], xmm1");
  asm ("vmovaps [rdi+2*16], xmm2");
  asm ("vmovaps [rdi+3*16], xmm3");

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
  asm ("vmovaps xmm8,  [rdi+0*16]");
  asm ("vmovaps xmm10, [rdi+1*16]");
  asm ("vmovaps xmm12, [rdi+2*16]");
  asm ("vmovaps xmm14, [rdi+3*16]");

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
  asm ("vpxor xmm8,  xmm8,  [rdi+0*16]");
  asm ("vpxor xmm10, xmm10, [rdi+1*16]");
  asm ("vpxor xmm12, xmm12, [rdi+2*16]");
  asm ("vpxor xmm14, xmm14, [rdi+3*16]");

  /* transform state back from row ordering into column ordering */
  /* result: final hash value in xmm9, xmm11 */
  Matrix_Transpose_A(8, 10, 12, 14, 4, 9, 11, 0);

  /* we only need to return the truncated half of the state */
  asm ("vmovaps [rdi+2*16], xmm9");
  asm ("vmovaps [rdi+3*16], xmm11");

  Pop_All_Regs();
  asm (".att_syntax noprefix");

  return;
}

#endif

#if (LENGTH > 256)

#define SET_CONSTANTS(){\
  ((u64*)TRANSP_MASK)[0] = 0x0d0509010c040800ULL;\
  ((u64*)TRANSP_MASK)[1] = 0x0f070b030e060a02ULL;\
  ((u64*)ALL_FF)[0] = 0xffffffffffffffffULL;\
  ((u64*)ALL_FF)[1] = 0xffffffffffffffffULL;\
  ((u64*)ALL_FF)[2] = 0x0000000000000000ULL;\
  ((u64*)ALL_FF)[3] = 0x0000000000000000ULL;\
  ((u64*)ALL_1B)[0] = 0x1b1b1b1b1b1b1b1bULL;\
  ((u64*)ALL_1B)[1] = 0x1b1b1b1b1b1b1b1bULL;\
  ((u64*)ALL_1B)[2] = 0x1b1b1b1b1b1b1b1bULL;\
  ((u64*)ALL_1B)[3] = 0x1b1b1b1b1b1b1b1bULL;\
  ((u64*)SUBSH_MASK)[ 0] = 0x0b0e0104070a0d00ULL;\
  ((u64*)SUBSH_MASK)[ 1] = 0x0306090c0f020508ULL;\
  ((u64*)SUBSH_MASK)[ 2] = 0x0c0f0205080b0e01ULL;\
  ((u64*)SUBSH_MASK)[ 3] = 0x04070a0d00030609ULL;\
  ((u64*)SUBSH_MASK)[ 4] = 0x0d000306090c0f02ULL;\
  ((u64*)SUBSH_MASK)[ 5] = 0x05080b0e0104070aULL;\
  ((u64*)SUBSH_MASK)[ 6] = 0x0e0104070a0d0003ULL;\
  ((u64*)SUBSH_MASK)[ 7] = 0x06090c0f0205080bULL;\
  ((u64*)SUBSH_MASK)[ 8] = 0x0f0205080b0e0104ULL;\
  ((u64*)SUBSH_MASK)[ 9] = 0x070a0d000306090cULL;\
  ((u64*)SUBSH_MASK)[10] = 0x000306090c0f0205ULL;\
  ((u64*)SUBSH_MASK)[11] = 0x080b0e0104070a0dULL;\
  ((u64*)SUBSH_MASK)[12] = 0x0104070a0d000306ULL;\
  ((u64*)SUBSH_MASK)[13] = 0x090c0f0205080b0eULL;\
  ((u64*)SUBSH_MASK)[14] = 0x06090c0f0205080bULL;\
  ((u64*)SUBSH_MASK)[15] = 0x0e0104070a0d0003ULL;\
  for(i = 0; i < ROUNDS1024; i++)\
  {\
    ((u64*)ROUND_CONST_P)[2*i+1] = (i * 0x0101010101010101ULL) ^ 0xf0e0d0c0b0a09080ULL;\
    ((u64*)ROUND_CONST_P)[2*i+0] = (i * 0x0101010101010101ULL) ^ 0x7060504030201000ULL;\
    ((u64*)ROUND_CONST_Q)[2*i+1] = (i * 0x0101010101010101ULL) ^ 0x0f1f2f3f4f5f6f7fULL;\
    ((u64*)ROUND_CONST_Q)[2*i+0] = (i * 0x0101010101010101ULL) ^ 0x8f9fafbfcfdfefffULL;\
  }\
}while(0);

#define Push_All_Regs() do{\
    asm("push rax");\
    asm("push rbx");\
    asm("push rcx");\
}while(0);

#define Pop_All_Regs() do{\
    asm("pop rcx");\
    asm("pop rbx");\
    asm("pop rax");\
}while(0);

/* AVX MUL2
 * ymm[i] will be multiplied by 2
 * ymm[j] will be lost
 * ymm[k] has to be all 0x1b
 * ymm[z] has to be zero
 * clobbers: t2, t3 */
#define VMUL2(i, j, k, z, ih, jh){\
    asm("vextractf128 xmm"tostr(ih)", ymm"tostr(i)", 1");\
    asm("vpcmpgtb xmm"tostr(j)", xmm"tostr(z)", xmm"tostr(i)"");\
    asm("vpcmpgtb xmm"tostr(jh)", xmm"tostr(z)", xmm"tostr(ih)"");\
    asm("vpaddb xmm"tostr(i)",  xmm"tostr(i)",  xmm"tostr(i)"");\
    asm("vpaddb xmm"tostr(ih)", xmm"tostr(ih)", xmm"tostr(ih)"");\
    asm("vinsertf128 ymm"tostr(j)", ymm"tostr(j)", xmm"tostr(jh)", 1");\
    asm("vinsertf128 ymm"tostr(i)",  ymm"tostr(i)",  xmm"tostr(ih)", 1");\
    asm("vandpd ymm"tostr(j)", ymm"tostr(j)", ymm"tostr(k)"");\
    asm("vxorpd ymm"tostr(i)",  ymm"tostr(i)",  ymm"tostr(j)"");\
}/**/

/* xmm[i] will be multiplied by 2
 * xmm[j] will be lost
 * xmm[k] has to be all 0x1b
 * xmm[z] has to be zero */
#define VMUL2v2(i, j, k, z){\
  asm("vpcmpgtb xmm"tostr(j)", xmm"tostr(z)", xmm"tostr(i)"");\
  asm("vpaddb xmm"tostr(i)", xmm"tostr(i)", xmm"tostr(i)"");\
  asm("vpand xmm"tostr(j)", xmm"tostr(j)", xmm"tostr(k)"");\
  asm("vpxor xmm"tostr(i)", xmm"tostr(i)", xmm"tostr(j)"");\
}/**/

/* xmm[i] will be multiplied by 2
 * xmm[j] will be lost
 * xmm[k] has to be all 0x1b
 * xmm[z] has to be zero */
#define VMUL2v3(i, j, k, z){\
  asm("vpblendvb xmm"tostr(j)", xmm"tostr(z)", xmm"tostr(k)", xmm"tostr(i)"");\
  asm("vpaddb xmm"tostr(i)", xmm"tostr(i)", xmm"tostr(i)"");\
  asm("vpxor xmm"tostr(i)", xmm"tostr(i)", xmm"tostr(j)"");\
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
  /* ymm"tostr(8..ymm"tostr(15 = a2 a3... a0 a1 */\
  asm("vmovdqa ymm"tostr(b0)", ymm"tostr(a2)"");\
  asm("vmovdqa ymm"tostr(b1)", ymm"tostr(a3)"");\
  asm("vmovdqa ymm"tostr(b2)", ymm"tostr(a4)"");\
  asm("vmovdqa ymm"tostr(b3)", ymm"tostr(a5)"");\
  asm("vmovdqa ymm"tostr(b4)", ymm"tostr(a6)"");\
  asm("vmovdqa ymm"tostr(b5)", ymm"tostr(a7)"");\
  asm("vmovdqa ymm"tostr(b6)", ymm"tostr(a0)"");\
  asm("vmovdqa ymm"tostr(b7)", ymm"tostr(a1)"");\
  \
  /* t_i = a_i + a_{i+1} */\
  asm("vxorpd ymm"tostr(a0)", ymm"tostr(a0)", ymm"tostr(a1)"");\
  asm("vxorpd ymm"tostr(a1)", ymm"tostr(a1)", ymm"tostr(a2)"");\
  asm("vxorpd ymm"tostr(a2)", ymm"tostr(a2)", ymm"tostr(a3)"");\
  asm("vxorpd ymm"tostr(a3)", ymm"tostr(a3)", ymm"tostr(a4)"");\
  asm("vxorpd ymm"tostr(a4)", ymm"tostr(a4)", ymm"tostr(a5)"");\
  asm("vxorpd ymm"tostr(a5)", ymm"tostr(a5)", ymm"tostr(a6)"");\
  asm("vxorpd ymm"tostr(a6)", ymm"tostr(a6)", ymm"tostr(a7)"");\
  asm("vxorpd ymm"tostr(a7)", ymm"tostr(a7)", ymm"tostr(b6)"");\
  \
  /* build y4 y5 y6 ... in regs ymm8, ymm9, ymm10 by adding t_i*/\
  asm("vxorpd ymm"tostr(b0)", ymm"tostr(b0)", ymm"tostr(a4)"");\
  asm("vxorpd ymm"tostr(b1)", ymm"tostr(b1)", ymm"tostr(a5)"");\
  asm("vxorpd ymm"tostr(b2)", ymm"tostr(b2)", ymm"tostr(a6)"");\
  asm("vxorpd ymm"tostr(b3)", ymm"tostr(b3)", ymm"tostr(a7)"");\
  asm("vxorpd ymm"tostr(b4)", ymm"tostr(b4)", ymm"tostr(a0)"");\
  asm("vxorpd ymm"tostr(b5)", ymm"tostr(b5)", ymm"tostr(a1)"");\
  asm("vxorpd ymm"tostr(b6)", ymm"tostr(b6)", ymm"tostr(a2)"");\
  asm("vxorpd ymm"tostr(b7)", ymm"tostr(b7)", ymm"tostr(a3)"");\
  \
  asm("vxorpd ymm"tostr(b0)", ymm"tostr(b0)", ymm"tostr(a6)"");\
  asm("vxorpd ymm"tostr(b1)", ymm"tostr(b1)", ymm"tostr(a7)"");\
  asm("vxorpd ymm"tostr(b2)", ymm"tostr(b2)", ymm"tostr(a0)"");\
  asm("vxorpd ymm"tostr(b3)", ymm"tostr(b3)", ymm"tostr(a1)"");\
  asm("vxorpd ymm"tostr(b4)", ymm"tostr(b4)", ymm"tostr(a2)"");\
  asm("vxorpd ymm"tostr(b5)", ymm"tostr(b5)", ymm"tostr(a3)"");\
  asm("vxorpd ymm"tostr(b6)", ymm"tostr(b6)", ymm"tostr(a4)"");\
  asm("vxorpd ymm"tostr(b7)", ymm"tostr(b7)", ymm"tostr(a5)"");\
  \
  /* spill values y_4, y_5 to memory */\
  asm("vmovaps [TEMP+0*32], ymm"tostr(b0)"");\
  asm("vmovaps [TEMP+1*32], ymm"tostr(b1)"");\
  asm("vmovaps [TEMP+2*32], ymm"tostr(b2)"");\
  asm("vmovaps [TEMP+3*32], ymm"tostr(b3)"");\
  asm("vmovaps [TEMP+4*32], ymm"tostr(b4)"");\
  \
  /* save values t0, t1, t2 to ymm8, ymm9 and memory */\
  asm("vmovdqa ymm"tostr(b0)", ymm"tostr(a0)"");\
  asm("vmovdqa ymm"tostr(b1)", ymm"tostr(a1)"");\
  asm("vmovaps [TEMP+5*32], ymm"tostr(a2)"");\
  \
  /* compute x_i = t_i + t_{i+3} */\
  asm("vxorpd ymm"tostr(a0)", ymm"tostr(a0)", ymm"tostr(a3)"");\
  asm("vxorpd ymm"tostr(a1)", ymm"tostr(a1)", ymm"tostr(a4)"");\
  asm("vxorpd ymm"tostr(a2)", ymm"tostr(a2)", ymm"tostr(a5)"");\
  asm("vxorpd ymm"tostr(a3)", ymm"tostr(a3)", ymm"tostr(a6)"");\
  asm("vxorpd ymm"tostr(a4)", ymm"tostr(a4)", ymm"tostr(a7)"");\
  asm("vxorpd ymm"tostr(a5)", ymm"tostr(a5)", ymm"tostr(b0)"");\
  asm("vxorpd ymm"tostr(a6)", ymm"tostr(a6)", ymm"tostr(b1)"");\
  asm("vxorpd ymm"tostr(a7)", ymm"tostr(a7)", [TEMP+5*32]");\
  \
  /*compute z_i : double x_i using temp ymm8 and 1B ymm9 */\
  asm("vmovaps ymm"tostr(b1)", [ALL_1B]");\
  asm("vxorpd ymm"tostr(b2)", ymm"tostr(b2)", ymm"tostr(b2)"");\
  VMUL2(a7, b0, b1, b2, b3, b4);\
  VMUL2(a6, b0, b1, b2, b3, b4);\
  VMUL2(a5, b0, b1, b2, b3, b4);\
  VMUL2(a4, b0, b1, b2, b3, b4);\
  VMUL2(a3, b0, b1, b2, b3, b4);\
  VMUL2(a2, b0, b1, b2, b3, b4);\
  VMUL2(a1, b0, b1, b2, b3, b4);\
  VMUL2(a0, b0, b1, b2, b3, b4);\
  \
  /* compute w_i :  add y_{i+4} */\
  asm("vxorpd ymm"tostr(a0)", ymm"tostr(a0)", [TEMP+0*32]");\
  asm("vxorpd ymm"tostr(a1)", ymm"tostr(a1)", [TEMP+1*32]");\
  asm("vxorpd ymm"tostr(a2)", ymm"tostr(a2)", [TEMP+2*32]");\
  asm("vxorpd ymm"tostr(a3)", ymm"tostr(a3)", [TEMP+3*32]");\
  asm("vxorpd ymm"tostr(a4)", ymm"tostr(a4)", [TEMP+4*32]");\
  asm("vxorpd ymm"tostr(a5)", ymm"tostr(a5)", ymm"tostr(b5)"");\
  asm("vxorpd ymm"tostr(a6)", ymm"tostr(a6)", ymm"tostr(b6)"");\
  asm("vxorpd ymm"tostr(a7)", ymm"tostr(a7)", ymm"tostr(b7)"");\
  \
  /*compute v_i: double w_i */\
  VMUL2(a0, b0, b1, b2, b3, b4);\
  VMUL2(a1, b0, b1, b2, b3, b4);\
  VMUL2(a2, b0, b1, b2, b3, b4);\
  VMUL2(a3, b0, b1, b2, b3, b4);\
  VMUL2(a4, b0, b1, b2, b3, b4);\
  VMUL2(a5, b0, b1, b2, b3, b4);\
  VMUL2(a6, b0, b1, b2, b3, b4);\
  VMUL2(a7, b0, b1, b2, b3, b4);\
  \
  /* add to y_4 y_5 .. v3, v4, ... */\
  asm("vxorpd ymm"tostr(b0)", ymm"tostr(a3)", [TEMP+0*32]");\
  asm("vxorpd ymm"tostr(b1)", ymm"tostr(a4)", [TEMP+1*32]");\
  asm("vxorpd ymm"tostr(b2)", ymm"tostr(a5)", [TEMP+2*32]");\
  asm("vxorpd ymm"tostr(b3)", ymm"tostr(a6)", [TEMP+3*32]");\
  asm("vxorpd ymm"tostr(b4)", ymm"tostr(a7)", [TEMP+4*32]");\
  asm("vxorpd ymm"tostr(b5)", ymm"tostr(a0)", ymm"tostr(b5)"");\
  asm("vxorpd ymm"tostr(b6)", ymm"tostr(a1)", ymm"tostr(b6)"");\
  asm("vxorpd ymm"tostr(b7)", ymm"tostr(a2)", ymm"tostr(b7)"");\
}/*MixBytes*/

/* AVX SubShift
 * inputs:
 * * i
 * * c0 (must be 0)
 * * ShiftP
 * * ShiftQ
 * output i = S[Shift(i_1, ShiftQ)|Shift(i_0, ShiftP)]
 * clobbers: t0
 * */
#define SubShift(i, t0, c0, ShiftP, ShiftQ){\
    asm("vextractf128 xmm"tostr(t0)", ymm"tostr(i)", 1");\
    asm("vpshufb xmm"tostr(i)",  xmm"tostr(i)",  [SUBSH_MASK+"tostr(ShiftP)"*16]");\
    asm("vpshufb xmm"tostr(t0)", xmm"tostr(t0)", [SUBSH_MASK+"tostr(ShiftQ)"*16]");\
    asm("vaesenclast xmm"tostr(i)",  xmm"tostr(i)",  xmm"tostr(c0)"");\
    asm("vaesenclast xmm"tostr(t0)", xmm"tostr(t0)", xmm"tostr(c0)"");\
    asm("vinsertf128 ymm"tostr(i)",  ymm"tostr(i)",  xmm"tostr(t0)", 1");\
}/**/

/* one round
 * a0-a7 = input rows
 * b0-b7 = output rows
 */
#define SUBSHIFTMIX(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7){\
    /* ShiftBytes + SubBytes */\
    asm ("vpxor xmm"tostr(b0)", xmm"tostr(b0)", xmm"tostr(b0)"");\
    SubShift(a0, b1, b0, 0, 1);\
    SubShift(a1, b1, b0, 1, 3);\
    SubShift(a2, b1, b0, 2, 5);\
    SubShift(a3, b1, b0, 3, 7);\
    SubShift(a4, b1, b0, 4, 0);\
    SubShift(a5, b1, b0, 5, 2);\
    SubShift(a6, b1, b0, 6, 4);\
    SubShift(a7, b1, b0, 7, 6);\
    /* MixBytes */\
    MixBytes(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7);\
}

#define ROUNDS_P_Q(){\
  asm ("xor rax, rax");\
  asm ("1:");\
  /* AddRoundConstant */\
  asm ("vxorpd ymm6, ymm6, ymm6");\
  asm ("vinsertf128 ymm7, ymm6, [ROUND_CONST_Q+eax*8], 1");\
  asm ("vinsertf128 ymm6, ymm6, [ALL_FF], 1");\
  asm ("vinsertf128 ymm0, ymm6, [ROUND_CONST_P+eax*8], 0");\
  asm ("vxorpd ymm0, ymm8,  ymm0");\
  asm ("vxorpd ymm1, ymm9,  ymm6");\
  asm ("vxorpd ymm2, ymm10, ymm6");\
  asm ("vxorpd ymm3, ymm11, ymm6");\
  asm ("vxorpd ymm4, ymm12, ymm6");\
  asm ("vxorpd ymm5, ymm13, ymm6");\
  asm ("vxorpd ymm6, ymm14, ymm6");\
  asm ("vxorpd ymm7, ymm15, ymm7");\
  /* SubBytes + ShiftBytes + MixBytes */\
  SUBSHIFTMIX(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);\
  asm ("add al, 2");\
  asm ("mov rbx, rax");\
  asm ("sub bl, 28");\
  asm ("jb 1b");\
}

/* Matrix Transpose
 * input is a 1024-bit state with two columns in one xmm
 * output is a 1024-bit state with two rows in one xmm
 * inputs: i0-i7
 * outputs: i0-i7
 * clobbers: t0-t7
 */
#define Matrix_Transpose(i0, i1, i2, i3, i4, i5, i6, i7, t0, t1, t2, t3, t4, t5, t6, t7){\
  asm ("vmovaps xmm"tostr(t0)", [TRANSP_MASK]");\
\
  asm ("vpshufb xmm"tostr(i6)", xmm"tostr(i6)", xmm"tostr(t0)"");\
  asm ("vpshufb xmm"tostr(i0)", xmm"tostr(i0)", xmm"tostr(t0)"");\
  asm ("vpshufb xmm"tostr(i1)", xmm"tostr(i1)", xmm"tostr(t0)"");\
  asm ("vpshufb xmm"tostr(i2)", xmm"tostr(i2)", xmm"tostr(t0)"");\
  asm ("vpshufb xmm"tostr(i3)", xmm"tostr(i3)", xmm"tostr(t0)"");\
  asm ("vpshufb xmm"tostr(i4)", xmm"tostr(i4)", xmm"tostr(t0)"");\
  asm ("vpshufb xmm"tostr(i5)", xmm"tostr(i5)", xmm"tostr(t0)"");\
  asm ("vpshufb xmm"tostr(i7)", xmm"tostr(i7)", xmm"tostr(t0)"");\
\
  /* continue with unpack */\
  asm ("vpunpckhwd xmm"tostr(t0)", xmm"tostr(i0)", xmm"tostr(i1)"");\
  asm ("vpunpckhwd xmm"tostr(t1)", xmm"tostr(i2)", xmm"tostr(i3)"");\
  asm ("vpunpckhwd xmm"tostr(t2)", xmm"tostr(i4)", xmm"tostr(i5)"");\
  asm ("vpunpckhwd xmm"tostr(t3)", xmm"tostr(i6)", xmm"tostr(i7)"");\
  asm ("vpunpcklwd xmm"tostr(i0)", xmm"tostr(i0)", xmm"tostr(i1)"");\
  asm ("vpunpcklwd xmm"tostr(i2)", xmm"tostr(i2)", xmm"tostr(i3)"");\
  asm ("vpunpcklwd xmm"tostr(i4)", xmm"tostr(i4)", xmm"tostr(i5)"");\
  asm ("vpunpcklwd xmm"tostr(i6)", xmm"tostr(i6)", xmm"tostr(i7)"");\
\
  /* shuffle with immediate */\
  asm ("vpshufd xmm"tostr(t0)", xmm"tostr(t0)", 216");\
  asm ("vpshufd xmm"tostr(t1)", xmm"tostr(t1)", 216");\
  asm ("vpshufd xmm"tostr(t2)", xmm"tostr(t2)", 216");\
  asm ("vpshufd xmm"tostr(t3)", xmm"tostr(t3)", 216");\
  asm ("vpshufd xmm"tostr(i0)", xmm"tostr(i0)", 216");\
  asm ("vpshufd xmm"tostr(i2)", xmm"tostr(i2)", 216");\
  asm ("vpshufd xmm"tostr(i4)", xmm"tostr(i4)", 216");\
  asm ("vpshufd xmm"tostr(i6)", xmm"tostr(i6)", 216");\
\
  /* continue with unpack */\
  asm ("vpunpckhdq xmm"tostr(t4)", xmm"tostr(i0)",  xmm"tostr(i2)"");\
  asm ("vpunpckldq xmm"tostr(i0)", xmm"tostr(i0)",  xmm"tostr(i2)"");\
  asm ("vpunpckhdq xmm"tostr(t5)", xmm"tostr(t0)",  xmm"tostr(t1)"");\
  asm ("vpunpckldq xmm"tostr(t0)", xmm"tostr(t0)",  xmm"tostr(t1)"");\
  asm ("vpunpckhdq xmm"tostr(t6)", xmm"tostr(i4)",  xmm"tostr(i6)"");\
  asm ("vpunpckldq xmm"tostr(i4)", xmm"tostr(i4)", xmm"tostr(i6)"");\
  asm ("vpunpckhdq xmm"tostr(t7)", xmm"tostr(t2)",  xmm"tostr(t3)"");\
  asm ("vpunpckldq xmm"tostr(t2)", xmm"tostr(t2)",  xmm"tostr(t3)"");\
\
  /* there are now 2 rows in each xmm */\
  /* unpack to get 1 row of CV in each xmm */\
  asm ("vpunpckhqdq xmm"tostr(i1)", xmm"tostr(i0)", xmm"tostr(i4)"");\
  asm ("vpunpcklqdq xmm"tostr(i0)", xmm"tostr(i0)", xmm"tostr(i4)"");\
  asm ("vpunpcklqdq xmm"tostr(i2)", xmm"tostr(t0)", xmm"tostr(t2)"");\
  asm ("vpunpckhqdq xmm"tostr(i3)", xmm"tostr(t0)", xmm"tostr(t2)"");\
  asm ("vpunpcklqdq xmm"tostr(i4)", xmm"tostr(t4)", xmm"tostr(t6)"");\
  asm ("vpunpckhqdq xmm"tostr(i5)", xmm"tostr(t4)", xmm"tostr(t6)"");\
  asm ("vpunpcklqdq xmm"tostr(i6)", xmm"tostr(t5)", xmm"tostr(t7)"");\
  asm ("vpunpckhqdq xmm"tostr(i7)", xmm"tostr(t5)", xmm"tostr(t7)"");\
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
  asm ("vmovaps xmm"tostr(o0)", [TRANSP_MASK]");\
  /*  transpose matrix to get output format */\
  asm ("vpunpckhqdq xmm"tostr(o1)", xmm"tostr(i0)", xmm"tostr(i1)"");\
  asm ("vpunpcklqdq xmm"tostr(i0)", xmm"tostr(i0)", xmm"tostr(i1)"");\
  asm ("vpunpckhqdq xmm"tostr(t0)", xmm"tostr(i2)", xmm"tostr(i3)"");\
  asm ("vpunpcklqdq xmm"tostr(i2)", xmm"tostr(i2)", xmm"tostr(i3)"");\
  asm ("vpunpckhqdq xmm"tostr(t1)", xmm"tostr(i4)", xmm"tostr(i5)"");\
  asm ("vpunpcklqdq xmm"tostr(i4)", xmm"tostr(i4)", xmm"tostr(i5)"");\
  asm ("vpunpckhqdq xmm"tostr(t2)", xmm"tostr(i6)", xmm"tostr(i7)"");\
  asm ("vpunpcklqdq xmm"tostr(i6)", xmm"tostr(i6)", xmm"tostr(i7)"");\
  /* load transpose mask into a register, because it will be used 8 times */\
  asm ("vpshufb xmm"tostr(i0)", xmm"tostr(i0)", xmm"tostr(o0)"");\
  asm ("vpshufb xmm"tostr(i2)", xmm"tostr(i2)", xmm"tostr(o0)"");\
  asm ("vpshufb xmm"tostr(i4)", xmm"tostr(i4)", xmm"tostr(o0)"");\
  asm ("vpshufb xmm"tostr(i6)", xmm"tostr(i6)", xmm"tostr(o0)"");\
  asm ("vpshufb xmm"tostr(o1)", xmm"tostr(o1)", xmm"tostr(o0)"");\
  asm ("vpshufb xmm"tostr(t0)", xmm"tostr(t0)", xmm"tostr(o0)"");\
  asm ("vpshufb xmm"tostr(t1)", xmm"tostr(t1)", xmm"tostr(o0)"");\
  asm ("vpshufb xmm"tostr(t2)", xmm"tostr(t2)", xmm"tostr(o0)"");\
  /* continue with unpack */\
  asm ("vpunpckhwd xmm"tostr(t3)", xmm"tostr(i4)", xmm"tostr(i6)"");\
  asm ("vpunpcklwd xmm"tostr(i4)", xmm"tostr(i4)", xmm"tostr(i6)"");\
  asm ("vpunpckhwd xmm"tostr(o0)", xmm"tostr(i0)", xmm"tostr(i2)"");\
  asm ("vpunpcklwd xmm"tostr(i0)", xmm"tostr(i0)", xmm"tostr(i2)"");\
  asm ("vpunpckhwd xmm"tostr(o2)", xmm"tostr(o1)", xmm"tostr(t0)"");\
  asm ("vpunpcklwd xmm"tostr(o1)", xmm"tostr(o1)", xmm"tostr(t0)"");\
  asm ("vpunpckhwd xmm"tostr(t4)", xmm"tostr(t1)", xmm"tostr(t2)"");\
  asm ("vpunpcklwd xmm"tostr(t1)", xmm"tostr(t1)", xmm"tostr(t2)"");\
  /* shuffle with immediate */\
  asm ("vpshufd xmm"tostr(i4)", xmm"tostr(i4)", 216");\
  asm ("vpshufd xmm"tostr(t3)", xmm"tostr(t3)", 216");\
  asm ("vpshufd xmm"tostr(o1)", xmm"tostr(o1)", 216");\
  asm ("vpshufd xmm"tostr(o2)", xmm"tostr(o2)", 216");\
  asm ("vpshufd xmm"tostr(i0)", xmm"tostr(i0)", 216");\
  asm ("vpshufd xmm"tostr(o0)", xmm"tostr(o0)", 216");\
  asm ("vpshufd xmm"tostr(t1)", xmm"tostr(t1)", 216");\
  asm ("vpshufd xmm"tostr(t4)", xmm"tostr(t4)", 216");\
  /* continue with unpack */\
  asm ("vpunpckhdq xmm"tostr(i1)", xmm"tostr(i0)", xmm"tostr(i4)"");\
  asm ("vpunpckldq xmm"tostr(i0)", xmm"tostr(i0)", xmm"tostr(i4)"");\
  asm ("vpunpckhdq xmm"tostr(i3)", xmm"tostr(o0)", xmm"tostr(t3)"");\
  asm ("vpunpckldq xmm"tostr(o0)", xmm"tostr(o0)", xmm"tostr(t3)"");\
  asm ("vpunpckhdq xmm"tostr(i5)", xmm"tostr(o1)", xmm"tostr(t1)"");\
  asm ("vpunpckldq xmm"tostr(o1)", xmm"tostr(o1)", xmm"tostr(t1)"");\
  asm ("vpunpckhdq xmm"tostr(i7)", xmm"tostr(o2)", xmm"tostr(t4)"");\
  asm ("vpunpckldq xmm"tostr(o2)", xmm"tostr(o2)", xmm"tostr(t4)"");\
  /* transpose done */\
}/**/


void INIT(u64* h)
{
  /* __cdecl calling convention: */
  /* chaining value CV in rdi    */

  asm (".intel_syntax noprefix");
  asm volatile ("emms");

  /* load IV into registers xmm8 - xmm15 */
  asm ("vmovaps xmm8,  [rdi+0*16]");
  asm ("vmovaps xmm9,  [rdi+1*16]");
  asm ("vmovaps xmm10, [rdi+2*16]");
  asm ("vmovaps xmm11, [rdi+3*16]");
  asm ("vmovaps xmm12, [rdi+4*16]");
  asm ("vmovaps xmm13, [rdi+5*16]");
  asm ("vmovaps xmm14, [rdi+6*16]");
  asm ("vmovaps xmm15, [rdi+7*16]");

  /* transform chaining value from column ordering into row ordering */
  Matrix_Transpose(8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7);

  /* store transposed IV */
  asm ("vmovaps [rdi+0*16], xmm8");
  asm ("vmovaps [rdi+1*16], xmm9");
  asm ("vmovaps [rdi+2*16], xmm10");
  asm ("vmovaps [rdi+3*16], xmm11");
  asm ("vmovaps [rdi+4*16], xmm12");
  asm ("vmovaps [rdi+5*16], xmm13");
  asm ("vmovaps [rdi+6*16], xmm14");
  asm ("vmovaps [rdi+7*16], xmm15");

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

  /* load message into registers xmm8...xmm15 (Q = message) */
  asm ("vmovaps xmm0, [rsi+0*16]");
  asm ("vmovaps xmm1, [rsi+1*16]");
  asm ("vmovaps xmm2, [rsi+2*16]");
  asm ("vmovaps xmm3, [rsi+3*16]");
  asm ("vmovaps xmm4, [rsi+4*16]");
  asm ("vmovaps xmm5, [rsi+5*16]");
  asm ("vmovaps xmm6, [rsi+6*16]");
  asm ("vmovaps xmm7, [rsi+7*16]");

  /* transform message M from column ordering into row ordering */
  Matrix_Transpose(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

  /* load previous chaining value and xor message to CV to get input of P */
  /* we put two rows (2x64 bit) of the CV into one 128-bit xmm register */
  /* result: CV+M in xmm8...xmm15 */
  asm ("vpxor xmm8,  xmm0, [rdi+0*16]");
  asm ("vpxor xmm9,  xmm1, [rdi+1*16]");
  asm ("vpxor xmm10, xmm2, [rdi+2*16]");
  asm ("vpxor xmm11, xmm3, [rdi+3*16]");
  asm ("vpxor xmm12, xmm4, [rdi+4*16]");
  asm ("vpxor xmm13, xmm5, [rdi+5*16]");
  asm ("vpxor xmm14, xmm6, [rdi+6*16]");
  asm ("vpxor xmm15, xmm7, [rdi+7*16]");

  /* generate AVX registers with Q in high and P in low 128 bits */
  asm ("vinsertf128 ymm8,  ymm8,  xmm0, 1");
  asm ("vinsertf128 ymm9,  ymm9,  xmm1, 1");
  asm ("vinsertf128 ymm10, ymm10, xmm2, 1");
  asm ("vinsertf128 ymm11, ymm11, xmm3, 1");
  asm ("vinsertf128 ymm12, ymm12, xmm4, 1");
  asm ("vinsertf128 ymm13, ymm13, xmm5, 1");
  asm ("vinsertf128 ymm14, ymm14, xmm6, 1");
  asm ("vinsertf128 ymm15, ymm15, xmm7, 1");

  /* compute the two permutations P and Q in parallel */
  ROUNDS_P_Q();

  /* extract output of Q to xmm0...xmm7 */
  asm ("vextractf128 xmm0, ymm8,  1");
  asm ("vextractf128 xmm1, ymm9,  1");
  asm ("vextractf128 xmm2, ymm10, 1");
  asm ("vextractf128 xmm3, ymm11, 1");
  asm ("vextractf128 xmm4, ymm12, 1");
  asm ("vextractf128 xmm5, ymm13, 1");
  asm ("vextractf128 xmm6, ymm14, 1");
  asm ("vextractf128 xmm7, ymm15, 1");

  /* xor output of P and Q */
  /* result: P(CV+M)+Q(M) in xmm8...xmm15 */
  asm ("vpxor xmm8,  xmm8,  xmm0");
  asm ("vpxor xmm9,  xmm9,  xmm1");
  asm ("vpxor xmm10, xmm10, xmm2");
  asm ("vpxor xmm11, xmm11, xmm3");
  asm ("vpxor xmm12, xmm12, xmm4");
  asm ("vpxor xmm13, xmm13, xmm5");
  asm ("vpxor xmm14, xmm14, xmm6");
  asm ("vpxor xmm15, xmm15, xmm7");

  /* xor CV (feed-forward) */
  /* result: P(CV+M)+Q(M)+CV in xmm8...xmm15 */
  asm ("vpxor xmm8,  xmm8,  [rdi+0*16]");
  asm ("vpxor xmm9,  xmm9,  [rdi+1*16]");
  asm ("vpxor xmm10, xmm10, [rdi+2*16]");
  asm ("vpxor xmm11, xmm11, [rdi+3*16]");
  asm ("vpxor xmm12, xmm12, [rdi+4*16]");
  asm ("vpxor xmm13, xmm13, [rdi+5*16]");
  asm ("vpxor xmm14, xmm14, [rdi+6*16]");
  asm ("vpxor xmm15, xmm15, [rdi+7*16]");

  /* store CV */
  asm ("vmovaps [rdi+0*16], xmm8");
  asm ("vmovaps [rdi+1*16], xmm9");
  asm ("vmovaps [rdi+2*16], xmm10");
  asm ("vmovaps [rdi+3*16], xmm11");
  asm ("vmovaps [rdi+4*16], xmm12");
  asm ("vmovaps [rdi+5*16], xmm13");
  asm ("vmovaps [rdi+6*16], xmm14");
  asm ("vmovaps [rdi+7*16], xmm15");

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

  asm ("vpxor xmm0, xmm0, xmm0");

  /* load CV into registers xmm8...xmm15 */
  asm ("vmovaps xmm8,  [rdi+0*16]");
  asm ("vmovaps xmm9,  [rdi+1*16]");
  asm ("vmovaps xmm10, [rdi+2*16]");
  asm ("vmovaps xmm11, [rdi+3*16]");
  asm ("vmovaps xmm12, [rdi+4*16]");
  asm ("vmovaps xmm13, [rdi+5*16]");
  asm ("vmovaps xmm14, [rdi+6*16]");
  asm ("vmovaps xmm15, [rdi+7*16]");

  /* compute the permutation P */
  /* result: the output of P(CV) in xmm8...xmm15 */
  ROUNDS_P_Q();

  /* xor CV to P output (feed-forward) */
  /* result: P(CV)+CV in xmm8...xmm15 */
  asm ("vpxor xmm8,  xmm8,  [rdi+0*16]");
  asm ("vpxor xmm9,  xmm9,  [rdi+1*16]");
  asm ("vpxor xmm10, xmm10, [rdi+2*16]");
  asm ("vpxor xmm11, xmm11, [rdi+3*16]");
  asm ("vpxor xmm12, xmm12, [rdi+4*16]");
  asm ("vpxor xmm13, xmm13, [rdi+5*16]");
  asm ("vpxor xmm14, xmm14, [rdi+6*16]");
  asm ("vpxor xmm15, xmm15, [rdi+7*16]");

  /* transpose CV back from row ordering to column ordering */
  /* result: final hash value in xmm0, xmm6, xmm13, xmm15 */
  Matrix_Transpose_INV(8, 9, 10, 11, 12, 13, 14, 15, 4, 0, 6, 1, 2, 3, 5, 7);

  /* we only need to return the truncated half of the state */
  asm ("vmovaps [rdi+4*16], xmm0");
  asm ("vmovaps [rdi+5*16], xmm6");
  asm ("vmovaps [rdi+6*16], xmm13");
  asm ("vmovaps [rdi+7*16], xmm15");

  Pop_All_Regs();
  asm (".att_syntax noprefix");

  return;
}

#endif

