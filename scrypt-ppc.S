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

#ifndef __APPLE__

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


#ifdef __ALTIVEC__

#ifdef __APPLE__
	.machine ppc7400
#endif

.macro salsa8_core_doubleround
	vadduwm	v4, v0, v1
	vrlw	v4, v4, v16
	vxor	v3, v3, v4
	
	vadduwm	v4, v3, v0
	vrlw	v4, v4, v17
	vxor	v2, v2, v4
	
	vadduwm	v4, v2, v3
	vrlw	v4, v4, v18
	vsldoi	v3, v3, v3, 12
	vxor	v1, v1, v4
	
	vadduwm	v4, v1, v2
	vrlw	v4, v4, v19
	vsldoi	v1, v1, v1, 4
	vxor	v0, v0, v4
	
	vadduwm	v4, v0, v3
	vrlw	v4, v4, v16
	vsldoi	v2, v2, v2, 8
	vxor	v1, v1, v4
	
	vadduwm	v4, v1, v0
	vrlw	v4, v4, v17
	vxor	v2, v2, v4
	
	vadduwm	v4, v2, v1
	vrlw	v4, v4, v18
	vsldoi	v1, v1, v1, 12
	vxor	v3, v3, v4
	
	vadduwm	v4, v3, v2
	vrlw	v4, v4, v19
	vsldoi	v3, v3, v3, 4
	vxor	v0, v0, v4
	vsldoi	v2, v2, v2, 8
.endm

.macro salsa8_core
	salsa8_core_doubleround
	salsa8_core_doubleround
	salsa8_core_doubleround
	salsa8_core_doubleround
.endm

#ifdef _AIX
	.csect .text[PR]
#else
	.text
#endif
	.align 2
	.globl scrypt_core
	.globl _scrypt_core
	.globl .scrypt_core
#ifdef __ELF__
	.type scrypt_core, %function
#endif
scrypt_core:
_scrypt_core:
.scrypt_core:
	stdu	r1, -4*4(r1)
	mfspr	r0, 256
	std	r0, 2*4(r1)
	oris	r0, r0, 0xffff
	ori	r0, r0, 0xf000
	mtspr	256, r0
	
	li	r6, 1*16
	li	r7, 2*16
	li	r8, 3*16
	li	r9, 4*16
	li	r10, 5*16
	li	r11, 6*16
	li	r12, 7*16
	
	lvx	v8, 0, r3
	lvx	v9, r3, r6
	lvx	v10, r3, r7
	lvx	v11, r3, r8
	lvx	v12, r3, r9
	lvx	v13, r3, r10
	lvx	v14, r3, r11
	lvx	v15, r3, r12
	
	vxor	v0, v0, v0
	vnor	v1, v0, v0
	vsldoi	v2, v0, v1, 4
	vsldoi	v3, v2, v0, 8
	vor	v3, v3, v2
	vsldoi	v1, v0, v1, 8
	
	vor	v4, v8, v8
	vsel	v8, v8, v9, v3
	vsel	v9, v9, v10, v3
	vsel	v10, v10, v11, v3
	vsel	v11, v11, v4, v3
	vor	v4, v8, v8
	vor	v5, v9, v9
	vsel	v8, v8, v10, v1
	vsel	v9, v11, v9, v1
	vsel	v10, v10, v4, v1
	vsel	v11, v5, v11, v1
	
	vor	v4, v12, v12
	vsel	v12, v12, v13, v3
	vsel	v13, v13, v14, v3
	vsel	v14, v14, v15, v3
	vsel	v15, v15, v4, v3
	vor	v4, v12, v12
	vor	v5, v13, v13
	vsel	v12, v12, v14, v1
	vsel	v13, v15, v13, v1
	vsel	v14, v14, v4, v1
	vsel	v15, v5, v15, v1
	
	vspltisw	v16, 7
	vspltisw	v17, 9
	vspltisw	v18, 13
	vadduwm	v19, v17, v17
	
	mtctr	r5
scrypt_core_loop1:
	vxor	v8, v8, v12
	stvx	v8, 0, r4
	vxor	v9, v9, v13
	stvx	v9, r4, r6
	vxor	v10, v10, v14
	stvx	v10, r4, r7
	vxor	v11, v11, v15
	stvx	v11, r4, r8
	vor	v0, v8, v8
	stvx	v12, r4, r9
	vor	v1, v9, v9
	stvx	v13, r4, r10
	vor	v2, v10, v10
	stvx	v14, r4, r11
	vor	v3, v11, v11
	stvx	v15, r4, r12
	
	salsa8_core
	
	vadduwm	v8, v8, v0
	vadduwm	v9, v9, v1
	vadduwm	v10, v10, v2
	vadduwm	v11, v11, v3
	
	vxor	v12, v12, v8
	vxor	v13, v13, v9
	vxor	v14, v14, v10
	vxor	v15, v15, v11
	vor	v0, v12, v12
	vor	v1, v13, v13
	vor	v2, v14, v14
	vor	v3, v15, v15
	
	salsa8_core
	
	vadduwm	v12, v12, v0
	vadduwm	v13, v13, v1
	vadduwm	v14, v14, v2
	vadduwm	v15, v15, v3
	
	addi	r4, r4, 32*4
	bdnz	scrypt_core_loop1
	
	stvx	v12, 0, r3
	slwi	r6, r5, 7
	subf	r4, r6, r4
	mtctr	r5
	addi	r5, r5, -1
	addi	r7, r4, 1*16
	addi	r8, r4, 2*16
	addi	r9, r4, 3*16
scrypt_core_loop2:
	lwz	r6, 0(r3)
	and	r6, r6, r5
	slwi	r6, r6, 7
	lvx	v0, r4, r6
	vxor	v8, v8, v12
	lvx	v1, r7, r6
	vxor	v9, v9, v13
	lvx	v2, r8, r6
	vxor	v10, v10, v14
	lvx	v3, r9, r6
	vxor	v11, v11, v15
	vxor	v0, v0, v8
	vxor	v1, v1, v9
	vxor	v2, v2, v10
	vxor	v3, v3, v11
	addi	r6, r6, 64
	vor	v8, v0, v0
	vor	v9, v1, v1
	lvx	v5, r4, r6
	vor	v10, v2, v2
	lvx	v6, r7, r6
	vor	v11, v3, v3
	lvx	v7, r8, r6
	
	salsa8_core
	
	vadduwm	v8, v8, v0
	lvx	v0, r9, r6
	vadduwm	v9, v9, v1
	vadduwm	v10, v10, v2
	vadduwm	v11, v11, v3
	
	vxor	v12, v12, v5
	vxor	v13, v13, v6
	vxor	v14, v14, v7
	vxor	v15, v15, v0
	vxor	v12, v12, v8
	vxor	v13, v13, v9
	vxor	v14, v14, v10
	vxor	v15, v15, v11
	vor	v0, v12, v12
	vor	v1, v13, v13
	vor	v2, v14, v14
	vor	v3, v15, v15
	
	salsa8_core
	
	vadduwm	v12, v12, v0
	stvx	v12, 0, r3
	vadduwm	v13, v13, v1
	vadduwm	v14, v14, v2
	vadduwm	v15, v15, v3
	
	bdnz	scrypt_core_loop2
	
	vxor	v0, v0, v0
	vnor	v1, v0, v0
	vsldoi	v2, v0, v1, 4
	vsldoi	v3, v2, v0, 8
	vor	v3, v3, v2
	vsldoi	v1, v0, v1, 8
	
	vor	v4, v8, v8
	vsel	v8, v8, v9, v3
	vsel	v9, v9, v10, v3
	vsel	v10, v10, v11, v3
	vsel	v11, v11, v4, v3
	vor	v4, v8, v8
	vor	v5, v9, v9
	vsel	v8, v8, v10, v1
	vsel	v9, v11, v9, v1
	vsel	v10, v10, v4, v1
	vsel	v11, v5, v11, v1
	
	vor	v4, v12, v12
	vsel	v12, v12, v13, v3
	vsel	v13, v13, v14, v3
	vsel	v14, v14, v15, v3
	vsel	v15, v15, v4, v3
	vor	v4, v12, v12
	vor	v5, v13, v13
	vsel	v12, v12, v14, v1
	vsel	v13, v15, v13, v1
	vsel	v14, v14, v4, v1
	vsel	v15, v5, v15, v1
	
	li	r6, 1*16
	li	r7, 2*16
	li	r8, 3*16
	li	r9, 4*16
	
	stvx	v8, 0, r3
	stvx	v9, r3, r6
	stvx	v10, r3, r7
	stvx	v11, r3, r8
	stvx	v12, r3, r9
	stvx	v13, r3, r10
	stvx	v14, r3, r11
	stvx	v15, r3, r12
	
	ld	r0, 2*4(r1)
	mtspr	256, r0
	addi	r1, r1, 4*4
	blr

#else /* __ALTIVEC__ */

.macro salsa8_core_doubleround
	add	r0, r16, r28
	add	r5, r21, r17
	add	r6, r26, r22
	add	r7, r31, r27
	rotlwi	r0, r0, 7
	rotlwi	r5, r5, 7
	rotlwi	r6, r6, 7
	rotlwi	r7, r7, 7
	xor	r20, r20, r0
	xor	r25, r25, r5
	xor	r30, r30, r6
	xor	r19, r19, r7
	
	add	r0, r20, r16
	add	r5, r25, r21
	add	r6, r30, r26
	add	r7, r19, r31
	rotlwi	r0, r0, 9
	rotlwi	r5, r5, 9
	rotlwi	r6, r6, 9
	rotlwi	r7, r7, 9
	xor	r24, r24, r0
	xor	r29, r29, r5
	xor	r18, r18, r6
	xor	r23, r23, r7
	
	add	r0, r24, r20
	add	r5, r29, r25
	add	r6, r18, r30
	add	r7, r23, r19
	rotlwi	r0, r0, 13
	rotlwi	r5, r5, 13
	rotlwi	r6, r6, 13
	rotlwi	r7, r7, 13
	xor	r28, r28, r0
	xor	r17, r17, r5
	xor	r22, r22, r6
	xor	r27, r27, r7
	
	add	r0, r28, r24
	add	r5, r17, r29
	add	r6, r22, r18
	add	r7, r27, r23
	rotlwi	r0, r0, 18
	rotlwi	r5, r5, 18
	rotlwi	r6, r6, 18
	rotlwi	r7, r7, 18
	xor	r16, r16, r0
	xor	r21, r21, r5
	xor	r26, r26, r6
	xor	r31, r31, r7
	
	add	r0, r16, r19
	add	r5, r21, r20
	add	r6, r26, r25
	add	r7, r31, r30
	rotlwi	r0, r0, 7
	rotlwi	r5, r5, 7
	rotlwi	r6, r6, 7
	rotlwi	r7, r7, 7
	xor	r17, r17, r0
	xor	r22, r22, r5
	xor	r27, r27, r6
	xor	r28, r28, r7
	
	add	r0, r17, r16
	add	r5, r22, r21
	add	r6, r27, r26
	add	r7, r28, r31
	rotlwi	r0, r0, 9
	rotlwi	r5, r5, 9
	rotlwi	r6, r6, 9
	rotlwi	r7, r7, 9
	xor	r18, r18, r0
	xor	r23, r23, r5
	xor	r24, r24, r6
	xor	r29, r29, r7
	
	add	r0, r18, r17
	add	r5, r23, r22
	add	r6, r24, r27
	add	r7, r29, r28
	rotlwi	r0, r0, 13
	rotlwi	r5, r5, 13
	rotlwi	r6, r6, 13
	rotlwi	r7, r7, 13
	xor	r19, r19, r0
	xor	r20, r20, r5
	xor	r25, r25, r6
	xor	r30, r30, r7
	
	add	r0, r19, r18
	add	r5, r20, r23
	add	r6, r25, r24
	add	r7, r30, r29
	rotlwi	r0, r0, 18
	rotlwi	r5, r5, 18
	rotlwi	r6, r6, 18
	rotlwi	r7, r7, 18
	xor	r16, r16, r0
	xor	r21, r21, r5
	xor	r26, r26, r6
	xor	r31, r31, r7
.endm

.macro salsa8_core
	salsa8_core_doubleround
	salsa8_core_doubleround
	salsa8_core_doubleround
	salsa8_core_doubleround
.endm

#ifdef _AIX
	.csect .text[PR]
#else
	.text
#endif
	.align 2
	.globl scrypt_core
	.globl _scrypt_core
	.globl .scrypt_core
#ifdef __ELF__
	.type scrypt_core, %function
#endif
scrypt_core:
_scrypt_core:
.scrypt_core:
	stdu	r1, -68*4(r1)
	stw	r5, 2*4(r1)
	std	r13, 4*4(r1)
	std	r14, 6*4(r1)
	std	r15, 8*4(r1)
	std	r16, 10*4(r1)
	std	r17, 12*4(r1)
	std	r18, 14*4(r1)
	std	r19, 16*4(r1)
	std	r20, 18*4(r1)
	std	r21, 20*4(r1)
	std	r3, 22*4(r1)
	std	r22, 48*4(r1)
	std	r23, 50*4(r1)
	std	r24, 52*4(r1)
	std	r25, 54*4(r1)
	std	r26, 56*4(r1)
	std	r27, 58*4(r1)
	std	r28, 60*4(r1)
	std	r29, 62*4(r1)
	std	r30, 64*4(r1)
	std	r31, 66*4(r1)
	
	lwz	r16, 0*4(r3)
	lwz	r17, 1*4(r3)
	lwz	r18, 2*4(r3)
	lwz	r19, 3*4(r3)
	lwz	r20, 4*4(r3)
	lwz	r21, 5*4(r3)
	lwz	r22, 6*4(r3)
	lwz	r23, 7*4(r3)
	stw	r16, 24*4(r1)
	stw	r17, 25*4(r1)
	stw	r18, 26*4(r1)
	stw	r19, 27*4(r1)
	stw	r20, 28*4(r1)
	stw	r21, 29*4(r1)
	stw	r22, 30*4(r1)
	stw	r23, 31*4(r1)
	lwz	r24, 8*4(r3)
	lwz	r25, 9*4(r3)
	lwz	r26, 10*4(r3)
	lwz	r27, 11*4(r3)
	lwz	r28, 12*4(r3)
	lwz	r29, 13*4(r3)
	lwz	r30, 14*4(r3)
	lwz	r31, 15*4(r3)
	stw	r24, 32*4(r1)
	stw	r25, 33*4(r1)
	stw	r26, 34*4(r1)
	stw	r27, 35*4(r1)
	stw	r28, 36*4(r1)
	stw	r29, 37*4(r1)
	stw	r30, 38*4(r1)
	stw	r31, 39*4(r1)
	lwz	r16, 16*4(r3)
	lwz	r17, 17*4(r3)
	lwz	r18, 18*4(r3)
	lwz	r19, 19*4(r3)
	lwz	r20, 20*4(r3)
	lwz	r21, 21*4(r3)
	lwz	r22, 22*4(r3)
	lwz	r23, 23*4(r3)
	stw	r16, 40*4(r1)
	stw	r17, 41*4(r1)
	stw	r18, 42*4(r1)
	stw	r19, 43*4(r1)
	stw	r20, 44*4(r1)
	stw	r21, 45*4(r1)
	stw	r22, 46*4(r1)
	stw	r23, 47*4(r1)
	lwz	r8, 24*4(r3)
	lwz	r9, 25*4(r3)
	lwz	r10, 26*4(r3)
	lwz	r11, 27*4(r3)
	lwz	r12, 28*4(r3)
	lwz	r13, 29*4(r3)
	lwz	r14, 30*4(r3)
	lwz	r15, 31*4(r3)
	
	mtctr	r5
scrypt_core_loop1:
	lwz	r16, 24*4(r1)
	lwz	r17, 25*4(r1)
	lwz	r18, 26*4(r1)
	lwz	r19, 27*4(r1)
	lwz	r20, 28*4(r1)
	lwz	r21, 29*4(r1)
	lwz	r22, 30*4(r1)
	lwz	r23, 31*4(r1)
	lwz	r24, 32*4(r1)
	lwz	r25, 33*4(r1)
	lwz	r26, 34*4(r1)
	lwz	r27, 35*4(r1)
	lwz	r28, 36*4(r1)
	lwz	r29, 37*4(r1)
	lwz	r30, 38*4(r1)
	lwz	r31, 39*4(r1)
	
	lwz	r0, 40*4(r1)
	lwz	r5, 41*4(r1)
	lwz	r6, 42*4(r1)
	lwz	r7, 43*4(r1)
	xor	r16, r16, r0
	xor	r17, r17, r5
	xor	r18, r18, r6
	xor	r19, r19, r7
	stw	r16, 0*4(r4)
	stw	r17, 1*4(r4)
	stw	r18, 2*4(r4)
	stw	r19, 3*4(r4)
	stw	r0, 16*4(r4)
	stw	r5, 17*4(r4)
	stw	r6, 18*4(r4)
	stw	r7, 19*4(r4)
	
	lwz	r0, 44*4(r1)
	lwz	r5, 45*4(r1)
	lwz	r6, 46*4(r1)
	lwz	r7, 47*4(r1)
	xor	r20, r20, r0
	xor	r21, r21, r5
	xor	r22, r22, r6
	xor	r23, r23, r7
	stw	r0, 20*4(r4)
	stw	r5, 21*4(r4)
	stw	r6, 22*4(r4)
	stw	r7, 23*4(r4)
	stw	r20, 4*4(r4)
	stw	r21, 5*4(r4)
	stw	r22, 6*4(r4)
	stw	r23, 7*4(r4)
	
	xor	r24, r24, r8
	xor	r25, r25, r9
	xor	r26, r26, r10
	xor	r27, r27, r11
	xor	r28, r28, r12
	xor	r29, r29, r13
	xor	r30, r30, r14
	xor	r31, r31, r15
	stw	r24, 8*4(r4)
	stw	r25, 9*4(r4)
	stw	r26, 10*4(r4)
	stw	r27, 11*4(r4)
	stw	r28, 12*4(r4)
	stw	r29, 13*4(r4)
	stw	r30, 14*4(r4)
	stw	r31, 15*4(r4)
	stw	r8, 24*4(r4)
	stw	r9, 25*4(r4)
	stw	r10, 26*4(r4)
	stw	r11, 27*4(r4)
	stw	r12, 28*4(r4)
	stw	r13, 29*4(r4)
	stw	r14, 30*4(r4)
	stw	r15, 31*4(r4)
	
	salsa8_core
	
	lwz	r0, 0*4(r4)
	lwz	r5, 1*4(r4)
	lwz	r6, 2*4(r4)
	lwz	r7, 3*4(r4)
	add	r16, r16, r0
	add	r17, r17, r5
	add	r18, r18, r6
	add	r19, r19, r7
	lwz	r0, 4*4(r4)
	lwz	r5, 5*4(r4)
	lwz	r6, 6*4(r4)
	lwz	r7, 7*4(r4)
	add	r20, r20, r0
	add	r21, r21, r5
	add	r22, r22, r6
	add	r23, r23, r7
	lwz	r0, 8*4(r4)
	lwz	r5, 9*4(r4)
	lwz	r6, 10*4(r4)
	lwz	r7, 11*4(r4)
	add	r24, r24, r0
	add	r25, r25, r5
	add	r26, r26, r6
	add	r27, r27, r7
	lwz	r0, 12*4(r4)
	lwz	r5, 13*4(r4)
	lwz	r6, 14*4(r4)
	lwz	r7, 15*4(r4)
	add	r28, r28, r0
	add	r29, r29, r5
	add	r30, r30, r6
	add	r31, r31, r7
	
	stw	r16, 24*4(r1)
	stw	r17, 25*4(r1)
	stw	r18, 26*4(r1)
	stw	r19, 27*4(r1)
	stw	r20, 28*4(r1)
	stw	r21, 29*4(r1)
	stw	r22, 30*4(r1)
	stw	r23, 31*4(r1)
	stw	r24, 32*4(r1)
	stw	r25, 33*4(r1)
	stw	r26, 34*4(r1)
	stw	r27, 35*4(r1)
	stw	r28, 36*4(r1)
	stw	r29, 37*4(r1)
	stw	r30, 38*4(r1)
	stw	r31, 39*4(r1)
	
	lwz	r0, 40*4(r1)
	lwz	r5, 41*4(r1)
	lwz	r6, 42*4(r1)
	lwz	r7, 43*4(r1)
	xor	r16, r16, r0
	xor	r17, r17, r5
	xor	r18, r18, r6
	xor	r19, r19, r7
	lwz	r0, 44*4(r1)
	lwz	r5, 45*4(r1)
	lwz	r6, 46*4(r1)
	lwz	r7, 47*4(r1)
	xor	r20, r20, r0
	xor	r21, r21, r5
	xor	r22, r22, r6
	xor	r23, r23, r7
	xor	r24, r24, r8
	xor	r25, r25, r9
	xor	r26, r26, r10
	xor	r27, r27, r11
	xor	r28, r28, r12
	xor	r29, r29, r13
	xor	r30, r30, r14
	xor	r31, r31, r15
	stw	r16, 40*4(r1)
	stw	r17, 41*4(r1)
	stw	r18, 42*4(r1)
	stw	r19, 43*4(r1)
	mr	r8, r24
	mr	r9, r25
	mr	r10, r26
	mr	r11, r27
	stw	r20, 44*4(r1)
	stw	r21, 45*4(r1)
	stw	r22, 46*4(r1)
	stw	r23, 47*4(r1)
	mr	r12, r28
	mr	r13, r29
	mr	r14, r30
	mr	r15, r31
	
	salsa8_core
	
	lwz	r0, 40*4(r1)
	lwz	r5, 41*4(r1)
	lwz	r6, 42*4(r1)
	lwz	r7, 43*4(r1)
	add	r16, r16, r0
	add	r17, r17, r5
	add	r18, r18, r6
	add	r19, r19, r7
	lwz	r0, 44*4(r1)
	lwz	r5, 45*4(r1)
	lwz	r6, 46*4(r1)
	lwz	r7, 47*4(r1)
	add	r20, r20, r0
	add	r21, r21, r5
	add	r22, r22, r6
	add	r23, r23, r7
	add	r8, r8, r24
	add	r9, r9, r25
	add	r10, r10, r26
	add	r11, r11, r27
	stw	r16, 40*4(r1)
	stw	r17, 41*4(r1)
	stw	r18, 42*4(r1)
	stw	r19, 43*4(r1)
	add	r12, r12, r28
	add	r13, r13, r29
	add	r14, r14, r30
	add	r15, r15, r31
	stw	r20, 44*4(r1)
	stw	r21, 45*4(r1)
	stw	r22, 46*4(r1)
	stw	r23, 47*4(r1)
	
	addi	r4, r4, 32*4
	bdnz	scrypt_core_loop1
	
	lwz	r5, 2*4(r1)
	slwi	r3, r5, 7
	subf	r4, r3, r4
	mtctr	r5
	addi	r5, r5, -1
	stw	r5, 2*4(r1)
scrypt_core_loop2:
	and	r3, r16, r5
	slwi	r3, r3, 7
	add	r3, r3, r4
	mr	r0, r16
	mr	r5, r17
	mr	r6, r18
	mr	r7, r19
	lwz	r16, 24*4(r1)
	lwz	r17, 25*4(r1)
	lwz	r18, 26*4(r1)
	lwz	r19, 27*4(r1)
	lwz	r20, 28*4(r1)
	lwz	r21, 29*4(r1)
	lwz	r22, 30*4(r1)
	lwz	r23, 31*4(r1)
	lwz	r24, 32*4(r1)
	lwz	r25, 33*4(r1)
	lwz	r26, 34*4(r1)
	lwz	r27, 35*4(r1)
	lwz	r28, 36*4(r1)
	lwz	r29, 37*4(r1)
	lwz	r30, 38*4(r1)
	lwz	r31, 39*4(r1)
	xor	r16, r16, r0
	xor	r17, r17, r5
	xor	r18, r18, r6
	xor	r19, r19, r7
	lwz	r0, 44*4(r1)
	lwz	r5, 45*4(r1)
	lwz	r6, 46*4(r1)
	lwz	r7, 47*4(r1)
	xor	r20, r20, r0
	xor	r21, r21, r5
	xor	r22, r22, r6
	xor	r23, r23, r7
	xor	r24, r24, r8
	xor	r25, r25, r9
	xor	r26, r26, r10
	xor	r27, r27, r11
	xor	r28, r28, r12
	xor	r29, r29, r13
	xor	r30, r30, r14
	xor	r31, r31, r15
	
	lwz	r0, 0*4(r3)
	lwz	r5, 1*4(r3)
	lwz	r6, 2*4(r3)
	lwz	r7, 3*4(r3)
	xor	r16, r16, r0
	xor	r17, r17, r5
	xor	r18, r18, r6
	xor	r19, r19, r7
	lwz	r0, 4*4(r3)
	lwz	r5, 5*4(r3)
	lwz	r6, 6*4(r3)
	lwz	r7, 7*4(r3)
	xor	r20, r20, r0
	xor	r21, r21, r5
	xor	r22, r22, r6
	xor	r23, r23, r7
	lwz	r0, 8*4(r3)
	lwz	r5, 9*4(r3)
	lwz	r6, 10*4(r3)
	lwz	r7, 11*4(r3)
	xor	r24, r24, r0
	xor	r25, r25, r5
	xor	r26, r26, r6
	xor	r27, r27, r7
	lwz	r0, 12*4(r3)
	lwz	r5, 13*4(r3)
	lwz	r6, 14*4(r3)
	lwz	r7, 15*4(r3)
	xor	r28, r28, r0
	xor	r29, r29, r5
	xor	r30, r30, r6
	xor	r31, r31, r7
	
	stw	r16, 24*4(r1)
	stw	r17, 25*4(r1)
	stw	r18, 26*4(r1)
	stw	r19, 27*4(r1)
	stw	r20, 28*4(r1)
	stw	r21, 29*4(r1)
	stw	r22, 30*4(r1)
	stw	r23, 31*4(r1)
	stw	r24, 32*4(r1)
	stw	r25, 33*4(r1)
	stw	r26, 34*4(r1)
	stw	r27, 35*4(r1)
	stw	r28, 36*4(r1)
	stw	r29, 37*4(r1)
	stw	r30, 38*4(r1)
	stw	r31, 39*4(r1)
	
	salsa8_core
	
	lwz	r0, 24*4(r1)
	lwz	r5, 25*4(r1)
	lwz	r6, 26*4(r1)
	lwz	r7, 27*4(r1)
	add	r16, r16, r0
	add	r17, r17, r5
	add	r18, r18, r6
	add	r19, r19, r7
	lwz	r0, 28*4(r1)
	lwz	r5, 29*4(r1)
	lwz	r6, 30*4(r1)
	lwz	r7, 31*4(r1)
	add	r20, r20, r0
	add	r21, r21, r5
	add	r22, r22, r6
	add	r23, r23, r7
	lwz	r0, 32*4(r1)
	lwz	r5, 33*4(r1)
	lwz	r6, 34*4(r1)
	lwz	r7, 35*4(r1)
	add	r24, r24, r0
	add	r25, r25, r5
	add	r26, r26, r6
	add	r27, r27, r7
	lwz	r0, 36*4(r1)
	lwz	r5, 37*4(r1)
	lwz	r6, 38*4(r1)
	lwz	r7, 39*4(r1)
	add	r28, r28, r0
	add	r29, r29, r5
	add	r30, r30, r6
	add	r31, r31, r7
	
	stw	r16, 24*4(r1)
	stw	r17, 25*4(r1)
	stw	r18, 26*4(r1)
	stw	r19, 27*4(r1)
	stw	r20, 28*4(r1)
	stw	r21, 29*4(r1)
	stw	r22, 30*4(r1)
	stw	r23, 31*4(r1)
	stw	r24, 32*4(r1)
	stw	r25, 33*4(r1)
	stw	r26, 34*4(r1)
	stw	r27, 35*4(r1)
	stw	r28, 36*4(r1)
	stw	r29, 37*4(r1)
	stw	r30, 38*4(r1)
	stw	r31, 39*4(r1)
	
	lwz	r0, 16*4(r3)
	lwz	r5, 17*4(r3)
	lwz	r6, 18*4(r3)
	lwz	r7, 19*4(r3)
	xor	r16, r16, r0
	xor	r17, r17, r5
	xor	r18, r18, r6
	xor	r19, r19, r7
	lwz	r0, 20*4(r3)
	lwz	r5, 21*4(r3)
	lwz	r6, 22*4(r3)
	lwz	r7, 23*4(r3)
	xor	r20, r20, r0
	xor	r21, r21, r5
	xor	r22, r22, r6
	xor	r23, r23, r7
	lwz	r0, 24*4(r3)
	lwz	r5, 25*4(r3)
	lwz	r6, 26*4(r3)
	lwz	r7, 27*4(r3)
	xor	r24, r24, r0
	xor	r25, r25, r5
	xor	r26, r26, r6
	xor	r27, r27, r7
	lwz	r0, 28*4(r3)
	lwz	r5, 29*4(r3)
	lwz	r6, 30*4(r3)
	lwz	r7, 31*4(r3)
	xor	r28, r28, r0
	xor	r29, r29, r5
	xor	r30, r30, r6
	xor	r31, r31, r7
	
	lwz	r0, 40*4(r1)
	lwz	r5, 41*4(r1)
	lwz	r6, 42*4(r1)
	lwz	r7, 43*4(r1)
	xor	r16, r16, r0
	xor	r17, r17, r5
	xor	r18, r18, r6
	xor	r19, r19, r7
	lwz	r0, 44*4(r1)
	lwz	r5, 45*4(r1)
	lwz	r6, 46*4(r1)
	lwz	r7, 47*4(r1)
	xor	r20, r20, r0
	xor	r21, r21, r5
	xor	r22, r22, r6
	xor	r23, r23, r7
	xor	r24, r24, r8
	xor	r25, r25, r9
	xor	r26, r26, r10
	xor	r27, r27, r11
	xor	r28, r28, r12
	xor	r29, r29, r13
	xor	r30, r30, r14
	xor	r31, r31, r15
	stw	r16, 40*4(r1)
	stw	r17, 41*4(r1)
	stw	r18, 42*4(r1)
	stw	r19, 43*4(r1)
	mr	r8, r24
	mr	r9, r25
	mr	r10, r26
	mr	r11, r27
	stw	r20, 44*4(r1)
	stw	r21, 45*4(r1)
	stw	r22, 46*4(r1)
	stw	r23, 47*4(r1)
	mr	r12, r28
	mr	r13, r29
	mr	r14, r30
	mr	r15, r31
	
	salsa8_core
	
	lwz	r0, 40*4(r1)
	lwz	r5, 41*4(r1)
	lwz	r6, 42*4(r1)
	lwz	r7, 43*4(r1)
	add	r16, r16, r0
	add	r17, r17, r5
	add	r18, r18, r6
	add	r19, r19, r7
	lwz	r0, 44*4(r1)
	lwz	r5, 45*4(r1)
	lwz	r6, 46*4(r1)
	lwz	r7, 47*4(r1)
	add	r20, r20, r0
	add	r21, r21, r5
	add	r22, r22, r6
	add	r23, r23, r7
	lwz	r5, 2*4(r1)
	add	r8, r8, r24
	add	r9, r9, r25
	add	r10, r10, r26
	add	r11, r11, r27
	add	r12, r12, r28
	add	r13, r13, r29
	add	r14, r14, r30
	add	r15, r15, r31
	stw	r16, 40*4(r1)
	stw	r17, 41*4(r1)
	stw	r18, 42*4(r1)
	stw	r19, 43*4(r1)
	stw	r20, 44*4(r1)
	stw	r21, 45*4(r1)
	stw	r22, 46*4(r1)
	stw	r23, 47*4(r1)
	bdnz	scrypt_core_loop2
	
	ld	r3, 22*4(r1)
	
	lwz	r16, 24*4(r1)
	lwz	r17, 25*4(r1)
	lwz	r18, 26*4(r1)
	lwz	r19, 27*4(r1)
	lwz	r20, 28*4(r1)
	lwz	r21, 29*4(r1)
	lwz	r22, 30*4(r1)
	lwz	r23, 31*4(r1)
	stw	r16, 0*4(r3)
	stw	r17, 1*4(r3)
	stw	r18, 2*4(r3)
	stw	r19, 3*4(r3)
	stw	r20, 4*4(r3)
	stw	r21, 5*4(r3)
	stw	r22, 6*4(r3)
	stw	r23, 7*4(r3)
	lwz	r24, 32*4(r1)
	lwz	r25, 33*4(r1)
	lwz	r26, 34*4(r1)
	lwz	r27, 35*4(r1)
	lwz	r28, 36*4(r1)
	lwz	r29, 37*4(r1)
	lwz	r30, 38*4(r1)
	lwz	r31, 39*4(r1)
	stw	r24, 8*4(r3)
	stw	r25, 9*4(r3)
	stw	r26, 10*4(r3)
	stw	r27, 11*4(r3)
	stw	r28, 12*4(r3)
	stw	r29, 13*4(r3)
	stw	r30, 14*4(r3)
	stw	r31, 15*4(r3)
	lwz	r16, 40*4(r1)
	lwz	r17, 41*4(r1)
	lwz	r18, 42*4(r1)
	lwz	r19, 43*4(r1)
	lwz	r20, 44*4(r1)
	lwz	r21, 45*4(r1)
	lwz	r22, 46*4(r1)
	lwz	r23, 47*4(r1)
	stw	r16, 16*4(r3)
	stw	r17, 17*4(r3)
	stw	r18, 18*4(r3)
	stw	r19, 19*4(r3)
	stw	r20, 20*4(r3)
	stw	r21, 21*4(r3)
	stw	r22, 22*4(r3)
	stw	r23, 23*4(r3)
	stw	r8, 24*4(r3)
	stw	r9, 25*4(r3)
	stw	r10, 26*4(r3)
	stw	r11, 27*4(r3)
	stw	r12, 28*4(r3)
	stw	r13, 29*4(r3)
	stw	r14, 30*4(r3)
	stw	r15, 31*4(r3)
	
	ld	r13, 4*4(r1)
	ld	r14, 6*4(r1)
	ld	r15, 8*4(r1)
	ld	r16, 10*4(r1)
	ld	r17, 12*4(r1)
	ld	r18, 14*4(r1)
	ld	r19, 16*4(r1)
	ld	r20, 18*4(r1)
	ld	r21, 20*4(r1)
	ld	r22, 48*4(r1)
	ld	r23, 50*4(r1)
	ld	r24, 52*4(r1)
	ld	r25, 54*4(r1)
	ld	r26, 56*4(r1)
	ld	r27, 58*4(r1)
	ld	r28, 60*4(r1)
	ld	r29, 62*4(r1)
	ld	r30, 64*4(r1)
	ld	r31, 66*4(r1)
	addi	r1, r1, 68*4
	blr

#endif /* __ALTIVEC__ */

#endif
