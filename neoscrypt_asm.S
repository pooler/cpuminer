/*
 * Copyright (c) 2014 John Doering <ghostlander@phoenixcoin.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if (ASM) && (__x86_64__)

/* neoscrypt_blkcpy(dst, src, len) = SSE2 based block memcpy();
 * len must be a multiple of 64 bytes aligned properly */
.globl neoscrypt_blkcpy
neoscrypt_blkcpy:
#if (WIN64)
	movq	%rdi, %r10
	movq	%rsi, %r11
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	movq	%r8, %rdx
#endif
	xorq	%rcx, %rcx
	movl	%edx, %ecx
	shrl	$6, %ecx
	movq	$64, %rax
.blkcpy:
	movdqa	0(%rsi), %xmm0
	movdqa	16(%rsi), %xmm1
	movdqa	32(%rsi), %xmm2
	movdqa	48(%rsi), %xmm3
	movdqa	%xmm0, 0(%rdi)
	movdqa	%xmm1, 16(%rdi)
	movdqa	%xmm2, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	addq	%rax, %rdi
	addq	%rax, %rsi
	decl	%ecx
	jnz	.blkcpy
#if (WIN64)
	movq	%r10, %rdi
	movq	%r11, %rsi
#endif
	ret


/* neoscrypt_blkswp(blkA, blkB, len) = SSE2 based block swapper;
 * len must be a multiple of 64 bytes aligned properly */
.globl neoscrypt_blkswp
neoscrypt_blkswp:
#if (WIN64)
	movq	%rdi, %r10
	movq	%rsi, %r11
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	movq	%r8, %rdx
#endif
	xorq	%rcx, %rcx
	movl	%edx, %ecx
	shrl	$6, %ecx
	movq	$64, %rax
.blkswp:
	movdqa	0(%rdi), %xmm0
	movdqa	16(%rdi), %xmm1
	movdqa	32(%rdi), %xmm2
	movdqa	48(%rdi), %xmm3
	movdqa	0(%rsi), %xmm4
	movdqa	16(%rsi), %xmm5
	movdqa	32(%rsi), %xmm8
	movdqa	48(%rsi), %xmm9
	movdqa	%xmm0, 0(%rsi)
	movdqa	%xmm1, 16(%rsi)
	movdqa	%xmm2, 32(%rsi)
	movdqa	%xmm3, 48(%rsi)
	movdqa	%xmm4, 0(%rdi)
	movdqa	%xmm5, 16(%rdi)
	movdqa	%xmm8, 32(%rdi)
	movdqa	%xmm9, 48(%rdi)
	addq	%rax, %rdi
	addq	%rax, %rsi
	decl	%ecx
	jnz	.blkswp
#if (WIN64)
	movq	%r10, %rdi
	movq	%r11, %rsi
#endif
	ret


/* neoscrypt_blkxor(dst, src, len) = SSE2 based block XOR engine;
 * len must be a multiple of 64 bytes aligned properly */
.globl neoscrypt_blkxor
neoscrypt_blkxor:
#if (WIN64)
	movq	%rdi, %r10
	movq	%rsi, %r11
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	movq	%r8, %rdx
#endif
	xorq	%rcx, %rcx
	movl	%edx, %ecx
	shrl	$6, %ecx
	movq	$64, %rax
.blkxor:
	movdqa	0(%rdi), %xmm0
	movdqa	16(%rdi), %xmm1
	movdqa	32(%rdi), %xmm2
	movdqa	48(%rdi), %xmm3
	movdqa	0(%rsi), %xmm4
	movdqa	16(%rsi), %xmm5
	movdqa	32(%rsi), %xmm8
	movdqa	48(%rsi), %xmm9
	pxor	%xmm4, %xmm0
	pxor	%xmm5, %xmm1
	pxor	%xmm8, %xmm2
	pxor	%xmm9, %xmm3
	movdqa	%xmm0, 0(%rdi)
	movdqa	%xmm1, 16(%rdi)
	movdqa	%xmm2, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	addq	%rax, %rdi
	addq	%rax, %rsi
	decl	%ecx
	jnz	.blkxor
#if (WIN64)
	movq	%r10, %rdi
	movq	%r11, %rsi
#endif
	ret


/* neoscrypt_salsa(mem, rounds) = SSE2 based Salsa20;
 * mem must be aligned properly, rounds must be a multiple of 2 */
.globl neoscrypt_salsa
neoscrypt_salsa:
#if (WIN64)
	movq	%rdi, %r10
	movq	%rsi, %r11
	movq	%rcx, %rdi
	movq	%rdx, %rsi
#endif
	xorq	%rcx, %rcx
	movl	%esi, %ecx
	shrl	$1, %ecx
	movdqa	0(%rdi), %xmm0
	movdqa	%xmm0, %xmm12
	movdqa	16(%rdi), %xmm1
	movdqa	%xmm1, %xmm13
	movdqa	32(%rdi), %xmm2
	movdqa	%xmm2, %xmm14
	movdqa	48(%rdi), %xmm3
	movdqa	%xmm3, %xmm15
.salsa:
	movdqa	%xmm1, %xmm4
	paddd	%xmm0, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$7, %xmm4
	psrld	$25, %xmm5
	pxor	%xmm4, %xmm3
	movdqa	%xmm0, %xmm4
	pxor	%xmm5, %xmm3
	paddd	%xmm3, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$9, %xmm4
	psrld	$23, %xmm5
	pxor	%xmm4, %xmm2
	movdqa	%xmm3, %xmm4
	pxor	%xmm5, %xmm2
	pshufd	$0x93, %xmm3, %xmm3
	paddd	%xmm2, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$13, %xmm4
	psrld	$19, %xmm5
	pxor	%xmm4, %xmm1
	movdqa	%xmm2, %xmm4
	pxor	%xmm5, %xmm1
	pshufd	$0x4E, %xmm2, %xmm2
	paddd	%xmm1, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$18, %xmm4
	psrld	$14, %xmm5
	pxor	%xmm4, %xmm0
	movdqa	%xmm3, %xmm4
	pxor	%xmm5, %xmm0
	pshufd	$0x39, %xmm1, %xmm1
	paddd	%xmm0, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$7, %xmm4
	psrld	$25, %xmm5
	pxor	%xmm4, %xmm1
	movdqa	%xmm0, %xmm4
	pxor	%xmm5, %xmm1
	paddd	%xmm1, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$9, %xmm4
	psrld	$23, %xmm5
	pxor	%xmm4, %xmm2
	movdqa	%xmm1, %xmm4
	pxor	%xmm5, %xmm2
	pshufd	$0x93, %xmm1, %xmm1
	paddd	%xmm2, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$13, %xmm4
	psrld	$19, %xmm5
	pxor	%xmm4, %xmm3
	movdqa	%xmm2, %xmm4
	pxor	%xmm5, %xmm3
	pshufd	$0x4E, %xmm2, %xmm2
	paddd	%xmm3, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$18, %xmm4
	psrld	$14, %xmm5
	pxor	%xmm4, %xmm0
	pshufd	$0x39, %xmm3, %xmm3
	pxor	%xmm5, %xmm0
	decl	%ecx
	jnz	.salsa

	paddd	%xmm12, %xmm0
	movdqa	%xmm0, 0(%rdi)
	paddd	%xmm13, %xmm1
	movdqa	%xmm1, 16(%rdi)
	paddd	%xmm14, %xmm2
	movdqa	%xmm2, 32(%rdi)
	paddd	%xmm15, %xmm3
	movdqa	%xmm3, 48(%rdi)
#if (WIN64)
	movq	%r10, %rdi
	movq	%r11, %rsi
#endif
	ret


/* neoscrypt_salsa_tangle(mem, count) = Salsa20 SSE2 map switcher;
 * correct map:  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
 * SSE2 map:     0   5  10  15  12   1   6  11   8  13   2   7   4   9  14   3 */
.globl neoscrypt_salsa_tangle
neoscrypt_salsa_tangle:
#if (WIN64)
	movq	%rdi, %r10
	movq	%rsi, %r11
	movq	%rcx, %rdi
	movq	%rdx, %rsi
#endif
	xorq	%rcx, %rcx
	movl	%esi, %ecx
	movq	$64, %r8
.salsa_tangle:
	movl	4(%rdi), %eax
	movl	20(%rdi), %edx
	movl	%eax, 20(%rdi)
	movl	%edx, 4(%rdi)
	movl	8(%rdi), %eax
	movl	40(%rdi), %edx
	movl	%eax, 40(%rdi)
	movl	%edx, 8(%rdi)
	movl	12(%rdi), %eax
	movl	60(%rdi), %edx
	movl	%eax, 60(%rdi)
	movl	%edx, 12(%rdi)
	movl	16(%rdi), %eax
	movl	48(%rdi), %edx
	movl	%eax, 48(%rdi)
	movl	%edx, 16(%rdi)
	movl	28(%rdi), %eax
	movl	44(%rdi), %edx
	movl	%eax, 44(%rdi)
	movl	%edx, 28(%rdi)
	movl	36(%rdi), %eax
	movl	52(%rdi), %edx
	movl	%eax, 52(%rdi)
	movl	%edx, 36(%rdi)
	addq	%r8, %rdi
	decl	%ecx
	jnz	.salsa_tangle
#if (WIN64)
	movq	%r10, %rdi
	movq	%r11, %rsi
#endif
	ret


/* neoscrypt_chacha(mem, rounds) = SSE2 based ChaCha20;
 * mem must be aligned properly, rounds must be a multiple of 2 */
.globl neoscrypt_chacha
neoscrypt_chacha:
#if (WIN64)
	movq	%rdi, %r10
	movq	%rsi, %r11
	movq	%rcx, %rdi
	movq	%rdx, %rsi
#endif
	xorq	%rcx, %rcx
	movl	%esi, %ecx
	shrl	$1, %ecx
	movdqa	0(%rdi), %xmm0
	movdqa	%xmm0, %xmm12
	movdqa	16(%rdi), %xmm1
	movdqa	%xmm1, %xmm13
	movdqa	32(%rdi), %xmm2
	movdqa	%xmm2, %xmm14
	movdqa	48(%rdi), %xmm3
	movdqa	%xmm3, %xmm15
.chacha:
	paddd	%xmm1, %xmm0
	pxor 	%xmm0, %xmm3
	pshuflw	$0xB1, %xmm3, %xmm3
	pshufhw	$0xB1, %xmm3, %xmm3
	paddd	%xmm3, %xmm2
	pxor 	%xmm2, %xmm1
	movdqa	%xmm1, %xmm4
	pslld	$12, %xmm1
	psrld	$20, %xmm4
	pxor	%xmm4, %xmm1
	paddd	%xmm1, %xmm0
	pxor	%xmm0, %xmm3
	movdqa	%xmm3, %xmm4
	pslld	$8, %xmm3
	psrld	$24, %xmm4
	pxor	%xmm4, %xmm3
	pshufd	$0x93, %xmm0, %xmm0
	paddd	%xmm3, %xmm2
	pshufd	$0x4E, %xmm3, %xmm3
	pxor	%xmm2, %xmm1
	pshufd	$0x39, %xmm2, %xmm2
	movdqa	%xmm1, %xmm4
	pslld	$7, %xmm1
	psrld	$25, %xmm4
	pxor	%xmm4, %xmm1
	paddd	%xmm1, %xmm0
	pxor	%xmm0, %xmm3
	pshuflw	$0xB1, %xmm3, %xmm3
	pshufhw $0xB1, %xmm3, %xmm3
	paddd	%xmm3, %xmm2
	pxor	%xmm2, %xmm1
	movdqa	%xmm1, %xmm4
	pslld	$12, %xmm1
	psrld	$20, %xmm4
	pxor	%xmm4, %xmm1
	paddd	%xmm1, %xmm0
	pxor	%xmm0, %xmm3
	movdqa	%xmm3, %xmm4
	pslld	$8, %xmm3
	psrld	$24, %xmm4
	pxor	%xmm4, %xmm3
	pshufd	$0x39, %xmm0, %xmm0
	paddd	%xmm3, %xmm2
	pshufd	$0x4E, %xmm3, %xmm3
	pxor	%xmm2, %xmm1
	pshufd	$0x93, %xmm2, %xmm2
	movdqa	%xmm1, %xmm4
	pslld	$7, %xmm1
	psrld	$25, %xmm4
	pxor	%xmm4, %xmm1
	decl	%ecx
	jnz	.chacha

	paddd	%xmm12, %xmm0
	movdqa	%xmm0, 0(%rdi)
	paddd	%xmm13, %xmm1
	movdqa	%xmm1, 16(%rdi)
	paddd	%xmm14, %xmm2
	movdqa	%xmm2, 32(%rdi)
	paddd	%xmm15, %xmm3
	movdqa	%xmm3, 48(%rdi)
#if (WIN64)
	movq	%r10, %rdi
	movq	%r11, %rsi
#endif
	ret

#endif /* (ASM) && (__x86_64__) */


#if (ASM) && (__i386__)

/* neoscrypt_blkcpy(dst, src, len) = SSE2 based block memcpy();
 * len must be a multiple of 64 bytes aligned properly */
.globl neoscrypt_blkcpy
.globl _neoscrypt_blkcpy
neoscrypt_blkcpy:
_neoscrypt_blkcpy:
	pushl	%edi
	pushl	%esi
	movl	12(%esp), %edi
	movl	16(%esp), %esi
	movl	20(%esp), %ecx
	shrl	$6, %ecx
	movl	$64, %eax
.blkcpy:
	movdqa	0(%esi), %xmm0
	movdqa	16(%esi), %xmm1
	movdqa	32(%esi), %xmm2
	movdqa	48(%esi), %xmm3
	movdqa	%xmm0, 0(%edi)
	movdqa	%xmm1, 16(%edi)
	movdqa	%xmm2, 32(%edi)
	movdqa	%xmm3, 48(%edi)
	addl	%eax, %edi
	add	%eax, %esi
	decl	%ecx
	jnz	.blkcpy

	popl	%esi
	popl	%edi
	ret


/* neoscrypt_blkswp(blkA, blkB, len) = SSE2 based block swapper;
 * len must be a multiple of 64 bytes aligned properly */
.globl neoscrypt_blkswp
.globl _neoscrypt_blkswp
neoscrypt_blkswp:
_neoscrypt_blkswp:
	pushl	%edi
	pushl	%esi
	movl	12(%esp), %edi
	movl	16(%esp), %esi
	movl	20(%esp), %ecx
	shrl	$6, %ecx
	movl	$64, %eax
.blkswp:
	movdqa	0(%edi), %xmm0
	movdqa	16(%edi), %xmm1
	movdqa	32(%edi), %xmm2
	movdqa	48(%edi), %xmm3
	movdqa	0(%esi), %xmm4
	movdqa	16(%esi), %xmm5
	movdqa	32(%esi), %xmm6
	movdqa	48(%esi), %xmm7
	movdqa	%xmm0, 0(%esi)
	movdqa	%xmm1, 16(%esi)
	movdqa	%xmm2, 32(%esi)
	movdqa	%xmm3, 48(%esi)
	movdqa	%xmm4, 0(%edi)
	movdqa	%xmm5, 16(%edi)
	movdqa	%xmm6, 32(%edi)
	movdqa	%xmm7, 48(%edi)
	addl	%eax, %edi
	addl	%eax, %esi
	decl	%ecx
	jnz	.blkswp

	popl	%esi
	popl	%edi
	ret


/* neoscrypt_blkxor(dst, src, len) = SSE2 based block XOR engine;
 * len must be a multiple of 64 bytes aligned properly */
.globl neoscrypt_blkxor
.globl _neoscrypt_blkxor
neoscrypt_blkxor:
_neoscrypt_blkxor:
	pushl	%edi
	pushl	%esi
	movl	12(%esp), %edi
	movl	16(%esp), %esi
	movl	20(%esp), %ecx
	shrl	$6, %ecx
	movl	$64, %eax
.blkxor:
	movdqa	0(%edi), %xmm0
	movdqa	16(%edi), %xmm1
	movdqa	32(%edi), %xmm2
	movdqa	48(%edi), %xmm3
	movdqa	0(%esi), %xmm4
	movdqa	16(%esi), %xmm5
	movdqa	32(%esi), %xmm6
	movdqa	48(%esi), %xmm7
	pxor	%xmm4, %xmm0
	pxor	%xmm5, %xmm1
	pxor	%xmm6, %xmm2
	pxor	%xmm7, %xmm3
	movdqa	%xmm0, 0(%edi)
	movdqa	%xmm1, 16(%edi)
	movdqa	%xmm2, 32(%edi)
	movdqa	%xmm3, 48(%edi)
	addl	%eax, %edi
	addl	%eax, %esi
	decl	%ecx
	jnz	.blkxor

	popl	%esi
	popl	%edi
	ret


/* neoscrypt_salsa(mem, rounds) = SSE2 based Salsa20;
 * mem must be aligned properly, rounds must be a multiple of 2 */
.globl neoscrypt_salsa
.globl _neoscrypt_salsa
neoscrypt_salsa:
_neoscrypt_salsa:
	movl	4(%esp), %edx
	movl	8(%esp), %ecx
	shrl	$1, %ecx
	movdqa	0(%edx), %xmm0
	movdqa	%xmm0, %xmm6
	movdqa	16(%edx), %xmm1
	movdqa	%xmm1, %xmm7
	subl	$32, %esp
	movdqa	32(%edx), %xmm2
	movdqu	%xmm2, 0(%esp)
	movdqa	48(%edx), %xmm3
	movdqu	%xmm3, 16(%esp)
.salsa:
	movdqa	%xmm1, %xmm4
	paddd	%xmm0, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$7, %xmm4
	psrld	$25, %xmm5
	pxor	%xmm4, %xmm3
	movdqa	%xmm0, %xmm4
	pxor	%xmm5, %xmm3
	paddd	%xmm3, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$9, %xmm4
	psrld	$23, %xmm5
	pxor	%xmm4, %xmm2
	movdqa	%xmm3, %xmm4
	pxor	%xmm5, %xmm2
	pshufd	$0x93, %xmm3, %xmm3
	paddd	%xmm2, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$13, %xmm4
	psrld	$19, %xmm5
	pxor	%xmm4, %xmm1
	movdqa	%xmm2, %xmm4
	pxor	%xmm5, %xmm1
	pshufd	$0x4E, %xmm2, %xmm2
	paddd	%xmm1, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$18, %xmm4
	psrld	$14, %xmm5
	pxor	%xmm4, %xmm0
	movdqa	%xmm3, %xmm4
	pxor	%xmm5, %xmm0
	pshufd	$0x39, %xmm1, %xmm1
	paddd	%xmm0, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$7, %xmm4
	psrld	$25, %xmm5
	pxor	%xmm4, %xmm1
	movdqa	%xmm0, %xmm4
	pxor	%xmm5, %xmm1
	paddd	%xmm1, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$9, %xmm4
	psrld	$23, %xmm5
	pxor	%xmm4, %xmm2
	movdqa	%xmm1, %xmm4
	pxor	%xmm5, %xmm2
	pshufd	$0x93, %xmm1, %xmm1
	paddd	%xmm2, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$13, %xmm4
	psrld	$19, %xmm5
	pxor	%xmm4, %xmm3
	movdqa	%xmm2, %xmm4
	pxor	%xmm5, %xmm3
	pshufd	$0x4E, %xmm2, %xmm2
	paddd	%xmm3, %xmm4
	movdqa	%xmm4, %xmm5
	pslld	$18, %xmm4
	psrld	$14, %xmm5
	pxor	%xmm4, %xmm0
	pshufd	$0x39, %xmm3, %xmm3
	pxor	%xmm5, %xmm0
	decl	%ecx
	jnz	.salsa

	paddd	%xmm6, %xmm0
	movdqa	%xmm0, 0(%edx)
	paddd	%xmm7, %xmm1
	movdqa	%xmm1, 16(%edx)
	movdqu	0(%esp), %xmm6
	paddd	%xmm6, %xmm2
	movdqa	%xmm2, 32(%edx)
	movdqu	16(%esp), %xmm7
	paddd	%xmm7, %xmm3
	movdqa	%xmm3, 48(%edx)
	addl	$32, %esp
	ret


/* neoscrypt_salsa_tangle(mem, count) = Salsa20 SSE2 map switcher;
 * correct map:  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
 * SSE2 map:     0   5  10  15  12   1   6  11   8  13   2   7   4   9  14   3 */
.globl neoscrypt_salsa_tangle
.globl _neoscrypt_salsa_tangle
neoscrypt_salsa_tangle:
_neoscrypt_salsa_tangle:
	pushl	%ebx
	push	%ebp
	movl	12(%esp), %ebp
	movl	16(%esp), %ecx
	movl	$64, %ebx
.salsa_tangle:
	movl	4(%ebp), %eax
	movl	20(%ebp), %edx
	movl	%eax, 20(%ebp)
	movl	%edx, 4(%ebp)
	movl	8(%ebp), %eax
	movl	40(%ebp), %edx
	movl	%eax, 40(%ebp)
	movl	%edx, 8(%ebp)
	movl	12(%ebp), %eax
	movl	60(%ebp), %edx
	movl	%eax, 60(%ebp)
	movl	%edx, 12(%ebp)
	movl	16(%ebp), %eax
	movl	48(%ebp), %edx
	movl	%eax, 48(%ebp)
	movl	%edx, 16(%ebp)
	movl	28(%ebp), %eax
	movl	44(%ebp), %edx
	movl	%eax, 44(%ebp)
	movl	%edx, 28(%ebp)
	movl	36(%ebp), %eax
	movl	52(%ebp), %edx
	movl	%eax, 52(%ebp)
	movl	%edx, 36(%ebp)
	addl	%ebx, %ebp
	decl	%ecx
	jnz	.salsa_tangle

	popl	%ebp
	popl	%ebx
	ret


/* neoscrypt_chacha(mem, rounds) = SSE2 based ChaCha20;
 * mem must be aligned properly, rounds must be a multiple of 2 */
.globl neoscrypt_chacha
.globl _neoscrypt_chacha
neoscrypt_chacha:
_neoscrypt_chacha:
	movl	4(%esp), %edx
	movl	8(%esp), %ecx
	shrl	$1, %ecx
	movdqa	0(%edx), %xmm0
	movdqa	%xmm0, %xmm5
	movdqa	16(%edx), %xmm1
	movdqa	%xmm1, %xmm6
	movdqa	32(%edx), %xmm2
	movdqa	%xmm2, %xmm7
	subl	$16, %esp
	movdqa	48(%edx), %xmm3
	movdqu	%xmm3, 0(%esp)
.chacha:
	paddd	%xmm1, %xmm0
	pxor 	%xmm0, %xmm3
	pshuflw	$0xB1, %xmm3, %xmm3
	pshufhw	$0xB1, %xmm3, %xmm3
	paddd	%xmm3, %xmm2
	pxor 	%xmm2, %xmm1
	movdqa	%xmm1, %xmm4
	pslld	$12, %xmm1
	psrld	$20, %xmm4
	pxor	%xmm4, %xmm1
	paddd	%xmm1, %xmm0
	pxor	%xmm0, %xmm3
	movdqa	%xmm3, %xmm4
	pslld	$8, %xmm3
	psrld	$24, %xmm4
	pxor	%xmm4, %xmm3
	pshufd	$0x93, %xmm0, %xmm0
	paddd	%xmm3, %xmm2
	pshufd	$0x4E, %xmm3, %xmm3
	pxor	%xmm2, %xmm1
	pshufd	$0x39, %xmm2, %xmm2
	movdqa	%xmm1, %xmm4
	pslld	$7, %xmm1
	psrld	$25, %xmm4
	pxor	%xmm4, %xmm1
	paddd	%xmm1, %xmm0
	pxor	%xmm0, %xmm3
	pshuflw	$0xB1, %xmm3, %xmm3
	pshufhw $0xB1, %xmm3, %xmm3
	paddd	%xmm3, %xmm2
	pxor	%xmm2, %xmm1
	movdqa	%xmm1, %xmm4
	pslld	$12, %xmm1
	psrld	$20, %xmm4
	pxor	%xmm4, %xmm1
	paddd	%xmm1, %xmm0
	pxor	%xmm0, %xmm3
	movdqa	%xmm3, %xmm4
	pslld	$8, %xmm3
	psrld	$24, %xmm4
	pxor	%xmm4, %xmm3
	pshufd	$0x39, %xmm0, %xmm0
	paddd	%xmm3, %xmm2
	pshufd	$0x4E, %xmm3, %xmm3
	pxor	%xmm2, %xmm1
	pshufd	$0x93, %xmm2, %xmm2
	movdqa	%xmm1, %xmm4
	pslld	$7, %xmm1
	psrld	$25, %xmm4
	pxor	%xmm4, %xmm1
	decl	%ecx
	jnz	.chacha

	paddd	%xmm5, %xmm0
	movdqa	%xmm0, 0(%edx)
	paddd	%xmm6, %xmm1
	movdqa	%xmm1, 16(%edx)
	paddd	%xmm7, %xmm2
	movdqa	%xmm2, 32(%edx)
	movdqu	0(%esp), %xmm7
	paddd	%xmm7, %xmm3
	movdqa	%xmm3, 48(%edx)
	addl	$16, %esp
	ret

#endif /* (ASM) && (__i386__) */
