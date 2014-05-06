/* $Id: keccak.c 259 2011-07-19 22:11:27Z tp $ */
/*
 * Keccak implementation.
 *
 * ==========================(LICENSE BEGIN)============================
 *
 * Copyright (c) 2007-2010  Projet RNRT SAPHIR
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ===========================(LICENSE END)=============================
 *
 * @author   Thomas Pornin <thomas.pornin@cryptolog.com>
 */

#define QSTATIC static

#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "sph_keccak.h"

#ifdef __cplusplus
extern "C"{
#endif

/*
 * Parameters:
 *
 *  SPH_KECCAK_64          use a 64-bit type
 *  SPH_KECCAK_INTERLEAVE  use bit-interleaving (32-bit type only)
 *  SPH_KECCAK_NOCOPY      do not copy the state into local variables
 * 
 * If there is no usable 64-bit type, the code automatically switches
 * back to the 32-bit implementation.
 *
 * Some tests on an Intel Core2 Q6600 (both 64-bit and 32-bit, 32 kB L1
 * code cache), a PowerPC (G3, 32 kB L1 code cache), an ARM920T core
 * (16 kB L1 code cache), and a small MIPS-compatible CPU (Broadcom BCM3302,
 * 8 kB L1 code cache), seem to show that the following are optimal:
 *
 * -- x86, 64-bit: use the 64-bit implementation, unroll 8 rounds,
 * do not copy the state; unrolling 2, 6 or all rounds also provides
 * near-optimal performance.
 * -- x86, 32-bit: use the 32-bit implementation, unroll 6 rounds,
 * interleave, do not copy the state. Unrolling 1, 2, 4 or 8 rounds
 * also provides near-optimal performance.
 * -- PowerPC: use the 64-bit implementation, unroll 8 rounds,
 * copy the state. Unrolling 4 or 6 rounds is near-optimal.
 * -- ARM: use the 64-bit implementation, unroll 2 or 4 rounds,
 * copy the state.
 * -- MIPS: use the 64-bit implementation, unroll 2 rounds, copy
 * the state. Unrolling only 1 round is also near-optimal.
 *
 * Also, interleaving does not always yield actual improvements when
 * using a 32-bit implementation; in particular when the architecture
 * does not offer a native rotation opcode (interleaving replaces one
 * 64-bit rotation with two 32-bit rotations, which is a gain only if
 * there is a native 32-bit rotation opcode and not a native 64-bit
 * rotation opcode; also, interleaving implies a small overhead when
 * processing input words).
 *
 * To sum up:
 * -- when possible, use the 64-bit code
 * -- exception: on 32-bit x86, use 32-bit code
 * -- when using 32-bit code, use interleaving
 * -- copy the state, except on x86
 * -- unroll 8 rounds on "big" machine, 2 rounds on "small" machines
 */


#ifdef _MSC_VER
#pragma warning (disable: 4146)
#endif


static const sph_u64 RC[] = {
	SPH_C64(0x0000000000000001), SPH_C64(0x0000000000008082),
	SPH_C64(0x800000000000808A), SPH_C64(0x8000000080008000),
	SPH_C64(0x000000000000808B), SPH_C64(0x0000000080000001),
	SPH_C64(0x8000000080008081), SPH_C64(0x8000000000008009),
	SPH_C64(0x000000000000008A), SPH_C64(0x0000000000000088),
	SPH_C64(0x0000000080008009), SPH_C64(0x000000008000000A),
	SPH_C64(0x000000008000808B), SPH_C64(0x800000000000008B),
	SPH_C64(0x8000000000008089), SPH_C64(0x8000000000008003),
	SPH_C64(0x8000000000008002), SPH_C64(0x8000000000000080),
	SPH_C64(0x000000000000800A), SPH_C64(0x800000008000000A),
	SPH_C64(0x8000000080008081), SPH_C64(0x8000000000008080),
	SPH_C64(0x0000000080000001), SPH_C64(0x8000000080008008)
};

#define kekDECL_STATE \
	sph_u64 keca00, keca01, keca02, keca03, keca04; \
	sph_u64 keca10, keca11, keca12, keca13, keca14; \
	sph_u64 keca20, keca21, keca22, keca23, keca24; \
	sph_u64 keca30, keca31, keca32, keca33, keca34; \
	sph_u64 keca40, keca41, keca42, keca43, keca44;

#define kekREAD_STATE(state)   do { \
		keca00 = (state)->kecu.wide[ 0]; \
		keca10 = (state)->kecu.wide[ 1]; \
		keca20 = (state)->kecu.wide[ 2]; \
		keca30 = (state)->kecu.wide[ 3]; \
		keca40 = (state)->kecu.wide[ 4]; \
		keca01 = (state)->kecu.wide[ 5]; \
		keca11 = (state)->kecu.wide[ 6]; \
		keca21 = (state)->kecu.wide[ 7]; \
		keca31 = (state)->kecu.wide[ 8]; \
		keca41 = (state)->kecu.wide[ 9]; \
		keca02 = (state)->kecu.wide[10]; \
		keca12 = (state)->kecu.wide[11]; \
		keca22 = (state)->kecu.wide[12]; \
		keca32 = (state)->kecu.wide[13]; \
		keca42 = (state)->kecu.wide[14]; \
		keca03 = (state)->kecu.wide[15]; \
		keca13 = (state)->kecu.wide[16]; \
		keca23 = (state)->kecu.wide[17]; \
		keca33 = (state)->kecu.wide[18]; \
		keca43 = (state)->kecu.wide[19]; \
		keca04 = (state)->kecu.wide[20]; \
		keca14 = (state)->kecu.wide[21]; \
		keca24 = (state)->kecu.wide[22]; \
		keca34 = (state)->kecu.wide[23]; \
		keca44 = (state)->kecu.wide[24]; \
	} while (0)

#define kecREAD_STATE(state)   do { \
		keca00 = kecu.wide[ 0]; \
		keca10 = kecu.wide[ 1]; \
		keca20 = kecu.wide[ 2]; \
		keca30 = kecu.wide[ 3]; \
		keca40 = kecu.wide[ 4]; \
		keca01 = kecu.wide[ 5]; \
		keca11 = kecu.wide[ 6]; \
		keca21 = kecu.wide[ 7]; \
		keca31 = kecu.wide[ 8]; \
		keca41 = kecu.wide[ 9]; \
		keca02 = kecu.wide[10]; \
		keca12 = kecu.wide[11]; \
		keca22 = kecu.wide[12]; \
		keca32 = kecu.wide[13]; \
		keca42 = kecu.wide[14]; \
		keca03 = kecu.wide[15]; \
		keca13 = kecu.wide[16]; \
		keca23 = kecu.wide[17]; \
		keca33 = kecu.wide[18]; \
		keca43 = kecu.wide[19]; \
		keca04 = kecu.wide[20]; \
		keca14 = kecu.wide[21]; \
		keca24 = kecu.wide[22]; \
		keca34 = kecu.wide[23]; \
		keca44 = kecu.wide[24]; \
	} while (0)

#define kecINIT_STATE()   do { \
		keca00 = 0x0000000000000000  \
		    ^ sph_dec64le_aligned(buf +   0); \
		keca10 = 0xFFFFFFFFFFFFFFFF  \
		    ^ sph_dec64le_aligned(buf +   8); \
		keca20 = 0xFFFFFFFFFFFFFFFF  \
		    ^ sph_dec64le_aligned(buf +  16); \
		keca30 = 0x0000000000000000  \
		    ^ sph_dec64le_aligned(buf +  24); \
		keca40 = 0x0000000000000000  \
		    ^ sph_dec64le_aligned(buf +  32); \
		keca01 = 0x0000000000000000  \
		    ^ sph_dec64le_aligned(buf +  40); \
		keca11 = 0x0000000000000000  \
		    ^ sph_dec64le_aligned(buf +  48); \
		keca21 = 0x0000000000000000  \
		    ^ sph_dec64le_aligned(buf +  56); \
		keca31 = 0xFFFFFFFFFFFFFFFF  \
		    ^ sph_dec64le_aligned(buf +  64); \
		keca41 = 0x0000000000000000, \
		keca02 = 0x0000000000000000, \
		keca12 = 0x0000000000000000, \
		keca32 = 0x0000000000000000, \
		keca42 = 0x0000000000000000, \
		keca03 = 0x0000000000000000, \
		keca13 = 0x0000000000000000, \
		keca33 = 0x0000000000000000, \
		keca43 = 0x0000000000000000, \
		keca14 = 0x0000000000000000, \
		keca24 = 0x0000000000000000, \
		keca34 = 0x0000000000000000, \
		keca44 = 0x0000000000000000; \
		keca23 = 0xFFFFFFFFFFFFFFFF, \
		keca04 = 0xFFFFFFFFFFFFFFFF, \
		keca22 = 0xFFFFFFFFFFFFFFFF; \
	} while (0)

#define kekWRITE_STATE(state)   do { \
		(state)->kecu.wide[ 0] = keca00; \
		(state)->kecu.wide[ 1] = ~keca10; \
		(state)->kecu.wide[ 2] = ~keca20; \
		(state)->kecu.wide[ 3] = keca30; \
		(state)->kecu.wide[ 4] = keca40; \
		(state)->kecu.wide[ 5] = keca01; \
		(state)->kecu.wide[ 6] = keca11; \
		(state)->kecu.wide[ 7] = keca21; \
		(state)->kecu.wide[ 8] = ~keca31; \
		(state)->kecu.wide[ 9] = keca41; \
		(state)->kecu.wide[10] = keca02; \
		(state)->kecu.wide[11] = keca12; \
		(state)->kecu.wide[12] = ~keca22; \
		(state)->kecu.wide[13] = keca32; \
		(state)->kecu.wide[14] = keca42; \
		(state)->kecu.wide[15] = keca03; \
		(state)->kecu.wide[16] = keca13; \
		(state)->kecu.wide[17] = ~keca23; \
		(state)->kecu.wide[18] = keca33; \
		(state)->kecu.wide[19] = keca43; \
		(state)->kecu.wide[20] = ~keca04; \
		(state)->kecu.wide[21] = keca14; \
		(state)->kecu.wide[22] = keca24; \
		(state)->kecu.wide[23] = keca34; \
		(state)->kecu.wide[24] = keca44; \
	} while (0)

/* only usefull for one round final */
#define kecWRITE_STATE(state)   do { \
		kecu.wide[ 0] = keca00; \
		kecu.wide[ 1] = ~keca10; \
		kecu.wide[ 2] = ~keca20; \
		kecu.wide[ 3] = keca30; \
		kecu.wide[ 4] = keca40; \
		kecu.wide[ 5] = keca01; \
		kecu.wide[ 6] = keca11; \
		kecu.wide[ 7] = keca21; \
		kecu.wide[ 8] = ~keca31; \
		kecu.wide[ 9] = keca41; \
		kecu.wide[10] = keca02; \
		kecu.wide[11] = keca12; \
		kecu.wide[12] = ~keca22; \
		kecu.wide[13] = keca32; \
		kecu.wide[14] = keca42; \
		kecu.wide[15] = keca03; \
		kecu.wide[16] = keca13; \
		kecu.wide[17] = ~keca23; \
		kecu.wide[18] = keca33; \
		kecu.wide[19] = keca43; \
		kecu.wide[20] = ~keca04; \
		kecu.wide[21] = keca14; \
		kecu.wide[22] = keca24; \
		kecu.wide[23] = keca34; \
		kecu.wide[24] = keca44; \
	} while (0)

#define kecPRINT_STATE(state)   do { \
		printf("keca00=%lX\n", keca00); \
		printf("keca10=%lX\n", keca10); \
		printf("keca20=%lX\n", keca20); \
		printf("keca30=%lX\n", keca30); \
		printf("keca40=%lX\n", keca40); \
		printf("keca01=%lX\n", keca01); \
		printf("keca11=%lX\n", keca11); \
		printf("keca21=%lX\n", keca21); \
		printf("keca31=%lX\n", keca31); \
		printf("keca41=%lX\n", keca41); \
		printf("keca02=%lX\n", keca02); \
		printf("keca12=%lX\n", keca12); \
		printf("keca22=%lX\n", keca22); \
		printf("keca32=%lX\n", keca32); \
		printf("keca42=%lX\n", keca42); \
		printf("keca03=%lX\n", keca03); \
		printf("keca13=%lX\n", keca13); \
		printf("keca23=%lX\n", keca23); \
		printf("keca33=%lX\n", keca33); \
		printf("keca43=%lX\n", keca43); \
		printf("keca04=%lX\n", keca04); \
		printf("keca14=%lX\n", keca14); \
		printf("keca24=%lX\n", keca24); \
		printf("keca34=%lX\n", keca34); \
		printf("keca44=%lX\n", keca44); \
                abort(); \
	} while (0)

#define kekINPUT_BUF()   do { \
	} while (0)


#define kekDECL64(x)        sph_u64 x
#define MOV64(d, s)      (d = s)
#define XOR64(d, a, b)   (d = a ^ b)
#define AND64(d, a, b)   (d = a & b)
#define OR64(d, a, b)    (d = a | b)
#define NOT64(d, s)      (d = SPH_T64(~s))
#define ROL64(d, v, n)   (d = SPH_ROTL64(v, n))
#define XOR64_IOTA       XOR64

#define TH_ELT(t, c0, c1, c2, c3, c4, d0, d1, d2, d3, d4)   do { \
		kekDECL64(tt0); \
		kekDECL64(tt1); \
		kekDECL64(tt2); \
		kekDECL64(tt3); \
		XOR64(tt0, d0, d1); \
		XOR64(tt1, d2, d3); \
		XOR64(tt0, tt0, d4); \
		XOR64(tt0, tt0, tt1); \
		ROL64(tt0, tt0, 1); \
		XOR64(tt2, c0, c1); \
		XOR64(tt3, c2, c3); \
		XOR64(tt0, tt0, c4); \
		XOR64(tt2, tt2, tt3); \
		XOR64(t, tt0, tt2); \
	} while (0)

#define THETA(b00, b01, b02, b03, b04, b10, b11, b12, b13, b14, \
	b20, b21, b22, b23, b24, b30, b31, b32, b33, b34, \
	b40, b41, b42, b43, b44) \
	do { \
		kekDECL64(t0); \
		kekDECL64(t1); \
		kekDECL64(t2); \
		kekDECL64(t3); \
		kekDECL64(t4); \
		TH_ELT(t0, b40, b41, b42, b43, b44, b10, b11, b12, b13, b14); \
		TH_ELT(t1, b00, b01, b02, b03, b04, b20, b21, b22, b23, b24); \
		TH_ELT(t2, b10, b11, b12, b13, b14, b30, b31, b32, b33, b34); \
		TH_ELT(t3, b20, b21, b22, b23, b24, b40, b41, b42, b43, b44); \
		TH_ELT(t4, b30, b31, b32, b33, b34, b00, b01, b02, b03, b04); \
		XOR64(b00, b00, t0); \
		XOR64(b01, b01, t0); \
		XOR64(b02, b02, t0); \
		XOR64(b03, b03, t0); \
		XOR64(b04, b04, t0); \
		XOR64(b10, b10, t1); \
		XOR64(b11, b11, t1); \
		XOR64(b12, b12, t1); \
		XOR64(b13, b13, t1); \
		XOR64(b14, b14, t1); \
		XOR64(b20, b20, t2); \
		XOR64(b21, b21, t2); \
		XOR64(b22, b22, t2); \
		XOR64(b23, b23, t2); \
		XOR64(b24, b24, t2); \
		XOR64(b30, b30, t3); \
		XOR64(b31, b31, t3); \
		XOR64(b32, b32, t3); \
		XOR64(b33, b33, t3); \
		XOR64(b34, b34, t3); \
		XOR64(b40, b40, t4); \
		XOR64(b41, b41, t4); \
		XOR64(b42, b42, t4); \
		XOR64(b43, b43, t4); \
		XOR64(b44, b44, t4); \
	} while (0)

#define RHO(b00, b01, b02, b03, b04, b10, b11, b12, b13, b14, \
	b20, b21, b22, b23, b24, b30, b31, b32, b33, b34, \
	b40, b41, b42, b43, b44) \
	do { \
		/* ROL64(b00, b00,  0); */ \
		ROL64(b01, b01, 36); \
		ROL64(b02, b02,  3); \
		ROL64(b03, b03, 41); \
		ROL64(b04, b04, 18); \
		ROL64(b10, b10,  1); \
		ROL64(b11, b11, 44); \
		ROL64(b12, b12, 10); \
		ROL64(b13, b13, 45); \
		ROL64(b14, b14,  2); \
		ROL64(b20, b20, 62); \
		ROL64(b21, b21,  6); \
		ROL64(b22, b22, 43); \
		ROL64(b23, b23, 15); \
		ROL64(b24, b24, 61); \
		ROL64(b30, b30, 28); \
		ROL64(b31, b31, 55); \
		ROL64(b32, b32, 25); \
		ROL64(b33, b33, 21); \
		ROL64(b34, b34, 56); \
		ROL64(b40, b40, 27); \
		ROL64(b41, b41, 20); \
		ROL64(b42, b42, 39); \
		ROL64(b43, b43,  8); \
		ROL64(b44, b44, 14); \
	} while (0)

/*
 * The KHI macro integrates the "lane complement" optimization. On input,
 * some words are complemented:
 *    keca00 keca01 keca02 keca04 keca13 keca20 keca21 keca22 keca30 keca33 keca34 keca43
 * On output, the following words are complemented:
 *    keca04 keca10 keca20 keca22 keca23 keca31
 *
 * The (implicit) permutation and the theta expansion will bring back
 * the input mask for the next round.
 */

#define KHI_XO(d, a, b, c)   do { \
		kekDECL64(kt); \
		OR64(kt, b, c); \
		XOR64(d, a, kt); \
	} while (0)

#define KHI_XA(d, a, b, c)   do { \
		kekDECL64(kt); \
		AND64(kt, b, c); \
		XOR64(d, a, kt); \
	} while (0)

#define KHI(b00, b01, b02, b03, b04, b10, b11, b12, b13, b14, \
	b20, b21, b22, b23, b24, b30, b31, b32, b33, b34, \
	b40, b41, b42, b43, b44) \
	do { \
		kekDECL64(c0); \
		kekDECL64(c1); \
		kekDECL64(c2); \
		kekDECL64(c3); \
		kekDECL64(c4); \
		kekDECL64(bnn); \
		NOT64(bnn, b20); \
		KHI_XO(c0, b00, b10, b20); \
		KHI_XO(c1, b10, bnn, b30); \
		KHI_XA(c2, b20, b30, b40); \
		KHI_XO(c3, b30, b40, b00); \
		KHI_XA(c4, b40, b00, b10); \
		MOV64(b00, c0); \
		MOV64(b10, c1); \
		MOV64(b20, c2); \
		MOV64(b30, c3); \
		MOV64(b40, c4); \
		NOT64(bnn, b41); \
		KHI_XO(c0, b01, b11, b21); \
		KHI_XA(c1, b11, b21, b31); \
		KHI_XO(c2, b21, b31, bnn); \
		KHI_XO(c3, b31, b41, b01); \
		KHI_XA(c4, b41, b01, b11); \
		MOV64(b01, c0); \
		MOV64(b11, c1); \
		MOV64(b21, c2); \
		MOV64(b31, c3); \
		MOV64(b41, c4); \
		NOT64(bnn, b32); \
		KHI_XO(c0, b02, b12, b22); \
		KHI_XA(c1, b12, b22, b32); \
		KHI_XA(c2, b22, bnn, b42); \
		KHI_XO(c3, bnn, b42, b02); \
		KHI_XA(c4, b42, b02, b12); \
		MOV64(b02, c0); \
		MOV64(b12, c1); \
		MOV64(b22, c2); \
		MOV64(b32, c3); \
		MOV64(b42, c4); \
		NOT64(bnn, b33); \
		KHI_XA(c0, b03, b13, b23); \
		KHI_XO(c1, b13, b23, b33); \
		KHI_XO(c2, b23, bnn, b43); \
		KHI_XA(c3, bnn, b43, b03); \
		KHI_XO(c4, b43, b03, b13); \
		MOV64(b03, c0); \
		MOV64(b13, c1); \
		MOV64(b23, c2); \
		MOV64(b33, c3); \
		MOV64(b43, c4); \
		NOT64(bnn, b14); \
		KHI_XA(c0, b04, bnn, b24); \
		KHI_XO(c1, bnn, b24, b34); \
		KHI_XA(c2, b24, b34, b44); \
		KHI_XO(c3, b34, b44, b04); \
		KHI_XA(c4, b44, b04, b14); \
		MOV64(b04, c0); \
		MOV64(b14, c1); \
		MOV64(b24, c2); \
		MOV64(b34, c3); \
		MOV64(b44, c4); \
	} while (0)

#define IOTA(r)   XOR64_IOTA(keca00, keca00, r)

#define P0    keca00, keca01, keca02, keca03, keca04, keca10, keca11, keca12, keca13, keca14, keca20, keca21, \
              keca22, keca23, keca24, keca30, keca31, keca32, keca33, keca34, keca40, keca41, keca42, keca43, keca44
#define P1    keca00, keca30, keca10, keca40, keca20, keca11, keca41, keca21, keca01, keca31, keca22, keca02, \
              keca32, keca12, keca42, keca33, keca13, keca43, keca23, keca03, keca44, keca24, keca04, keca34, keca14
#define P2    keca00, keca33, keca11, keca44, keca22, keca41, keca24, keca02, keca30, keca13, keca32, keca10, \
              keca43, keca21, keca04, keca23, keca01, keca34, keca12, keca40, keca14, keca42, keca20, keca03, keca31
#define P3    keca00, keca23, keca41, keca14, keca32, keca24, keca42, keca10, keca33, keca01, keca43, keca11, \
              keca34, keca02, keca20, keca12, keca30, keca03, keca21, keca44, keca31, keca04, keca22, keca40, keca13
#define P4    keca00, keca12, keca24, keca31, keca43, keca42, keca04, keca11, keca23, keca30, keca34, keca41, \
              keca03, keca10, keca22, keca21, keca33, keca40, keca02, keca14, keca13, keca20, keca32, keca44, keca01
#define P5    keca00, keca21, keca42, keca13, keca34, keca04, keca20, keca41, keca12, keca33, keca03, keca24, \
              keca40, keca11, keca32, keca02, keca23, keca44, keca10, keca31, keca01, keca22, keca43, keca14, keca30
#define P6    keca00, keca02, keca04, keca01, keca03, keca20, keca22, keca24, keca21, keca23, keca40, keca42, \
              keca44, keca41, keca43, keca10, keca12, keca14, keca11, keca13, keca30, keca32, keca34, keca31, keca33
#define P7    keca00, keca10, keca20, keca30, keca40, keca22, keca32, keca42, keca02, keca12, keca44, keca04, \
              keca14, keca24, keca34, keca11, keca21, keca31, keca41, keca01, keca33, keca43, keca03, keca13, keca23
#define P8    keca00, keca11, keca22, keca33, keca44, keca32, keca43, keca04, keca10, keca21, keca14, keca20, \
              keca31, keca42, keca03, keca41, keca02, keca13, keca24, keca30, keca23, keca34, keca40, keca01, keca12
#define P9    keca00, keca41, keca32, keca23, keca14, keca43, keca34, keca20, keca11, keca02, keca31, keca22, \
              keca13, keca04, keca40, keca24, keca10, keca01, keca42, keca33, keca12, keca03, keca44, keca30, keca21
#define P10   keca00, keca24, keca43, keca12, keca31, keca34, keca03, keca22, keca41, keca10, keca13, keca32, \
              keca01, keca20, keca44, keca42, keca11, keca30, keca04, keca23, keca21, keca40, keca14, keca33, keca02
#define P11   keca00, keca42, keca34, keca21, keca13, keca03, keca40, keca32, keca24, keca11, keca01, keca43, \
              keca30, keca22, keca14, keca04, keca41, keca33, keca20, keca12, keca02, keca44, keca31, keca23, keca10
#define P12   keca00, keca04, keca03, keca02, keca01, keca40, keca44, keca43, keca42, keca41, keca30, keca34, \
              keca33, keca32, keca31, keca20, keca24, keca23, keca22, keca21, keca10, keca14, keca13, keca12, keca11
#define P13   keca00, keca20, keca40, keca10, keca30, keca44, keca14, keca34, keca04, keca24, keca33, keca03, \
              keca23, keca43, keca13, keca22, keca42, keca12, keca32, keca02, keca11, keca31, keca01, keca21, keca41
#define P14   keca00, keca22, keca44, keca11, keca33, keca14, keca31, keca03, keca20, keca42, keca23, keca40, \
              keca12, keca34, keca01, keca32, keca04, keca21, keca43, keca10, keca41, keca13, keca30, keca02, keca24
#define P15   keca00, keca32, keca14, keca41, keca23, keca31, keca13, keca40, keca22, keca04, keca12, keca44, \
              keca21, keca03, keca30, keca43, keca20, keca02, keca34, keca11, keca24, keca01, keca33, keca10, keca42
#define P16   keca00, keca43, keca31, keca24, keca12, keca13, keca01, keca44, keca32, keca20, keca21, keca14, \
              keca02, keca40, keca33, keca34, keca22, keca10, keca03, keca41, keca42, keca30, keca23, keca11, keca04
#define P17   keca00, keca34, keca13, keca42, keca21, keca01, keca30, keca14, keca43, keca22, keca02, keca31, \
              keca10, keca44, keca23, keca03, keca32, keca11, keca40, keca24, keca04, keca33, keca12, keca41, keca20
#define P18   keca00, keca03, keca01, keca04, keca02, keca30, keca33, keca31, keca34, keca32, keca10, keca13, \
              keca11, keca14, keca12, keca40, keca43, keca41, keca44, keca42, keca20, keca23, keca21, keca24, keca22
#define P19   keca00, keca40, keca30, keca20, keca10, keca33, keca23, keca13, keca03, keca43, keca11, keca01, \
              keca41, keca31, keca21, keca44, keca34, keca24, keca14, keca04, keca22, keca12, keca02, keca42, keca32
#define P20   keca00, keca44, keca33, keca22, keca11, keca23, keca12, keca01, keca40, keca34, keca41, keca30, \
              keca24, keca13, keca02, keca14, keca03, keca42, keca31, keca20, keca32, keca21, keca10, keca04, keca43
#define P21   keca00, keca14, keca23, keca32, keca41, keca12, keca21, keca30, keca44, keca03, keca24, keca33, \
              keca42, keca01, keca10, keca31, keca40, keca04, keca13, keca22, keca43, keca02, keca11, keca20, keca34
#define P22   keca00, keca31, keca12, keca43, keca24, keca21, keca02, keca33, keca14, keca40, keca42, keca23, \
              keca04, keca30, keca11, keca13, keca44, keca20, keca01, keca32, keca34, keca10, keca41, keca22, keca03
#define P23   keca00, keca13, keca21, keca34, keca42, keca02, keca10, keca23, keca31, keca44, keca04, keca12, \
              keca20, keca33, keca41, keca01, keca14, keca22, keca30, keca43, keca03, keca11, keca24, keca32, keca40

#define P1_TO_P0   do { \
		kekDECL64(t); \
		MOV64(t, keca01); \
		MOV64(keca01, keca30); \
		MOV64(keca30, keca33); \
		MOV64(keca33, keca23); \
		MOV64(keca23, keca12); \
		MOV64(keca12, keca21); \
		MOV64(keca21, keca02); \
		MOV64(keca02, keca10); \
		MOV64(keca10, keca11); \
		MOV64(keca11, keca41); \
		MOV64(keca41, keca24); \
		MOV64(keca24, keca42); \
		MOV64(keca42, keca04); \
		MOV64(keca04, keca20); \
		MOV64(keca20, keca22); \
		MOV64(keca22, keca32); \
		MOV64(keca32, keca43); \
		MOV64(keca43, keca34); \
		MOV64(keca34, keca03); \
		MOV64(keca03, keca40); \
		MOV64(keca40, keca44); \
		MOV64(keca44, keca14); \
		MOV64(keca14, keca31); \
		MOV64(keca31, keca13); \
		MOV64(keca13, t); \
	} while (0)

#define P2_TO_P0   do { \
		kekDECL64(t); \
		MOV64(t, keca01); \
		MOV64(keca01, keca33); \
		MOV64(keca33, keca12); \
		MOV64(keca12, keca02); \
		MOV64(keca02, keca11); \
		MOV64(keca11, keca24); \
		MOV64(keca24, keca04); \
		MOV64(keca04, keca22); \
		MOV64(keca22, keca43); \
		MOV64(keca43, keca03); \
		MOV64(keca03, keca44); \
		MOV64(keca44, keca31); \
		MOV64(keca31, t); \
		MOV64(t, keca10); \
		MOV64(keca10, keca41); \
		MOV64(keca41, keca42); \
		MOV64(keca42, keca20); \
		MOV64(keca20, keca32); \
		MOV64(keca32, keca34); \
		MOV64(keca34, keca40); \
		MOV64(keca40, keca14); \
		MOV64(keca14, keca13); \
		MOV64(keca13, keca30); \
		MOV64(keca30, keca23); \
		MOV64(keca23, keca21); \
		MOV64(keca21, t); \
	} while (0)

#define P4_TO_P0   do { \
		kekDECL64(t); \
		MOV64(t, keca01); \
		MOV64(keca01, keca12); \
		MOV64(keca12, keca11); \
		MOV64(keca11, keca04); \
		MOV64(keca04, keca43); \
		MOV64(keca43, keca44); \
		MOV64(keca44, t); \
		MOV64(t, keca02); \
		MOV64(keca02, keca24); \
		MOV64(keca24, keca22); \
		MOV64(keca22, keca03); \
		MOV64(keca03, keca31); \
		MOV64(keca31, keca33); \
		MOV64(keca33, t); \
		MOV64(t, keca10); \
		MOV64(keca10, keca42); \
		MOV64(keca42, keca32); \
		MOV64(keca32, keca40); \
		MOV64(keca40, keca13); \
		MOV64(keca13, keca23); \
		MOV64(keca23, t); \
		MOV64(t, keca14); \
		MOV64(keca14, keca30); \
		MOV64(keca30, keca21); \
		MOV64(keca21, keca41); \
		MOV64(keca41, keca20); \
		MOV64(keca20, keca34); \
		MOV64(keca34, t); \
	} while (0)

#define P6_TO_P0   do { \
		kekDECL64(t); \
		MOV64(t, keca01); \
		MOV64(keca01, keca02); \
		MOV64(keca02, keca04); \
		MOV64(keca04, keca03); \
		MOV64(keca03, t); \
		MOV64(t, keca10); \
		MOV64(keca10, keca20); \
		MOV64(keca20, keca40); \
		MOV64(keca40, keca30); \
		MOV64(keca30, t); \
		MOV64(t, keca11); \
		MOV64(keca11, keca22); \
		MOV64(keca22, keca44); \
		MOV64(keca44, keca33); \
		MOV64(keca33, t); \
		MOV64(t, keca12); \
		MOV64(keca12, keca24); \
		MOV64(keca24, keca43); \
		MOV64(keca43, keca31); \
		MOV64(keca31, t); \
		MOV64(t, keca13); \
		MOV64(keca13, keca21); \
		MOV64(keca21, keca42); \
		MOV64(keca42, keca34); \
		MOV64(keca34, t); \
		MOV64(t, keca14); \
		MOV64(keca14, keca23); \
		MOV64(keca23, keca41); \
		MOV64(keca41, keca32); \
		MOV64(keca32, t); \
	} while (0)

#define P8_TO_P0   do { \
		kekDECL64(t); \
		MOV64(t, keca01); \
		MOV64(keca01, keca11); \
		MOV64(keca11, keca43); \
		MOV64(keca43, t); \
		MOV64(t, keca02); \
		MOV64(keca02, keca22); \
		MOV64(keca22, keca31); \
		MOV64(keca31, t); \
		MOV64(t, keca03); \
		MOV64(keca03, keca33); \
		MOV64(keca33, keca24); \
		MOV64(keca24, t); \
		MOV64(t, keca04); \
		MOV64(keca04, keca44); \
		MOV64(keca44, keca12); \
		MOV64(keca12, t); \
		MOV64(t, keca10); \
		MOV64(keca10, keca32); \
		MOV64(keca32, keca13); \
		MOV64(keca13, t); \
		MOV64(t, keca14); \
		MOV64(keca14, keca21); \
		MOV64(keca21, keca20); \
		MOV64(keca20, t); \
		MOV64(t, keca23); \
		MOV64(keca23, keca42); \
		MOV64(keca42, keca40); \
		MOV64(keca40, t); \
		MOV64(t, keca30); \
		MOV64(keca30, keca41); \
		MOV64(keca41, keca34); \
		MOV64(keca34, t); \
	} while (0)

#define P12_TO_P0   do { \
		kekDECL64(t); \
		MOV64(t, keca01); \
		MOV64(keca01, keca04); \
		MOV64(keca04, t); \
		MOV64(t, keca02); \
		MOV64(keca02, keca03); \
		MOV64(keca03, t); \
		MOV64(t, keca10); \
		MOV64(keca10, keca40); \
		MOV64(keca40, t); \
		MOV64(t, keca11); \
		MOV64(keca11, keca44); \
		MOV64(keca44, t); \
		MOV64(t, keca12); \
		MOV64(keca12, keca43); \
		MOV64(keca43, t); \
		MOV64(t, keca13); \
		MOV64(keca13, keca42); \
		MOV64(keca42, t); \
		MOV64(t, keca14); \
		MOV64(keca14, keca41); \
		MOV64(keca41, t); \
		MOV64(t, keca20); \
		MOV64(keca20, keca30); \
		MOV64(keca30, t); \
		MOV64(t, keca21); \
		MOV64(keca21, keca34); \
		MOV64(keca34, t); \
		MOV64(t, keca22); \
		MOV64(keca22, keca33); \
		MOV64(keca33, t); \
		MOV64(t, keca23); \
		MOV64(keca23, keca32); \
		MOV64(keca32, t); \
		MOV64(t, keca24); \
		MOV64(keca24, keca31); \
		MOV64(keca31, t); \
	} while (0)

#define LPAR   (
#define RPAR   )

#define KF_ELT(r, s, k)   do { \
		THETA LPAR P ## r RPAR; \
		RHO LPAR P ## r RPAR; \
		KHI LPAR P ## s RPAR; \
		IOTA(k); \
	} while (0)

#define DO(x)   x

#define KECCAK_F_1600   DO(KECCAK_F_1600_)

/*
 * removed loop unrolling 
 * tested faster saving space
*/
#define KECCAK_F_1600_   do { \
		int j; \
		for (j = 0; j < 24; j += 4) { \
			KF_ELT( 0,  1, RC[j + 0]); \
			KF_ELT( 1,  2, RC[j + 1]); \
			KF_ELT( 2,  3, RC[j + 2]); \
			KF_ELT( 3,  4, RC[j + 3]); \
			P4_TO_P0; \
		} \
	} while (0)

/*
			KF_ELT( 0,  1, RC[j + 0]); \
			KF_ELT( 1,  2, RC[j + 1]); \
			KF_ELT( 2,  3, RC[j + 2]); \
			KF_ELT( 3,  4, RC[j + 3]); \
			KF_ELT( 4,  5, RC[j + 4]); \
			KF_ELT( 5,  6, RC[j + 5]); \
			KF_ELT( 6,  7, RC[j + 6]); \
			KF_ELT( 7,  8, RC[j + 7]); \
*/

	//kekDECL_STATE \
        
#define DECL_KEC  


/* 
	sph_u64 keca00, keca01, keca02, keca03, keca04; \
	sph_u64 keca10, keca11, keca12, keca13, keca14; \
	sph_u64 keca20, keca21, keca22, keca23, keca24; \
	sph_u64 keca30, keca31, keca32, keca33, keca34; \
	sph_u64 keca40, keca41, keca42, keca43, keca44;
*/

/* load initial constants */
#define KEC_I 

unsigned char keczword[8] = { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 }; \
/* load hash for loop */
#define KEC_U \
do { \
    /*memcpy(hashbuf, hash, 64); */ \
    memcpy(hash + 64, keczword, 8); \
} while (0); 

/* keccak512 hash loaded */
/* hash = keccak512(loaded */

#define KEC_C \
do { \
    kekDECL_STATE \
    unsigned char *buf = hash; \
    /*BEGIN CORE */ \
    kecINIT_STATE(); \
    KECCAK_F_1600; \
    /*END CORE */ \
    /* Finalize the "lane complement" */ \
    sph_enc64le_aligned((unsigned char*)(hash) +  0,  keca00); \
    sph_enc64le_aligned((unsigned char*)(hash) +  8, ~keca10); \
    sph_enc64le_aligned((unsigned char*)(hash) + 16, ~keca20); \
    sph_enc64le_aligned((unsigned char*)(hash) + 24,  keca30); \
    sph_enc64le_aligned((unsigned char*)(hash) + 32,  keca40); \
    sph_enc64le_aligned((unsigned char*)(hash) + 40,  keca01); \
    sph_enc64le_aligned((unsigned char*)(hash) + 48,  keca11); \
    sph_enc64le_aligned((unsigned char*)(hash) + 56,  keca21); \
} while (0);

#ifdef __cplusplus
}
#endif
