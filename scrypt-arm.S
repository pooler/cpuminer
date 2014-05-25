/*
 * Copyright 2012, 2014 pooler@litecoinpool.org
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.  See COPYING for more details.
 */

#include "cpuminer-config.h"

#if defined(USE_ASM) && defined(__arm__) && defined(__APCS_32__)

#if defined(__ARM_ARCH_5E__) || defined(__ARM_ARCH_5TE__) || \
	defined(__ARM_ARCH_5TEJ__) || defined(__ARM_ARCH_6__) || \
	defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || \
	defined(__ARM_ARCH_6M__) || defined(__ARM_ARCH_6T2__) || \
	defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
#define __ARM_ARCH_5E_OR_6__
#endif

#if defined(__ARM_ARCH_5E_OR_6__) || defined(__ARM_ARCH_7__) || \
	defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || \
	defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
#define __ARM_ARCH_5E_OR_6_OR_7__
#endif

#ifdef __ARM_ARCH_5E_OR_6__

.macro scrypt_shuffle
	add	lr, r0, #9*4
	ldmia	r0, {r2-r7}
	ldmia	lr, {r2, r8-r12, lr}
	str	r3, [r0, #5*4]
	str	r5, [r0, #15*4]
	str	r6, [r0, #12*4]
	str	r7, [r0, #1*4]
	ldr r5, [r0, #7*4]
	str	r2, [r0, #13*4]
	str	r8, [r0, #2*4]
	strd	r4, [r0, #10*4]
	str	r9, [r0, #7*4]
	str	r10, [r0, #4*4]
	str	r11, [r0, #9*4]
	str	lr, [r0, #3*4]
	
	add	r2, r0, #64+0*4
	add	lr, r0, #64+9*4
	ldmia	r2, {r2-r7}
	ldmia	lr, {r2, r8-r12, lr}
	str	r3, [r0, #64+5*4]
	str	r5, [r0, #64+15*4]
	str	r6, [r0, #64+12*4]
	str	r7, [r0, #64+1*4]
	ldr r5, [r0, #64+7*4]
	str	r2, [r0, #64+13*4]
	str	r8, [r0, #64+2*4]
	strd	r4, [r0, #64+10*4]
	str	r9, [r0, #64+7*4]
	str	r10, [r0, #64+4*4]
	str	r11, [r0, #64+9*4]
	str	lr, [r0, #64+3*4]
.endm

.macro salsa8_core_doubleround_body
	add	r6, r2, r6
	add	r7, r3, r7
	eor	r10, r10, r6, ror #25
	add	r6, r0, r4
	eor	r11, r11, r7, ror #25
	add	r7, r1, r5
	strd	r10, [sp, #14*4]
	eor	r12, r12, r6, ror #25
	eor	lr, lr, r7, ror #25
	
	ldrd	r6, [sp, #10*4]
	add	r2, r10, r2
	add	r3, r11, r3
	eor	r6, r6, r2, ror #23
	add	r2, r12, r0
	eor	r7, r7, r3, ror #23
	add	r3, lr, r1
	strd	r6, [sp, #10*4]
	eor	r8, r8, r2, ror #23
	eor	r9, r9, r3, ror #23
	
	ldrd	r2, [sp, #6*4]
	add	r10, r6, r10
	add	r11, r7, r11
	eor	r2, r2, r10, ror #19
	add	r10, r8, r12
	eor	r3, r3, r11, ror #19
	add	r11, r9, lr
	eor	r4, r4, r10, ror #19
	eor	r5, r5, r11, ror #19
	
	ldrd	r10, [sp, #2*4]
	add	r6, r2, r6
	add	r7, r3, r7
	eor	r10, r10, r6, ror #14
	add	r6, r4, r8
	eor	r11, r11, r7, ror #14
	add	r7, r5, r9
	eor	r0, r0, r6, ror #14
	eor	r1, r1, r7, ror #14
	
	
	ldrd	r6, [sp, #14*4]
	strd	r2, [sp, #6*4]
	strd	r10, [sp, #2*4]
	add	r6, r11, r6
	add	r7, r0, r7
	eor	r4, r4, r6, ror #25
	add	r6, r1, r12
	eor	r5, r5, r7, ror #25
	add	r7, r10, lr
	eor	r2, r2, r6, ror #25
	eor	r3, r3, r7, ror #25
	strd	r2, [sp, #6*4]
	
	add	r10, r3, r10
	ldrd	r6, [sp, #10*4]
	add	r11, r4, r11
	eor	r8, r8, r10, ror #23
	add	r10, r5, r0
	eor	r9, r9, r11, ror #23
	add	r11, r2, r1
	eor	r6, r6, r10, ror #23
	eor	r7, r7, r11, ror #23
	strd	r6, [sp, #10*4]
	
	add	r2, r7, r2
	ldrd	r10, [sp, #14*4]
	add	r3, r8, r3
	eor	r12, r12, r2, ror #19
	add	r2, r9, r4
	eor	lr, lr, r3, ror #19
	add	r3, r6, r5
	eor	r10, r10, r2, ror #19
	eor	r11, r11, r3, ror #19
	
	ldrd	r2, [sp, #2*4]
	add	r6, r11, r6
	add	r7, r12, r7
	eor	r0, r0, r6, ror #14
	add	r6, lr, r8
	eor	r1, r1, r7, ror #14
	add	r7, r10, r9
	eor	r2, r2, r6, ror #14
	eor	r3, r3, r7, ror #14
.endm

.macro salsa8_core
	ldmia	sp, {r0-r12, lr}
	
	ldrd	r10, [sp, #14*4]
	salsa8_core_doubleround_body
	ldrd	r6, [sp, #6*4]
	strd	r2, [sp, #2*4]
	strd	r10, [sp, #14*4]
	salsa8_core_doubleround_body
	ldrd	r6, [sp, #6*4]
	strd	r2, [sp, #2*4]
	strd	r10, [sp, #14*4]
	salsa8_core_doubleround_body
	ldrd	r6, [sp, #6*4]
	strd	r2, [sp, #2*4]
	strd	r10, [sp, #14*4]
	salsa8_core_doubleround_body
	
	stmia	sp, {r0-r5}
	strd	r8, [sp, #8*4]
	str	r12, [sp, #12*4]
	str	lr, [sp, #13*4]
	strd	r10, [sp, #14*4]
.endm

#else

.macro scrypt_shuffle
.endm

.macro salsa8_core_doubleround_body
	ldr	r8, [sp, #8*4]
	add	r11, r11, r10
	ldr	lr, [sp, #13*4]
	add	r12, r12, r3
	eor	r2, r2, r11, ror #23
	add	r11, r4, r0
	eor	r7, r7, r12, ror #23
	add	r12, r9, r5
	str	r9, [sp, #9*4]
	eor	r8, r8, r11, ror #23
	str	r10, [sp, #14*4]
	eor	lr, lr, r12, ror #23
	
	ldr	r11, [sp, #11*4]
	add	r9, lr, r9
	ldr	r12, [sp, #12*4]
	add	r10, r2, r10
	eor	r1, r1, r9, ror #19
	add	r9, r7, r3
	eor	r6, r6, r10, ror #19
	add	r10, r8, r4
	str	r8, [sp, #8*4]
	eor	r11, r11, r9, ror #19
	str	lr, [sp, #13*4]
	eor	r12, r12, r10, ror #19
	
	ldr	r9, [sp, #10*4]
	add	r8, r12, r8
	ldr	r10, [sp, #15*4]
	add	lr, r1, lr
	eor	r0, r0, r8, ror #14
	add	r8, r6, r2
	eor	r5, r5, lr, ror #14
	add	lr, r11, r7
	eor	r9, r9, r8, ror #14
	ldr	r8, [sp, #9*4]
	eor	r10, r10, lr, ror #14
	ldr	lr, [sp, #14*4]
	
	
	add	r8, r9, r8
	str	r9, [sp, #10*4]
	add	lr, r10, lr
	str	r10, [sp, #15*4]
	eor	r11, r11, r8, ror #25
	add	r8, r0, r3
	eor	r12, r12, lr, ror #25
	add	lr, r5, r4
	eor	r1, r1, r8, ror #25
	ldr	r8, [sp, #8*4]
	eor	r6, r6, lr, ror #25
	
	add	r9, r11, r9
	ldr	lr, [sp, #13*4]
	add	r10, r12, r10
	eor	r8, r8, r9, ror #23
	add	r9, r1, r0
	eor	lr, lr, r10, ror #23
	add	r10, r6, r5
	str	r11, [sp, #11*4]
	eor	r2, r2, r9, ror #23
	str	r12, [sp, #12*4]
	eor	r7, r7, r10, ror #23
	
	ldr	r9, [sp, #9*4]
	add	r11, r8, r11
	ldr	r10, [sp, #14*4]
	add	r12, lr, r12
	eor	r9, r9, r11, ror #19
	add	r11, r2, r1
	eor	r10, r10, r12, ror #19
	add	r12, r7, r6
	str	r8, [sp, #8*4]
	eor	r3, r3, r11, ror #19
	str	lr, [sp, #13*4]
	eor	r4, r4, r12, ror #19
.endm

.macro salsa8_core
	ldmia	sp, {r0-r7}
	
	ldr	r12, [sp, #15*4]
	ldr	r8, [sp, #11*4]
	ldr	lr, [sp, #12*4]
	
	ldr	r9, [sp, #9*4]
	add	r8, r8, r12
	ldr	r11, [sp, #10*4]
	add	lr, lr, r0
	eor	r3, r3, r8, ror #25
	add	r8, r5, r1
	ldr	r10, [sp, #14*4]
	eor	r4, r4, lr, ror #25
	add	lr, r11, r6
	eor	r9, r9, r8, ror #25
	eor	r10, r10, lr, ror #25
	
	salsa8_core_doubleround_body
	
	ldr	r11, [sp, #10*4]
	add	r8, r9, r8
	ldr	r12, [sp, #15*4]
	add	lr, r10, lr
	eor	r11, r11, r8, ror #14
	add	r8, r3, r2
	eor	r12, r12, lr, ror #14
	add	lr, r4, r7
	eor	r0, r0, r8, ror #14
	ldr	r8, [sp, #11*4]
	eor	r5, r5, lr, ror #14
	ldr	lr, [sp, #12*4]
	
	add	r8, r8, r12
	str	r11, [sp, #10*4]
	add	lr, lr, r0
	str	r12, [sp, #15*4]
	eor	r3, r3, r8, ror #25
	add	r8, r5, r1
	eor	r4, r4, lr, ror #25
	add	lr, r11, r6
	str	r9, [sp, #9*4]
	eor	r9, r9, r8, ror #25
	str	r10, [sp, #14*4]
	eor	r10, r10, lr, ror #25
	
	salsa8_core_doubleround_body
	
	ldr	r11, [sp, #10*4]
	add	r8, r9, r8
	ldr	r12, [sp, #15*4]
	add	lr, r10, lr
	eor	r11, r11, r8, ror #14
	add	r8, r3, r2
	eor	r12, r12, lr, ror #14
	add	lr, r4, r7
	eor	r0, r0, r8, ror #14
	ldr	r8, [sp, #11*4]
	eor	r5, r5, lr, ror #14
	ldr	lr, [sp, #12*4]
	
	add	r8, r8, r12
	str	r11, [sp, #10*4]
	add	lr, lr, r0
	str	r12, [sp, #15*4]
	eor	r3, r3, r8, ror #25
	add	r8, r5, r1
	eor	r4, r4, lr, ror #25
	add	lr, r11, r6
	str	r9, [sp, #9*4]
	eor	r9, r9, r8, ror #25
	str	r10, [sp, #14*4]
	eor	r10, r10, lr, ror #25
	
	salsa8_core_doubleround_body
	
	ldr	r11, [sp, #10*4]
	add	r8, r9, r8
	ldr	r12, [sp, #15*4]
	add	lr, r10, lr
	eor	r11, r11, r8, ror #14
	add	r8, r3, r2
	eor	r12, r12, lr, ror #14
	add	lr, r4, r7
	eor	r0, r0, r8, ror #14
	ldr	r8, [sp, #11*4]
	eor	r5, r5, lr, ror #14
	ldr	lr, [sp, #12*4]
	
	add	r8, r8, r12
	str	r11, [sp, #10*4]
	add	lr, lr, r0
	str	r12, [sp, #15*4]
	eor	r3, r3, r8, ror #25
	add	r8, r5, r1
	eor	r4, r4, lr, ror #25
	add	lr, r11, r6
	str	r9, [sp, #9*4]
	eor	r9, r9, r8, ror #25
	str	r10, [sp, #14*4]
	eor	r10, r10, lr, ror #25
	
	salsa8_core_doubleround_body
	
	ldr	r11, [sp, #10*4]
	add	r8, r9, r8
	ldr	r12, [sp, #15*4]
	add	lr, r10, lr
	str	r9, [sp, #9*4]
	eor	r11, r11, r8, ror #14
	eor	r12, r12, lr, ror #14
	add	r8, r3, r2
	str	r10, [sp, #14*4]
	add	lr, r4, r7
	str	r11, [sp, #10*4]
	eor	r0, r0, r8, ror #14
	str	r12, [sp, #15*4]
	eor	r5, r5, lr, ror #14
	
	stmia	sp, {r0-r7}
.endm

#endif


.macro scrypt_core_macro1a_x4
	ldmia	r0, {r4-r7}
	ldmia	lr!, {r8-r11}
	stmia	r1!, {r4-r7}
	stmia	r3!, {r8-r11}
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	stmia	r0!, {r4-r7}
	stmia	r12!, {r4-r7}
.endm

.macro scrypt_core_macro1b_x4
	ldmia	r3!, {r8-r11}
	ldmia	r2, {r4-r7}
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	ldmia	r0, {r4-r7}
	stmia	r2!, {r8-r11}
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	ldmia	r1!, {r8-r11}
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	stmia	r0!, {r4-r7}
	stmia	r12!, {r4-r7}
.endm

.macro scrypt_core_macro2_x4
	ldmia	r12, {r4-r7}
	ldmia	r0, {r8-r11}
	add	r4, r4, r8
	add	r5, r5, r9
	add	r6, r6, r10
	add	r7, r7, r11
	stmia	r0!, {r4-r7}
	ldmia	r2, {r8-r11}
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	stmia	r2!, {r4-r7}
	stmia	r12!, {r4-r7}
.endm

.macro scrypt_core_macro3_x4
	ldmia	r1!, {r4-r7}
	ldmia	r0, {r8-r11}
	add	r4, r4, r8
	add	r5, r5, r9
	add	r6, r6, r10
	add	r7, r7, r11
	stmia	r0!, {r4-r7}
.endm

.macro scrypt_core_macro3_x6
	ldmia	r1!, {r2-r7}
	ldmia	r0, {r8-r12, lr}
	add	r2, r2, r8
	add	r3, r3, r9
	add	r4, r4, r10
	add	r5, r5, r11
	add	r6, r6, r12
	add	r7, r7, lr
	stmia	r0!, {r2-r7}
.endm


	.text
	.code 32
	.align 2
	.globl scrypt_core
	.globl _scrypt_core
#ifdef __ELF__
	.type scrypt_core, %function
#endif
scrypt_core:
_scrypt_core:
	stmfd	sp!, {r4-r11, lr}
	mov	r12, sp
	sub	sp, sp, #22*4
	bic	sp, sp, #63
	str	r12, [sp, #20*4]
	str	r2, [sp, #21*4]
	
	scrypt_shuffle
	
	ldr	r2, [sp, #21*4]
	str	r0, [sp, #16*4]
	add	r12, r1, r2, lsl #7
	str	r12, [sp, #18*4]
scrypt_core_loop1:
	add	lr, r0, #16*4
	add	r3, r1, #16*4
	mov	r12, sp
	scrypt_core_macro1a_x4
	scrypt_core_macro1a_x4
	scrypt_core_macro1a_x4
	scrypt_core_macro1a_x4
	str	r1, [sp, #17*4]
	
	salsa8_core
	
	ldr	r0, [sp, #16*4]
	mov	r12, sp
	add	r2, r0, #16*4
	scrypt_core_macro2_x4
	scrypt_core_macro2_x4
	scrypt_core_macro2_x4
	scrypt_core_macro2_x4
	
	salsa8_core
	
	ldr	r0, [sp, #16*4]
	mov	r1, sp
	add	r0, r0, #16*4
	scrypt_core_macro3_x6
	scrypt_core_macro3_x6
	ldr	r3, [sp, #17*4]
	ldr	r12, [sp, #18*4]
	scrypt_core_macro3_x4
	
	add	r1, r3, #16*4
	sub	r0, r0, #32*4
	cmp	r1, r12
	bne	scrypt_core_loop1
	
	ldr	r12, [sp, #21*4]
	ldr	r4, [r0, #16*4]
	sub	r2, r12, #1
	str	r2, [sp, #21*4]
	sub	r1, r1, r12, lsl #7
	str	r1, [sp, #17*4]
	and	r4, r4, r2
	add	r1, r1, r4, lsl #7
scrypt_core_loop2:
	add	r2, r0, #16*4
	add	r3, r1, #16*4
	str	r12, [sp, #18*4]
	mov	r12, sp
#ifdef __ARM_ARCH_5E_OR_6_OR_7__
	pld [r1, #24*4]
	pld [r1, #8*4]
#endif
	scrypt_core_macro1b_x4
	scrypt_core_macro1b_x4
	scrypt_core_macro1b_x4
	scrypt_core_macro1b_x4
	
	salsa8_core
	
	ldr	r0, [sp, #16*4]
	mov	r12, sp
	add	r2, r0, #16*4
	scrypt_core_macro2_x4
	scrypt_core_macro2_x4
	scrypt_core_macro2_x4
	scrypt_core_macro2_x4
	
	salsa8_core
	
	ldr	r0, [sp, #16*4]
	mov	r1, sp
	ldr	r3, [sp, #17*4]
	add	r0, r0, #16*4
	ldr	r2, [sp, #21*4]
	scrypt_core_macro3_x4
	and	r4, r4, r2
	add	r3, r3, r4, lsl #7
	str	r3, [sp, #19*4]
#ifdef __ARM_ARCH_5E_OR_6_OR_7__
	pld	[r3, #16*4]
	pld	[r3]
#endif
	scrypt_core_macro3_x6
	scrypt_core_macro3_x6
	
	ldr	r12, [sp, #18*4]
	sub	r0, r0, #32*4
	ldr	r1, [sp, #19*4]
	subs	r12, r12, #1
	bne	scrypt_core_loop2
	
	scrypt_shuffle
	
	ldr	sp, [sp, #20*4]
#ifdef __thumb__
	ldmfd	sp!, {r4-r11, lr}
	bx	lr
#else
	ldmfd	sp!, {r4-r11, pc}
#endif


#ifdef __ARM_NEON__

.macro salsa8_core_3way_doubleround
	ldrd	r6, [sp, #6*4]
	vadd.u32	q4, q0, q1
	add	r6, r2, r6
	vadd.u32	q6, q8, q9
	add	r7, r3, r7
	vshl.u32	q5, q4, #7
	eor	r10, r10, r6, ror #25
	vshl.u32	q7, q6, #7
	add	r6, r0, r4
	vshr.u32	q4, q4, #32-7
	eor	r11, r11, r7, ror #25
	vshr.u32	q6, q6, #32-7
	add	r7, r1, r5
	veor.u32	q3, q3, q5
	strd	r10, [sp, #14*4]
	veor.u32	q11, q11, q7
	eor	r12, r12, r6, ror #25
	veor.u32	q3, q3, q4
	eor	lr, lr, r7, ror #25
	veor.u32	q11, q11, q6
	
	ldrd	r6, [sp, #10*4]
	vadd.u32	q4, q3, q0
	add	r2, r10, r2
	vadd.u32	q6, q11, q8
	add	r3, r11, r3
	vshl.u32	q5, q4, #9
	eor	r6, r6, r2, ror #23
	vshl.u32	q7, q6, #9
	add	r2, r12, r0
	vshr.u32	q4, q4, #32-9
	eor	r7, r7, r3, ror #23
	vshr.u32	q6, q6, #32-9
	add	r3, lr, r1
	veor.u32	q2, q2, q5
	strd	r6, [sp, #10*4]
	veor.u32	q10, q10, q7
	eor	r8, r8, r2, ror #23
	veor.u32	q2, q2, q4
	eor	r9, r9, r3, ror #23
	veor.u32	q10, q10, q6
	
	ldrd	r2, [sp, #6*4]
	vadd.u32	q4, q2, q3
	add	r10, r6, r10
	vadd.u32	q6, q10, q11
	add	r11, r7, r11
	vext.u32	q3, q3, q3, #3
	eor	r2, r2, r10, ror #19
	vshl.u32	q5, q4, #13
	add	r10, r8, r12
	vext.u32	q11, q11, q11, #3
	eor	r3, r3, r11, ror #19
	vshl.u32	q7, q6, #13
	add	r11, r9, lr
	vshr.u32	q4, q4, #32-13
	eor	r4, r4, r10, ror #19
	vshr.u32	q6, q6, #32-13
	eor	r5, r5, r11, ror #19
	veor.u32	q1, q1, q5
	veor.u32	q9, q9, q7
	veor.u32	q1, q1, q4
	veor.u32	q9, q9, q6
	
	ldrd	r10, [sp, #2*4]
	vadd.u32	q4, q1, q2
	add	r6, r2, r6
	vadd.u32	q6, q9, q10
	add	r7, r3, r7
	vswp.u32	d4, d5
	eor	r10, r10, r6, ror #14
	vshl.u32	q5, q4, #18
	add	r6, r4, r8
	vswp.u32	d20, d21
	eor	r11, r11, r7, ror #14
	vshl.u32	q7, q6, #18
	add	r7, r5, r9
	vshr.u32	q4, q4, #32-18
	eor	r0, r0, r6, ror #14
	vshr.u32	q6, q6, #32-18
	eor	r1, r1, r7, ror #14
	veor.u32	q0, q0, q5
	ldrd	r6, [sp, #14*4]
	veor.u32	q8, q8, q7
	veor.u32	q0, q0, q4
	veor.u32	q8, q8, q6
	
	
	strd	r2, [sp, #6*4]
	vadd.u32	q4, q0, q3
	strd	r10, [sp, #2*4]
	vadd.u32	q6, q8, q11
	add	r6, r11, r6
	vext.u32	q1, q1, q1, #1
	add	r7, r0, r7
	vshl.u32	q5, q4, #7
	eor	r4, r4, r6, ror #25
	vext.u32	q9, q9, q9, #1
	add	r6, r1, r12
	vshl.u32	q7, q6, #7
	eor	r5, r5, r7, ror #25
	vshr.u32	q4, q4, #32-7
	add	r7, r10, lr
	vshr.u32	q6, q6, #32-7
	eor	r2, r2, r6, ror #25
	veor.u32	q1, q1, q5
	eor	r3, r3, r7, ror #25
	veor.u32	q9, q9, q7
	strd	r2, [sp, #6*4]
	veor.u32	q1, q1, q4
	veor.u32	q9, q9, q6
	
	add	r10, r3, r10
	vadd.u32	q4, q1, q0
	ldrd	r6, [sp, #10*4]
	vadd.u32	q6, q9, q8
	add	r11, r4, r11
	vshl.u32	q5, q4, #9
	eor	r8, r8, r10, ror #23
	vshl.u32	q7, q6, #9
	add	r10, r5, r0
	vshr.u32	q4, q4, #32-9
	eor	r9, r9, r11, ror #23
	vshr.u32	q6, q6, #32-9
	add	r11, r2, r1
	veor.u32	q2, q2, q5
	eor	r6, r6, r10, ror #23
	veor.u32	q10, q10, q7
	eor	r7, r7, r11, ror #23
	veor.u32	q2, q2, q4
	strd	r6, [sp, #10*4]
	veor.u32	q10, q10, q6
	
	add	r2, r7, r2
	vadd.u32	q4, q2, q1
	ldrd	r10, [sp, #14*4]
	vadd.u32	q6, q10, q9
	add	r3, r8, r3
	vext.u32	q1, q1, q1, #3
	eor	r12, r12, r2, ror #19
	vshl.u32	q5, q4, #13
	add	r2, r9, r4
	vext.u32	q9, q9, q9, #3
	eor	lr, lr, r3, ror #19
	vshl.u32	q7, q6, #13
	add	r3, r6, r5
	vshr.u32	q4, q4, #32-13
	eor	r10, r10, r2, ror #19
	vshr.u32	q6, q6, #32-13
	eor	r11, r11, r3, ror #19
	veor.u32	q3, q3, q5
	veor.u32	q11, q11, q7
	veor.u32	q3, q3, q4
	veor.u32	q11, q11, q6
	
	ldrd	r2, [sp, #2*4]
	vadd.u32	q4, q3, q2
	add	r6, r11, r6
	vadd.u32	q6, q11, q10
	add	r7, r12, r7
	vswp.u32	d4, d5
	eor	r0, r0, r6, ror #14
	vshl.u32	q5, q4, #18
	add	r6, lr, r8
	vswp.u32	d20, d21
	eor	r1, r1, r7, ror #14
	vshl.u32	q7, q6, #18
	add	r7, r10, r9
	vext.u32	q3, q3, q3, #1
	eor	r2, r2, r6, ror #14
	vshr.u32	q4, q4, #32-18
	eor	r3, r3, r7, ror #14
	vshr.u32	q6, q6, #32-18
	strd	r2, [sp, #2*4]
	vext.u32	q11, q11, q11, #1
	strd	r10, [sp, #14*4]
	veor.u32	q0, q0, q5
	veor.u32	q8, q8, q7
	veor.u32	q0, q0, q4
	veor.u32	q8, q8, q6
.endm

.macro salsa8_core_3way
	ldmia	sp, {r0-r12, lr}
	ldrd	r10, [sp, #14*4]
	salsa8_core_3way_doubleround
	salsa8_core_3way_doubleround
	salsa8_core_3way_doubleround
	salsa8_core_3way_doubleround
	stmia	sp, {r0-r5}
	strd	r8, [sp, #8*4]
	str	r12, [sp, #12*4]
	str	lr, [sp, #13*4]
.endm

	.text
	.code 32
	.align 2
	.globl scrypt_core_3way
	.globl _scrypt_core_3way
#ifdef __ELF__
	.type scrypt_core_3way, %function
#endif
scrypt_core_3way:
_scrypt_core_3way:
	stmfd	sp!, {r4-r11, lr}
	vpush	{q4-q7}
	mov	r12, sp
	sub	sp, sp, #24*16
	bic	sp, sp, #63
	str	r2, [sp, #4*16+3*4]
	str	r12, [sp, #4*16+4*4]
	
	mov	r3, r0
	vldmia	r3!, {q8-q15}
	vmov.u64	q0, #0xffffffff
	vmov.u32	q1, q8
	vmov.u32	q2, q12
	vbif.u32	q8, q9, q0
	vbif.u32	q12, q13, q0
	vbif.u32	q9, q10, q0
	vbif.u32	q13, q14, q0
	vbif.u32	q10, q11, q0
	vbif.u32	q14, q15, q0
	vbif.u32	q11, q1, q0
	vbif.u32	q15, q2, q0
	vldmia	r3!, {q0-q7}
	vswp.u32	d17, d21
	vswp.u32	d25, d29
	vswp.u32	d18, d22
	vswp.u32	d26, d30
	vstmia	r0, {q8-q15}
	vmov.u64	q8, #0xffffffff
	vmov.u32	q9, q0
	vmov.u32	q10, q4
	vbif.u32	q0, q1, q8
	vbif.u32	q4, q5, q8
	vbif.u32	q1, q2, q8
	vbif.u32	q5, q6, q8
	vbif.u32	q2, q3, q8
	vbif.u32	q6, q7, q8
	vbif.u32	q3, q9, q8
	vbif.u32	q7, q10, q8
	vldmia	r3, {q8-q15}
	vswp.u32	d1, d5
	vswp.u32	d9, d13
	vswp.u32	d2, d6
	vswp.u32	d10, d14
	add	r12, sp, #8*16
	vstmia	r12!, {q0-q7}
	vmov.u64	q0, #0xffffffff
	vmov.u32	q1, q8
	vmov.u32	q2, q12
	vbif.u32	q8, q9, q0
	vbif.u32	q12, q13, q0
	vbif.u32	q9, q10, q0
	vbif.u32	q13, q14, q0
	vbif.u32	q10, q11, q0
	vbif.u32	q14, q15, q0
	vbif.u32	q11, q1, q0
	vbif.u32	q15, q2, q0
	vswp.u32	d17, d21
	vswp.u32	d25, d29
	vswp.u32	d18, d22
	vswp.u32	d26, d30
	vstmia	r12, {q8-q15}
	
	add	lr, sp, #128
	vldmia	lr, {q0-q7}
	add	r2, r1, r2, lsl #7
	str	r0, [sp, #4*16+0*4]
	str	r2, [sp, #4*16+2*4]
scrypt_core_3way_loop1:
	add	lr, r0, #16*4
	add	r3, r1, #16*4
	str	r1, [sp, #4*16+1*4]
	mov	r12, sp
	scrypt_core_macro1a_x4
	scrypt_core_macro1a_x4
	scrypt_core_macro1a_x4
	ldr	r2, [sp, #4*16+3*4]
	scrypt_core_macro1a_x4
	sub	r1, r1, #4*16
	
	add	r1, r1, r2, lsl #7
	vstmia	r1, {q0-q7}
	add	r3, r1, r2, lsl #7
	vstmia	r3, {q8-q15}
	
	add	lr, sp, #128
	veor.u32	q0, q0, q4
	veor.u32	q1, q1, q5
	veor.u32	q2, q2, q6
	veor.u32	q3, q3, q7
	vstmia	lr, {q0-q3}
	veor.u32	q8, q8, q12
	veor.u32	q9, q9, q13
	veor.u32	q10, q10, q14
	veor.u32	q11, q11, q15
	add	r12, sp, #256
	vstmia	r12, {q8-q11}
	
	salsa8_core_3way
	
	ldr	r0, [sp, #4*16+0*4]
	mov	r12, sp
	add	r2, r0, #16*4
	scrypt_core_macro2_x4
	scrypt_core_macro2_x4
	scrypt_core_macro2_x4
	scrypt_core_macro2_x4
	
	add	lr, sp, #128
	vldmia	lr, {q4-q7}
	vadd.u32	q4, q4, q0
	vadd.u32	q5, q5, q1
	vadd.u32	q6, q6, q2
	vadd.u32	q7, q7, q3
	add	r12, sp, #256
	vldmia	r12, {q0-q3}
	vstmia	lr, {q4-q7}
	vadd.u32	q8, q8, q0
	vadd.u32	q9, q9, q1
	vadd.u32	q10, q10, q2
	vadd.u32	q11, q11, q3
	
	add	r4, sp, #128+4*16
	vldmia	r4, {q0-q3}
	vstmia	r12, {q8-q11}
	veor.u32	q0, q0, q4
	veor.u32	q1, q1, q5
	veor.u32	q2, q2, q6
	veor.u32	q3, q3, q7
	vstmia	r4, {q0-q3}
	veor.u32	q8, q8, q12
	veor.u32	q9, q9, q13
	veor.u32	q10, q10, q14
	veor.u32	q11, q11, q15
	vmov	q12, q8
	vmov	q13, q9
	vmov	q14, q10
	vmov	q15, q11
	
	salsa8_core_3way
	
	ldr	r0, [sp, #4*16+0*4]
	mov	r1, sp
	add	r0, r0, #16*4
	scrypt_core_macro3_x6
	scrypt_core_macro3_x6
	scrypt_core_macro3_x4
	sub	r0, r0, #8*16
	
	ldr	r1, [sp, #4*16+1*4]
	ldr	r2, [sp, #4*16+2*4]
	add	lr, sp, #128
	add	r4, sp, #128+4*16
	vldmia	r4, {q4-q7}
	vadd.u32	q4, q4, q0
	vadd.u32	q5, q5, q1
	vadd.u32	q6, q6, q2
	vadd.u32	q7, q7, q3
	vstmia	r4, {q4-q7}
	vldmia	lr, {q0-q3}
	vadd.u32	q12, q12, q8
	vadd.u32	q13, q13, q9
	vadd.u32	q14, q14, q10
	vadd.u32	q15, q15, q11
	add	r12, sp, #256
	vldmia	r12, {q8-q11}
	
	add	r1, r1, #8*16
	cmp	r1, r2
	bne	scrypt_core_3way_loop1
	
	ldr	r2, [sp, #4*16+3*4]
	add	r5, sp, #256+4*16
	vstmia	r5, {q12-q15}
	
	sub	r1, r1, r2, lsl #7
	str	r1, [sp, #4*16+1*4]
scrypt_core_3way_loop2:
	str	r2, [sp, #4*16+2*4]
	
	ldr	r0, [sp, #4*16+0*4]
	ldr	r1, [sp, #4*16+1*4]
	ldr	r2, [sp, #4*16+3*4]
	ldr	r4, [r0, #16*4]
	sub	r2, r2, #1
	and	r4, r4, r2
	add	r1, r1, r4, lsl #7
	add	r2, r0, #16*4
	add	r3, r1, #16*4
	mov	r12, sp
	scrypt_core_macro1b_x4
	scrypt_core_macro1b_x4
	scrypt_core_macro1b_x4
	scrypt_core_macro1b_x4
	
	ldr	r1, [sp, #4*16+1*4]
	ldr	r2, [sp, #4*16+3*4]
	add	r1, r1, r2, lsl #7
	add	r3, r1, r2, lsl #7
	sub	r2, r2, #1
	vmov	r6, r7, d8
	and	r6, r6, r2
	add	r6, r1, r6, lsl #7
	vmov	r7, r8, d24
	add	lr, sp, #128
	vldmia	lr, {q0-q3}
	pld	[r6]
	pld	[r6, #8*4]
	pld	[r6, #16*4]
	pld	[r6, #24*4]
	vldmia	r6, {q8-q15}
	and	r7, r7, r2
	add	r7, r3, r7, lsl #7
	veor.u32	q8, q8, q0
	veor.u32	q9, q9, q1
	veor.u32	q10, q10, q2
	veor.u32	q11, q11, q3
	pld	[r7]
	pld	[r7, #8*4]
	pld	[r7, #16*4]
	pld	[r7, #24*4]
	veor.u32	q12, q12, q4
	veor.u32	q13, q13, q5
	veor.u32	q14, q14, q6
	veor.u32	q15, q15, q7
	vldmia	r7, {q0-q7}
	vstmia	lr, {q8-q15}
	add	r12, sp, #256
	vldmia	r12, {q8-q15}
	veor.u32	q8, q8, q0
	veor.u32	q9, q9, q1
	veor.u32	q10, q10, q2
	veor.u32	q11, q11, q3
	veor.u32	q12, q12, q4
	veor.u32	q13, q13, q5
	veor.u32	q14, q14, q6
	veor.u32	q15, q15, q7
	
	vldmia	lr, {q0-q7}
	veor.u32	q0, q0, q4
	veor.u32	q1, q1, q5
	veor.u32	q2, q2, q6
	veor.u32	q3, q3, q7
	vstmia	lr, {q0-q3}
	veor.u32	q8, q8, q12
	veor.u32	q9, q9, q13
	veor.u32	q10, q10, q14
	veor.u32	q11, q11, q15
	vstmia	r12, {q8-q15}
	
	salsa8_core_3way
	
	ldr	r0, [sp, #4*16+0*4]
	mov	r12, sp
	add	r2, r0, #16*4
	scrypt_core_macro2_x4
	scrypt_core_macro2_x4
	scrypt_core_macro2_x4
	scrypt_core_macro2_x4
	
	add	lr, sp, #128
	vldmia	lr, {q4-q7}
	vadd.u32	q4, q4, q0
	vadd.u32	q5, q5, q1
	vadd.u32	q6, q6, q2
	vadd.u32	q7, q7, q3
	add	r12, sp, #256
	vldmia	r12, {q12-q15}
	vstmia	lr, {q4-q7}
	vadd.u32	q12, q12, q8
	vadd.u32	q13, q13, q9
	vadd.u32	q14, q14, q10
	vadd.u32	q15, q15, q11
	
	add	r4, sp, #128+4*16
	vldmia	r4, {q0-q3}
	vstmia	r12, {q12-q15}
	veor.u32	q0, q0, q4
	veor.u32	q1, q1, q5
	veor.u32	q2, q2, q6
	veor.u32	q3, q3, q7
	add	r5, sp, #256+4*16
	vldmia	r5, {q8-q11}
	vstmia	r4, {q0-q3}
	veor.u32	q8, q8, q12
	veor.u32	q9, q9, q13
	veor.u32	q10, q10, q14
	veor.u32	q11, q11, q15
	vmov	q12, q8
	vmov	q13, q9
	vmov	q14, q10
	vmov	q15, q11
	
	salsa8_core_3way
	
	ldr	r0, [sp, #4*16+0*4]
	ldr	r3, [sp, #4*16+1*4]
	ldr	r2, [sp, #4*16+3*4]
	mov	r1, sp
	add	r0, r0, #16*4
	sub	r2, r2, #1
	scrypt_core_macro3_x4
	and	r4, r4, r2
	add	r3, r3, r4, lsl #7
	pld	[r3, #16*4]
	pld	[r3]
	pld	[r3, #24*4]
	pld	[r3, #8*4]
	scrypt_core_macro3_x6
	scrypt_core_macro3_x6
	
	add	lr, sp, #128
	add	r4, sp, #128+4*16
	vldmia	r4, {q4-q7}
	vadd.u32	q4, q4, q0
	vadd.u32	q5, q5, q1
	vadd.u32	q6, q6, q2
	vadd.u32	q7, q7, q3
	vstmia	r4, {q4-q7}
	vadd.u32	q12, q12, q8
	vadd.u32	q13, q13, q9
	vadd.u32	q14, q14, q10
	vadd.u32	q15, q15, q11
	add	r5, sp, #256+4*16
	vstmia	r5, {q12-q15}
	
	ldr	r2, [sp, #4*16+2*4]
	subs	r2, r2, #1
	bne	scrypt_core_3way_loop2
	
	ldr	r0, [sp, #4*16+0*4]
	vldmia	r0, {q8-q15}
	vmov.u64	q0, #0xffffffff
	vmov.u32	q1, q8
	vmov.u32	q2, q12
	vbif.u32	q8, q9, q0
	vbif.u32	q12, q13, q0
	vbif.u32	q9, q10, q0
	vbif.u32	q13, q14, q0
	vbif.u32	q10, q11, q0
	vbif.u32	q14, q15, q0
	vbif.u32	q11, q1, q0
	vbif.u32	q15, q2, q0
	add	r12, sp, #8*16
	vldmia	r12!, {q0-q7}
	vswp.u32	d17, d21
	vswp.u32	d25, d29
	vswp.u32	d18, d22
	vswp.u32	d26, d30
	vstmia	r0!, {q8-q15}
	vmov.u64	q8, #0xffffffff
	vmov.u32	q9, q0
	vmov.u32	q10, q4
	vbif.u32	q0, q1, q8
	vbif.u32	q4, q5, q8
	vbif.u32	q1, q2, q8
	vbif.u32	q5, q6, q8
	vbif.u32	q2, q3, q8
	vbif.u32	q6, q7, q8
	vbif.u32	q3, q9, q8
	vbif.u32	q7, q10, q8
	vldmia	r12, {q8-q15}
	vswp.u32	d1, d5
	vswp.u32	d9, d13
	vswp.u32	d2, d6
	vswp.u32	d10, d14
	vstmia	r0!, {q0-q7}
	vmov.u64	q0, #0xffffffff
	vmov.u32	q1, q8
	vmov.u32	q2, q12
	vbif.u32	q8, q9, q0
	vbif.u32	q12, q13, q0
	vbif.u32	q9, q10, q0
	vbif.u32	q13, q14, q0
	vbif.u32	q10, q11, q0
	vbif.u32	q14, q15, q0
	vbif.u32	q11, q1, q0
	vbif.u32	q15, q2, q0
	vswp.u32	d17, d21
	vswp.u32	d25, d29
	vswp.u32	d18, d22
	vswp.u32	d26, d30
	vstmia	r0, {q8-q15}
	
	ldr	sp, [sp, #4*16+4*4]
	vpop	{q4-q7}
	ldmfd	sp!, {r4-r11, pc}

#endif /* __ARM_NEON__ */

#endif
