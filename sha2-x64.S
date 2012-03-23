/*
 * Copyright 2012 pooler@litecoinpool.org
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.  See COPYING for more details.
 */

#include "cpuminer-config.h"

#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif

#if defined(__x86_64__)

	.data
	.p2align 7
sha256_4h:
	.long 0x6a09e667, 0x6a09e667, 0x6a09e667, 0x6a09e667
	.long 0xbb67ae85, 0xbb67ae85, 0xbb67ae85, 0xbb67ae85
	.long 0x3c6ef372, 0x3c6ef372, 0x3c6ef372, 0x3c6ef372
	.long 0xa54ff53a, 0xa54ff53a, 0xa54ff53a, 0xa54ff53a
	.long 0x510e527f, 0x510e527f, 0x510e527f, 0x510e527f
	.long 0x9b05688c, 0x9b05688c, 0x9b05688c, 0x9b05688c
	.long 0x1f83d9ab, 0x1f83d9ab, 0x1f83d9ab, 0x1f83d9ab
	.long 0x5be0cd19, 0x5be0cd19, 0x5be0cd19, 0x5be0cd19

	.data
	.p2align 7
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

	.text
	.p2align 6
	.globl sha256_init_4way
	.globl _sha256_init_4way
sha256_init_4way:
_sha256_init_4way:
#if defined(WIN64)
	pushq	%rdi
	movq	%rcx, %rdi
#endif
	movdqa	sha256_4h+0(%rip), %xmm0
	movdqa	sha256_4h+16(%rip), %xmm1
	movdqa	sha256_4h+32(%rip), %xmm2
	movdqa	sha256_4h+48(%rip), %xmm3
	movdqu	%xmm0, 0(%rdi)
	movdqu	%xmm1, 16(%rdi)
	movdqu	%xmm2, 32(%rdi)
	movdqu	%xmm3, 48(%rdi)
	movdqa	sha256_4h+64(%rip), %xmm0
	movdqa	sha256_4h+80(%rip), %xmm1
	movdqa	sha256_4h+96(%rip), %xmm2
	movdqa	sha256_4h+112(%rip), %xmm3
	movdqu	%xmm0, 64(%rdi)
	movdqu	%xmm1, 80(%rdi)
	movdqu	%xmm2, 96(%rdi)
	movdqu	%xmm3, 112(%rdi)
#if defined(WIN64)
	popq	%rdi
#endif
	ret
	

.macro sha256_sse2_extend_round i
	movdqa	(\i-15)*16(%rcx), %xmm0
	movdqa	(\i-14)*16(%rcx), %xmm4
	movdqa	%xmm0, %xmm2
	movdqa	%xmm4, %xmm6
	psrld	$3, %xmm0
	psrld	$3, %xmm4
	movdqa	%xmm0, %xmm1
	movdqa	%xmm4, %xmm5
	pslld	$14, %xmm2
	pslld	$14, %xmm6
	psrld	$4, %xmm1
	psrld	$4, %xmm5
	pxor	%xmm1, %xmm0
	pxor	%xmm5, %xmm4
	psrld	$11, %xmm1
	psrld	$11, %xmm5
	pxor	%xmm2, %xmm0
	pxor	%xmm6, %xmm4
	pslld	$11, %xmm2
	pslld	$11, %xmm6
	pxor	%xmm1, %xmm0
	pxor	%xmm5, %xmm4
	pxor	%xmm2, %xmm0
	pxor	%xmm6, %xmm4

	movdqa	(\i-2)*16(%rcx), %xmm3
	movdqa	(\i-1)*16(%rcx), %xmm7
	paddd	(\i-16)*16(%rcx), %xmm0
	paddd	(\i-15)*16(%rcx), %xmm4

	movdqa	%xmm3, %xmm2
	movdqa	%xmm7, %xmm6
	psrld	$10, %xmm3
	psrld	$10, %xmm7
	movdqa	%xmm3, %xmm1
	movdqa	%xmm7, %xmm5

	paddd	(\i-7)*16(%rcx), %xmm0

	pslld	$13, %xmm2
	pslld	$13, %xmm6
	psrld	$7, %xmm1
	psrld	$7, %xmm5

	paddd	(\i-6)*16(%rcx), %xmm4

	pxor	%xmm1, %xmm3
	pxor	%xmm5, %xmm7
	psrld	$2, %xmm1
	psrld	$2, %xmm5
	pxor	%xmm2, %xmm3
	pxor	%xmm6, %xmm7
	pslld	$2, %xmm2
	pslld	$2, %xmm6
	pxor	%xmm1, %xmm3
	pxor	%xmm5, %xmm7
	pxor	%xmm2, %xmm3
	pxor	%xmm6, %xmm7

	paddd	%xmm3, %xmm0
	paddd	%xmm7, %xmm4
	movdqa	%xmm0, \i*16(%rcx)
	movdqa	%xmm4, (\i+1)*16(%rcx)
.endm

	.text
	.p2align 6
sha256_sse2_extend_loop:
	sha256_sse2_extend_round 0
	sha256_sse2_extend_round 2
	sha256_sse2_extend_round 4
	sha256_sse2_extend_round 6
	sha256_sse2_extend_round 8
	sha256_sse2_extend_round 10
	sha256_sse2_extend_round 12
	sha256_sse2_extend_round 14
	sha256_sse2_extend_round 16
	sha256_sse2_extend_round 18
	sha256_sse2_extend_round 20
	sha256_sse2_extend_round 22
	sha256_sse2_extend_round 24
	sha256_sse2_extend_round 26
	sha256_sse2_extend_round 28
	sha256_sse2_extend_round 30
	sha256_sse2_extend_round 32
	sha256_sse2_extend_round 34
	sha256_sse2_extend_round 36
	sha256_sse2_extend_round 38
	sha256_sse2_extend_round 40
	sha256_sse2_extend_round 42
	sha256_sse2_extend_round 44
	sha256_sse2_extend_round 46
	ret
	
.macro sha256_sse2_main_round i
	movdqa	16*\i(%rax), %xmm6
	paddd	16*\i(%rcx), %xmm6
	paddd	%xmm10, %xmm6

	movdqa	%xmm0, %xmm1
	movdqa	%xmm9, %xmm2
	pandn	%xmm2, %xmm1

	movdqa	%xmm2, %xmm10
	movdqa	%xmm8, %xmm2
	movdqa	%xmm2, %xmm9

	pand	%xmm0, %xmm2
	pxor	%xmm2, %xmm1
	movdqa	%xmm0, %xmm8

	paddd	%xmm1, %xmm6

	movdqa	%xmm0, %xmm1
	psrld	$6, %xmm0
	movdqa	%xmm0, %xmm2
	pslld	$7, %xmm1
	psrld	$5, %xmm2
	pxor	%xmm1, %xmm0
	pxor	%xmm2, %xmm0
	pslld	$14, %xmm1
	psrld	$14, %xmm2
	pxor	%xmm1, %xmm0
	pxor	%xmm2, %xmm0
	pslld	$5, %xmm1
	pxor	%xmm1, %xmm0
	paddd	%xmm0, %xmm6

	movdqa	%xmm3, %xmm0
	paddd	%xmm6, %xmm0

	movdqa	%xmm5, %xmm1
	movdqa	%xmm4, %xmm3
	movdqa	%xmm4, %xmm2
	pand	%xmm5, %xmm2
	pand	%xmm7, %xmm4
	pand	%xmm7, %xmm1
	pxor	%xmm4, %xmm1
	movdqa	%xmm5, %xmm4
	movdqa	%xmm7, %xmm5
	pxor	%xmm2, %xmm1
	paddd	%xmm1, %xmm6

	movdqa	%xmm7, %xmm2
	psrld	$2, %xmm7
	movdqa	%xmm7, %xmm1
	pslld	$10, %xmm2
	psrld	$11, %xmm1
	pxor	%xmm2, %xmm7
	pxor	%xmm1, %xmm7
	pslld	$9, %xmm2
	psrld	$9, %xmm1
	pxor	%xmm2, %xmm7
	pxor	%xmm1, %xmm7
	pslld	$11, %xmm2
	pxor	%xmm2, %xmm7
	paddd	%xmm6, %xmm7
.endm

	.text
	.p2align 6
sha256_sse2_main_loop:
	sha256_sse2_main_round 0
	sha256_sse2_main_round 1
	sha256_sse2_main_round 2
	sha256_sse2_main_round 3
	sha256_sse2_main_round 4
	sha256_sse2_main_round 5
	sha256_sse2_main_round 6
	sha256_sse2_main_round 7
	sha256_sse2_main_round 8
	sha256_sse2_main_round 9
	sha256_sse2_main_round 10
	sha256_sse2_main_round 11
	sha256_sse2_main_round 12
	sha256_sse2_main_round 13
	sha256_sse2_main_round 14
	sha256_sse2_main_round 15
	sha256_sse2_main_round 16
	sha256_sse2_main_round 17
	sha256_sse2_main_round 18
	sha256_sse2_main_round 19
	sha256_sse2_main_round 20
	sha256_sse2_main_round 21
	sha256_sse2_main_round 22
	sha256_sse2_main_round 23
	sha256_sse2_main_round 24
	sha256_sse2_main_round 25
	sha256_sse2_main_round 26
	sha256_sse2_main_round 27
	sha256_sse2_main_round 28
	sha256_sse2_main_round 29
	sha256_sse2_main_round 30
	sha256_sse2_main_round 31
	sha256_sse2_main_round 32
	sha256_sse2_main_round 33
	sha256_sse2_main_round 34
	sha256_sse2_main_round 35
	sha256_sse2_main_round 36
	sha256_sse2_main_round 37
	sha256_sse2_main_round 38
	sha256_sse2_main_round 39
	sha256_sse2_main_round 40
	sha256_sse2_main_round 41
	sha256_sse2_main_round 42
	sha256_sse2_main_round 43
	sha256_sse2_main_round 44
	sha256_sse2_main_round 45
	sha256_sse2_main_round 46
	sha256_sse2_main_round 47
	sha256_sse2_main_round 48
	sha256_sse2_main_round 49
	sha256_sse2_main_round 50
	sha256_sse2_main_round 51
	sha256_sse2_main_round 52
	sha256_sse2_main_round 53
	sha256_sse2_main_round 54
	sha256_sse2_main_round 55
	sha256_sse2_main_round 56
	sha256_sse2_main_round 57
	sha256_sse2_main_round 58
	sha256_sse2_main_round 59
	sha256_sse2_main_round 60
	sha256_sse2_main_round 61
	sha256_sse2_main_round 62
	sha256_sse2_main_round 63
	ret


#if defined(USE_AVX)	

.macro sha256_avx_extend_round i
	vmovdqa	(\i-15)*16(%rcx), %xmm0
	vmovdqa	(\i-14)*16(%rcx), %xmm4
	vpslld	$14, %xmm0, %xmm2
	vpslld	$14, %xmm4, %xmm6
	vpsrld	$3, %xmm0, %xmm0
	vpsrld	$3, %xmm4, %xmm4
	vpsrld	$4, %xmm0, %xmm1
	vpsrld	$4, %xmm4, %xmm5
	vpxor	%xmm1, %xmm0, %xmm0
	vpxor	%xmm5, %xmm4, %xmm4
	vpsrld	$11, %xmm1, %xmm1
	vpsrld	$11, %xmm5, %xmm5
	vpxor	%xmm2, %xmm0, %xmm0
	vpxor	%xmm6, %xmm4, %xmm4
	vpslld	$11, %xmm2, %xmm2
	vpslld	$11, %xmm6, %xmm6
	vpxor	%xmm1, %xmm0, %xmm0
	vpxor	%xmm5, %xmm4, %xmm4
	vpxor	%xmm2, %xmm0, %xmm0
	vpxor	%xmm6, %xmm4, %xmm4

	vmovdqa	(\i-2)*16(%rcx), %xmm3
	vmovdqa	(\i-1)*16(%rcx), %xmm7
	vpaddd	(\i-16)*16(%rcx), %xmm0, %xmm0
	vpaddd	(\i-15)*16(%rcx), %xmm4, %xmm4

	vpslld	$13, %xmm3, %xmm2
	vpslld	$13, %xmm7, %xmm6
	vpsrld	$10, %xmm3, %xmm3
	vpsrld	$10, %xmm7, %xmm7

	vpaddd	(\i-7)*16(%rcx), %xmm0, %xmm0

	vpsrld	$7, %xmm3, %xmm1
	vpsrld	$7, %xmm7, %xmm5

	vpaddd	(\i-6)*16(%rcx), %xmm4, %xmm4

	vpxor	%xmm1, %xmm3, %xmm3
	vpxor	%xmm5, %xmm7, %xmm7
	vpsrld	$2, %xmm1, %xmm1
	vpsrld	$2, %xmm5, %xmm5
	vpxor	%xmm2, %xmm3, %xmm3
	vpxor	%xmm6, %xmm7, %xmm7
	vpslld	$2, %xmm2, %xmm2
	vpslld	$2, %xmm6, %xmm6
	vpxor	%xmm1, %xmm3, %xmm3
	vpxor	%xmm5, %xmm7, %xmm7
	vpxor	%xmm2, %xmm3, %xmm3
	vpxor	%xmm6, %xmm7, %xmm7

	vpaddd	%xmm3, %xmm0, %xmm0
	vpaddd	%xmm7, %xmm4, %xmm4
	vmovdqa	%xmm0, \i*16(%rcx)
	vmovdqa	%xmm4, (\i+1)*16(%rcx)
.endm

	.text
	.p2align 6
sha256_avx_extend_loop:
	sha256_avx_extend_round 0
	sha256_avx_extend_round 2
	sha256_avx_extend_round 4
	sha256_avx_extend_round 6
	sha256_avx_extend_round 8
	sha256_avx_extend_round 10
	sha256_avx_extend_round 12
	sha256_avx_extend_round 14
	sha256_avx_extend_round 16
	sha256_avx_extend_round 18
	sha256_avx_extend_round 20
	sha256_avx_extend_round 22
	sha256_avx_extend_round 24
	sha256_avx_extend_round 26
	sha256_avx_extend_round 28
	sha256_avx_extend_round 30
	sha256_avx_extend_round 32
	sha256_avx_extend_round 34
	sha256_avx_extend_round 36
	sha256_avx_extend_round 38
	sha256_avx_extend_round 40
	sha256_avx_extend_round 42
	sha256_avx_extend_round 44
	sha256_avx_extend_round 46
	ret

.macro sha256_avx_main_round i, r0, r1, r2, r3, r4, r5, r6, r7
	vpaddd	16*(\i)(%rax), \r0, %xmm6
	vpaddd	16*(\i)(%rcx), %xmm6, %xmm6

	vpandn	\r1, \r3, %xmm1
	vpand	\r3, \r2, %xmm2
	vpxor	%xmm2, %xmm1, %xmm1
	vpaddd	%xmm1, %xmm6, %xmm6

	vpslld	$7, \r3, %xmm1
	vpsrld	$6, \r3, \r0
	vpsrld	$5, \r0, %xmm2
	vpxor	%xmm1, \r0, \r0
	vpxor	%xmm2, \r0, \r0
	vpslld	$14, %xmm1, %xmm1
	vpsrld	$14, %xmm2, %xmm2
	vpxor	%xmm1, \r0, \r0
	vpxor	%xmm2, \r0, \r0
	vpslld	$5, %xmm1, %xmm1
	vpxor	%xmm1, \r0, \r0
	vpaddd	\r0, %xmm6, %xmm6

	vpaddd	%xmm6, \r4, \r0

	vpand	\r6, \r5, %xmm2
	vpand	\r7, \r5, \r4
	vpand	\r7, \r6, %xmm1
	vpxor	\r4, %xmm1, %xmm1
	vpxor	%xmm2, %xmm1, %xmm1
	vpaddd	%xmm1, %xmm6, %xmm6

	vpslld	$10, \r7, %xmm2
	vpsrld	$2, \r7, \r4
	vpsrld	$11, \r4, %xmm1
	vpxor	%xmm2, \r4, \r4
	vpxor	%xmm1, \r4, \r4
	vpslld	$9, %xmm2, %xmm2
	vpsrld	$9, %xmm1, %xmm1
	vpxor	%xmm2, \r4, \r4
	vpxor	%xmm1, \r4, \r4
	vpslld	$11, %xmm2, %xmm2
	vpxor	%xmm2, \r4, \r4
	vpaddd	%xmm6, \r4, \r4
.endm

.macro sha256_avx_main_quadround i
	sha256_avx_main_round \i+0, %xmm10, %xmm9, %xmm8, %xmm0, %xmm3, %xmm4, %xmm5, %xmm7
	sha256_avx_main_round \i+1, %xmm9, %xmm8, %xmm0, %xmm10, %xmm4, %xmm5, %xmm7, %xmm3
	sha256_avx_main_round \i+2, %xmm8, %xmm0, %xmm10, %xmm9, %xmm5, %xmm7, %xmm3, %xmm4
	sha256_avx_main_round \i+3, %xmm0, %xmm10, %xmm9, %xmm8, %xmm7, %xmm3, %xmm4, %xmm5
.endm

	.text
	.p2align 6
sha256_avx_main_loop:
	sha256_avx_main_quadround 0
	sha256_avx_main_quadround 4
	sha256_avx_main_quadround 8
	sha256_avx_main_quadround 12
	sha256_avx_main_quadround 16
	sha256_avx_main_quadround 20
	sha256_avx_main_quadround 24
	sha256_avx_main_quadround 28
	sha256_avx_main_quadround 32
	sha256_avx_main_quadround 36
	sha256_avx_main_quadround 40
	sha256_avx_main_quadround 44
	sha256_avx_main_quadround 48
	sha256_avx_main_quadround 52
	sha256_avx_main_quadround 56
	sha256_avx_main_quadround 60
	ret

#endif /* USE_AVX */


#if defined(USE_XOP)

.macro sha256_xop_extend_round i
	vmovdqa	(\i-15)*16(%rcx), %xmm0
	vmovdqa	(\i-14)*16(%rcx), %xmm4
	vprotd	$25, %xmm0, %xmm1
	vprotd	$25, %xmm4, %xmm5
	vprotd	$14, %xmm0, %xmm2
	vprotd	$14, %xmm4, %xmm6
	vpxor	%xmm1, %xmm2, %xmm2
	vpxor	%xmm5, %xmm6, %xmm6
	vpsrld	$3, %xmm0, %xmm0
	vpsrld	$3, %xmm4, %xmm4
	vpxor	%xmm2, %xmm0, %xmm0
	vpxor	%xmm6, %xmm4, %xmm4

	vmovdqa	(\i-2)*16(%rcx), %xmm3
	vmovdqa	(\i-1)*16(%rcx), %xmm7
	vpaddd	(\i-16)*16(%rcx), %xmm0, %xmm0
	vpaddd	(\i-15)*16(%rcx), %xmm4, %xmm4

	vprotd	$15, %xmm3, %xmm1
	vprotd	$15, %xmm7, %xmm5
	vprotd	$13, %xmm3, %xmm2
	vprotd	$13, %xmm7, %xmm6
	vpxor	%xmm1, %xmm2, %xmm2
	vpxor	%xmm5, %xmm6, %xmm6

	vpaddd	(\i-7)*16(%rcx), %xmm0, %xmm0
	vpaddd	(\i-6)*16(%rcx), %xmm4, %xmm4

	vpsrld	$10, %xmm3, %xmm3
	vpsrld	$10, %xmm7, %xmm7
	vpxor	%xmm2, %xmm3, %xmm3
	vpxor	%xmm6, %xmm7, %xmm7

	vpaddd	%xmm3, %xmm0, %xmm0
	vpaddd	%xmm7, %xmm4, %xmm4
	vmovdqa	%xmm0, \i*16(%rcx)
	vmovdqa	%xmm4, (\i+1)*16(%rcx)
.endm

	.text
	.p2align 6
sha256_xop_extend_loop:
	sha256_xop_extend_round 0
	sha256_xop_extend_round 2
	sha256_xop_extend_round 4
	sha256_xop_extend_round 6
	sha256_xop_extend_round 8
	sha256_xop_extend_round 10
	sha256_xop_extend_round 12
	sha256_xop_extend_round 14
	sha256_xop_extend_round 16
	sha256_xop_extend_round 18
	sha256_xop_extend_round 20
	sha256_xop_extend_round 22
	sha256_xop_extend_round 24
	sha256_xop_extend_round 26
	sha256_xop_extend_round 28
	sha256_xop_extend_round 30
	sha256_xop_extend_round 32
	sha256_xop_extend_round 34
	sha256_xop_extend_round 36
	sha256_xop_extend_round 38
	sha256_xop_extend_round 40
	sha256_xop_extend_round 42
	sha256_xop_extend_round 44
	sha256_xop_extend_round 46
	ret
	
.macro sha256_xop_main_round i, r0, r1, r2, r3, r4, r5, r6, r7
	vpaddd	16*(\i)(%rax), \r0, %xmm6
	vpaddd	16*(\i)(%rcx), %xmm6, %xmm6

	vpandn	\r1, \r3, %xmm1
	vpand	\r3, \r2, %xmm2
	vpxor	%xmm2, %xmm1, %xmm1
	vpaddd	%xmm1, %xmm6, %xmm6

	vprotd	$26, \r3, %xmm1
	vprotd	$21, \r3, %xmm2
	vpxor	%xmm1, %xmm2, %xmm2
	vprotd	$7, \r3, \r0
	vpxor	%xmm2, \r0, \r0
	vpaddd	\r0, %xmm6, %xmm6

	vpaddd	%xmm6, \r4, \r0

	vpand	\r6, \r5, %xmm2
	vpand	\r7, \r5, \r4
	vpand	\r7, \r6, %xmm1
	vpxor	\r4, %xmm1, %xmm1
	vpxor	%xmm2, %xmm1, %xmm1
	vpaddd	%xmm1, %xmm6, %xmm6

	vprotd	$30, \r7, %xmm1
	vprotd	$19, \r7, %xmm2
	vpxor	%xmm1, %xmm2, %xmm2
	vprotd	$10, \r7, \r4
	vpxor	%xmm2, \r4, \r4
	vpaddd	%xmm6, \r4, \r4
.endm

.macro sha256_xop_main_quadround i
	sha256_xop_main_round \i+0, %xmm10, %xmm9, %xmm8, %xmm0, %xmm3, %xmm4, %xmm5, %xmm7
	sha256_xop_main_round \i+1, %xmm9, %xmm8, %xmm0, %xmm10, %xmm4, %xmm5, %xmm7, %xmm3
	sha256_xop_main_round \i+2, %xmm8, %xmm0, %xmm10, %xmm9, %xmm5, %xmm7, %xmm3, %xmm4
	sha256_xop_main_round \i+3, %xmm0, %xmm10, %xmm9, %xmm8, %xmm7, %xmm3, %xmm4, %xmm5
.endm

	.text
	.p2align 6
sha256_xop_main_loop:
	sha256_xop_main_quadround 0
	sha256_xop_main_quadround 4
	sha256_xop_main_quadround 8
	sha256_xop_main_quadround 12
	sha256_xop_main_quadround 16
	sha256_xop_main_quadround 20
	sha256_xop_main_quadround 24
	sha256_xop_main_quadround 28
	sha256_xop_main_quadround 32
	sha256_xop_main_quadround 36
	sha256_xop_main_quadround 40
	sha256_xop_main_quadround 44
	sha256_xop_main_quadround 48
	sha256_xop_main_quadround 52
	sha256_xop_main_quadround 56
	sha256_xop_main_quadround 60
	ret

#endif /* USE_XOP */


	.text
	.p2align 6
sha256_transform_4way_core_sse2:
	leaq	256(%rsp), %rcx
	leaq	48*16(%rcx), %rax
sha256_transform_4way_sse2_extend_loop:
	movdqa	-15*16(%rcx), %xmm0
	movdqa	-14*16(%rcx), %xmm4
	movdqa	%xmm0, %xmm2
	movdqa	%xmm4, %xmm6
	psrld	$3, %xmm0
	psrld	$3, %xmm4
	movdqa	%xmm0, %xmm1
	movdqa	%xmm4, %xmm5
	pslld	$14, %xmm2
	pslld	$14, %xmm6
	psrld	$4, %xmm1
	psrld	$4, %xmm5
	pxor	%xmm1, %xmm0
	pxor	%xmm5, %xmm4
	psrld	$11, %xmm1
	psrld	$11, %xmm5
	pxor	%xmm2, %xmm0
	pxor	%xmm6, %xmm4
	pslld	$11, %xmm2
	pslld	$11, %xmm6
	pxor	%xmm1, %xmm0
	pxor	%xmm5, %xmm4
	pxor	%xmm2, %xmm0
	pxor	%xmm6, %xmm4

	movdqa	-2*16(%rcx), %xmm3
	movdqa	-1*16(%rcx), %xmm7
	paddd	-16*16(%rcx), %xmm0
	paddd	-15*16(%rcx), %xmm4

	movdqa	%xmm3, %xmm2
	movdqa	%xmm7, %xmm6
	psrld	$10, %xmm3
	psrld	$10, %xmm7
	movdqa	%xmm3, %xmm1
	movdqa	%xmm7, %xmm5

	paddd	-7*16(%rcx), %xmm0

	pslld	$13, %xmm2
	pslld	$13, %xmm6
	psrld	$7, %xmm1
	psrld	$7, %xmm5

	paddd	-6*16(%rcx), %xmm4

	pxor	%xmm1, %xmm3
	pxor	%xmm5, %xmm7
	psrld	$2, %xmm1
	psrld	$2, %xmm5
	pxor	%xmm2, %xmm3
	pxor	%xmm6, %xmm7
	pslld	$2, %xmm2
	pslld	$2, %xmm6
	pxor	%xmm1, %xmm3
	pxor	%xmm5, %xmm7
	pxor	%xmm2, %xmm3
	pxor	%xmm6, %xmm7

	paddd	%xmm3, %xmm0
	paddd	%xmm7, %xmm4
	movdqa	%xmm0, (%rcx)
	movdqa	%xmm4, 16(%rcx)
	addq	$2*16, %rcx
	cmpq	%rcx, %rax
	jne sha256_transform_4way_sse2_extend_loop
	
	movdqu	0(%rdi), %xmm7
	movdqu	16(%rdi), %xmm5
	movdqu	32(%rdi), %xmm4
	movdqu	48(%rdi), %xmm3
	movdqu	64(%rdi), %xmm0
	movdqu	80(%rdi), %xmm8
	movdqu	96(%rdi), %xmm9
	movdqu	112(%rdi), %xmm10
	
	leaq	sha256_4k(%rip), %rcx
	xorq	%rax, %rax
sha256_transform_4way_sse2_main_loop:
	movdqa	(%rsp, %rax), %xmm6
	paddd	(%rcx, %rax), %xmm6
	paddd	%xmm10, %xmm6

	movdqa	%xmm0, %xmm1
	movdqa	%xmm9, %xmm2
	pandn	%xmm2, %xmm1

	movdqa	%xmm2, %xmm10
	movdqa	%xmm8, %xmm2
	movdqa	%xmm2, %xmm9

	pand	%xmm0, %xmm2
	pxor	%xmm2, %xmm1
	movdqa	%xmm0, %xmm8

	paddd	%xmm1, %xmm6

	movdqa	%xmm0, %xmm1
	psrld	$6, %xmm0
	movdqa	%xmm0, %xmm2
	pslld	$7, %xmm1
	psrld	$5, %xmm2
	pxor	%xmm1, %xmm0
	pxor	%xmm2, %xmm0
	pslld	$14, %xmm1
	psrld	$14, %xmm2
	pxor	%xmm1, %xmm0
	pxor	%xmm2, %xmm0
	pslld	$5, %xmm1
	pxor	%xmm1, %xmm0
	paddd	%xmm0, %xmm6

	movdqa	%xmm3, %xmm0
	paddd	%xmm6, %xmm0

	movdqa	%xmm5, %xmm1
	movdqa	%xmm4, %xmm3
	movdqa	%xmm4, %xmm2
	pand	%xmm5, %xmm2
	pand	%xmm7, %xmm4
	pand	%xmm7, %xmm1
	pxor	%xmm4, %xmm1
	movdqa	%xmm5, %xmm4
	movdqa	%xmm7, %xmm5
	pxor	%xmm2, %xmm1
	paddd	%xmm1, %xmm6

	movdqa	%xmm7, %xmm2
	psrld	$2, %xmm7
	movdqa	%xmm7, %xmm1
	pslld	$10, %xmm2
	psrld	$11, %xmm1
	pxor	%xmm2, %xmm7
	pxor	%xmm1, %xmm7
	pslld	$9, %xmm2
	psrld	$9, %xmm1
	pxor	%xmm2, %xmm7
	pxor	%xmm1, %xmm7
	pslld	$11, %xmm2
	pxor	%xmm2, %xmm7
	paddd	%xmm6, %xmm7
	
	addq	$16, %rax
	cmpq	$16*64, %rax
	jne sha256_transform_4way_sse2_main_loop
	jmp sha256_transform_4way_finish

#if defined(USE_AVX)
	.text
	.p2align 6
sha256_transform_4way_core_avx:
	leaq	256(%rsp), %rcx
	call sha256_avx_extend_loop
	movdqu	0(%rdi), %xmm7
	movdqu	16(%rdi), %xmm5
	movdqu	32(%rdi), %xmm4
	movdqu	48(%rdi), %xmm3
	movdqu	64(%rdi), %xmm0
	movdqu	80(%rdi), %xmm8
	movdqu	96(%rdi), %xmm9
	movdqu	112(%rdi), %xmm10
	movq	%rsp, %rax
	leaq	sha256_4k(%rip), %rcx
	call sha256_avx_main_loop
	jmp sha256_transform_4way_finish
#endif /* USE_AVX */

#if defined(USE_XOP)
	.text
	.p2align 6
sha256_transform_4way_core_xop:
	leaq	256(%rsp), %rcx
	call sha256_xop_extend_loop
	movdqu	0(%rdi), %xmm7
	movdqu	16(%rdi), %xmm5
	movdqu	32(%rdi), %xmm4
	movdqu	48(%rdi), %xmm3
	movdqu	64(%rdi), %xmm0
	movdqu	80(%rdi), %xmm8
	movdqu	96(%rdi), %xmm9
	movdqu	112(%rdi), %xmm10
	movq	%rsp, %rax
	leaq	sha256_4k(%rip), %rcx
	call sha256_xop_main_loop
	jmp sha256_transform_4way_finish
#endif /* USE_XOP */


	.data
	.p2align 3
sha256_transform_4way_core_addr:
	.quad 0x0

.macro p2bswap_rsi_rsp i
	movdqu	\i*16(%rsi), %xmm0
	movdqu	(\i+1)*16(%rsi), %xmm2
	pshuflw	$0xb1, %xmm0, %xmm0
	pshuflw	$0xb1, %xmm2, %xmm2
	pshufhw	$0xb1, %xmm0, %xmm0
	pshufhw	$0xb1, %xmm2, %xmm2
	movdqa	%xmm0, %xmm1
	movdqa	%xmm2, %xmm3
	psrlw	$8, %xmm1
	psrlw	$8, %xmm3
	psllw	$8, %xmm0
	psllw	$8, %xmm2
	pxor	%xmm1, %xmm0
	pxor	%xmm3, %xmm2
	movdqa	%xmm0, \i*16(%rsp)
	movdqa	%xmm2, (\i+1)*16(%rsp)
.endm
	
	.text
	.p2align 6
	.globl sha256_transform_4way
	.globl _sha256_transform_4way
sha256_transform_4way:
_sha256_transform_4way:
#if defined(WIN64)
	pushq	%rdi
	subq	$96, %rsp
	movdqa	%xmm6, 0(%rsp)
	movdqa	%xmm7, 16(%rsp)
	movdqa	%xmm8, 32(%rsp)
	movdqa	%xmm9, 48(%rsp)
	movdqa	%xmm10, 64(%rsp)
	movdqa	%xmm11, 80(%rsp)
	pushq	%rsi
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	movq	%r8, %rdx
#endif
	movq	%rsp, %r8
	subq	$1032, %rsp
	andq	$-128, %rsp
	
	testq	%rdx, %rdx
	jnz sha256_transform_4way_swap
	
	movdqu	0*16(%rsi), %xmm0
	movdqu	1*16(%rsi), %xmm1
	movdqu	2*16(%rsi), %xmm2
	movdqu	3*16(%rsi), %xmm3
	movdqu	4*16(%rsi), %xmm4
	movdqu	5*16(%rsi), %xmm5
	movdqu	6*16(%rsi), %xmm6
	movdqu	7*16(%rsi), %xmm7
	movdqa	%xmm0, 0*16(%rsp)
	movdqa	%xmm1, 1*16(%rsp)
	movdqa	%xmm2, 2*16(%rsp)
	movdqa	%xmm3, 3*16(%rsp)
	movdqa	%xmm4, 4*16(%rsp)
	movdqa	%xmm5, 5*16(%rsp)
	movdqa	%xmm6, 6*16(%rsp)
	movdqa	%xmm7, 7*16(%rsp)
	movdqu	8*16(%rsi), %xmm0
	movdqu	9*16(%rsi), %xmm1
	movdqu	10*16(%rsi), %xmm2
	movdqu	11*16(%rsi), %xmm3
	movdqu	12*16(%rsi), %xmm4
	movdqu	13*16(%rsi), %xmm5
	movdqu	14*16(%rsi), %xmm6
	movdqu	15*16(%rsi), %xmm7
	movdqa	%xmm0, 8*16(%rsp)
	movdqa	%xmm1, 9*16(%rsp)
	movdqa	%xmm2, 10*16(%rsp)
	movdqa	%xmm3, 11*16(%rsp)
	movdqa	%xmm4, 12*16(%rsp)
	movdqa	%xmm5, 13*16(%rsp)
	movdqa	%xmm6, 14*16(%rsp)
	movdqa	%xmm7, 15*16(%rsp)
	jmp *sha256_transform_4way_core_addr(%rip)
	
	.p2align 6
sha256_transform_4way_swap:
	p2bswap_rsi_rsp 0
	p2bswap_rsi_rsp 2
	p2bswap_rsi_rsp 4
	p2bswap_rsi_rsp 6
	p2bswap_rsi_rsp 8
	p2bswap_rsi_rsp 10
	p2bswap_rsi_rsp 12
	p2bswap_rsi_rsp 14
	jmp *sha256_transform_4way_core_addr(%rip)
	
	.p2align 6
sha256_transform_4way_finish:
	movdqu	0(%rdi), %xmm2
	movdqu	16(%rdi), %xmm6
	movdqu	32(%rdi), %xmm11
	movdqu	48(%rdi), %xmm1
	paddd	%xmm2, %xmm7
	paddd	%xmm6, %xmm5
	paddd	%xmm11, %xmm4
	paddd	%xmm1, %xmm3
	movdqu	64(%rdi), %xmm2
	movdqu	80(%rdi), %xmm6
	movdqu	96(%rdi), %xmm11
	movdqu	112(%rdi), %xmm1
	paddd	%xmm2, %xmm0
	paddd	%xmm6, %xmm8
	paddd	%xmm11, %xmm9
	paddd	%xmm1, %xmm10
	
	movdqu	%xmm7, 0(%rdi)
	movdqu	%xmm5, 16(%rdi)
	movdqu	%xmm4, 32(%rdi)
	movdqu	%xmm3, 48(%rdi)
	movdqu	%xmm0, 64(%rdi)
	movdqu	%xmm8, 80(%rdi)
	movdqu	%xmm9, 96(%rdi)
	movdqu	%xmm10, 112(%rdi)
	
	movq	%r8, %rsp
#if defined(WIN64)
	popq	%rsi
	movdqa	0(%rsp), %xmm6
	movdqa	16(%rsp), %xmm7
	movdqa	32(%rsp), %xmm8
	movdqa	48(%rsp), %xmm9
	movdqa	64(%rsp), %xmm10
	movdqa	80(%rsp), %xmm11
	addq	$96, %rsp
	popq	%rdi
#endif
	ret
	
	
	.data
	.p2align 3
sha256d_4way_addr:
	.quad 0x0
	
	.text
	.p2align 6
	.globl sha256d_4way
	.globl _sha256d_4way
sha256d_4way:
_sha256d_4way:
	jmp *sha256d_4way_addr(%rip)
	
	
	.p2align 6
sha256d_4way_sse2:
#if defined(WIN64)
	pushq	%rdi
	subq	$80, %rsp
	movdqa	%xmm6, 0(%rsp)
	movdqa	%xmm7, 16(%rsp)
	movdqa	%xmm8, 32(%rsp)
	movdqa	%xmm9, 48(%rsp)
	movdqa	%xmm10, 64(%rsp)
	pushq	%rsi
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	movq	%r8, %rdx
#endif
	subq	$1032, %rsp
	
	leaq	256(%rsi), %rcx
	call sha256_sse2_extend_loop
	
	movdqa	0(%rdx), %xmm7
	movdqa	16(%rdx), %xmm5
	movdqa	32(%rdx), %xmm4
	movdqa	48(%rdx), %xmm3
	movdqa	64(%rdx), %xmm0
	movdqa	80(%rdx), %xmm8
	movdqa	96(%rdx), %xmm9
	movdqa	112(%rdx), %xmm10
	
	movq	%rsi, %rax
	leaq	sha256_4k(%rip), %rcx
	call sha256_sse2_main_loop
	
	paddd	0(%rdx), %xmm7
	paddd	16(%rdx), %xmm5
	paddd	32(%rdx), %xmm4
	paddd	48(%rdx), %xmm3
	paddd	64(%rdx), %xmm0
	paddd	80(%rdx), %xmm8
	paddd	96(%rdx), %xmm9
	paddd	112(%rdx), %xmm10
	
	movdqa	%xmm7, 0(%rsp)
	movdqa	%xmm5, 16(%rsp)
	movdqa	%xmm4, 32(%rsp)
	movdqa	%xmm3, 48(%rsp)
	movdqa	%xmm0, 64(%rsp)
	movdqa	%xmm8, 80(%rsp)
	movdqa	%xmm9, 96(%rsp)
	movdqa	%xmm10, 112(%rsp)
	
	pxor	%xmm0, %xmm0
	movq	$0x8000000000000100, %rax
	movd	%rax, %xmm1
	pshufd	$0x55, %xmm1, %xmm2
	pshufd	$0x00, %xmm1, %xmm1
	movdqa	%xmm2, 128(%rsp)
	movdqa	%xmm0, 144(%rsp)
	movdqa	%xmm0, 160(%rsp)
	movdqa	%xmm0, 176(%rsp)
	movdqa	%xmm0, 192(%rsp)
	movdqa	%xmm0, 208(%rsp)
	movdqa	%xmm0, 224(%rsp)
	movdqa	%xmm1, 240(%rsp)
	
	leaq	256(%rsp), %rcx
	call sha256_sse2_extend_loop
	
	movdqa	sha256_4h+0(%rip), %xmm7
	movdqa	sha256_4h+16(%rip), %xmm5
	movdqa	sha256_4h+32(%rip), %xmm4
	movdqa	sha256_4h+48(%rip), %xmm3
	movdqa	sha256_4h+64(%rip), %xmm0
	movdqa	sha256_4h+80(%rip), %xmm8
	movdqa	sha256_4h+96(%rip), %xmm9
	movdqa	sha256_4h+112(%rip), %xmm10
	
	movq	%rsp, %rax
	leaq	sha256_4k(%rip), %rcx
	call sha256_sse2_main_loop
	
	paddd	sha256_4h+0(%rip), %xmm7
	paddd	sha256_4h+16(%rip), %xmm5
	paddd	sha256_4h+32(%rip), %xmm4
	paddd	sha256_4h+48(%rip), %xmm3
	paddd	sha256_4h+64(%rip), %xmm0
	paddd	sha256_4h+80(%rip), %xmm8
	paddd	sha256_4h+96(%rip), %xmm9
	paddd	sha256_4h+112(%rip), %xmm10
	
	movdqa	%xmm7, 0(%rdi)
	movdqa	%xmm5, 16(%rdi)
	movdqa	%xmm4, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	movdqa	%xmm0, 64(%rdi)
	movdqa	%xmm8, 80(%rdi)
	movdqa	%xmm9, 96(%rdi)
	movdqa	%xmm10, 112(%rdi)
	
	addq	$1032, %rsp
#if defined(WIN64)
	popq	%rsi
	movdqa	0(%rsp), %xmm6
	movdqa	16(%rsp), %xmm7
	movdqa	32(%rsp), %xmm8
	movdqa	48(%rsp), %xmm9
	movdqa	64(%rsp), %xmm10
	addq	$80, %rsp
	popq	%rdi
#endif
	ret
	
	
#if defined(USE_AVX)
	
	.p2align 6
sha256d_4way_avx:
#if defined(WIN64)
	pushq	%rdi
	subq	$80, %rsp
	movdqa	%xmm6, 0(%rsp)
	movdqa	%xmm7, 16(%rsp)
	movdqa	%xmm8, 32(%rsp)
	movdqa	%xmm9, 48(%rsp)
	movdqa	%xmm10, 64(%rsp)
	pushq	%rsi
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	movq	%r8, %rdx
#endif
	subq	$1032, %rsp
	
	leaq	256(%rsi), %rcx
	call sha256_avx_extend_loop
	
	movdqa	0(%rdx), %xmm7
	movdqa	16(%rdx), %xmm5
	movdqa	32(%rdx), %xmm4
	movdqa	48(%rdx), %xmm3
	movdqa	64(%rdx), %xmm0
	movdqa	80(%rdx), %xmm8
	movdqa	96(%rdx), %xmm9
	movdqa	112(%rdx), %xmm10
	
	movq	%rsi, %rax
	leaq	sha256_4k(%rip), %rcx
	call sha256_avx_main_loop
	
	paddd	0(%rdx), %xmm7
	paddd	16(%rdx), %xmm5
	paddd	32(%rdx), %xmm4
	paddd	48(%rdx), %xmm3
	paddd	64(%rdx), %xmm0
	paddd	80(%rdx), %xmm8
	paddd	96(%rdx), %xmm9
	paddd	112(%rdx), %xmm10
	
	movdqa	%xmm7, 0(%rsp)
	movdqa	%xmm5, 16(%rsp)
	movdqa	%xmm4, 32(%rsp)
	movdqa	%xmm3, 48(%rsp)
	movdqa	%xmm0, 64(%rsp)
	movdqa	%xmm8, 80(%rsp)
	movdqa	%xmm9, 96(%rsp)
	movdqa	%xmm10, 112(%rsp)
	
	pxor	%xmm0, %xmm0
	movq	$0x8000000000000100, %rax
	movd	%rax, %xmm1
	pshufd	$0x55, %xmm1, %xmm2
	pshufd	$0x00, %xmm1, %xmm1
	movdqa	%xmm2, 128(%rsp)
	movdqa	%xmm0, 144(%rsp)
	movdqa	%xmm0, 160(%rsp)
	movdqa	%xmm0, 176(%rsp)
	movdqa	%xmm0, 192(%rsp)
	movdqa	%xmm0, 208(%rsp)
	movdqa	%xmm0, 224(%rsp)
	movdqa	%xmm1, 240(%rsp)
	
	leaq	256(%rsp), %rcx
	call sha256_avx_extend_loop
	
	movdqa	sha256_4h+0(%rip), %xmm7
	movdqa	sha256_4h+16(%rip), %xmm5
	movdqa	sha256_4h+32(%rip), %xmm4
	movdqa	sha256_4h+48(%rip), %xmm3
	movdqa	sha256_4h+64(%rip), %xmm0
	movdqa	sha256_4h+80(%rip), %xmm8
	movdqa	sha256_4h+96(%rip), %xmm9
	movdqa	sha256_4h+112(%rip), %xmm10
	
	movq	%rsp, %rax
	leaq	sha256_4k(%rip), %rcx
	call sha256_avx_main_loop
	
	paddd	sha256_4h+0(%rip), %xmm7
	paddd	sha256_4h+16(%rip), %xmm5
	paddd	sha256_4h+32(%rip), %xmm4
	paddd	sha256_4h+48(%rip), %xmm3
	paddd	sha256_4h+64(%rip), %xmm0
	paddd	sha256_4h+80(%rip), %xmm8
	paddd	sha256_4h+96(%rip), %xmm9
	paddd	sha256_4h+112(%rip), %xmm10
	
	movdqa	%xmm7, 0(%rdi)
	movdqa	%xmm5, 16(%rdi)
	movdqa	%xmm4, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	movdqa	%xmm0, 64(%rdi)
	movdqa	%xmm8, 80(%rdi)
	movdqa	%xmm9, 96(%rdi)
	movdqa	%xmm10, 112(%rdi)
	
	addq	$1032, %rsp
#if defined(WIN64)
	popq	%rsi
	movdqa	0(%rsp), %xmm6
	movdqa	16(%rsp), %xmm7
	movdqa	32(%rsp), %xmm8
	movdqa	48(%rsp), %xmm9
	movdqa	64(%rsp), %xmm10
	addq	$80, %rsp
	popq	%rdi
#endif
	ret
	
#endif /* USE_AVX */
	
	
#if defined(USE_XOP)
	
	.p2align 6
sha256d_4way_xop:
#if defined(WIN64)
	pushq	%rdi
	subq	$80, %rsp
	movdqa	%xmm6, 0(%rsp)
	movdqa	%xmm7, 16(%rsp)
	movdqa	%xmm8, 32(%rsp)
	movdqa	%xmm9, 48(%rsp)
	movdqa	%xmm10, 64(%rsp)
	pushq	%rsi
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	movq	%r8, %rdx
#endif
	subq	$1032, %rsp
	
	leaq	256(%rsi), %rcx
	call sha256_xop_extend_loop
	
	movdqa	0(%rdx), %xmm7
	movdqa	16(%rdx), %xmm5
	movdqa	32(%rdx), %xmm4
	movdqa	48(%rdx), %xmm3
	movdqa	64(%rdx), %xmm0
	movdqa	80(%rdx), %xmm8
	movdqa	96(%rdx), %xmm9
	movdqa	112(%rdx), %xmm10
	
	movq	%rsi, %rax
	leaq	sha256_4k(%rip), %rcx
	call sha256_xop_main_loop
	
	paddd	0(%rdx), %xmm7
	paddd	16(%rdx), %xmm5
	paddd	32(%rdx), %xmm4
	paddd	48(%rdx), %xmm3
	paddd	64(%rdx), %xmm0
	paddd	80(%rdx), %xmm8
	paddd	96(%rdx), %xmm9
	paddd	112(%rdx), %xmm10
	
	movdqa	%xmm7, 0(%rsp)
	movdqa	%xmm5, 16(%rsp)
	movdqa	%xmm4, 32(%rsp)
	movdqa	%xmm3, 48(%rsp)
	movdqa	%xmm0, 64(%rsp)
	movdqa	%xmm8, 80(%rsp)
	movdqa	%xmm9, 96(%rsp)
	movdqa	%xmm10, 112(%rsp)
	
	pxor	%xmm0, %xmm0
	movq	$0x8000000000000100, %rax
	movd	%rax, %xmm1
	pshufd	$0x55, %xmm1, %xmm2
	pshufd	$0x00, %xmm1, %xmm1
	movdqa	%xmm2, 128(%rsp)
	movdqa	%xmm0, 144(%rsp)
	movdqa	%xmm0, 160(%rsp)
	movdqa	%xmm0, 176(%rsp)
	movdqa	%xmm0, 192(%rsp)
	movdqa	%xmm0, 208(%rsp)
	movdqa	%xmm0, 224(%rsp)
	movdqa	%xmm1, 240(%rsp)
	
	leaq	256(%rsp), %rcx
	call sha256_xop_extend_loop
	
	movdqa	sha256_4h+0(%rip), %xmm7
	movdqa	sha256_4h+16(%rip), %xmm5
	movdqa	sha256_4h+32(%rip), %xmm4
	movdqa	sha256_4h+48(%rip), %xmm3
	movdqa	sha256_4h+64(%rip), %xmm0
	movdqa	sha256_4h+80(%rip), %xmm8
	movdqa	sha256_4h+96(%rip), %xmm9
	movdqa	sha256_4h+112(%rip), %xmm10
	
	movq	%rsp, %rax
	leaq	sha256_4k(%rip), %rcx
	call sha256_xop_main_loop
	
	paddd	sha256_4h+0(%rip), %xmm7
	paddd	sha256_4h+16(%rip), %xmm5
	paddd	sha256_4h+32(%rip), %xmm4
	paddd	sha256_4h+48(%rip), %xmm3
	paddd	sha256_4h+64(%rip), %xmm0
	paddd	sha256_4h+80(%rip), %xmm8
	paddd	sha256_4h+96(%rip), %xmm9
	paddd	sha256_4h+112(%rip), %xmm10
	
	movdqa	%xmm7, 0(%rdi)
	movdqa	%xmm5, 16(%rdi)
	movdqa	%xmm4, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	movdqa	%xmm0, 64(%rdi)
	movdqa	%xmm8, 80(%rdi)
	movdqa	%xmm9, 96(%rdi)
	movdqa	%xmm10, 112(%rdi)
	
	addq	$1032, %rsp
#if defined(WIN64)
	popq	%rsi
	movdqa	0(%rsp), %xmm6
	movdqa	16(%rsp), %xmm7
	movdqa	32(%rsp), %xmm8
	movdqa	48(%rsp), %xmm9
	movdqa	64(%rsp), %xmm10
	addq	$80, %rsp
	popq	%rdi
#endif
	ret
	
#endif /* USE_XOP */


	.text
	.p2align 6
	.globl sha256_use_4way
	.globl _sha256_use_4way
sha256_use_4way:
_sha256_use_4way:
	pushq	%rbx
	pushq	%rcx
	pushq	%rdx
	
#if defined(USE_AVX)
	# Check for AVX and OSXSAVE support
	movl	$1, %eax
	cpuid
	andl	$0x18000000, %ecx
	cmpl	$0x18000000, %ecx
	jne sha2_4way_init_sse2
	# Check for XMM and YMM state support
	xorl	%ecx, %ecx
	xgetbv
	andl	$0x00000006, %eax
	cmpl	$0x00000006, %eax
	jne sha2_4way_init_sse2
#if defined(USE_XOP)
	# Check for XOP support
	movl	$0x80000001, %eax
	cpuid
	andl	$0x00000800, %ecx
	jz sha2_4way_init_avx
	
sha2_4way_init_xop:
	leaq	sha256d_4way_xop(%rip), %rax
	leaq	sha256_transform_4way_core_xop(%rip), %rdx
	jmp sha2_4way_init_done
#endif /* USE_XOP */
	
sha2_4way_init_avx:
	leaq	sha256d_4way_avx(%rip), %rax
	leaq	sha256_transform_4way_core_avx(%rip), %rdx
	jmp sha2_4way_init_done
#endif /* USE_AVX */
	
sha2_4way_init_sse2:
	leaq	sha256d_4way_sse2(%rip), %rax
	leaq	sha256_transform_4way_core_sse2(%rip), %rdx
	
sha2_4way_init_done:
	movq	%rax, sha256d_4way_addr(%rip)
	movq	%rdx, sha256_transform_4way_core_addr(%rip)
	popq	%rdx
	popq	%rcx
	popq	%rbx
	movl	$1, %eax
	ret

#endif
