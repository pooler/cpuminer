;; SHA-256 for X86-64 for Linux, based off of:

; (c) Ufasoft 2011 http://ufasoft.com mailto:support@ufasoft.com
; Version 2011
; This software is Public Domain

; SHA-256 CPU SSE cruncher for Bitcoin Miner

ALIGN 32
BITS 64

%define hash rdi
%define data rsi
%define init rdx

extern g_4sha256_k

global CalcSha256_x64	
;	CalcSha256	hash(rdi), data(rsi), init(rdx)
CalcSha256_x64:	

	push	rbx

LAB_NEXT_NONCE:
	mov	r11, data
;	mov	rax, pnonce
;	mov	eax, [rax]
;	mov	[rbx+3*16], eax
;	inc	eax
;	mov	[rbx+3*16+4], eax
;	inc	eax
;	mov	[rbx+3*16+8], eax
;	inc	eax
;	mov	[rbx+3*16+12], eax

	mov	rcx, 64*4 ;rcx is # of SHA-2 rounds
	mov	rax, 16*4 ;rax is where we expand to

LAB_SHA:
	push	rcx
	lea	rcx, qword [r11+rcx*4]
	lea	r11, qword [r11+rax*4]
LAB_CALC:
	movdqa	xmm0, [r11-15*16]
	movdqa	xmm2, xmm0					; (Rotr32(w_15, 7) ^ Rotr32(w_15, 18) ^ (w_15 >> 3))
	psrld	xmm0, 3
	movdqa	xmm1, xmm0
	pslld	xmm2, 14
	psrld	xmm1, 4
	pxor	xmm0, xmm1
	pxor	xmm0, xmm2
	pslld	xmm2, 11
	psrld	xmm1, 11
	pxor	xmm0, xmm1
	pxor	xmm0, xmm2

	paddd	xmm0, [r11-16*16]

	movdqa	xmm3, [r11-2*16]
	movdqa	xmm2, xmm3					; (Rotr32(w_2, 17) ^ Rotr32(w_2, 19) ^ (w_2 >> 10))
	psrld	xmm3, 10
	movdqa	xmm1, xmm3
	pslld	xmm2, 13
	psrld	xmm1, 7
	pxor	xmm3, xmm1
	pxor	xmm3, xmm2
	pslld	xmm2, 2
	psrld	xmm1, 2
	pxor	xmm3, xmm1
	pxor	xmm3, xmm2
	paddd	xmm0, xmm3
	
	paddd	xmm0, [r11-7*16]
	movdqa	[r11], xmm0
	add	r11, 16
	cmp	r11, rcx
	jb	LAB_CALC
	pop	rcx

	mov rax, 0

; Load the init values of the message into the hash.

	movd	xmm0, dword [rdx+4*4]		; xmm0 == e
	pshufd  xmm0, xmm0, 0
	movd	xmm3, dword [rdx+3*4]		; xmm3 == d
	pshufd  xmm3, xmm3, 0
	movd	xmm4, dword [rdx+2*4]		; xmm4 == c
	pshufd  xmm4, xmm4, 0
	movd	xmm5, dword [rdx+1*4]		; xmm5 == b
	pshufd  xmm5, xmm5, 0
	movd	xmm7, dword [rdx+0*4]		; xmm7 == a
	pshufd  xmm7, xmm7, 0
	movd	xmm8, dword [rdx+5*4]		; xmm8 == f
	pshufd  xmm8, xmm8, 0
	movd	xmm9, dword [rdx+6*4]		; xmm9 == g
	pshufd  xmm9, xmm9, 0
	movd	xmm10, dword [rdx+7*4]		; xmm10 == h
	pshufd  xmm10, xmm10, 0

LAB_LOOP:

;; T t1 = h + (Rotr32(e, 6) ^ Rotr32(e, 11) ^ Rotr32(e, 25)) + ((e & f) ^ AndNot(e, g)) + Expand32<T>(g_sha256_k[j]) + w[j]

	movdqa	xmm6, [rsi+rax*4]
	paddd	xmm6, g_4sha256_k[rax*4]
	add	rax, 4

	paddd	xmm6, xmm10	; +h

	movdqa	xmm1, xmm0
	movdqa	xmm2, xmm9
	pandn	xmm1, xmm2	; ~e & g

	movdqa	xmm10, xmm2	; h = g
	movdqa	xmm2, xmm8	; f
	movdqa	xmm9, xmm2	; g = f

	pand	xmm2, xmm0	; e & f
	pxor	xmm1, xmm2	; (e & f) ^ (~e & g)
	movdqa	xmm8, xmm0	; f = e

	paddd	xmm6, xmm1	; Ch + h + w[i] + k[i]

	movdqa	xmm1, xmm0
	psrld	xmm0, 6
	movdqa	xmm2, xmm0
	pslld	xmm1, 7
	psrld	xmm2, 5
	pxor	xmm0, xmm1
	pxor	xmm0, xmm2
	pslld	xmm1, 14
	psrld	xmm2, 14
	pxor	xmm0, xmm1
	pxor	xmm0, xmm2
	pslld	xmm1, 5
	pxor	xmm0, xmm1	; Rotr32(e, 6) ^ Rotr32(e, 11) ^ Rotr32(e, 25)
	paddd	xmm6, xmm0	; xmm6 = t1

	movdqa	xmm0, xmm3	; d
	paddd	xmm0, xmm6	; e = d+t1

	movdqa	xmm1, xmm5	; =b
	movdqa	xmm3, xmm4	; d = c
	movdqa	xmm2, xmm4	; c
	pand	xmm2, xmm5	; b & c
	pand	xmm4, xmm7	; a & c
	pand	xmm1, xmm7	; a & b
	pxor	xmm1, xmm4
	movdqa	xmm4, xmm5	; c = b
	movdqa	xmm5, xmm7	; b = a
	pxor	xmm1, xmm2	; (a & c) ^ (a & d) ^ (c & d)
	paddd	xmm6, xmm1	; t1 + ((a & c) ^ (a & d) ^ (c & d))
		
	movdqa	xmm2, xmm7
	psrld	xmm7, 2
	movdqa	xmm1, xmm7	
	pslld	xmm2, 10
	psrld	xmm1, 11
	pxor	xmm7, xmm2
	pxor	xmm7, xmm1
	pslld	xmm2, 9
	psrld	xmm1, 9
	pxor	xmm7, xmm2
	pxor	xmm7, xmm1
	pslld	xmm2, 11
	pxor	xmm7, xmm2
	paddd	xmm7, xmm6	; a = t1 + (Rotr32(a, 2) ^ Rotr32(a, 13) ^ Rotr32(a, 22)) + ((a & c) ^ (a & d) ^ (c & d));	

	cmp	rax, rcx
	jb	LAB_LOOP

; Finished the 64 rounds, calculate hash and save

	movd	xmm1, dword [rdx+0*4]
	pshufd  xmm1, xmm1, 0
	paddd	xmm7, xmm1

	movd	xmm1, dword [rdx+1*4]
	pshufd  xmm1, xmm1, 0
	paddd	xmm5, xmm1

	movd	xmm1, dword [rdx+2*4]
	pshufd  xmm1, xmm1, 0
	paddd	xmm4, xmm1

	movd	xmm1, dword [rdx+3*4]
	pshufd  xmm1, xmm1, 0
	paddd	xmm3, xmm1

	movd	xmm1, dword [rdx+4*4]
	pshufd  xmm1, xmm1, 0
	paddd	xmm0, xmm1

	movd	xmm1, dword [rdx+5*4]
	pshufd  xmm1, xmm1, 0
	paddd	xmm8, xmm1

	movd	xmm1, dword [rdx+6*4]
	pshufd  xmm1, xmm1, 0
	paddd	xmm9, xmm1

	movd	xmm1, dword [rdx+7*4]
	pshufd  xmm1, xmm1, 0
	paddd	xmm10, xmm1

debug_me:
	movdqa	[rdi+0*16], xmm7	
	movdqa	[rdi+1*16], xmm5	
	movdqa	[rdi+2*16], xmm4
	movdqa	[rdi+3*16], xmm3
	movdqa	[rdi+4*16], xmm0
	movdqa	[rdi+5*16], xmm8
	movdqa	[rdi+6*16], xmm9	
	movdqa	[rdi+7*16], xmm10

LAB_RET:
	pop	rbx
	ret
