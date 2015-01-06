/*
 * Copyright (c) 2014-2015 John Doering <ghostlander@phoenixcoin.org>
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

/* neoscrypt_copy(dst, src, len)
 * AMD64 memcpy() */
.globl neoscrypt_copy
.globl _neoscrypt_copy
neoscrypt_copy:
_neoscrypt_copy:
#if (WIN64)
	movq	%rdi, %r10
	movq	%rsi, %r11
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	movq	%r8, %rdx
#endif
	xorq	%rcx, %rcx
	movl	%edx, %ecx
	shrq	$4, %rcx
	xorq	%r9, %r9
	cmpq	%r9, %rcx
	jz	.4byte_copy_test
.16byte_copy:
	movq	0(%rsi), %rax
	movq	8(%rsi), %r8
	movq	%rax, 0(%rdi)
	movq	%r8, 8(%rdi)
	addq	$16, %rsi
	addq	$16, %rdi
	decq	%rcx
	jnz	.16byte_copy

.4byte_copy_test:
	movl	%edx, %ecx
	shrq	$2, %rcx
	andq	$0x3, %rcx
	cmpq	%r9, %rcx
	jz	.byte_copy_test
.4byte_copy:
	movl	0(%rsi), %eax
	movl	%eax, 0(%rdi)
	addq	$4, %rsi
	addq	$4, %rdi
	decq	%rcx
	jnz	.4byte_copy

.byte_copy_test:
	movl	%edx, %ecx
	andq	$0x3, %rcx
	cmpq	%r9, %rcx
	jz	.copy_finish
.byte_copy:
	movb	0(%rsi), %al
	movb	%al, 0(%rdi)
	incq	%rsi
	incq	%rdi
	decq	%rcx
	jnz	.byte_copy

.copy_finish:
#if (WIN64)
	movq	%r10, %rdi
	movq	%r11, %rsi
#endif
	ret


/* neoscrypt_erase(dst, len)
 * AMD64 memory eraser */
.globl neoscrypt_erase
.globl _neoscrypt_erase
neoscrypt_erase:
_neoscrypt_erase:
#if (WIN64)
	movq	%rdi, %r10
	movq	%rsi, %r11
	movq	%rcx, %rdi
	movq	%rdx, %rsi
#endif
	xorq	%rcx, %rcx
	movl	%esi, %ecx
	shrq	$4, %rcx
	xorq	%rax, %rax
	cmpq	%rax, %rcx
	jz	.4byte_erase_test
.16byte_erase:
	movq	%rax, 0(%rdi)
	movq	%rax, 8(%rdi)
	addq	$16, %rdi
	decq	%rcx
	jnz	.16byte_erase

.4byte_erase_test:
	movl	%esi, %ecx
	shrq	$2, %rcx
	andq	$0x3, %rcx
	cmpq	%rax, %rcx
	jz	.byte_erase_test
.4byte_erase:
	movl	%eax, 0(%rdi)
	addq	$4, %rdi
	decq	%rcx
	jnz	.4byte_erase

.byte_erase_test:
	movl	%esi, %ecx
	andq	$0x3, %rcx
	cmpq	%rax, %rcx
	jz	.erase_finish
.byte_erase:
	movb	%al, 0(%rdi)
	incq	%rdi
	decq	%rcx
	jnz	.byte_erase

.erase_finish:
#if (WIN64)
	movq	%r10, %rdi
	movq	%r11, %rsi
#endif
	ret


/* neoscrypt_xor(dst, src, len)
 * AMD64 XOR engine */
.globl neoscrypt_xor
.globl _neoscrypt_xor
neoscrypt_xor:
_neoscrypt_xor:
#if (WIN64)
	movq	%rdi, %r10
	movq	%rsi, %r11
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	movq	%r8, %rdx
#endif
	xorq	%rcx, %rcx
	movl	%edx, %ecx
	shrq	$4, %rcx
	xorq	%r9, %r9
	cmpq	%r9, %rcx
	jz	.4byte_xor_test
.16byte_xor:
	movq	0(%rsi), %rax
	movq	8(%rsi), %r8
	xorq	0(%rdi), %rax
	xorq	8(%rdi), %r8
	movq	%rax, 0(%rdi)
	movq	%r8, 8(%rdi)
	addq	$16, %rsi
	addq	$16, %rdi
	decq	%rcx
	jnz	.16byte_xor

.4byte_xor_test:
	movl	%edx, %ecx
	shrq	$2, %rcx
	andq	$0x3, %rcx
	cmpq	%r9, %rcx
	jz	.byte_xor_test
.4byte_xor:
	movl	0(%rsi), %eax
	xorl	0(%rdi), %eax
	movl	%eax, 0(%rdi)
	addq	$4, %rsi
	addq	$4, %rdi
	decq	%rcx
	jnz	.4byte_xor

.byte_xor_test:
	movl	%edx, %ecx
	andq	$0x3, %rcx
	cmpq	%r9, %rcx
	jz	.xor_finish
.byte_xor:
	movb	0(%rsi), %al
	xorb	0(%rdi), %al
	movb	%al, 0(%rdi)
	incq	%rsi
	incq	%rdi
	decq	%rcx
	jnz	.byte_xor

.xor_finish:
#if (WIN64)
	movq	%r10, %rdi
	movq	%r11, %rsi
#endif
	ret


/* neoscrypt_salsa_tangle(mem, count)
 * AMD64 (SSE2) Salsa20 map switcher;
 * correct map:  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
 * SSE2 map:     0   5  10  15  12   1   6  11   8  13   2   7   4   9  14   3
 * NOTE: arguments passed in %r8 and %r9; %rbx not preserved */
neoscrypt_salsa_tangle_sse2:
.salsa_tangle_sse2:
	movl	4(%r8), %eax
	movl	20(%r8), %ebx
	movl	8(%r8), %ecx
	movl	40(%r8), %edx
	movl	%eax, 20(%r8)
	movl	%ebx, 4(%r8)
	movl	%ecx, 40(%r8)
	movl	%edx, 8(%r8)
	movl	12(%r8), %eax
	movl	60(%r8), %ebx
	movl	16(%r8), %ecx
	movl	48(%r8), %edx
	movl	%eax, 60(%r8)
	movl	%ebx, 12(%r8)
	movl	%ecx, 48(%r8)
	movl	%edx, 16(%r8)
	movl	28(%r8), %eax
	movl	44(%r8), %ebx
	movl	36(%r8), %ecx
	movl	52(%r8), %edx
	movl	%eax, 44(%r8)
	movl	%ebx, 28(%r8)
	movl	%ecx, 52(%r8)
	movl	%edx, 36(%r8)
	addq	$64, %r8
	decq	%r9
	jnz	.salsa_tangle_sse2

	ret


/* neoscrypt_xor_salsa_sse2(mem, xormem, double_rounds)
 * AMD64 (SSE2) Salsa20 with XOR;
 * mem and xormem must be aligned properly;
 * NOTE: arguments passed in %r8, %r9, %r10 */
neoscrypt_xor_salsa_sse2:
	movdqa	0(%r8), %xmm0
	movdqa	16(%r8), %xmm1
	movdqa	32(%r8), %xmm2
	movdqa	48(%r8), %xmm3
	pxor	0(%r9), %xmm0
	pxor	16(%r9), %xmm1
	pxor	32(%r9), %xmm2
	pxor	48(%r9), %xmm3
	movdqa	%xmm0, %xmm12
	movdqa	%xmm1, %xmm13
	movdqa	%xmm2, %xmm14
	movdqa	%xmm3, %xmm15
.xor_salsa_sse2:
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
	decq	%r10
	jnz	.xor_salsa_sse2

	paddd	%xmm12, %xmm0
	paddd	%xmm13, %xmm1
	paddd	%xmm14, %xmm2
	paddd	%xmm15, %xmm3
	movdqa	%xmm0, 0(%r8)
	movdqa	%xmm1, 16(%r8)
	movdqa	%xmm2, 32(%r8)
	movdqa	%xmm3, 48(%r8)

	ret


/* neoscrypt_xor_chacha_sse2(mem, xormem, double_rounds)
 * AMD64 (SSE2) ChaCha20 with XOR;
 * mem and xormem must be aligned properly;
 * NOTE: arguments passed in %r8, %r9, %r10 */
neoscrypt_xor_chacha_sse2:
	movdqa	0(%r8), %xmm0
	movdqa	16(%r8), %xmm1
	movdqa	32(%r8), %xmm2
	movdqa	48(%r8), %xmm3
	pxor	0(%r9), %xmm0
	pxor	16(%r9), %xmm1
	pxor	32(%r9), %xmm2
	pxor	48(%r9), %xmm3
	movdqa	%xmm0, %xmm12
	movdqa	%xmm1, %xmm13
	movdqa	%xmm2, %xmm14
	movdqa	%xmm3, %xmm15
.xor_chacha_sse2:
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
	decq	%r10
	jnz	.xor_chacha_sse2

	paddd	%xmm12, %xmm0
	paddd	%xmm13, %xmm1
	paddd	%xmm14, %xmm2
	paddd	%xmm15, %xmm3
	movdqa	%xmm0, 0(%r8)
	movdqa	%xmm1, 16(%r8)
	movdqa	%xmm2, 32(%r8)
	movdqa	%xmm3, 48(%r8)

	ret


/* neoscrypt_xor_salsa(mem, xormem, tempmem, double_rounds)
 * AMD64 (INT) Salsa20 with XOR (SSE2 support required);
 * NOTE: arguments passed in %r8, %r9, %r10, %r11 */
neoscrypt_xor_salsa:
/* XOR and copy to temporary memory */
	movdqa	0(%r8), %xmm0
	movdqa	16(%r8), %xmm1
	movdqa	32(%r8), %xmm2
	movdqa	48(%r8), %xmm3
	pxor	0(%r9), %xmm0
	pxor	16(%r9), %xmm1
	pxor	32(%r9), %xmm2
	pxor	48(%r9), %xmm3
	movdqa	%xmm0, 0(%r10)
	movdqa	%xmm1, 16(%r10)
	movdqa	%xmm2, 32(%r10)
	movdqa	%xmm3, 48(%r10)
	movdqa	%xmm0, %xmm12
	movdqa	%xmm1, %xmm13
	movdqa	%xmm2, %xmm14
	movdqa	%xmm3, %xmm15
.xor_salsa:
/* quarters A and B, initial C and D */
	movl	0(%r10), %eax	/* A: load a */
	movl	20(%r10), %ebx	/* B: load a */
	addl	48(%r10), %eax	/* A: t = a + d */
	addl	4(%r10), %ebx	/* B: t = a + d */
	roll	$7, %eax	/* A: rotate t */
	roll	$7, %ebx	/* B: rotate t */
	xorl	16(%r10), %eax	/* A: b = b ^ t */
	xorl	36(%r10), %ebx	/* B: b = b ^ t */
	movl	%eax, %esi	/* A: copy b */
	movl	%ebx, %edi	/* B: copy b */
	movl	%esi, 16(%r10)	/* A: store b */
	movl	%edi, 36(%r10)	/* B: store b */
	addl	0(%r10), %eax	/* A: t = b + a */
	addl	20(%r10), %ebx	/* B: t = b + a */
	roll	$9, %eax	/* A: rotate t */
	roll	$9, %ebx	/* B: rotate t */
	xorl	32(%r10), %eax	/* A: c = c ^ t */
	xorl	52(%r10), %ebx	/* B: c = c ^ t */
	movl	%eax, %ecx	/* A: copy c */
	movl	%ebx, %edx	/* B: copy c */
	movl	%ecx, 32(%r10)	/* A: store c */
	movl	%edx, 52(%r10)	/* B: store c */
	addl	%esi, %eax	/* A: t = c + b */
	addl	%edi, %ebx	/* B: t = c + b */
	roll	$13, %eax	/* A: rotate t */
	roll	$13, %ebx	/* B: rotate t */
	xorl	48(%r10), %eax	/* A: d = d ^ t */
	xorl	4(%r10), %ebx	/* B: d = d ^ t */
	movl	%eax, 48(%r10)	/* A: store d */
	movl	%ebx, 4(%r10)	/* B: store d */
	addl	%eax, %ecx	/* A: t = d + c */
	movl	40(%r10), %eax	/* C: load a */
	addl	%ebx, %edx	/* B: t = d + c */
	movl	60(%r10), %ebx	/* D: load a */
	roll	$18, %ecx	/* A: rotate t */
	addl	24(%r10), %eax	/* C: t = a + d */
	roll	$18, %edx	/* B: rotate t */
	addl	44(%r10), %ebx	/* D: t = a + d */
	xorl	0(%r10), %ecx	/* A: a = a ^ t */
	roll	$7, %eax	/* C: rotate t */
	xorl	20(%r10), %edx	/* B: a = a ^ t */
	roll	$7, %ebx	/* D: rotate t */
	movl	%ecx, 0(%r10)	/* A: store a */
	movl	%edx, 20(%r10)	/* B: store a */
/* quarters C and D, initial E and F */
	xorl	56(%r10), %eax	/* C: b = b ^ t */
	xorl	12(%r10), %ebx	/* D: b = b ^ t */
	movl	%eax, %esi	/* C: copy b */
	movl	%ebx, %edi	/* D: copy b */
	movl	%esi, 56(%r10)	/* C: store b */
	movl	%edi, 12(%r10)	/* D: store b */
	addl	40(%r10), %eax	/* C: t = b + a */
	addl	60(%r10), %ebx	/* D: t = b + a */
	roll	$9, %eax	/* C: rotate t */
	roll	$9, %ebx	/* D: rotate t */
	xorl	8(%r10), %eax	/* C: c = c ^ t */
	xorl	28(%r10), %ebx	/* D: c = c ^ t */
	movl	%eax, %ecx	/* C: copy c */
	movl	%ebx, %edx	/* D: copy c */
	movl	%ecx, 8(%r10)	/* C: store c */
	movl	%edx, 28(%r10)	/* D: store c */
	addl	%esi, %eax	/* C: t = c + b */
	addl	%edi, %ebx	/* D: t = c + b */
	roll	$13, %eax	/* C: rotate t */
	roll	$13, %ebx	/* D: rotate t */
	xorl	24(%r10), %eax	/* C: d = d ^ t */
	xorl	44(%r10), %ebx	/* D: d = d ^ t */
	movl	%eax, 24(%r10)	/* C: store d */
	movl	%ebx, 44(%r10)	/* D: store d */
	addl	%eax, %ecx	/* C: t = d + c */
	movl	0(%r10), %eax	/* E: load a */
	addl	%ebx, %edx	/* D: t = d + c */
	movl	20(%r10), %ebx	/* F: load a */
	roll	$18, %ecx	/* C: rotate t */
	addl	12(%r10), %eax	/* E: t = a + d */
	roll	$18, %edx	/* D: rotate t */
	addl	16(%r10), %ebx	/* F: t = a + d */
	xorl	40(%r10), %ecx	/* C: a = a ^ t */
	roll	$7, %eax	/* E: rotate t */
	xorl	60(%r10), %edx	/* D: a = a ^ t */
	roll	$7, %ebx	/* F: rotate t */
	movl	%ecx, 40(%r10)	/* C: store a */
	movl	%edx, 60(%r10)	/* D: store a */
/* quarters E and F, initial G and H */
	xorl	4(%r10), %eax	/* E: b = b ^ t */
	xorl	24(%r10), %ebx	/* F: b = b ^ t */
	movl	%eax, %esi	/* E: copy b */
	movl	%ebx, %edi	/* F: copy b */
	movl	%esi, 4(%r10)	/* E: store b */
	movl	%edi, 24(%r10)	/* F: store b */
	addl	0(%r10), %eax	/* E: t = b + a */
	addl	20(%r10), %ebx	/* F: t = b + a */
	roll	$9, %eax	/* E: rotate t */
	roll	$9, %ebx	/* F: rotate t */
	xorl	8(%r10), %eax	/* E: c = c ^ t */
	xorl	28(%r10), %ebx	/* F: c = c ^ t */
	movl	%eax, %ecx	/* E: copy c */
	movl	%ebx, %edx	/* F: copy c */
	movl	%ecx, 8(%r10)	/* E: store c */
	movl	%edx, 28(%r10)	/* F: store c */
	addl	%esi, %eax	/* E: t = c + b */
	addl	%edi, %ebx	/* F: t = c + b */
	roll	$13, %eax	/* E: rotate t */
	roll	$13, %ebx	/* F: rotate t */
	xorl	12(%r10), %eax	/* E: d = d ^ t */
	xorl	16(%r10), %ebx	/* F: d = d ^ t */
	movl	%eax, 12(%r10)	/* E: store d */
	movl	%ebx, 16(%r10)	/* F: store d */
	addl	%eax, %ecx	/* E: t = d + c */
	movl	40(%r10), %eax	/* G: load a */
	addl	%ebx, %edx	/* F: t = d + c */
	movl	60(%r10), %ebx	/* H: load a */
	roll	$18, %ecx	/* E: rotate t */
	addl	36(%r10), %eax	/* G: t = a + d */
	roll	$18, %edx	/* F: rotate t */
	addl	56(%r10), %ebx	/* H: t = a + d */
	xorl	0(%r10), %ecx	/* E: a = a ^ t */
	roll	$7, %eax	/* G: rotate t */
	xorl	20(%r10), %edx	/* F: a = a ^ t */
	roll	$7, %ebx	/* H: rotate t */
	movl	%ecx, 0(%r10)	/* E: store a */
	movl	%edx, 20(%r10)	/* F: store a */
/* quarters G and H */
	xorl	44(%r10), %eax	/* G: b = b ^ t */
	xorl	48(%r10), %ebx	/* H: b = b ^ t */
	movl	%eax, %esi	/* G: copy b */
	movl	%ebx, %edi	/* H: copy b */
	movl	%esi, 44(%r10)	/* G: store b */
	movl	%edi, 48(%r10)	/* H: store b */
	addl	40(%r10), %eax	/* G: t = b + a */
	addl	60(%r10), %ebx	/* H: t = b + a */
	roll	$9, %eax	/* G: rotate t */
	roll	$9, %ebx	/* H: rotate t */
	xorl	32(%r10), %eax	/* G: c = c ^ t */
	xorl	52(%r10), %ebx	/* H: c = c ^ t */
	movl	%eax, %ecx	/* G: copy c */
	movl	%ebx, %edx	/* H: copy c */
	movl	%ecx, 32(%r10)	/* G: store c */
	movl	%edx, 52(%r10)	/* H: store c */
	addl	%esi, %eax	/* G: t = c + b */
	addl	%edi, %ebx	/* H: t = c + b */
	roll	$13, %eax	/* G: rotate t */
	roll	$13, %ebx	/* H: rotate t */
	xorl	36(%r10), %eax	/* G: d = d ^ t */
	xorl	56(%r10), %ebx	/* H: d = d ^ t */
	movl	%eax, 36(%r10)	/* G: store d */
	movl	%ebx, 56(%r10)	/* H: store d */
	addl	%eax, %ecx	/* G: t = d + c */
	addl	%ebx, %edx	/* H: t = d + c */
	roll	$18, %ecx	/* G: rotate t */
	roll	$18, %edx	/* H: rotate t */
	xorl	40(%r10), %ecx	/* G: a = a ^ t */
	xorl	60(%r10), %edx	/* H: a = a ^ t */
	movl	%ecx, 40(%r10)	/* G: store a */
	movl	%edx, 60(%r10)	/* H: store a */
	decq	%r11
	jnz	.xor_salsa

	movdqa	0(%r10), %xmm0
	movdqa	16(%r10), %xmm1
	movdqa	32(%r10), %xmm2
	movdqa	48(%r10), %xmm3
	paddd	%xmm12, %xmm0
	paddd	%xmm13, %xmm1
	paddd	%xmm14, %xmm2
	paddd	%xmm15, %xmm3
	movdqa	%xmm0, 0(%r8)
	movdqa	%xmm1, 16(%r8)
	movdqa	%xmm2, 32(%r8)
	movdqa	%xmm3, 48(%r8)

	ret


/* neoscrypt_xor_chacha(mem, xormem, tempmem, double_rounds)
 * AMD64 (INT) ChaCha20 with XOR (SSE2 support required);
 * NOTE: arguments passed in %r8, %r9, %r10, %r11 */
neoscrypt_xor_chacha:
/* XOR and copy to temporary memory */
	movdqa	0(%r8), %xmm0
	movdqa	16(%r8), %xmm1
	movdqa	32(%r8), %xmm2
	movdqa	48(%r8), %xmm3
	pxor	0(%r9), %xmm0
	pxor	16(%r9), %xmm1
	pxor	32(%r9), %xmm2
	pxor	48(%r9), %xmm3
	movdqa	%xmm0, 0(%r10)
	movdqa	%xmm1, 16(%r10)
	movdqa	%xmm2, 32(%r10)
	movdqa	%xmm3, 48(%r10)
	movdqa	%xmm0, %xmm12
	movdqa	%xmm1, %xmm13
	movdqa	%xmm2, %xmm14
	movdqa	%xmm3, %xmm15
.xor_chacha:
/* quarters A and B, initial C */
	movl	0(%r10), %eax	/* A: load a */
	movl	16(%r10), %ebx	/* A: load b */
	addl	%ebx, %eax	/* A: a = a + b */
	movl	32(%r10), %ecx	/* A: load c */
	movl	48(%r10), %edx	/* A: load d */
	xorl	%eax, %edx	/* A: d = d ^ a */
	movl	4(%r10), %edi	/* B: load a */
	roll	$16, %edx	/* A: rotate d */
	movl	20(%r10), %esi	/* B: load b */
	addl	%edx, %ecx	/* A: c = c + d */
	xorl	%ecx, %ebx	/* A: b = b ^ c */
	addl	%esi, %edi	/* B: a = a + b */
	roll	$12, %ebx	/* A: rotate b */
	addl	%ebx, %eax	/* A: a = a + b */
	movl	%eax, 0(%r10)	/* A: store a */
	xorl	%eax, %edx	/* A: d = d ^ a */
	movl	52(%r10), %eax	/* B: load d */
	roll	$8, %edx	/* A: rotate d */
	xorl	%edi, %eax	/* B: d = d ^ a */
	movl	%edx, 48(%r10)	/* A: store d */
	addl	%edx, %ecx	/* A: c = c + d */
	movl	36(%r10), %edx	/* B: load c */
	movl	%ecx, 32(%r10)	/* A: store c */
	xorl	%ecx, %ebx	/* A: b = b ^ c */
	roll	$16, %eax	/* B: rotate d */
	movl	40(%r10), %ecx	/* C: load c */
	roll	$7, %ebx	/* A: rotate b */
	addl	%eax, %edx	/* B: c = c + d */
	movl	%ebx, 16(%r10)	/* A: store b */
	xorl	%edx, %esi	/* B: b = b ^ c */
	movl	24(%r10), %ebx	/* C: load b */
	roll	$12, %esi	/* B: rotate b */
	addl	%esi, %edi	/* B: a = a + b */
	movl	%edi, 4(%r10)	/* B: store a */
	xorl	%edi, %eax	/* B: d = d ^ a */
	roll	$8, %eax	/* B: rotate d */
	movl	%eax, 52(%r10)	/* B: store d */
	addl	%eax, %edx	/* B: c = c + d */
	movl	8(%r10), %eax	/* C: load a */
	movl	%edx, 36(%r10)	/* B: store c */
	xorl	%edx, %esi	/* B: b = b ^ c */
	movl	56(%r10), %edx	/* C: load d */
	roll	$7, %esi	/* B: rotate b */
	addl	%ebx, %eax	/* C: a = a + b */
	movl	%esi, 20(%r10)	/* B: store b */
/* quarters C and D, initial E */
	xorl	%eax, %edx	/* C: d = d ^ a */
	movl	12(%r10), %edi	/* D: load a */
	roll	$16, %edx	/* C: rotate d */
	movl	28(%r10), %esi	/* D: load b */
	addl	%edx, %ecx	/* C: c = c + d */
	xorl	%ecx, %ebx	/* C: b = b ^ c */
	addl	%esi, %edi	/* D: a = a + b */
	roll	$12, %ebx	/* C: rotate b */
	addl	%ebx, %eax	/* C: a = a + b */
	movl	%eax, 8(%r10)	/* C: store a */
	xorl	%eax, %edx	/* C: d = d ^ a */
	movl	60(%r10), %eax	/* D: load d */
	roll	$8, %edx	/* C: rotate d */
	xorl	%edi, %eax	/* D: d = d ^ a */
	movl	%edx, 56(%r10)	/* C: store d */
	addl	%edx, %ecx	/* C: c = c + d */
	movl	44(%r10), %edx	/* D: load c */
	movl	%ecx, 40(%r10)	/* C: store c */
	xorl	%ecx, %ebx	/* C: b = b ^ c */
	roll	$16, %eax	/* D: rotate d */
	movl	40(%r10), %ecx	/* E: load c */
	roll	$7, %ebx	/* C: rotate b */
	addl	%eax, %edx	/* D: c = c + d */
	movl	%ebx, 24(%r10)	/* C: store b */
	xorl	%edx, %esi	/* D: b = b ^ c */
	movl	20(%r10), %ebx	/* E: load b */
	roll	$12, %esi	/* D: rotate b */
	addl	%esi, %edi	/* D: a = a + b */
	movl	%edi, 12(%r10)	/* D: store a */
	xorl	%edi, %eax	/* D: d = d ^ a */
	roll	$8, %eax	/* D: rotate d */
	movl	%eax, 60(%r10)	/* D: store d */
	addl	%eax, %edx	/* D: c = c + d */
	movl	0(%r10), %eax	/* E: load a */
	movl	%edx, 44(%r10)	/* D: store c */
	xorl	%edx, %esi	/* D: b = b ^ c */
	movl	60(%r10), %edx	/* E: load d */
	roll	$7, %esi	/* D: rotate b */
	addl	%ebx, %eax	/* E: a = a + b */
	movl	%esi, 28(%r10)	/* D: store b */
/* quarters E and F, initial G */
	xorl	%eax, %edx	/* E: d = d ^ a */
	movl	4(%r10), %edi	/* F: load a */
	roll	$16, %edx	/* E: rotate d */
	movl	24(%r10), %esi	/* F: load b */
	addl	%edx, %ecx	/* E: c = c + d */
	xorl	%ecx, %ebx	/* E: b = b ^ c */
	addl	%esi, %edi	/* F: a = a + b */
	roll	$12, %ebx	/* E: rotate b */
	addl	%ebx, %eax	/* E: a = a + b */
	movl	%eax, 0(%r10)	/* E: store a */
	xorl	%eax, %edx	/* E: d = d ^ a */
	movl	48(%r10), %eax	/* F: load d */
	roll	$8, %edx	/* E: rotate d */
	xorl	%edi, %eax	/* F: d = d ^ a */
	movl	%edx, 60(%r10)	/* E: store d */
	addl	%edx, %ecx	/* E: c = c + d */
	movl	44(%r10), %edx	/* F: load c */
	movl	%ecx, 40(%r10)	/* E: store c */
	xorl	%ecx, %ebx	/* E: b = b ^ c */
	roll	$16, %eax	/* F: rotate d */
	movl	32(%r10), %ecx	/* G: load c */
	roll	$7, %ebx	/* E: rotate b */
	addl	%eax, %edx	/* F: c = c + d */
	movl	%ebx, 20(%r10)	/* E: store b */
	xorl	%edx, %esi	/* F: b = b ^ c */
	movl	28(%r10), %ebx	/* G: load b */
	roll	$12, %esi	/* F: rotate b */
	addl	%esi, %edi	/* F: a = a + b */
	movl	%edi, 4(%r10)	/* F: store a */
	xorl	%edi, %eax	/* F: d = d ^ a */
	roll	$8, %eax	/* F: rotate d */
	movl	%eax, 48(%r10)	/* F: store d */
	addl	%eax, %edx	/* F: c = c + d */
	movl	8(%r10), %eax	/* G: load a */
	movl	%edx, 44(%r10)	/* F: store c */
	xorl	%edx, %esi	/* F: b = b ^ c */
	movl	52(%r10), %edx	/* G: load d */
	roll	$7, %esi	/* F: rotate b */
	addl	%ebx, %eax	/* G: a = a + b */
	movl	%esi, 24(%r10)	/* F: store b */
/* quarters G and H */
	xorl	%eax, %edx	/* G: d = d ^ a */
	movl	12(%r10), %edi	/* H: load a */
	roll	$16, %edx	/* G: rotate d */
	movl	16(%r10), %esi	/* H: load b */
	addl	%edx, %ecx	/* G: c = c + d */
	xorl	%ecx, %ebx	/* G: b = b ^ c */
	addl	%esi, %edi	/* H: a = a + b */
	roll	$12, %ebx	/* G: rotate b */
	addl	%ebx, %eax	/* G: a = a + b */
	movl	%eax, 8(%r10)	/* G: store a */
	xorl	%eax, %edx	/* G: d = d ^ a */
	movl	56(%r10), %eax	/* H: load d */
	roll	$8, %edx	/* G: rotate d */
	xorl	%edi, %eax	/* H: d = d ^ a */
	movl	%edx, 52(%r10)	/* G: store d */
	addl	%edx, %ecx	/* G: c = c + d */
	movl	36(%r10), %edx	/* H: load c */
	movl	%ecx, 32(%r10)	/* G: store c */
	xorl	%ecx, %ebx	/* G: b = b ^ c */
	roll	$16, %eax	/* H: rotate d */
	roll	$7, %ebx	/* G: rotate b */
	addl	%eax, %edx	/* H: c = c + d */
	movl	%ebx, 28(%r10)	/* G: store b */
	xorl	%edx, %esi	/* H: b = b ^ c */
	roll	$12, %esi	/* H: rotate b */
	addl	%esi, %edi	/* H: a = a + b */
	movl	%edi, 12(%r10)	/* H: store a */
	xorl	%edi, %eax	/* H: d = d ^ a */
	roll	$8, %eax	/* H: rotate d */
	movl	%eax, 56(%r10)	/* H: store d */
	addl	%eax, %edx	/* H: c = c + d */
	movl	%edx, 36(%r10)	/* H: store c */
	xorl	%edx, %esi	/* H: b = b ^ c */
	roll	$7, %esi	/* H: rotate b */
	movl	%esi, 16(%r10)	/* H: store b */
	decq	%r11
	jnz	.xor_chacha

	movdqa	0(%r10), %xmm0
	movdqa	16(%r10), %xmm1
	movdqa	32(%r10), %xmm2
	movdqa	48(%r10), %xmm3
	paddd	%xmm12, %xmm0
	paddd	%xmm13, %xmm1
	paddd	%xmm14, %xmm2
	paddd	%xmm15, %xmm3
	movdqa	%xmm0, 0(%r8)
	movdqa	%xmm1, 16(%r8)
	movdqa	%xmm2, 32(%r8)
	movdqa	%xmm3, 48(%r8)

	ret


/* neoscrypt(input, output, profile)
 * AMD64 (INT, SSE2) NeoScrypt engine (SSE2 required for INT);
 * supports NeoScrypt and Scrypt only */
.globl neoscrypt
.globl _neoscrypt
neoscrypt:
_neoscrypt:
#if (WIN64)
	pushq	%rdi
	pushq	%rsi
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	movq	%r8, %rdx
#endif
	pushq	%rbx
	pushq	%rbp
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15
/* save input, output and profile */
	movq	%rdi, %r14
	movq	%rsi, %r15
	movq	%rdx, %rbx

#if (SHA256)
/* Scrypt mode */
	testl	$0x01, %ebx
	jnz	.scrypt
#endif

#if (WIN64)
/* attempt to allocate 33280 + 128 bytes of stack space fails miserably;
 * have to use malloc() and free() instead */
	subq	$128, %rsp
/* allocate memory (9 pages of 4Kb each) */
	movq	$0x9000, %rcx
	call	malloc
/* save memory address */
	movq	%rax, 64(%rsp)
/* align memory */
	addq	$64, %rax
	andq	$0xFFFFFFFFFFFFFFC0, %rax
/* memory base: X, Z, V */
	leaq	64(%rax), %rbp
#else
/* align stack */
	movq	%rsp, %rax
	andq	$0xFFFFFFFFFFFFFFC0, %rsp
	subq	$0x8280, %rsp
/* save unaligned stack */
	movq	%rax, 32(%rsp)
/* memory base: X, Z, V */
	leaq	128(%rsp), %rbp
#endif /* (WIN64) */

/* FastKDF */
#if (WIN64)
#if (OPT)
	movq	%r14, %rcx
	movq	%r14, %rdx
	movq	%rbp, %r8
	xorq	%r9, %r9
	call	neoscrypt_fastkdf_opt
#else
	movq	$80, %rax
	movq	%r14, %rcx
	movq	%rax, %rdx
	movq	%r14, %r8
	movq	%rax, %r9
	movq	$32, 32(%rsp)
	movq	%rbp, 40(%rsp)
	movq	$256, 48(%rsp)
#endif /* (OPT) */
#else
#if (OPT)
	movq	%r14, %rdi
	movq	%r14, %rsi
	movq	%rbp, %rdx
	xorq	%rcx, %rcx
#if (__APPLE__)
	call	_neoscrypt_fastkdf_opt
#else
	call	neoscrypt_fastkdf_opt
#endif /* (__APPLE__) */
#else
	movq	$80, %rax
	movq	%r14, %rdi
	movq	%rax, %rsi
	movq	%r14, %rdx
	movq	%rax, %rcx
	movq	$32, %r8
	movq	%rbp, %r9
	movq	$256, 0(%rsp)
#if (__APPLE__)
	call	_neoscrypt_fastkdf
#else
	call	neoscrypt_fastkdf
#endif /* (__APPLE__) */
#endif /* (OPT) */
#endif /* (WIN64) */

/* blkcpy(Z, X) */
	leaq	256(%rbp), %rax
	movdqa	0(%rbp), %xmm0
	movdqa	16(%rbp), %xmm1
	movdqa	32(%rbp), %xmm2
	movdqa	48(%rbp), %xmm3
	movdqa	64(%rbp), %xmm4
	movdqa	80(%rbp), %xmm5
	movdqa	96(%rbp), %xmm6
	movdqa	112(%rbp), %xmm7
	movdqa	128(%rbp), %xmm8
	movdqa	144(%rbp), %xmm9
	movdqa	160(%rbp), %xmm10
	movdqa	176(%rbp), %xmm11
	movdqa	192(%rbp), %xmm12
	movdqa	208(%rbp), %xmm13
	movdqa	224(%rbp), %xmm14
	movdqa	240(%rbp), %xmm15
	movdqa	%xmm0, 0(%rax)
	movdqa	%xmm1, 16(%rax)
	movdqa	%xmm2, 32(%rax)
	movdqa	%xmm3, 48(%rax)
	movdqa	%xmm4, 64(%rax)
	movdqa	%xmm5, 80(%rax)
	movdqa	%xmm6, 96(%rax)
	movdqa	%xmm7, 112(%rax)
	movdqa	%xmm8, 128(%rax)
	movdqa	%xmm9, 144(%rax)
	movdqa	%xmm10, 160(%rax)
	movdqa	%xmm11, 176(%rax)
	movdqa	%xmm12, 192(%rax)
	movdqa	%xmm13, 208(%rax)
	movdqa	%xmm14, 224(%rax)
	movdqa	%xmm15, 240(%rax)

/* SSE2 switch */
	testl	$0x1000, %ebx
	jnz	.neoscrypt_sse2

/* tempmem and double rounds */
	leaq	-64(%rbp), %r10
	movq	$10, %r12

	xorq	%r13, %r13
.chacha_ns1:
/* blkcpy(V, Z) */
	leaq	512(%rbp), %rax
	movq	%r13, %rdx
	movb	$8, %cl
	shlq	%cl, %rdx
	leaq	256(%rbp), %rcx
	addq	%rdx, %rax
	movdqa	0(%rcx), %xmm0
	movdqa	16(%rcx), %xmm1
	movdqa	32(%rcx), %xmm2
	movdqa	48(%rcx), %xmm3
	movdqa	64(%rcx), %xmm4
	movdqa	80(%rcx), %xmm5
	movdqa	96(%rcx), %xmm6
	movdqa	112(%rcx), %xmm7
	movdqa	128(%rcx), %xmm8
	movdqa	144(%rcx), %xmm9
	movdqa	160(%rcx), %xmm10
	movdqa	176(%rcx), %xmm11
	movdqa	192(%rcx), %xmm12
	movdqa	208(%rcx), %xmm13
	movdqa	224(%rcx), %xmm14
	movdqa	240(%rcx), %xmm15
	movdqa	%xmm0, 0(%rax)
	movdqa	%xmm1, 16(%rax)
	movdqa	%xmm2, 32(%rax)
	movdqa	%xmm3, 48(%rax)
	movdqa	%xmm4, 64(%rax)
	movdqa	%xmm5, 80(%rax)
	movdqa	%xmm6, 96(%rax)
	movdqa	%xmm7, 112(%rax)
	movdqa	%xmm8, 128(%rax)
	movdqa	%xmm9, 144(%rax)
	movdqa	%xmm10, 160(%rax)
	movdqa	%xmm11, 176(%rax)
	movdqa	%xmm12, 192(%rax)
	movdqa	%xmm13, 208(%rax)
	movdqa	%xmm14, 224(%rax)
	movdqa	%xmm15, 240(%rax)
/* blkmix(Z) */
	leaq	256(%rbp), %r8
	leaq	448(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_chacha
	leaq	320(%rbp), %r8
	leaq	256(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_chacha
	leaq	384(%rbp), %r8
	leaq	320(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_chacha
	leaq	448(%rbp), %r8
	leaq	384(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_chacha
	leaq	320(%rbp), %rax
	leaq	384(%rbp), %rdx
	movdqa	0(%rax), %xmm0
	movdqa	16(%rax), %xmm1
	movdqa	32(%rax), %xmm2
	movdqa	48(%rax), %xmm3
	movdqa	0(%rdx), %xmm4
	movdqa	16(%rdx), %xmm5
	movdqa	32(%rdx), %xmm6
	movdqa	48(%rdx), %xmm7
	movdqa	%xmm0, 0(%rdx)
	movdqa	%xmm1, 16(%rdx)
	movdqa	%xmm2, 32(%rdx)
	movdqa	%xmm3, 48(%rdx)
	movdqa	%xmm4, 0(%rax)
	movdqa	%xmm5, 16(%rax)
	movdqa	%xmm6, 32(%rax)
	movdqa	%xmm7, 48(%rax)
	incq	%r13
	cmpq	$128, %r13
	jnz	.chacha_ns1

	xorq	%r13, %r13
.chacha_ns2:
/* integerify(Z) mod 128 */
	leaq	256(%rbp), %rax
	leaq	512(%rbp), %rcx
	xorq	%rdx, %rdx
	movl	448(%rbp), %edx
	andl	$0x7F, %edx
	shlq	$8, %rdx
	addq	%rdx, %rcx
/* blkxor(Z, V) */
	movdqa	0(%rax), %xmm0
	movdqa	16(%rax), %xmm1
	movdqa	32(%rax), %xmm2
	movdqa	48(%rax), %xmm3
	movdqa	64(%rax), %xmm4
	movdqa	80(%rax), %xmm5
	movdqa	96(%rax), %xmm6
	movdqa	112(%rax), %xmm7
	movdqa	128(%rax), %xmm8
	movdqa	144(%rax), %xmm9
	movdqa	160(%rax), %xmm10
	movdqa	176(%rax), %xmm11
	movdqa	192(%rax), %xmm12
	movdqa	208(%rax), %xmm13
	movdqa	224(%rax), %xmm14
	movdqa	240(%rax), %xmm15
	pxor	0(%rcx), %xmm0
	pxor	16(%rcx), %xmm1
	pxor	32(%rcx), %xmm2
	pxor	48(%rcx), %xmm3
	pxor	64(%rcx), %xmm4
	pxor	80(%rcx), %xmm5
	pxor	96(%rcx), %xmm6
	pxor	112(%rcx), %xmm7
	pxor	128(%rcx), %xmm8
	pxor	144(%rcx), %xmm9
	pxor	160(%rcx), %xmm10
	pxor	176(%rcx), %xmm11
	pxor	192(%rcx), %xmm12
	pxor	208(%rcx), %xmm13
	pxor	224(%rcx), %xmm14
	pxor	240(%rcx), %xmm15
	movdqa	%xmm0, 0(%rax)
	movdqa	%xmm1, 16(%rax)
	movdqa	%xmm2, 32(%rax)
	movdqa	%xmm3, 48(%rax)
	movdqa	%xmm4, 64(%rax)
	movdqa	%xmm5, 80(%rax)
	movdqa	%xmm6, 96(%rax)
	movdqa	%xmm7, 112(%rax)
	movdqa	%xmm8, 128(%rax)
	movdqa	%xmm9, 144(%rax)
	movdqa	%xmm10, 160(%rax)
	movdqa	%xmm11, 176(%rax)
	movdqa	%xmm12, 192(%rax)
	movdqa	%xmm13, 208(%rax)
	movdqa	%xmm14, 224(%rax)
	movdqa	%xmm15, 240(%rax)
/* blkmix(Z) */
	leaq	256(%rbp), %r8
	leaq	448(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_chacha
	leaq	320(%rbp), %r8
	leaq	256(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_chacha
	leaq	384(%rbp), %r8
	leaq	320(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_chacha
	leaq	448(%rbp), %r8
	leaq	384(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_chacha
	leaq	320(%rbp), %rax
	leaq	384(%rbp), %rdx
	movdqa	0(%rax), %xmm0
	movdqa	16(%rax), %xmm1
	movdqa	32(%rax), %xmm2
	movdqa	48(%rax), %xmm3
	movdqa	0(%rdx), %xmm4
	movdqa	16(%rdx), %xmm5
	movdqa	32(%rdx), %xmm6
	movdqa	48(%rdx), %xmm7
	movdqa	%xmm0, 0(%rdx)
	movdqa	%xmm1, 16(%rdx)
	movdqa	%xmm2, 32(%rdx)
	movdqa	%xmm3, 48(%rdx)
	movdqa	%xmm4, 0(%rax)
	movdqa	%xmm5, 16(%rax)
	movdqa	%xmm6, 32(%rax)
	movdqa	%xmm7, 48(%rax)
	incq	%r13
	cmpq	$128, %r13
	jnz	.chacha_ns2

	xorq	%r13, %r13
.salsa_ns1:
/* blkcpy(V, X) */
	leaq	512(%rbp), %rax
	movq	%r13, %rdx
	movb	$8, %cl
	shlq	%cl, %rdx
	addq	%rdx, %rax
	movdqa	0(%rbp), %xmm0
	movdqa	16(%rbp), %xmm1
	movdqa	32(%rbp), %xmm2
	movdqa	48(%rbp), %xmm3
	movdqa	64(%rbp), %xmm4
	movdqa	80(%rbp), %xmm5
	movdqa	96(%rbp), %xmm6
	movdqa	112(%rbp), %xmm7
	movdqa	128(%rbp), %xmm8
	movdqa	144(%rbp), %xmm9
	movdqa	160(%rbp), %xmm10
	movdqa	176(%rbp), %xmm11
	movdqa	192(%rbp), %xmm12
	movdqa	208(%rbp), %xmm13
	movdqa	224(%rbp), %xmm14
	movdqa	240(%rbp), %xmm15
	movdqa	%xmm0, 0(%rax)
	movdqa	%xmm1, 16(%rax)
	movdqa	%xmm2, 32(%rax)
	movdqa	%xmm3, 48(%rax)
	movdqa	%xmm4, 64(%rax)
	movdqa	%xmm5, 80(%rax)
	movdqa	%xmm6, 96(%rax)
	movdqa	%xmm7, 112(%rax)
	movdqa	%xmm8, 128(%rax)
	movdqa	%xmm9, 144(%rax)
	movdqa	%xmm10, 160(%rax)
	movdqa	%xmm11, 176(%rax)
	movdqa	%xmm12, 192(%rax)
	movdqa	%xmm13, 208(%rax)
	movdqa	%xmm14, 224(%rax)
	movdqa	%xmm15, 240(%rax)
/* blkmix(X) */
	movq	%rbp, %r8
	leaq	192(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_salsa
	leaq	64(%rbp), %r8
	movq	%rbp, %r9
	movq	%r12, %r11
	call	neoscrypt_xor_salsa
	leaq	128(%rbp), %r8
	leaq	64(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_salsa
	leaq	192(%rbp), %r8
	leaq	128(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_salsa
	leaq	64(%rbp), %rax
	leaq	128(%rbp), %rdx
	movdqa	0(%rax), %xmm0
	movdqa	16(%rax), %xmm1
	movdqa	32(%rax), %xmm2
	movdqa	48(%rax), %xmm3
	movdqa	0(%rdx), %xmm4
	movdqa	16(%rdx), %xmm5
	movdqa	32(%rdx), %xmm6
	movdqa	48(%rdx), %xmm7
	movdqa	%xmm0, 0(%rdx)
	movdqa	%xmm1, 16(%rdx)
	movdqa	%xmm2, 32(%rdx)
	movdqa	%xmm3, 48(%rdx)
	movdqa	%xmm4, 0(%rax)
	movdqa	%xmm5, 16(%rax)
	movdqa	%xmm6, 32(%rax)
	movdqa	%xmm7, 48(%rax)
	incq	%r13
	cmpq	$128, %r13
	jnz	.salsa_ns1

	xorq	%r13, %r13
.salsa_ns2:
/* integerify(X) mod 128 */
	leaq	512(%rbp), %rcx
	xorq	%rdx, %rdx
	movl	192(%rbp), %edx
	andl	$0x7F, %edx
	shlq	$8, %rdx
	addq	%rdx, %rcx
/* blkxor(X, V) */
	movdqa	0(%rbp), %xmm0
	movdqa	16(%rbp), %xmm1
	movdqa	32(%rbp), %xmm2
	movdqa	48(%rbp), %xmm3
	movdqa	64(%rbp), %xmm4
	movdqa	80(%rbp), %xmm5
	movdqa	96(%rbp), %xmm6
	movdqa	112(%rbp), %xmm7
	movdqa	128(%rbp), %xmm8
	movdqa	144(%rbp), %xmm9
	movdqa	160(%rbp), %xmm10
	movdqa	176(%rbp), %xmm11
	movdqa	192(%rbp), %xmm12
	movdqa	208(%rbp), %xmm13
	movdqa	224(%rbp), %xmm14
	movdqa	240(%rbp), %xmm15
	pxor	0(%rcx), %xmm0
	pxor	16(%rcx), %xmm1
	pxor	32(%rcx), %xmm2
	pxor	48(%rcx), %xmm3
	pxor	64(%rcx), %xmm4
	pxor	80(%rcx), %xmm5
	pxor	96(%rcx), %xmm6
	pxor	112(%rcx), %xmm7
	pxor	128(%rcx), %xmm8
	pxor	144(%rcx), %xmm9
	pxor	160(%rcx), %xmm10
	pxor	176(%rcx), %xmm11
	pxor	192(%rcx), %xmm12
	pxor	208(%rcx), %xmm13
	pxor	224(%rcx), %xmm14
	pxor	240(%rcx), %xmm15
	movdqa	%xmm0, 0(%rbp)
	movdqa	%xmm1, 16(%rbp)
	movdqa	%xmm2, 32(%rbp)
	movdqa	%xmm3, 48(%rbp)
	movdqa	%xmm4, 64(%rbp)
	movdqa	%xmm5, 80(%rbp)
	movdqa	%xmm6, 96(%rbp)
	movdqa	%xmm7, 112(%rbp)
	movdqa	%xmm8, 128(%rbp)
	movdqa	%xmm9, 144(%rbp)
	movdqa	%xmm10, 160(%rbp)
	movdqa	%xmm11, 176(%rbp)
	movdqa	%xmm12, 192(%rbp)
	movdqa	%xmm13, 208(%rbp)
	movdqa	%xmm14, 224(%rbp)
	movdqa	%xmm15, 240(%rbp)
/* blkmix(X) */
	movq	%rbp, %r8
	leaq	192(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_salsa
	leaq	64(%rbp), %r8
	movq	%rbp, %r9
	movq	%r12, %r11
	call	neoscrypt_xor_salsa
	leaq	128(%rbp), %r8
	leaq	64(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_salsa
	leaq	192(%rbp), %r8
	leaq	128(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_salsa
	leaq	64(%rbp), %rax
	leaq	128(%rbp), %rdx
	movdqa	0(%rax), %xmm0
	movdqa	16(%rax), %xmm1
	movdqa	32(%rax), %xmm2
	movdqa	48(%rax), %xmm3
	movdqa	0(%rdx), %xmm4
	movdqa	16(%rdx), %xmm5
	movdqa	32(%rdx), %xmm6
	movdqa	48(%rdx), %xmm7
	movdqa	%xmm0, 0(%rdx)
	movdqa	%xmm1, 16(%rdx)
	movdqa	%xmm2, 32(%rdx)
	movdqa	%xmm3, 48(%rdx)
	movdqa	%xmm4, 0(%rax)
	movdqa	%xmm5, 16(%rax)
	movdqa	%xmm6, 32(%rax)
	movdqa	%xmm7, 48(%rax)
	incq	%r13
	cmpq	$128, %r13
	jnz	.salsa_ns2

/* blkxor(X, Z) */
	leaq	256(%rbp), %rcx
	movdqa	0(%rbp), %xmm0
	movdqa	16(%rbp), %xmm1
	movdqa	32(%rbp), %xmm2
	movdqa	48(%rbp), %xmm3
	movdqa	64(%rbp), %xmm4
	movdqa	80(%rbp), %xmm5
	movdqa	96(%rbp), %xmm6
	movdqa	112(%rbp), %xmm7
	movdqa	128(%rbp), %xmm8
	movdqa	144(%rbp), %xmm9
	movdqa	160(%rbp), %xmm10
	movdqa	176(%rbp), %xmm11
	movdqa	192(%rbp), %xmm12
	movdqa	208(%rbp), %xmm13
	movdqa	224(%rbp), %xmm14
	movdqa	240(%rbp), %xmm15
	pxor	0(%rcx), %xmm0
	pxor	16(%rcx), %xmm1
	pxor	32(%rcx), %xmm2
	pxor	48(%rcx), %xmm3
	pxor	64(%rcx), %xmm4
	pxor	80(%rcx), %xmm5
	pxor	96(%rcx), %xmm6
	pxor	112(%rcx), %xmm7
	pxor	128(%rcx), %xmm8
	pxor	144(%rcx), %xmm9
	pxor	160(%rcx), %xmm10
	pxor	176(%rcx), %xmm11
	pxor	192(%rcx), %xmm12
	pxor	208(%rcx), %xmm13
	pxor	224(%rcx), %xmm14
	pxor	240(%rcx), %xmm15
	movdqa	%xmm0, 0(%rbp)
	movdqa	%xmm1, 16(%rbp)
	movdqa	%xmm2, 32(%rbp)
	movdqa	%xmm3, 48(%rbp)
	movdqa	%xmm4, 64(%rbp)
	movdqa	%xmm5, 80(%rbp)
	movdqa	%xmm6, 96(%rbp)
	movdqa	%xmm7, 112(%rbp)
	movdqa	%xmm8, 128(%rbp)
	movdqa	%xmm9, 144(%rbp)
	movdqa	%xmm10, 160(%rbp)
	movdqa	%xmm11, 176(%rbp)
	movdqa	%xmm12, 192(%rbp)
	movdqa	%xmm13, 208(%rbp)
	movdqa	%xmm14, 224(%rbp)
	movdqa	%xmm15, 240(%rbp)

/* FastKDF */
#if (WIN64)
#if (OPT)
	movq	%r14, %rcx
	movq	%rbp, %rdx
	movq	%r15, %r8
	xorq	%r9, %r9
	incq	%r9
	call	neoscrypt_fastkdf_opt
#else
	movq	%r14, %rcx
	movq	$80, %rdx
	movq	%rbp, %r8
	movq	$256, %r9
	movq	$32, %rax
	movq	%rax, 32(%rsp)
	movq	%r15, 40(%rsp)
	movq	%rax, 48(%rsp)
	call	neoscrypt_fastkdf
#endif /* (OPT) */
#else
#if (OPT)
	movq	%r14, %rdi
	movq	%rbp, %rsi
	movq	%r15, %rdx
	xorq	%rcx, %rcx
	incq	%rcx
#if (__APPLE__)
	call	_neoscrypt_fastkdf_opt
#else
	call	neoscrypt_fastkdf_opt
#endif /* (__APPLE__) */
#else
	movq	%r14, %rdi
	movq	$80, %rsi
	movq	%rbp, %rdx
	movq	$256, %rcx
	movq	$32, %r8
	movq	%r15, %r9
	movq	$32, 0(%rsp)
#if (__APPLE__)
	call	_neoscrypt_fastkdf
#else
	call	neoscrypt_fastkdf
#endif /* (__APPLE__) */
#endif /* (OPT) */
#endif /* (WIN64) */

#if (WIN64)
/* free memory */
	movq	64(%rsp), %rcx
	call	free
/* restore stack */
	addq	$128, %rsp
#else
/* restore stack */
	movq	32(%rsp), %rsp
#endif
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbp
	popq	%rbx
#if (WIN64)
	popq	%rsi
	popq	%rdi
#endif
	ret

.neoscrypt_sse2:
	movq	$10, %r12

	xorq	%r13, %r13
.chacha_ns1_sse2:
/* blkcpy(V, Z) */
	leaq	512(%rbp), %rax
	movq	%r13, %rdx
	movb	$8, %cl
	shlq	%cl, %rdx
	leaq	256(%rbp), %rcx
	addq	%rdx, %rax
	movdqa	0(%rcx), %xmm0
	movdqa	16(%rcx), %xmm1
	movdqa	32(%rcx), %xmm2
	movdqa	48(%rcx), %xmm3
	movdqa	64(%rcx), %xmm4
	movdqa	80(%rcx), %xmm5
	movdqa	96(%rcx), %xmm6
	movdqa	112(%rcx), %xmm7
	movdqa	128(%rcx), %xmm8
	movdqa	144(%rcx), %xmm9
	movdqa	160(%rcx), %xmm10
	movdqa	176(%rcx), %xmm11
	movdqa	192(%rcx), %xmm12
	movdqa	208(%rcx), %xmm13
	movdqa	224(%rcx), %xmm14
	movdqa	240(%rcx), %xmm15
	movdqa	%xmm0, 0(%rax)
	movdqa	%xmm1, 16(%rax)
	movdqa	%xmm2, 32(%rax)
	movdqa	%xmm3, 48(%rax)
	movdqa	%xmm4, 64(%rax)
	movdqa	%xmm5, 80(%rax)
	movdqa	%xmm6, 96(%rax)
	movdqa	%xmm7, 112(%rax)
	movdqa	%xmm8, 128(%rax)
	movdqa	%xmm9, 144(%rax)
	movdqa	%xmm10, 160(%rax)
	movdqa	%xmm11, 176(%rax)
	movdqa	%xmm12, 192(%rax)
	movdqa	%xmm13, 208(%rax)
	movdqa	%xmm14, 224(%rax)
	movdqa	%xmm15, 240(%rax)
/* blkmix(Z) */
	leaq	256(%rbp), %r8
	leaq	448(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_chacha_sse2
	leaq	320(%rbp), %r8
	leaq	256(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_chacha_sse2
	leaq	384(%rbp), %r8
	leaq	320(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_chacha_sse2
	leaq	448(%rbp), %r8
	leaq	384(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_chacha_sse2
	leaq	320(%rbp), %rax
	leaq	384(%rbp), %rdx
	movdqa	0(%rax), %xmm0
	movdqa	16(%rax), %xmm1
	movdqa	32(%rax), %xmm2
	movdqa	48(%rax), %xmm3
	movdqa	0(%rdx), %xmm4
	movdqa	16(%rdx), %xmm5
	movdqa	32(%rdx), %xmm6
	movdqa	48(%rdx), %xmm7
	movdqa	%xmm0, 0(%rdx)
	movdqa	%xmm1, 16(%rdx)
	movdqa	%xmm2, 32(%rdx)
	movdqa	%xmm3, 48(%rdx)
	movdqa	%xmm4, 0(%rax)
	movdqa	%xmm5, 16(%rax)
	movdqa	%xmm6, 32(%rax)
	movdqa	%xmm7, 48(%rax)
	incq	%r13
	cmpq	$128, %r13
	jnz	.chacha_ns1_sse2

	xorq	%r13, %r13
.chacha_ns2_sse2:
/* integerify(Z) mod 128 */
	leaq	256(%rbp), %rax
	leaq	512(%rbp), %rcx
	xorq	%rdx, %rdx
	movl	448(%rbp), %edx
	andl	$0x7F, %edx
	shlq	$8, %rdx
	addq	%rdx, %rcx
/* blkxor(Z, V) */
	movdqa	0(%rax), %xmm0
	movdqa	16(%rax), %xmm1
	movdqa	32(%rax), %xmm2
	movdqa	48(%rax), %xmm3
	movdqa	64(%rax), %xmm4
	movdqa	80(%rax), %xmm5
	movdqa	96(%rax), %xmm6
	movdqa	112(%rax), %xmm7
	movdqa	128(%rax), %xmm8
	movdqa	144(%rax), %xmm9
	movdqa	160(%rax), %xmm10
	movdqa	176(%rax), %xmm11
	movdqa	192(%rax), %xmm12
	movdqa	208(%rax), %xmm13
	movdqa	224(%rax), %xmm14
	movdqa	240(%rax), %xmm15
	pxor	0(%rcx), %xmm0
	pxor	16(%rcx), %xmm1
	pxor	32(%rcx), %xmm2
	pxor	48(%rcx), %xmm3
	pxor	64(%rcx), %xmm4
	pxor	80(%rcx), %xmm5
	pxor	96(%rcx), %xmm6
	pxor	112(%rcx), %xmm7
	pxor	128(%rcx), %xmm8
	pxor	144(%rcx), %xmm9
	pxor	160(%rcx), %xmm10
	pxor	176(%rcx), %xmm11
	pxor	192(%rcx), %xmm12
	pxor	208(%rcx), %xmm13
	pxor	224(%rcx), %xmm14
	pxor	240(%rcx), %xmm15
	movdqa	%xmm0, 0(%rax)
	movdqa	%xmm1, 16(%rax)
	movdqa	%xmm2, 32(%rax)
	movdqa	%xmm3, 48(%rax)
	movdqa	%xmm4, 64(%rax)
	movdqa	%xmm5, 80(%rax)
	movdqa	%xmm6, 96(%rax)
	movdqa	%xmm7, 112(%rax)
	movdqa	%xmm8, 128(%rax)
	movdqa	%xmm9, 144(%rax)
	movdqa	%xmm10, 160(%rax)
	movdqa	%xmm11, 176(%rax)
	movdqa	%xmm12, 192(%rax)
	movdqa	%xmm13, 208(%rax)
	movdqa	%xmm14, 224(%rax)
	movdqa	%xmm15, 240(%rax)
/* blkmix(Z) */
	leaq	256(%rbp), %r8
	leaq	448(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_chacha_sse2
	leaq	320(%rbp), %r8
	leaq	256(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_chacha_sse2
	leaq	384(%rbp), %r8
	leaq	320(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_chacha_sse2
	leaq	448(%rbp), %r8
	leaq	384(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_chacha_sse2
	leaq	320(%rbp), %rax
	leaq	384(%rbp), %rdx
	movdqa	0(%rax), %xmm0
	movdqa	16(%rax), %xmm1
	movdqa	32(%rax), %xmm2
	movdqa	48(%rax), %xmm3
	movdqa	0(%rdx), %xmm4
	movdqa	16(%rdx), %xmm5
	movdqa	32(%rdx), %xmm6
	movdqa	48(%rdx), %xmm7
	movdqa	%xmm0, 0(%rdx)
	movdqa	%xmm1, 16(%rdx)
	movdqa	%xmm2, 32(%rdx)
	movdqa	%xmm3, 48(%rdx)
	movdqa	%xmm4, 0(%rax)
	movdqa	%xmm5, 16(%rax)
	movdqa	%xmm6, 32(%rax)
	movdqa	%xmm7, 48(%rax)
	incq	%r13
	cmpq	$128, %r13
	jnz	.chacha_ns2_sse2

	movq	%rbp, %r8
	movq	$4, %r9
	call	neoscrypt_salsa_tangle_sse2

	xorq	%r13, %r13
.salsa_ns1_sse2:
/* blkcpy(V, X) */
	leaq	512(%rbp), %rax
	movq	%r13, %rdx
	movb	$8, %cl
	shlq	%cl, %rdx
	addq	%rdx, %rax
	movdqa	0(%rbp), %xmm0
	movdqa	16(%rbp), %xmm1
	movdqa	32(%rbp), %xmm2
	movdqa	48(%rbp), %xmm3
	movdqa	64(%rbp), %xmm4
	movdqa	80(%rbp), %xmm5
	movdqa	96(%rbp), %xmm6
	movdqa	112(%rbp), %xmm7
	movdqa	128(%rbp), %xmm8
	movdqa	144(%rbp), %xmm9
	movdqa	160(%rbp), %xmm10
	movdqa	176(%rbp), %xmm11
	movdqa	192(%rbp), %xmm12
	movdqa	208(%rbp), %xmm13
	movdqa	224(%rbp), %xmm14
	movdqa	240(%rbp), %xmm15
	movdqa	%xmm0, 0(%rax)
	movdqa	%xmm1, 16(%rax)
	movdqa	%xmm2, 32(%rax)
	movdqa	%xmm3, 48(%rax)
	movdqa	%xmm4, 64(%rax)
	movdqa	%xmm5, 80(%rax)
	movdqa	%xmm6, 96(%rax)
	movdqa	%xmm7, 112(%rax)
	movdqa	%xmm8, 128(%rax)
	movdqa	%xmm9, 144(%rax)
	movdqa	%xmm10, 160(%rax)
	movdqa	%xmm11, 176(%rax)
	movdqa	%xmm12, 192(%rax)
	movdqa	%xmm13, 208(%rax)
	movdqa	%xmm14, 224(%rax)
	movdqa	%xmm15, 240(%rax)
/* blkmix(X) */
	movq	%rbp, %r8
	leaq	192(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_salsa_sse2
	leaq	64(%rbp), %r8
	movq	%rbp, %r9
	movq	%r12, %r10
	call	neoscrypt_xor_salsa_sse2
	leaq	128(%rbp), %r8
	leaq	64(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_salsa_sse2
	leaq	192(%rbp), %r8
	leaq	128(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_salsa_sse2
	leaq	64(%rbp), %rax
	leaq	128(%rbp), %rdx
	movdqa	0(%rax), %xmm0
	movdqa	16(%rax), %xmm1
	movdqa	32(%rax), %xmm2
	movdqa	48(%rax), %xmm3
	movdqa	0(%rdx), %xmm4
	movdqa	16(%rdx), %xmm5
	movdqa	32(%rdx), %xmm6
	movdqa	48(%rdx), %xmm7
	movdqa	%xmm0, 0(%rdx)
	movdqa	%xmm1, 16(%rdx)
	movdqa	%xmm2, 32(%rdx)
	movdqa	%xmm3, 48(%rdx)
	movdqa	%xmm4, 0(%rax)
	movdqa	%xmm5, 16(%rax)
	movdqa	%xmm6, 32(%rax)
	movdqa	%xmm7, 48(%rax)
	incq	%r13
	cmpq	$128, %r13
	jnz	.salsa_ns1_sse2

	xorq	%r13, %r13
.salsa_ns2_sse2:
/* integerify(X) mod 128 */
	leaq	512(%rbp), %rcx
	xorq	%rdx, %rdx
	movl	192(%rbp), %edx
	andl	$0x7F, %edx
	shlq	$8, %rdx
	addq	%rdx, %rcx
/* blkxor(X, V) */
	movdqa	0(%rbp), %xmm0
	movdqa	16(%rbp), %xmm1
	movdqa	32(%rbp), %xmm2
	movdqa	48(%rbp), %xmm3
	movdqa	64(%rbp), %xmm4
	movdqa	80(%rbp), %xmm5
	movdqa	96(%rbp), %xmm6
	movdqa	112(%rbp), %xmm7
	movdqa	128(%rbp), %xmm8
	movdqa	144(%rbp), %xmm9
	movdqa	160(%rbp), %xmm10
	movdqa	176(%rbp), %xmm11
	movdqa	192(%rbp), %xmm12
	movdqa	208(%rbp), %xmm13
	movdqa	224(%rbp), %xmm14
	movdqa	240(%rbp), %xmm15
	pxor	0(%rcx), %xmm0
	pxor	16(%rcx), %xmm1
	pxor	32(%rcx), %xmm2
	pxor	48(%rcx), %xmm3
	pxor	64(%rcx), %xmm4
	pxor	80(%rcx), %xmm5
	pxor	96(%rcx), %xmm6
	pxor	112(%rcx), %xmm7
	pxor	128(%rcx), %xmm8
	pxor	144(%rcx), %xmm9
	pxor	160(%rcx), %xmm10
	pxor	176(%rcx), %xmm11
	pxor	192(%rcx), %xmm12
	pxor	208(%rcx), %xmm13
	pxor	224(%rcx), %xmm14
	pxor	240(%rcx), %xmm15
	movdqa	%xmm0, 0(%rbp)
	movdqa	%xmm1, 16(%rbp)
	movdqa	%xmm2, 32(%rbp)
	movdqa	%xmm3, 48(%rbp)
	movdqa	%xmm4, 64(%rbp)
	movdqa	%xmm5, 80(%rbp)
	movdqa	%xmm6, 96(%rbp)
	movdqa	%xmm7, 112(%rbp)
	movdqa	%xmm8, 128(%rbp)
	movdqa	%xmm9, 144(%rbp)
	movdqa	%xmm10, 160(%rbp)
	movdqa	%xmm11, 176(%rbp)
	movdqa	%xmm12, 192(%rbp)
	movdqa	%xmm13, 208(%rbp)
	movdqa	%xmm14, 224(%rbp)
	movdqa	%xmm15, 240(%rbp)
/* blkmix(X) */
	movq	%rbp, %r8
	leaq	192(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_salsa_sse2
	leaq	64(%rbp), %r8
	movq	%rbp, %r9
	movq	%r12, %r10
	call	neoscrypt_xor_salsa_sse2
	leaq	128(%rbp), %r8
	leaq	64(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_salsa_sse2
	leaq	192(%rbp), %r8
	leaq	128(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_salsa_sse2
	leaq	64(%rbp), %rax
	leaq	128(%rbp), %rdx
	movdqa	0(%rax), %xmm0
	movdqa	16(%rax), %xmm1
	movdqa	32(%rax), %xmm2
	movdqa	48(%rax), %xmm3
	movdqa	0(%rdx), %xmm4
	movdqa	16(%rdx), %xmm5
	movdqa	32(%rdx), %xmm6
	movdqa	48(%rdx), %xmm7
	movdqa	%xmm0, 0(%rdx)
	movdqa	%xmm1, 16(%rdx)
	movdqa	%xmm2, 32(%rdx)
	movdqa	%xmm3, 48(%rdx)
	movdqa	%xmm4, 0(%rax)
	movdqa	%xmm5, 16(%rax)
	movdqa	%xmm6, 32(%rax)
	movdqa	%xmm7, 48(%rax)
	incq	%r13
	cmpq	$128, %r13
	jnz	.salsa_ns2_sse2

	movq	%rbp, %r8
	movq	$4, %r9
	call	neoscrypt_salsa_tangle_sse2

/* blkxor(X, Z) */
	leaq	256(%rbp), %rcx
	movdqa	0(%rbp), %xmm0
	movdqa	16(%rbp), %xmm1
	movdqa	32(%rbp), %xmm2
	movdqa	48(%rbp), %xmm3
	movdqa	64(%rbp), %xmm4
	movdqa	80(%rbp), %xmm5
	movdqa	96(%rbp), %xmm6
	movdqa	112(%rbp), %xmm7
	movdqa	128(%rbp), %xmm8
	movdqa	144(%rbp), %xmm9
	movdqa	160(%rbp), %xmm10
	movdqa	176(%rbp), %xmm11
	movdqa	192(%rbp), %xmm12
	movdqa	208(%rbp), %xmm13
	movdqa	224(%rbp), %xmm14
	movdqa	240(%rbp), %xmm15
	pxor	0(%rcx), %xmm0
	pxor	16(%rcx), %xmm1
	pxor	32(%rcx), %xmm2
	pxor	48(%rcx), %xmm3
	pxor	64(%rcx), %xmm4
	pxor	80(%rcx), %xmm5
	pxor	96(%rcx), %xmm6
	pxor	112(%rcx), %xmm7
	pxor	128(%rcx), %xmm8
	pxor	144(%rcx), %xmm9
	pxor	160(%rcx), %xmm10
	pxor	176(%rcx), %xmm11
	pxor	192(%rcx), %xmm12
	pxor	208(%rcx), %xmm13
	pxor	224(%rcx), %xmm14
	pxor	240(%rcx), %xmm15
	movdqa	%xmm0, 0(%rbp)
	movdqa	%xmm1, 16(%rbp)
	movdqa	%xmm2, 32(%rbp)
	movdqa	%xmm3, 48(%rbp)
	movdqa	%xmm4, 64(%rbp)
	movdqa	%xmm5, 80(%rbp)
	movdqa	%xmm6, 96(%rbp)
	movdqa	%xmm7, 112(%rbp)
	movdqa	%xmm8, 128(%rbp)
	movdqa	%xmm9, 144(%rbp)
	movdqa	%xmm10, 160(%rbp)
	movdqa	%xmm11, 176(%rbp)
	movdqa	%xmm12, 192(%rbp)
	movdqa	%xmm13, 208(%rbp)
	movdqa	%xmm14, 224(%rbp)
	movdqa	%xmm15, 240(%rbp)

/* FastKDF */
#if (WIN64)
#if (OPT)
	movq	%r14, %rcx
	movq	%rbp, %rdx
	movq	%r15, %r8
	xorq	%r9, %r9
	incq	%r9
	call	neoscrypt_fastkdf_opt
#else
	movq	%r14, %rcx
	movq	$80, %rdx
	movq	%rbp, %r8
	movq	$256, %r9
	movq	$32, %rax
	movq	%rax, 32(%rsp)
	movq	%r15, 40(%rsp)
	movq	%rax, 48(%rsp)
#endif /* (OPT) */
#else
#if (OPT)
	movq	%r14, %rdi
	movq	%rbp, %rsi
	movq	%r15, %rdx
	xorq	%rcx, %rcx
	incq	%rcx
#if (__APPLE__)
	call	_neoscrypt_fastkdf_opt
#else
	call	neoscrypt_fastkdf_opt
#endif /* (__APPLE__) */
#else
	movq	%r14, %rdi
	movq	$80, %rsi
	movq	%rbp, %rdx
	movq	$256, %rcx
	movq	$32, %rax
	movq	%rax, %r8
	movq	%r15, %r9
	movq	%rax, 0(%rsp)
#if (__APPLE__)
	call	_neoscrypt_fastkdf
#else
	call	neoscrypt_fastkdf
#endif /* (__APPLE__) */
#endif /* (OPT) */
#endif /* (WIN64) */

#if (WIN64)
/* free memory */
	movq	64(%rsp), %rcx
	call	free
/* restore stack */
	addq	$128, %rsp
#else
/* restore stack */
	movq	32(%rsp), %rsp
#endif
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbp
	popq	%rbx
#if (WIN64)
	popq	%rsi
	popq	%rdi
#endif
	ret

#if (SHA256)

.scrypt:
#if (WIN64)
/* attempt to allocate 131200 + 128 bytes of stack space fails miserably;
 * have to use malloc() and free() instead */
	subq	$128, %rsp
/* allocate memory (33 pages of 4Kb each) */
	movq	$0x21000, %rcx
	call	malloc
/* save memory address */
	movq	%rax, 64(%rsp)
/* align memory */
	addq	$64, %rax
	andq	$0xFFFFFFFFFFFFFFC0, %rax
/* memory base: X, Z, V */
	leaq	64(%rax), %rbp
#else
/* align stack */
	movq	%rsp, %rax
	andq	$0xFFFFFFFFFFFFFFC0, %rsp
	subq	$0x20100, %rsp
/* save unaligned stack */
	movq	%rax, 32(%rsp)
/* memory base: X, Z, V */
	leaq	128(%rsp), %rbp
#endif /* (WIN64) */

/* PBKDF2-HMAC-SHA256 */
#if (WIN64)
	movq	$80, %rax
	movq	%r14, %rcx
	movq	%rax, %rdx
	movq	%r14, %r8
	movq	%rax, %r9
	movq	$1, 32(%rsp)
	movq	%rbp, 40(%rsp)
	movq	$128, 48(%rsp)
	call	neoscrypt_pbkdf2_sha256
#else
	movq	$80, %rax
	movq	%r14, %rdi
	movq	%rax, %rsi
	movq	%r14, %rdx
	movq	%rax, %rcx
	movq	$1, %r8
	movq	%rbp, %r9
	movq	$128, 0(%rsp)
#if (__APPLE__)
	call	_neoscrypt_pbkdf2_sha256
#else
	call	neoscrypt_pbkdf2_sha256
#endif /* (__APPLE__) */
#endif /* (WIN64) */

/* SSE2 switch */
	testl	$0x1000, %ebx
	jnz	.scrypt_sse2

/* tempmem and double rounds */
	leaq	-64(%rbp), %r10
	movq	$4, %r12

	xorq	%r13, %r13
.salsa_s1:
/* blkcpy(V, X) */
	leaq	128(%rbp), %rax
	movq	%r13, %rdx
	movb	$7, %cl
	shlq	%cl, %rdx
	addq	%rdx, %rax
	movdqa	0(%rbp), %xmm0
	movdqa	16(%rbp), %xmm1
	movdqa	32(%rbp), %xmm2
	movdqa	48(%rbp), %xmm3
	movdqa	64(%rbp), %xmm4
	movdqa	80(%rbp), %xmm5
	movdqa	96(%rbp), %xmm6
	movdqa	112(%rbp), %xmm7
	movdqa	%xmm0, 0(%rax)
	movdqa	%xmm1, 16(%rax)
	movdqa	%xmm2, 32(%rax)
	movdqa	%xmm3, 48(%rax)
	movdqa	%xmm4, 64(%rax)
	movdqa	%xmm5, 80(%rax)
	movdqa	%xmm6, 96(%rax)
	movdqa	%xmm7, 112(%rax)
/* blkmix(X) */
	movq	%rbp, %r8
	leaq	64(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_salsa
	leaq	64(%rbp), %r8
	movq	%rbp, %r9
	movq	%r12, %r11
	call	neoscrypt_xor_salsa
	incq	%r13
	cmpq	$1024, %r13
	jnz	.salsa_s1

	xorq	%r13, %r13
.salsa_s2:
/* integerify(X) mod 1024 */
	leaq	128(%rbp), %rcx
	xorq	%rdx, %rdx
	movl	64(%rbp), %edx
	andl	$0x03FF, %edx
	shlq	$7, %rdx
	addq	%rdx, %rcx
/* blkxor(X, V) */
	movdqa	0(%rbp), %xmm0
	movdqa	16(%rbp), %xmm1
	movdqa	32(%rbp), %xmm2
	movdqa	48(%rbp), %xmm3
	movdqa	64(%rbp), %xmm4
	movdqa	80(%rbp), %xmm5
	movdqa	96(%rbp), %xmm6
	movdqa	112(%rbp), %xmm7
	pxor	0(%rcx), %xmm0
	pxor	16(%rcx), %xmm1
	pxor	32(%rcx), %xmm2
	pxor	48(%rcx), %xmm3
	pxor	64(%rcx), %xmm4
	pxor	80(%rcx), %xmm5
	pxor	96(%rcx), %xmm6
	pxor	112(%rcx), %xmm7
	movdqa	%xmm0, 0(%rbp)
	movdqa	%xmm1, 16(%rbp)
	movdqa	%xmm2, 32(%rbp)
	movdqa	%xmm3, 48(%rbp)
	movdqa	%xmm4, 64(%rbp)
	movdqa	%xmm5, 80(%rbp)
	movdqa	%xmm6, 96(%rbp)
	movdqa	%xmm7, 112(%rbp)
/* blkmix(X) */
	movq	%rbp, %r8
	leaq	64(%rbp), %r9
	movq	%r12, %r11
	call	neoscrypt_xor_salsa
	leaq	64(%rbp), %r8
	movq	%rbp, %r9
	movq	%r12, %r11
	call	neoscrypt_xor_salsa
	incq	%r13
	cmpq	$1024, %r13
	jnz	.salsa_s2

/* PBKDF2-HMAC-SHA256 */
#if (WIN64)
	movq	%r14, %rcx
	movq	$80, %rdx
	movq	%rbp, %r8
	movq	$128, %r9
	movq	$1, 32(%rsp)
	movq	%r15, 40(%rsp)
	movq	$32, 48(%rsp)
	call	neoscrypt_pbkdf2_sha256
#else
	movq	%r14, %rdi
	movq	$80, %rsi
	movq	%rbp, %rdx
	movq	$128, %rcx
	movq	$1, %r8
	movq	%r15, %r9
	movq	$32, 0(%rsp)
#if (__APPLE__)
	call	_neoscrypt_pbkdf2_sha256
#else
	call	neoscrypt_pbkdf2_sha256
#endif /* (__APPLE__) */

#endif /* (WIN64) */

#if (WIN64)
/* free memory */
	movq	64(%rsp), %rcx
	call	free
/* restore stack */
	addq	$128, %rsp
#else
/* restore stack */
	movq	32(%rsp), %rsp
#endif
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbp
	popq	%rbx
#if (WIN64)
	popq	%rsi
	popq	%rdi
#endif
	ret

.scrypt_sse2:
	movq	%rbp, %r8
	movq	$2, %r9
	call	neoscrypt_salsa_tangle_sse2

	movq	$4, %r12

	xorq	%r13, %r13
.salsa_s1_sse2:
/* blkcpy(V, X) */
	leaq	128(%rbp), %rax
	movq	%r13, %rdx
	movb	$7, %cl
	shlq	%cl, %rdx
	addq	%rdx, %rax
	movdqa	0(%rbp), %xmm0
	movdqa	16(%rbp), %xmm1
	movdqa	32(%rbp), %xmm2
	movdqa	48(%rbp), %xmm3
	movdqa	64(%rbp), %xmm4
	movdqa	80(%rbp), %xmm5
	movdqa	96(%rbp), %xmm6
	movdqa	112(%rbp), %xmm7
	movdqa	%xmm0, 0(%rax)
	movdqa	%xmm1, 16(%rax)
	movdqa	%xmm2, 32(%rax)
	movdqa	%xmm3, 48(%rax)
	movdqa	%xmm4, 64(%rax)
	movdqa	%xmm5, 80(%rax)
	movdqa	%xmm6, 96(%rax)
	movdqa	%xmm7, 112(%rax)
/* blkmix(X) */
	movq	%rbp, %r8
	leaq	64(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_salsa_sse2
	leaq	64(%rbp), %r8
	movq	%rbp, %r9
	movq	%r12, %r10
	call	neoscrypt_xor_salsa_sse2
	incq	%r13
	cmpq	$1024, %r13
	jnz	.salsa_s1_sse2

	xorq	%r13, %r13
.salsa_s2_sse2:
/* integerify(X) mod 1024 */
	leaq	128(%rbp), %rcx
	xorq	%rdx, %rdx
	movl	64(%rbp), %edx
	andl	$0x03FF, %edx
	shlq	$7, %rdx
	addq	%rdx, %rcx
/* blkxor(X, V) */
	movdqa	0(%rbp), %xmm0
	movdqa	16(%rbp), %xmm1
	movdqa	32(%rbp), %xmm2
	movdqa	48(%rbp), %xmm3
	movdqa	64(%rbp), %xmm4
	movdqa	80(%rbp), %xmm5
	movdqa	96(%rbp), %xmm6
	movdqa	112(%rbp), %xmm7
	pxor	0(%rcx), %xmm0
	pxor	16(%rcx), %xmm1
	pxor	32(%rcx), %xmm2
	pxor	48(%rcx), %xmm3
	pxor	64(%rcx), %xmm4
	pxor	80(%rcx), %xmm5
	pxor	96(%rcx), %xmm6
	pxor	112(%rcx), %xmm7
	movdqa	%xmm0, 0(%rbp)
	movdqa	%xmm1, 16(%rbp)
	movdqa	%xmm2, 32(%rbp)
	movdqa	%xmm3, 48(%rbp)
	movdqa	%xmm4, 64(%rbp)
	movdqa	%xmm5, 80(%rbp)
	movdqa	%xmm6, 96(%rbp)
	movdqa	%xmm7, 112(%rbp)
/* blkmix(X) */
	movq	%rbp, %r8
	leaq	64(%rbp), %r9
	movq	%r12, %r10
	call	neoscrypt_xor_salsa_sse2
	leaq	64(%rbp), %r8
	movq	%rbp, %r9
	movq	%r12, %r10
	call	neoscrypt_xor_salsa_sse2
	incq	%r13
	cmpq	$1024, %r13
	jnz	.salsa_s2_sse2

	movq	%rbp, %r8
	movq	$2, %r9
	call	neoscrypt_salsa_tangle_sse2

/* PBKDF2-HMAC-SHA256 */
#if (WIN64)
	movq	%r14, %rcx
	movq	$80, %rdx
	movq	%rbp, %r8
	movq	$128, %r9
	movq	$1, 32(%rsp)
	movq	%r15, 40(%rsp)
	movq	$32, 48(%rsp)
	call	neoscrypt_pbkdf2_sha256
#else
	movq	%r14, %rdi
	movq	$80, %rsi
	movq	%rbp, %rdx
	movq	$128, %rcx
	movq	$1, %r8
	movq	%r15, %r9
	movq	$32, 0(%rsp)
#if (__APPLE__)
	call	_neoscrypt_pbkdf2_sha256
#else
	call	neoscrypt_pbkdf2_sha256
#endif /* (__APPLE__) */
#endif /* (WIN64) */

#if (WIN64)
/* free memory */
	movq	64(%rsp), %rcx
	call	free
/* restore stack */
	addq	$128, %rsp
#else
/* restore stack */
	movq	32(%rsp), %rsp
#endif
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbp
	popq	%rbx
#if (WIN64)
	popq	%rsi
	popq	%rdi
#endif
	ret

#endif /* (SHA256) */

#if (MINER_4WAY)

/* neoscrypt_blkcpy(dst, src, len)
 * AMD64 (SSE2) block memcpy();
 * len must be a multiple of 64 bytes aligned properly */
.globl neoscrypt_blkcpy
.globl _neoscrypt_blkcpy
neoscrypt_blkcpy:
_neoscrypt_blkcpy:
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


/* neoscrypt_blkswp(blkA, blkB, len)
 * AMD64 (SSE2) block swapper;
 * len must be a multiple of 64 bytes aligned properly */
.globl neoscrypt_blkswp
.globl _neoscrypt_blkswp
neoscrypt_blkswp:
_neoscrypt_blkswp:
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


/* neoscrypt_blkxor(dst, src, len)
 * AMD64 (SSE2) block XOR engine;
 * len must be a multiple of 64 bytes aligned properly */
.globl neoscrypt_blkxor
.globl _neoscrypt_blkxor
neoscrypt_blkxor:
_neoscrypt_blkxor:
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
	pxor	0(%rsi), %xmm0
	pxor	16(%rsi), %xmm1
	pxor	32(%rsi), %xmm2
	pxor	48(%rsi), %xmm3
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


/* neoscrypt_xor_salsa_4way(mem, xormem, workmem, double_rounds)
 * AMD64 (SSE2) Salsa20 4-way with XOR */
.globl neoscrypt_xor_salsa_4way
.globl _neoscrypt_xor_salsa_4way
neoscrypt_xor_salsa_4way:
_neoscrypt_xor_salsa_4way:
#if (WIN64)
	movq	%rdi, %r10
	movq	%rsi, %r11
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	movq	%r8, %rdx
	movq	%r9, %rcx
#endif
/* XOR and copy to temporary memory */
	movdqa	0(%rdi), %xmm0
	movdqa	16(%rdi), %xmm1
	movdqa	32(%rdi), %xmm2
	movdqa	48(%rdi), %xmm3
	movdqa	64(%rdi), %xmm4
	movdqa	80(%rdi), %xmm5
	movdqa	96(%rdi), %xmm6
	movdqa	112(%rdi), %xmm7
	movdqa	128(%rdi), %xmm8
	movdqa	144(%rdi), %xmm9
	movdqa	160(%rdi), %xmm10
	movdqa	176(%rdi), %xmm11
	movdqa	192(%rdi), %xmm12
	movdqa	208(%rdi), %xmm13
	movdqa	224(%rdi), %xmm14
	movdqa	240(%rdi), %xmm15
	pxor	0(%rsi), %xmm0
	pxor	16(%rsi), %xmm1
	pxor	32(%rsi), %xmm2
	pxor	48(%rsi), %xmm3
	pxor	64(%rsi), %xmm4
	pxor	80(%rsi), %xmm5
	pxor	96(%rsi), %xmm6
	pxor	112(%rsi), %xmm7
	pxor	128(%rsi), %xmm8
	pxor	144(%rsi), %xmm9
	pxor	160(%rsi), %xmm10
	pxor	176(%rsi), %xmm11
	pxor	192(%rsi), %xmm12
	pxor	208(%rsi), %xmm13
	pxor	224(%rsi), %xmm14
	pxor	240(%rsi), %xmm15
	movdqa	%xmm0, 0(%rdi)
	movdqa	%xmm1, 16(%rdi)
	movdqa	%xmm2, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	movdqa	%xmm4, 64(%rdi)
	movdqa	%xmm5, 80(%rdi)
	movdqa	%xmm6, 96(%rdi)
	movdqa	%xmm7, 112(%rdi)
	movdqa	%xmm8, 128(%rdi)
	movdqa	%xmm9, 144(%rdi)
	movdqa	%xmm10, 160(%rdi)
	movdqa	%xmm11, 176(%rdi)
	movdqa	%xmm12, 192(%rdi)
	movdqa	%xmm13, 208(%rdi)
	movdqa	%xmm14, 224(%rdi)
	movdqa	%xmm15, 240(%rdi)
	movdqa	%xmm0, 0(%rdx)
	movdqa	%xmm1, 16(%rdx)
	movdqa	%xmm2, 32(%rdx)
	movdqa	%xmm3, 48(%rdx)
	movdqa	%xmm4, 64(%rdx)
	movdqa	%xmm5, 80(%rdx)
	movdqa	%xmm6, 96(%rdx)
	movdqa	%xmm7, 112(%rdx)
	movdqa	%xmm8, 128(%rdx)
	movdqa	%xmm9, 144(%rdx)
	movdqa	%xmm10, 160(%rdx)
	movdqa	%xmm11, 176(%rdx)
	movdqa	%xmm12, 192(%rdx)
	movdqa	%xmm13, 208(%rdx)
	movdqa	%xmm14, 224(%rdx)
	movdqa	%xmm15, 240(%rdx)
.xor_salsa_4way:
/* quarters A and B */
	movdqa	0(%rdx), %xmm0		/* A: load a */
	movdqa	80(%rdx), %xmm4		/* B: load a */
	paddd	192(%rdx), %xmm0	/* A: t = a + d */
	paddd	16(%rdx), %xmm4		/* B: t = a + d */
	movdqa	%xmm0, %xmm3		/* A: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* B: rotate t (0) */
	pslld	$7, %xmm0		/* A: rotate t (1) */
	psrld	$25, %xmm3		/* A: rotate t (2) */
	pslld	$7, %xmm4		/* B: rotate t (1) */
	psrld	$25, %xmm7		/* B: rotate t (2) */
	por	%xmm3, %xmm0		/* A: rotate t (3) */
	por	%xmm7, %xmm4		/* B: rotate t (3) */
	pxor	64(%rdx), %xmm0		/* A: b = b ^ t */
	pxor	144(%rdx), %xmm4	/* B: b = b ^ t */
	movdqa	%xmm0, %xmm1		/* A: copy b */
	movdqa	%xmm4, %xmm5		/* B: copy b */
	movdqa	%xmm1, 64(%rdx)		/* A: store b */
	movdqa	%xmm5, 144(%rdx)	/* B: store b */
	paddd	0(%rdx), %xmm0		/* A: t = b + a */
	paddd	80(%rdx), %xmm4		/* B: t = b + a */
	movdqa	%xmm0, %xmm3		/* A: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* B: rotate t (0) */
	pslld	$9, %xmm0		/* A: rotate t (1) */
	psrld	$23, %xmm3		/* A: rotate t (2) */
	pslld	$9, %xmm4		/* B: rotate t (1) */
	psrld	$23, %xmm7		/* B: rotate t (2) */
	por	%xmm3, %xmm0		/* A: rotate t (3) */
	por	%xmm7, %xmm4		/* B: rotate t (3) */
	pxor	128(%rdx), %xmm0	/* A: c = c ^ t */
	pxor	208(%rdx), %xmm4	/* B: c = c ^ t */
	movdqa	%xmm0, %xmm2		/* A: copy c */
	movdqa	%xmm4, %xmm6		/* B: copy c */
	movdqa	%xmm2, 128(%rdx)	/* A: store c */
	movdqa	%xmm6, 208(%rdx)	/* B: store c */
	paddd	%xmm1, %xmm0		/* A: t = c + b */
	paddd	%xmm5, %xmm4		/* B: t = c + b */
	movdqa	%xmm0, %xmm3		/* A: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* B: rotate t (0) */
	pslld	$13, %xmm0		/* A: rotate t (1) */
	psrld	$19, %xmm3		/* A: rotate t (2) */
	pslld	$13, %xmm4		/* B: rotate t (1) */
	psrld	$19, %xmm7		/* B: rotate t (2) */
	por	%xmm3, %xmm0		/* A: rotate t (3) */
	por	%xmm7, %xmm4		/* B: rotate t (3) */
	pxor	192(%rdx), %xmm0	/* A: d = d ^ t */
	pxor	16(%rdx), %xmm4		/* B: d = d ^ t */
	movdqa	%xmm0, 192(%rdx)	/* A: store d */
	movdqa	%xmm4, 16(%rdx)		/* B: store d */
	paddd	%xmm2, %xmm0		/* A: t = d + c */
	paddd	%xmm6, %xmm4		/* B: t = d + c */
	movdqa	%xmm0, %xmm3		/* A: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* B: rotate t (0) */
	pslld	$18, %xmm0		/* A: rotate t (1) */
	psrld	$14, %xmm3		/* A: rotate t (2) */
	pslld	$18, %xmm4		/* B: rotate t (1) */
	psrld	$14, %xmm7		/* B: rotate t (2) */
	por	%xmm3, %xmm0		/* A: rotate t (3) */
	por	%xmm7, %xmm4		/* B: rotate t (3) */
	pxor	0(%rdx), %xmm0		/* A: a = a ^ t */
	pxor	80(%rdx), %xmm4		/* B: a = a ^ t */
	movdqa	%xmm0, 0(%rdx)		/* A: store a */
	movdqa	%xmm4, 80(%rdx)		/* B: store a */
/* quarters C and D*/
	movdqa	160(%rdx), %xmm0	/* C: load a */
	movdqa	240(%rdx), %xmm4	/* D: load a */
	paddd	96(%rdx), %xmm0		/* C: t = a + d */
	paddd	176(%rdx), %xmm4	/* D: t = a + d */
	movdqa	%xmm0, %xmm3		/* C: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* D: rotate t (0) */
	pslld	$7, %xmm0		/* C: rotate t (1) */
	psrld	$25, %xmm3		/* C: rotate t (2) */
	pslld	$7, %xmm4		/* D: rotate t (1) */
	psrld	$25, %xmm7		/* D: rotate t (2) */
	por	%xmm3, %xmm0		/* C: rotate t (3) */
	por	%xmm7, %xmm4		/* D: rotate t (3) */
	pxor	224(%rdx), %xmm0	/* C: b = b ^ t */
	pxor	48(%rdx), %xmm4		/* D: b = b ^ t */
	movdqa	%xmm0, %xmm1		/* C: copy b */
	movdqa	%xmm4, %xmm5		/* D: copy b */
	movdqa	%xmm1, 224(%rdx)	/* C: store b */
	movdqa	%xmm5, 48(%rdx)		/* D: store b */
	paddd	160(%rdx), %xmm0	/* C: t = b + a */
	paddd	240(%rdx), %xmm4	/* D: t = b + a */
	movdqa	%xmm0, %xmm3		/* C: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* D: rotate t (0) */
	pslld	$9, %xmm0		/* C: rotate t (1) */
	psrld	$23, %xmm3		/* C: rotate t (2) */
	pslld	$9, %xmm4		/* D: rotate t (1) */
	psrld	$23, %xmm7		/* D: rotate t (2) */
	por	%xmm3, %xmm0		/* C: rotate t (3) */
	por	%xmm7, %xmm4		/* D: rotate t (3) */
	pxor	32(%rdx), %xmm0		/* C: c = c ^ t */
	pxor	112(%rdx), %xmm4	/* D: c = c ^ t */
	movdqa	%xmm0, %xmm2		/* C: copy c */
	movdqa	%xmm4, %xmm6		/* D: copy c */
	movdqa	%xmm2, 32(%rdx)		/* C: store c */
	movdqa	%xmm6, 112(%rdx)	/* D: store c */
	paddd	%xmm1, %xmm0		/* C: t = c + b */
	paddd	%xmm5, %xmm4		/* D: t = c + b */
	movdqa	%xmm0, %xmm3		/* C: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* D: rotate t (0) */
	pslld	$13, %xmm0		/* C: rotate t (1) */
	psrld	$19, %xmm3		/* C: rotate t (2) */
	pslld	$13, %xmm4		/* D: rotate t (1) */
	psrld	$19, %xmm7		/* D: rotate t (2) */
	por	%xmm3, %xmm0		/* C: rotate t (3) */
	por	%xmm7, %xmm4		/* D: rotate t (3) */
	pxor	96(%rdx), %xmm0		/* C: d = d ^ t */
	pxor	176(%rdx), %xmm4	/* D: d = d ^ t */
	movdqa	%xmm0, 96(%rdx)		/* C: store d */
	movdqa	%xmm4, 176(%rdx)	/* D: store d */
	paddd	%xmm2, %xmm0		/* C: t = d + c */
	paddd	%xmm6, %xmm4		/* D: t = d + c */
	movdqa	%xmm0, %xmm3		/* C: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* D: rotate t (0) */
	pslld	$18, %xmm0		/* C: rotate t (1) */
	psrld	$14, %xmm3		/* C: rotate t (2) */
	pslld	$18, %xmm4		/* D: rotate t (1) */
	psrld	$14, %xmm7		/* D: rotate t (2) */
	por	%xmm3, %xmm0		/* C: rotate t (3) */
	por	%xmm7, %xmm4		/* D: rotate t (3) */
	pxor	160(%rdx), %xmm0	/* C: a = a ^ t */
	pxor	240(%rdx), %xmm4	/* D: a = a ^ t */
	movdqa	%xmm0, 160(%rdx)	/* C: store a */
	movdqa	%xmm4, 240(%rdx)	/* D: store a */
/* quarters E and F */
	movdqa	0(%rdx), %xmm0		/* E: load a */
	movdqa	80(%rdx), %xmm4		/* F: load a */
	paddd	48(%rdx), %xmm0		/* E: t = a + d */
	paddd	64(%rdx), %xmm4		/* F: t = a + d */
	movdqa	%xmm0, %xmm3		/* E: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* F: rotate t (0) */
	pslld	$7, %xmm0		/* E: rotate t (1) */
	psrld	$25, %xmm3		/* E: rotate t (2) */
	pslld	$7, %xmm4		/* F: rotate t (1) */
	psrld	$25, %xmm7		/* F: rotate t (2) */
	por	%xmm3, %xmm0		/* E: rotate t (3) */
	por	%xmm7, %xmm4		/* F: rotate t (3) */
	pxor	16(%rdx), %xmm0		/* E: b = b ^ t */
	pxor	96(%rdx), %xmm4		/* F: b = b ^ t */
	movdqa	%xmm0, %xmm1		/* E: copy b */
	movdqa	%xmm4, %xmm5		/* F: copy b */
	movdqa	%xmm1, 16(%rdx)		/* E: store b */
	movdqa	%xmm5, 96(%rdx)		/* F: store b */
	paddd	0(%rdx), %xmm0		/* E: t = b + a */
	paddd	80(%rdx), %xmm4		/* F: t = b + a */
	movdqa	%xmm0, %xmm3		/* E: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* F: rotate t (0) */
	pslld	$9, %xmm0		/* E: rotate t (1) */
	psrld	$23, %xmm3		/* E: rotate t (2) */
	pslld	$9, %xmm4		/* F: rotate t (1) */
	psrld	$23, %xmm7		/* F: rotate t (2) */
	por	%xmm3, %xmm0		/* E: rotate t (3) */
	por	%xmm7, %xmm4		/* F: rotate t (3) */
	pxor	32(%rdx), %xmm0		/* E: c = c ^ t */
	pxor	112(%rdx), %xmm4	/* F: c = c ^ t */
	movdqa	%xmm0, %xmm2		/* E: copy c */
	movdqa	%xmm4, %xmm6		/* F: copy c */
	movdqa	%xmm2, 32(%rdx)		/* E: store c */
	movdqa	%xmm6, 112(%rdx)	/* F: store c */
	paddd	%xmm1, %xmm0		/* E: t = c + b */
	paddd	%xmm5, %xmm4		/* F: t = c + b */
	movdqa	%xmm0, %xmm3		/* E: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* F: rotate t (0) */
	pslld	$13, %xmm0		/* E: rotate t (1) */
	psrld	$19, %xmm3		/* E: rotate t (2) */
	pslld	$13, %xmm4		/* F: rotate t (1) */
	psrld	$19, %xmm7		/* F: rotate t (2) */
	por	%xmm3, %xmm0		/* E: rotate t (3) */
	por	%xmm7, %xmm4		/* F: rotate t (3) */
	pxor	48(%rdx), %xmm0		/* E: d = d ^ t */
	pxor	64(%rdx), %xmm4		/* F: d = d ^ t */
	movdqa	%xmm0, 48(%rdx)		/* E: store d */
	movdqa	%xmm4, 64(%rdx)		/* F: store d */
	paddd	%xmm2, %xmm0		/* E: t = d + c */
	paddd	%xmm6, %xmm4		/* F: t = d + c */
	movdqa	%xmm0, %xmm3		/* E: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* F: rotate t (0) */
	pslld	$18, %xmm0		/* E: rotate t (1) */
	psrld	$14, %xmm3		/* E: rotate t (2) */
	pslld	$18, %xmm4		/* F: rotate t (1) */
	psrld	$14, %xmm7		/* F: rotate t (2) */
	por	%xmm3, %xmm0		/* E: rotate t (3) */
	por	%xmm7, %xmm4		/* F: rotate t (3) */
	pxor	0(%rdx), %xmm0		/* E: a = a ^ t */
	pxor	80(%rdx), %xmm4		/* F: a = a ^ t */
	movdqa	%xmm0, 0(%rdx)		/* E: store a */
	movdqa	%xmm4, 80(%rdx)		/* F: store a */
/* quarters G and H */
	movdqa	160(%rdx), %xmm0	/* G: load a */
	movdqa	240(%rdx), %xmm4	/* H: load a */
	paddd	144(%rdx), %xmm0	/* G: t = a + d */
	paddd	224(%rdx), %xmm4	/* H: t = a + d */
	movdqa	%xmm0, %xmm3		/* G: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* H: rotate t (0) */
	pslld	$7, %xmm0		/* G: rotate t (1) */
	psrld	$25, %xmm3		/* G: rotate t (2) */
	pslld	$7, %xmm4		/* H: rotate t (1) */
	psrld	$25, %xmm7		/* H: rotate t (2) */
	por	%xmm3, %xmm0		/* G: rotate t (3) */
	por	%xmm7, %xmm4		/* H: rotate t (3) */
	pxor	176(%rdx), %xmm0	/* G: b = b ^ t */
	pxor	192(%rdx), %xmm4	/* H: b = b ^ t */
	movdqa	%xmm0, %xmm1		/* G: copy b */
	movdqa	%xmm4, %xmm5		/* H: copy b */
	movdqa	%xmm1, 176(%rdx)	/* G: store b */
	movdqa	%xmm5, 192(%rdx)	/* H: store b */
	paddd	160(%rdx), %xmm0	/* G: t = b + a */
	paddd	240(%rdx), %xmm4	/* H: t = b + a */
	movdqa	%xmm0, %xmm3		/* G: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* H: rotate t (0) */
	pslld	$9, %xmm0		/* G: rotate t (1) */
	psrld	$23, %xmm3		/* G: rotate t (2) */
	pslld	$9, %xmm4		/* H: rotate t (1) */
	psrld	$23, %xmm7		/* H: rotate t (2) */
	por	%xmm3, %xmm0		/* G: rotate t (3) */
	por	%xmm7, %xmm4		/* H: rotate t (3) */
	pxor	128(%rdx), %xmm0	/* G: c = c ^ t */
	pxor	208(%rdx), %xmm4	/* H: c = c ^ t */
	movdqa	%xmm0, %xmm2		/* G: copy c */
	movdqa	%xmm4, %xmm6		/* H: copy c */
	movdqa	%xmm2, 128(%rdx)	/* G: store c */
	movdqa	%xmm6, 208(%rdx)	/* H: store c */
	paddd	%xmm1, %xmm0		/* G: t = c + b */
	paddd	%xmm5, %xmm4		/* H: t = c + b */
	movdqa	%xmm0, %xmm3		/* G: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* H: rotate t (0) */
	pslld	$13, %xmm0		/* G: rotate t (1) */
	psrld	$19, %xmm3		/* G: rotate t (2) */
	pslld	$13, %xmm4		/* H: rotate t (1) */
	psrld	$19, %xmm7		/* H: rotate t (2) */
	por	%xmm3, %xmm0		/* G: rotate t (3) */
	por	%xmm7, %xmm4		/* H: rotate t (3) */
	pxor	144(%rdx), %xmm0	/* G: d = d ^ t */
	pxor	224(%rdx), %xmm4	/* H: d = d ^ t */
	movdqa	%xmm0, 144(%rdx)	/* G: store d */
	movdqa	%xmm4, 224(%rdx)	/* H: store d */
	paddd	%xmm2, %xmm0		/* G: t = d + c */
	paddd	%xmm6, %xmm4		/* H: t = d + c */
	movdqa	%xmm0, %xmm3		/* G: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* H: rotate t (0) */
	pslld	$18, %xmm0		/* G: rotate t (1) */
	psrld	$14, %xmm3		/* G: rotate t (2) */
	pslld	$18, %xmm4		/* H: rotate t (1) */
	psrld	$14, %xmm7		/* H: rotate t (2) */
	por	%xmm3, %xmm0		/* G: rotate t (3) */
	por	%xmm7, %xmm4		/* H: rotate t (3) */
	pxor	160(%rdx), %xmm0	/* G: a = a ^ t */
	pxor	240(%rdx), %xmm4	/* H: a = a ^ t */
	movdqa	%xmm0, 160(%rdx)	/* G: store a */
	movdqa	%xmm4, 240(%rdx)	/* H: store a */
	decl	%ecx
	jnz	.xor_salsa_4way

/* write back data */
	movdqa	0(%rdi), %xmm0
	movdqa	16(%rdi), %xmm1
	movdqa	32(%rdi), %xmm2
	movdqa	48(%rdi), %xmm3
	movdqa	64(%rdi), %xmm4
	movdqa	80(%rdi), %xmm5
	movdqa	96(%rdi), %xmm6
	movdqa	112(%rdi), %xmm7
	movdqa	128(%rdi), %xmm8
	movdqa	144(%rdi), %xmm9
	movdqa	160(%rdi), %xmm10
	movdqa	176(%rdi), %xmm11
	movdqa	192(%rdi), %xmm12
	movdqa	208(%rdi), %xmm13
	movdqa	224(%rdi), %xmm14
	movdqa	240(%rdi), %xmm15
	paddd	0(%rdx), %xmm0
	paddd	16(%rdx), %xmm1
	paddd	32(%rdx), %xmm2
	paddd	48(%rdx), %xmm3
	paddd	64(%rdx), %xmm4
	paddd	80(%rdx), %xmm5
	paddd	96(%rdx), %xmm6
	paddd	112(%rdx), %xmm7
	paddd	128(%rdx), %xmm8
	paddd	144(%rdx), %xmm9
	paddd	160(%rdx), %xmm10
	paddd	176(%rdx), %xmm11
	paddd	192(%rdx), %xmm12
	paddd	208(%rdx), %xmm13
	paddd	224(%rdx), %xmm14
	paddd	240(%rdx), %xmm15
	movdqa	%xmm0, 0(%rdi)
	movdqa	%xmm1, 16(%rdi)
	movdqa	%xmm2, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	movdqa	%xmm4, 64(%rdi)
	movdqa	%xmm5, 80(%rdi)
	movdqa	%xmm6, 96(%rdi)
	movdqa	%xmm7, 112(%rdi)
	movdqa	%xmm8, 128(%rdi)
	movdqa	%xmm9, 144(%rdi)
	movdqa	%xmm10, 160(%rdi)
	movdqa	%xmm11, 176(%rdi)
	movdqa	%xmm12, 192(%rdi)
	movdqa	%xmm13, 208(%rdi)
	movdqa	%xmm14, 224(%rdi)
	movdqa	%xmm15, 240(%rdi)
#if (WIN64)
	movq	%r10, %rdi
	movq	%r11, %rsi
#endif
	ret


/* neoscrypt_xor_chacha_4way(mem, xormem, workmem, double_rounds)
 * AMD64 (SSE2) ChaCha20 4-way with XOR */
.globl neoscrypt_xor_chacha_4way
.globl _neoscrypt_xor_chacha_4way
neoscrypt_xor_chacha_4way:
_neoscrypt_xor_chacha_4way:
#if (WIN64)
	movq	%rdi, %r10
	movq	%rsi, %r11
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	movq	%r8, %rdx
	movq	%r9, %rcx
#endif
/* XOR and copy to temporary memory */
	movdqa	0(%rdi), %xmm0
	movdqa	16(%rdi), %xmm1
	movdqa	32(%rdi), %xmm2
	movdqa	48(%rdi), %xmm3
	movdqa	64(%rdi), %xmm4
	movdqa	80(%rdi), %xmm5
	movdqa	96(%rdi), %xmm6
	movdqa	112(%rdi), %xmm7
	movdqa	128(%rdi), %xmm8
	movdqa	144(%rdi), %xmm9
	movdqa	160(%rdi), %xmm10
	movdqa	176(%rdi), %xmm11
	movdqa	192(%rdi), %xmm12
	movdqa	208(%rdi), %xmm13
	movdqa	224(%rdi), %xmm14
	movdqa	240(%rdi), %xmm15
	pxor	0(%rsi), %xmm0
	pxor	16(%rsi), %xmm1
	pxor	32(%rsi), %xmm2
	pxor	48(%rsi), %xmm3
	pxor	64(%rsi), %xmm4
	pxor	80(%rsi), %xmm5
	pxor	96(%rsi), %xmm6
	pxor	112(%rsi), %xmm7
	pxor	128(%rsi), %xmm8
	pxor	144(%rsi), %xmm9
	pxor	160(%rsi), %xmm10
	pxor	176(%rsi), %xmm11
	pxor	192(%rsi), %xmm12
	pxor	208(%rsi), %xmm13
	pxor	224(%rsi), %xmm14
	pxor	240(%rsi), %xmm15
	movdqa	%xmm0, 0(%rdi)
	movdqa	%xmm1, 16(%rdi)
	movdqa	%xmm2, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	movdqa	%xmm4, 64(%rdi)
	movdqa	%xmm5, 80(%rdi)
	movdqa	%xmm6, 96(%rdi)
	movdqa	%xmm7, 112(%rdi)
	movdqa	%xmm8, 128(%rdi)
	movdqa	%xmm9, 144(%rdi)
	movdqa	%xmm10, 160(%rdi)
	movdqa	%xmm11, 176(%rdi)
	movdqa	%xmm12, 192(%rdi)
	movdqa	%xmm13, 208(%rdi)
	movdqa	%xmm14, 224(%rdi)
	movdqa	%xmm15, 240(%rdi)
	movdqa	%xmm0, 0(%rdx)
	movdqa	%xmm1, 16(%rdx)
	movdqa	%xmm2, 32(%rdx)
	movdqa	%xmm3, 48(%rdx)
	movdqa	%xmm4, 64(%rdx)
	movdqa	%xmm5, 80(%rdx)
	movdqa	%xmm6, 96(%rdx)
	movdqa	%xmm7, 112(%rdx)
	movdqa	%xmm8, 128(%rdx)
	movdqa	%xmm9, 144(%rdx)
	movdqa	%xmm10, 160(%rdx)
	movdqa	%xmm11, 176(%rdx)
	movdqa	%xmm12, 192(%rdx)
	movdqa	%xmm13, 208(%rdx)
	movdqa	%xmm14, 224(%rdx)
	movdqa	%xmm15, 240(%rdx)
.xor_chacha_4way:
/* quarters A and B */
	movdqa	0(%rdx), %xmm0		/* A: load a */
	movdqa	16(%rdx), %xmm4		/* B: load a */
	movdqa	64(%rdx), %xmm1		/* A: load b */
	movdqa	80(%rdx), %xmm5		/* B: load b */
	paddd	%xmm1, %xmm0		/* A: a = a + b */
	paddd	%xmm5, %xmm4		/* B: a = a + b */
	movdqa	192(%rdx), %xmm3	/* A: load d */
	movdqa	208(%rdx), %xmm7	/* B: load d */
	pxor	%xmm0, %xmm3		/* A: d = d ^ a */
	pxor	%xmm4, %xmm7		/* B: d = d ^ a */
	movdqa	128(%rdx), %xmm2	/* A: load c */
	movdqa	144(%rdx), %xmm6	/* B: load c */
	movdqa	%xmm3, %xmm14		/* A: rotate d (0) */
	movdqa	%xmm7, %xmm15		/* B: rotate d (0) */
	pslld	$16, %xmm3		/* A: rotate d (1) */
	psrld	$16, %xmm14		/* A: rotate d (2) */
	pslld	$16, %xmm7		/* B: rotate d (1) */
	psrld	$16, %xmm15		/* B: rotate d (2) */
	por	%xmm14, %xmm3		/* A: rotate d (3) */
	por	%xmm15, %xmm7		/* B: rotate d (3) */
	paddd	%xmm3, %xmm2		/* A: c = c + d */
	paddd	%xmm7, %xmm6		/* B: c = c + d */
	pxor	%xmm2, %xmm1		/* A: b = b ^ c */
	pxor	%xmm6, %xmm5		/* B: b = b ^ c */
	movdqa	%xmm1, %xmm14		/* A: rotate b (0) */
	movdqa	%xmm5, %xmm15		/* B: rotate b (0) */
	pslld	$12, %xmm1		/* A: rotate b (1) */
	psrld	$20, %xmm14		/* A: rotate b (2) */
	pslld	$12, %xmm5		/* B: rotate b (1) */
	psrld	$20, %xmm15		/* B: rotate b (2) */
	por	%xmm14, %xmm1		/* A: rotate b (3) */
	por	%xmm15, %xmm5		/* B: rotate b (3) */
	paddd	%xmm1, %xmm0		/* A: a = a + b */
	paddd	%xmm5, %xmm4		/* B: a = a + b */
	movdqa	%xmm0, 0(%rdx)		/* A: store a */
	movdqa	%xmm4, 16(%rdx)		/* B: store a */
	pxor	%xmm0, %xmm3		/* A: d = d ^ a */
	pxor	%xmm4, %xmm7		/* B: d = d ^ a */
	movdqa	%xmm3, %xmm14		/* A: rotate d (0) */
	movdqa	%xmm7, %xmm15		/* B: rotate d (0) */
	pslld	$8, %xmm3		/* A: rotate d (1) */
	psrld	$24, %xmm14		/* A: rotate d (2) */
	pslld	$8, %xmm7		/* B: rotate d (1) */
	psrld	$24, %xmm15		/* B: rotate d (2) */
	por	%xmm14, %xmm3		/* A: rotate d (3) */
	por	%xmm15, %xmm7		/* B: rotate d (3) */
	movdqa	%xmm3, 192(%rdx)	/* A: store d */
	movdqa	%xmm7, 208(%rdx)	/* B: store d */
	paddd	%xmm3, %xmm2		/* A: c = c + d */
	paddd	%xmm7, %xmm6		/* B: c = c + d */
	movdqa	%xmm2, 128(%rdx)	/* A: store c */
	movdqa	%xmm6, 144(%rdx)	/* B: store c */
	pxor	%xmm2, %xmm1		/* A: b = b ^ c */
	pxor	%xmm6, %xmm5		/* B: b = b ^ c */
	movdqa	%xmm1, %xmm14		/* A: rotate b (0) */
	movdqa	%xmm5, %xmm15		/* B: rotate b (0) */
	pslld	$7, %xmm1		/* A: rotate b (1) */
	psrld	$25, %xmm14		/* A: rotate b (2) */
	pslld	$7, %xmm5		/* B: rotate b (1) */
	psrld	$25, %xmm15		/* B: rotate b (2) */
	por	%xmm14, %xmm1		/* A: rotate b (3) */
	por	%xmm15, %xmm5		/* B: rotate b (3) */
	movdqa	%xmm1, 64(%rdx)		/* A: store b */
	movdqa	%xmm5, 80(%rdx)		/* B: store b */
/* quarters C and D */
	movdqa	32(%rdx), %xmm0		/* C: load a */
	movdqa	48(%rdx), %xmm4		/* D: load a */
	movdqa	96(%rdx), %xmm1		/* C: load b */
	movdqa	112(%rdx), %xmm5	/* D: load b */
	paddd	%xmm1, %xmm0		/* C: a = a + b */
	paddd	%xmm5, %xmm4		/* D: a = a + b */
	movdqa	224(%rdx), %xmm3	/* C: load d */
	movdqa	240(%rdx), %xmm7	/* D: load d */
	pxor	%xmm0, %xmm3		/* C: d = d ^ a */
	pxor	%xmm4, %xmm7		/* D: d = d ^ a */
	movdqa	160(%rdx), %xmm2	/* C: load c */
	movdqa	176(%rdx), %xmm6	/* D: load c */
	movdqa	%xmm3, %xmm14		/* C: rotate d (0) */
	movdqa	%xmm7, %xmm15		/* D: rotate d (0) */
	pslld	$16, %xmm3		/* C: rotate d (1) */
	psrld	$16, %xmm14		/* C: rotate d (2) */
	pslld	$16, %xmm7		/* D: rotate d (1) */
	psrld	$16, %xmm15		/* D: rotate d (2) */
	por	%xmm14, %xmm3		/* C: rotate d (3) */
	por	%xmm15, %xmm7		/* D: rotate d (3) */
	paddd	%xmm3, %xmm2		/* C: c = c + d */
	paddd	%xmm7, %xmm6		/* D: c = c + d */
	pxor	%xmm2, %xmm1		/* C: b = b ^ c */
	pxor	%xmm6, %xmm5		/* D: b = b ^ c */
	movdqa	%xmm1, %xmm14		/* C: rotate b (0) */
	movdqa	%xmm5, %xmm15		/* D: rotate b (0) */
	pslld	$12, %xmm1		/* C: rotate b (1) */
	psrld	$20, %xmm14		/* C: rotate b (2) */
	pslld	$12, %xmm5		/* D: rotate b (1) */
	psrld	$20, %xmm15		/* D: rotate b (2) */
	por	%xmm14, %xmm1		/* C: rotate b (3) */
	por	%xmm15, %xmm5		/* D: rotate b (3) */
	paddd	%xmm1, %xmm0		/* C: a = a + b */
	paddd	%xmm5, %xmm4		/* D: a = a + b */
	movdqa	%xmm0, 32(%rdx)		/* C: store a */
	movdqa	%xmm4, 48(%rdx)		/* D: store a */
	pxor	%xmm0, %xmm3		/* C: d = d ^ a */
	pxor	%xmm4, %xmm7		/* D: d = d ^ a */
	movdqa	%xmm3, %xmm14		/* C: rotate d (0) */
	movdqa	%xmm7, %xmm15		/* D: rotate d (0) */
	pslld	$8, %xmm3		/* C: rotate d (1) */
	psrld	$24, %xmm14		/* C: rotate d (2) */
	pslld	$8, %xmm7		/* D: rotate d (1) */
	psrld	$24, %xmm15		/* D: rotate d (2) */
	por	%xmm14, %xmm3		/* C: rotate d (3) */
	por	%xmm15, %xmm7		/* D: rotate d (3) */
	movdqa	%xmm3, 224(%rdx)	/* C: store d */
	movdqa	%xmm7, 240(%rdx)	/* D: store d */
	paddd	%xmm3, %xmm2		/* C: c = c + d */
	paddd	%xmm7, %xmm6		/* D: c = c + d */
	movdqa	%xmm2, 160(%rdx)	/* C: store c */
	movdqa	%xmm6, 176(%rdx)	/* D: store c */
	pxor	%xmm2, %xmm1		/* C: b = b ^ c */
	pxor	%xmm6, %xmm5		/* D: b = b ^ c */
	movdqa	%xmm1, %xmm14		/* C: rotate b (0) */
	movdqa	%xmm5, %xmm15		/* D: rotate b (0) */
	pslld	$7, %xmm1		/* C: rotate b (1) */
	psrld	$25, %xmm14		/* C: rotate b (2) */
	pslld	$7, %xmm5		/* D: rotate b (1) */
	psrld	$25, %xmm15		/* D: rotate b (2) */
	por	%xmm14, %xmm1		/* C: rotate b (3) */
	por	%xmm15, %xmm5		/* D: rotate b (3) */
	movdqa	%xmm1, 96(%rdx)		/* C: store b */
	movdqa	%xmm5, 112(%rdx)	/* D: store b */
/* quarters E and F */
	movdqa	0(%rdx), %xmm0		/* E: load a */
	movdqa	16(%rdx), %xmm4		/* F: load a */
	movdqa	80(%rdx), %xmm1		/* E: load b */
	movdqa	96(%rdx), %xmm5		/* F: load b */
	paddd	%xmm1, %xmm0		/* E: a = a + b */
	paddd	%xmm5, %xmm4		/* F: a = a + b */
	movdqa	240(%rdx), %xmm3	/* E: load d */
	movdqa	192(%rdx), %xmm7	/* F: load d */
	pxor	%xmm0, %xmm3		/* E: d = d ^ a */
	pxor	%xmm4, %xmm7		/* F: d = d ^ a */
	movdqa	160(%rdx), %xmm2	/* E: load c */
	movdqa	176(%rdx), %xmm6	/* F: load c */
	movdqa	%xmm3, %xmm14		/* E: rotate d (0) */
	movdqa	%xmm7, %xmm15		/* F: rotate d (0) */
	pslld	$16, %xmm3		/* E: rotate d (1) */
	psrld	$16, %xmm14		/* E: rotate d (2) */
	pslld	$16, %xmm7		/* F: rotate d (1) */
	psrld	$16, %xmm15		/* F: rotate d (2) */
	por	%xmm14, %xmm3		/* E: rotate d (3) */
	por	%xmm15, %xmm7		/* F: rotate d (3) */
	paddd	%xmm3, %xmm2		/* E: c = c + d */
	paddd	%xmm7, %xmm6		/* F: c = c + d */
	pxor	%xmm2, %xmm1		/* E: b = b ^ c */
	pxor	%xmm6, %xmm5		/* F: b = b ^ c */
	movdqa	%xmm1, %xmm14		/* E: rotate b (0) */
	movdqa	%xmm5, %xmm15		/* F: rotate b (0) */
	pslld	$12, %xmm1		/* E: rotate b (1) */
	psrld	$20, %xmm14		/* E: rotate b (2) */
	pslld	$12, %xmm5		/* F: rotate b (1) */
	psrld	$20, %xmm15		/* F: rotate b (2) */
	por	%xmm14, %xmm1		/* E: rotate b (3) */
	por	%xmm15, %xmm5		/* F: rotate b (3) */
	paddd	%xmm1, %xmm0		/* E: a = a + b */
	paddd	%xmm5, %xmm4		/* F: a = a + b */
	movdqa	%xmm0, 0(%rdx)		/* E: store a */
	movdqa	%xmm4, 16(%rdx)		/* F: store a */
	pxor	%xmm0, %xmm3		/* E: d = d ^ a */
	pxor	%xmm4, %xmm7		/* F: d = d ^ a */
	movdqa	%xmm3, %xmm14		/* E: rotate d (0) */
	movdqa	%xmm7, %xmm15		/* F: rotate d (0) */
	pslld	$8, %xmm3		/* E: rotate d (1) */
	psrld	$24, %xmm14		/* E: rotate d (2) */
	pslld	$8, %xmm7		/* F: rotate d (1) */
	psrld	$24, %xmm15		/* F: rotate d (2) */
	por	%xmm14, %xmm3		/* E: rotate d (3) */
	por	%xmm15, %xmm7		/* F: rotate d (3) */
	movdqa	%xmm3, 240(%rdx)	/* E: store d */
	movdqa	%xmm7, 192(%rdx)	/* F: store d */
	paddd	%xmm3, %xmm2		/* E: c = c + d */
	paddd	%xmm7, %xmm6		/* F: c = c + d */
	movdqa	%xmm2, 160(%rdx)	/* E: store c */
	movdqa	%xmm6, 176(%rdx)	/* F: store c */
	pxor	%xmm2, %xmm1		/* E: b = b ^ c */
	pxor	%xmm6, %xmm5		/* F: b = b ^ c */
	movdqa	%xmm1, %xmm14		/* E: rotate b (0) */
	movdqa	%xmm5, %xmm15		/* F: rotate b (0) */
	pslld	$7, %xmm1		/* E: rotate b (1) */
	psrld	$25, %xmm14		/* E: rotate b (2) */
	pslld	$7, %xmm5		/* F: rotate b (1) */
	psrld	$25, %xmm15		/* F: rotate b (2) */
	por	%xmm14, %xmm1		/* E: rotate b (3) */
	por	%xmm15, %xmm5		/* F: rotate b (3) */
	movdqa	%xmm1, 80(%rdx)		/* E: store b */
	movdqa	%xmm5, 96(%rdx)		/* F: store b */
/* quarter G and H */
	movdqa	32(%rdx), %xmm0		/* G: load a */
	movdqa	48(%rdx), %xmm4		/* H: load a */
	movdqa	64(%rdx), %xmm5		/* H: load b */
	movdqa	112(%rdx), %xmm1	/* G: load b */
	paddd	%xmm1, %xmm0		/* G: a = a + b */
	paddd	%xmm5, %xmm4		/* H: a = a + b */
	movdqa	208(%rdx), %xmm3	/* G: load d */
	movdqa	224(%rdx), %xmm7	/* H: load d */
	pxor	%xmm0, %xmm3		/* G: d = d ^ a */
	pxor	%xmm4, %xmm7		/* H: d = d ^ a */
	movdqa	128(%rdx), %xmm2	/* G: load c */
	movdqa	144(%rdx), %xmm6	/* H: load c */
	movdqa	%xmm3, %xmm14		/* G: rotate d (0) */
	movdqa	%xmm7, %xmm15		/* H: rotate d (0) */
	pslld	$16, %xmm3		/* G: rotate d (1) */
	psrld	$16, %xmm14		/* G: rotate d (2) */
	pslld	$16, %xmm7		/* H: rotate d (1) */
	psrld	$16, %xmm15		/* H: rotate d (2) */
	por	%xmm14, %xmm3		/* G: rotate d (3) */
	por	%xmm15, %xmm7		/* H: rotate d (3) */
	paddd	%xmm3, %xmm2		/* G: c = c + d */
	paddd	%xmm7, %xmm6		/* H: c = c + d */
	pxor	%xmm2, %xmm1		/* G: b = b ^ c */
	pxor	%xmm6, %xmm5		/* H: b = b ^ c */
	movdqa	%xmm1, %xmm14		/* G: rotate b (0) */
	movdqa	%xmm5, %xmm15		/* H: rotate b (0) */
	pslld	$12, %xmm1		/* G: rotate b (1) */
	psrld	$20, %xmm14		/* G: rotate b (2) */
	pslld	$12, %xmm5		/* H: rotate b (1) */
	psrld	$20, %xmm15		/* H: rotate b (2) */
	por	%xmm14, %xmm1		/* G: rotate b (3) */
	por	%xmm15, %xmm5		/* H: rotate b (3) */
	paddd	%xmm1, %xmm0		/* G: a = a + b */
	paddd	%xmm5, %xmm4		/* H: a = a + b */
	movdqa	%xmm0, 32(%rdx)		/* G: store a */
	movdqa	%xmm4, 48(%rdx)		/* H: store a */
	pxor	%xmm0, %xmm3		/* G: d = d ^ a */
	pxor	%xmm4, %xmm7		/* H: d = d ^ a */
	movdqa	%xmm3, %xmm14		/* G: rotate d (0) */
	movdqa	%xmm7, %xmm15		/* H: rotate d (0) */
	pslld	$8, %xmm3		/* G: rotate d (1) */
	psrld	$24, %xmm14		/* G: rotate d (2) */
	pslld	$8, %xmm7		/* H: rotate d (1) */
	psrld	$24, %xmm15		/* H: rotate d (2) */
	por	%xmm14, %xmm3		/* G: rotate d (3) */
	por	%xmm15, %xmm7		/* H: rotate d (3) */
	movdqa	%xmm3, 208(%rdx)	/* G: store d */
	movdqa	%xmm7, 224(%rdx)	/* H: store d */
	paddd	%xmm3, %xmm2		/* G: c = c + d */
	paddd	%xmm7, %xmm6		/* H: c = c + d */
	movdqa	%xmm2, 128(%rdx)	/* G: store c */
	movdqa	%xmm6, 144(%rdx)	/* H: store c */
	pxor	%xmm2, %xmm1		/* G: b = b ^ c */
	pxor	%xmm6, %xmm5		/* H: b = b ^ c */
	movdqa	%xmm1, %xmm14		/* G: rotate b (0) */
	movdqa	%xmm5, %xmm15		/* H: rotate b (0) */
	pslld	$7, %xmm1		/* G: rotate b (1) */
	psrld	$25, %xmm14		/* G: rotate b (2) */
	pslld	$7, %xmm5		/* H: rotate b (1) */
	psrld	$25, %xmm15		/* H: rotate b (2) */
	por	%xmm14, %xmm1		/* G: rotate b (3) */
	por	%xmm15, %xmm5		/* H: rotate b (3) */
	movdqa	%xmm5, 64(%rdx)		/* H: store b */
	movdqa	%xmm1, 112(%rdx)	/* G: store b */
	decl	%ecx
	jnz	.xor_chacha_4way

/* write back data */
	movdqa	0(%rdi), %xmm0
	movdqa	16(%rdi), %xmm1
	movdqa	32(%rdi), %xmm2
	movdqa	48(%rdi), %xmm3
	movdqa	64(%rdi), %xmm4
	movdqa	80(%rdi), %xmm5
	movdqa	96(%rdi), %xmm6
	movdqa	112(%rdi), %xmm7
	movdqa	128(%rdi), %xmm8
	movdqa	144(%rdi), %xmm9
	movdqa	160(%rdi), %xmm10
	movdqa	176(%rdi), %xmm11
	movdqa	192(%rdi), %xmm12
	movdqa	208(%rdi), %xmm13
	movdqa	224(%rdi), %xmm14
	movdqa	240(%rdi), %xmm15
	paddd	0(%rdx), %xmm0
	paddd	16(%rdx), %xmm1
	paddd	32(%rdx), %xmm2
	paddd	48(%rdx), %xmm3
	paddd	64(%rdx), %xmm4
	paddd	80(%rdx), %xmm5
	paddd	96(%rdx), %xmm6
	paddd	112(%rdx), %xmm7
	paddd	128(%rdx), %xmm8
	paddd	144(%rdx), %xmm9
	paddd	160(%rdx), %xmm10
	paddd	176(%rdx), %xmm11
	paddd	192(%rdx), %xmm12
	paddd	208(%rdx), %xmm13
	paddd	224(%rdx), %xmm14
	paddd	240(%rdx), %xmm15
	movdqa	%xmm0, 0(%rdi)
	movdqa	%xmm1, 16(%rdi)
	movdqa	%xmm2, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	movdqa	%xmm4, 64(%rdi)
	movdqa	%xmm5, 80(%rdi)
	movdqa	%xmm6, 96(%rdi)
	movdqa	%xmm7, 112(%rdi)
	movdqa	%xmm8, 128(%rdi)
	movdqa	%xmm9, 144(%rdi)
	movdqa	%xmm10, 160(%rdi)
	movdqa	%xmm11, 176(%rdi)
	movdqa	%xmm12, 192(%rdi)
	movdqa	%xmm13, 208(%rdi)
	movdqa	%xmm14, 224(%rdi)
	movdqa	%xmm15, 240(%rdi)
#if (WIN64)
	movq	%r10, %rdi
	movq	%r11, %rsi
#endif
	ret

#endif /* (MINER_4WAY) */

#endif /* (ASM) && (__x86_64__) */


#if (ASM) && (__i386__)

/* neoscrypt_copy(dst, src, len)
 * i386 memcpy() */
.globl neoscrypt_copy
.globl _neoscrypt_copy
neoscrypt_copy:
_neoscrypt_copy:
	pushl	%ebx
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	movl	20(%esp), %edi
	movl	24(%esp), %esi
	movl	28(%esp), %ecx
	shrl	$4, %ecx
	xorl	%eax, %eax
	cmpl	%eax, %ecx
	jz	.copy_tail
.copy_16b:
	movl	0(%esi), %eax
	movl	4(%esi), %edx
	movl	8(%esi), %ebx
	movl	12(%esi), %ebp
	movl	%eax, 0(%edi)
	movl	%edx, 4(%edi)
	movl	%ebx, 8(%edi)
	movl	%ebp, 12(%edi)
	addl	$16, %esi
	addl	$16, %edi
	decl	%ecx
	jnz	.copy_16b

.copy_tail:
	xorl	%eax, %eax
	movl	28(%esp), %ecx
	andl	$0xF, %ecx
	cmpl	%eax, %ecx
	jz	.copy_finish
	movb	%cl, %ch
	andb	$0x3, %cl
	shrb	$2, %ch
	cmpb	%ah, %ch
	jz	.copy_1b
.copy_4b:
	movl	0(%esi), %edx
	movl	%edx, 0(%edi)
	addl	$4, %esi
	addl	$4, %edi
	decb	%ch
	jnz	.copy_4b

	cmpb	%al, %cl
	jz	.copy_finish
.copy_1b:
	movb	0(%esi), %dl
	movb	%dl, 0(%edi)
	incl	%esi
	incl	%edi
	decb	%cl
	jnz	.copy_1b

.copy_finish:
	popl	%esi
	popl	%edi
	popl	%ebp
	popl	%ebx
	ret


/* neoscrypt_erase(dst, len)
 * i386 memory eraser */
.globl neoscrypt_erase
.globl _neoscrypt_erase
neoscrypt_erase:
_neoscrypt_erase:
	movl	4(%esp), %edx
	movl	8(%esp), %ecx
	shrl	$4, %ecx
	xorl	%eax, %eax
	cmpl	%eax, %ecx
	jz	.erase_tail
.erase_16b:
	movl	%eax, 0(%edx)
	movl	%eax, 4(%edx)
	movl	%eax, 8(%edx)
	movl	%eax, 12(%edx)
	addl	$16, %edx
	decl	%ecx
	jnz	.erase_16b

.erase_tail:
	movl	8(%esp), %ecx
	andl	$0xF, %ecx
	cmpl	%eax, %ecx
	jz	.erase_finish
	movb	%cl, %ch
	andb	$0x3, %cl
	shrb	$2, %ch
	cmpb	%ah, %ch
	jz	.erase_1b
.erase_4b:
	movl	%eax, 0(%edx)
	addl	$4, %edx
	decb	%ch
	jnz	.erase_4b

	cmpb	%al, %cl
	jz	.erase_finish
.erase_1b:
	movb	%al, 0(%edx)
	incl	%edx
	decb	%cl
	jnz	.erase_1b

.erase_finish:
	ret


/* neoscrypt_xor(dst, src, len)
 * i386 XOR engine */
.globl neoscrypt_xor
.globl _neoscrypt_xor
neoscrypt_xor:
_neoscrypt_xor:
	pushl	%ebx
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	movl	20(%esp), %edi
	movl	24(%esp), %esi
	movl	28(%esp), %ecx
	shrl	$4, %ecx
	xorl	%eax, %eax
	cmpl	%eax, %ecx
	jz	.xor_tail
.xor_16b:
	movl	0(%edi), %eax
	movl	4(%edi), %edx
	movl	8(%edi), %ebx
	movl	12(%edi), %ebp
	xorl	0(%esi), %eax
	xorl	4(%esi), %edx
	xorl	8(%esi), %ebx
	xorl	12(%esi), %ebp
	movl	%eax, 0(%edi)
	movl	%edx, 4(%edi)
	movl	%ebx, 8(%edi)
	movl	%ebp, 12(%edi)
	addl	$16, %esi
	addl	$16, %edi
	decl	%ecx
	jnz	.xor_16b

.xor_tail:
	xorl	%eax, %eax
	movl	28(%esp), %ecx
	andl	$0xF, %ecx
	cmpl	%eax, %ecx
	jz	.xor_finish
	movb	%cl, %ch
	andb	$0x3, %cl
	shrb	$2, %ch
	cmpb	%ah, %ch
	jz	.xor_1b
.xor_4b:
	movl	0(%edi), %edx
	xorl	0(%esi), %edx
	movl	%edx, 0(%edi)
	addl	$4, %esi
	addl	$4, %edi
	decb	%ch
	jnz	.xor_4b

	cmpb	%al, %cl
	jz	.xor_finish
.xor_1b:
	movb	0(%edi), %dl
	xorb	0(%esi), %dl
	movb	%dl, 0(%edi)
	incl	%esi
	incl	%edi
	decb	%cl
	jnz	.xor_1b

.xor_finish:
	popl	%esi
	popl	%edi
	popl	%ebp
	popl	%ebx
	ret


/* neoscrypt_xor_salsa(mem, xormem, workmem, double_rounds)
 * i386 (INT) Salsa20 with XOR (MMX support required) */
neoscrypt_xor_salsa:
	pushl	%ebx
	pushl	%ebp
	pushl	%esi
	pushl	%edi
/* XOR and copy to temporary memory */
	movl	20(%esp), %ebx
	movl	24(%esp), %ecx
	movl	28(%esp), %ebp
	movq	0(%ebx), %mm0
	movq	8(%ebx), %mm1
	movq	16(%ebx), %mm2
	movq	24(%ebx), %mm3
	movq	32(%ebx), %mm4
	movq	40(%ebx), %mm5
	movq	48(%ebx), %mm6
	movq	56(%ebx), %mm7
	pxor	0(%ecx), %mm0
	pxor	8(%ecx), %mm1
	pxor	16(%ecx), %mm2
	pxor	24(%ecx), %mm3
	pxor	32(%ecx), %mm4
	pxor	40(%ecx), %mm5
	pxor	48(%ecx), %mm6
	pxor	56(%ecx), %mm7
	movq	%mm0, 0(%ebx)
	movq	%mm1, 8(%ebx)
	movq	%mm2, 16(%ebx)
	movq	%mm3, 24(%ebx)
	movq	%mm4, 32(%ebx)
	movq	%mm5, 40(%ebx)
	movq	%mm6, 48(%ebx)
	movq	%mm7, 56(%ebx)
	movq	%mm0, 0(%ebp)
	movq	%mm1, 8(%ebp)
	movq	%mm2, 16(%ebp)
	movq	%mm3, 24(%ebp)
	movq	%mm4, 32(%ebp)
	movq	%mm5, 40(%ebp)
	movq	%mm6, 48(%ebp)
	movq	%mm7, 56(%ebp)
/* number of double rounds */
	movl	32(%esp), %eax
	movl	%eax, -4(%esp)
.xor_salsa:
/* quarters A and B, initial C and D */
	movl	0(%ebp), %eax	/* A: load a */
	movl	20(%ebp), %ebx	/* B: load a */
	addl	48(%ebp), %eax	/* A: t = a + d */
	addl	4(%ebp), %ebx	/* B: t = a + d */
	roll	$7, %eax	/* A: rotate t */
	roll	$7, %ebx	/* B: rotate t */
	xorl	16(%ebp), %eax	/* A: b = b ^ t */
	xorl	36(%ebp), %ebx	/* B: b = b ^ t */
	movl	%eax, %esi	/* A: copy b */
	movl	%ebx, %edi	/* B: copy b */
	movl	%esi, 16(%ebp)	/* A: store b */
	movl	%edi, 36(%ebp)	/* B: store b */
	addl	0(%ebp), %eax	/* A: t = b + a */
	addl	20(%ebp), %ebx	/* B: t = b + a */
	roll	$9, %eax	/* A: rotate t */
	roll	$9, %ebx	/* B: rotate t */
	xorl	32(%ebp), %eax	/* A: c = c ^ t */
	xorl	52(%ebp), %ebx	/* B: c = c ^ t */
	movl	%eax, %ecx	/* A: copy c */
	movl	%ebx, %edx	/* B: copy c */
	movl	%ecx, 32(%ebp)	/* A: store c */
	movl	%edx, 52(%ebp)	/* B: store c */
	addl	%esi, %eax	/* A: t = c + b */
	addl	%edi, %ebx	/* B: t = c + b */
	roll	$13, %eax	/* A: rotate t */
	roll	$13, %ebx	/* B: rotate t */
	xorl	48(%ebp), %eax	/* A: d = d ^ t */
	xorl	4(%ebp), %ebx	/* B: d = d ^ t */
	movl	%eax, 48(%ebp)	/* A: store d */
	movl	%ebx, 4(%ebp)	/* B: store d */
	addl	%eax, %ecx	/* A: t = d + c */
	movl	40(%ebp), %eax	/* C: load a */
	addl	%ebx, %edx	/* B: t = d + c */
	movl	60(%ebp), %ebx	/* D: load a */
	roll	$18, %ecx	/* A: rotate t */
	addl	24(%ebp), %eax	/* C: t = a + d */
	roll	$18, %edx	/* B: rotate t */
	addl	44(%ebp), %ebx	/* D: t = a + d */
	xorl	0(%ebp), %ecx	/* A: a = a ^ t */
	roll	$7, %eax	/* C: rotate t */
	xorl	20(%ebp), %edx	/* B: a = a ^ t */
	roll	$7, %ebx	/* D: rotate t */
	movl	%ecx, 0(%ebp)	/* A: store a */
	movl	%edx, 20(%ebp)	/* B: store a */
/* quarters C and D, initial E and F */
	xorl	56(%ebp), %eax	/* C: b = b ^ t */
	xorl	12(%ebp), %ebx	/* D: b = b ^ t */
	movl	%eax, %esi	/* C: copy b */
	movl	%ebx, %edi	/* D: copy b */
	movl	%esi, 56(%ebp)	/* C: store b */
	movl	%edi, 12(%ebp)	/* D: store b */
	addl	40(%ebp), %eax	/* C: t = b + a */
	addl	60(%ebp), %ebx	/* D: t = b + a */
	roll	$9, %eax	/* C: rotate t */
	roll	$9, %ebx	/* D: rotate t */
	xorl	8(%ebp), %eax	/* C: c = c ^ t */
	xorl	28(%ebp), %ebx	/* D: c = c ^ t */
	movl	%eax, %ecx	/* C: copy c */
	movl	%ebx, %edx	/* D: copy c */
	movl	%ecx, 8(%ebp)	/* C: store c */
	movl	%edx, 28(%ebp)	/* D: store c */
	addl	%esi, %eax	/* C: t = c + b */
	addl	%edi, %ebx	/* D: t = c + b */
	roll	$13, %eax	/* C: rotate t */
	roll	$13, %ebx	/* D: rotate t */
	xorl	24(%ebp), %eax	/* C: d = d ^ t */
	xorl	44(%ebp), %ebx	/* D: d = d ^ t */
	movl	%eax, 24(%ebp)	/* C: store d */
	movl	%ebx, 44(%ebp)	/* D: store d */
	addl	%eax, %ecx	/* C: t = d + c */
	movl	0(%ebp), %eax	/* E: load a */
	addl	%ebx, %edx	/* D: t = d + c */
	movl	20(%ebp), %ebx	/* F: load a */
	roll	$18, %ecx	/* C: rotate t */
	addl	12(%ebp), %eax	/* E: t = a + d */
	roll	$18, %edx	/* D: rotate t */
	addl	16(%ebp), %ebx	/* F: t = a + d */
	xorl	40(%ebp), %ecx	/* C: a = a ^ t */
	roll	$7, %eax	/* E: rotate t */
	xorl	60(%ebp), %edx	/* D: a = a ^ t */
	roll	$7, %ebx	/* F: rotate t */
	movl	%ecx, 40(%ebp)	/* C: store a */
	movl	%edx, 60(%ebp)	/* D: store a */
/* quarters E and F, initial G and H */
	xorl	4(%ebp), %eax	/* E: b = b ^ t */
	xorl	24(%ebp), %ebx	/* F: b = b ^ t */
	movl	%eax, %esi	/* E: copy b */
	movl	%ebx, %edi	/* F: copy b */
	movl	%esi, 4(%ebp)	/* E: store b */
	movl	%edi, 24(%ebp)	/* F: store b */
	addl	0(%ebp), %eax	/* E: t = b + a */
	addl	20(%ebp), %ebx	/* F: t = b + a */
	roll	$9, %eax	/* E: rotate t */
	roll	$9, %ebx	/* F: rotate t */
	xorl	8(%ebp), %eax	/* E: c = c ^ t */
	xorl	28(%ebp), %ebx	/* F: c = c ^ t */
	movl	%eax, %ecx	/* E: copy c */
	movl	%ebx, %edx	/* F: copy c */
	movl	%ecx, 8(%ebp)	/* E: store c */
	movl	%edx, 28(%ebp)	/* F: store c */
	addl	%esi, %eax	/* E: t = c + b */
	addl	%edi, %ebx	/* F: t = c + b */
	roll	$13, %eax	/* E: rotate t */
	roll	$13, %ebx	/* F: rotate t */
	xorl	12(%ebp), %eax	/* E: d = d ^ t */
	xorl	16(%ebp), %ebx	/* F: d = d ^ t */
	movl	%eax, 12(%ebp)	/* E: store d */
	movl	%ebx, 16(%ebp)	/* F: store d */
	addl	%eax, %ecx	/* E: t = d + c */
	movl	40(%ebp), %eax	/* G: load a */
	addl	%ebx, %edx	/* F: t = d + c */
	movl	60(%ebp), %ebx	/* H: load a */
	roll	$18, %ecx	/* E: rotate t */
	addl	36(%ebp), %eax	/* G: t = a + d */
	roll	$18, %edx	/* F: rotate t */
	addl	56(%ebp), %ebx	/* H: t = a + d */
	xorl	0(%ebp), %ecx	/* E: a = a ^ t */
	roll	$7, %eax	/* G: rotate t */
	xorl	20(%ebp), %edx	/* F: a = a ^ t */
	roll	$7, %ebx	/* H: rotate t */
	movl	%ecx, 0(%ebp)	/* E: store a */
	movl	%edx, 20(%ebp)	/* F: store a */
/* quarters G and H */
	xorl	44(%ebp), %eax	/* G: b = b ^ t */
	xorl	48(%ebp), %ebx	/* H: b = b ^ t */
	movl	%eax, %esi	/* G: copy b */
	movl	%ebx, %edi	/* H: copy b */
	movl	%esi, 44(%ebp)	/* G: store b */
	movl	%edi, 48(%ebp)	/* H: store b */
	addl	40(%ebp), %eax	/* G: t = b + a */
	addl	60(%ebp), %ebx	/* H: t = b + a */
	roll	$9, %eax	/* G: rotate t */
	roll	$9, %ebx	/* H: rotate t */
	xorl	32(%ebp), %eax	/* G: c = c ^ t */
	xorl	52(%ebp), %ebx	/* H: c = c ^ t */
	movl	%eax, %ecx	/* G: copy c */
	movl	%ebx, %edx	/* H: copy c */
	movl	%ecx, 32(%ebp)	/* G: store c */
	movl	%edx, 52(%ebp)	/* H: store c */
	addl	%esi, %eax	/* G: t = c + b */
	addl	%edi, %ebx	/* H: t = c + b */
	roll	$13, %eax	/* G: rotate t */
	roll	$13, %ebx	/* H: rotate t */
	xorl	36(%ebp), %eax	/* G: d = d ^ t */
	xorl	56(%ebp), %ebx	/* H: d = d ^ t */
	movl	%eax, 36(%ebp)	/* G: store d */
	movl	%ebx, 56(%ebp)	/* H: store d */
	addl	%eax, %ecx	/* G: t = d + c */
	addl	%ebx, %edx	/* H: t = d + c */
	roll	$18, %ecx	/* G: rotate t */
	roll	$18, %edx	/* H: rotate t */
	xorl	40(%ebp), %ecx	/* G: a = a ^ t */
	xorl	60(%ebp), %edx	/* H: a = a ^ t */
	movl	%ecx, 40(%ebp)	/* G: store a */
	movl	%edx, 60(%ebp)	/* H: store a */
	decl	-4(%esp)
	jnz	.xor_salsa

/* write back data */
	movl	20(%esp), %ebx
	movq	0(%ebx), %mm0
	movq	8(%ebx), %mm1
	movq	16(%ebx), %mm2
	movq	24(%ebx), %mm3
	movq	32(%ebx), %mm4
	movq	40(%ebx), %mm5
	movq	48(%ebx), %mm6
	movq	56(%ebx), %mm7
	paddd	0(%ebp), %mm0
	paddd	8(%ebp), %mm1
	paddd	16(%ebp), %mm2
	paddd	24(%ebp), %mm3
	paddd	32(%ebp), %mm4
	paddd	40(%ebp), %mm5
	paddd	48(%ebp), %mm6
	paddd	56(%ebp), %mm7
	movq	%mm0, 0(%ebx)
	movq	%mm1, 8(%ebx)
	movq	%mm2, 16(%ebx)
	movq	%mm3, 24(%ebx)
	movq	%mm4, 32(%ebx)
	movq	%mm5, 40(%ebx)
	movq	%mm6, 48(%ebx)
	movq	%mm7, 56(%ebx)

	popl	%edi
	popl	%esi
	popl	%ebp
	popl	%ebx
	ret


/* neoscrypt_xor_chacha(mem, xormem, tempmem, double_rounds)
 * i386 (INT) ChaCha20 with XOR (MMX support required) */
neoscrypt_xor_chacha:
	pushl	%ebx
	pushl	%ebp
	pushl	%esi
	pushl	%edi
/* XOR and copy to temporary memory */
	movl	20(%esp), %ebx
	movl	24(%esp), %ecx
	movl	28(%esp), %ebp
	movq	0(%ebx), %mm0
	movq	8(%ebx), %mm1
	movq	16(%ebx), %mm2
	movq	24(%ebx), %mm3
	movq	32(%ebx), %mm4
	movq	40(%ebx), %mm5
	movq	48(%ebx), %mm6
	movq	56(%ebx), %mm7
	pxor	0(%ecx), %mm0
	pxor	8(%ecx), %mm1
	pxor	16(%ecx), %mm2
	pxor	24(%ecx), %mm3
	pxor	32(%ecx), %mm4
	pxor	40(%ecx), %mm5
	pxor	48(%ecx), %mm6
	pxor	56(%ecx), %mm7
	movq	%mm0, 0(%ebx)
	movq	%mm1, 8(%ebx)
	movq	%mm2, 16(%ebx)
	movq	%mm3, 24(%ebx)
	movq	%mm4, 32(%ebx)
	movq	%mm5, 40(%ebx)
	movq	%mm6, 48(%ebx)
	movq	%mm7, 56(%ebx)
	movq	%mm0, 0(%ebp)
	movq	%mm1, 8(%ebp)
	movq	%mm2, 16(%ebp)
	movq	%mm3, 24(%ebp)
	movq	%mm4, 32(%ebp)
	movq	%mm5, 40(%ebp)
	movq	%mm6, 48(%ebp)
	movq	%mm7, 56(%ebp)
/* number of double rounds */
	movl	32(%esp), %eax
	movl	%eax, -4(%esp)
.xor_chacha:
/* quarters A and B, initial C */
	movl	0(%ebp), %eax	/* A: load a */
	movl	16(%ebp), %ebx	/* A: load b */
	addl	%ebx, %eax	/* A: a = a + b */
	movl	32(%ebp), %ecx	/* A: load c */
	movl	48(%ebp), %edx	/* A: load d */
	xorl	%eax, %edx	/* A: d = d ^ a */
	movl	4(%ebp), %edi	/* B: load a */
	roll	$16, %edx	/* A: rotate d */
	movl	20(%ebp), %esi	/* B: load b */
	addl	%edx, %ecx	/* A: c = c + d */
	xorl	%ecx, %ebx	/* A: b = b ^ c */
	addl	%esi, %edi	/* B: a = a + b */
	roll	$12, %ebx	/* A: rotate b */
	addl	%ebx, %eax	/* A: a = a + b */
	movl	%eax, 0(%ebp)	/* A: store a */
	xorl	%eax, %edx	/* A: d = d ^ a */
	movl	52(%ebp), %eax	/* B: load d */
	roll	$8, %edx	/* A: rotate d */
	xorl	%edi, %eax	/* B: d = d ^ a */
	movl	%edx, 48(%ebp)	/* A: store d */
	addl	%edx, %ecx	/* A: c = c + d */
	movl	36(%ebp), %edx	/* B: load c */
	movl	%ecx, 32(%ebp)	/* A: store c */
	xorl	%ecx, %ebx	/* A: b = b ^ c */
	roll	$16, %eax	/* B: rotate d */
	movl	40(%ebp), %ecx	/* C: load c */
	roll	$7, %ebx	/* A: rotate b */
	addl	%eax, %edx	/* B: c = c + d */
	movl	%ebx, 16(%ebp)	/* A: store b */
	xorl	%edx, %esi	/* B: b = b ^ c */
	movl	24(%ebp), %ebx	/* C: load b */
	roll	$12, %esi	/* B: rotate b */
	addl	%esi, %edi	/* B: a = a + b */
	movl	%edi, 4(%ebp)	/* B: store a */
	xorl	%edi, %eax	/* B: d = d ^ a */
	roll	$8, %eax	/* B: rotate d */
	movl	%eax, 52(%ebp)	/* B: store d */
	addl	%eax, %edx	/* B: c = c + d */
	movl	8(%ebp), %eax	/* C: load a */
	movl	%edx, 36(%ebp)	/* B: store c */
	xorl	%edx, %esi	/* B: b = b ^ c */
	movl	56(%ebp), %edx	/* C: load d */
	roll	$7, %esi	/* B: rotate b */
	addl	%ebx, %eax	/* C: a = a + b */
	movl	%esi, 20(%ebp)	/* B: store b */
/* quarters C and D, initial E */
	xorl	%eax, %edx	/* C: d = d ^ a */
	movl	12(%ebp), %edi	/* D: load a */
	roll	$16, %edx	/* C: rotate d */
	movl	28(%ebp), %esi	/* D: load b */
	addl	%edx, %ecx	/* C: c = c + d */
	xorl	%ecx, %ebx	/* C: b = b ^ c */
	addl	%esi, %edi	/* D: a = a + b */
	roll	$12, %ebx	/* C: rotate b */
	addl	%ebx, %eax	/* C: a = a + b */
	movl	%eax, 8(%ebp)	/* C: store a */
	xorl	%eax, %edx	/* C: d = d ^ a */
	movl	60(%ebp), %eax	/* D: load d */
	roll	$8, %edx	/* C: rotate d */
	xorl	%edi, %eax	/* D: d = d ^ a */
	movl	%edx, 56(%ebp)	/* C: store d */
	addl	%edx, %ecx	/* C: c = c + d */
	movl	44(%ebp), %edx	/* D: load c */
	movl	%ecx, 40(%ebp)	/* C: store c */
	xorl	%ecx, %ebx	/* C: b = b ^ c */
	roll	$16, %eax	/* D: rotate d */
	movl	40(%ebp), %ecx	/* E: load c */
	roll	$7, %ebx	/* C: rotate b */
	addl	%eax, %edx	/* D: c = c + d */
	movl	%ebx, 24(%ebp)	/* C: store b */
	xorl	%edx, %esi	/* D: b = b ^ c */
	movl	20(%ebp), %ebx	/* E: load b */
	roll	$12, %esi	/* D: rotate b */
	addl	%esi, %edi	/* D: a = a + b */
	movl	%edi, 12(%ebp)	/* D: store a */
	xorl	%edi, %eax	/* D: d = d ^ a */
	roll	$8, %eax	/* D: rotate d */
	movl	%eax, 60(%ebp)	/* D: store d */
	addl	%eax, %edx	/* D: c = c + d */
	movl	0(%ebp), %eax	/* E: load a */
	movl	%edx, 44(%ebp)	/* D: store c */
	xorl	%edx, %esi	/* D: b = b ^ c */
	movl	60(%ebp), %edx	/* E: load d */
	roll	$7, %esi	/* D: rotate b */
	addl	%ebx, %eax	/* E: a = a + b */
	movl	%esi, 28(%ebp)	/* D: store b */
/* quarters E and F, initial G */
	xorl	%eax, %edx	/* E: d = d ^ a */
	movl	4(%ebp), %edi	/* F: load a */
	roll	$16, %edx	/* E: rotate d */
	movl	24(%ebp), %esi	/* F: load b */
	addl	%edx, %ecx	/* E: c = c + d */
	xorl	%ecx, %ebx	/* E: b = b ^ c */
	addl	%esi, %edi	/* F: a = a + b */
	roll	$12, %ebx	/* E: rotate b */
	addl	%ebx, %eax	/* E: a = a + b */
	movl	%eax, 0(%ebp)	/* E: store a */
	xorl	%eax, %edx	/* E: d = d ^ a */
	movl	48(%ebp), %eax	/* F: load d */
	roll	$8, %edx	/* E: rotate d */
	xorl	%edi, %eax	/* F: d = d ^ a */
	movl	%edx, 60(%ebp)	/* E: store d */
	addl	%edx, %ecx	/* E: c = c + d */
	movl	44(%ebp), %edx	/* F: load c */
	movl	%ecx, 40(%ebp)	/* E: store c */
	xorl	%ecx, %ebx	/* E: b = b ^ c */
	roll	$16, %eax	/* F: rotate d */
	movl	32(%ebp), %ecx	/* G: load c */
	roll	$7, %ebx	/* E: rotate b */
	addl	%eax, %edx	/* F: c = c + d */
	movl	%ebx, 20(%ebp)	/* E: store b */
	xorl	%edx, %esi	/* F: b = b ^ c */
	movl	28(%ebp), %ebx	/* G: load b */
	roll	$12, %esi	/* F: rotate b */
	addl	%esi, %edi	/* F: a = a + b */
	movl	%edi, 4(%ebp)	/* F: store a */
	xorl	%edi, %eax	/* F: d = d ^ a */
	roll	$8, %eax	/* F: rotate d */
	movl	%eax, 48(%ebp)	/* F: store d */
	addl	%eax, %edx	/* F: c = c + d */
	movl	8(%ebp), %eax	/* G: load a */
	movl	%edx, 44(%ebp)	/* F: store c */
	xorl	%edx, %esi	/* F: b = b ^ c */
	movl	52(%ebp), %edx	/* G: load d */
	roll	$7, %esi	/* F: rotate b */
	addl	%ebx, %eax	/* G: a = a + b */
	movl	%esi, 24(%ebp)	/* F: store b */
/* quarters G and H */
	xorl	%eax, %edx	/* G: d = d ^ a */
	movl	12(%ebp), %edi	/* H: load a */
	roll	$16, %edx	/* G: rotate d */
	movl	16(%ebp), %esi	/* H: load b */
	addl	%edx, %ecx	/* G: c = c + d */
	xorl	%ecx, %ebx	/* G: b = b ^ c */
	addl	%esi, %edi	/* H: a = a + b */
	roll	$12, %ebx	/* G: rotate b */
	addl	%ebx, %eax	/* G: a = a + b */
	movl	%eax, 8(%ebp)	/* G: store a */
	xorl	%eax, %edx	/* G: d = d ^ a */
	movl	56(%ebp), %eax	/* H: load d */
	roll	$8, %edx	/* G: rotate d */
	xorl	%edi, %eax	/* H: d = d ^ a */
	movl	%edx, 52(%ebp)	/* G: store d */
	addl	%edx, %ecx	/* G: c = c + d */
	movl	36(%ebp), %edx	/* H: load c */
	movl	%ecx, 32(%ebp)	/* G: store c */
	xorl	%ecx, %ebx	/* G: b = b ^ c */
	roll	$16, %eax	/* H: rotate d */
	roll	$7, %ebx	/* G: rotate b */
	addl	%eax, %edx	/* H: c = c + d */
	movl	%ebx, 28(%ebp)	/* G: store b */
	xorl	%edx, %esi	/* H: b = b ^ c */
	roll	$12, %esi	/* H: rotate b */
	addl	%esi, %edi	/* H: a = a + b */
	movl	%edi, 12(%ebp)	/* H: store a */
	xorl	%edi, %eax	/* H: d = d ^ a */
	roll	$8, %eax	/* H: rotate d */
	movl	%eax, 56(%ebp)	/* H: store d */
	addl	%eax, %edx	/* H: c = c + d */
	movl	%edx, 36(%ebp)	/* H: store c */
	xorl	%edx, %esi	/* H: b = b ^ c */
	roll	$7, %esi	/* H: rotate b */
	movl	%esi, 16(%ebp)	/* H: store b */
	decl	-4(%esp)
	jnz	.xor_chacha

/* write back data */
	movl	20(%esp), %ebx
	movq	0(%ebx), %mm0
	movq	8(%ebx), %mm1
	movq	16(%ebx), %mm2
	movq	24(%ebx), %mm3
	movq	32(%ebx), %mm4
	movq	40(%ebx), %mm5
	movq	48(%ebx), %mm6
	movq	56(%ebx), %mm7
	paddd	0(%ebp), %mm0
	paddd	8(%ebp), %mm1
	paddd	16(%ebp), %mm2
	paddd	24(%ebp), %mm3
	paddd	32(%ebp), %mm4
	paddd	40(%ebp), %mm5
	paddd	48(%ebp), %mm6
	paddd	56(%ebp), %mm7
	movq	%mm0, 0(%ebx)
	movq	%mm1, 8(%ebx)
	movq	%mm2, 16(%ebx)
	movq	%mm3, 24(%ebx)
	movq	%mm4, 32(%ebx)
	movq	%mm5, 40(%ebx)
	movq	%mm6, 48(%ebx)
	movq	%mm7, 56(%ebx)

	popl	%edi
	popl	%esi
	popl	%ebp
	popl	%ebx
	ret


/* neoscrypt_salsa_tangle_sse2(mem, count)
 * i386 (SSE2) Salsa20 map switcher;
 * correct map:  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
 * SSE2 map:     0   5  10  15  12   1   6  11   8  13   2   7   4   9  14   3 */
neoscrypt_salsa_tangle_sse2:
	pushl	%ebx
	movl	8(%esp), %ebx
	movl	12(%esp), %ecx
.salsa_tangle_sse2:
	movl	4(%ebx), %eax
	movl	20(%ebx), %edx
	movl	%eax, 20(%ebx)
	movl	%edx, 4(%ebx)
	movl	8(%ebx), %eax
	movl	40(%ebx), %edx
	movl	%eax, 40(%ebx)
	movl	%edx, 8(%ebx)
	movl	12(%ebx), %eax
	movl	60(%ebx), %edx
	movl	%eax, 60(%ebx)
	movl	%edx, 12(%ebx)
	movl	16(%ebx), %eax
	movl	48(%ebx), %edx
	movl	%eax, 48(%ebx)
	movl	%edx, 16(%ebx)
	movl	28(%ebx), %eax
	movl	44(%ebx), %edx
	movl	%eax, 44(%ebx)
	movl	%edx, 28(%ebx)
	movl	36(%ebx), %eax
	movl	52(%ebx), %edx
	movl	%eax, 52(%ebx)
	movl	%edx, 36(%ebx)
	addl	$64, %ebx
	decl	%ecx
	jnz	.salsa_tangle_sse2

	popl	%ebx
	ret


/* neoscrypt_xor_salsa_sse2(mem, xormem, double_rounds)
 * i386 (SSE2) Salsa20 with XOR;
 * mem and xormem must be aligned properly */
neoscrypt_xor_salsa_sse2:
	movl	4(%esp), %edx
	movl	8(%esp), %eax
	movl	12(%esp), %ecx
	movdqa	0(%edx), %xmm0
	movdqa	16(%edx), %xmm1
	movdqa	32(%edx), %xmm2
	movdqa	48(%edx), %xmm3
	pxor	0(%eax), %xmm0
	pxor	16(%eax), %xmm1
	pxor	32(%eax), %xmm2
	pxor	48(%eax), %xmm3
	movdqa	%xmm0, %xmm6
	movdqa	%xmm1, %xmm7
	movdqa	%xmm2, 32(%edx)
	movdqa	%xmm3, 48(%edx)
.xor_salsa_sse2:
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
	jnz	.xor_salsa_sse2

	paddd	%xmm6, %xmm0
	paddd	%xmm7, %xmm1
	paddd	32(%edx), %xmm2
	paddd	48(%edx), %xmm3
	movdqa	%xmm0, 0(%edx)
	movdqa	%xmm1, 16(%edx)
	movdqa	%xmm2, 32(%edx)
	movdqa	%xmm3, 48(%edx)

	ret


/* neoscrypt_xor_chacha_sse2(mem, xormem, double_rounds)
 * i386 (SSE2) ChaCha20 with XOR;
 * mem and xormem must be aligned properly */
neoscrypt_xor_chacha_sse2:
	movl	4(%esp), %edx
	movl	8(%esp), %eax
	movl	12(%esp), %ecx
	movdqa	0(%edx), %xmm0
	movdqa	16(%edx), %xmm1
	movdqa	32(%edx), %xmm2
	movdqa	48(%edx), %xmm3
	pxor	0(%eax), %xmm0
	pxor	16(%eax), %xmm1
	pxor	32(%eax), %xmm2
	pxor	48(%eax), %xmm3
	movdqa	%xmm0, %xmm5
	movdqa	%xmm1, %xmm6
	movdqa	%xmm2, %xmm7
	movdqa	%xmm3, 48(%edx)
.xor_chacha_sse2:
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
	jnz	.xor_chacha_sse2

	paddd	%xmm5, %xmm0
	paddd	%xmm6, %xmm1
	paddd	%xmm7, %xmm2
	paddd	48(%edx), %xmm3
	movdqa	%xmm0, 0(%edx)
	movdqa	%xmm1, 16(%edx)
	movdqa	%xmm2, 32(%edx)
	movdqa	%xmm3, 48(%edx)

	ret


/* neoscrypt(input, output, profile)
 * i386 (INT, SSE2) NeoScrypt engine (MMX required for INT);
 * supports NeoScrypt and Scrypt only */
.globl neoscrypt
.globl _neoscrypt
neoscrypt:
_neoscrypt:
	pushl	%ebx
	pushl	%ebp
	pushl	%esi
	pushl	%edi
	movl	20(%esp), %esi
	movl	24(%esp), %edi
	movl	28(%esp), %ebx

#if (SHA256)
/* Scrypt mode */
	testl	$0x01, %ebx
	jnz	.scrypt
#endif

#if (WIN32)
/* attempt to allocate 33280 + 128 bytes of stack space fails miserably;
 * have to use malloc() and free() instead */
	subl	$64, %esp
/* allocate memory (9 pages of 4Kb each) */
	movl	$0x9000, 0(%esp)
	call	_malloc
/* save memory address */
	movl	%eax, 32(%esp)
/* align memory */
	addl	$64, %eax
	andl	$0xFFFFFFC0, %eax
/* memory base: X, Z, V */
	leal	64(%eax), %ebp
#else
/* align stack */
	movl	%esp, %eax
	andl	$0xFFFFFFC0, %esp
	subl	$0x8280, %esp
/* save unaligned stack */
	movl	%eax, 32(%esp)
/* memory base: X, Z, V */
	leal	128(%esp), %ebp
#endif /* (WIN32) */

/* FastKDF */
#if (OPT)
	movl	%esi, 0(%esp)
	movl	%esi, 4(%esp)
	movl	%ebp, 8(%esp)
	xorl	%eax, %eax
	movl	%eax, 12(%esp)
#if (WIN32) || (__APPLE__)
	call	_neoscrypt_fastkdf_opt
#else
	call	neoscrypt_fastkdf_opt
#endif /* (WIN32) || (__APPLE__) */
#else
	movl	$80, %eax
	movl	%esi, 0(%esp)
	movl	%eax, 4(%esp)
	movl	%esi, 8(%esp)
	movl	%eax, 12(%esp)
	movl	$32, 16(%esp)
	movl	%ebp, 20(%esp)
	movl	$256, 24(%esp)
#if (WIN32) || (__APPLE__)
	call	_neoscrypt_fastkdf
#else
	call	neoscrypt_fastkdf
#endif /* (WIN32) || (__APPLE__) */
#endif /* (OPT) */

/* SSE2 switch */
	testl	$0x1000, %ebx
	jnz	.neoscrypt_sse2

/* blkcpy(Z, X) */
	leal	256(%ebp), %eax
	movq	0(%ebp), %mm0
	movq	8(%ebp), %mm1
	movq	16(%ebp), %mm2
	movq	24(%ebp), %mm3
	movq	32(%ebp), %mm4
	movq	40(%ebp), %mm5
	movq	48(%ebp), %mm6
	movq	56(%ebp), %mm7
	movq	%mm0, 0(%eax)
	movq	%mm1, 8(%eax)
	movq	%mm2, 16(%eax)
	movq	%mm3, 24(%eax)
	movq	%mm4, 32(%eax)
	movq	%mm5, 40(%eax)
	movq	%mm6, 48(%eax)
	movq	%mm7, 56(%eax)
	movq	64(%ebp), %mm0
	movq	72(%ebp), %mm1
	movq	80(%ebp), %mm2
	movq	88(%ebp), %mm3
	movq	96(%ebp), %mm4
	movq	104(%ebp), %mm5
	movq	112(%ebp), %mm6
	movq	120(%ebp), %mm7
	movq	%mm0, 64(%eax)
	movq	%mm1, 72(%eax)
	movq	%mm2, 80(%eax)
	movq	%mm3, 88(%eax)
	movq	%mm4, 96(%eax)
	movq	%mm5, 104(%eax)
	movq	%mm6, 112(%eax)
	movq	%mm7, 120(%eax)
	movq	128(%ebp), %mm0
	movq	136(%ebp), %mm1
	movq	144(%ebp), %mm2
	movq	152(%ebp), %mm3
	movq	160(%ebp), %mm4
	movq	168(%ebp), %mm5
	movq	176(%ebp), %mm6
	movq	184(%ebp), %mm7
	movq	%mm0, 128(%eax)
	movq	%mm1, 136(%eax)
	movq	%mm2, 144(%eax)
	movq	%mm3, 152(%eax)
	movq	%mm4, 160(%eax)
	movq	%mm5, 168(%eax)
	movq	%mm6, 176(%eax)
	movq	%mm7, 184(%eax)
	movq	192(%ebp), %mm0
	movq	200(%ebp), %mm1
	movq	208(%ebp), %mm2
	movq	216(%ebp), %mm3
	movq	224(%ebp), %mm4
	movq	232(%ebp), %mm5
	movq	240(%ebp), %mm6
	movq	248(%ebp), %mm7
	movq	%mm0, 192(%eax)
	movq	%mm1, 200(%eax)
	movq	%mm2, 208(%eax)
	movq	%mm3, 216(%eax)
	movq	%mm4, 224(%eax)
	movq	%mm5, 232(%eax)
	movq	%mm6, 240(%eax)
	movq	%mm7, 248(%eax)

	leal	-64(%ebp), %edx
	movl	%edx, 8(%esp)
	movl	$10, 12(%esp)

	xorl	%ebx, %ebx
.chacha_ns1:
/* blkcpy(V, Z) */
	leal	512(%ebp), %eax
	movl	%ebx, %edx
	movb	$8, %cl
	shll	%cl, %edx
	leal	256(%ebp), %ecx
	addl	%edx, %eax
	movq	0(%ecx), %mm0
	movq	8(%ecx), %mm1
	movq	16(%ecx), %mm2
	movq	24(%ecx), %mm3
	movq	32(%ecx), %mm4
	movq	40(%ecx), %mm5
	movq	48(%ecx), %mm6
	movq	56(%ecx), %mm7
	movq	%mm0, 0(%eax)
	movq	%mm1, 8(%eax)
	movq	%mm2, 16(%eax)
	movq	%mm3, 24(%eax)
	movq	%mm4, 32(%eax)
	movq	%mm5, 40(%eax)
	movq	%mm6, 48(%eax)
	movq	%mm7, 56(%eax)
	movq	64(%ecx), %mm0
	movq	72(%ecx), %mm1
	movq	80(%ecx), %mm2
	movq	88(%ecx), %mm3
	movq	96(%ecx), %mm4
	movq	104(%ecx), %mm5
	movq	112(%ecx), %mm6
	movq	120(%ecx), %mm7
	movq	%mm0, 64(%eax)
	movq	%mm1, 72(%eax)
	movq	%mm2, 80(%eax)
	movq	%mm3, 88(%eax)
	movq	%mm4, 96(%eax)
	movq	%mm5, 104(%eax)
	movq	%mm6, 112(%eax)
	movq	%mm7, 120(%eax)
	movq	128(%ecx), %mm0
	movq	136(%ecx), %mm1
	movq	144(%ecx), %mm2
	movq	152(%ecx), %mm3
	movq	160(%ecx), %mm4
	movq	168(%ecx), %mm5
	movq	176(%ecx), %mm6
	movq	184(%ecx), %mm7
	movq	%mm0, 128(%eax)
	movq	%mm1, 136(%eax)
	movq	%mm2, 144(%eax)
	movq	%mm3, 152(%eax)
	movq	%mm4, 160(%eax)
	movq	%mm5, 168(%eax)
	movq	%mm6, 176(%eax)
	movq	%mm7, 184(%eax)
	movq	192(%ecx), %mm0
	movq	200(%ecx), %mm1
	movq	208(%ecx), %mm2
	movq	216(%ecx), %mm3
	movq	224(%ecx), %mm4
	movq	232(%ecx), %mm5
	movq	240(%ecx), %mm6
	movq	248(%ecx), %mm7
	movq	%mm0, 192(%eax)
	movq	%mm1, 200(%eax)
	movq	%mm2, 208(%eax)
	movq	%mm3, 216(%eax)
	movq	%mm4, 224(%eax)
	movq	%mm5, 232(%eax)
	movq	%mm6, 240(%eax)
	movq	%mm7, 248(%eax)
/* blkmix(Z) */
	leal	256(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	448(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha
	leal	320(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	256(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha
	leal	384(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	320(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha
	leal	448(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	384(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha
	leal	320(%ebp), %eax
	leal	384(%ebp), %edx
	movq	0(%eax), %mm0
	movq	8(%eax), %mm1
	movq	16(%eax), %mm2
	movq	24(%eax), %mm3
	movq	0(%edx), %mm4
	movq	8(%edx), %mm5
	movq	16(%edx), %mm6
	movq	24(%edx), %mm7
	movq	%mm0, 0(%edx)
	movq	%mm1, 8(%edx)
	movq	%mm2, 16(%edx)
	movq	%mm3, 24(%edx)
	movq	%mm4, 0(%eax)
	movq	%mm5, 8(%eax)
	movq	%mm6, 16(%eax)
	movq	%mm7, 24(%eax)
	movq	32(%eax), %mm0
	movq	40(%eax), %mm1
	movq	48(%eax), %mm2
	movq	56(%eax), %mm3
	movq	32(%edx), %mm4
	movq	40(%edx), %mm5
	movq	48(%edx), %mm6
	movq	56(%edx), %mm7
	movq	%mm0, 32(%edx)
	movq	%mm1, 40(%edx)
	movq	%mm2, 48(%edx)
	movq	%mm3, 56(%edx)
	movq	%mm4, 32(%eax)
	movq	%mm5, 40(%eax)
	movq	%mm6, 48(%eax)
	movq	%mm7, 56(%eax)
	incl	%ebx
	cmpl	$128, %ebx
	jnz	.chacha_ns1

	xorl	%ebx, %ebx
.chacha_ns2:
/* integerify(Z) mod 128 */
	leal	256(%ebp), %eax
	leal	512(%ebp), %ecx
	movl	448(%ebp), %edx
	andl	$0x7F, %edx
	shll	$8, %edx
	addl	%edx, %ecx
/* blkxor(Z, V) */
	movq	0(%eax), %mm0
	movq	8(%eax), %mm1
	movq	16(%eax), %mm2
	movq	24(%eax), %mm3
	movq	32(%eax), %mm4
	movq	40(%eax), %mm5
	movq	48(%eax), %mm6
	movq	56(%eax), %mm7
	pxor	0(%ecx), %mm0
	pxor	8(%ecx), %mm1
	pxor	16(%ecx), %mm2
	pxor	24(%ecx), %mm3
	pxor	32(%ecx), %mm4
	pxor	40(%ecx), %mm5
	pxor	48(%ecx), %mm6
	pxor	56(%ecx), %mm7
	movq	%mm0, 0(%eax)
	movq	%mm1, 8(%eax)
	movq	%mm2, 16(%eax)
	movq	%mm3, 24(%eax)
	movq	%mm4, 32(%eax)
	movq	%mm5, 40(%eax)
	movq	%mm6, 48(%eax)
	movq	%mm7, 56(%eax)
	movq	64(%eax), %mm0
	movq	72(%eax), %mm1
	movq	80(%eax), %mm2
	movq	88(%eax), %mm3
	movq	96(%eax), %mm4
	movq	104(%eax), %mm5
	movq	112(%eax), %mm6
	movq	120(%eax), %mm7
	pxor	64(%ecx), %mm0
	pxor	72(%ecx), %mm1
	pxor	80(%ecx), %mm2
	pxor	88(%ecx), %mm3
	pxor	96(%ecx), %mm4
	pxor	104(%ecx), %mm5
	pxor	112(%ecx), %mm6
	pxor	120(%ecx), %mm7
	movq	%mm0, 64(%eax)
	movq	%mm1, 72(%eax)
	movq	%mm2, 80(%eax)
	movq	%mm3, 88(%eax)
	movq	%mm4, 96(%eax)
	movq	%mm5, 104(%eax)
	movq	%mm6, 112(%eax)
	movq	%mm7, 120(%eax)
	movq	128(%eax), %mm0
	movq	136(%eax), %mm1
	movq	144(%eax), %mm2
	movq	152(%eax), %mm3
	movq	160(%eax), %mm4
	movq	168(%eax), %mm5
	movq	176(%eax), %mm6
	movq	184(%eax), %mm7
	pxor	128(%ecx), %mm0
	pxor	136(%ecx), %mm1
	pxor	144(%ecx), %mm2
	pxor	152(%ecx), %mm3
	pxor	160(%ecx), %mm4
	pxor	168(%ecx), %mm5
	pxor	176(%ecx), %mm6
	pxor	184(%ecx), %mm7
	movq	%mm0, 128(%eax)
	movq	%mm1, 136(%eax)
	movq	%mm2, 144(%eax)
	movq	%mm3, 152(%eax)
	movq	%mm4, 160(%eax)
	movq	%mm5, 168(%eax)
	movq	%mm6, 176(%eax)
	movq	%mm7, 184(%eax)
	movq	192(%eax), %mm0
	movq	200(%eax), %mm1
	movq	208(%eax), %mm2
	movq	216(%eax), %mm3
	movq	224(%eax), %mm4
	movq	232(%eax), %mm5
	movq	240(%eax), %mm6
	movq	248(%eax), %mm7
	pxor	192(%ecx), %mm0
	pxor	200(%ecx), %mm1
	pxor	208(%ecx), %mm2
	pxor	216(%ecx), %mm3
	pxor	224(%ecx), %mm4
	pxor	232(%ecx), %mm5
	pxor	240(%ecx), %mm6
	pxor	248(%ecx), %mm7
	movq	%mm0, 192(%eax)
	movq	%mm1, 200(%eax)
	movq	%mm2, 208(%eax)
	movq	%mm3, 216(%eax)
	movq	%mm4, 224(%eax)
	movq	%mm5, 232(%eax)
	movq	%mm6, 240(%eax)
	movq	%mm7, 248(%eax)
/* blkmix(Z) */
	leal	256(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	448(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha
	leal	320(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	256(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha
	leal	384(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	320(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha
	leal	448(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	384(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha
	leal	320(%ebp), %eax
	leal	384(%ebp), %edx
	movq	0(%eax), %mm0
	movq	8(%eax), %mm1
	movq	16(%eax), %mm2
	movq	24(%eax), %mm3
	movq	0(%edx), %mm4
	movq	8(%edx), %mm5
	movq	16(%edx), %mm6
	movq	24(%edx), %mm7
	movq	%mm0, 0(%edx)
	movq	%mm1, 8(%edx)
	movq	%mm2, 16(%edx)
	movq	%mm3, 24(%edx)
	movq	%mm4, 0(%eax)
	movq	%mm5, 8(%eax)
	movq	%mm6, 16(%eax)
	movq	%mm7, 24(%eax)
	movq	32(%eax), %mm0
	movq	40(%eax), %mm1
	movq	48(%eax), %mm2
	movq	56(%eax), %mm3
	movq	32(%edx), %mm4
	movq	40(%edx), %mm5
	movq	48(%edx), %mm6
	movq	56(%edx), %mm7
	movq	%mm0, 32(%edx)
	movq	%mm1, 40(%edx)
	movq	%mm2, 48(%edx)
	movq	%mm3, 56(%edx)
	movq	%mm4, 32(%eax)
	movq	%mm5, 40(%eax)
	movq	%mm6, 48(%eax)
	movq	%mm7, 56(%eax)
	incl	%ebx
	cmpl	$128, %ebx
	jnz	.chacha_ns2

	xorl	%ebx, %ebx
.salsa_ns1:
/* blkcpy(V, X) */
	leal	512(%ebp), %eax
	movl	%ebx, %edx
	movl	$8, %ecx
	shll	%cl, %edx
	addl	%edx, %eax
	movq	0(%ebp), %mm0
	movq	8(%ebp), %mm1
	movq	16(%ebp), %mm2
	movq	24(%ebp), %mm3
	movq	32(%ebp), %mm4
	movq	40(%ebp), %mm5
	movq	48(%ebp), %mm6
	movq	56(%ebp), %mm7
	movq	%mm0, 0(%eax)
	movq	%mm1, 8(%eax)
	movq	%mm2, 16(%eax)
	movq	%mm3, 24(%eax)
	movq	%mm4, 32(%eax)
	movq	%mm5, 40(%eax)
	movq	%mm6, 48(%eax)
	movq	%mm7, 56(%eax)
	movq	64(%ebp), %mm0
	movq	72(%ebp), %mm1
	movq	80(%ebp), %mm2
	movq	88(%ebp), %mm3
	movq	96(%ebp), %mm4
	movq	104(%ebp), %mm5
	movq	112(%ebp), %mm6
	movq	120(%ebp), %mm7
	movq	%mm0, 64(%eax)
	movq	%mm1, 72(%eax)
	movq	%mm2, 80(%eax)
	movq	%mm3, 88(%eax)
	movq	%mm4, 96(%eax)
	movq	%mm5, 104(%eax)
	movq	%mm6, 112(%eax)
	movq	%mm7, 120(%eax)
	movq	128(%ebp), %mm0
	movq	136(%ebp), %mm1
	movq	144(%ebp), %mm2
	movq	152(%ebp), %mm3
	movq	160(%ebp), %mm4
	movq	168(%ebp), %mm5
	movq	176(%ebp), %mm6
	movq	184(%ebp), %mm7
	movq	%mm0, 128(%eax)
	movq	%mm1, 136(%eax)
	movq	%mm2, 144(%eax)
	movq	%mm3, 152(%eax)
	movq	%mm4, 160(%eax)
	movq	%mm5, 168(%eax)
	movq	%mm6, 176(%eax)
	movq	%mm7, 184(%eax)
	movq	192(%ebp), %mm0
	movq	200(%ebp), %mm1
	movq	208(%ebp), %mm2
	movq	216(%ebp), %mm3
	movq	224(%ebp), %mm4
	movq	232(%ebp), %mm5
	movq	240(%ebp), %mm6
	movq	248(%ebp), %mm7
	movq	%mm0, 192(%eax)
	movq	%mm1, 200(%eax)
	movq	%mm2, 208(%eax)
	movq	%mm3, 216(%eax)
	movq	%mm4, 224(%eax)
	movq	%mm5, 232(%eax)
	movq	%mm6, 240(%eax)
	movq	%mm7, 248(%eax)
/* blkmix(X) */
	movl	%ebp, 0(%esp)
	leal	192(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa
	leal	64(%ebp), %eax
	movl	%eax, 0(%esp)
	movl	%ebp, 4(%esp)
	call	neoscrypt_xor_salsa
	leal	128(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	64(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa
	leal	192(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	128(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa
	leal	64(%ebp), %eax
	leal	128(%ebp), %edx
	movq	0(%eax), %mm0
	movq	8(%eax), %mm1
	movq	16(%eax), %mm2
	movq	24(%eax), %mm3
	movq	0(%edx), %mm4
	movq	8(%edx), %mm5
	movq	16(%edx), %mm6
	movq	24(%edx), %mm7
	movq	%mm0, 0(%edx)
	movq	%mm1, 8(%edx)
	movq	%mm2, 16(%edx)
	movq	%mm3, 24(%edx)
	movq	%mm4, 0(%eax)
	movq	%mm5, 8(%eax)
	movq	%mm6, 16(%eax)
	movq	%mm7, 24(%eax)
	movq	32(%eax), %mm0
	movq	40(%eax), %mm1
	movq	48(%eax), %mm2
	movq	56(%eax), %mm3
	movq	32(%edx), %mm4
	movq	40(%edx), %mm5
	movq	48(%edx), %mm6
	movq	56(%edx), %mm7
	movq	%mm0, 32(%edx)
	movq	%mm1, 40(%edx)
	movq	%mm2, 48(%edx)
	movq	%mm3, 56(%edx)
	movq	%mm4, 32(%eax)
	movq	%mm5, 40(%eax)
	movq	%mm6, 48(%eax)
	movq	%mm7, 56(%eax)
	incl	%ebx
	cmpl	$128, %ebx
	jnz	.salsa_ns1

	xorl	%ebx, %ebx
.salsa_ns2:
/* integerify(X) mod 128 */
	leal	512(%ebp), %ecx
	movl	192(%ebp), %edx
	andl	$0x7F, %edx
	shll	$8, %edx
	addl	%edx, %ecx
/* blkxor(X, V) */
	movq	0(%ebp), %mm0
	movq	8(%ebp), %mm1
	movq	16(%ebp), %mm2
	movq	24(%ebp), %mm3
	movq	32(%ebp), %mm4
	movq	40(%ebp), %mm5
	movq	48(%ebp), %mm6
	movq	56(%ebp), %mm7
	pxor	0(%ecx), %mm0
	pxor	8(%ecx), %mm1
	pxor	16(%ecx), %mm2
	pxor	24(%ecx), %mm3
	pxor	32(%ecx), %mm4
	pxor	40(%ecx), %mm5
	pxor	48(%ecx), %mm6
	pxor	56(%ecx), %mm7
	movq	%mm0, 0(%ebp)
	movq	%mm1, 8(%ebp)
	movq	%mm2, 16(%ebp)
	movq	%mm3, 24(%ebp)
	movq	%mm4, 32(%ebp)
	movq	%mm5, 40(%ebp)
	movq	%mm6, 48(%ebp)
	movq	%mm7, 56(%ebp)
	movq	64(%ebp), %mm0
	movq	72(%ebp), %mm1
	movq	80(%ebp), %mm2
	movq	88(%ebp), %mm3
	movq	96(%ebp), %mm4
	movq	104(%ebp), %mm5
	movq	112(%ebp), %mm6
	movq	120(%ebp), %mm7
	pxor	64(%ecx), %mm0
	pxor	72(%ecx), %mm1
	pxor	80(%ecx), %mm2
	pxor	88(%ecx), %mm3
	pxor	96(%ecx), %mm4
	pxor	104(%ecx), %mm5
	pxor	112(%ecx), %mm6
	pxor	120(%ecx), %mm7
	movq	%mm0, 64(%ebp)
	movq	%mm1, 72(%ebp)
	movq	%mm2, 80(%ebp)
	movq	%mm3, 88(%ebp)
	movq	%mm4, 96(%ebp)
	movq	%mm5, 104(%ebp)
	movq	%mm6, 112(%ebp)
	movq	%mm7, 120(%ebp)
	movq	128(%ebp), %mm0
	movq	136(%ebp), %mm1
	movq	144(%ebp), %mm2
	movq	152(%ebp), %mm3
	movq	160(%ebp), %mm4
	movq	168(%ebp), %mm5
	movq	176(%ebp), %mm6
	movq	184(%ebp), %mm7
	pxor	128(%ecx), %mm0
	pxor	136(%ecx), %mm1
	pxor	144(%ecx), %mm2
	pxor	152(%ecx), %mm3
	pxor	160(%ecx), %mm4
	pxor	168(%ecx), %mm5
	pxor	176(%ecx), %mm6
	pxor	184(%ecx), %mm7
	movq	%mm0, 128(%ebp)
	movq	%mm1, 136(%ebp)
	movq	%mm2, 144(%ebp)
	movq	%mm3, 152(%ebp)
	movq	%mm4, 160(%ebp)
	movq	%mm5, 168(%ebp)
	movq	%mm6, 176(%ebp)
	movq	%mm7, 184(%ebp)
	movq	192(%ebp), %mm0
	movq	200(%ebp), %mm1
	movq	208(%ebp), %mm2
	movq	216(%ebp), %mm3
	movq	224(%ebp), %mm4
	movq	232(%ebp), %mm5
	movq	240(%ebp), %mm6
	movq	248(%ebp), %mm7
	pxor	192(%ecx), %mm0
	pxor	200(%ecx), %mm1
	pxor	208(%ecx), %mm2
	pxor	216(%ecx), %mm3
	pxor	224(%ecx), %mm4
	pxor	232(%ecx), %mm5
	pxor	240(%ecx), %mm6
	pxor	248(%ecx), %mm7
	movq	%mm0, 192(%ebp)
	movq	%mm1, 200(%ebp)
	movq	%mm2, 208(%ebp)
	movq	%mm3, 216(%ebp)
	movq	%mm4, 224(%ebp)
	movq	%mm5, 232(%ebp)
	movq	%mm6, 240(%ebp)
	movq	%mm7, 248(%ebp)
/* blkmix(X) */
	movl	%ebp, 0(%esp)
	leal	192(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa
	leal	64(%ebp), %eax
	movl	%eax, 0(%esp)
	movl	%ebp, 4(%esp)
	call	neoscrypt_xor_salsa
	leal	128(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	64(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa
	leal	192(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	128(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa
	leal	64(%ebp), %eax
	leal	128(%ebp), %edx
	movq	0(%eax), %mm0
	movq	8(%eax), %mm1
	movq	16(%eax), %mm2
	movq	24(%eax), %mm3
	movq	0(%edx), %mm4
	movq	8(%edx), %mm5
	movq	16(%edx), %mm6
	movq	24(%edx), %mm7
	movq	%mm0, 0(%edx)
	movq	%mm1, 8(%edx)
	movq	%mm2, 16(%edx)
	movq	%mm3, 24(%edx)
	movq	%mm4, 0(%eax)
	movq	%mm5, 8(%eax)
	movq	%mm6, 16(%eax)
	movq	%mm7, 24(%eax)
	movq	32(%eax), %mm0
	movq	40(%eax), %mm1
	movq	48(%eax), %mm2
	movq	56(%eax), %mm3
	movq	32(%edx), %mm4
	movq	40(%edx), %mm5
	movq	48(%edx), %mm6
	movq	56(%edx), %mm7
	movq	%mm0, 32(%edx)
	movq	%mm1, 40(%edx)
	movq	%mm2, 48(%edx)
	movq	%mm3, 56(%edx)
	movq	%mm4, 32(%eax)
	movq	%mm5, 40(%eax)
	movq	%mm6, 48(%eax)
	movq	%mm7, 56(%eax)
	incl	%ebx
	cmpl	$128, %ebx
	jnz	.salsa_ns2

/* blkxor(X, Z) */
	leal	256(%ebp), %ecx
	movq	0(%ebp), %mm0
	movq	8(%ebp), %mm1
	movq	16(%ebp), %mm2
	movq	24(%ebp), %mm3
	movq	32(%ebp), %mm4
	movq	40(%ebp), %mm5
	movq	48(%ebp), %mm6
	movq	56(%ebp), %mm7
	pxor	0(%ecx), %mm0
	pxor	8(%ecx), %mm1
	pxor	16(%ecx), %mm2
	pxor	24(%ecx), %mm3
	pxor	32(%ecx), %mm4
	pxor	40(%ecx), %mm5
	pxor	48(%ecx), %mm6
	pxor	56(%ecx), %mm7
	movq	%mm0, 0(%ebp)
	movq	%mm1, 8(%ebp)
	movq	%mm2, 16(%ebp)
	movq	%mm3, 24(%ebp)
	movq	%mm4, 32(%ebp)
	movq	%mm5, 40(%ebp)
	movq	%mm6, 48(%ebp)
	movq	%mm7, 56(%ebp)
	movq	64(%ebp), %mm0
	movq	72(%ebp), %mm1
	movq	80(%ebp), %mm2
	movq	88(%ebp), %mm3
	movq	96(%ebp), %mm4
	movq	104(%ebp), %mm5
	movq	112(%ebp), %mm6
	movq	120(%ebp), %mm7
	pxor	64(%ecx), %mm0
	pxor	72(%ecx), %mm1
	pxor	80(%ecx), %mm2
	pxor	88(%ecx), %mm3
	pxor	96(%ecx), %mm4
	pxor	104(%ecx), %mm5
	pxor	112(%ecx), %mm6
	pxor	120(%ecx), %mm7
	movq	%mm0, 64(%ebp)
	movq	%mm1, 72(%ebp)
	movq	%mm2, 80(%ebp)
	movq	%mm3, 88(%ebp)
	movq	%mm4, 96(%ebp)
	movq	%mm5, 104(%ebp)
	movq	%mm6, 112(%ebp)
	movq	%mm7, 120(%ebp)
	movq	128(%ebp), %mm0
	movq	136(%ebp), %mm1
	movq	144(%ebp), %mm2
	movq	152(%ebp), %mm3
	movq	160(%ebp), %mm4
	movq	168(%ebp), %mm5
	movq	176(%ebp), %mm6
	movq	184(%ebp), %mm7
	pxor	128(%ecx), %mm0
	pxor	136(%ecx), %mm1
	pxor	144(%ecx), %mm2
	pxor	152(%ecx), %mm3
	pxor	160(%ecx), %mm4
	pxor	168(%ecx), %mm5
	pxor	176(%ecx), %mm6
	pxor	184(%ecx), %mm7
	movq	%mm0, 128(%ebp)
	movq	%mm1, 136(%ebp)
	movq	%mm2, 144(%ebp)
	movq	%mm3, 152(%ebp)
	movq	%mm4, 160(%ebp)
	movq	%mm5, 168(%ebp)
	movq	%mm6, 176(%ebp)
	movq	%mm7, 184(%ebp)
	movq	192(%ebp), %mm0
	movq	200(%ebp), %mm1
	movq	208(%ebp), %mm2
	movq	216(%ebp), %mm3
	movq	224(%ebp), %mm4
	movq	232(%ebp), %mm5
	movq	240(%ebp), %mm6
	movq	248(%ebp), %mm7
	pxor	192(%ecx), %mm0
	pxor	200(%ecx), %mm1
	pxor	208(%ecx), %mm2
	pxor	216(%ecx), %mm3
	pxor	224(%ecx), %mm4
	pxor	232(%ecx), %mm5
	pxor	240(%ecx), %mm6
	pxor	248(%ecx), %mm7
	movq	%mm0, 192(%ebp)
	movq	%mm1, 200(%ebp)
	movq	%mm2, 208(%ebp)
	movq	%mm3, 216(%ebp)
	movq	%mm4, 224(%ebp)
	movq	%mm5, 232(%ebp)
	movq	%mm6, 240(%ebp)
	movq	%mm7, 248(%ebp)

/* FastKDF */
#if (OPT)
	movl	%esi, 0(%esp)
	movl	%ebp, 4(%esp)
	movl	%edi, 8(%esp)
	movl	$1, 12(%esp)
#if (WIN32) || (__APPLE__)
	call	_neoscrypt_fastkdf_opt
#else
	call	neoscrypt_fastkdf_opt
#endif /* (WIN32) || (__APPLE__) */
#else
	movl	%esi, 0(%esp)
	movl	$80, 4(%esp)
	movl	%ebp, 8(%esp)
	movl	$256, 12(%esp)
	movl	$32, 16(%esp)
	movl	%edi, 20(%esp)
	movl	$32, 24(%esp)
#if (WIN32) || (__APPLE__)
	call	_neoscrypt_fastkdf
#else
	call	neoscrypt_fastkdf
#endif /* (WIN32) || (__APPLE__) */
#endif /* (OPT) */

#if (WIN32)
/* free memory */
	movl	32(%esp), %eax
	movl	%eax, 0(%esp)
	call	_free
/* restore stack */
	addl	$64, %esp
#else
/* restore stack */
	movl	32(%esp), %esp
#endif
	popl	%edi
	popl	%esi
	popl	%ebp
	popl	%ebx
	emms
	ret

.neoscrypt_sse2:
/* blkcpy(Z, X) */
	leal	256(%ebp), %eax
	movdqa	0(%ebp), %xmm0
	movdqa	16(%ebp), %xmm1
	movdqa	32(%ebp), %xmm2
	movdqa	48(%ebp), %xmm3
	movdqa	64(%ebp), %xmm4
	movdqa	80(%ebp), %xmm5
	movdqa	96(%ebp), %xmm6
	movdqa	112(%ebp), %xmm7
	movdqa	%xmm0, 0(%eax)
	movdqa	%xmm1, 16(%eax)
	movdqa	%xmm2, 32(%eax)
	movdqa	%xmm3, 48(%eax)
	movdqa	%xmm4, 64(%eax)
	movdqa	%xmm5, 80(%eax)
	movdqa	%xmm6, 96(%eax)
	movdqa	%xmm7, 112(%eax)
	movdqa	128(%ebp), %xmm0
	movdqa	144(%ebp), %xmm1
	movdqa	160(%ebp), %xmm2
	movdqa	176(%ebp), %xmm3
	movdqa	192(%ebp), %xmm4
	movdqa	208(%ebp), %xmm5
	movdqa	224(%ebp), %xmm6
	movdqa	240(%ebp), %xmm7
	movdqa	%xmm0, 128(%eax)
	movdqa	%xmm1, 144(%eax)
	movdqa	%xmm2, 160(%eax)
	movdqa	%xmm3, 176(%eax)
	movdqa	%xmm4, 192(%eax)
	movdqa	%xmm5, 208(%eax)
	movdqa	%xmm6, 224(%eax)
	movdqa	%xmm7, 240(%eax)

	movl	$10, 8(%esp)

	xorl	%ebx, %ebx
.chacha_ns1_sse2:
/* blkcpy(V, Z) */
	leal	512(%ebp), %eax
	movl	%ebx, %edx
	movb	$8, %cl
	shll	%cl, %edx
	leal	256(%ebp), %ecx
	addl	%edx, %eax
	movdqa	0(%ecx), %xmm0
	movdqa	16(%ecx), %xmm1
	movdqa	32(%ecx), %xmm2
	movdqa	48(%ecx), %xmm3
	movdqa	64(%ecx), %xmm4
	movdqa	80(%ecx), %xmm5
	movdqa	96(%ecx), %xmm6
	movdqa	112(%ecx), %xmm7
	movdqa	%xmm0, 0(%eax)
	movdqa	%xmm1, 16(%eax)
	movdqa	%xmm2, 32(%eax)
	movdqa	%xmm3, 48(%eax)
	movdqa	%xmm4, 64(%eax)
	movdqa	%xmm5, 80(%eax)
	movdqa	%xmm6, 96(%eax)
	movdqa	%xmm7, 112(%eax)
	movdqa	128(%ecx), %xmm0
	movdqa	144(%ecx), %xmm1
	movdqa	160(%ecx), %xmm2
	movdqa	176(%ecx), %xmm3
	movdqa	192(%ecx), %xmm4
	movdqa	208(%ecx), %xmm5
	movdqa	224(%ecx), %xmm6
	movdqa	240(%ecx), %xmm7
	movdqa	%xmm0, 128(%eax)
	movdqa	%xmm1, 144(%eax)
	movdqa	%xmm2, 160(%eax)
	movdqa	%xmm3, 176(%eax)
	movdqa	%xmm4, 192(%eax)
	movdqa	%xmm5, 208(%eax)
	movdqa	%xmm6, 224(%eax)
	movdqa	%xmm7, 240(%eax)
/* blkmix(Z) */
	leal	256(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	448(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha_sse2
	leal	320(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	256(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha_sse2
	leal	384(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	320(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha_sse2
	leal	448(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	384(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha_sse2
	leal	320(%ebp), %eax
	leal	384(%ebp), %edx
	movdqa	0(%eax), %xmm0
	movdqa	16(%eax), %xmm1
	movdqa	32(%eax), %xmm2
	movdqa	48(%eax), %xmm3
	movdqa	0(%edx), %xmm4
	movdqa	16(%edx), %xmm5
	movdqa	32(%edx), %xmm6
	movdqa	48(%edx), %xmm7
	movdqa	%xmm0, 0(%edx)
	movdqa	%xmm1, 16(%edx)
	movdqa	%xmm2, 32(%edx)
	movdqa	%xmm3, 48(%edx)
	movdqa	%xmm4, 0(%eax)
	movdqa	%xmm5, 16(%eax)
	movdqa	%xmm6, 32(%eax)
	movdqa	%xmm7, 48(%eax)
	incl	%ebx
	cmpl	$128, %ebx
	jnz	.chacha_ns1_sse2

	xorl	%ebx, %ebx
.chacha_ns2_sse2:
/* integerify(Z) mod 128 */
	leal	256(%ebp), %eax
	leal	512(%ebp), %ecx
	movl	448(%ebp), %edx
	andl	$0x7F, %edx
	shll	$8, %edx
	addl	%edx, %ecx
/* blkxor(Z, V) */
	movdqa	0(%eax), %xmm0
	movdqa	16(%eax), %xmm1
	movdqa	32(%eax), %xmm2
	movdqa	48(%eax), %xmm3
	movdqa	64(%eax), %xmm4
	movdqa	80(%eax), %xmm5
	movdqa	96(%eax), %xmm6
	movdqa	112(%eax), %xmm7
	pxor	0(%ecx), %xmm0
	pxor	16(%ecx), %xmm1
	pxor	32(%ecx), %xmm2
	pxor	48(%ecx), %xmm3
	pxor	64(%ecx), %xmm4
	pxor	80(%ecx), %xmm5
	pxor	96(%ecx), %xmm6
	pxor	112(%ecx), %xmm7
	movdqa	%xmm0, 0(%eax)
	movdqa	%xmm1, 16(%eax)
	movdqa	%xmm2, 32(%eax)
	movdqa	%xmm3, 48(%eax)
	movdqa	%xmm4, 64(%eax)
	movdqa	%xmm5, 80(%eax)
	movdqa	%xmm6, 96(%eax)
	movdqa	%xmm7, 112(%eax)
	movdqa	128(%eax), %xmm0
	movdqa	144(%eax), %xmm1
	movdqa	160(%eax), %xmm2
	movdqa	176(%eax), %xmm3
	movdqa	192(%eax), %xmm4
	movdqa	208(%eax), %xmm5
	movdqa	224(%eax), %xmm6
	movdqa	240(%eax), %xmm7
	pxor	128(%ecx), %xmm0
	pxor	144(%ecx), %xmm1
	pxor	160(%ecx), %xmm2
	pxor	176(%ecx), %xmm3
	pxor	192(%ecx), %xmm4
	pxor	208(%ecx), %xmm5
	pxor	224(%ecx), %xmm6
	pxor	240(%ecx), %xmm7
	movdqa	%xmm0, 128(%eax)
	movdqa	%xmm1, 144(%eax)
	movdqa	%xmm2, 160(%eax)
	movdqa	%xmm3, 176(%eax)
	movdqa	%xmm4, 192(%eax)
	movdqa	%xmm5, 208(%eax)
	movdqa	%xmm6, 224(%eax)
	movdqa	%xmm7, 240(%eax)
/* blkmix(Z) */
	leal	256(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	448(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha_sse2
	leal	320(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	256(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha_sse2
	leal	384(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	320(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha_sse2
	leal	448(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	384(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_chacha_sse2
	leal	320(%ebp), %eax
	leal	384(%ebp), %edx
	movdqa	0(%eax), %xmm0
	movdqa	16(%eax), %xmm1
	movdqa	32(%eax), %xmm2
	movdqa	48(%eax), %xmm3
	movdqa	0(%edx), %xmm4
	movdqa	16(%edx), %xmm5
	movdqa	32(%edx), %xmm6
	movdqa	48(%edx), %xmm7
	movdqa	%xmm0, 0(%edx)
	movdqa	%xmm1, 16(%edx)
	movdqa	%xmm2, 32(%edx)
	movdqa	%xmm3, 48(%edx)
	movdqa	%xmm4, 0(%eax)
	movdqa	%xmm5, 16(%eax)
	movdqa	%xmm6, 32(%eax)
	movdqa	%xmm7, 48(%eax)
	incl	%ebx
	cmpl	$128, %ebx
	jnz	.chacha_ns2_sse2

	movl	%ebp, 0(%esp)
	movl	$4, 4(%esp)
	call	neoscrypt_salsa_tangle_sse2

	xorl	%ebx, %ebx
.salsa_ns1_sse2:
/* blkcpy(V, X) */
	leal	512(%ebp), %eax
	movl	%ebx, %edx
	movl	$8, %ecx
	shll	%cl, %edx
	addl	%edx, %eax
	movdqa	0(%ebp), %xmm0
	movdqa	16(%ebp), %xmm1
	movdqa	32(%ebp), %xmm2
	movdqa	48(%ebp), %xmm3
	movdqa	64(%ebp), %xmm4
	movdqa	80(%ebp), %xmm5
	movdqa	96(%ebp), %xmm6
	movdqa	112(%ebp), %xmm7
	movdqa	%xmm0, 0(%eax)
	movdqa	%xmm1, 16(%eax)
	movdqa	%xmm2, 32(%eax)
	movdqa	%xmm3, 48(%eax)
	movdqa	%xmm4, 64(%eax)
	movdqa	%xmm5, 80(%eax)
	movdqa	%xmm6, 96(%eax)
	movdqa	%xmm7, 112(%eax)
	movdqa	128(%ebp), %xmm0
	movdqa	144(%ebp), %xmm1
	movdqa	160(%ebp), %xmm2
	movdqa	176(%ebp), %xmm3
	movdqa	192(%ebp), %xmm4
	movdqa	208(%ebp), %xmm5
	movdqa	224(%ebp), %xmm6
	movdqa	240(%ebp), %xmm7
	movdqa	%xmm0, 128(%eax)
	movdqa	%xmm1, 144(%eax)
	movdqa	%xmm2, 160(%eax)
	movdqa	%xmm3, 176(%eax)
	movdqa	%xmm4, 192(%eax)
	movdqa	%xmm5, 208(%eax)
	movdqa	%xmm6, 224(%eax)
	movdqa	%xmm7, 240(%eax)
/* blkmix(X) */
	movl	%ebp, 0(%esp)
	leal	192(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa_sse2
	leal	64(%ebp), %eax
	movl	%eax, 0(%esp)
	movl	%ebp, 4(%esp)
	call	neoscrypt_xor_salsa_sse2
	leal	128(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	64(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa_sse2
	leal	192(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	128(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa_sse2
	leal	64(%ebp), %eax
	leal	128(%ebp), %edx
	movdqa	0(%eax), %xmm0
	movdqa	16(%eax), %xmm1
	movdqa	32(%eax), %xmm2
	movdqa	48(%eax), %xmm3
	movdqa	0(%edx), %xmm4
	movdqa	16(%edx), %xmm5
	movdqa	32(%edx), %xmm6
	movdqa	48(%edx), %xmm7
	movdqa	%xmm0, 0(%edx)
	movdqa	%xmm1, 16(%edx)
	movdqa	%xmm2, 32(%edx)
	movdqa	%xmm3, 48(%edx)
	movdqa	%xmm4, 0(%eax)
	movdqa	%xmm5, 16(%eax)
	movdqa	%xmm6, 32(%eax)
	movdqa	%xmm7, 48(%eax)
	incl	%ebx
	cmpl	$128, %ebx
	jnz	.salsa_ns1_sse2

	xorl	%ebx, %ebx
.salsa_ns2_sse2:
/* integerify(X) mod 128 */
	leal	512(%ebp), %ecx
	movl	192(%ebp), %edx
	andl	$0x7F, %edx
	shll	$8, %edx
	addl	%edx, %ecx
/* blkxor(X, V) */
	movdqa	0(%ebp), %xmm0
	movdqa	16(%ebp), %xmm1
	movdqa	32(%ebp), %xmm2
	movdqa	48(%ebp), %xmm3
	movdqa	64(%ebp), %xmm4
	movdqa	80(%ebp), %xmm5
	movdqa	96(%ebp), %xmm6
	movdqa	112(%ebp), %xmm7
	pxor	0(%ecx), %xmm0
	pxor	16(%ecx), %xmm1
	pxor	32(%ecx), %xmm2
	pxor	48(%ecx), %xmm3
	pxor	64(%ecx), %xmm4
	pxor	80(%ecx), %xmm5
	pxor	96(%ecx), %xmm6
	pxor	112(%ecx), %xmm7
	movdqa	%xmm0, 0(%ebp)
	movdqa	%xmm1, 16(%ebp)
	movdqa	%xmm2, 32(%ebp)
	movdqa	%xmm3, 48(%ebp)
	movdqa	%xmm4, 64(%ebp)
	movdqa	%xmm5, 80(%ebp)
	movdqa	%xmm6, 96(%ebp)
	movdqa	%xmm7, 112(%ebp)
	movdqa	128(%ebp), %xmm0
	movdqa	144(%ebp), %xmm1
	movdqa	160(%ebp), %xmm2
	movdqa	176(%ebp), %xmm3
	movdqa	192(%ebp), %xmm4
	movdqa	208(%ebp), %xmm5
	movdqa	224(%ebp), %xmm6
	movdqa	240(%ebp), %xmm7
	pxor	128(%ecx), %xmm0
	pxor	144(%ecx), %xmm1
	pxor	160(%ecx), %xmm2
	pxor	176(%ecx), %xmm3
	pxor	192(%ecx), %xmm4
	pxor	208(%ecx), %xmm5
	pxor	224(%ecx), %xmm6
	pxor	240(%ecx), %xmm7
	movdqa	%xmm0, 128(%ebp)
	movdqa	%xmm1, 144(%ebp)
	movdqa	%xmm2, 160(%ebp)
	movdqa	%xmm3, 176(%ebp)
	movdqa	%xmm4, 192(%ebp)
	movdqa	%xmm5, 208(%ebp)
	movdqa	%xmm6, 224(%ebp)
	movdqa	%xmm7, 240(%ebp)
/* blkmix(X) */
	movl	%ebp, 0(%esp)
	leal	192(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa_sse2
	leal	64(%ebp), %eax
	movl	%eax, 0(%esp)
	movl	%ebp, 4(%esp)
	call	neoscrypt_xor_salsa_sse2
	leal	128(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	64(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa_sse2
	leal	192(%ebp), %eax
	movl	%eax, 0(%esp)
	leal	128(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa_sse2
	leal	64(%ebp), %eax
	leal	128(%ebp), %edx
	movdqa	0(%eax), %xmm0
	movdqa	16(%eax), %xmm1
	movdqa	32(%eax), %xmm2
	movdqa	48(%eax), %xmm3
	movdqa	0(%edx), %xmm4
	movdqa	16(%edx), %xmm5
	movdqa	32(%edx), %xmm6
	movdqa	48(%edx), %xmm7
	movdqa	%xmm0, 0(%edx)
	movdqa	%xmm1, 16(%edx)
	movdqa	%xmm2, 32(%edx)
	movdqa	%xmm3, 48(%edx)
	movdqa	%xmm4, 0(%eax)
	movdqa	%xmm5, 16(%eax)
	movdqa	%xmm6, 32(%eax)
	movdqa	%xmm7, 48(%eax)
	incl	%ebx
	cmpl	$128, %ebx
	jnz	.salsa_ns2_sse2

	movl	%ebp, 0(%esp)
	movl	$4, 4(%esp)
	call	neoscrypt_salsa_tangle_sse2

/* blkxor(X, Z) */
	leal	256(%ebp), %ecx
	movdqa	0(%ebp), %xmm0
	movdqa	16(%ebp), %xmm1
	movdqa	32(%ebp), %xmm2
	movdqa	48(%ebp), %xmm3
	movdqa	64(%ebp), %xmm4
	movdqa	80(%ebp), %xmm5
	movdqa	96(%ebp), %xmm6
	movdqa	112(%ebp), %xmm7
	pxor	0(%ecx), %xmm0
	pxor	16(%ecx), %xmm1
	pxor	32(%ecx), %xmm2
	pxor	48(%ecx), %xmm3
	pxor	64(%ecx), %xmm4
	pxor	80(%ecx), %xmm5
	pxor	96(%ecx), %xmm6
	pxor	112(%ecx), %xmm7
	movdqa	%xmm0, 0(%ebp)
	movdqa	%xmm1, 16(%ebp)
	movdqa	%xmm2, 32(%ebp)
	movdqa	%xmm3, 48(%ebp)
	movdqa	%xmm4, 64(%ebp)
	movdqa	%xmm5, 80(%ebp)
	movdqa	%xmm6, 96(%ebp)
	movdqa	%xmm7, 112(%ebp)
	movdqa	128(%ebp), %xmm0
	movdqa	144(%ebp), %xmm1
	movdqa	160(%ebp), %xmm2
	movdqa	176(%ebp), %xmm3
	movdqa	192(%ebp), %xmm4
	movdqa	208(%ebp), %xmm5
	movdqa	224(%ebp), %xmm6
	movdqa	240(%ebp), %xmm7
	pxor	128(%ecx), %xmm0
	pxor	144(%ecx), %xmm1
	pxor	160(%ecx), %xmm2
	pxor	176(%ecx), %xmm3
	pxor	192(%ecx), %xmm4
	pxor	208(%ecx), %xmm5
	pxor	224(%ecx), %xmm6
	pxor	240(%ecx), %xmm7
	movdqa	%xmm0, 128(%ebp)
	movdqa	%xmm1, 144(%ebp)
	movdqa	%xmm2, 160(%ebp)
	movdqa	%xmm3, 176(%ebp)
	movdqa	%xmm4, 192(%ebp)
	movdqa	%xmm5, 208(%ebp)
	movdqa	%xmm6, 224(%ebp)
	movdqa	%xmm7, 240(%ebp)

/* FastKDF */
#if (OPT)
	movl	%esi, 0(%esp)
	movl	%ebp, 4(%esp)
	movl	%edi, 8(%esp)
	movl	$1, 12(%esp)
#if (WIN32) || (__APPLE__)
	call	_neoscrypt_fastkdf_opt
#else
	call	neoscrypt_fastkdf_opt
#endif /* (WIN32) || (__APPLE__) */
#else
	movl	%esi, 0(%esp)
	movl	$80, 4(%esp)
	movl	%ebp, 8(%esp)
	movl	$256, 12(%esp)
	movl	$32, 16(%esp)
	movl	%edi, 20(%esp)
	movl	$32, 24(%esp)
#if (WIN32) || (__APPLE__)
	call	_neoscrypt_fastkdf
#else
	call	neoscrypt_fastkdf
#endif /* (WIN32) || (__APPLE__) */
#endif /* (OPT) */

#if (WIN32)
/* free memory */
	movl	32(%esp), %eax
	movl	%eax, 0(%esp)
	call	_free
/* restore stack */
	addl	$64, %esp
#else
/* restore stack */
	movl	32(%esp), %esp
#endif
	popl	%edi
	popl	%esi
	popl	%ebp
	popl	%ebx
	ret

#if (SHA256)

.scrypt:
#if (WIN32)
/* attempt to allocate 131200 + 128 bytes of stack space fails miserably;
 * have to use malloc() and free() instead */
	subl	$64, %esp
/* allocate memory (33 pages of 4Kb each) */
	movl	$0x21000, 0(%esp)
	call	_malloc
/* save memory address */
	movl	%eax, 32(%esp)
/* align memory */
	addl	$64, %eax
	andl	$0xFFFFFFC0, %eax
/* memory base: X, Z, V */
	leal	64(%eax), %ebp
#else
/* align stack */
	movl	%esp, %eax
	andl	$0xFFFFFFC0, %esp
	subl	$0x20100, %esp
/* save unaligned stack */
	movl	%eax, 32(%esp)
/* memory base: X, Z, V */
	leal	128(%esp), %ebp
#endif /* (WIN32) */

/* PBKDF2-HMAC-SHA256 */
	movl	$80, %eax
	movl	%esi, 0(%esp)
	movl	%eax, 4(%esp)
	movl	%esi, 8(%esp)
	movl	%eax, 12(%esp)
	movl	$1, 16(%esp)
	movl	%ebp, 20(%esp)
	movl	$128, 24(%esp)
#if (WIN32) || (__APPLE__)
	call	_neoscrypt_pbkdf2_sha256
#else
	call	neoscrypt_pbkdf2_sha256
#endif

/* SSE2 switch */
	testl	$0x1000, %ebx
	jnz	.scrypt_sse2

	leal	-64(%ebp), %edx
	movl	%edx, 8(%esp)
	movl	$4, 12(%esp)

	xorl	%ebx, %ebx
.salsa_s1:
/* blkcpy(V, X) */
	leal	128(%ebp), %eax
	movl	%ebx, %edx
	movl	$7, %ecx
	shll	%cl, %edx
	addl	%edx, %eax
	movq	0(%ebp), %mm0
	movq	8(%ebp), %mm1
	movq	16(%ebp), %mm2
	movq	24(%ebp), %mm3
	movq	32(%ebp), %mm4
	movq	40(%ebp), %mm5
	movq	48(%ebp), %mm6
	movq	56(%ebp), %mm7
	movq	%mm0, 0(%eax)
	movq	%mm1, 8(%eax)
	movq	%mm2, 16(%eax)
	movq	%mm3, 24(%eax)
	movq	%mm4, 32(%eax)
	movq	%mm5, 40(%eax)
	movq	%mm6, 48(%eax)
	movq	%mm7, 56(%eax)
	movq	64(%ebp), %mm0
	movq	72(%ebp), %mm1
	movq	80(%ebp), %mm2
	movq	88(%ebp), %mm3
	movq	96(%ebp), %mm4
	movq	104(%ebp), %mm5
	movq	112(%ebp), %mm6
	movq	120(%ebp), %mm7
	movq	%mm0, 64(%eax)
	movq	%mm1, 72(%eax)
	movq	%mm2, 80(%eax)
	movq	%mm3, 88(%eax)
	movq	%mm4, 96(%eax)
	movq	%mm5, 104(%eax)
	movq	%mm6, 112(%eax)
	movq	%mm7, 120(%eax)
/* blkmix(X) */
	movl	%ebp, 0(%esp)
	leal	64(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa
	leal	64(%ebp), %eax
	movl	%eax, 0(%esp)
	movl	%ebp, 4(%esp)
	call	neoscrypt_xor_salsa
	incl	%ebx
	cmpl	$1024, %ebx
	jnz	.salsa_s1

	xorl	%ebx, %ebx
.salsa_s2:
/* integerify(X) mod 1024 */
	leal	128(%ebp), %eax
	movl	64(%ebp), %edx
	andl	$0x03FF, %edx
	shll	$7, %edx
	addl	%edx, %eax
/* blkxor(X, V) */
	movq	0(%ebp), %mm0
	movq	8(%ebp), %mm1
	movq	16(%ebp), %mm2
	movq	24(%ebp), %mm3
	movq	32(%ebp), %mm4
	movq	40(%ebp), %mm5
	movq	48(%ebp), %mm6
	movq	56(%ebp), %mm7
	pxor	0(%eax), %mm0
	pxor	8(%eax), %mm1
	pxor	16(%eax), %mm2
	pxor	24(%eax), %mm3
	pxor	32(%eax), %mm4
	pxor	40(%eax), %mm5
	pxor	48(%eax), %mm6
	pxor	56(%eax), %mm7
	movq	%mm0, 0(%ebp)
	movq	%mm1, 8(%ebp)
	movq	%mm2, 16(%ebp)
	movq	%mm3, 24(%ebp)
	movq	%mm4, 32(%ebp)
	movq	%mm5, 40(%ebp)
	movq	%mm6, 48(%ebp)
	movq	%mm7, 56(%ebp)
	movq	64(%ebp), %mm0
	movq	72(%ebp), %mm1
	movq	80(%ebp), %mm2
	movq	88(%ebp), %mm3
	movq	96(%ebp), %mm4
	movq	104(%ebp), %mm5
	movq	112(%ebp), %mm6
	movq	120(%ebp), %mm7
	pxor	64(%eax), %mm0
	pxor	72(%eax), %mm1
	pxor	80(%eax), %mm2
	pxor	88(%eax), %mm3
	pxor	96(%eax), %mm4
	pxor	104(%eax), %mm5
	pxor	112(%eax), %mm6
	pxor	120(%eax), %mm7
	movq	%mm0, 64(%ebp)
	movq	%mm1, 72(%ebp)
	movq	%mm2, 80(%ebp)
	movq	%mm3, 88(%ebp)
	movq	%mm4, 96(%ebp)
	movq	%mm5, 104(%ebp)
	movq	%mm6, 112(%ebp)
	movq	%mm7, 120(%ebp)
/* blkmix(X) */
	movl	%ebp, 0(%esp)
	leal	64(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa
	leal	64(%ebp), %eax
	movl	%eax, 0(%esp)
	movl	%ebp, 4(%esp)
	call	neoscrypt_xor_salsa
	incl	%ebx
	cmpl	$1024, %ebx
	jnz	.salsa_s2

/* PBKDF2-HMAC-SHA256 */
	movl	%esi, 0(%esp)
	movl	$80, 4(%esp)
	movl	%ebp, 8(%esp)
	movl	$128, 12(%esp)
	movl	$1, 16(%esp)
	movl	%edi, 20(%esp)
	movl	$32, 24(%esp)
#if (WIN32) || (__APPLE__)
	call	_neoscrypt_pbkdf2_sha256
#else
	call	neoscrypt_pbkdf2_sha256
#endif

#if (WIN32)
/* free memory */
	movl	32(%esp), %eax
	movl	%eax, 0(%esp)
	call	_free
/* restore stack */
	addl	$64, %esp
#else
/* restore stack */
	movl	32(%esp), %esp
#endif
	popl	%edi
	popl	%esi
	popl	%ebp
	popl	%ebx
	emms
	ret

.scrypt_sse2:
	movl	%ebp, 0(%esp)
	movl	$2, 4(%esp)
	call	neoscrypt_salsa_tangle_sse2

	movl	$4, 8(%esp)

	xorl	%ebx, %ebx
.salsa_s1_sse2:
/* blkcpy(V, X) */
	leal	128(%ebp), %eax
	movl	%ebx, %edx
	movl	$7, %ecx
	shll	%cl, %edx
	addl	%edx, %eax
	movdqa	0(%ebp), %xmm0
	movdqa	16(%ebp), %xmm1
	movdqa	32(%ebp), %xmm2
	movdqa	48(%ebp), %xmm3
	movdqa	64(%ebp), %xmm4
	movdqa	80(%ebp), %xmm5
	movdqa	96(%ebp), %xmm6
	movdqa	112(%ebp), %xmm7
	movdqa	%xmm0, 0(%eax)
	movdqa	%xmm1, 16(%eax)
	movdqa	%xmm2, 32(%eax)
	movdqa	%xmm3, 48(%eax)
	movdqa	%xmm4, 64(%eax)
	movdqa	%xmm5, 80(%eax)
	movdqa	%xmm6, 96(%eax)
	movdqa	%xmm7, 112(%eax)
/* blkmix(X) */
	movl	%ebp, 0(%esp)
	leal	64(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa_sse2
	leal	64(%ebp), %eax
	movl	%eax, 0(%esp)
	movl	%ebp, 4(%esp)
	call	neoscrypt_xor_salsa_sse2
	incl	%ebx
	cmpl	$1024, %ebx
	jnz	.salsa_s1_sse2

	xorl	%ebx, %ebx
.salsa_s2_sse2:
/* integerify(X) mod 1024 */
	leal	128(%ebp), %eax
	movl	64(%ebp), %edx
	andl	$0x03FF, %edx
	shll	$7, %edx
	addl	%edx, %eax
/* blkxor(X, V) */
	movdqa	0(%ebp), %xmm0
	movdqa	16(%ebp), %xmm1
	movdqa	32(%ebp), %xmm2
	movdqa	48(%ebp), %xmm3
	movdqa	64(%ebp), %xmm4
	movdqa	80(%ebp), %xmm5
	movdqa	96(%ebp), %xmm6
	movdqa	112(%ebp), %xmm7
	pxor	0(%eax), %xmm0
	pxor	16(%eax), %xmm1
	pxor	32(%eax), %xmm2
	pxor	48(%eax), %xmm3
	pxor	64(%eax), %xmm4
	pxor	80(%eax), %xmm5
	pxor	96(%eax), %xmm6
	pxor	112(%eax), %xmm7
	movdqa	%xmm0, 0(%ebp)
	movdqa	%xmm1, 16(%ebp)
	movdqa	%xmm2, 32(%ebp)
	movdqa	%xmm3, 48(%ebp)
	movdqa	%xmm4, 64(%ebp)
	movdqa	%xmm5, 80(%ebp)
	movdqa	%xmm6, 96(%ebp)
	movdqa	%xmm7, 112(%ebp)
/* blkmix(X) */
	movl	%ebp, 0(%esp)
	leal	64(%ebp), %edx
	movl	%edx, 4(%esp)
	call	neoscrypt_xor_salsa_sse2
	leal	64(%ebp), %eax
	movl	%eax, 0(%esp)
	movl	%ebp, 4(%esp)
	call	neoscrypt_xor_salsa_sse2
	incl	%ebx
	cmpl	$1024, %ebx
	jnz	.salsa_s2_sse2

	movl	%ebp, 0(%esp)
	movl	$2, 4(%esp)
	call	neoscrypt_salsa_tangle_sse2

/* PBKDF2-HMAC-SHA256 */
	movl	%esi, 0(%esp)
	movl	$80, 4(%esp)
	movl	%ebp, 8(%esp)
	movl	$128, 12(%esp)
	movl	$1, 16(%esp)
	movl	%edi, 20(%esp)
	movl	$32, 24(%esp)
#if (WIN32) || (__APPLE__)
	call	_neoscrypt_pbkdf2_sha256
#else
	call	neoscrypt_pbkdf2_sha256
#endif

#if (WIN32)
/* free memory */
	movl	32(%esp), %eax
	movl	%eax, 0(%esp)
	call	_free
/* restore stack */
	addl	$64, %esp
#else
/* restore stack */
	movl	32(%esp), %esp
#endif
	popl	%edi
	popl	%esi
	popl	%ebp
	popl	%ebx
	ret

#endif /* (SHA256) */

#if (MINER_4WAY)

/* neoscrypt_blkcpy(dst, src, len)
 * i386 (SSE2) block memcpy();
 * len must be a multiple of 64 bytes aligned properly */
.globl neoscrypt_blkcpy
.globl _neoscrypt_blkcpy
neoscrypt_blkcpy:
_neoscrypt_blkcpy:
	movl	4(%esp), %eax
	movl	8(%esp), %edx
	movl	12(%esp), %ecx
	shrl	$6, %ecx
.blkcpy:
	movdqa	0(%edx), %xmm0
	movdqa	16(%edx), %xmm1
	movdqa	32(%edx), %xmm2
	movdqa	48(%edx), %xmm3
	movdqa	%xmm0, 0(%eax)
	movdqa	%xmm1, 16(%eax)
	movdqa	%xmm2, 32(%eax)
	movdqa	%xmm3, 48(%eax)
	addl	$64, %edx
	addl	$64, %eax
	decl	%ecx
	jnz	.blkcpy

	ret


/* neoscrypt_blkswp(blkA, blkB, len)
 * i386 (SSE2) block swapper;
 * len must be a multiple of 64 bytes aligned properly */
.globl neoscrypt_blkswp
.globl _neoscrypt_blkswp
neoscrypt_blkswp:
_neoscrypt_blkswp:
	movl	4(%esp), %eax
	movl	8(%esp), %edx
	movl	12(%esp), %ecx
	shrl	$6, %ecx
.blkswp:
	movdqa	0(%eax), %xmm0
	movdqa	16(%eax), %xmm1
	movdqa	32(%eax), %xmm2
	movdqa	48(%eax), %xmm3
	movdqa	0(%edx), %xmm4
	movdqa	16(%edx), %xmm5
	movdqa	32(%edx), %xmm6
	movdqa	48(%edx), %xmm7
	movdqa	%xmm0, 0(%edx)
	movdqa	%xmm1, 16(%edx)
	movdqa	%xmm2, 32(%edx)
	movdqa	%xmm3, 48(%edx)
	movdqa	%xmm4, 0(%eax)
	movdqa	%xmm5, 16(%eax)
	movdqa	%xmm6, 32(%eax)
	movdqa	%xmm7, 48(%eax)
	addl	$64, %eax
	addl	$64, %edx
	decl	%ecx
	jnz	.blkswp

	ret


/* neoscrypt_blkxor(dst, src, len)
 * i386 (SSE2) block XOR engine;
 * len must be a multiple of 64 bytes aligned properly */
.globl neoscrypt_blkxor
.globl _neoscrypt_blkxor
neoscrypt_blkxor:
_neoscrypt_blkxor:
	movl	4(%esp), %eax
	movl	8(%esp), %edx
	movl	12(%esp), %ecx
	shrl	$6, %ecx
.blkxor:
	movdqa	0(%eax), %xmm0
	movdqa	16(%eax), %xmm1
	movdqa	32(%eax), %xmm2
	movdqa	48(%eax), %xmm3
	pxor	0(%edx), %xmm0
	pxor	16(%edx), %xmm1
	pxor	32(%edx), %xmm2
	pxor	48(%edx), %xmm3
	movdqa	%xmm0, 0(%eax)
	movdqa	%xmm1, 16(%eax)
	movdqa	%xmm2, 32(%eax)
	movdqa	%xmm3, 48(%eax)
	addl	$64, %eax
	addl	$64, %edx
	decl	%ecx
	jnz	.blkxor

	ret


/* neoscrypt_xor_salsa_4way(mem, xormem, workmem, rounds)
 * i386 (SSE2) Salsa20 4-way with XOR */
.globl neoscrypt_xor_salsa_4way
.globl _neoscrypt_xor_salsa_4way
neoscrypt_xor_salsa_4way:
_neoscrypt_xor_salsa_4way:
/* XOR and copy to temporary memory */
	movl	4(%esp), %eax
	movl	8(%esp), %ecx
	movl	12(%esp), %edx
	movdqa	0(%eax), %xmm0
	movdqa	16(%eax), %xmm1
	movdqa	32(%eax), %xmm2
	movdqa	48(%eax), %xmm3
	movdqa	64(%eax), %xmm4
	movdqa	80(%eax), %xmm5
	movdqa	96(%eax), %xmm6
	movdqa	112(%eax), %xmm7
	pxor	0(%ecx), %xmm0
	pxor	16(%ecx), %xmm1
	pxor	32(%ecx), %xmm2
	pxor	48(%ecx), %xmm3
	pxor	64(%ecx), %xmm4
	pxor	80(%ecx), %xmm5
	pxor	96(%ecx), %xmm6
	pxor	112(%ecx), %xmm7
	movdqa	%xmm0, 0(%eax)
	movdqa	%xmm1, 16(%eax)
	movdqa	%xmm2, 32(%eax)
	movdqa	%xmm3, 48(%eax)
	movdqa	%xmm4, 64(%eax)
	movdqa	%xmm5, 80(%eax)
	movdqa	%xmm6, 96(%eax)
	movdqa	%xmm7, 112(%eax)
	movdqa	%xmm0, 0(%edx)
	movdqa	%xmm1, 16(%edx)
	movdqa	%xmm2, 32(%edx)
	movdqa	%xmm3, 48(%edx)
	movdqa	%xmm4, 64(%edx)
	movdqa	%xmm5, 80(%edx)
	movdqa	%xmm6, 96(%edx)
	movdqa	%xmm7, 112(%edx)
	movdqa	128(%eax), %xmm0
	movdqa	144(%eax), %xmm1
	movdqa	160(%eax), %xmm2
	movdqa	176(%eax), %xmm3
	movdqa	192(%eax), %xmm4
	movdqa	208(%eax), %xmm5
	movdqa	224(%eax), %xmm6
	movdqa	240(%eax), %xmm7
	pxor	128(%ecx), %xmm0
	pxor	144(%ecx), %xmm1
	pxor	160(%ecx), %xmm2
	pxor	176(%ecx), %xmm3
	pxor	192(%ecx), %xmm4
	pxor	208(%ecx), %xmm5
	pxor	224(%ecx), %xmm6
	pxor	240(%ecx), %xmm7
	movdqa	%xmm0, 128(%eax)
	movdqa	%xmm1, 144(%eax)
	movdqa	%xmm2, 160(%eax)
	movdqa	%xmm3, 176(%eax)
	movdqa	%xmm4, 192(%eax)
	movdqa	%xmm5, 208(%eax)
	movdqa	%xmm6, 224(%eax)
	movdqa	%xmm7, 240(%eax)
	movdqa	%xmm0, 128(%edx)
	movdqa	%xmm1, 144(%edx)
	movdqa	%xmm2, 160(%edx)
	movdqa	%xmm3, 176(%edx)
	movdqa	%xmm4, 192(%edx)
	movdqa	%xmm5, 208(%edx)
	movdqa	%xmm6, 224(%edx)
	movdqa	%xmm7, 240(%edx)
/* number of double rounds */
	movl	16(%esp), %ecx
.xor_salsa_4way:
/* quarters A and B */
	movdqa	0(%edx), %xmm0		/* A: load a */
	movdqa	80(%edx), %xmm4		/* B: load a */
	paddd	192(%edx), %xmm0	/* A: t = a + d */
	paddd	16(%edx), %xmm4		/* B: t = a + d */
	movdqa	%xmm0, %xmm3		/* A: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* B: rotate t (0) */
	pslld	$7, %xmm0		/* A: rotate t (1) */
	psrld	$25, %xmm3		/* A: rotate t (2) */
	pslld	$7, %xmm4		/* B: rotate t (1) */
	psrld	$25, %xmm7		/* B: rotate t (2) */
	por	%xmm3, %xmm0		/* A: rotate t (3) */
	por	%xmm7, %xmm4		/* B: rotate t (3) */
	pxor	64(%edx), %xmm0		/* A: b = b ^ t */
	pxor	144(%edx), %xmm4	/* B: b = b ^ t */
	movdqa	%xmm0, %xmm1		/* A: copy b */
	movdqa	%xmm4, %xmm5		/* B: copy b */
	movdqa	%xmm1, 64(%edx)		/* A: store b */
	movdqa	%xmm5, 144(%edx)	/* B: store b */
	paddd	0(%edx), %xmm0		/* A: t = b + a */
	paddd	80(%edx), %xmm4		/* B: t = b + a */
	movdqa	%xmm0, %xmm3		/* A: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* B: rotate t (0) */
	pslld	$9, %xmm0		/* A: rotate t (1) */
	psrld	$23, %xmm3		/* A: rotate t (2) */
	pslld	$9, %xmm4		/* B: rotate t (1) */
	psrld	$23, %xmm7		/* B: rotate t (2) */
	por	%xmm3, %xmm0		/* A: rotate t (3) */
	por	%xmm7, %xmm4		/* B: rotate t (3) */
	pxor	128(%edx), %xmm0	/* A: c = c ^ t */
	pxor	208(%edx), %xmm4	/* B: c = c ^ t */
	movdqa	%xmm0, %xmm2		/* A: copy c */
	movdqa	%xmm4, %xmm6		/* B: copy c */
	movdqa	%xmm2, 128(%edx)	/* A: store c */
	movdqa	%xmm6, 208(%edx)	/* B: store c */
	paddd	%xmm1, %xmm0		/* A: t = c + b */
	paddd	%xmm5, %xmm4		/* B: t = c + b */
	movdqa	%xmm0, %xmm3		/* A: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* B: rotate t (0) */
	pslld	$13, %xmm0		/* A: rotate t (1) */
	psrld	$19, %xmm3		/* A: rotate t (2) */
	pslld	$13, %xmm4		/* B: rotate t (1) */
	psrld	$19, %xmm7		/* B: rotate t (2) */
	por	%xmm3, %xmm0		/* A: rotate t (3) */
	por	%xmm7, %xmm4		/* B: rotate t (3) */
	pxor	192(%edx), %xmm0	/* A: d = d ^ t */
	pxor	16(%edx), %xmm4		/* B: d = d ^ t */
	movdqa	%xmm0, 192(%edx)	/* A: store d */
	movdqa	%xmm4, 16(%edx)		/* B: store d */
	paddd	%xmm2, %xmm0		/* A: t = d + c */
	paddd	%xmm6, %xmm4		/* B: t = d + c */
	movdqa	%xmm0, %xmm3		/* A: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* B: rotate t (0) */
	pslld	$18, %xmm0		/* A: rotate t (1) */
	psrld	$14, %xmm3		/* A: rotate t (2) */
	pslld	$18, %xmm4		/* B: rotate t (1) */
	psrld	$14, %xmm7		/* B: rotate t (2) */
	por	%xmm3, %xmm0		/* A: rotate t (3) */
	por	%xmm7, %xmm4		/* B: rotate t (3) */
	pxor	0(%edx), %xmm0		/* A: a = a ^ t */
	pxor	80(%edx), %xmm4		/* B: a = a ^ t */
	movdqa	%xmm0, 0(%edx)		/* A: store a */
	movdqa	%xmm4, 80(%edx)		/* B: store a */
/* quarters C and D*/
	movdqa	160(%edx), %xmm0	/* C: load a */
	movdqa	240(%edx), %xmm4	/* D: load a */
	paddd	96(%edx), %xmm0		/* C: t = a + d */
	paddd	176(%edx), %xmm4	/* D: t = a + d */
	movdqa	%xmm0, %xmm3		/* C: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* D: rotate t (0) */
	pslld	$7, %xmm0		/* C: rotate t (1) */
	psrld	$25, %xmm3		/* C: rotate t (2) */
	pslld	$7, %xmm4		/* D: rotate t (1) */
	psrld	$25, %xmm7		/* D: rotate t (2) */
	por	%xmm3, %xmm0		/* C: rotate t (3) */
	por	%xmm7, %xmm4		/* D: rotate t (3) */
	pxor	224(%edx), %xmm0	/* C: b = b ^ t */
	pxor	48(%edx), %xmm4		/* D: b = b ^ t */
	movdqa	%xmm0, %xmm1		/* C: copy b */
	movdqa	%xmm4, %xmm5		/* D: copy b */
	movdqa	%xmm1, 224(%edx)	/* C: store b */
	movdqa	%xmm5, 48(%edx)		/* D: store b */
	paddd	160(%edx), %xmm0	/* C: t = b + a */
	paddd	240(%edx), %xmm4	/* D: t = b + a */
	movdqa	%xmm0, %xmm3		/* C: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* D: rotate t (0) */
	pslld	$9, %xmm0		/* C: rotate t (1) */
	psrld	$23, %xmm3		/* C: rotate t (2) */
	pslld	$9, %xmm4		/* D: rotate t (1) */
	psrld	$23, %xmm7		/* D: rotate t (2) */
	por	%xmm3, %xmm0		/* C: rotate t (3) */
	por	%xmm7, %xmm4		/* D: rotate t (3) */
	pxor	32(%edx), %xmm0		/* C: c = c ^ t */
	pxor	112(%edx), %xmm4	/* D: c = c ^ t */
	movdqa	%xmm0, %xmm2		/* C: copy c */
	movdqa	%xmm4, %xmm6		/* D: copy c */
	movdqa	%xmm2, 32(%edx)		/* C: store c */
	movdqa	%xmm6, 112(%edx)	/* D: store c */
	paddd	%xmm1, %xmm0		/* C: t = c + b */
	paddd	%xmm5, %xmm4		/* D: t = c + b */
	movdqa	%xmm0, %xmm3		/* C: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* D: rotate t (0) */
	pslld	$13, %xmm0		/* C: rotate t (1) */
	psrld	$19, %xmm3		/* C: rotate t (2) */
	pslld	$13, %xmm4		/* D: rotate t (1) */
	psrld	$19, %xmm7		/* D: rotate t (2) */
	por	%xmm3, %xmm0		/* C: rotate t (3) */
	por	%xmm7, %xmm4		/* D: rotate t (3) */
	pxor	96(%edx), %xmm0		/* C: d = d ^ t */
	pxor	176(%edx), %xmm4	/* D: d = d ^ t */
	movdqa	%xmm0, 96(%edx)		/* C: store d */
	movdqa	%xmm4, 176(%edx)	/* D: store d */
	paddd	%xmm2, %xmm0		/* C: t = d + c */
	paddd	%xmm6, %xmm4		/* D: t = d + c */
	movdqa	%xmm0, %xmm3		/* C: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* D: rotate t (0) */
	pslld	$18, %xmm0		/* C: rotate t (1) */
	psrld	$14, %xmm3		/* C: rotate t (2) */
	pslld	$18, %xmm4		/* D: rotate t (1) */
	psrld	$14, %xmm7		/* D: rotate t (2) */
	por	%xmm3, %xmm0		/* C: rotate t (3) */
	por	%xmm7, %xmm4		/* D: rotate t (3) */
	pxor	160(%edx), %xmm0	/* C: a = a ^ t */
	pxor	240(%edx), %xmm4	/* D: a = a ^ t */
	movdqa	%xmm0, 160(%edx)	/* C: store a */
	movdqa	%xmm4, 240(%edx)	/* D: store a */
/* quarters E and F */
	movdqa	0(%edx), %xmm0		/* E: load a */
	movdqa	80(%edx), %xmm4		/* F: load a */
	paddd	48(%edx), %xmm0		/* E: t = a + d */
	paddd	64(%edx), %xmm4		/* F: t = a + d */
	movdqa	%xmm0, %xmm3		/* E: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* F: rotate t (0) */
	pslld	$7, %xmm0		/* E: rotate t (1) */
	psrld	$25, %xmm3		/* E: rotate t (2) */
	pslld	$7, %xmm4		/* F: rotate t (1) */
	psrld	$25, %xmm7		/* F: rotate t (2) */
	por	%xmm3, %xmm0		/* E: rotate t (3) */
	por	%xmm7, %xmm4		/* F: rotate t (3) */
	pxor	16(%edx), %xmm0		/* E: b = b ^ t */
	pxor	96(%edx), %xmm4		/* F: b = b ^ t */
	movdqa	%xmm0, %xmm1		/* E: copy b */
	movdqa	%xmm4, %xmm5		/* F: copy b */
	movdqa	%xmm1, 16(%edx)		/* E: store b */
	movdqa	%xmm5, 96(%edx)		/* F: store b */
	paddd	0(%edx), %xmm0		/* E: t = b + a */
	paddd	80(%edx), %xmm4		/* F: t = b + a */
	movdqa	%xmm0, %xmm3		/* E: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* F: rotate t (0) */
	pslld	$9, %xmm0		/* E: rotate t (1) */
	psrld	$23, %xmm3		/* E: rotate t (2) */
	pslld	$9, %xmm4		/* F: rotate t (1) */
	psrld	$23, %xmm7		/* F: rotate t (2) */
	por	%xmm3, %xmm0		/* E: rotate t (3) */
	por	%xmm7, %xmm4		/* F: rotate t (3) */
	pxor	32(%edx), %xmm0		/* E: c = c ^ t */
	pxor	112(%edx), %xmm4	/* F: c = c ^ t */
	movdqa	%xmm0, %xmm2		/* E: copy c */
	movdqa	%xmm4, %xmm6		/* F: copy c */
	movdqa	%xmm2, 32(%edx)		/* E: store c */
	movdqa	%xmm6, 112(%edx)	/* F: store c */
	paddd	%xmm1, %xmm0		/* E: t = c + b */
	paddd	%xmm5, %xmm4		/* F: t = c + b */
	movdqa	%xmm0, %xmm3		/* E: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* F: rotate t (0) */
	pslld	$13, %xmm0		/* E: rotate t (1) */
	psrld	$19, %xmm3		/* E: rotate t (2) */
	pslld	$13, %xmm4		/* F: rotate t (1) */
	psrld	$19, %xmm7		/* F: rotate t (2) */
	por	%xmm3, %xmm0		/* E: rotate t (3) */
	por	%xmm7, %xmm4		/* F: rotate t (3) */
	pxor	48(%edx), %xmm0		/* E: d = d ^ t */
	pxor	64(%edx), %xmm4		/* F: d = d ^ t */
	movdqa	%xmm0, 48(%edx)		/* E: store d */
	movdqa	%xmm4, 64(%edx)		/* F: store d */
	paddd	%xmm2, %xmm0		/* E: t = d + c */
	paddd	%xmm6, %xmm4		/* F: t = d + c */
	movdqa	%xmm0, %xmm3		/* E: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* F: rotate t (0) */
	pslld	$18, %xmm0		/* E: rotate t (1) */
	psrld	$14, %xmm3		/* E: rotate t (2) */
	pslld	$18, %xmm4		/* F: rotate t (1) */
	psrld	$14, %xmm7		/* F: rotate t (2) */
	por	%xmm3, %xmm0		/* E: rotate t (3) */
	por	%xmm7, %xmm4		/* F: rotate t (3) */
	pxor	0(%edx), %xmm0		/* E: a = a ^ t */
	pxor	80(%edx), %xmm4		/* F: a = a ^ t */
	movdqa	%xmm0, 0(%edx)		/* E: store a */
	movdqa	%xmm4, 80(%edx)		/* F: store a */
/* quarters G and H */
	movdqa	160(%edx), %xmm0	/* G: load a */
	movdqa	240(%edx), %xmm4	/* H: load a */
	paddd	144(%edx), %xmm0	/* G: t = a + d */
	paddd	224(%edx), %xmm4	/* H: t = a + d */
	movdqa	%xmm0, %xmm3		/* G: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* H: rotate t (0) */
	pslld	$7, %xmm0		/* G: rotate t (1) */
	psrld	$25, %xmm3		/* G: rotate t (2) */
	pslld	$7, %xmm4		/* H: rotate t (1) */
	psrld	$25, %xmm7		/* H: rotate t (2) */
	por	%xmm3, %xmm0		/* G: rotate t (3) */
	por	%xmm7, %xmm4		/* H: rotate t (3) */
	pxor	176(%edx), %xmm0	/* G: b = b ^ t */
	pxor	192(%edx), %xmm4	/* H: b = b ^ t */
	movdqa	%xmm0, %xmm1		/* G: copy b */
	movdqa	%xmm4, %xmm5		/* H: copy b */
	movdqa	%xmm1, 176(%edx)	/* G: store b */
	movdqa	%xmm5, 192(%edx)	/* H: store b */
	paddd	160(%edx), %xmm0	/* G: t = b + a */
	paddd	240(%edx), %xmm4	/* H: t = b + a */
	movdqa	%xmm0, %xmm3		/* G: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* H: rotate t (0) */
	pslld	$9, %xmm0		/* G: rotate t (1) */
	psrld	$23, %xmm3		/* G: rotate t (2) */
	pslld	$9, %xmm4		/* H: rotate t (1) */
	psrld	$23, %xmm7		/* H: rotate t (2) */
	por	%xmm3, %xmm0		/* G: rotate t (3) */
	por	%xmm7, %xmm4		/* H: rotate t (3) */
	pxor	128(%edx), %xmm0	/* G: c = c ^ t */
	pxor	208(%edx), %xmm4	/* H: c = c ^ t */
	movdqa	%xmm0, %xmm2		/* G: copy c */
	movdqa	%xmm4, %xmm6		/* H: copy c */
	movdqa	%xmm2, 128(%edx)	/* G: store c */
	movdqa	%xmm6, 208(%edx)	/* H: store c */
	paddd	%xmm1, %xmm0		/* G: t = c + b */
	paddd	%xmm5, %xmm4		/* H: t = c + b */
	movdqa	%xmm0, %xmm3		/* G: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* H: rotate t (0) */
	pslld	$13, %xmm0		/* G: rotate t (1) */
	psrld	$19, %xmm3		/* G: rotate t (2) */
	pslld	$13, %xmm4		/* H: rotate t (1) */
	psrld	$19, %xmm7		/* H: rotate t (2) */
	por	%xmm3, %xmm0		/* G: rotate t (3) */
	por	%xmm7, %xmm4		/* H: rotate t (3) */
	pxor	144(%edx), %xmm0	/* G: d = d ^ t */
	pxor	224(%edx), %xmm4	/* H: d = d ^ t */
	movdqa	%xmm0, 144(%edx)	/* G: store d */
	movdqa	%xmm4, 224(%edx)	/* H: store d */
	paddd	%xmm2, %xmm0		/* G: t = d + c */
	paddd	%xmm6, %xmm4		/* H: t = d + c */
	movdqa	%xmm0, %xmm3		/* G: rotate t (0) */
	movdqa	%xmm4, %xmm7		/* H: rotate t (0) */
	pslld	$18, %xmm0		/* G: rotate t (1) */
	psrld	$14, %xmm3		/* G: rotate t (2) */
	pslld	$18, %xmm4		/* H: rotate t (1) */
	psrld	$14, %xmm7		/* H: rotate t (2) */
	por	%xmm3, %xmm0		/* G: rotate t (3) */
	por	%xmm7, %xmm4		/* H: rotate t (3) */
	pxor	160(%edx), %xmm0	/* G: a = a ^ t */
	pxor	240(%edx), %xmm4	/* H: a = a ^ t */
	movdqa	%xmm0, 160(%edx)	/* G: store a */
	movdqa	%xmm4, 240(%edx)	/* H: store a */
	decl	%ecx
	jnz	.xor_salsa_4way

/* write back data */
	movdqa	0(%eax), %xmm0
	movdqa	16(%eax), %xmm1
	movdqa	32(%eax), %xmm2
	movdqa	48(%eax), %xmm3
	movdqa	64(%eax), %xmm4
	movdqa	80(%eax), %xmm5
	movdqa	96(%eax), %xmm6
	movdqa	112(%eax), %xmm7
	paddd	0(%edx), %xmm0
	paddd	16(%edx), %xmm1
	paddd	32(%edx), %xmm2
	paddd	48(%edx), %xmm3
	paddd	64(%edx), %xmm4
	paddd	80(%edx), %xmm5
	paddd	96(%edx), %xmm6
	paddd	112(%edx), %xmm7
	movdqa	%xmm0, 0(%eax)
	movdqa	%xmm1, 16(%eax)
	movdqa	%xmm2, 32(%eax)
	movdqa	%xmm3, 48(%eax)
	movdqa	%xmm4, 64(%eax)
	movdqa	%xmm5, 80(%eax)
	movdqa	%xmm6, 96(%eax)
	movdqa	%xmm7, 112(%eax)
	movdqa	128(%eax), %xmm0
	movdqa	144(%eax), %xmm1
	movdqa	160(%eax), %xmm2
	movdqa	176(%eax), %xmm3
	movdqa	192(%eax), %xmm4
	movdqa	208(%eax), %xmm5
	movdqa	224(%eax), %xmm6
	movdqa	240(%eax), %xmm7
	paddd	128(%edx), %xmm0
	paddd	144(%edx), %xmm1
	paddd	160(%edx), %xmm2
	paddd	176(%edx), %xmm3
	paddd	192(%edx), %xmm4
	paddd	208(%edx), %xmm5
	paddd	224(%edx), %xmm6
	paddd	240(%edx), %xmm7
	movdqa	%xmm0, 128(%eax)
	movdqa	%xmm1, 144(%eax)
	movdqa	%xmm2, 160(%eax)
	movdqa	%xmm3, 176(%eax)
	movdqa	%xmm4, 192(%eax)
	movdqa	%xmm5, 208(%eax)
	movdqa	%xmm6, 224(%eax)
	movdqa	%xmm7, 240(%eax)

	ret


/* neoscrypt_xor_chacha_4way(mem, xormem, workmem, double_rounds)
 * i386 (SSE2) ChaCha20 4-way with XOR */
.globl neoscrypt_xor_chacha_4way
.globl _neoscrypt_xor_chacha_4way
neoscrypt_xor_chacha_4way:
_neoscrypt_xor_chacha_4way:
/* XOR and copy to temporary memory */
	movl	4(%esp), %eax
	movl	8(%esp), %ecx
	movl	12(%esp), %edx
	movdqa	0(%eax), %xmm0
	movdqa	16(%eax), %xmm1
	movdqa	32(%eax), %xmm2
	movdqa	48(%eax), %xmm3
	movdqa	64(%eax), %xmm4
	movdqa	80(%eax), %xmm5
	movdqa	96(%eax), %xmm6
	movdqa	112(%eax), %xmm7
	pxor	0(%ecx), %xmm0
	pxor	16(%ecx), %xmm1
	pxor	32(%ecx), %xmm2
	pxor	48(%ecx), %xmm3
	pxor	64(%ecx), %xmm4
	pxor	80(%ecx), %xmm5
	pxor	96(%ecx), %xmm6
	pxor	112(%ecx), %xmm7
	movdqa	%xmm0, 0(%eax)
	movdqa	%xmm1, 16(%eax)
	movdqa	%xmm2, 32(%eax)
	movdqa	%xmm3, 48(%eax)
	movdqa	%xmm4, 64(%eax)
	movdqa	%xmm5, 80(%eax)
	movdqa	%xmm6, 96(%eax)
	movdqa	%xmm7, 112(%eax)
	movdqa	%xmm0, 0(%edx)
	movdqa	%xmm1, 16(%edx)
	movdqa	%xmm2, 32(%edx)
	movdqa	%xmm3, 48(%edx)
	movdqa	%xmm4, 64(%edx)
	movdqa	%xmm5, 80(%edx)
	movdqa	%xmm6, 96(%edx)
	movdqa	%xmm7, 112(%edx)
	movdqa	128(%eax), %xmm0
	movdqa	144(%eax), %xmm1
	movdqa	160(%eax), %xmm2
	movdqa	176(%eax), %xmm3
	movdqa	192(%eax), %xmm4
	movdqa	208(%eax), %xmm5
	movdqa	224(%eax), %xmm6
	movdqa	240(%eax), %xmm7
	pxor	128(%ecx), %xmm0
	pxor	144(%ecx), %xmm1
	pxor	160(%ecx), %xmm2
	pxor	176(%ecx), %xmm3
	pxor	192(%ecx), %xmm4
	pxor	208(%ecx), %xmm5
	pxor	224(%ecx), %xmm6
	pxor	240(%ecx), %xmm7
	movdqa	%xmm0, 128(%eax)
	movdqa	%xmm1, 144(%eax)
	movdqa	%xmm2, 160(%eax)
	movdqa	%xmm3, 176(%eax)
	movdqa	%xmm4, 192(%eax)
	movdqa	%xmm5, 208(%eax)
	movdqa	%xmm6, 224(%eax)
	movdqa	%xmm7, 240(%eax)
	movdqa	%xmm0, 128(%edx)
	movdqa	%xmm1, 144(%edx)
	movdqa	%xmm2, 160(%edx)
	movdqa	%xmm3, 176(%edx)
	movdqa	%xmm4, 192(%edx)
	movdqa	%xmm5, 208(%edx)
	movdqa	%xmm6, 224(%edx)
	movdqa	%xmm7, 240(%edx)
/* number of double rounds */
	movl	16(%esp), %ecx
.xor_chacha_4way:
/* quarters A and B, loads for C */
	movdqa	0(%edx), %xmm0		/* A: load a */
	movdqa	64(%edx), %xmm1		/* A: load b */
	paddd	%xmm1, %xmm0		/* A: a = a + b */
	movdqa	192(%edx), %xmm3	/* A: load d */
	pxor	%xmm0, %xmm3		/* A: d = d ^ a */
	movdqa	128(%edx), %xmm2	/* A: load c */
	movdqa	%xmm3, %xmm7		/* A: rotate d (0) */
	movdqa	16(%edx), %xmm4		/* B: load a */
	pslld	$16, %xmm3		/* A: rotate d (1) */
	psrld	$16, %xmm7		/* A: rotate d (2) */
	movdqa	80(%edx), %xmm5		/* B: load b */
	por	%xmm7, %xmm3		/* A: rotate d (3) */
	paddd	%xmm3, %xmm2		/* A: c = c + d */
	paddd	%xmm5, %xmm4		/* B: a = a + b */
	pxor	%xmm2, %xmm1		/* A: b = b ^ c */
	movdqa	%xmm1, %xmm7		/* A: rotate b (0) */
	movdqa	208(%edx), %xmm6	/* B: load d */
	pslld	$12, %xmm1		/* A: rotate b (1) */
	psrld	$20, %xmm7		/* A: rotate b (2) */
	pxor	%xmm4, %xmm6		/* B: d = d ^ a */
	por	%xmm7, %xmm1		/* A: rotate b (3) */
	movdqa	%xmm6, %xmm7		/* B: rotate d (0) */
	paddd	%xmm1, %xmm0		/* A: a = a + b */
	pslld	$16, %xmm6		/* B: rotate d (1) */
	psrld	$16, %xmm7		/* B: rotate d (2) */
	movdqa	%xmm0, 0(%edx)		/* A: store a */
	pxor	%xmm0, %xmm3		/* A: d = d ^ a */
	por	%xmm7, %xmm6		/* B: rotate d (3) */
	movdqa	144(%edx), %xmm0	/* B: load c */
	movdqa	%xmm3, %xmm7		/* A: rotate d (0) */
	paddd	%xmm6, %xmm0		/* B: c = c + d */
	pslld	$8, %xmm3		/* A: rotate d (1) */
	psrld	$24, %xmm7		/* A: rotate d (2) */
	pxor	%xmm0, %xmm5		/* B: b = b ^ c */
	por	%xmm7, %xmm3		/* A: rotate d (3) */
	movdqa	%xmm5, %xmm7		/* B: rotate b (0) */
	movdqa	%xmm3, 192(%edx)	/* A: store d */
	paddd	%xmm3, %xmm2		/* A: c = c + d */
	pslld	$12, %xmm5		/* B: rotate b (1) */
	psrld	$20, %xmm7		/* B: rotate b (2) */
	movdqa	%xmm2, 128(%edx)	/* A: store c */
	pxor	%xmm2, %xmm1		/* A: b = b ^ c */
	por	%xmm7, %xmm5		/* B: rotate b (3) */
	movdqa	224(%edx), %xmm3	/* C: load d */
	movdqa	%xmm1, %xmm7		/* A: rotate b (0) */
	paddd	%xmm5, %xmm4		/* B: a = a + b */
	pslld	$7, %xmm1		/* A: rotate b (1) */
	psrld	$25, %xmm7		/* A: rotate b (2) */
	movdqa	%xmm4, 16(%edx)		/* B: store a */
	pxor	%xmm4, %xmm6		/* B: d = d ^ a */
	por	%xmm7, %xmm1		/* A: rotate b (3) */
	movdqa	160(%edx), %xmm2	/* C: load c */
	movdqa	%xmm6, %xmm7		/* B: rotate d (0) */
	movdqa	%xmm1, 64(%edx)		/* A: store b */
	pslld	$8, %xmm6		/* B: rotate d (1) */
	psrld	$24, %xmm7		/* B: rotate d (2) */
	movdqa	96(%edx), %xmm1		/* C: load b */
	por	%xmm7, %xmm6		/* B: rotate d (3) */
	movdqa	%xmm6, 208(%edx)	/* B: store d */
	paddd	%xmm6, %xmm0		/* B: c = c + d */
	movdqa	%xmm0, 144(%edx)	/* B: store c */
	pxor	%xmm0, %xmm5		/* B: b = b ^ c */
	movdqa	%xmm5, %xmm7		/* B: rotate b (0) */
	movdqa	32(%edx), %xmm0		/* C: load a */
	pslld	$7, %xmm5		/* B: rotate b (1) */
	psrld	$25, %xmm7		/* B: rotate b (2) */
	por	%xmm7, %xmm5		/* B: rotate b (3) */
	movdqa	%xmm5, 80(%edx)		/* B: store b */
/* quarters C and D, loads for E */
	paddd	%xmm1, %xmm0		/* C: a = a + b */
	pxor	%xmm0, %xmm3		/* C: d = d ^ a */
	movdqa	%xmm3, %xmm7		/* C: rotate d (0) */
	movdqa	48(%edx), %xmm4		/* D: load a */
	pslld	$16, %xmm3		/* C: rotate d (1) */
	psrld	$16, %xmm7		/* C: rotate d (2) */
	movdqa	112(%edx), %xmm5	/* D: load b */
	por	%xmm7, %xmm3		/* C: rotate d (3) */
	paddd	%xmm3, %xmm2		/* C: c = c + d */
	paddd	%xmm5, %xmm4		/* D: a = a + b */
	pxor	%xmm2, %xmm1		/* C: b = b ^ c */
	movdqa	%xmm1, %xmm7		/* C: rotate b (0) */
	movdqa	240(%edx), %xmm6	/* D: load d */
	pslld	$12, %xmm1		/* C: rotate b (1) */
	psrld	$20, %xmm7		/* C: rotate b (2) */
	pxor	%xmm4, %xmm6		/* D: d = d ^ a */
	por	%xmm7, %xmm1		/* C: rotate b (3) */
	movdqa	%xmm6, %xmm7		/* D: rotate d (0) */
	paddd	%xmm1, %xmm0		/* C: a = a + b */
	pslld	$16, %xmm6		/* D: rotate d (1) */
	psrld	$16, %xmm7		/* D: rotate d (2) */
	movdqa	%xmm0, 32(%edx)		/* C: store a */
	pxor	%xmm0, %xmm3		/* C: d = d ^ a */
	por	%xmm7, %xmm6		/* D: rotate d (3) */
	movdqa	176(%edx), %xmm0	/* D: load c */
	movdqa	%xmm3, %xmm7		/* C: rotate d (0) */
	paddd	%xmm6, %xmm0		/* D: c = c + d */
	pslld	$8, %xmm3		/* C: rotate d (1) */
	psrld	$24, %xmm7		/* C: rotate d (2) */
	pxor	%xmm0, %xmm5		/* D: b = b ^ c */
	por	%xmm7, %xmm3		/* C: rotate d (3) */
	movdqa	%xmm5, %xmm7		/* D: rotate b (0) */
	movdqa	%xmm3, 224(%edx)	/* C: store d */
	paddd	%xmm3, %xmm2		/* C: c = c + d */
	pslld	$12, %xmm5		/* D: rotate b (1) */
	psrld	$20, %xmm7		/* D: rotate b (2) */
	movdqa	%xmm2, 160(%edx)	/* C: store c */
	pxor	%xmm2, %xmm1		/* C: b = b ^ c */
	por	%xmm7, %xmm5		/* D: rotate b (3) */
	movdqa	%xmm1, %xmm7		/* C: rotate b (0) */
	paddd	%xmm5, %xmm4		/* D: a = a + b */
	pslld	$7, %xmm1		/* C: rotate b (1) */
	psrld	$25, %xmm7		/* C: rotate b (2) */
	movdqa	%xmm4, 48(%edx)		/* D: store a */
	pxor	%xmm4, %xmm6		/* D: d = d ^ a */
	por	%xmm7, %xmm1		/* C: rotate b (3) */
	movdqa	160(%edx), %xmm2	/* E: load c */
	movdqa	%xmm6, %xmm7		/* D: rotate d (0) */
	movdqa	%xmm1, 96(%edx)		/* C: store b */
	pslld	$8, %xmm6		/* D: rotate d (1) */
	psrld	$24, %xmm7		/* D: rotate d (2) */
	movdqa	80(%edx), %xmm1		/* E: load b */
	por	%xmm7, %xmm6		/* D: rotate d (3) */
	movdqa	%xmm6, 240(%edx)	/* D: store d */
	movdqa	%xmm6, %xmm3		/* E: load d */
	paddd	%xmm6, %xmm0		/* D: c = c + d */
	movdqa	%xmm0, 176(%edx)	/* D: store c */
	pxor	%xmm0, %xmm5		/* D: b = b ^ c */
	movdqa	%xmm5, %xmm7		/* D: rotate b (0) */
	movdqa	0(%edx), %xmm0		/* E: load a */
	pslld	$7, %xmm5		/* D: rotate b (1) */
	psrld	$25, %xmm7		/* D: rotate b (2) */
	por	%xmm7, %xmm5		/* D: rotate b (3) */
	movdqa	%xmm5, 112(%edx)	/* D: store b */
/* quarters E and F, loads for G */
	paddd	%xmm1, %xmm0		/* E: a = a + b */
	pxor	%xmm0, %xmm3		/* E: d = d ^ a */
	movdqa	%xmm3, %xmm7		/* E: rotate d (0) */
	movdqa	16(%edx), %xmm4		/* F: load a */
	pslld	$16, %xmm3		/* E: rotate d (1) */
	psrld	$16, %xmm7		/* E: rotate d (2) */
	movdqa	96(%edx), %xmm5		/* F: load b */
	por	%xmm7, %xmm3		/* E: rotate d (3) */
	paddd	%xmm3, %xmm2		/* E: c = c + d */
	paddd	%xmm5, %xmm4		/* F: a = a + b */
	pxor	%xmm2, %xmm1		/* E: b = b ^ c */
	movdqa	%xmm1, %xmm7		/* E: rotate b (0) */
	movdqa	192(%edx), %xmm6	/* F: load d */
	pslld	$12, %xmm1		/* E: rotate b (1) */
	psrld	$20, %xmm7		/* E: rotate b (2) */
	pxor	%xmm4, %xmm6		/* F: d = d ^ a */
	por	%xmm7, %xmm1		/* E: rotate b (3) */
	movdqa	%xmm6, %xmm7		/* F: rotate d (0) */
	paddd	%xmm1, %xmm0		/* E: a = a + b */
	pslld	$16, %xmm6		/* F: rotate d (1) */
	psrld	$16, %xmm7		/* F: rotate d (2) */
	movdqa	%xmm0, 0(%edx)		/* E: store a */
	pxor	%xmm0, %xmm3		/* E: d = d ^ a */
	por	%xmm7, %xmm6		/* F: rotate d (3) */
	movdqa	176(%edx), %xmm0	/* F: load c */
	movdqa	%xmm3, %xmm7		/* E: rotate d (0) */
	paddd	%xmm6, %xmm0		/* F: c = c + d */
	pslld	$8, %xmm3		/* E: rotate d (1) */
	psrld	$24, %xmm7		/* E: rotate d (2) */
	pxor	%xmm0, %xmm5		/* F: b = b ^ c */
	por	%xmm7, %xmm3		/* E: rotate d (3) */
	movdqa	%xmm5, %xmm7		/* F: rotate b (0) */
	movdqa	%xmm3, 240(%edx)	/* E: store d */
	paddd	%xmm3, %xmm2		/* E: c = c + d */
	pslld	$12, %xmm5		/* F: rotate b (1) */
	psrld	$20, %xmm7		/* F: rotate b (2) */
	movdqa	%xmm2, 160(%edx)	/* E: store c */
	pxor	%xmm2, %xmm1		/* E: b = b ^ c */
	por	%xmm7, %xmm5		/* F: rotate b (3) */
	movdqa	208(%edx), %xmm3	/* G: load d */
	movdqa	%xmm1, %xmm7		/* E: rotate b (0) */
	paddd	%xmm5, %xmm4		/* F: a = a + b */
	pslld	$7, %xmm1		/* E: rotate b (1) */
	psrld	$25, %xmm7		/* E: rotate b (2) */
	movdqa	%xmm4, 16(%edx)		/* F: store a */
	pxor	%xmm4, %xmm6		/* F: d = d ^ a */
	por	%xmm7, %xmm1		/* E: rotate b (3) */
	movdqa	128(%edx), %xmm2	/* G: load c */
	movdqa	%xmm6, %xmm7		/* F: rotate d (0) */
	movdqa	%xmm1, 80(%edx)		/* E: store b */
	pslld	$8, %xmm6		/* F: rotate d (1) */
	psrld	$24, %xmm7		/* F: rotate d (2) */
	movdqa	112(%edx), %xmm1	/* G: load b */
	por	%xmm7, %xmm6		/* F: rotate d (3) */
	movdqa	%xmm6, 192(%edx)	/* F: store d */
	paddd	%xmm6, %xmm0		/* F: c = c + d */
	movdqa	%xmm0, 176(%edx)	/* F: store c */
	pxor	%xmm0, %xmm5		/* F: b = b ^ c */
	movdqa	%xmm5, %xmm7		/* F: rotate b (0) */
	movdqa	32(%edx), %xmm0		/* G: load a */
	pslld	$7, %xmm5		/* F: rotate b (1) */
	psrld	$25, %xmm7		/* F: rotate b (2) */
	por	%xmm7, %xmm5		/* F: rotate b (3) */
	movdqa	%xmm5, 96(%edx)		/* F: store b */
/* quarters G and H */
	paddd	%xmm1, %xmm0		/* G: a = a + b */
	pxor	%xmm0, %xmm3		/* G: d = d ^ a */
	movdqa	%xmm3, %xmm7		/* G: rotate d (0) */
	movdqa	48(%edx), %xmm4		/* H: load a */
	pslld	$16, %xmm3		/* G: rotate d (1) */
	psrld	$16, %xmm7		/* G: rotate d (2) */
	movdqa	64(%edx), %xmm5		/* H: load b */
	por	%xmm7, %xmm3		/* G: rotate d (3) */
	paddd	%xmm3, %xmm2		/* G: c = c + d */
	paddd	%xmm5, %xmm4		/* H: a = a + b */
	pxor	%xmm2, %xmm1		/* G: b = b ^ c */
	movdqa	%xmm1, %xmm7		/* G: rotate b (0) */
	movdqa	224(%edx), %xmm6	/* H: load d */
	pslld	$12, %xmm1		/* G: rotate b (1) */
	psrld	$20, %xmm7		/* G: rotate b (2) */
	pxor	%xmm4, %xmm6		/* H: d = d ^ a */
	por	%xmm7, %xmm1		/* G: rotate b (3) */
	movdqa	%xmm6, %xmm7		/* H: rotate d (0) */
	paddd	%xmm1, %xmm0		/* G: a = a + b */
	pslld	$16, %xmm6		/* H: rotate d (1) */
	psrld	$16, %xmm7		/* H: rotate d (2) */
	movdqa	%xmm0, 32(%edx)		/* G: store a */
	pxor	%xmm0, %xmm3		/* G: d = d ^ a */
	por	%xmm7, %xmm6		/* H: rotate d (3) */
	movdqa	144(%edx), %xmm0	/* H: load c */
	movdqa	%xmm3, %xmm7		/* G: rotate d (0) */
	paddd	%xmm6, %xmm0		/* H: c = c + d */
	pslld	$8, %xmm3		/* G: rotate d (1) */
	psrld	$24, %xmm7		/* G: rotate d (2) */
	pxor	%xmm0, %xmm5		/* H: b = b ^ c */
	por	%xmm7, %xmm3		/* G: rotate d (3) */
	movdqa	%xmm5, %xmm7		/* H: rotate b (0) */
	movdqa	%xmm3, 208(%edx)	/* G: store d */
	paddd	%xmm3, %xmm2		/* G: c = c + d */
	pslld	$12, %xmm5		/* H: rotate b (1) */
	psrld	$20, %xmm7		/* H: rotate b (2) */
	movdqa	%xmm2, 128(%edx)	/* G: store c */
	pxor	%xmm2, %xmm1		/* G: b = b ^ c */
	por	%xmm7, %xmm5		/* H: rotate b (3) */
	movdqa	%xmm1, %xmm7		/* G: rotate b (0) */
	paddd	%xmm5, %xmm4		/* H: a = a + b */
	pslld	$7, %xmm1		/* G: rotate b (1) */
	psrld	$25, %xmm7		/* G: rotate b (2) */
	movdqa	%xmm4, 48(%edx)		/* H: store a */
	pxor	%xmm4, %xmm6		/* H: d = d ^ a */
	por	%xmm7, %xmm1		/* G: rotate b (3) */
	movdqa	%xmm6, %xmm7		/* H: rotate d (0) */
	movdqa	%xmm1, 112(%edx)	/* G: store b */
	pslld	$8, %xmm6		/* H: rotate d (1) */
	psrld	$24, %xmm7		/* H: rotate d (2) */
	por	%xmm7, %xmm6		/* H: rotate d (3) */
	movdqa	%xmm6, 224(%edx)	/* H: store d */
	paddd	%xmm6, %xmm0		/* H: c = c + d */
	movdqa	%xmm0, 144(%edx)	/* H: store c */
	pxor	%xmm0, %xmm5		/* H: b = b ^ c */
	movdqa	%xmm5, %xmm7		/* H: rotate b (0) */
	pslld	$7, %xmm5		/* H: rotate b (1) */
	psrld	$25, %xmm7		/* H: rotate b (2) */
	por	%xmm7, %xmm5		/* H: rotate b (3) */
	movdqa	%xmm5, 64(%edx)		/* H: store b */
	decl	%ecx
	jnz	.xor_chacha_4way

/* write back data */
	movdqa	0(%eax), %xmm0
	movdqa	16(%eax), %xmm1
	movdqa	32(%eax), %xmm2
	movdqa	48(%eax), %xmm3
	movdqa	64(%eax), %xmm4
	movdqa	80(%eax), %xmm5
	movdqa	96(%eax), %xmm6
	movdqa	112(%eax), %xmm7
	paddd	0(%edx), %xmm0
	paddd	16(%edx), %xmm1
	paddd	32(%edx), %xmm2
	paddd	48(%edx), %xmm3
	paddd	64(%edx), %xmm4
	paddd	80(%edx), %xmm5
	paddd	96(%edx), %xmm6
	paddd	112(%edx), %xmm7
	movdqa	%xmm0, 0(%eax)
	movdqa	%xmm1, 16(%eax)
	movdqa	%xmm2, 32(%eax)
	movdqa	%xmm3, 48(%eax)
	movdqa	%xmm4, 64(%eax)
	movdqa	%xmm5, 80(%eax)
	movdqa	%xmm6, 96(%eax)
	movdqa	%xmm7, 112(%eax)
	movdqa	128(%eax), %xmm0
	movdqa	144(%eax), %xmm1
	movdqa	160(%eax), %xmm2
	movdqa	176(%eax), %xmm3
	movdqa	192(%eax), %xmm4
	movdqa	208(%eax), %xmm5
	movdqa	224(%eax), %xmm6
	movdqa	240(%eax), %xmm7
	paddd	128(%edx), %xmm0
	paddd	144(%edx), %xmm1
	paddd	160(%edx), %xmm2
	paddd	176(%edx), %xmm3
	paddd	192(%edx), %xmm4
	paddd	208(%edx), %xmm5
	paddd	224(%edx), %xmm6
	paddd	240(%edx), %xmm7
	movdqa	%xmm0, 128(%eax)
	movdqa	%xmm1, 144(%eax)
	movdqa	%xmm2, 160(%eax)
	movdqa	%xmm3, 176(%eax)
	movdqa	%xmm4, 192(%eax)
	movdqa	%xmm5, 208(%eax)
	movdqa	%xmm6, 224(%eax)
	movdqa	%xmm7, 240(%eax)

	ret

#endif /* (MINER_4WAY) */

#endif /* (ASM) && (__i386__) */
