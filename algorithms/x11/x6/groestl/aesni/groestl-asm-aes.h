/* groestl-asm-aes.h     Aug 2011
 *
 * Groestl implementation with inline assembly using ssse3, sse4.1, and aes
 * instructions.
 * Authors: Günther A. Roland, Martin Schläffer, Krystian Matusiewicz
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
__attribute__ ((aligned (16))) unsigned char ALL_1B[16];
__attribute__ ((aligned (16))) unsigned char ALL_FF[16];

/* temporary variables  */
__attribute__ ((aligned (16))) unsigned char QTEMP[8*16];
__attribute__ ((aligned (16))) unsigned char TEMP[3*16];


#define tos(a)    #a
#define tostr(a)  tos(a)


/* xmm[i] will be multiplied by 2
 * xmm[j] will be lost
 * xmm[k] has to be all 0x1b */
#define MUL2(i, j, k){\
  asm("pxor xmm"tostr(j)", xmm"tostr(j)"");\
  asm("pcmpgtb xmm"tostr(j)", xmm"tostr(i)"");\
  asm("paddb xmm"tostr(i)", xmm"tostr(i)"");\
  asm("pand xmm"tostr(j)", xmm"tostr(k)"");\
  asm("pxor xmm"tostr(i)", xmm"tostr(j)"");\
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
  /* t_i = a_i + a_{i+1} */\
  asm("movdqa xmm"tostr(b6)", xmm"tostr(a0)"");\
  asm("movdqa xmm"tostr(b7)", xmm"tostr(a1)"");\
  asm("pxor xmm"tostr(a0)", xmm"tostr(a1)"");\
  asm("movdqa xmm"tostr(b0)", xmm"tostr(a2)"");\
  asm("pxor xmm"tostr(a1)", xmm"tostr(a2)"");\
  asm("movdqa xmm"tostr(b1)", xmm"tostr(a3)"");\
  asm("pxor xmm"tostr(a2)", xmm"tostr(a3)"");\
  asm("movdqa xmm"tostr(b2)", xmm"tostr(a4)"");\
  asm("pxor xmm"tostr(a3)", xmm"tostr(a4)"");\
  asm("movdqa xmm"tostr(b3)", xmm"tostr(a5)"");\
  asm("pxor xmm"tostr(a4)", xmm"tostr(a5)"");\
  asm("movdqa xmm"tostr(b4)", xmm"tostr(a6)"");\
  asm("pxor xmm"tostr(a5)", xmm"tostr(a6)"");\
  asm("movdqa xmm"tostr(b5)", xmm"tostr(a7)"");\
  asm("pxor xmm"tostr(a6)", xmm"tostr(a7)"");\
  asm("pxor xmm"tostr(a7)", xmm"tostr(b6)"");\
  \
  /* build y4 y5 y6 ... in regs xmm8, xmm9, xmm10 by adding t_i*/\
  asm("pxor xmm"tostr(b0)", xmm"tostr(a4)"");\
  asm("pxor xmm"tostr(b6)", xmm"tostr(a4)"");\
  asm("pxor xmm"tostr(b1)", xmm"tostr(a5)"");\
  asm("pxor xmm"tostr(b7)", xmm"tostr(a5)"");\
  asm("pxor xmm"tostr(b2)", xmm"tostr(a6)"");\
  asm("pxor xmm"tostr(b0)", xmm"tostr(a6)"");\
  /* spill values y_4, y_5 to memory */\
  asm("movaps [TEMP+0*16], xmm"tostr(b0)"");\
  asm("pxor xmm"tostr(b3)", xmm"tostr(a7)"");\
  asm("pxor xmm"tostr(b1)", xmm"tostr(a7)"");\
  asm("movaps [TEMP+1*16], xmm"tostr(b1)"");\
  asm("pxor xmm"tostr(b4)", xmm"tostr(a0)"");\
  asm("pxor xmm"tostr(b2)", xmm"tostr(a0)"");\
  /* save values t0, t1, t2 to xmm8, xmm9 and memory */\
  asm("movdqa xmm"tostr(b0)", xmm"tostr(a0)"");\
  asm("pxor xmm"tostr(b5)", xmm"tostr(a1)"");\
  asm("pxor xmm"tostr(b3)", xmm"tostr(a1)"");\
  asm("movdqa xmm"tostr(b1)", xmm"tostr(a1)"");\
  asm("pxor xmm"tostr(b6)", xmm"tostr(a2)"");\
  asm("pxor xmm"tostr(b4)", xmm"tostr(a2)"");\
  asm("movaps [TEMP+2*16], xmm"tostr(a2)"");\
  asm("pxor xmm"tostr(b7)", xmm"tostr(a3)"");\
  asm("pxor xmm"tostr(b5)", xmm"tostr(a3)"");\
  \
  /* compute x_i = t_i + t_{i+3} */\
  asm("pxor xmm"tostr(a0)", xmm"tostr(a3)"");\
  asm("pxor xmm"tostr(a1)", xmm"tostr(a4)"");\
  asm("pxor xmm"tostr(a2)", xmm"tostr(a5)"");\
  asm("pxor xmm"tostr(a3)", xmm"tostr(a6)"");\
  asm("pxor xmm"tostr(a4)", xmm"tostr(a7)"");\
  asm("pxor xmm"tostr(a5)", xmm"tostr(b0)"");\
  asm("pxor xmm"tostr(a6)", xmm"tostr(b1)"");\
  asm("pxor xmm"tostr(a7)", [TEMP+2*16]");\
  \
  /* compute z_i : double x_i using temp xmm8 and 1B xmm9 */\
  /* compute w_i : add y_{i+4} */\
  asm("movaps xmm"tostr(b1)", [ALL_1B]");\
  MUL2(a0, b0, b1);\
  asm("pxor xmm"tostr(a0)", [TEMP+0*16]");\
  MUL2(a1, b0, b1);\
  asm("pxor xmm"tostr(a1)", [TEMP+1*16]");\
  MUL2(a2, b0, b1);\
  asm("pxor xmm"tostr(a2)", xmm"tostr(b2)"");\
  MUL2(a3, b0, b1);\
  asm("pxor xmm"tostr(a3)", xmm"tostr(b3)"");\
  MUL2(a4, b0, b1);\
  asm("pxor xmm"tostr(a4)", xmm"tostr(b4)"");\
  MUL2(a5, b0, b1);\
  asm("pxor xmm"tostr(a5)", xmm"tostr(b5)"");\
  MUL2(a6, b0, b1);\
  asm("pxor xmm"tostr(a6)", xmm"tostr(b6)"");\
  MUL2(a7, b0, b1);\
  asm("pxor xmm"tostr(a7)", xmm"tostr(b7)"");\
  \
  /* compute v_i : double w_i      */\
  /* add to y_4 y_5 .. v3, v4, ... */\
  MUL2(a0, b0, b1);\
  asm("pxor xmm"tostr(b5)", xmm"tostr(a0)"");\
  MUL2(a1, b0, b1);\
  asm("pxor xmm"tostr(b6)", xmm"tostr(a1)"");\
  MUL2(a2, b0, b1);\
  asm("pxor xmm"tostr(b7)", xmm"tostr(a2)"");\
  MUL2(a5, b0, b1);\
  asm("pxor xmm"tostr(b2)", xmm"tostr(a5)"");\
  MUL2(a6, b0, b1);\
  asm("pxor xmm"tostr(b3)", xmm"tostr(a6)"");\
  MUL2(a7, b0, b1);\
  asm("pxor xmm"tostr(b4)", xmm"tostr(a7)"");\
  MUL2(a3, b0, b1);\
  MUL2(a4, b0, b1);\
  asm("movaps xmm"tostr(b0)", [TEMP+0*16]");\
  asm("movaps xmm"tostr(b1)", [TEMP+1*16]");\
  asm("pxor xmm"tostr(b0)", xmm"tostr(a3)"");\
  asm("pxor xmm"tostr(b1)", xmm"tostr(a4)"");\
}/*MixBytes*/

#if (LENGTH <= 256)

#define SET_CONSTANTS(){\
  ((u64*)ALL_1B)[0] = 0x1b1b1b1b1b1b1b1bULL;\
  ((u64*)ALL_1B)[1] = 0x1b1b1b1b1b1b1b1bULL;\
  ((u64*)TRANSP_MASK)[0] = 0x0d0509010c040800ULL;\
  ((u64*)TRANSP_MASK)[1] = 0x0f070b030e060a02ULL;\
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

/* one round
 * i = round number
 * a0-a7 = input rows
 * b0-b7 = output rows
 */
#define ROUND(i, a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7){\
  /* AddRoundConstant */\
  asm ("movaps xmm"tostr(b1)", [ROUND_CONST_Lx]");\
  asm ("pxor   xmm"tostr(a0)", [ROUND_CONST_L0+"tostr(i)"*16]");\
  asm ("pxor   xmm"tostr(a1)", xmm"tostr(b1)"");\
  asm ("pxor   xmm"tostr(a2)", xmm"tostr(b1)"");\
  asm ("pxor   xmm"tostr(a3)", xmm"tostr(b1)"");\
  asm ("pxor   xmm"tostr(a4)", xmm"tostr(b1)"");\
  asm ("pxor   xmm"tostr(a5)", xmm"tostr(b1)"");\
  asm ("pxor   xmm"tostr(a6)", xmm"tostr(b1)"");\
  asm ("pxor   xmm"tostr(a7)", [ROUND_CONST_L7+"tostr(i)"*16]");\
  /* ShiftBytes + SubBytes (interleaved) */\
  asm ("pxor xmm"tostr(b0)",  xmm"tostr(b0)"");\
  asm ("pshufb     xmm"tostr(a0)", [SUBSH_MASK+0*16]");\
  asm ("aesenclast xmm"tostr(a0)", xmm"tostr(b0)"");\
  asm ("pshufb     xmm"tostr(a1)", [SUBSH_MASK+1*16]");\
  asm ("aesenclast xmm"tostr(a1)", xmm"tostr(b0)"");\
  asm ("pshufb     xmm"tostr(a2)", [SUBSH_MASK+2*16]");\
  asm ("aesenclast xmm"tostr(a2)", xmm"tostr(b0)"");\
  asm ("pshufb     xmm"tostr(a3)", [SUBSH_MASK+3*16]");\
  asm ("aesenclast xmm"tostr(a3)", xmm"tostr(b0)"");\
  asm ("pshufb     xmm"tostr(a4)", [SUBSH_MASK+4*16]");\
  asm ("aesenclast xmm"tostr(a4)", xmm"tostr(b0)"");\
  asm ("pshufb     xmm"tostr(a5)", [SUBSH_MASK+5*16]");\
  asm ("aesenclast xmm"tostr(a5)", xmm"tostr(b0)"");\
  asm ("pshufb     xmm"tostr(a6)", [SUBSH_MASK+6*16]");\
  asm ("aesenclast xmm"tostr(a6)", xmm"tostr(b0)"");\
  asm ("pshufb     xmm"tostr(a7)", [SUBSH_MASK+7*16]");\
  asm ("aesenclast xmm"tostr(a7)", xmm"tostr(b0)"");\
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


void INIT(u64* h)
{
  /* __cdecl calling convention: */
  /* chaining value CV in rdi    */

  asm (".intel_syntax noprefix");
  asm volatile ("emms");

  /* load IV into registers xmm12 - xmm15 */
  asm ("movaps xmm12, [rdi+0*16]");
  asm ("movaps xmm13, [rdi+1*16]");
  asm ("movaps xmm14, [rdi+2*16]");
  asm ("movaps xmm15, [rdi+3*16]");

  /* transform chaining value from column ordering into row ordering */
  /* we put two rows (64 bit) of the IV into one 128-bit XMM register */
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
  /* we first put two rows (2x64 bit) of the message into one 128-bit xmm register */
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
  ((u64*)ALL_1B)[0] = 0x1b1b1b1b1b1b1b1bULL;\
  ((u64*)ALL_1B)[1] = 0x1b1b1b1b1b1b1b1bULL;\
  ((u64*)ALL_FF)[0] = 0xffffffffffffffffULL;\
  ((u64*)ALL_FF)[1] = 0xffffffffffffffffULL;\
  ((u64*)TRANSP_MASK)[0] = 0x0d0509010c040800ULL;\
  ((u64*)TRANSP_MASK)[1] = 0x0f070b030e060a02ULL;\
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

/* one round
 * a0-a7 = input rows
 * b0-b7 = output rows
 */
#define SUBMIX(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7){\
  /* SubBytes */\
  asm ("pxor       xmm"tostr(b0)", xmm"tostr(b0)"");\
  asm ("aesenclast xmm"tostr(a0)", xmm"tostr(b0)"");\
  asm ("aesenclast xmm"tostr(a1)", xmm"tostr(b0)"");\
  asm ("aesenclast xmm"tostr(a2)", xmm"tostr(b0)"");\
  asm ("aesenclast xmm"tostr(a3)", xmm"tostr(b0)"");\
  asm ("aesenclast xmm"tostr(a4)", xmm"tostr(b0)"");\
  asm ("aesenclast xmm"tostr(a5)", xmm"tostr(b0)"");\
  asm ("aesenclast xmm"tostr(a6)", xmm"tostr(b0)"");\
  asm ("aesenclast xmm"tostr(a7)", xmm"tostr(b0)"");\
  /* MixBytes */\
  MixBytes(a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7);\
}

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
  \
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
  asm ("add al, 4");\
  asm ("add bl, 4");\
  asm ("mov rcx, rax");\
  asm ("sub cl, 28");\
  asm ("jb 1b");\
}

#define ROUNDS_Q(){\
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
  \
  /* AddConstant */\
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
}

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


void INIT(u64* h)
{
  /* __cdecl calling convention: */
  /* chaining value CV in rdi    */

  asm (".intel_syntax noprefix");
  asm volatile ("emms");

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

