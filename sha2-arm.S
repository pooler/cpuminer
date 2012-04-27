/*
 * Copyright 2012 pooler@litecoinpool.org
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.  See COPYING for more details.
 */

#include "cpuminer-config.h"

#if defined(__arm__) && defined(__APCS_32__)

.macro sha256_k
	.align 2
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
.endm

.macro sha256_extend_round i, rw, ra, rb, ry, rz
	ldr	lr, [\rw, #(\i+1)*4]
	mov	r12, \ry, ror #17
	eor	r12, r12, \ry, ror #19
	eor	r12, r12, \ry, lsr #10
	add	r11, r11, r12
	add	r11, r11, \ra
	mov	r12, lr, ror #7
	eor	r12, r12, lr, ror #18
	eor	r12, r12, lr, lsr #3
	add	\ra, r11, r12
	str	\ra, [\rw, #(\i+16)*4]
.endm

.macro sha256_extend_doubleround i, rw, ra, rb, ry, rz
	ldr	lr, [\rw, #(\i+1)*4]
	mov	r12, \ry, ror #17
	eor	r12, r12, \ry, ror #19
	eor	r12, r12, \ry, lsr #10
	add	r11, r11, r12
	add	r11, r11, \ra
	mov	r12, lr, ror #7
	eor	r12, r12, lr, ror #18
	eor	r12, r12, lr, lsr #3
	add	\ra, r11, r12
	str	\ra, [\rw, #(\i+16)*4]

	ldr	r11, [\rw, #(\i+2)*4]
	mov	r12, \rz, ror #17
	eor	r12, r12, \rz, ror #19
	eor	r12, r12, \rz, lsr #10
	add	lr, lr, r12
	add	lr, lr, \rb
	mov	r12, r11, ror #7
	eor	r12, r12, r11, ror #18
	eor	r12, r12, r11, lsr #3
	add	\rb, lr, r12
	str	\rb, [\rw, #(\i+17)*4]
.endm

.macro sha256_main_round i, ka, rw, ra, rb, rc, rd, re, rf, rg, rh
	ldr	r12, [\rw, #(\i)*4]
	and	r3, \rf, \re
	bic	lr, \rg, \re
	orr	lr, lr, r3
	ldr	r3, \ka + (\i)*4
	add	r12, r12, lr
	eor	lr, \re, \re, ror #5
	eor	lr, lr, \re, ror #19
	add	r12, r12, \rh
	add	r12, r12, r3
	add	r12, r12, lr, ror #6
	add	\rh, \rd, r12

	eor	lr, \ra, \rb
	and	lr, lr, \rc
	and	r3, \ra, \rb
	eor	lr, lr, r3
	eor	r3, \ra, \ra, ror #11
	eor	r3, r3, \ra, ror #20
	add	r12, r12, lr
	add	\rd, r12, r3, ror #2
.endm

.macro sha256_main_quadround i, ka, rw
	sha256_main_round \i+0, \ka, \rw, r4, r5, r6, r7, r8, r9, r10, r11
	sha256_main_round \i+1, \ka, \rw, r7, r4, r5, r6, r11, r8, r9, r10
	sha256_main_round \i+2, \ka, \rw, r6, r7, r4, r5, r10, r11, r8, r9
	sha256_main_round \i+3, \ka, \rw, r5, r6, r7, r4, r9, r10, r11, r8
.endm


	.text
	.code 32
	.align 2
	.globl sha256_transform
	.globl _sha256_transform
sha256_transform:
_sha256_transform:
	stmfd	sp!, {r4-r11, lr}
	cmp	r2, #0
	sub	sp, sp, #64*4
	bne	sha256_transform_swap
	
	ldmia	r1!, {r4-r11}
	stmia	sp, {r4-r11}
	add	r3, sp, #8*4
	ldmia	r1, {r4-r11}
	stmia	r3, {r4-r11}
	b	sha256_transform_extend

.macro bswap rd, rn
	eor	r12, \rn, \rn, ror #16
	bic	r12, r12, #0x00ff0000
	mov	\rd, \rn, ror #8
	eor	\rd, \rd, r12, lsr #8
.endm

sha256_transform_swap:
	ldmia	r1!, {r4-r11}
	bswap	r4, r4
	bswap	r5, r5
	bswap	r6, r6
	bswap	r7, r7
	bswap	r8, r8
	bswap	r9, r9
	bswap	r10, r10
	bswap	r11, r11
	stmia	sp, {r4-r11}
	add	r3, sp, #8*4
	ldmia	r1, {r4-r11}
	bswap	r4, r4
	bswap	r5, r5
	bswap	r6, r6
	bswap	r7, r7
	bswap	r8, r8
	bswap	r9, r9
	bswap	r10, r10
	bswap	r11, r11
	stmia	r3, {r4-r11}
	
sha256_transform_extend:
	add	r12, sp, #9*4
	ldr	r11, [sp, #0*4]
	ldmia	r12, {r4-r10}
	sha256_extend_doubleround  0, sp, r4, r5, r9, r10
	sha256_extend_doubleround  2, sp, r6, r7, r4, r5
	sha256_extend_doubleround  4, sp, r8, r9, r6, r7
	sha256_extend_doubleround  6, sp, r10, r4, r8, r9
	sha256_extend_doubleround  8, sp, r5, r6, r10, r4
	sha256_extend_doubleround 10, sp, r7, r8, r5, r6
	sha256_extend_doubleround 12, sp, r9, r10, r7, r8
	sha256_extend_doubleround 14, sp, r4, r5, r9, r10
	sha256_extend_doubleround 16, sp, r6, r7, r4, r5
	sha256_extend_doubleround 18, sp, r8, r9, r6, r7
	sha256_extend_doubleround 20, sp, r10, r4, r8, r9
	sha256_extend_doubleround 22, sp, r5, r6, r10, r4
	sha256_extend_doubleround 24, sp, r7, r8, r5, r6
	sha256_extend_doubleround 26, sp, r9, r10, r7, r8
	sha256_extend_doubleround 28, sp, r4, r5, r9, r10
	sha256_extend_doubleround 30, sp, r6, r7, r4, r5
	sha256_extend_doubleround 32, sp, r8, r9, r6, r7
	sha256_extend_doubleround 34, sp, r10, r4, r8, r9
	sha256_extend_doubleround 36, sp, r5, r6, r10, r4
	sha256_extend_doubleround 38, sp, r7, r8, r5, r6
	sha256_extend_doubleround 40, sp, r9, r10, r7, r8
	sha256_extend_doubleround 42, sp, r4, r5, r9, r10
	sha256_extend_doubleround 44, sp, r6, r7, r4, r5
	sha256_extend_doubleround 46, sp, r8, r9, r6, r7
	
	ldmia	r0, {r4-r11}
	sha256_main_quadround  0, sha256_transform_k, sp
	sha256_main_quadround  4, sha256_transform_k, sp
	sha256_main_quadround  8, sha256_transform_k, sp
	sha256_main_quadround 12, sha256_transform_k, sp
	sha256_main_quadround 16, sha256_transform_k, sp
	sha256_main_quadround 20, sha256_transform_k, sp
	sha256_main_quadround 24, sha256_transform_k, sp
	sha256_main_quadround 28, sha256_transform_k, sp
	b	sha256_transform_k_over
sha256_transform_k:
	sha256_k
sha256_transform_k_over:
	sha256_main_quadround 32, sha256_transform_k, sp
	sha256_main_quadround 36, sha256_transform_k, sp
	sha256_main_quadround 40, sha256_transform_k, sp
	sha256_main_quadround 44, sha256_transform_k, sp
	sha256_main_quadround 48, sha256_transform_k, sp
	sha256_main_quadround 52, sha256_transform_k, sp
	sha256_main_quadround 56, sha256_transform_k, sp
	sha256_main_quadround 60, sha256_transform_k, sp
	
	ldmia	r0, {r1, r2, r3, r12}
	add	r4, r4, r1
	add	r5, r5, r2
	add	r6, r6, r3
	add	r7, r7, r12
	stmia	r0!, {r4-r7}
	ldmia	r0, {r1, r2, r3, r12}
	add	r8, r8, r1
	add	r9, r9, r2
	add	r10, r10, r3
	add	r11, r11, r12
	stmia	r0, {r8-r11}
	
	add	sp, sp, #64*4
#ifdef __thumb__
	ldmfd	sp!, {r4-r11, lr}
	bx	lr
#else
	ldmfd	sp!, {r4-r11, pc}
#endif


	.text
	.code 32
	.align 2
	.globl sha256d_ms
	.globl _sha256d_ms
sha256d_ms:
_sha256d_ms:
	stmfd	sp!, {r4-r11, lr}
	sub	sp, sp, #64*4
	
	cmp	r0, r0
	
	ldr	lr, [r1, #3*4]
	ldr	r6, [r1, #18*4]
	ldr	r7, [r1, #19*4]
	ldr	r8, [r1, #20*4]
	ldr	r10, [r1, #22*4]
	ldr	r4, [r1, #23*4]
	ldr	r5, [r1, #24*4]
	ldr	r11, [r1, #30*4]
	str	r6, [sp, #18*4]
	str	r7, [sp, #19*4]
	str	r8, [sp, #20*4]
	str	r10, [sp, #21*4]
	str	r4, [sp, #22*4]
	str	r5, [sp, #23*4]
	str	r11, [sp, #24*4]
	
	mov	r12, lr, ror #7
	eor	r12, r12, lr, ror #18
	eor	r12, r12, lr, lsr #3
	add	r6, r6, r12
	str	r6, [r1, #18*4]
	
	add	r7, r7, lr
	str	r7, [r1, #19*4]
	
	mov	r12, r6, ror #17
	eor	r12, r12, r6, ror #19
	eor	r12, r12, r6, lsr #10
	add	r8, r8, r12
	str	r8, [r1, #20*4]
	
	mov	r12, r7, ror #17
	eor	r12, r12, r7, ror #19
	eor	r9, r12, r7, lsr #10
	str	r9, [r1, #21*4]
	
	mov	r12, r8, ror #17
	eor	r12, r12, r8, ror #19
	eor	r12, r12, r8, lsr #10
	add	r10, r10, r12
	str	r10, [r1, #22*4]
	
	mov	r12, r9, ror #17
	eor	r12, r12, r9, ror #19
	eor	r12, r12, r9, lsr #10
	add	r4, r4, r12
	str	r4, [r1, #23*4]
	
	mov	r12, r10, ror #17
	eor	r12, r12, r10, ror #19
	eor	r12, r12, r10, lsr #10
	add	r5, r5, r12
	str	r5, [r1, #24*4]
	
	mov	r12, r4, ror #17
	eor	r12, r12, r4, ror #19
	eor	r12, r12, r4, lsr #10
	add	r6, r6, r12
	str	r6, [r1, #25*4]
	
	mov	r12, r5, ror #17
	eor	r12, r12, r5, ror #19
	eor	r12, r12, r5, lsr #10
	add	r7, r7, r12
	str	r7, [r1, #26*4]
	
	mov	r12, r6, ror #17
	eor	r12, r12, r6, ror #19
	eor	r12, r12, r6, lsr #10
	add	r8, r8, r12
	str	r8, [r1, #27*4]
	
	mov	r12, r7, ror #17
	eor	r12, r12, r7, ror #19
	eor	r12, r12, r7, lsr #10
	add	r9, r9, r12
	str	r9, [r1, #28*4]
	
	mov	r12, r8, ror #17
	eor	r12, r12, r8, ror #19
	eor	r12, r12, r8, lsr #10
	add	r10, r10, r12
	str	r10, [r1, #29*4]
	
	ldr	lr, [r1, #31*4]
	mov	r12, r9, ror #17
	eor	r12, r12, r9, ror #19
	eor	r12, r12, r9, lsr #10
	add	r11, r11, r12
	add	r4, r4, r11
	str	r4, [r1, #30*4]
	
	str	lr, [sp, #25*4]
	ldr	r11, [r1, #16*4]
	mov	r12, r10, ror #17
	eor	r12, r12, r10, ror #19
	eor	r12, r12, r10, lsr #10
	add	lr, lr, r12
	add	r5, r5, lr
	str	r5, [r1, #31*4]
	
sha256d_ms_extend_loop2:
	sha256_extend_doubleround 16, r1, r6, r7, r4, r5
	sha256_extend_doubleround 18, r1, r8, r9, r6, r7
	sha256_extend_doubleround 20, r1, r10, r4, r8, r9
	sha256_extend_doubleround 22, r1, r5, r6, r10, r4
	sha256_extend_doubleround 24, r1, r7, r8, r5, r6
	sha256_extend_doubleround 26, r1, r9, r10, r7, r8
	sha256_extend_doubleround 28, r1, r4, r5, r9, r10
	sha256_extend_doubleround 30, r1, r6, r7, r4, r5
	sha256_extend_doubleround 32, r1, r8, r9, r6, r7
	sha256_extend_doubleround 34, r1, r10, r4, r8, r9
	sha256_extend_doubleround 36, r1, r5, r6, r10, r4
	sha256_extend_doubleround 38, r1, r7, r8, r5, r6
	sha256_extend_doubleround 40, r1, r9, r10, r7, r8
	sha256_extend_doubleround 42, r1, r4, r5, r9, r10
	bne	sha256d_ms_extend_coda2
	sha256_extend_doubleround 44, r1, r6, r7, r4, r5
	sha256_extend_doubleround 46, r1, r8, r9, r6, r7
	
	ldr	r4,  [r3, #0*4]
	ldr	r9,  [r3, #1*4]
	ldr	r10, [r3, #2*4]
	ldr	r11, [r3, #3*4]
	ldr	r8,  [r3, #4*4]
	ldr	r5,  [r3, #5*4]
	ldr	r6,  [r3, #6*4]
	ldr	r7,  [r3, #7*4]
	b	sha256d_ms_main_loop1
	
sha256d_ms_main_loop2:
	sha256_main_round  0, sha256d_ms_k, r1, r4, r5, r6, r7, r8, r9, r10, r11
	sha256_main_round  1, sha256d_ms_k, r1, r7, r4, r5, r6, r11, r8, r9, r10
	sha256_main_round  2, sha256d_ms_k, r1, r6, r7, r4, r5, r10, r11, r8, r9
sha256d_ms_main_loop1:
	sha256_main_round  3, sha256d_ms_k, r1, r5, r6, r7, r4, r9, r10, r11, r8
	sha256_main_quadround  4, sha256d_ms_k, r1
	sha256_main_quadround  8, sha256d_ms_k, r1
	sha256_main_quadround 12, sha256d_ms_k, r1
	sha256_main_quadround 16, sha256d_ms_k, r1
	sha256_main_quadround 20, sha256d_ms_k, r1
	sha256_main_quadround 24, sha256d_ms_k, r1
	sha256_main_quadround 28, sha256d_ms_k, r1
	b	sha256d_ms_k_over
sha256d_ms_k:
	sha256_k
sha256d_ms_k_over:
	sha256_main_quadround 32, sha256d_ms_k, r1
	sha256_main_quadround 36, sha256d_ms_k, r1
	sha256_main_quadround 40, sha256d_ms_k, r1
	sha256_main_quadround 44, sha256d_ms_k, r1
	sha256_main_quadround 48, sha256d_ms_k, r1
	sha256_main_quadround 52, sha256d_ms_k, r1
	sha256_main_round 56, sha256d_ms_k, r1, r4, r5, r6, r7, r8, r9, r10, r11
	bne	sha256d_ms_finish
	sha256_main_round 57, sha256d_ms_k, r1, r7, r4, r5, r6, r11, r8, r9, r10
	sha256_main_round 58, sha256d_ms_k, r1, r6, r7, r4, r5, r10, r11, r8, r9
	sha256_main_round 59, sha256d_ms_k, r1, r5, r6, r7, r4, r9, r10, r11, r8
	sha256_main_quadround 60, sha256d_ms_k, r1
	
	ldmia	r2!, {r3, r12, lr}
	add	r4, r4, r3
	add	r5, r5, r12
	add	r6, r6, lr
	stmia	sp, {r4-r6}
	ldmia	r2, {r3, r4, r5, r6, r12}
	add	lr, sp, #3*4
	add	r7, r7, r3
	add	r8, r8, r4
	add	r9, r9, r5
	add	r10, r10, r6
	add	r11, r11, r12
	add	r12, sp, #18*4
	stmia	lr!, {r7-r11}
	
	ldmia	r12, {r4-r11}
	str	r4,  [r1, #18*4]
	str	r5,  [r1, #19*4]
	str	r6,  [r1, #20*4]
	str	r7,  [r1, #22*4]
	str	r8,  [r1, #23*4]
	str	r9,  [r1, #24*4]
	str	r10, [r1, #30*4]
	str	r11, [r1, #31*4]
	
	mov	r3,  #0x80000000
	mov	r4,  #0
	mov	r5,  #0
	mov	r6,  #0
	mov	r7,  #0
	mov	r8,  #0
	mov	r9,  #0
	mov	r10, #0x00000100
	stmia	lr, {r3-r10}
	
	ldr	lr, [sp, #1*4]
	movs	r1, sp
	ldr	r4, [sp, #0*4]
	
	ldr	r11, [sp, #2*4]
	mov	r12, lr, ror #7
	eor	r12, r12, lr, ror #18
	eor	r12, r12, lr, lsr #3
	add	r4, r4, r12
	str	r4, [sp, #16*4]
	
	add	lr, lr, #0x00a00000
	mov	r12, r11, ror #7
	eor	r12, r12, r11, ror #18
	eor	r12, r12, r11, lsr #3
	add	r5, lr, r12
	str	r5, [sp, #17*4]
	
	ldr	lr, [sp, #3*4]
	mov	r12, r4, ror #17
	eor	r12, r12, r4, ror #19
	eor	r12, r12, r4, lsr #10
	add	r11, r11, r12
	mov	r12, lr, ror #7
	eor	r12, r12, lr, ror #18
	eor	r12, r12, lr, lsr #3
	add	r6, r11, r12
	str	r6, [sp, #18*4]
	
	ldr	r11, [sp, #4*4]
	mov	r12, r5, ror #17
	eor	r12, r12, r5, ror #19
	eor	r12, r12, r5, lsr #10
	add	lr, lr, r12
	mov	r12, r11, ror #7
	eor	r12, r12, r11, ror #18
	eor	r12, r12, r11, lsr #3
	add	r7, lr, r12
	str	r7, [sp, #19*4]
	
	ldr	lr, [sp, #5*4]
	mov	r12, r6, ror #17
	eor	r12, r12, r6, ror #19
	eor	r12, r12, r6, lsr #10
	add	r11, r11, r12
	mov	r12, lr, ror #7
	eor	r12, r12, lr, ror #18
	eor	r12, r12, lr, lsr #3
	add	r8, r11, r12
	str	r8, [sp, #20*4]
	
	ldr	r11, [sp, #6*4]
	mov	r12, r7, ror #17
	eor	r12, r12, r7, ror #19
	eor	r12, r12, r7, lsr #10
	add	lr, lr, r12
	mov	r12, r11, ror #7
	eor	r12, r12, r11, ror #18
	eor	r12, r12, r11, lsr #3
	add	r9, lr, r12
	str	r9, [sp, #21*4]
	
	ldr	lr, [sp, #7*4]
	mov	r12, r8, ror #17
	eor	r12, r12, r8, ror #19
	eor	r12, r12, r8, lsr #10
	add	r11, r11, r12
	add	r11, r11, #0x00000100
	mov	r12, lr, ror #7
	eor	r12, r12, lr, ror #18
	eor	r12, r12, lr, lsr #3
	add	r10, r11, r12
	str	r10, [sp, #22*4]
	
	mov	r12, r9, ror #17
	eor	r12, r12, r9, ror #19
	eor	r12, r12, r9, lsr #10
	add	lr, lr, r12
	add	lr, lr, r4
	add	lr, lr, #0x11000000
	add	r4, lr, #0x00002000
	str	r4, [sp, #23*4]
	
	mov	r12, r10, ror #17
	eor	r12, r12, r10, ror #19
	eor	r12, r12, r10, lsr #10
	add	r5, r5, r12
	add	r5, r5, #0x80000000
	str	r5, [sp, #24*4]

	mov	r12, r4, ror #17
	eor	r12, r12, r4, ror #19
	eor	r12, r12, r4, lsr #10
	add	r6, r6, r12
	str	r6, [sp, #25*4]
	
	mov	r12, r5, ror #17
	eor	r12, r12, r5, ror #19
	eor	r12, r12, r5, lsr #10
	add	r7, r7, r12
	str	r7, [sp, #26*4]
	
	mov	r12, r6, ror #17
	eor	r12, r12, r6, ror #19
	eor	r12, r12, r6, lsr #10
	add	r8, r8, r12
	str	r8, [sp, #27*4]
	
	mov	r12, r7, ror #17
	eor	r12, r12, r7, ror #19
	eor	r12, r12, r7, lsr #10
	add	r9, r9, r12
	str	r9, [sp, #28*4]
	
	mov	r12, r8, ror #17
	eor	r12, r12, r8, ror #19
	eor	r12, r12, r8, lsr #10
	add	r10, r10, r12
	str	r10, [sp, #29*4]
	
	mov	r12, r9, ror #17
	eor	r12, r12, r9, ror #19
	eor	r12, r12, r9, lsr #10
	add	r4, r4, r12
	add	r4, r4, #0x00400000
	add	r4, r4, #0x00000022
	str	r4, [sp, #30*4]
	
	ldr	r11, [sp, #16*4]
	mov	r12, r10, ror #17
	eor	r12, r12, r10, ror #19
	eor	r12, r12, r10, lsr #10
	add	lr, r12, #0x00000100
	add	lr, lr, r5
	mov	r12, r11, ror #7
	eor	r12, r12, r11, ror #18
	eor	r12, r12, r11, lsr #3
	add	r5, lr, r12
	str	r5, [sp, #31*4]
	
	b	sha256d_ms_extend_loop2
	
sha256d_ms_extend_coda2:
	sha256_extend_round 44, r1, r6, r7, r4, r5
	
	adr	r2, sha256d_ms_h
	ldmia	r2, {r4-r11}
	b	sha256d_ms_main_loop2

sha256d_ms_h:
	.long 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a
	.long 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19

.macro sha256_main_round_red i, ka, rw, rd, re, rf, rg, rh
	ldr	r12, [\rw, #(\i)*4]
	and	r3, \rf, \re
	bic	lr, \rg, \re
	orr	lr, lr, r3
	ldr	r3, \ka + (\i)*4
	add	r12, r12, lr
	eor	lr, \re, \re, ror #5
	eor	lr, lr, \re, ror #19
	add	r12, r12, \rh
	add	r12, r12, r3
	add	r12, r12, lr, ror #6
	add	\rh, \rd, r12
.endm
	
sha256d_ms_finish:
	sha256_main_round_red 57, sha256d_ms_k, r1, r6, r11, r8, r9, r10
	sha256_main_round_red 58, sha256d_ms_k, r1, r5, r10, r11, r8, r9
	sha256_main_round_red 59, sha256d_ms_k, r1, r4, r9, r10, r11, r8
	ldr	r5, [r2, #7*4]
	sha256_main_round_red 60, sha256d_ms_k, r1, r7, r8, r9, r10, r11
	
	add	r11, r11, r5
	str	r11, [r0, #7*4]
	
	add	sp, sp, #64*4
#ifdef __thumb__
	ldmfd	sp!, {r4-r11, lr}
	bx	lr
#else
	ldmfd	sp!, {r4-r11, pc}
#endif

#endif
