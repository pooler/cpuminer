/*
 * Copyright 2014-2015 pooler@litecoinpool.org
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.  See COPYING for more details.
 */

#include "cpuminer-config.h"

#if defined(USE_ASM) && (defined(__powerpc__) || defined(__ppc__) || defined(__PPC__))

#ifdef __APPLE__

#define HI(name) ha16(name)
#define LO(name) lo16(name)

#else

#define HI(name) name@ha
#define LO(name) name@l

#define r0 0
#define r1 1
#define r2 2
#define r3 3
#define r4 4
#define r5 5
#define r6 6
#define r7 7
#define r8 8
#define r9 9
#define r10 10
#define r11 11
#define r12 12
#define r13 13
#define r14 14
#define r15 15
#define r16 16
#define r17 17
#define r18 18
#define r19 19
#define r20 20
#define r21 21
#define r22 22
#define r23 23
#define r24 24
#define r25 25
#define r26 26
#define r27 27
#define r28 28
#define r29 29
#define r30 30
#define r31 31

#ifdef __ALTIVEC__
#define v0 0
#define v1 1
#define v2 2
#define v3 3
#define v4 4
#define v5 5
#define v6 6
#define v7 7
#define v8 8
#define v9 9
#define v10 10
#define v11 11
#define v12 12
#define v13 13
#define v14 14
#define v15 15
#define v16 16
#define v17 17
#define v18 18
#define v19 19
#define v20 20
#define v21 21
#define v22 22
#define v23 23
#define v24 24
#define v25 25
#define v26 26
#define v27 27
#define v28 28
#define v29 29
#define v30 30
#define v31 31
#endif

#endif

#if !(defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) || \
	defined(__64BIT__) || defined(_LP64) || defined(__LP64__))
#define ld lwz
#define std stw
#define stdu stwu
#define stdux stwux
#endif


#ifdef _AIX
	.csect .text[RO]
#else
	.data
#endif
	.align 2
sha256_h:
	.long 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a
	.long 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19

	.align 2
sha256_k:
	.long 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5
	.long 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5
	.long 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3
	.long 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174
	.long 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc
	.long 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da
	.long 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7
	.long 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967
	.long 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13
	.long 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85
	.long 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3
	.long 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070
	.long 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5
	.long 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3
	.long 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208
	.long 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2

#ifdef _AIX
	.toc
T.sha256_h:
	.tc sha256_h[TC], sha256_h
T.sha256_k:
	.tc sha256_k[TC], sha256_k
#endif


.macro sha256_extend_doubleround i, rw, wo, ra, rb, ry, rz
	lwz	r14, \wo+(\i+1)*4(\rw)
	rotrwi	r12, \ry, 17
	rotrwi	r13, \ry, 19
	add	r11, r11, \ra
	xor	r12, r12, r13
	srwi	r13, \ry, 10
	rotrwi	\ra, r14, 7
	xor	r12, r12, r13
	rotrwi	r13, r14, 18
	add	r12, r12, r11
	xor	\ra, \ra, r13
	srwi	r13, r14, 3
	lwz	r11, \wo+(\i+2)*4(\rw)
	xor	\ra, \ra, r13
	rotrwi	r13, \rz, 19
	add	\ra, \ra, r12

	rotrwi	r12, \rz, 17
	add	r14, r14, \rb
	xor	r12, r12, r13
	srwi	r13, \rz, 10
	rotrwi	\rb, r11, 7
	xor	r12, r12, r13
	rotrwi	r13, r11, 18
	stw	\ra, \wo+(\i+16)*4(\rw)
	xor	\rb, \rb, r13
	srwi	r13, r11, 3
	add	r14, r14, r12
	xor	\rb, \rb, r13
	add	\rb, \rb, r14
	stw	\rb, \wo+(\i+17)*4(\rw)
.endm


.macro sha256_main_round i, rk, rw, wo, ra, rb, rc, rd, re, rf, rg, rh
	lwz	r12, \wo+(\i)*4(\rw)
	and	r13, \rf, \re
	andc	r14, \rg, \re
	lwz	r15, (\i)*4(\rk)
	or	r14, r14, r13
	rotrwi	r13, \re, 5
	add	\rh, \rh, r14
	xor	r14, \re, r13
	rotrwi	r13, \re, 19
	add	\rh, \rh, r12
	xor	r14, r14, r13
	add	\rh, \rh, r15
	rotrwi	r13, r14, 6
	xor	r15, \ra, \rb
	add	\rh, \rh, r13

	rotrwi	r13, \ra, 11
	and	r15, r15, \rc
	xor	r12, \ra, r13
	rotrwi	r13, \ra, 20
	and	r14, \ra, \rb
	xor	r12, r12, r13
	xor	r14, r14, r15
	rotrwi	r13, r12, 2
	add	r15, \rh, r14
	add	\rh, \rh, \rd
	add	\rd, r15, r13
.endm

.macro sha256_main_quadround i, rk, rw, wo
	sha256_main_round \i+0, \rk, \rw, \wo, r4, r5, r6, r7, r8, r9, r10, r11
	sha256_main_round \i+1, \rk, \rw, \wo, r7, r4, r5, r6, r11, r8, r9, r10
	sha256_main_round \i+2, \rk, \rw, \wo, r6, r7, r4, r5, r10, r11, r8, r9
	sha256_main_round \i+3, \rk, \rw, \wo, r5, r6, r7, r4, r9, r10, r11, r8
.endm


#ifdef _AIX
	.csect .text[PR]
#else
	.text
#endif
	.align 2
	.globl sha256_transform
	.globl _sha256_transform
	.globl .sha256_transform
#ifdef __ELF__
	.type sha256_transform, %function
#endif
sha256_transform:
_sha256_transform:
.sha256_transform:
	stdu	r1, -76*4(r1)
	cmpwi	0, r5, 0
	std	r13, 2*4(r1)
	std	r14, 4*4(r1)
	std	r15, 6*4(r1)
	std	r16, 72*4(r1)
	
	bne	0, sha256_transform_swap
	
	lwz	r11, 0*4(r4)
	lwz	r14, 1*4(r4)
	lwz	r15, 2*4(r4)
	lwz	r7, 3*4(r4)
	lwz	r8, 4*4(r4)
	lwz	r9, 5*4(r4)
	lwz	r10, 6*4(r4)
	lwz	r0, 7*4(r4)
	lwz	r12, 8*4(r4)
	lwz	r13, 9*4(r4)
	lwz	r5, 10*4(r4)
	lwz	r6, 11*4(r4)
	stw	r11, 8*4+0*4(r1)
	stw	r14, 8*4+1*4(r1)
	stw	r15, 8*4+2*4(r1)
	stw	r7, 8*4+3*4(r1)
	stw	r8, 8*4+4*4(r1)
	stw	r9, 8*4+5*4(r1)
	stw	r10, 8*4+6*4(r1)
	stw	r0, 8*4+7*4(r1)
	stw	r12, 8*4+8*4(r1)
	stw	r13, 8*4+9*4(r1)
	stw	r5, 8*4+10*4(r1)
	stw	r6, 8*4+11*4(r1)
	lwz	r7, 12*4(r4)
	lwz	r8, 13*4(r4)
	lwz	r9, 14*4(r4)
	lwz	r10, 15*4(r4)
	mr	r4, r13
	stw	r7, 8*4+12*4(r1)
	stw	r8, 8*4+13*4(r1)
	stw	r9, 8*4+14*4(r1)
	stw	r10, 8*4+15*4(r1)
	b	sha256_transform_extend
	
sha256_transform_swap:
	li	r13, 1*4
	li	r14, 2*4
	li	r15, 3*4
	lwbrx	r11, 0, r4
	lwbrx	r7, r4, r13
	lwbrx	r8, r4, r14
	lwbrx	r9, r4, r15
	addi	r4, r4, 4*4
	stw	r11, 8*4+0*4(r1)
	stw	r7, 8*4+1*4(r1)
	stw	r8, 8*4+2*4(r1)
	stw	r9, 8*4+3*4(r1)
	lwbrx	r7, 0, r4
	lwbrx	r8, r4, r13
	lwbrx	r9, r4, r14
	lwbrx	r10, r4, r15
	addi	r4, r4, 4*4
	stw	r7, 8*4+4*4(r1)
	stw	r8, 8*4+5*4(r1)
	stw	r9, 8*4+6*4(r1)
	stw	r10, 8*4+7*4(r1)
	lwbrx	r8, 0, r4
	lwbrx	r12, r4, r13
	lwbrx	r5, r4, r14
	lwbrx	r6, r4, r15
	addi	r4, r4, 4*4
	stw	r8, 8*4+8*4(r1)
	stw	r12, 8*4+9*4(r1)
	stw	r5, 8*4+10*4(r1)
	stw	r6, 8*4+11*4(r1)
	lwbrx	r7, 0, r4
	lwbrx	r8, r4, r13
	lwbrx	r9, r4, r14
	lwbrx	r10, r4, r15
	mr	r4, r12
	stw	r7, 8*4+12*4(r1)
	stw	r8, 8*4+13*4(r1)
	stw	r9, 8*4+14*4(r1)
	stw	r10, 8*4+15*4(r1)
	
sha256_transform_extend:
	sha256_extend_doubleround  0, r1, 8*4, r4, r5, r9, r10
	sha256_extend_doubleround  2, r1, 8*4, r6, r7, r4, r5
	sha256_extend_doubleround  4, r1, 8*4, r8, r9, r6, r7
	sha256_extend_doubleround  6, r1, 8*4, r10, r4, r8, r9
	sha256_extend_doubleround  8, r1, 8*4, r5, r6, r10, r4
	sha256_extend_doubleround 10, r1, 8*4, r7, r8, r5, r6
	sha256_extend_doubleround 12, r1, 8*4, r9, r10, r7, r8
	sha256_extend_doubleround 14, r1, 8*4, r4, r5, r9, r10
	sha256_extend_doubleround 16, r1, 8*4, r6, r7, r4, r5
	sha256_extend_doubleround 18, r1, 8*4, r8, r9, r6, r7
	sha256_extend_doubleround 20, r1, 8*4, r10, r4, r8, r9
	sha256_extend_doubleround 22, r1, 8*4, r5, r6, r10, r4
	sha256_extend_doubleround 24, r1, 8*4, r7, r8, r5, r6
	sha256_extend_doubleround 26, r1, 8*4, r9, r10, r7, r8
	sha256_extend_doubleround 28, r1, 8*4, r4, r5, r9, r10
	sha256_extend_doubleround 30, r1, 8*4, r6, r7, r4, r5
	sha256_extend_doubleround 32, r1, 8*4, r8, r9, r6, r7
	sha256_extend_doubleround 34, r1, 8*4, r10, r4, r8, r9
	sha256_extend_doubleround 36, r1, 8*4, r5, r6, r10, r4
	sha256_extend_doubleround 38, r1, 8*4, r7, r8, r5, r6
	sha256_extend_doubleround 40, r1, 8*4, r9, r10, r7, r8
	sha256_extend_doubleround 42, r1, 8*4, r4, r5, r9, r10
	sha256_extend_doubleround 44, r1, 8*4, r6, r7, r4, r5
	sha256_extend_doubleround 46, r1, 8*4, r8, r9, r6, r7
	
	lwz	r4, 0*4(r3)
	lwz	r5, 1*4(r3)
	lwz	r6, 2*4(r3)
	lwz	r7, 3*4(r3)
	lwz	r8, 4*4(r3)
	lwz	r9, 5*4(r3)
	lwz	r10, 6*4(r3)
	lwz	r11, 7*4(r3)
#ifdef _AIX
	ld	r16, T.sha256_k(r2)
#else
	lis	r16, HI(sha256_k)
	addi	r16, r16, LO(sha256_k)
#endif
	sha256_main_quadround  0, r16, r1, 8*4
	sha256_main_quadround  4, r16, r1, 8*4
	sha256_main_quadround  8, r16, r1, 8*4
	sha256_main_quadround 12, r16, r1, 8*4
	sha256_main_quadround 16, r16, r1, 8*4
	sha256_main_quadround 20, r16, r1, 8*4
	sha256_main_quadround 24, r16, r1, 8*4
	sha256_main_quadround 28, r16, r1, 8*4
	sha256_main_quadround 32, r16, r1, 8*4
	sha256_main_quadround 36, r16, r1, 8*4
	sha256_main_quadround 40, r16, r1, 8*4
	sha256_main_quadround 44, r16, r1, 8*4
	sha256_main_quadround 48, r16, r1, 8*4
	sha256_main_quadround 52, r16, r1, 8*4
	sha256_main_quadround 56, r16, r1, 8*4
	sha256_main_quadround 60, r16, r1, 8*4
	
	lwz	r12, 0*4(r3)
	lwz	r13, 1*4(r3)
	lwz	r14, 2*4(r3)
	lwz	r15, 3*4(r3)
	add	r4, r4, r12
	add	r5, r5, r13
	add	r6, r6, r14
	add	r7, r7, r15
	stw	r4, 0*4(r3)
	stw	r5, 1*4(r3)
	stw	r6, 2*4(r3)
	stw	r7, 3*4(r3)
	lwz	r12, 4*4(r3)
	lwz	r13, 5*4(r3)
	lwz	r14, 6*4(r3)
	lwz	r15, 7*4(r3)
	add	r8, r8, r12
	add	r9, r9, r13
	add	r10, r10, r14
	add	r11, r11, r15
	stw	r8, 4*4(r3)
	stw	r9, 5*4(r3)
	stw	r10, 6*4(r3)
	stw	r11, 7*4(r3)
	
	ld	r13, 2*4(r1)
	ld	r14, 4*4(r1)
	ld	r15, 6*4(r1)
	ld	r16, 72*4(r1)
	addi	r1, r1, 76*4
	blr


	.align 2
	.globl sha256d_ms
	.globl _sha256d_ms
	.globl .sha256d_ms
#ifdef __ELF__
	.type sha256d_ms, %function
#endif
sha256d_ms:
_sha256d_ms:
.sha256d_ms:
	stdu	r1, -80*4(r1)
	std	r13, 2*4(r1)
	std	r14, 4*4(r1)
	std	r15, 6*4(r1)
	std	r16, 72*4(r1)
	std	r17, 74*4(r1)
	std	r18, 76*4(r1)
	
	mr	r17, r4
	mr	r18, r5
	mr	r16, r6
	
	lwz	r14, 3*4(r17)
	lwz	r6, 18*4(r17)
	lwz	r7, 19*4(r17)
	
	rotrwi	r12, r14, 7
	rotrwi	r13, r14, 18
	stw	r6, 8*4+18*4(r1)
	xor	r12, r12, r13
	srwi	r13, r14, 3
	stw	r7, 8*4+19*4(r1)
	xor	r12, r12, r13
	lwz	r8, 20*4(r17)
	add	r6, r6, r12
	lwz	r10, 22*4(r17)
	add	r7, r7, r14
	stw	r6, 18*4(r17)
	
	rotrwi	r12, r6, 17
	rotrwi	r13, r6, 19
	stw	r7, 19*4(r17)
	xor	r12, r12, r13
	srwi	r13, r6, 10
	stw	r8, 8*4+20*4(r1)
	xor	r12, r12, r13
	lwz	r4, 23*4(r17)
	add	r8, r8, r12
	lwz	r5, 24*4(r17)
	
	rotrwi	r9, r7, 17
	rotrwi	r13, r7, 19
	stw	r8, 20*4(r17)
	xor	r9, r9, r13
	srwi	r13, r7, 10
	stw	r10, 8*4+21*4(r1)
	xor	r9, r9, r13
	stw	r4, 8*4+22*4(r1)
	
	rotrwi	r12, r8, 17
	rotrwi	r13, r8, 19
	stw	r9, 21*4(r17)
	xor	r12, r12, r13
	srwi	r13, r8, 10
	stw	r5, 8*4+23*4(r1)
	xor	r12, r12, r13
	rotrwi	r14, r9, 17
	rotrwi	r13, r9, 19
	add	r10, r10, r12
	lwz	r11, 30*4(r17)
	
	xor	r14, r14, r13
	srwi	r13, r9, 10
	stw	r10, 22*4(r17)
	xor	r14, r14, r13
	stw	r11, 8*4+24*4(r1)
	add	r4, r4, r14
	
	rotrwi	r12, r10, 17
	rotrwi	r13, r10, 19
	stw	r4, 23*4(r17)
	xor	r12, r12, r13
	srwi	r13, r10, 10
	rotrwi	r14, r4, 17
	xor	r12, r12, r13
	rotrwi	r13, r4, 19
	xor	r14, r14, r13
	srwi	r13, r4, 10
	add	r5, r5, r12
	xor	r14, r14, r13
	stw	r5, 24*4(r17)
	add	r6, r6, r14
	
	rotrwi	r12, r5, 17
	rotrwi	r13, r5, 19
	stw	r6, 25*4(r17)
	xor	r12, r12, r13
	srwi	r13, r5, 10
	rotrwi	r14, r6, 17
	xor	r12, r12, r13
	rotrwi	r13, r6, 19
	xor	r14, r14, r13
	srwi	r13, r6, 10
	add	r7, r7, r12
	xor	r14, r14, r13
	stw	r7, 26*4(r17)
	add	r8, r8, r14
	
	rotrwi	r12, r7, 17
	rotrwi	r13, r7, 19
	stw	r8, 27*4(r17)
	xor	r12, r12, r13
	srwi	r13, r7, 10
	rotrwi	r14, r8, 17
	xor	r12, r12, r13
	rotrwi	r13, r8, 19
	xor	r14, r14, r13
	srwi	r13, r8, 10
	add	r9, r9, r12
	xor	r14, r14, r13
	stw	r9, 28*4(r17)
	add	r10, r10, r14
	
	lwz	r14, 31*4(r17)
	rotrwi	r12, r9, 17
	rotrwi	r13, r9, 19
	stw	r10, 29*4(r17)
	xor	r12, r12, r13
	srwi	r13, r9, 10
	stw	r14, 8*4+25*4(r1)
	xor	r12, r12, r13
	add	r11, r11, r12
	add	r5, r5, r14
	rotrwi	r12, r10, 17
	rotrwi	r13, r10, 19
	add	r4, r4, r11
	
	lwz	r11, 16*4(r17)
	xor	r12, r12, r13
	srwi	r13, r10, 10
	stw	r4, 30*4(r17)
	xor	r12, r12, r13
	add	r5, r5, r12
	stw	r5, 31*4(r17)
	
	sha256_extend_doubleround 16, r17, 0, r6, r7, r4, r5
	sha256_extend_doubleround 18, r17, 0, r8, r9, r6, r7
	sha256_extend_doubleround 20, r17, 0, r10, r4, r8, r9
	sha256_extend_doubleround 22, r17, 0, r5, r6, r10, r4
	sha256_extend_doubleround 24, r17, 0, r7, r8, r5, r6
	sha256_extend_doubleround 26, r17, 0, r9, r10, r7, r8
	sha256_extend_doubleround 28, r17, 0, r4, r5, r9, r10
	sha256_extend_doubleround 30, r17, 0, r6, r7, r4, r5
	sha256_extend_doubleround 32, r17, 0, r8, r9, r6, r7
	sha256_extend_doubleround 34, r17, 0, r10, r4, r8, r9
	sha256_extend_doubleround 36, r17, 0, r5, r6, r10, r4
	sha256_extend_doubleround 38, r17, 0, r7, r8, r5, r6
	sha256_extend_doubleround 40, r17, 0, r9, r10, r7, r8
	sha256_extend_doubleround 42, r17, 0, r4, r5, r9, r10
	sha256_extend_doubleround 44, r17, 0, r6, r7, r4, r5
	sha256_extend_doubleround 46, r17, 0, r8, r9, r6, r7
	
	lwz	r4,  0*4(r16)
	lwz	r9,  1*4(r16)
	lwz	r10, 2*4(r16)
	lwz	r11, 3*4(r16)
	lwz	r8,  4*4(r16)
	lwz	r5,  5*4(r16)
	lwz	r6,  6*4(r16)
	lwz	r7,  7*4(r16)
#ifdef _AIX
	ld	r16, T.sha256_k(r2)
#else
	lis	r16, HI(sha256_k)
	addi	r16, r16, LO(sha256_k)
#endif
	
	sha256_main_round  3, r16, r17, 0, r5, r6, r7, r4, r9, r10, r11, r8
	sha256_main_quadround  4, r16, r17, 0
	sha256_main_quadround  8, r16, r17, 0
	sha256_main_quadround 12, r16, r17, 0
	sha256_main_quadround 16, r16, r17, 0
	sha256_main_quadround 20, r16, r17, 0
	sha256_main_quadround 24, r16, r17, 0
	sha256_main_quadround 28, r16, r17, 0
	sha256_main_quadround 32, r16, r17, 0
	sha256_main_quadround 36, r16, r17, 0
	sha256_main_quadround 40, r16, r17, 0
	sha256_main_quadround 44, r16, r17, 0
	sha256_main_quadround 48, r16, r17, 0
	sha256_main_quadround 52, r16, r17, 0
	sha256_main_quadround 56, r16, r17, 0
	sha256_main_quadround 60, r16, r17, 0
	
	lwz	r12, 0*4(r18)
	lwz	r13, 1*4(r18)
	lwz	r14, 2*4(r18)
	lwz	r15, 3*4(r18)
	add	r4, r4, r12
	add	r5, r5, r13
	add	r6, r6, r14
	add	r7, r7, r15
	stw	r4, 8*4+0*4(r1)
	stw	r5, 8*4+1*4(r1)
	stw	r6, 8*4+2*4(r1)
	stw	r7, 8*4+3*4(r1)
	lwz	r12, 4*4(r18)
	lwz	r13, 5*4(r18)
	lwz	r14, 6*4(r18)
	lwz	r15, 7*4(r18)
	add	r8, r8, r12
	add	r9, r9, r13
	add	r10, r10, r14
	add	r11, r11, r15
	stw	r8, 8*4+4*4(r1)
	stw	r9, 8*4+5*4(r1)
	stw	r10, 8*4+6*4(r1)
	stw	r11, 8*4+7*4(r1)

	lwz	r4, 8*4+18*4(r1)
	lwz	r5, 8*4+19*4(r1)
	lwz	r6, 8*4+20*4(r1)
	lwz	r7, 8*4+21*4(r1)
	lwz	r8, 8*4+22*4(r1)
	lwz	r9, 8*4+23*4(r1)
	lwz	r10, 8*4+24*4(r1)
	lwz	r11, 8*4+25*4(r1)
	stw	r4,  18*4(r17)
	stw	r5,  19*4(r17)
	stw	r6,  20*4(r17)
	stw	r7,  22*4(r17)
	stw	r8,  23*4(r17)
	stw	r9,  24*4(r17)
	stw	r10, 30*4(r17)
	stw	r11, 31*4(r17)
	
	lis	r8, 0x8000
	li	r9,  0
	li	r10, 0x0100
	
	lwz	r14, 8*4+1*4(r1)
	lwz	r4, 8*4+0*4(r1)
	
	lwz	r11, 8*4+2*4(r1)
	rotrwi	r12, r14, 7
	rotrwi	r13, r14, 18
	
	stw	r8, 8*4+8*4(r1)
	stw	r9, 8*4+9*4(r1)
	stw	r9, 8*4+10*4(r1)
	stw	r9, 8*4+11*4(r1)
	stw	r9, 8*4+12*4(r1)
	stw	r9, 8*4+13*4(r1)
	stw	r9, 8*4+14*4(r1)
	stw	r10, 8*4+15*4(r1)
	
	xor	r12, r12, r13
	srwi	r13, r14, 3
	addis	r5, r14, 0x00a0
	xor	r12, r12, r13
	rotrwi	r14, r11, 7
	rotrwi	r13, r11, 18
	add	r4, r4, r12
	xor	r14, r14, r13
	srwi	r13, r11, 3
	stw	r4, 8*4+16*4(r1)
	xor	r14, r14, r13
	rotrwi	r12, r4, 17
	rotrwi	r13, r4, 19
	add	r5, r5, r14
	lwz	r14, 8*4+3*4(r1)
	
	stw	r5, 8*4+17*4(r1)
	xor	r12, r12, r13
	srwi	r13, r4, 10
	rotrwi	r6, r14, 7
	xor	r12, r12, r13
	rotrwi	r13, r14, 18
	xor	r6, r6, r13
	srwi	r13, r14, 3
	add	r11, r11, r12
	xor	r6, r6, r13
	rotrwi	r12, r5, 17
	rotrwi	r13, r5, 19
	add	r6, r6, r11
	lwz	r11, 8*4+4*4(r1)
	
	stw	r6, 8*4+18*4(r1)
	xor	r12, r12, r13
	srwi	r13, r5, 10
	rotrwi	r7, r11, 7
	xor	r12, r12, r13
	rotrwi	r13, r11, 18
	xor	r7, r7, r13
	srwi	r13, r11, 3
	add	r14, r14, r12
	xor	r7, r7, r13
	rotrwi	r12, r6, 17
	rotrwi	r13, r6, 19
	add	r7, r7, r14
	lwz	r14, 8*4+5*4(r1)
	
	stw	r7, 8*4+19*4(r1)
	xor	r12, r12, r13
	srwi	r13, r6, 10
	rotrwi	r8, r14, 7
	xor	r12, r12, r13
	rotrwi	r13, r14, 18
	xor	r8, r8, r13
	srwi	r13, r14, 3
	add	r11, r11, r12
	xor	r8, r8, r13
	rotrwi	r12, r7, 17
	rotrwi	r13, r7, 19
	add	r8, r8, r11
	lwz	r11, 8*4+6*4(r1)
	
	stw	r8, 8*4+20*4(r1)
	xor	r12, r12, r13
	srwi	r13, r7, 10
	rotrwi	r9, r11, 7
	xor	r12, r12, r13
	rotrwi	r13, r11, 18
	xor	r9, r9, r13
	srwi	r13, r11, 3
	add	r14, r14, r12
	xor	r9, r9, r13
	rotrwi	r12, r8, 17
	rotrwi	r13, r8, 19
	add	r9, r9, r14
	lwz	r14, 8*4+7*4(r1)
	
	stw	r9, 8*4+21*4(r1)
	xor	r12, r12, r13
	srwi	r13, r8, 10
	rotrwi	r10, r14, 7
	xor	r12, r12, r13
	rotrwi	r13, r14, 18
	xor	r10, r10, r13
	srwi	r13, r14, 3
	add	r11, r11, r12
	xor	r10, r10, r13
	rotrwi	r12, r9, 17
	rotrwi	r13, r9, 19
	addi	r11, r11, 0x0100
	add	r14, r14, r4
	add	r10, r10, r11
	
	xor	r12, r12, r13
	srwi	r13, r9, 10
	stw	r10, 8*4+22*4(r1)
	addis	r14, r14, 0x1100
	xor	r12, r12, r13
	add	r14, r14, r12
	rotrwi	r12, r10, 17
	rotrwi	r13, r10, 19
	addi	r4, r14, 0x2000
	xor	r12, r12, r13
	srwi	r13, r10, 10
	stw	r4, 8*4+23*4(r1)
	addis	r5, r5, 0x8000
	xor	r12, r12, r13
	add	r5, r5, r12

	rotrwi	r12, r4, 17
	rotrwi	r13, r4, 19
	stw	r5, 8*4+24*4(r1)
	xor	r12, r12, r13
	srwi	r13, r4, 10
	rotrwi	r11, r5, 17
	xor	r12, r12, r13
	rotrwi	r13, r5, 19
	xor	r11, r11, r13
	srwi	r13, r5, 10
	add	r6, r6, r12
	xor	r11, r11, r13
	stw	r6, 8*4+25*4(r1)
	add	r7, r7, r11
	
	rotrwi	r12, r6, 17
	rotrwi	r13, r6, 19
	stw	r7, 8*4+26*4(r1)
	xor	r12, r12, r13
	srwi	r13, r6, 10
	rotrwi	r11, r7, 17
	xor	r12, r12, r13
	rotrwi	r13, r7, 19
	xor	r11, r11, r13
	srwi	r13, r7, 10
	add	r8, r8, r12
	xor	r11, r11, r13
	stw	r8, 8*4+27*4(r1)
	add	r9, r9, r11
	
	rotrwi	r14, r8, 17
	rotrwi	r13, r8, 19
	rotrwi	r12, r9, 17
	stw	r9, 8*4+28*4(r1)
	addis	r4, r4, 0x0040
	xor	r14, r14, r13
	rotrwi	r13, r9, 19
	xor	r12, r12, r13
	srwi	r13, r8, 10
	xor	r14, r14, r13
	srwi	r13, r9, 10
	xor	r12, r12, r13
	addi	r4, r4, 0x0022
	add	r10, r10, r14
	add	r4, r4, r12
	lwz	r11, 8*4+16*4(r1)
	
	addi	r5, r5, 0x0100
	stw	r4, 8*4+30*4(r1)
	rotrwi	r14, r11, 7
	stw	r10, 8*4+29*4(r1)
	rotrwi	r13, r11, 18
	rotrwi	r12, r10, 17
	xor	r14, r14, r13
	rotrwi	r13, r10, 19
	xor	r12, r12, r13
	srwi	r13, r11, 3
	xor	r14, r14, r13
	srwi	r13, r10, 10
	xor	r12, r12, r13
	add	r5, r5, r14
	add	r5, r5, r12
	stw	r5, 8*4+31*4(r1)
	
	sha256_extend_doubleround 16, r1, 8*4, r6, r7, r4, r5
	sha256_extend_doubleround 18, r1, 8*4, r8, r9, r6, r7
	sha256_extend_doubleround 20, r1, 8*4, r10, r4, r8, r9
	sha256_extend_doubleround 22, r1, 8*4, r5, r6, r10, r4
	sha256_extend_doubleround 24, r1, 8*4, r7, r8, r5, r6
	sha256_extend_doubleround 26, r1, 8*4, r9, r10, r7, r8
	sha256_extend_doubleround 28, r1, 8*4, r4, r5, r9, r10
	sha256_extend_doubleround 30, r1, 8*4, r6, r7, r4, r5
	sha256_extend_doubleround 32, r1, 8*4, r8, r9, r6, r7
	sha256_extend_doubleround 34, r1, 8*4, r10, r4, r8, r9
	sha256_extend_doubleround 36, r1, 8*4, r5, r6, r10, r4
	sha256_extend_doubleround 38, r1, 8*4, r7, r8, r5, r6
	sha256_extend_doubleround 40, r1, 8*4, r9, r10, r7, r8
	sha256_extend_doubleround 42, r1, 8*4, r4, r5, r9, r10
	
#ifdef _AIX
	ld	r18, T.sha256_h(r2)
#else
	lis	r18, HI(sha256_h)
	addi	r18, r18, LO(sha256_h)
#endif
	
	lwz	r14, 8*4+(44+1)*4(r1)
	rotrwi	r12, r4, 17
	rotrwi	r13, r4, 19
	add	r15, r11, r6
	rotrwi	r6, r14, 7
	rotrwi	r11, r14, 18
	xor	r12, r12, r13
	xor	r6, r6, r11
	
	lwz	r8, 4*4(r18)
	lwz	r9, 5*4(r18)
	lwz	r10, 6*4(r18)
	lwz	r11, 7*4(r18)
	
	srwi	r13, r4, 10
	srwi	r14, r14, 3
	xor	r12, r12, r13
	xor	r6, r6, r14
	add	r12, r12, r15
	add	r6, r6, r12
	stw	r6, 8*4+(44+16)*4(r1)
	
	lwz	r4, 0*4(r18)
	lwz	r5, 1*4(r18)
	lwz	r6, 2*4(r18)
	lwz	r7, 3*4(r18)
	
	sha256_main_quadround  0, r16, r1, 8*4
	sha256_main_quadround  4, r16, r1, 8*4
	sha256_main_quadround  8, r16, r1, 8*4
	sha256_main_quadround 12, r16, r1, 8*4
	sha256_main_quadround 16, r16, r1, 8*4
	sha256_main_quadround 20, r16, r1, 8*4
	sha256_main_quadround 24, r16, r1, 8*4
	sha256_main_quadround 28, r16, r1, 8*4
	sha256_main_quadround 32, r16, r1, 8*4
	sha256_main_quadround 36, r16, r1, 8*4
	sha256_main_quadround 40, r16, r1, 8*4
	sha256_main_quadround 44, r16, r1, 8*4
	sha256_main_quadround 48, r16, r1, 8*4
	sha256_main_quadround 52, r16, r1, 8*4
	sha256_main_round 56, r16, r1, 8*4, r4, r5, r6, r7, r8, r9, r10, r11

.macro sha256_main_round_red i, rk, rw, wo, rd, re, rf, rg, rh
	lwz	r12, \wo+(\i)*4(\rw)
	and	r15, \rf, \re
	andc	r14, \rg, \re
	add	\rh, \rh, \rd
	or	r14, r14, r15
	lwz	r15, (\i)*4(\rk)
	rotrwi	r13, \re, 5
	add	\rh, \rh, r14
	xor	r14, \re, r13
	rotrwi	r13, \re, 19
	add	\rh, \rh, r12
	xor	r14, r14, r13
	add	\rh, \rh, r15
	rotrwi	r13, r14, 6
	add	\rh, \rh, r13
.endm
	
	sha256_main_round_red 57, r16, r1, 8*4, r6, r11, r8, r9, r10
	sha256_main_round_red 58, r16, r1, 8*4, r5, r10, r11, r8, r9
	sha256_main_round_red 59, r16, r1, 8*4, r4, r9, r10, r11, r8
	lwz	r5, 7*4(r18)
	sha256_main_round_red 60, r16, r1, 8*4, r7, r8, r9, r10, r11
	
	add	r11, r11, r5
	stw	r11, 7*4(r3)
	
	ld	r13, 2*4(r1)
	ld	r14, 4*4(r1)
	ld	r15, 6*4(r1)
	ld	r16, 72*4(r1)
	ld	r17, 74*4(r1)
	ld	r18, 76*4(r1)
	addi	r1, r1, 80*4
	blr


#ifdef __ALTIVEC__

#ifdef __APPLE__
	.machine ppc7400
#endif

#ifdef _AIX
	.csect .text[RO]
#else
	.data
#endif
	.align 4
sha256_4h:
	.long 0x6a09e667, 0x6a09e667, 0x6a09e667, 0x6a09e667
	.long 0xbb67ae85, 0xbb67ae85, 0xbb67ae85, 0xbb67ae85
	.long 0x3c6ef372, 0x3c6ef372, 0x3c6ef372, 0x3c6ef372
	.long 0xa54ff53a, 0xa54ff53a, 0xa54ff53a, 0xa54ff53a
	.long 0x510e527f, 0x510e527f, 0x510e527f, 0x510e527f
	.long 0x9b05688c, 0x9b05688c, 0x9b05688c, 0x9b05688c
	.long 0x1f83d9ab, 0x1f83d9ab, 0x1f83d9ab, 0x1f83d9ab
	.long 0x5be0cd19, 0x5be0cd19, 0x5be0cd19, 0x5be0cd19

	.align 4
sha256_4k:
	.long 0x428a2f98, 0x428a2f98, 0x428a2f98, 0x428a2f98
	.long 0x71374491, 0x71374491, 0x71374491, 0x71374491
	.long 0xb5c0fbcf, 0xb5c0fbcf, 0xb5c0fbcf, 0xb5c0fbcf
	.long 0xe9b5dba5, 0xe9b5dba5, 0xe9b5dba5, 0xe9b5dba5
	.long 0x3956c25b, 0x3956c25b, 0x3956c25b, 0x3956c25b
	.long 0x59f111f1, 0x59f111f1, 0x59f111f1, 0x59f111f1
	.long 0x923f82a4, 0x923f82a4, 0x923f82a4, 0x923f82a4
	.long 0xab1c5ed5, 0xab1c5ed5, 0xab1c5ed5, 0xab1c5ed5
	.long 0xd807aa98, 0xd807aa98, 0xd807aa98, 0xd807aa98
	.long 0x12835b01, 0x12835b01, 0x12835b01, 0x12835b01
	.long 0x243185be, 0x243185be, 0x243185be, 0x243185be
	.long 0x550c7dc3, 0x550c7dc3, 0x550c7dc3, 0x550c7dc3
	.long 0x72be5d74, 0x72be5d74, 0x72be5d74, 0x72be5d74
	.long 0x80deb1fe, 0x80deb1fe, 0x80deb1fe, 0x80deb1fe
	.long 0x9bdc06a7, 0x9bdc06a7, 0x9bdc06a7, 0x9bdc06a7
	.long 0xc19bf174, 0xc19bf174, 0xc19bf174, 0xc19bf174
	.long 0xe49b69c1, 0xe49b69c1, 0xe49b69c1, 0xe49b69c1
	.long 0xefbe4786, 0xefbe4786, 0xefbe4786, 0xefbe4786
	.long 0x0fc19dc6, 0x0fc19dc6, 0x0fc19dc6, 0x0fc19dc6
	.long 0x240ca1cc, 0x240ca1cc, 0x240ca1cc, 0x240ca1cc
	.long 0x2de92c6f, 0x2de92c6f, 0x2de92c6f, 0x2de92c6f
	.long 0x4a7484aa, 0x4a7484aa, 0x4a7484aa, 0x4a7484aa
	.long 0x5cb0a9dc, 0x5cb0a9dc, 0x5cb0a9dc, 0x5cb0a9dc
	.long 0x76f988da, 0x76f988da, 0x76f988da, 0x76f988da
	.long 0x983e5152, 0x983e5152, 0x983e5152, 0x983e5152
	.long 0xa831c66d, 0xa831c66d, 0xa831c66d, 0xa831c66d
	.long 0xb00327c8, 0xb00327c8, 0xb00327c8, 0xb00327c8
	.long 0xbf597fc7, 0xbf597fc7, 0xbf597fc7, 0xbf597fc7
	.long 0xc6e00bf3, 0xc6e00bf3, 0xc6e00bf3, 0xc6e00bf3
	.long 0xd5a79147, 0xd5a79147, 0xd5a79147, 0xd5a79147
	.long 0x06ca6351, 0x06ca6351, 0x06ca6351, 0x06ca6351
	.long 0x14292967, 0x14292967, 0x14292967, 0x14292967
	.long 0x27b70a85, 0x27b70a85, 0x27b70a85, 0x27b70a85
	.long 0x2e1b2138, 0x2e1b2138, 0x2e1b2138, 0x2e1b2138
	.long 0x4d2c6dfc, 0x4d2c6dfc, 0x4d2c6dfc, 0x4d2c6dfc
	.long 0x53380d13, 0x53380d13, 0x53380d13, 0x53380d13
	.long 0x650a7354, 0x650a7354, 0x650a7354, 0x650a7354
	.long 0x766a0abb, 0x766a0abb, 0x766a0abb, 0x766a0abb
	.long 0x81c2c92e, 0x81c2c92e, 0x81c2c92e, 0x81c2c92e
	.long 0x92722c85, 0x92722c85, 0x92722c85, 0x92722c85
	.long 0xa2bfe8a1, 0xa2bfe8a1, 0xa2bfe8a1, 0xa2bfe8a1
	.long 0xa81a664b, 0xa81a664b, 0xa81a664b, 0xa81a664b
	.long 0xc24b8b70, 0xc24b8b70, 0xc24b8b70, 0xc24b8b70
	.long 0xc76c51a3, 0xc76c51a3, 0xc76c51a3, 0xc76c51a3
	.long 0xd192e819, 0xd192e819, 0xd192e819, 0xd192e819
	.long 0xd6990624, 0xd6990624, 0xd6990624, 0xd6990624
	.long 0xf40e3585, 0xf40e3585, 0xf40e3585, 0xf40e3585
	.long 0x106aa070, 0x106aa070, 0x106aa070, 0x106aa070
	.long 0x19a4c116, 0x19a4c116, 0x19a4c116, 0x19a4c116
	.long 0x1e376c08, 0x1e376c08, 0x1e376c08, 0x1e376c08
	.long 0x2748774c, 0x2748774c, 0x2748774c, 0x2748774c
	.long 0x34b0bcb5, 0x34b0bcb5, 0x34b0bcb5, 0x34b0bcb5
	.long 0x391c0cb3, 0x391c0cb3, 0x391c0cb3, 0x391c0cb3
	.long 0x4ed8aa4a, 0x4ed8aa4a, 0x4ed8aa4a, 0x4ed8aa4a
	.long 0x5b9cca4f, 0x5b9cca4f, 0x5b9cca4f, 0x5b9cca4f
	.long 0x682e6ff3, 0x682e6ff3, 0x682e6ff3, 0x682e6ff3
	.long 0x748f82ee, 0x748f82ee, 0x748f82ee, 0x748f82ee
	.long 0x78a5636f, 0x78a5636f, 0x78a5636f, 0x78a5636f
	.long 0x84c87814, 0x84c87814, 0x84c87814, 0x84c87814
	.long 0x8cc70208, 0x8cc70208, 0x8cc70208, 0x8cc70208
	.long 0x90befffa, 0x90befffa, 0x90befffa, 0x90befffa
	.long 0xa4506ceb, 0xa4506ceb, 0xa4506ceb, 0xa4506ceb
	.long 0xbef9a3f7, 0xbef9a3f7, 0xbef9a3f7, 0xbef9a3f7
	.long 0xc67178f2, 0xc67178f2, 0xc67178f2, 0xc67178f2

	.align 4
sha256d_4preext2:
	.long 0x00a00000, 0x00a00000, 0x00a00000, 0x00a00000
	.long 0x11002000, 0x11002000, 0x11002000, 0x11002000
	.long 0x80000000, 0x80000000, 0x80000000, 0x80000000
	.long 0x00400022, 0x00400022, 0x00400022, 0x00400022

	.align 4
br_perm:
	.long 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c

#ifdef _AIX
	.toc
T.sha256_4h:
	.tc sha256_4h[TC], sha256_4h
T.sha256_4k:
	.tc sha256_4k[TC], sha256_4k
T.sha256d_4preext2:
	.tc sha256d_4preext2[TC], sha256d_4preext2
T.br_perm:
	.tc br_perm[TC], br_perm
#endif


.macro sha256_4way_extend_setup
	vspltisw	v0, 10
	vspltisw	v1, -7
	vspltisw	v16, 3
	vspltisw	v17, 15
	vspltisw	v18, 14
	vspltisw	v19, 13
.endm

.macro sha256_4way_extend_doubleround i, rw, va, vb, vy, vz
	lvx	v14, \rw, r7
	vrlw	v12, \vy, v17
	vrlw	v13, \vy, v19
	vadduwm	v11, v11, \va
	vxor	v12, v12, v13
	vsrw	v13, \vy, v0
	vrlw	\va, v14, v1
	vxor	v12, v12, v13
	vrlw	v13, v14, v18
	vadduwm	v12, v12, v11
	vxor	\va, \va, v13
	vsrw	v13, v14, v16
	lvx	v11, \rw, r8
	vxor	\va, \va, v13
	vrlw	v13, \vz, v19
	vadduwm	\va, \va, v12

	vrlw	v12, \vz, v17
	vadduwm	v14, v14, \vb
	vxor	v12, v12, v13
	vsrw	v13, \vz, v0
	vrlw	\vb, v11, v1
	vxor	v12, v12, v13
	vrlw	v13, v11, v18
	stvx	\va, \rw, r10
	vxor	\vb, \vb, v13
	vsrw	v13, v11, v16
	vadduwm	v14, v14, v12
	vxor	\vb, \vb, v13
	vadduwm	\vb, \vb, v14
	stvx	\vb, \rw, r11
	addi	\rw, \rw, 2*16
.endm


.macro sha256_4way_main_setup
	vspltisw	v2, 12
	vspltisw	v3, -5
	vspltisw	v16, -6
	vspltisw	v17, -11
	vspltisw	v18, -2
.endm

.macro sha256_4way_main_round i, rk, rw, va, vb, vc, vd, ve, vf, vg, vh
	li	r6, (\i)*16
	lvx	v12, \rw, r6
	vand	v13, \vf, \ve
	vandc	v14, \vg, \ve
	lvx	v15, \rk, r6
	vor	v14, v14, v13
	vrlw	v13, \ve, v3
	vadduwm	\vh, \vh, v14
	vxor	v14, \ve, v13
	vrlw	v13, \ve, v19
	vadduwm	\vh, \vh, v12
	vxor	v14, v14, v13
	vadduwm	\vh, \vh, v15
	vrlw	v13, v14, v16
	vxor	v15, \va, \vb
	vadduwm	\vh, \vh, v13

	vrlw	v13, \va, v17
	vand	v15, v15, \vc
	vxor	v12, \va, v13
	vrlw	v13, \va, v2
	vand	v14, \va, \vb
	vxor	v12, v12, v13
	vxor	v14, v14, v15
	vrlw	v13, v12, v18
	vadduwm	v15, \vh, v14
	vadduwm	\vh, \vh, \vd
	vadduwm	\vd, v15, v13
.endm

.macro sha256_4way_main_quadround i, rk, rw
	sha256_4way_main_round \i+0, \rk, \rw, v4, v5, v6, v7, v8, v9, v10, v11
	sha256_4way_main_round \i+1, \rk, \rw, v7, v4, v5, v6, v11, v8, v9, v10
	sha256_4way_main_round \i+2, \rk, \rw, v6, v7, v4, v5, v10, v11, v8, v9
	sha256_4way_main_round \i+3, \rk, \rw, v5, v6, v7, v4, v9, v10, v11, v8
.endm


#ifdef _AIX
	.csect .text[PR]
#else
	.text
#endif
	.align 2
	.globl sha256_init_4way
	.globl _sha256_init_4way
	.globl .sha256_init_4way
#ifdef __ELF__
	.type sha256_init_4way, %function
#endif
sha256_init_4way:
_sha256_init_4way:
.sha256_init_4way:
	mfspr	r0, 256
	oris	r12, r0, 0xff00
	mtspr	256, r12
	
#ifdef _AIX
	ld	r4, T.sha256_4h(r2)
#else
	lis	r4, HI(sha256_4h)
	addi	r4, r4, LO(sha256_4h)
#endif
	li	r5, 1*16
	li	r6, 2*16
	li	r7, 3*16
	li	r8, 4*16
	li	r9, 5*16
	li	r10, 6*16
	li	r11, 7*16
	lvx	v0, 0, r4
	lvx	v1, r4, r5
	lvx	v2, r4, r6
	lvx	v3, r4, r7
	lvx	v4, r4, r8
	lvx	v5, r4, r9
	lvx	v6, r4, r10
	lvx	v7, r4, r11
	stvx	v0, 0, r3
	stvx	v1, r3, r5
	stvx	v2, r3, r6
	stvx	v3, r3, r7
	stvx	v4, r3, r8
	stvx	v5, r3, r9
	stvx	v6, r3, r10
	stvx	v7, r3, r11
	
	mtspr	256, r0
	blr


	.align 2
	.globl sha256_transform_4way
	.globl _sha256_transform_4way
	.globl .sha256_transform_4way
#ifdef __ELF__
	.type sha256_transform_4way, %function
#endif
sha256_transform_4way:
_sha256_transform_4way:
.sha256_transform_4way:
	mfspr	r0, 256
	oris	r12, r0, 0xffff
	ori	r12, r12, 0xf000
	mtspr	256, r12
	
	andi.	r6, r1, 15
	cmpwi	0, r5, 0
	li	r7, -(4*4+64*16)
	subf	r6, r6, r7
	stdux	r1, r1, r6

	li	r7, 1*16
	li	r8, 2*16
	li	r9, 3*16
	li	r10, 4*16
	li	r11, 5*16
	li	r12, 6*16
	li	r6, 7*16
	
	bne	0, sha256_transform_4way_swap
	
	lvx	v11, 0, r4
	lvx	v1, r4, r7
	lvx	v2, r4, r8
	lvx	v3, r4, r9
	lvx	v4, r4, r10
	lvx	v5, r4, r11
	lvx	v6, r4, r12
	lvx	v7, r4, r6
	addi	r5, r1, 4*4
	stvx	v11, 0, r5
	stvx	v1, r5, r7
	stvx	v2, r5, r8
	stvx	v3, r5, r9
	stvx	v4, r5, r10
	stvx	v5, r5, r11
	stvx	v6, r5, r12
	stvx	v7, r5, r6
	addi	r4, r4, 8*16
	lvx	v0, 0, r4
	lvx	v4, r4, r7
	lvx	v5, r4, r8
	lvx	v6, r4, r9
	lvx	v7, r4, r10
	lvx	v8, r4, r11
	lvx	v9, r4, r12
	lvx	v10, r4, r6
	addi	r4, r1, 4*4+8*16
	stvx	v0, 0, r4
	stvx	v4, r4, r7
	stvx	v5, r4, r8
	stvx	v6, r4, r9
	stvx	v7, r4, r10
	stvx	v8, r4, r11
	stvx	v9, r4, r12
	stvx	v10, r4, r6
	b	sha256_transform_4way_extend

sha256_transform_4way_swap:
#ifdef _AIX
	ld	r5, T.br_perm(r2)
#else
	lis	r5, HI(br_perm)
	addi	r5, r5, LO(br_perm)
#endif
	lvx	v19, 0, r5
	
	lvx	v11, 0, r4
	lvx	v1, r4, r7
	lvx	v2, r4, r8
	lvx	v3, r4, r9
	lvx	v4, r4, r10
	lvx	v5, r4, r11
	lvx	v6, r4, r12
	lvx	v7, r4, r6
	vperm	v11, v11, v11, v19
	vperm	v1, v1, v1, v19
	vperm	v2, v2, v2, v19
	vperm	v3, v3, v3, v19
	vperm	v4, v4, v4, v19
	vperm	v5, v5, v5, v19
	vperm	v6, v6, v6, v19
	vperm	v7, v7, v7, v19
	addi	r5, r1, 4*4
	stvx	v11, 0, r5
	stvx	v1, r5, r7
	stvx	v2, r5, r8
	stvx	v3, r5, r9
	stvx	v4, r5, r10
	stvx	v5, r5, r11
	stvx	v6, r5, r12
	stvx	v7, r5, r6
	addi	r4, r4, 8*16
	lvx	v0, 0, r4
	lvx	v4, r4, r7
	lvx	v5, r4, r8
	lvx	v6, r4, r9
	lvx	v7, r4, r10
	lvx	v8, r4, r11
	lvx	v9, r4, r12
	lvx	v10, r4, r6
	vperm	v0, v0, v0, v19
	vperm	v4, v4, v4, v19
	vperm	v5, v5, v5, v19
	vperm	v6, v6, v6, v19
	vperm	v7, v7, v7, v19
	vperm	v8, v8, v8, v19
	vperm	v9, v9, v9, v19
	vperm	v10, v10, v10, v19
	addi	r4, r1, 4*4+8*16
	stvx	v0, 0, r4
	stvx	v4, r4, r7
	stvx	v5, r4, r8
	stvx	v6, r4, r9
	stvx	v7, r4, r10
	stvx	v8, r4, r11
	stvx	v9, r4, r12
	stvx	v10, r4, r6
	
sha256_transform_4way_extend:
	li	r10, 16*16
	li	r11, 17*16
	sha256_4way_extend_setup
	sha256_4way_extend_doubleround  0, r5, v4, v5, v9, v10
	sha256_4way_extend_doubleround  2, r5, v6, v7, v4, v5
	sha256_4way_extend_doubleround  4, r5, v8, v9, v6, v7
	sha256_4way_extend_doubleround  6, r5, v10, v4, v8, v9
	sha256_4way_extend_doubleround  8, r5, v5, v6, v10, v4
	sha256_4way_extend_doubleround 10, r5, v7, v8, v5, v6
	sha256_4way_extend_doubleround 12, r5, v9, v10, v7, v8
	sha256_4way_extend_doubleround 14, r5, v4, v5, v9, v10
	sha256_4way_extend_doubleround 16, r5, v6, v7, v4, v5
	sha256_4way_extend_doubleround 18, r5, v8, v9, v6, v7
	sha256_4way_extend_doubleround 20, r5, v10, v4, v8, v9
	sha256_4way_extend_doubleround 22, r5, v5, v6, v10, v4
	sha256_4way_extend_doubleround 24, r5, v7, v8, v5, v6
	sha256_4way_extend_doubleround 26, r5, v9, v10, v7, v8
	sha256_4way_extend_doubleround 28, r5, v4, v5, v9, v10
	sha256_4way_extend_doubleround 30, r5, v6, v7, v4, v5
	sha256_4way_extend_doubleround 32, r5, v8, v9, v6, v7
	sha256_4way_extend_doubleround 34, r5, v10, v4, v8, v9
	sha256_4way_extend_doubleround 36, r5, v5, v6, v10, v4
	sha256_4way_extend_doubleround 38, r5, v7, v8, v5, v6
	sha256_4way_extend_doubleround 40, r5, v9, v10, v7, v8
	sha256_4way_extend_doubleround 42, r5, v4, v5, v9, v10
	sha256_4way_extend_doubleround 44, r5, v6, v7, v4, v5
	sha256_4way_extend_doubleround 46, r5, v8, v9, v6, v7
	
	addi	r11, r3, 4*16
	lvx	v4, 0, r3
	lvx	v5, r3, r7
	lvx	v6, r3, r8
	lvx	v7, r3, r9
	lvx	v8, 0, r11
	lvx	v9, r11, r7
	lvx	v10, r11, r8
	lvx	v11, r11, r9
#ifdef _AIX
	ld	r12, T.sha256_4k(r2)
#else
	lis	r12, HI(sha256_4k)
	addi	r12, r12, LO(sha256_4k)
#endif
	addi	r5, r1, 4*4
	sha256_4way_main_setup
	sha256_4way_main_quadround  0, r12, r5
	sha256_4way_main_quadround  4, r12, r5
	sha256_4way_main_quadround  8, r12, r5
	sha256_4way_main_quadround 12, r12, r5
	sha256_4way_main_quadround 16, r12, r5
	sha256_4way_main_quadround 20, r12, r5
	sha256_4way_main_quadround 24, r12, r5
	sha256_4way_main_quadround 28, r12, r5
	sha256_4way_main_quadround 32, r12, r5
	sha256_4way_main_quadround 36, r12, r5
	sha256_4way_main_quadround 40, r12, r5
	sha256_4way_main_quadround 44, r12, r5
	sha256_4way_main_quadround 48, r12, r5
	sha256_4way_main_quadround 52, r12, r5
	sha256_4way_main_quadround 56, r12, r5
	sha256_4way_main_quadround 60, r12, r5
	
	lvx	v12, 0, r3
	lvx	v13, r3, r7
	lvx	v14, r3, r8
	lvx	v15, r3, r9
	lvx	v16, 0, r11
	lvx	v17, r11, r7
	lvx	v18, r11, r8
	lvx	v19, r11, r9
	vadduwm	v4, v4, v12
	vadduwm	v5, v5, v13
	vadduwm	v6, v6, v14
	vadduwm	v7, v7, v15
	vadduwm	v8, v8, v16
	vadduwm	v9, v9, v17
	vadduwm	v10, v10, v18
	vadduwm	v11, v11, v19
	stvx	v4, 0, r3
	stvx	v5, r3, r7
	stvx	v6, r3, r8
	stvx	v7, r3, r9
	stvx	v8, 0, r11
	stvx	v9, r11, r7
	stvx	v10, r11, r8
	stvx	v11, r11, r9
	
	ld	r1, 0(r1)
	mtspr	256, r0
	blr


	.align 2
	.globl sha256d_ms_4way
	.globl _sha256d_ms_4way
	.globl .sha256d_ms_4way
#ifdef __ELF__
	.type sha256d_ms_4way, %function
#endif
sha256d_ms_4way:
_sha256d_ms_4way:
.sha256d_ms_4way:
	mfspr	r0, 256
	oris	r12, r0, 0xffff
	ori	r12, r12, 0xf000
	mtspr	256, r12
	
	andi.	r12, r1, 15
	li	r11, -(4*4+64*16)
	subf	r12, r12, r11
	stdux	r1, r1, r12
	
	li	r7, 1*16
	li	r8, 2*16
	li	r9, 3*16
	li	r10, 16*16
	li	r11, 17*16
	
	sha256_4way_extend_setup
	
	addi	r4, r4, 2*16
	addi	r12, r1, 4*4+18*16
	lvx	v14, r4, r7
	lvx	v6, r4, r10
	lvx	v7, r4, r11
	
	vrlw	v12, v14, v1
	vrlw	v13, v14, v18
	stvx	v6, 0, r12
	vxor	v12, v12, v13
	vsrw	v13, v14, v16
	stvx	v7, r12, r7
	vxor	v12, v12, v13
	vadduwm	v6, v6, v12
	vadduwm	v7, v7, v14
	stvx	v6, r4, r10
	
	vrlw	v12, v6, v17
	vrlw	v13, v6, v19
	stvx	v7, r4, r11
	addi	r4, r4, 18*16
	lvx	v8, 0, r4
	vxor	v12, v12, v13
	vsrw	v13, v6, v0
	stvx	v8, r12, r8
	vxor	v12, v12, v13
	vadduwm	v8, v8, v12
	
	vrlw	v9, v7, v17
	vrlw	v13, v7, v19
	stvx	v8, 0, r4
	vxor	v9, v9, v13
	vsrw	v13, v7, v0
	vxor	v9, v9, v13
	
	vrlw	v12, v8, v17
	vrlw	v13, v8, v19
	stvx	v9, r4, r7
	vxor	v12, v12, v13
	vsrw	v13, v8, v0
	lvx	v10, r4, r8
	lvx	v4, r4, r9
	vxor	v12, v12, v13
	stvx	v10, r12, r9
	addi	r12, r12, 4*16
	stvx	v4, 0, r12
	vrlw	v14, v9, v17
	vrlw	v13, v9, v19
	vadduwm	v10, v10, v12
	
	vxor	v14, v14, v13
	vsrw	v13, v9, v0
	stvx	v10, r4, r8
	vxor	v14, v14, v13
	vadduwm	v4, v4, v14
	
	vrlw	v12, v10, v17
	vrlw	v13, v10, v19
	stvx	v4, r4, r9
	vxor	v12, v12, v13
	vsrw	v13, v10, v0
	vrlw	v14, v4, v17
	vxor	v12, v12, v13
	vrlw	v13, v4, v19
	addi	r4, r4, 4*16
	lvx	v5, 0, r4
	vxor	v14, v14, v13
	stvx	v5, r12, r7
	vsrw	v13, v4, v0
	vadduwm	v5, v5, v12
	vxor	v14, v14, v13
	stvx	v5, 0, r4
	vadduwm	v6, v6, v14
	
	vrlw	v12, v5, v17
	vrlw	v13, v5, v19
	stvx	v6, r4, r7
	vxor	v12, v12, v13
	vsrw	v13, v5, v0
	vrlw	v14, v6, v17
	vxor	v12, v12, v13
	vrlw	v13, v6, v19
	vxor	v14, v14, v13
	vsrw	v13, v6, v0
	vadduwm	v7, v7, v12
	vxor	v14, v14, v13
	stvx	v7, r4, r8
	vadduwm	v8, v8, v14
	
	vrlw	v12, v7, v17
	vrlw	v13, v7, v19
	stvx	v8, r4, r9
	vxor	v12, v12, v13
	vsrw	v13, v7, v0
	vrlw	v14, v8, v17
	vxor	v12, v12, v13
	vrlw	v13, v8, v19
	vxor	v14, v14, v13
	vsrw	v13, v8, v0
	vadduwm	v9, v9, v12
	vxor	v14, v14, v13
	addi	r4, r4, 4*16
	stvx	v9, 0, r4
	vadduwm	v10, v10, v14
	
	vrlw	v12, v9, v17
	vrlw	v13, v9, v19
	stvx	v10, r4, r7
	vxor	v12, v12, v13
	vsrw	v13, v9, v0
	lvx	v11, r4, r8
	lvx	v14, r4, r9
	stvx	v11, r12, r8
	stvx	v14, r12, r9
	vxor	v12, v12, v13
	vadduwm	v11, v11, v12
	vadduwm	v5, v5, v14
	vrlw	v12, v10, v17
	vrlw	v13, v10, v19
	vadduwm	v4, v4, v11
	
	vxor	v12, v12, v13
	vsrw	v13, v10, v0
	stvx	v4, r4, r8
	vxor	v12, v12, v13
	vadduwm	v5, v5, v12
	stvx	v5, r4, r9
	addi	r4, r4, -12*16
	lvx	v11, 0, r4
	
	sha256_4way_extend_doubleround 16, r4, v6, v7, v4, v5
	sha256_4way_extend_doubleround 18, r4, v8, v9, v6, v7
	sha256_4way_extend_doubleround 20, r4, v10, v4, v8, v9
	sha256_4way_extend_doubleround 22, r4, v5, v6, v10, v4
	sha256_4way_extend_doubleround 24, r4, v7, v8, v5, v6
	sha256_4way_extend_doubleround 26, r4, v9, v10, v7, v8
	sha256_4way_extend_doubleround 28, r4, v4, v5, v9, v10
	sha256_4way_extend_doubleround 30, r4, v6, v7, v4, v5
	sha256_4way_extend_doubleround 32, r4, v8, v9, v6, v7
	sha256_4way_extend_doubleround 34, r4, v10, v4, v8, v9
	sha256_4way_extend_doubleround 36, r4, v5, v6, v10, v4
	sha256_4way_extend_doubleround 38, r4, v7, v8, v5, v6
	sha256_4way_extend_doubleround 40, r4, v9, v10, v7, v8
	sha256_4way_extend_doubleround 42, r4, v4, v5, v9, v10
	sha256_4way_extend_doubleround 44, r4, v6, v7, v4, v5
	sha256_4way_extend_doubleround 46, r4, v8, v9, v6, v7
	addi	r4, r4, -48*16
	
	lvx	v4, 0, r6
	lvx	v9, r6, r7
	lvx	v10, r6, r8
	lvx	v11, r6, r9
	addi	r12, r6, 4*16
	lvx	v8, 0, r12
	lvx	v5, r12, r7
	lvx	v6, r12, r8
	lvx	v7, r12, r9
#ifdef _AIX
	ld	r12, T.sha256_4k(r2)
#else
	lis	r12, HI(sha256_4k)
	addi	r12, r12, LO(sha256_4k)
#endif
	sha256_4way_main_setup
	sha256_4way_main_round  3, r12, r4, v5, v6, v7, v4, v9, v10, v11, v8
	sha256_4way_main_quadround  4, r12, r4
	sha256_4way_main_quadround  8, r12, r4
	sha256_4way_main_quadround 12, r12, r4
	sha256_4way_main_quadround 16, r12, r4
	sha256_4way_main_quadround 20, r12, r4
	sha256_4way_main_quadround 24, r12, r4
	sha256_4way_main_quadround 28, r12, r4
	sha256_4way_main_quadround 32, r12, r4
	sha256_4way_main_quadround 36, r12, r4
	sha256_4way_main_quadround 40, r12, r4
	sha256_4way_main_quadround 44, r12, r4
	sha256_4way_main_quadround 48, r12, r4
	sha256_4way_main_quadround 52, r12, r4
	sha256_4way_main_quadround 56, r12, r4
	sha256_4way_main_quadround 60, r12, r4
	
	lvx	v12, 0, r5
	lvx	v13, r5, r7
	lvx	v14, r5, r8
	lvx	v15, r5, r9
	addi	r12, r5, 4*16
	lvx	v16, 0, r12
	lvx	v17, r12, r7
	lvx	v18, r12, r8
	lvx	v19, r12, r9
	vadduwm	v4, v4, v12
	vadduwm	v5, v5, v13
	vadduwm	v6, v6, v14
	vadduwm	v7, v7, v15
	vadduwm	v8, v8, v16
	vadduwm	v9, v9, v17
	vadduwm	v10, v10, v18
	vadduwm	v11, v11, v19
	addi	r12, r1, 4*4
	stvx	v4, 0, r12
	stvx	v5, r12, r7
	stvx	v6, r12, r8
	stvx	v7, r12, r9
	addi	r12, r12, 4*16
	stvx	v8, 0, r12
	stvx	v9, r12, r7
	stvx	v10, r12, r8
	stvx	v11, r12, r9
	
	addi	r12, r1, 4*4+18*16
	lvx	v4, 0, r12
	lvx	v5, r12, r7
	lvx	v6, r12, r8
	lvx	v7, r12, r9
	addi	r12, r12, 4*16
	lvx	v8, 0, r12
	lvx	v9, r12, r7
	lvx	v10, r12, r8
	lvx	v11, r12, r9
	addi	r12, r4, 18*16
	stvx	v4, 0, r12
	stvx	v5, r12, r7
	stvx	v6, r12, r8
	addi	r12, r4, 22*16
	stvx	v7, 0, r12
	stvx	v8, r12, r7
	stvx	v9, r12, r8
	addi	r12, r4, 30*16
	stvx	v10, 0, r12
	stvx	v11, r12, r7
	
	addi	r4, r1, 4*4
	
	sha256_4way_extend_setup
	
#ifdef _AIX
	ld	r12, T.sha256d_4preext2(r2)
#else
	lis	r12, HI(sha256d_4preext2)
	addi	r12, r12, LO(sha256d_4preext2)
#endif
	lvx	v2, 0, r12
	
	vxor	v9, v9, v9
	vspltisw	v3, 1
	lvx	v4, r12, r8
	vsldoi	v3, v3, v3, 1
	addi	r5, r1, 4*4+8*16
	stvx	v4, 0, r5
	stvx	v9, r5, r7
	stvx	v9, r5, r8
	stvx	v9, r5, r9
	addi	r5, r5, 4*16
	stvx	v9, 0, r5
	stvx	v9, r5, r7
	stvx	v9, r5, r8
	stvx	v3, r5, r9
	
	lvx	v4, 0, r4
	lvx	v14, r4, r7
	
	lvx	v11, r4, r8
	vrlw	v12, v14, v1
	vrlw	v13, v14, v18
	
	vxor	v12, v12, v13
	vsrw	v13, v14, v16
	vadduwm	v5, v14, v2
	vxor	v12, v12, v13
	vrlw	v14, v11, v1
	vrlw	v13, v11, v18
	vadduwm	v4, v4, v12
	vxor	v14, v14, v13
	vsrw	v13, v11, v16
	stvx	v4, r4, r10
	vxor	v14, v14, v13
	vrlw	v12, v4, v17
	vrlw	v13, v4, v19
	vadduwm	v5, v5, v14
	
	stvx	v5, r4, r11
	addi	r4, r4, 2*16
	lvx	v14, r4, r7
	vxor	v12, v12, v13
	vsrw	v13, v4, v0
	vrlw	v6, v14, v1
	vxor	v12, v12, v13
	vrlw	v13, v14, v18
	vxor	v6, v6, v13
	vsrw	v13, v14, v16
	vadduwm	v11, v11, v12
	vxor	v6, v6, v13
	vrlw	v12, v5, v17
	vrlw	v13, v5, v19
	vadduwm	v6, v6, v11
	lvx	v11, r4, r8
	
	stvx	v6, r4, r10
	vxor	v12, v12, v13
	vsrw	v13, v5, v0
	vrlw	v7, v11, v1
	vxor	v12, v12, v13
	vrlw	v13, v11, v18
	vxor	v7, v7, v13
	vsrw	v13, v11, v16
	vadduwm	v14, v14, v12
	vxor	v7, v7, v13
	vrlw	v12, v6, v17
	vrlw	v13, v6, v19
	vadduwm	v7, v7, v14
	
	stvx	v7, r4, r11
	addi	r4, r4, 2*16
	lvx	v14, r4, r7
	vxor	v12, v12, v13
	vsrw	v13, v6, v0
	vrlw	v8, v14, v1
	vxor	v12, v12, v13
	vrlw	v13, v14, v18
	vxor	v8, v8, v13
	vsrw	v13, v14, v16
	vadduwm	v11, v11, v12
	vxor	v8, v8, v13
	vrlw	v12, v7, v17
	vrlw	v13, v7, v19
	vadduwm	v8, v8, v11
	lvx	v11, r4, r8
	
	stvx	v8, r4, r10
	vxor	v12, v12, v13
	vsrw	v13, v7, v0
	vrlw	v9, v11, v1
	vxor	v12, v12, v13
	vrlw	v13, v11, v18
	vxor	v9, v9, v13
	vsrw	v13, v11, v16
	vadduwm	v14, v14, v12
	vxor	v9, v9, v13
	vrlw	v12, v8, v17
	vrlw	v13, v8, v19
	vadduwm	v9, v9, v14
	
	stvx	v9, r4, r11
	addi	r4, r4, 2*16
	lvx	v14, r4, r7
	vxor	v12, v12, v13
	vsrw	v13, v8, v0
	vrlw	v10, v14, v1
	vxor	v12, v12, v13
	vrlw	v13, v14, v18
	vxor	v10, v10, v13
	vsrw	v13, v14, v16
	vadduwm	v11, v11, v12
	vxor	v10, v10, v13
	vrlw	v12, v9, v17
	vrlw	v13, v9, v19
	vadduwm	v11, v11, v3
	vadduwm	v14, v14, v4
	vadduwm	v10, v10, v11
	
	lvx	v2, r12, r7
	vxor	v12, v12, v13
	vsrw	v13, v9, v0
	stvx	v10, r4, r10
	vxor	v12, v12, v13
	vadduwm	v14, v14, v12
	vrlw	v12, v10, v17
	vrlw	v13, v10, v19
	vadduwm	v4, v14, v2
	lvx	v2, r12, r8
	vxor	v12, v12, v13
	vsrw	v13, v10, v0
	stvx	v4, r4, r11
	vadduwm	v5, v5, v2
	vxor	v12, v12, v13
	vadduwm	v5, v5, v12

	vrlw	v12, v4, v17
	vrlw	v13, v4, v19
	addi	r4, r4, 2*16
	stvx	v5, r4, r10
	vxor	v12, v12, v13
	vsrw	v13, v4, v0
	vrlw	v11, v5, v17
	vxor	v12, v12, v13
	vrlw	v13, v5, v19
	vxor	v11, v11, v13
	vsrw	v13, v5, v0
	vadduwm	v6, v6, v12
	vxor	v11, v11, v13
	stvx	v6, r4, r11
	vadduwm	v7, v7, v11
	
	vrlw	v12, v6, v17
	vrlw	v13, v6, v19
	addi	r4, r4, 2*16
	stvx	v7, r4, r10
	vxor	v12, v12, v13
	vsrw	v13, v6, v0
	vrlw	v11, v7, v17
	vxor	v12, v12, v13
	vrlw	v13, v7, v19
	vxor	v11, v11, v13
	vsrw	v13, v7, v0
	vadduwm	v8, v8, v12
	vxor	v11, v11, v13
	stvx	v8, r4, r11
	vadduwm	v9, v9, v11
	
	lvx	v2, r12, r9
	vrlw	v14, v8, v17
	vrlw	v13, v8, v19
	vrlw	v12, v9, v17
	addi	r4, r4, 2*16
	stvx	v9, r4, r10
	vxor	v14, v14, v13
	vrlw	v13, v9, v19
	vxor	v12, v12, v13
	vsrw	v13, v8, v0
	vxor	v14, v14, v13
	vsrw	v13, v9, v0
	vxor	v12, v12, v13
	vadduwm	v4, v4, v2
	vadduwm	v10, v10, v14
	vadduwm	v4, v4, v12
	stvx	v10, r4, r11
	addi	r4, r4, 2*16
	lvx	v11, r4, r8
	
	vadduwm	v5, v5, v3
	stvx	v4, r4, r10
	vrlw	v14, v11, v1
	vrlw	v13, v11, v18
	vrlw	v12, v10, v17
	vxor	v14, v14, v13
	vrlw	v13, v10, v19
	vxor	v12, v12, v13
	vsrw	v13, v11, v16
	vxor	v14, v14, v13
	vsrw	v13, v10, v0
	vxor	v12, v12, v13
	vadduwm	v5, v5, v14
	vadduwm	v5, v5, v12
	stvx	v5, r4, r11
	addi	r4, r4, 2*16
	
	sha256_4way_extend_doubleround 16, r4, v6, v7, v4, v5
	sha256_4way_extend_doubleround 18, r4, v8, v9, v6, v7
	sha256_4way_extend_doubleround 20, r4, v10, v4, v8, v9
	sha256_4way_extend_doubleround 22, r4, v5, v6, v10, v4
	sha256_4way_extend_doubleround 24, r4, v7, v8, v5, v6
	sha256_4way_extend_doubleround 26, r4, v9, v10, v7, v8
	sha256_4way_extend_doubleround 28, r4, v4, v5, v9, v10
	sha256_4way_extend_doubleround 30, r4, v6, v7, v4, v5
	sha256_4way_extend_doubleround 32, r4, v8, v9, v6, v7
	sha256_4way_extend_doubleround 34, r4, v10, v4, v8, v9
	sha256_4way_extend_doubleround 36, r4, v5, v6, v10, v4
	sha256_4way_extend_doubleround 38, r4, v7, v8, v5, v6
	sha256_4way_extend_doubleround 40, r4, v9, v10, v7, v8
	sha256_4way_extend_doubleround 42, r4, v4, v5, v9, v10
	
	lvx	v14, r4, r7
	vrlw	v12, v4, v17
	vrlw	v13, v4, v19
	vadduwm	v15, v11, v6
	vrlw	v6, v14, v1
	vrlw	v11, v14, v18
	vxor	v12, v12, v13
	vxor	v6, v6, v11
	vsrw	v13, v4, v0
	vsrw	v14, v14, v16
	vxor	v12, v12, v13
	vxor	v6, v6, v14
	vadduwm	v12, v12, v15
	vadduwm	v6, v6, v12
	stvx	v6, r4, r10
	addi	r4, r4, -44*16
	
#ifdef _AIX
	ld	r5, T.sha256_4h(r2)
#else
	lis	r5, HI(sha256_4h)
	addi	r5, r5, LO(sha256_4h)
#endif
	lvx	v4, 0, r5
	lvx	v5, r5, r7
	lvx	v6, r5, r8
	lvx	v7, r5, r9
	addi	r12, r5, 4*16
	lvx	v8, 0, r12
	lvx	v9, r12, r7
	lvx	v10, r12, r8
	lvx	v11, r12, r9
#ifdef _AIX
	ld	r12, T.sha256_4k(r2)
#else
	lis	r12, HI(sha256_4k)
	addi	r12, r12, LO(sha256_4k)
#endif
	sha256_4way_main_setup
	sha256_4way_main_quadround  0, r12, r4
	sha256_4way_main_quadround  4, r12, r4
	sha256_4way_main_quadround  8, r12, r4
	sha256_4way_main_quadround 12, r12, r4
	sha256_4way_main_quadround 16, r12, r4
	sha256_4way_main_quadround 20, r12, r4
	sha256_4way_main_quadround 24, r12, r4
	sha256_4way_main_quadround 28, r12, r4
	sha256_4way_main_quadround 32, r12, r4
	sha256_4way_main_quadround 36, r12, r4
	sha256_4way_main_quadround 40, r12, r4
	sha256_4way_main_quadround 44, r12, r4
	sha256_4way_main_quadround 48, r12, r4
	sha256_4way_main_quadround 52, r12, r4
	sha256_4way_main_round 56, r12, r4, v4, v5, v6, v7, v8, v9, v10, v11

.macro sha256_4way_main_round_red i, rk, rw, vd, ve, vf, vg, vh
	li	r6, (\i)*16
	vand	v15, \vf, \ve
	vandc	v14, \vg, \ve
	lvx	v12, \rw, r6
	vadduwm	\vh, \vh, \vd
	vor	v14, v14, v15
	lvx	v15, \rk, r6
	vrlw	v13, \ve, v3
	vadduwm	\vh, \vh, v14
	vxor	v14, \ve, v13
	vrlw	v13, \ve, v19
	vadduwm	\vh, \vh, v12
	vxor	v14, v14, v13
	vadduwm	\vh, \vh, v15
	vrlw	v13, v14, v16
	vadduwm	\vh, \vh, v13
.endm

	sha256_4way_main_round_red 57, r12, r4, v6, v11, v8, v9, v10
	sha256_4way_main_round_red 58, r12, r4, v5, v10, v11, v8, v9
	sha256_4way_main_round_red 59, r12, r4, v4, v9, v10, v11, v8
	sha256_4way_main_round_red 60, r12, r4, v7, v8, v9, v10, v11
	
	li	r12, 7*16
	lvx	v19, r5, r12
	vadduwm	v11, v11, v19
	stvx	v11, r3, r12
	
	ld	r1, 0(r1)
	mtspr	256, r0
	blr


	.align 2
	.globl sha256_use_4way
	.globl _sha256_use_4way
	.globl .sha256_use_4way
#ifdef __ELF__
	.type sha256_use_4way, %function
#endif
sha256_use_4way:
_sha256_use_4way:
.sha256_use_4way:
	li	r3, 1
	blr

#endif /* __ALTIVEC__ */

#endif
