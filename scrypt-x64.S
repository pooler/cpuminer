# Copyright 2011 pooler@litecoinpool.org
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

#if defined(__x86_64__)

.macro gen_salsa8_core_doubleround
	movq	72(%rsp), %r15
	leaq	(%r14, %rdx), %rbp
	roll	$7, %ebp
	xorq	%rbp, %r9
	leaq	(%rdi, %r15), %rbp
	roll	$7, %ebp
	xorq	%rbp, %r10
	leaq	(%rdx, %r9), %rbp
	roll	$9, %ebp
	xorq	%rbp, %r11
	leaq	(%r15, %r10), %rbp
	roll	$9, %ebp
	xorq	%rbp, %r13
	leaq	(%r9, %r11), %rbp
	roll	$13, %ebp
	xorq	%rbp, %r14
	leaq	(%r10, %r13), %rbp
	roll	$13, %ebp
	xorq	%rbp, %rdi
	leaq	(%r11, %r14), %rbp
	roll	$18, %ebp
	xorq	%rbp, %rdx
	leaq	(%r13, %rdi), %rbp
	roll	$18, %ebp
	xorq	%rbp, %r15
	movq	48(%rsp), %rbp
	movq	%r15, 72(%rsp)
	leaq	(%rax, %rbp), %r15
	roll	$7, %r15d
	xorq	%r15, %rbx
	leaq	(%rbp, %rbx), %r15
	roll	$9, %r15d
	xorq	%r15, %rcx
	leaq	(%rbx, %rcx), %r15
	roll	$13, %r15d
	xorq	%r15, %rax
	leaq	(%rcx, %rax), %r15
	roll	$18, %r15d
	xorq	%r15, %rbp
	movq	88(%rsp), %r15
	movq	%rbp, 48(%rsp)
	leaq	(%r12, %r15), %rbp
	roll	$7, %ebp
	xorq	%rbp, %rsi
	leaq	(%r15, %rsi), %rbp
	roll	$9, %ebp
	xorq	%rbp, %r8
	leaq	(%rsi, %r8), %rbp
	roll	$13, %ebp
	xorq	%rbp, %r12
	leaq	(%r8, %r12), %rbp
	roll	$18, %ebp
	xorq	%rbp, %r15
	movq	%r15, 88(%rsp)
	movq	72(%rsp), %r15
	leaq	(%rsi, %rdx), %rbp
	roll	$7, %ebp
	xorq	%rbp, %rdi
	leaq	(%r9, %r15), %rbp
	roll	$7, %ebp
	xorq	%rbp, %rax
	leaq	(%rdx, %rdi), %rbp
	roll	$9, %ebp
	xorq	%rbp, %rcx
	leaq	(%r15, %rax), %rbp
	roll	$9, %ebp
	xorq	%rbp, %r8
	leaq	(%rdi, %rcx), %rbp
	roll	$13, %ebp
	xorq	%rbp, %rsi
	leaq	(%rax, %r8), %rbp
	roll	$13, %ebp
	xorq	%rbp, %r9
	leaq	(%rcx, %rsi), %rbp
	roll	$18, %ebp
	xorq	%rbp, %rdx
	leaq	(%r8, %r9), %rbp
	roll	$18, %ebp
	xorq	%rbp, %r15
	movq	48(%rsp), %rbp
	movq	%r15, 72(%rsp)
	leaq	(%r10, %rbp), %r15
	roll	$7, %r15d
	xorq	%r15, %r12
	leaq	(%rbp, %r12), %r15
	roll	$9, %r15d
	xorq	%r15, %r11
	leaq	(%r12, %r11), %r15
	roll	$13, %r15d
	xorq	%r15, %r10
	leaq	(%r11, %r10), %r15
	roll	$18, %r15d
	xorq	%r15, %rbp
	movq	88(%rsp), %r15
	movq	%rbp, 48(%rsp)
	leaq	(%rbx, %r15), %rbp
	roll	$7, %ebp
	xorq	%rbp, %r14
	leaq	(%r15, %r14), %rbp
	roll	$9, %ebp
	xorq	%rbp, %r13
	leaq	(%r14, %r13), %rbp
	roll	$13, %ebp
	xorq	%rbp, %rbx
	leaq	(%r13, %rbx), %rbp
	roll	$18, %ebp
	xorq	%rbp, %r15
	movq	%r15, 88(%rsp)
.endm

	.text
	.align 32
gen_salsa8_core:
	# 0: %rdx, %rdi, %rcx, %rsi
	movq	8(%rsp), %rdi
	movq	%rdi, %rdx
	shrq	$32, %rdi
	movq	16(%rsp), %rsi
	movq	%rsi, %rcx
	shrq	$32, %rsi
	# 1: %r9, 72(%rsp), %rax, %r8
	movq	24(%rsp), %r8
	movq	%r8, %r9
	shrq	$32, %r8
	movq	%r8, 72(%rsp)
	movq	32(%rsp), %r8
	movq	%r8, %rax
	shrq	$32, %r8
	# 2: %r11, %r10, 48(%rsp), %r12
	movq	40(%rsp), %r10
	movq	%r10, %r11
	shrq	$32, %r10
	movq	48(%rsp), %r12
	#movq	%r12, %r13
	#movq	%r13, 48(%rsp)
	shrq	$32, %r12
	# 3: %r14, %r13, %rbx, 88(%rsp)
	movq	56(%rsp), %r13
	movq	%r13, %r14
	shrq	$32, %r13
	movq	64(%rsp), %r15
	movq	%r15, %rbx
	shrq	$32, %r15
	movq	%r15, 88(%rsp)
	
	gen_salsa8_core_doubleround
	gen_salsa8_core_doubleround
	gen_salsa8_core_doubleround
	gen_salsa8_core_doubleround
	
	movl	%edx, %edx
	shlq	$32, %rdi
	addq	%rdi, %rdx
	movd	%rdx, %xmm0
	
	movl	%ecx, %ecx
	shlq	$32, %rsi
	addq	%rsi, %rcx
	movd	%rcx, %xmm4
	
	movq	72(%rsp), %rdi
	movl	%r9d, %r9d
	shlq	$32, %rdi
	addq	%rdi, %r9
	movd	%r9, %xmm1
	
	movl	%eax, %eax
	shlq	$32, %r8
	addq	%r8, %rax
	movd	%rax, %xmm5
	
	movl	%r11d, %r11d
	shlq	$32, %r10
	addq	%r10, %r11
	movd	%r11, %xmm2
	
	movl	48(%rsp), %r8d
	shlq	$32, %r12
	addq	%r12, %r8
	movd	%r8, %xmm6
	
	movl	%r14d, %r14d
	shlq	$32, %r13
	addq	%r13, %r14
	movd	%r14, %xmm3
	
	movq	88(%rsp), %rdi
	movl	%ebx, %ebx
	shlq	$32, %rdi
	addq	%rdi, %rbx
	movd	%rbx, %xmm7
	
	punpcklqdq	%xmm4, %xmm0
	punpcklqdq	%xmm5, %xmm1
	punpcklqdq	%xmm6, %xmm2
	punpcklqdq	%xmm7, %xmm3
	
	#movq	%rdx, 8(%rsp)
	#movq	%rcx, 16(%rsp)
	#movq	%r9, 24(%rsp)
	#movq	%rax, 32(%rsp)
	#movq	%r11, 40(%rsp)
	#movq	%r8, 48(%rsp)
	#movq	%r14, 56(%rsp)
	#movq	%rbx, 64(%rsp)
	
	ret
	
	
	.text
	.align 32
	.globl scrypt_core
	.globl _scrypt_core
scrypt_core:
_scrypt_core:
	pushq	%rbx
	pushq	%rbp
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15
#if defined(WIN64)
	subq	$176, %rsp
	movdqa	%xmm6, 8(%rsp)
	movdqa	%xmm7, 24(%rsp)
	movdqa	%xmm8, 40(%rsp)
	movdqa	%xmm9, 56(%rsp)
	movdqa	%xmm10, 72(%rsp)
	movdqa	%xmm11, 88(%rsp)
	movdqa	%xmm12, 104(%rsp)
	movdqa	%xmm13, 120(%rsp)
	movdqa	%xmm14, 136(%rsp)
	movdqa	%xmm15, 152(%rsp)
	pushq	%rdi
	pushq	%rsi
	movq	%rcx, %rdi
	movq	%rdx, %rsi
#endif

.macro scrypt_core_cleanup
#if defined(WIN64)
	popq	%rsi
	popq	%rdi
	movdqa	8(%rsp), %xmm6
	movdqa	24(%rsp), %xmm7
	movdqa	40(%rsp), %xmm8
	movdqa	56(%rsp), %xmm9
	movdqa	72(%rsp), %xmm10
	movdqa	88(%rsp), %xmm11
	movdqa	104(%rsp), %xmm12
	movdqa	120(%rsp), %xmm13
	movdqa	136(%rsp), %xmm14
	movdqa	152(%rsp), %xmm15
	addq	$176, %rsp
#endif
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbp
	popq	%rbx
.endm
	
	# GenuineIntel processors have fast SIMD
	xorl	%eax, %eax
	cpuid
	cmpl	$0x6c65746e, %ecx
	jne gen_scrypt_core
	cmpl	$0x49656e69, %edx
	jne gen_scrypt_core
	cmpl	$0x756e6547, %ebx
	je xmm_scrypt_core
	
gen_scrypt_core:
	subq	$136, %rsp
	movdqa	0(%rdi), %xmm8
	movdqa	16(%rdi), %xmm9
	movdqa	32(%rdi), %xmm10
	movdqa	48(%rdi), %xmm11
	movdqa	64(%rdi), %xmm12
	movdqa	80(%rdi), %xmm13
	movdqa	96(%rdi), %xmm14
	movdqa	112(%rdi), %xmm15
	
	leaq	131072(%rsi), %rcx
	movq	%rdi, 104(%rsp)
	movq	%rsi, 112(%rsp)
	movq	%rcx, 120(%rsp)
gen_scrypt_core_loop1:
	movdqa	%xmm8, 0(%rsi)
	movdqa	%xmm9, 16(%rsi)
	movdqa	%xmm10, 32(%rsi)
	movdqa	%xmm11, 48(%rsi)
	movdqa	%xmm12, 64(%rsi)
	movdqa	%xmm13, 80(%rsi)
	movdqa	%xmm14, 96(%rsi)
	movdqa	%xmm15, 112(%rsi)
	
	pxor	%xmm12, %xmm8
	pxor	%xmm13, %xmm9
	pxor	%xmm14, %xmm10
	pxor	%xmm15, %xmm11
	movdqa	%xmm8, 0(%rsp)
	movdqa	%xmm9, 16(%rsp)
	movdqa	%xmm10, 32(%rsp)
	movdqa	%xmm11, 48(%rsp)
	movq	%rsi, 128(%rsp)
	call gen_salsa8_core
	paddd	%xmm0, %xmm8
	paddd	%xmm1, %xmm9
	paddd	%xmm2, %xmm10
	paddd	%xmm3, %xmm11
	
	pxor	%xmm8, %xmm12
	pxor	%xmm9, %xmm13
	pxor	%xmm10, %xmm14
	pxor	%xmm11, %xmm15
	movdqa	%xmm12, 0(%rsp)
	movdqa	%xmm13, 16(%rsp)
	movdqa	%xmm14, 32(%rsp)
	movdqa	%xmm15, 48(%rsp)
	call gen_salsa8_core
	movq	128(%rsp), %rsi
	paddd	%xmm0, %xmm12
	paddd	%xmm1, %xmm13
	paddd	%xmm2, %xmm14
	paddd	%xmm3, %xmm15
	
	addq	$128, %rsi
	movq	120(%rsp), %rcx
	cmpq	%rcx, %rsi
	jne gen_scrypt_core_loop1
	
	movq	$1024, %rcx
gen_scrypt_core_loop2:
	movq	112(%rsp), %rsi
	movd	%xmm12, %edx
	andl	$1023, %edx
	shll	$7, %edx
	movdqa	0(%rsi, %rdx), %xmm0
	movdqa	16(%rsi, %rdx), %xmm1
	movdqa	32(%rsi, %rdx), %xmm2
	movdqa	48(%rsi, %rdx), %xmm3
	movdqa	64(%rsi, %rdx), %xmm4
	movdqa	80(%rsi, %rdx), %xmm5
	movdqa	96(%rsi, %rdx), %xmm6
	movdqa	112(%rsi, %rdx), %xmm7
	pxor	%xmm0, %xmm8
	pxor	%xmm1, %xmm9
	pxor	%xmm2, %xmm10
	pxor	%xmm3, %xmm11
	pxor	%xmm4, %xmm12
	pxor	%xmm5, %xmm13
	pxor	%xmm6, %xmm14
	pxor	%xmm7, %xmm15
	
	pxor	%xmm12, %xmm8
	pxor	%xmm13, %xmm9
	pxor	%xmm14, %xmm10
	pxor	%xmm15, %xmm11
	movdqa	%xmm8, 0(%rsp)
	movdqa	%xmm9, 16(%rsp)
	movdqa	%xmm10, 32(%rsp)
	movdqa	%xmm11, 48(%rsp)
	movq	%rcx, 128(%rsp)
	call gen_salsa8_core
	paddd	%xmm0, %xmm8
	paddd	%xmm1, %xmm9
	paddd	%xmm2, %xmm10
	paddd	%xmm3, %xmm11
	
	pxor	%xmm8, %xmm12
	pxor	%xmm9, %xmm13
	pxor	%xmm10, %xmm14
	pxor	%xmm11, %xmm15
	movdqa	%xmm12, 0(%rsp)
	movdqa	%xmm13, 16(%rsp)
	movdqa	%xmm14, 32(%rsp)
	movdqa	%xmm15, 48(%rsp)
	call gen_salsa8_core
	movq	128(%rsp), %rcx
	paddd	%xmm0, %xmm12
	paddd	%xmm1, %xmm13
	paddd	%xmm2, %xmm14
	paddd	%xmm3, %xmm15
	
	subq	$1, %rcx
	ja gen_scrypt_core_loop2
	
	movq	104(%rsp), %rdi
	movdqa	%xmm8, 0(%rdi)
	movdqa	%xmm9, 16(%rdi)
	movdqa	%xmm10, 32(%rdi)
	movdqa	%xmm11, 48(%rdi)
	movdqa	%xmm12, 64(%rdi)
	movdqa	%xmm13, 80(%rdi)
	movdqa	%xmm14, 96(%rdi)
	movdqa	%xmm15, 112(%rdi)
	
	addq	$136, %rsp
	scrypt_core_cleanup
	ret


.macro xmm_salsa8_core_doubleround
	paddd	%xmm0, %xmm4
	movdqa	%xmm0, %xmm5
	movdqa	%xmm4, %xmm6
	pslld	$7, %xmm4
	psrld	$25, %xmm6
	pxor	%xmm4, %xmm3
	pxor	%xmm6, %xmm3
	paddd	%xmm3, %xmm5
	movdqa	%xmm3, %xmm4
	movdqa	%xmm5, %xmm6
	pslld	$9, %xmm5
	psrld	$23, %xmm6
	pxor	%xmm5, %xmm2
	pshufd	$0x93, %xmm3, %xmm3
	pxor	%xmm6, %xmm2
	paddd	%xmm2, %xmm4
	movdqa	%xmm2, %xmm5
	movdqa	%xmm4, %xmm6
	pslld	$13, %xmm4
	psrld	$19, %xmm6
	pxor	%xmm4, %xmm1
	pshufd	$0x4e, %xmm2, %xmm2
	pxor	%xmm6, %xmm1
	paddd	%xmm1, %xmm5
	movdqa	%xmm3, %xmm4
	movdqa	%xmm5, %xmm6
	pslld	$18, %xmm5
	psrld	$14, %xmm6
	pxor	%xmm5, %xmm0
	pshufd	$0x39, %xmm1, %xmm1
	pxor	%xmm6, %xmm0
	
	paddd	%xmm0, %xmm4
	movdqa	%xmm0, %xmm5
	movdqa	%xmm4, %xmm6
	pslld	$7, %xmm4
	psrld	$25, %xmm6
	pxor	%xmm4, %xmm1
	pxor	%xmm6, %xmm1
	paddd	%xmm1, %xmm5
	movdqa	%xmm1, %xmm4
	movdqa	%xmm5, %xmm6
	pslld	$9, %xmm5
	psrld	$23, %xmm6
	pxor	%xmm5, %xmm2
	pshufd	$0x93, %xmm1, %xmm1
	pxor	%xmm6, %xmm2
	paddd	%xmm2, %xmm4
	movdqa	%xmm2, %xmm5
	movdqa	%xmm4, %xmm6
	pslld	$13, %xmm4
	psrld	$19, %xmm6
	pxor	%xmm4, %xmm3
	pshufd	$0x4e, %xmm2, %xmm2
	pxor	%xmm6, %xmm3
	paddd	%xmm3, %xmm5
	movdqa	%xmm1, %xmm4
	movdqa	%xmm5, %xmm6
	pslld	$18, %xmm5
	psrld	$14, %xmm6
	pxor	%xmm5, %xmm0
	pshufd	$0x39, %xmm3, %xmm3
	pxor	%xmm6, %xmm0
.endm

.macro xmm_salsa8_core
	movdqa	%xmm1, %xmm4
	xmm_salsa8_core_doubleround
	xmm_salsa8_core_doubleround
	xmm_salsa8_core_doubleround
	xmm_salsa8_core_doubleround
.endm
	
	.align 32
xmm_scrypt_core:
	# shuffle 1st block into %xmm8-%xmm11
	movl	60(%rdi), %edx
	movl	44(%rdi), %ecx
	movl	28(%rdi), %ebx
	movl	12(%rdi), %eax
	movd	%edx, %xmm0
	movd	%ecx, %xmm1
	movd	%ebx, %xmm2
	movd	%eax, %xmm3
	movl	40(%rdi), %ecx
	movl	24(%rdi), %ebx
	movl	8(%rdi), %eax
	movl	56(%rdi), %edx
	pshufd	$0x93, %xmm0, %xmm0
	pshufd	$0x93, %xmm1, %xmm1
	pshufd	$0x93, %xmm2, %xmm2
	pshufd	$0x93, %xmm3, %xmm3
	movd	%ecx, %xmm4
	movd	%ebx, %xmm5
	movd	%eax, %xmm6
	movd	%edx, %xmm7
	paddd	%xmm4, %xmm0
	paddd	%xmm5, %xmm1
	paddd	%xmm6, %xmm2
	paddd	%xmm7, %xmm3
	movl	20(%rdi), %ebx
	movl	4(%rdi), %eax
	movl	52(%rdi), %edx
	movl	36(%rdi), %ecx
	pshufd	$0x93, %xmm0, %xmm0
	pshufd	$0x93, %xmm1, %xmm1
	pshufd	$0x93, %xmm2, %xmm2
	pshufd	$0x93, %xmm3, %xmm3
	movd	%ebx, %xmm4
	movd	%eax, %xmm5
	movd	%edx, %xmm6
	movd	%ecx, %xmm7
	paddd	%xmm4, %xmm0
	paddd	%xmm5, %xmm1
	paddd	%xmm6, %xmm2
	paddd	%xmm7, %xmm3
	movl	0(%rdi), %eax
	movl	48(%rdi), %edx
	movl	32(%rdi), %ecx
	movl	16(%rdi), %ebx
	pshufd	$0x93, %xmm0, %xmm0
	pshufd	$0x93, %xmm1, %xmm1
	pshufd	$0x93, %xmm2, %xmm2
	pshufd	$0x93, %xmm3, %xmm3
	movd	%eax, %xmm8
	movd	%edx, %xmm9
	movd	%ecx, %xmm10
	movd	%ebx, %xmm11
	paddd	%xmm0, %xmm8
	paddd	%xmm1, %xmm9
	paddd	%xmm2, %xmm10
	paddd	%xmm3, %xmm11
	
	# shuffle 2nd block into %xmm12-%xmm15
	movl	124(%rdi), %edx
	movl	108(%rdi), %ecx
	movl	92(%rdi), %ebx
	movl	76(%rdi), %eax
	movd	%edx, %xmm0
	movd	%ecx, %xmm1
	movd	%ebx, %xmm2
	movd	%eax, %xmm3
	movl	104(%rdi), %ecx
	movl	88(%rdi), %ebx
	movl	72(%rdi), %eax
	movl	120(%rdi), %edx
	pshufd	$0x93, %xmm0, %xmm0
	pshufd	$0x93, %xmm1, %xmm1
	pshufd	$0x93, %xmm2, %xmm2
	pshufd	$0x93, %xmm3, %xmm3
	movd	%ecx, %xmm4
	movd	%ebx, %xmm5
	movd	%eax, %xmm6
	movd	%edx, %xmm7
	paddd	%xmm4, %xmm0
	paddd	%xmm5, %xmm1
	paddd	%xmm6, %xmm2
	paddd	%xmm7, %xmm3
	movl	84(%rdi), %ebx
	movl	68(%rdi), %eax
	movl	116(%rdi), %edx
	movl	100(%rdi), %ecx
	pshufd	$0x93, %xmm0, %xmm0
	pshufd	$0x93, %xmm1, %xmm1
	pshufd	$0x93, %xmm2, %xmm2
	pshufd	$0x93, %xmm3, %xmm3
	movd	%ebx, %xmm4
	movd	%eax, %xmm5
	movd	%edx, %xmm6
	movd	%ecx, %xmm7
	paddd	%xmm4, %xmm0
	paddd	%xmm5, %xmm1
	paddd	%xmm6, %xmm2
	paddd	%xmm7, %xmm3
	movl	64(%rdi), %eax
	movl	112(%rdi), %edx
	movl	96(%rdi), %ecx
	movl	80(%rdi), %ebx
	pshufd	$0x93, %xmm0, %xmm0
	pshufd	$0x93, %xmm1, %xmm1
	pshufd	$0x93, %xmm2, %xmm2
	pshufd	$0x93, %xmm3, %xmm3
	movd	%eax, %xmm12
	movd	%edx, %xmm13
	movd	%ecx, %xmm14
	movd	%ebx, %xmm15
	paddd	%xmm0, %xmm12
	paddd	%xmm1, %xmm13
	paddd	%xmm2, %xmm14
	paddd	%xmm3, %xmm15
	
	movq	%rsi, %rdx
	leaq	131072(%rsi), %rcx
xmm_scrypt_core_loop1:
	movdqa	%xmm8, 0(%rdx)
	movdqa	%xmm9, 16(%rdx)
	movdqa	%xmm10, 32(%rdx)
	movdqa	%xmm11, 48(%rdx)
	movdqa	%xmm12, 64(%rdx)
	movdqa	%xmm13, 80(%rdx)
	movdqa	%xmm14, 96(%rdx)
	movdqa	%xmm15, 112(%rdx)
	
	pxor	%xmm12, %xmm8
	pxor	%xmm13, %xmm9
	pxor	%xmm14, %xmm10
	pxor	%xmm15, %xmm11
	movdqa	%xmm8, %xmm0
	movdqa	%xmm9, %xmm1
	movdqa	%xmm10, %xmm2
	movdqa	%xmm11, %xmm3
	xmm_salsa8_core
	paddd	%xmm0, %xmm8
	paddd	%xmm1, %xmm9
	paddd	%xmm2, %xmm10
	paddd	%xmm3, %xmm11
	
	pxor	%xmm8, %xmm12
	pxor	%xmm9, %xmm13
	pxor	%xmm10, %xmm14
	pxor	%xmm11, %xmm15
	movdqa	%xmm12, %xmm0
	movdqa	%xmm13, %xmm1
	movdqa	%xmm14, %xmm2
	movdqa	%xmm15, %xmm3
	xmm_salsa8_core
	paddd	%xmm0, %xmm12
	paddd	%xmm1, %xmm13
	paddd	%xmm2, %xmm14
	paddd	%xmm3, %xmm15
	
	addq	$128, %rdx
	cmpq	%rcx, %rdx
	jne xmm_scrypt_core_loop1
	
	movq	$1024, %rcx
xmm_scrypt_core_loop2:
	movd	%xmm12, %edx
	andl	$1023, %edx
	shll	$7, %edx
	movdqa	0(%rsi, %rdx), %xmm0
	movdqa	16(%rsi, %rdx), %xmm1
	movdqa	32(%rsi, %rdx), %xmm2
	movdqa	48(%rsi, %rdx), %xmm3
	movdqa	64(%rsi, %rdx), %xmm4
	movdqa	80(%rsi, %rdx), %xmm5
	movdqa	96(%rsi, %rdx), %xmm6
	movdqa	112(%rsi, %rdx), %xmm7
	pxor	%xmm0, %xmm8
	pxor	%xmm1, %xmm9
	pxor	%xmm2, %xmm10
	pxor	%xmm3, %xmm11
	pxor	%xmm4, %xmm12
	pxor	%xmm5, %xmm13
	pxor	%xmm6, %xmm14
	pxor	%xmm7, %xmm15
	
	pxor	%xmm12, %xmm8
	pxor	%xmm13, %xmm9
	pxor	%xmm14, %xmm10
	pxor	%xmm15, %xmm11
	movdqa	%xmm8, %xmm0
	movdqa	%xmm9, %xmm1
	movdqa	%xmm10, %xmm2
	movdqa	%xmm11, %xmm3
	xmm_salsa8_core
	paddd	%xmm0, %xmm8
	paddd	%xmm1, %xmm9
	paddd	%xmm2, %xmm10
	paddd	%xmm3, %xmm11
	
	pxor	%xmm8, %xmm12
	pxor	%xmm9, %xmm13
	pxor	%xmm10, %xmm14
	pxor	%xmm11, %xmm15
	movdqa	%xmm12, %xmm0
	movdqa	%xmm13, %xmm1
	movdqa	%xmm14, %xmm2
	movdqa	%xmm15, %xmm3
	xmm_salsa8_core
	paddd	%xmm0, %xmm12
	paddd	%xmm1, %xmm13
	paddd	%xmm2, %xmm14
	paddd	%xmm3, %xmm15
	
	subq	$1, %rcx
	ja xmm_scrypt_core_loop2
	
	# re-shuffle 1st block back
	movd	%xmm8, %eax
	movd	%xmm9, %edx
	movd	%xmm10, %ecx
	movd	%xmm11, %ebx
	pshufd	$0x39, %xmm8, %xmm8
	pshufd	$0x39, %xmm9, %xmm9
	pshufd	$0x39, %xmm10, %xmm10
	pshufd	$0x39, %xmm11, %xmm11
	movl	%eax, 0(%rdi)
	movl	%edx, 48(%rdi)
	movl	%ecx, 32(%rdi)
	movl	%ebx, 16(%rdi)
	movd	%xmm8, %ebx
	movd	%xmm9, %eax
	movd	%xmm10, %edx
	movd	%xmm11, %ecx
	pshufd	$0x39, %xmm8, %xmm8
	pshufd	$0x39, %xmm9, %xmm9
	pshufd	$0x39, %xmm10, %xmm10
	pshufd	$0x39, %xmm11, %xmm11
	movl	%ebx, 20(%rdi)
	movl	%eax, 4(%rdi)
	movl	%edx, 52(%rdi)
	movl	%ecx, 36(%rdi)
	movd	%xmm8, %ecx
	movd	%xmm9, %ebx
	movd	%xmm10, %eax
	movd	%xmm11, %edx
	pshufd	$0x39, %xmm8, %xmm8
	pshufd	$0x39, %xmm9, %xmm9
	pshufd	$0x39, %xmm10, %xmm10
	pshufd	$0x39, %xmm11, %xmm11
	movl	%ecx, 40(%rdi)
	movl	%ebx, 24(%rdi)
	movl	%eax, 8(%rdi)
	movl	%edx, 56(%rdi)
	movd	%xmm8, %edx
	movd	%xmm9, %ecx
	movd	%xmm10, %ebx
	movd	%xmm11, %eax
	movl	%edx, 60(%rdi)
	movl	%ecx, 44(%rdi)
	movl	%ebx, 28(%rdi)
	movl	%eax, 12(%rdi)
	
	# re-shuffle 2nd block back
	movd	%xmm12, %eax
	movd	%xmm13, %edx
	movd	%xmm14, %ecx
	movd	%xmm15, %ebx
	pshufd	$0x39, %xmm12, %xmm12
	pshufd	$0x39, %xmm13, %xmm13
	pshufd	$0x39, %xmm14, %xmm14
	pshufd	$0x39, %xmm15, %xmm15
	movl	%eax, 64(%rdi)
	movl	%edx, 112(%rdi)
	movl	%ecx, 96(%rdi)
	movl	%ebx, 80(%rdi)
	movd	%xmm12, %ebx
	movd	%xmm13, %eax
	movd	%xmm14, %edx
	movd	%xmm15, %ecx
	pshufd	$0x39, %xmm12, %xmm12
	pshufd	$0x39, %xmm13, %xmm13
	pshufd	$0x39, %xmm14, %xmm14
	pshufd	$0x39, %xmm15, %xmm15
	movl	%ebx, 84(%rdi)
	movl	%eax, 68(%rdi)
	movl	%edx, 116(%rdi)
	movl	%ecx, 100(%rdi)
	movd	%xmm12, %ecx
	movd	%xmm13, %ebx
	movd	%xmm14, %eax
	movd	%xmm15, %edx
	pshufd	$0x39, %xmm12, %xmm12
	pshufd	$0x39, %xmm13, %xmm13
	pshufd	$0x39, %xmm14, %xmm14
	pshufd	$0x39, %xmm15, %xmm15
	movl	%ecx, 104(%rdi)
	movl	%ebx, 88(%rdi)
	movl	%eax, 72(%rdi)
	movl	%edx, 120(%rdi)
	movd	%xmm12, %edx
	movd	%xmm13, %ecx
	movd	%xmm14, %ebx
	movd	%xmm15, %eax
	movl	%edx, 124(%rdi)
	movl	%ecx, 108(%rdi)
	movl	%ebx, 92(%rdi)
	movl	%eax, 76(%rdi)
	
	scrypt_core_cleanup
	ret


	.text
	.align 32
	.globl prefer_dual_scrypt
	.globl _prefer_dual_scrypt
prefer_dual_scrypt:
_prefer_dual_scrypt:
	pushq	%rbx
	xorq	%rax, %rax
	cpuid
	xorq	%rax, %rax
	cmpl	$0x6c65746e, %ecx
	jne prefer_dual_scrypt_false
	cmpl	$0x49656e69, %edx
	jne prefer_dual_scrypt_false
	cmpl	$0x756e6547, %ebx
	jne prefer_dual_scrypt_false
	incl	%eax
prefer_dual_scrypt_false:
	popq	%rbx
	ret
	

.macro xmm_dual_salsa8_core_doubleround
	paddd	%xmm0, %xmm4
	paddd	%xmm8, %xmm12
	movdqa	%xmm0, %xmm5
	movdqa	%xmm8, %xmm13
	movdqa	%xmm4, %xmm6
	movdqa	%xmm12, %xmm14
	pslld	$7, %xmm4
	pslld	$7, %xmm12
	psrld	$25, %xmm6
	psrld	$25, %xmm14
	pxor	%xmm4, %xmm3
	pxor	%xmm12, %xmm11
	pxor	%xmm6, %xmm3
	pxor	%xmm14, %xmm11
	paddd	%xmm3, %xmm5
	paddd	%xmm11, %xmm13
	movdqa	%xmm3, %xmm4
	movdqa	%xmm11, %xmm12
	movdqa	%xmm5, %xmm6
	movdqa	%xmm13, %xmm14
	pslld	$9, %xmm5
	pslld	$9, %xmm13
	psrld	$23, %xmm6
	psrld	$23, %xmm14
	pxor	%xmm5, %xmm2
	pxor	%xmm13, %xmm10
	pshufd	$0x93, %xmm3, %xmm3
	pshufd	$0x93, %xmm11, %xmm11
	pxor	%xmm6, %xmm2
	pxor	%xmm14, %xmm10
	paddd	%xmm2, %xmm4
	paddd	%xmm10, %xmm12
	movdqa	%xmm2, %xmm5
	movdqa	%xmm10, %xmm13
	movdqa	%xmm4, %xmm6
	movdqa	%xmm12, %xmm14
	pslld	$13, %xmm4
	pslld	$13, %xmm12
	psrld	$19, %xmm6
	psrld	$19, %xmm14
	pxor	%xmm4, %xmm1
	pxor	%xmm12, %xmm9
	pshufd	$0x4e, %xmm2, %xmm2
	pshufd	$0x4e, %xmm10, %xmm10
	pxor	%xmm6, %xmm1
	pxor	%xmm14, %xmm9
	paddd	%xmm1, %xmm5
	paddd	%xmm9, %xmm13
	movdqa	%xmm3, %xmm4
	movdqa	%xmm11, %xmm12
	movdqa	%xmm5, %xmm6
	movdqa	%xmm13, %xmm14
	pslld	$18, %xmm5
	pslld	$18, %xmm13
	psrld	$14, %xmm6
	psrld	$14, %xmm14
	pxor	%xmm5, %xmm0
	pxor	%xmm13, %xmm8
	pshufd	$0x39, %xmm1, %xmm1
	pshufd	$0x39, %xmm9, %xmm9
	pxor	%xmm6, %xmm0
	pxor	%xmm14, %xmm8
	
	paddd	%xmm0, %xmm4
	paddd	%xmm8, %xmm12
	movdqa	%xmm0, %xmm5
	movdqa	%xmm8, %xmm13
	movdqa	%xmm4, %xmm6
	movdqa	%xmm12, %xmm14
	pslld	$7, %xmm4
	pslld	$7, %xmm12
	psrld	$25, %xmm6
	psrld	$25, %xmm14
	pxor	%xmm4, %xmm1
	pxor	%xmm12, %xmm9
	pxor	%xmm6, %xmm1
	pxor	%xmm14, %xmm9
	paddd	%xmm1, %xmm5
	paddd	%xmm9, %xmm13
	movdqa	%xmm1, %xmm4
	movdqa	%xmm9, %xmm12
	movdqa	%xmm5, %xmm6
	movdqa	%xmm13, %xmm14
	pslld	$9, %xmm5
	pslld	$9, %xmm13
	psrld	$23, %xmm6
	psrld	$23, %xmm14
	pxor	%xmm5, %xmm2
	pxor	%xmm13, %xmm10
	pshufd	$0x93, %xmm1, %xmm1
	pshufd	$0x93, %xmm9, %xmm9
	pxor	%xmm6, %xmm2
	pxor	%xmm14, %xmm10
	paddd	%xmm2, %xmm4
	paddd	%xmm10, %xmm12
	movdqa	%xmm2, %xmm5
	movdqa	%xmm10, %xmm13
	movdqa	%xmm4, %xmm6
	movdqa	%xmm12, %xmm14
	pslld	$13, %xmm4
	pslld	$13, %xmm12
	psrld	$19, %xmm6
	psrld	$19, %xmm14
	pxor	%xmm4, %xmm3
	pxor	%xmm12, %xmm11
	pshufd	$0x4e, %xmm2, %xmm2
	pshufd	$0x4e, %xmm10, %xmm10
	pxor	%xmm6, %xmm3
	pxor	%xmm14, %xmm11
	paddd	%xmm3, %xmm5
	paddd	%xmm11, %xmm13
	movdqa	%xmm1, %xmm4
	movdqa	%xmm9, %xmm12
	movdqa	%xmm5, %xmm6
	movdqa	%xmm13, %xmm14
	pslld	$18, %xmm5
	pslld	$18, %xmm13
	psrld	$14, %xmm6
	psrld	$14, %xmm14
	pxor	%xmm5, %xmm0
	pxor	%xmm13, %xmm8
	pshufd	$0x39, %xmm3, %xmm3
	pshufd	$0x39, %xmm11, %xmm11
	pxor	%xmm6, %xmm0
	pxor	%xmm14, %xmm8
.endm

.macro xmm_dual_salsa8_core
	movdqa	%xmm1, %xmm4
	movdqa	%xmm9, %xmm12
	xmm_dual_salsa8_core_doubleround
	xmm_dual_salsa8_core_doubleround
	xmm_dual_salsa8_core_doubleround
	xmm_dual_salsa8_core_doubleround
.endm
	
	
	.text
	.align 32
	.globl dual_scrypt_core
	.globl _dual_scrypt_core
dual_scrypt_core:
_dual_scrypt_core:
	pushq	%rbx
	pushq	%rbp
#if defined(WIN64)
	subq	$176, %rsp
	movdqa	%xmm6, 8(%rsp)
	movdqa	%xmm7, 24(%rsp)
	movdqa	%xmm8, 40(%rsp)
	movdqa	%xmm9, 56(%rsp)
	movdqa	%xmm10, 72(%rsp)
	movdqa	%xmm11, 88(%rsp)
	movdqa	%xmm12, 104(%rsp)
	movdqa	%xmm13, 120(%rsp)
	movdqa	%xmm14, 136(%rsp)
	movdqa	%xmm15, 152(%rsp)
	pushq	%rdi
	pushq	%rsi
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	movq	%r8, %rdx
#endif
	
	# shuffle 1st block
	movl	60(%rdi), %ebp
	movl	44(%rdi), %ecx
	movl	28(%rdi), %ebx
	movl	12(%rdi), %eax
	movl	%ebp, 12(%rdi)
	movl	%ecx, 28(%rdi)
	movl	%ebx, 44(%rdi)
	movl	%eax, 60(%rdi)
	movl	40(%rdi), %ecx
	movl	8(%rdi), %eax
	movl	48(%rdi), %ebp
	movl	16(%rdi), %ebx
	movl	%ecx, 8(%rdi)
	movl	%eax, 40(%rdi)
	movl	%ebp, 16(%rdi)
	movl	%ebx, 48(%rdi)
	movl	20(%rdi), %ebx
	movl	4(%rdi), %eax
	movl	52(%rdi), %ebp
	movl	36(%rdi), %ecx
	movl	%ebx, 4(%rdi)
	movl	%eax, 20(%rdi)
	movl	%ebp, 36(%rdi)
	movl	%ecx, 52(%rdi)
	
	# shuffle 2nd block
	movl	124(%rdi), %ebp
	movl	108(%rdi), %ecx
	movl	92(%rdi), %ebx
	movl	76(%rdi), %eax
	movl	%ebp, 76(%rdi)
	movl	%ecx, 92(%rdi)
	movl	%ebx, 108(%rdi)
	movl	%eax, 124(%rdi)
	movl	104(%rdi), %ecx
	movl	72(%rdi), %eax
	movl	112(%rdi), %ebp
	movl	80(%rdi), %ebx
	movl	%ecx, 72(%rdi)
	movl	%eax, 104(%rdi)
	movl	%ebp, 80(%rdi)
	movl	%ebx, 112(%rdi)
	movl	84(%rdi), %ebx
	movl	68(%rdi), %eax
	movl	116(%rdi), %ebp
	movl	100(%rdi), %ecx
	movl	%ebx, 68(%rdi)
	movl	%eax, 84(%rdi)
	movl	%ebp, 100(%rdi)
	movl	%ecx, 116(%rdi)
	
	# shuffle 3rd block
	movl	60(%rsi), %ebp
	movl	44(%rsi), %ecx
	movl	28(%rsi), %ebx
	movl	12(%rsi), %eax
	movl	%ebp, 12(%rsi)
	movl	%ecx, 28(%rsi)
	movl	%ebx, 44(%rsi)
	movl	%eax, 60(%rsi)
	movl	40(%rsi), %ecx
	movl	8(%rsi), %eax
	movl	48(%rsi), %ebp
	movl	16(%rsi), %ebx
	movl	%ecx, 8(%rsi)
	movl	%eax, 40(%rsi)
	movl	%ebp, 16(%rsi)
	movl	%ebx, 48(%rsi)
	movl	20(%rsi), %ebx
	movl	4(%rsi), %eax
	movl	52(%rsi), %ebp
	movl	36(%rsi), %ecx
	movl	%ebx, 4(%rsi)
	movl	%eax, 20(%rsi)
	movl	%ebp, 36(%rsi)
	movl	%ecx, 52(%rsi)
	
	# shuffle 4th block
	movl	124(%rsi), %ebp
	movl	108(%rsi), %ecx
	movl	92(%rsi), %ebx
	movl	76(%rsi), %eax
	movl	%ebp, 76(%rsi)
	movl	%ecx, 92(%rsi)
	movl	%ebx, 108(%rsi)
	movl	%eax, 124(%rsi)
	movl	104(%rsi), %ecx
	movl	72(%rsi), %eax
	movl	112(%rsi), %ebp
	movl	80(%rsi), %ebx
	movl	%ecx, 72(%rsi)
	movl	%eax, 104(%rsi)
	movl	%ebp, 80(%rsi)
	movl	%ebx, 112(%rsi)
	movl	84(%rsi), %ebx
	movl	68(%rsi), %eax
	movl	116(%rsi), %ebp
	movl	100(%rsi), %ecx
	movl	%ebx, 68(%rsi)
	movl	%eax, 84(%rsi)
	movl	%ebp, 100(%rsi)
	movl	%ecx, 116(%rsi)
	
	movq	%rdx, %rbp
	leaq	262144(%rdx), %rcx
	.align 8
dual_scrypt_core_loop1:
	movdqa	0(%rdi), %xmm0
	movdqa	16(%rdi), %xmm1
	movdqa	32(%rdi), %xmm2
	movdqa	48(%rdi), %xmm3
	movdqa	64(%rdi), %xmm4
	movdqa	80(%rdi), %xmm5
	movdqa	96(%rdi), %xmm6
	movdqa	112(%rdi), %xmm7
	movdqa	0(%rsi), %xmm8
	movdqa	16(%rsi), %xmm9
	movdqa	32(%rsi), %xmm10
	movdqa	48(%rsi), %xmm11
	movdqa	64(%rsi), %xmm12
	movdqa	80(%rsi), %xmm13
	movdqa	96(%rsi), %xmm14
	movdqa	112(%rsi), %xmm15
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
	
	pxor	%xmm4, %xmm0
	pxor	%xmm5, %xmm1
	pxor	%xmm6, %xmm2
	pxor	%xmm7, %xmm3
	pxor	%xmm12, %xmm8
	pxor	%xmm13, %xmm9
	pxor	%xmm14, %xmm10
	pxor	%xmm15, %xmm11
	movdqa	%xmm0, 0(%rdi)
	movdqa	%xmm1, 16(%rdi)
	movdqa	%xmm2, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	movdqa	%xmm8, 0(%rsi)
	movdqa	%xmm9, 16(%rsi)
	movdqa	%xmm10, 32(%rsi)
	movdqa	%xmm11, 48(%rsi)
	xmm_dual_salsa8_core
	paddd	0(%rdi), %xmm0
	paddd	16(%rdi), %xmm1
	paddd	32(%rdi), %xmm2
	paddd	48(%rdi), %xmm3
	paddd	0(%rsi), %xmm8
	paddd	16(%rsi), %xmm9
	paddd	32(%rsi), %xmm10
	paddd	48(%rsi), %xmm11
	movdqa	%xmm0, 0(%rdi)
	movdqa	%xmm1, 16(%rdi)
	movdqa	%xmm2, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	movdqa	%xmm8, 0(%rsi)
	movdqa	%xmm9, 16(%rsi)
	movdqa	%xmm10, 32(%rsi)
	movdqa	%xmm11, 48(%rsi)
	
	pxor	64(%rdi), %xmm0
	pxor	80(%rdi), %xmm1
	pxor	96(%rdi), %xmm2
	pxor	112(%rdi), %xmm3
	pxor	64(%rsi), %xmm8
	pxor	80(%rsi), %xmm9
	pxor	96(%rsi), %xmm10
	pxor	112(%rsi), %xmm11
	movdqa	%xmm0, 64(%rdi)
	movdqa	%xmm1, 80(%rdi)
	movdqa	%xmm2, 96(%rdi)
	movdqa	%xmm3, 112(%rdi)
	movdqa	%xmm8, 64(%rsi)
	movdqa	%xmm9, 80(%rsi)
	movdqa	%xmm10, 96(%rsi)
	movdqa	%xmm11, 112(%rsi)
	xmm_dual_salsa8_core
	paddd	64(%rdi), %xmm0
	paddd	80(%rdi), %xmm1
	paddd	96(%rdi), %xmm2
	paddd	112(%rdi), %xmm3
	paddd	64(%rsi), %xmm8
	paddd	80(%rsi), %xmm9
	paddd	96(%rsi), %xmm10
	paddd	112(%rsi), %xmm11
	movdqa	%xmm0, 64(%rdi)
	movdqa	%xmm1, 80(%rdi)
	movdqa	%xmm2, 96(%rdi)
	movdqa	%xmm3, 112(%rdi)
	movdqa	%xmm8, 64(%rsi)
	movdqa	%xmm9, 80(%rsi)
	movdqa	%xmm10, 96(%rsi)
	movdqa	%xmm11, 112(%rsi)
	
	addq	$256, %rbp
	cmpq	%rcx, %rbp
	jne dual_scrypt_core_loop1
	
	movq	$1024, %rcx
	.align 8
dual_scrypt_core_loop2:
	movl	64(%rdi), %ebp
	andl	$1023, %ebp
	shll	$8, %ebp
	movdqa	0(%rdx, %rbp), %xmm0
	movdqa	16(%rdx, %rbp), %xmm1
	movdqa	32(%rdx, %rbp), %xmm2
	movdqa	48(%rdx, %rbp), %xmm3
	movdqa	64(%rdx, %rbp), %xmm4
	movdqa	80(%rdx, %rbp), %xmm5
	movdqa	96(%rdx, %rbp), %xmm6
	movdqa	112(%rdx, %rbp), %xmm7
	movl	64(%rsi), %ebp
	andl	$1023, %ebp
	shll	$8, %ebp
	addl	$128, %ebp
	movdqa	0(%rdx, %rbp), %xmm8
	movdqa	16(%rdx, %rbp), %xmm9
	movdqa	32(%rdx, %rbp), %xmm10
	movdqa	48(%rdx, %rbp), %xmm11
	movdqa	64(%rdx, %rbp), %xmm12
	movdqa	80(%rdx, %rbp), %xmm13
	movdqa	96(%rdx, %rbp), %xmm14
	movdqa	112(%rdx, %rbp), %xmm15
	pxor	0(%rdi), %xmm0
	pxor	16(%rdi), %xmm1
	pxor	32(%rdi), %xmm2
	pxor	48(%rdi), %xmm3
	pxor	64(%rdi), %xmm4
	pxor	80(%rdi), %xmm5
	pxor	96(%rdi), %xmm6
	pxor	112(%rdi), %xmm7
	pxor	0(%rsi), %xmm8
	pxor	16(%rsi), %xmm9
	pxor	32(%rsi), %xmm10
	pxor	48(%rsi), %xmm11
	pxor	64(%rsi), %xmm12
	pxor	80(%rsi), %xmm13
	pxor	96(%rsi), %xmm14
	pxor	112(%rsi), %xmm15
	
	pxor	%xmm4, %xmm0
	pxor	%xmm5, %xmm1
	pxor	%xmm6, %xmm2
	pxor	%xmm7, %xmm3
	pxor	%xmm12, %xmm8
	pxor	%xmm13, %xmm9
	pxor	%xmm14, %xmm10
	pxor	%xmm15, %xmm11
	movdqa	%xmm0, 0(%rdi)
	movdqa	%xmm1, 16(%rdi)
	movdqa	%xmm2, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	movdqa	%xmm4, 64(%rdi)
	movdqa	%xmm5, 80(%rdi)
	movdqa	%xmm6, 96(%rdi)
	movdqa	%xmm7, 112(%rdi)
	movdqa	%xmm8, 0(%rsi)
	movdqa	%xmm9, 16(%rsi)
	movdqa	%xmm10, 32(%rsi)
	movdqa	%xmm11, 48(%rsi)
	movdqa	%xmm12, 64(%rsi)
	movdqa	%xmm13, 80(%rsi)
	movdqa	%xmm14, 96(%rsi)
	movdqa	%xmm15, 112(%rsi)
	xmm_dual_salsa8_core
	paddd	0(%rdi), %xmm0
	paddd	16(%rdi), %xmm1
	paddd	32(%rdi), %xmm2
	paddd	48(%rdi), %xmm3
	paddd	0(%rsi), %xmm8
	paddd	16(%rsi), %xmm9
	paddd	32(%rsi), %xmm10
	paddd	48(%rsi), %xmm11
	movdqa	%xmm0, 0(%rdi)
	movdqa	%xmm1, 16(%rdi)
	movdqa	%xmm2, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	movdqa	%xmm8, 0(%rsi)
	movdqa	%xmm9, 16(%rsi)
	movdqa	%xmm10, 32(%rsi)
	movdqa	%xmm11, 48(%rsi)
	
	pxor	64(%rdi), %xmm0
	pxor	80(%rdi), %xmm1
	pxor	96(%rdi), %xmm2
	pxor	112(%rdi), %xmm3
	pxor	64(%rsi), %xmm8
	pxor	80(%rsi), %xmm9
	pxor	96(%rsi), %xmm10
	pxor	112(%rsi), %xmm11
	movdqa	%xmm0, 64(%rdi)
	movdqa	%xmm1, 80(%rdi)
	movdqa	%xmm2, 96(%rdi)
	movdqa	%xmm3, 112(%rdi)
	movdqa	%xmm8, 64(%rsi)
	movdqa	%xmm9, 80(%rsi)
	movdqa	%xmm10, 96(%rsi)
	movdqa	%xmm11, 112(%rsi)
	xmm_dual_salsa8_core
	paddd	64(%rdi), %xmm0
	paddd	80(%rdi), %xmm1
	paddd	96(%rdi), %xmm2
	paddd	112(%rdi), %xmm3
	paddd	64(%rsi), %xmm8
	paddd	80(%rsi), %xmm9
	paddd	96(%rsi), %xmm10
	paddd	112(%rsi), %xmm11
	movdqa	%xmm0, 64(%rdi)
	movdqa	%xmm1, 80(%rdi)
	movdqa	%xmm2, 96(%rdi)
	movdqa	%xmm3, 112(%rdi)
	movdqa	%xmm8, 64(%rsi)
	movdqa	%xmm9, 80(%rsi)
	movdqa	%xmm10, 96(%rsi)
	movdqa	%xmm11, 112(%rsi)
	
	subq	$1, %rcx
	ja dual_scrypt_core_loop2
	
	# shuffle 1st block
	movl	60(%rdi), %ebp
	movl	44(%rdi), %ecx
	movl	28(%rdi), %ebx
	movl	12(%rdi), %eax
	movl	%ebp, 12(%rdi)
	movl	%ecx, 28(%rdi)
	movl	%ebx, 44(%rdi)
	movl	%eax, 60(%rdi)
	movl	40(%rdi), %ecx
	movl	8(%rdi), %eax
	movl	48(%rdi), %ebp
	movl	16(%rdi), %ebx
	movl	%ecx, 8(%rdi)
	movl	%eax, 40(%rdi)
	movl	%ebp, 16(%rdi)
	movl	%ebx, 48(%rdi)
	movl	20(%rdi), %ebx
	movl	4(%rdi), %eax
	movl	52(%rdi), %ebp
	movl	36(%rdi), %ecx
	movl	%ebx, 4(%rdi)
	movl	%eax, 20(%rdi)
	movl	%ebp, 36(%rdi)
	movl	%ecx, 52(%rdi)
	
	# shuffle 2nd block
	movl	124(%rdi), %ebp
	movl	108(%rdi), %ecx
	movl	92(%rdi), %ebx
	movl	76(%rdi), %eax
	movl	%ebp, 76(%rdi)
	movl	%ecx, 92(%rdi)
	movl	%ebx, 108(%rdi)
	movl	%eax, 124(%rdi)
	movl	104(%rdi), %ecx
	movl	72(%rdi), %eax
	movl	112(%rdi), %ebp
	movl	80(%rdi), %ebx
	movl	%ecx, 72(%rdi)
	movl	%eax, 104(%rdi)
	movl	%ebp, 80(%rdi)
	movl	%ebx, 112(%rdi)
	movl	84(%rdi), %ebx
	movl	68(%rdi), %eax
	movl	116(%rdi), %ebp
	movl	100(%rdi), %ecx
	movl	%ebx, 68(%rdi)
	movl	%eax, 84(%rdi)
	movl	%ebp, 100(%rdi)
	movl	%ecx, 116(%rdi)
	
	# shuffle 3rd block
	movl	60(%rsi), %ebp
	movl	44(%rsi), %ecx
	movl	28(%rsi), %ebx
	movl	12(%rsi), %eax
	movl	%ebp, 12(%rsi)
	movl	%ecx, 28(%rsi)
	movl	%ebx, 44(%rsi)
	movl	%eax, 60(%rsi)
	movl	40(%rsi), %ecx
	movl	8(%rsi), %eax
	movl	48(%rsi), %ebp
	movl	16(%rsi), %ebx
	movl	%ecx, 8(%rsi)
	movl	%eax, 40(%rsi)
	movl	%ebp, 16(%rsi)
	movl	%ebx, 48(%rsi)
	movl	20(%rsi), %ebx
	movl	4(%rsi), %eax
	movl	52(%rsi), %ebp
	movl	36(%rsi), %ecx
	movl	%ebx, 4(%rsi)
	movl	%eax, 20(%rsi)
	movl	%ebp, 36(%rsi)
	movl	%ecx, 52(%rsi)
	
	# shuffle 4th block
	movl	124(%rsi), %ebp
	movl	108(%rsi), %ecx
	movl	92(%rsi), %ebx
	movl	76(%rsi), %eax
	movl	%ebp, 76(%rsi)
	movl	%ecx, 92(%rsi)
	movl	%ebx, 108(%rsi)
	movl	%eax, 124(%rsi)
	movl	104(%rsi), %ecx
	movl	72(%rsi), %eax
	movl	112(%rsi), %ebp
	movl	80(%rsi), %ebx
	movl	%ecx, 72(%rsi)
	movl	%eax, 104(%rsi)
	movl	%ebp, 80(%rsi)
	movl	%ebx, 112(%rsi)
	movl	84(%rsi), %ebx
	movl	68(%rsi), %eax
	movl	116(%rsi), %ebp
	movl	100(%rsi), %ecx
	movl	%ebx, 68(%rsi)
	movl	%eax, 84(%rsi)
	movl	%ebp, 100(%rsi)
	movl	%ecx, 116(%rsi)
	
#if defined(WIN64)
	popq	%rsi
	popq	%rdi
	movdqa	8(%rsp), %xmm6
	movdqa	24(%rsp), %xmm7
	movdqa	40(%rsp), %xmm8
	movdqa	56(%rsp), %xmm9
	movdqa	72(%rsp), %xmm10
	movdqa	88(%rsp), %xmm11
	movdqa	104(%rsp), %xmm12
	movdqa	120(%rsp), %xmm13
	movdqa	136(%rsp), %xmm14
	movdqa	152(%rsp), %xmm15
	addq	$176, %rsp
#endif
	popq	%rbp
	popq	%rbx
	ret

#endif
