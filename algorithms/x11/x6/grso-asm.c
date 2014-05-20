/* mmx optimized asm */

#include "grso-asm.h"

void grsoP1024ASM (u64 *x) {
  asm (
       "\n	movq	8(%0), %%rcx"
       "\n	movq	24(%0), %%rdx"
       "\n	movq	$0, 8(%0)"
       "\n	1:"

       "\n	movq	0(%0), %%rax"
       "\n	movq	16(%0), %%rbx"

       "\n	xorq	$0x10, %%rcx"
       "\n	xorq	$0x30, %%rdx"
       "\n	xorq	8(%0), %%rcx"
       "\n	xorq	8(%0), %%rdx"
       "\n	xorq	$0x20, %%rbx"
       "\n	xorq	8(%0), %%rax"
       "\n	xorq	8(%0), %%rbx"

       "\n	# processing input words x[1]=rcx and x[3]=rdx "
       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	movq	grsoT0(,%%rdi,8), %%mm1"
       "\n	movq	grsoT1(,%%rsi,8), %%mm0"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	movq	grsoT0(,%%rsi,8), %%mm3"
       "\n	movq	grsoT1(,%%rdi,8), %%mm2"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	movq	grsoT2(,%%rdi,8), %%r15"
       "\n	movq	grsoT3(,%%rsi,8), %%r14"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	pxor	grsoT2(,%%rsi,8), %%mm1"
       "\n	pxor	grsoT3(,%%rdi,8), %%mm0"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	movq	grsoT4(,%%rdi,8), %%r13"
       "\n	movq	grsoT5(,%%rsi,8), %%r12"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	grsoT4(,%%rsi,8), %%r15"
       "\n	xorq	grsoT5(,%%rdi,8), %%r14"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	movq	grsoT6(,%%rdi,8), %%r11"
       "\n	movq	grsoT7(,%%rsi,8), %%mm6"
       "\n	movzbl	%%dl, %%edi"
       "\n	movzbl	%%dh, %%esi"
       "\n	xorq	grsoT6(,%%rdi,8), %%r13"
       "\n	movq	grsoT7(,%%rsi,8), %%r8"



       "\n	movq	40(%0), %%rcx"
       "\n	movq	56(%0), %%rdx"

       "\n	xorq	$0x50, %%rcx"
       "\n	xorq	$0x70, %%rdx"
       "\n	xorq	8(%0), %%rcx"
       "\n	xorq	8(%0), %%rdx"


       "\n	# processing input words x[0]=rax and x[2]=rbx "
       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	pxor	grsoT0(,%%rdi,8), %%mm0"
       "\n	xorq	grsoT1(,%%rsi,8), %%r15"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	pxor	grsoT0(,%%rsi,8), %%mm2"
       "\n	pxor	grsoT1(,%%rdi,8), %%mm1"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	grsoT2(,%%rdi,8), %%r14"
       "\n	xorq	grsoT3(,%%rsi,8), %%r13"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	pxor	grsoT2(,%%rsi,8), %%mm0"
       "\n	xorq	grsoT3(,%%rdi,8), %%r15"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	grsoT4(,%%rdi,8), %%r12"
       "\n	xorq	grsoT5(,%%rsi,8), %%r11"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	grsoT4(,%%rsi,8), %%r14"
       "\n	xorq	grsoT5(,%%rdi,8), %%r13"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	movq	grsoT6(,%%rdi,8), %%r10"
       "\n	movq	grsoT7(,%%rsi,8), %%mm5"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	grsoT6(,%%rsi,8), %%r12"
       "\n	movq	grsoT7(,%%rdi,8), %%mm7"



       "\n	movq	32(%0), %%rax"
       "\n	movq	48(%0), %%rbx"

       "\n	xorq	$0x40, %%rax"
       "\n	xorq	$0x60, %%rbx"
       "\n	xorq	8(%0), %%rax"
       "\n	xorq	8(%0), %%rbx"

       "\n	# processing input words x[5]=rcx and x[7]=rdx "
       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	pxor	grsoT0(,%%rdi,8), %%mm5"
       "\n	movq	grsoT1(,%%rsi,8), %%mm4"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	pxor	grsoT0(,%%rsi,8), %%mm7"
       "\n	pxor	grsoT1(,%%rdi,8), %%mm6"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	pxor	grsoT2(,%%rdi,8), %%mm3"
       "\n	pxor	grsoT3(,%%rsi,8), %%mm2"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	pxor	grsoT2(,%%rsi,8), %%mm5"
       "\n	pxor	grsoT3(,%%rdi,8), %%mm4"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	pxor	grsoT4(,%%rdi,8), %%mm1"
       "\n	pxor	grsoT5(,%%rsi,8), %%mm0"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	pxor	grsoT4(,%%rsi,8), %%mm3"
       "\n	pxor	grsoT5(,%%rdi,8), %%mm2"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	xorq	grsoT6(,%%rdi,8), %%r15"
       "\n	xorq	grsoT7(,%%rsi,8), %%r10"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	pxor	grsoT6(,%%rsi,8), %%mm1"
       "\n	xorq	grsoT7(,%%rdi,8), %%r12"



       "\n	movq	72(%0), %%rcx"
       "\n	movq	88(%0), %%rdx"

       "\n	xorq	$0x90, %%rcx"
       "\n	xorq	$0xb0, %%rdx"
       "\n	xorq	8(%0), %%rcx"
       "\n	xorq	8(%0), %%rdx"

       "\n	# processing input words x[4]=rax and x[6]=rbx "
       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	pxor	grsoT0(,%%rdi,8), %%mm4"
       "\n	pxor	grsoT1(,%%rsi,8), %%mm3"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	pxor	grsoT0(,%%rsi,8), %%mm6"
       "\n	pxor	grsoT1(,%%rdi,8), %%mm5"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	pxor	grsoT2(,%%rdi,8), %%mm2"
       "\n	pxor	grsoT3(,%%rsi,8), %%mm1"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	pxor	grsoT2(,%%rsi,8), %%mm4"
       "\n	pxor	grsoT3(,%%rdi,8), %%mm3"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	pxor	grsoT4(,%%rdi,8), %%mm0"
       "\n	xorq	grsoT5(,%%rsi,8), %%r15"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	pxor	grsoT4(,%%rsi,8), %%mm2"
       "\n	pxor	grsoT5(,%%rdi,8), %%mm1"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	grsoT6(,%%rdi,8), %%r14"
       "\n	movq	grsoT7(,%%rsi,8), %%r9"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	pxor	grsoT6(,%%rsi,8), %%mm0"
       "\n	xorq	grsoT7(,%%rdi,8), %%r11"


       "\n	movq	64(%0), %%rax"
       "\n	movq	80(%0), %%rbx"

       "\n	xorq	$0x80, %%rax"
       "\n	xorq	$0xa0, %%rbx"
       "\n	xorq	8(%0), %%rax"
       "\n	xorq	8(%0), %%rbx"

       "\n	# processing input words x[9]=rcx and x[11]=rdx "
       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	xorq	grsoT0(,%%rdi,8), %%r9"
       "\n	xorq	grsoT1(,%%rsi,8), %%r8"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	grsoT0(,%%rsi,8), %%r11"
       "\n	xorq	grsoT1(,%%rdi,8), %%r10"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	pxor	grsoT2(,%%rdi,8), %%mm7"
       "\n	pxor	grsoT3(,%%rsi,8), %%mm6"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	grsoT2(,%%rsi,8), %%r9"
       "\n	xorq	grsoT3(,%%rdi,8), %%r8"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	pxor	grsoT4(,%%rdi,8), %%mm5"
       "\n	pxor	grsoT5(,%%rsi,8), %%mm4"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	pxor	grsoT4(,%%rsi,8), %%mm7"
       "\n	pxor	grsoT5(,%%rdi,8), %%mm6"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	pxor	grsoT6(,%%rdi,8), %%mm3"
       "\n	xorq	grsoT7(,%%rsi,8), %%r14"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	pxor	grsoT6(,%%rsi,8), %%mm5"
       "\n	pxor	grsoT7(,%%rdi,8), %%mm0"



       "\n	movq	104(%0), %%rcx"
       "\n	movq	120(%0), %%rdx"

       "\n	xorq	$0xd0, %%rcx"
       "\n	xorq	$0xf0, %%rdx"
       "\n	xorq	8(%0), %%rcx"
       "\n	xorq	8(%0), %%rdx"

       "\n	# processing input words x[8]=rax and x[10]=rbx "
       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	grsoT0(,%%rdi,8), %%r8"
       "\n	pxor	grsoT1(,%%rsi,8), %%mm7"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	grsoT0(,%%rsi,8), %%r10"
       "\n	xorq	grsoT1(,%%rdi,8), %%r9"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	pxor	grsoT2(,%%rdi,8), %%mm6"
       "\n	pxor	grsoT3(,%%rsi,8), %%mm5"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	grsoT2(,%%rsi,8), %%r8"
       "\n	pxor	grsoT3(,%%rdi,8), %%mm7"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	pxor	grsoT4(,%%rdi,8), %%mm4"
       "\n	pxor	grsoT5(,%%rsi,8), %%mm3"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	pxor	grsoT4(,%%rsi,8), %%mm6"
       "\n	pxor	grsoT5(,%%rdi,8), %%mm5"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	pxor	grsoT6(,%%rdi,8), %%mm2"
       "\n	xorq	grsoT7(,%%rsi,8), %%r13"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	pxor	grsoT6(,%%rsi,8), %%mm4"
       "\n	xorq	grsoT7(,%%rdi,8), %%r15"

       "\n	movq	96(%0), %%rax"
       "\n	movq	112(%0), %%rbx"

       "\n	xorq	$0xc0, %%rax"
       "\n	xorq	$0xe0, %%rbx"
       "\n	xorq	8(%0), %%rax"
       "\n	xorq	8(%0), %%rbx"

       "\n	# processing input words x[13]=rcx and x[15]=rdx "
       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	xorq	grsoT0(,%%rdi,8), %%r13"
       "\n	xorq	grsoT1(,%%rsi,8), %%r12"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	grsoT0(,%%rsi,8), %%r15"
       "\n	xorq	grsoT1(,%%rdi,8), %%r14"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	xorq	grsoT2(,%%rdi,8), %%r11"
       "\n	xorq	grsoT3(,%%rsi,8), %%r10"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	grsoT2(,%%rsi,8), %%r13"
       "\n	xorq	grsoT3(,%%rdi,8), %%r12"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	xorq	grsoT4(,%%rdi,8), %%r9"
       "\n	xorq	grsoT5(,%%rsi,8), %%r8"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	grsoT4(,%%rsi,8), %%r11"
       "\n	xorq	grsoT5(,%%rdi,8), %%r10"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	pxor	grsoT6(,%%rdi,8), %%mm7"
       "\n	pxor	grsoT7(,%%rsi,8), %%mm2"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	grsoT6(,%%rsi,8), %%r9"
       "\n	pxor	grsoT7(,%%rdi,8), %%mm4"



       "\n	# processing input words x[12]=rax and x[14]=rbx "
       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	grsoT0(,%%rdi,8), %%r12"
       "\n	xorq	grsoT1(,%%rsi,8), %%r11"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	grsoT0(,%%rsi,8), %%r14"
       "\n	xorq	grsoT1(,%%rdi,8), %%r13"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	grsoT2(,%%rdi,8), %%r10"
       "\n	xorq	grsoT3(,%%rsi,8), %%r9"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	grsoT2(,%%rsi,8), %%r12"
       "\n	xorq	grsoT3(,%%rdi,8), %%r11"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	grsoT4(,%%rdi,8), %%r8"
       "\n	pxor	grsoT5(,%%rsi,8), %%mm7"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	grsoT4(,%%rsi,8), %%r10"
       "\n	xorq	grsoT5(,%%rdi,8), %%r9"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	pxor	grsoT6(,%%rdi,8), %%mm6"
       "\n	pxor	grsoT7(,%%rsi,8), %%mm1"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	grsoT6(,%%rsi,8), %%r8"
       "\n	pxor	grsoT7(,%%rdi,8), %%mm3"

       "\n	incq	8(%0)			#increment counter"

       "\n	movq	8(%0), %%rdi"
       "\n	cmp	$14, %%edi"
       "\n	je	2f"
       "\n	movq	%%mm1, %%rcx"
       "\n	movq	%%mm3, %%rdx"
       "\n	movq	%%mm0, 0(%0)"
       "\n	movq	%%mm2, 16(%0)"
       "\n	movq	%%mm4, 32(%0)"
       "\n	movq	%%mm5, 40(%0)"
       "\n	movq	%%mm6, 48(%0)"
       "\n	movq	%%mm7, 56(%0)"
       "\n	movq	%%r8 , 64(%0)"
       "\n	movq	%%r9 , 72(%0)"
       "\n	movq	%%r10, 80(%0)"
       "\n	movq	%%r11, 88(%0)"
       "\n	movq	%%r12, 96(%0)"
       "\n	movq	%%r13, 104(%0)"
       "\n	movq	%%r14, 112(%0)"
       "\n	movq	%%r15, 120(%0)"
       "\n	jmp	1b"
       "\n	2:"
       "\n	movq	%%mm0, 0(%0)"
       "\n	movq	%%mm1, 8(%0)"
       "\n	movq	%%mm2, 16(%0)"
       "\n	movq	%%mm3, 24(%0)"
       "\n	movq	%%mm4, 32(%0)"
       "\n	movq	%%mm5, 40(%0)"
       "\n	movq	%%mm6, 48(%0)"
       "\n	movq	%%mm7, 56(%0)"
       "\n	movq	%%r8 , 64(%0)"
       "\n	movq	%%r9 , 72(%0)"
       "\n	movq	%%r10, 80(%0)"
       "\n	movq	%%r11, 88(%0)"
       "\n	movq	%%r12, 96(%0)"
       "\n	movq	%%r13, 104(%0)"
       "\n	movq	%%r14, 112(%0)"
       "\n	movq	%%r15, 120(%0)"
       : /*no output, only memory is modified */
       : "r"(x)
       : "%rax", "%rbx", "%rcx", "%rdx", "%rdi", "%rsi", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15", "memory" , "%mm0", "%mm1", "%mm2" , "%mm3" , "%mm4" , "%mm5" , "%mm6" , "%mm7" );
}//P512ASM()


void grsoQ1024ASM (u64 *x) {
  asm (
       "\n	movq	8(%0), %%rcx"
       "\n	movq	24(%0), %%rdx"
       "\n	movq	$0, 8(%0)"
       "\n	1:"

       "\n	movq	0(%0), %%rax"
       "\n	movq	16(%0), %%rbx"

       /* add round constants to columns 0-3 */
       "\n	movq	$0xffffffffffffffff, %%r14"
       "\n	movq	$0xefffffffffffffff, %%r15"
       "\n	xorq	%%r14, %%rax"
       "\n	xorq	%%r15, %%rcx"
       "\n	movq	$0xdfffffffffffffff, %%r14"
       "\n	movq	$0xcfffffffffffffff, %%r15"
       "\n	xorq	%%r14, %%rbx"
       "\n	xorq	%%r15, %%rdx"

       "\n	# processing input words x[1]=rcx and x[3]=rdx "
       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	movq	grsoT0(,%%rdi,8), %%mm0"
       "\n	movq	grsoT1(,%%rsi,8), %%r14"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	movq	grsoT0(,%%rsi,8), %%mm2"
       "\n	pxor	grsoT1(,%%rdi,8), %%mm0"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	movq	grsoT2(,%%rdi,8), %%r12"
       "\n	movq	grsoT3(,%%rsi,8), %%mm6"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	grsoT2(,%%rsi,8), %%r14"
       "\n	movq	grsoT3(,%%rdi,8), %%r8"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	movq	grsoT4(,%%rdi,8), %%mm1"
       "\n	movq	grsoT5(,%%rsi,8), %%r15"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	movq	grsoT4(,%%rsi,8), %%mm3"
       "\n	pxor	grsoT5(,%%rdi,8), %%mm1"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	xorq	8(%0), %%rsi"
       "\n	movq	grsoT6(,%%rdi,8), %%r13"
       "\n	movq	grsoT7(,%%rsi,8), %%r11"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	8(%0), %%rdi"
       "\n	xorq	grsoT6(,%%rsi,8), %%r15"
       "\n	xorq	grsoT7(,%%rdi,8), %%r13"


       "\n	# processing input words x[0]=rax and x[2]=rbx "
       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	grsoT0(,%%rdi,8), %%r15"
       "\n	xorq	grsoT1(,%%rsi,8), %%r13"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	pxor	grsoT0(,%%rsi,8), %%mm1"
       "\n	xorq	grsoT1(,%%rdi,8), %%r15"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	grsoT2(,%%rdi,8), %%r11"
       "\n	movq	grsoT3(,%%rsi,8), %%mm5"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	grsoT2(,%%rsi,8), %%r13"
       "\n	movq	grsoT3(,%%rdi,8), %%mm7"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	pxor	grsoT4(,%%rdi,8), %%mm0"
       "\n	xorq	grsoT5(,%%rsi,8), %%r14"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	pxor	grsoT4(,%%rsi,8), %%mm2"
       "\n	pxor	grsoT5(,%%rdi,8), %%mm0"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	8(%0), %%rsi"
       "\n	xorq	grsoT6(,%%rdi,8), %%r12"
       "\n	movq	grsoT7(,%%rsi,8), %%r10"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	8(%0), %%rdi"
       "\n	xorq	grsoT6(,%%rsi,8), %%r14"
       "\n	xorq	grsoT7(,%%rdi,8), %%r12"

       /* read columns 4-7 from registers and add round constants to these */
       "\n	movq	%%r14, 128(%0)"
       "\n	movq	%%r15, 136(%0)"

       "\n	movq	32(%0), %%rax" /* read input column 4 */
       "\n	movq	40(%0), %%rcx" /* read input column 5 */
       "\n	movq	48(%0), %%rbx" /* read input column 6 */
       "\n	movq	56(%0), %%rdx" /* read input column 7 */

       "\n	movq	$0xbfffffffffffffff, %%r14"
       "\n	movq	$0xafffffffffffffff, %%r15"
       "\n	xorq	%%r14, %%rax"
       "\n	xorq	%%r15, %%rcx"
       "\n	movq	$0x9fffffffffffffff, %%r14"
       "\n	movq	$0x8fffffffffffffff, %%r15"
       "\n	xorq	%%r14, %%rbx"
       "\n	xorq	%%r15, %%rdx"

       "\n	movq	128(%0), %%r14"
       "\n	movq	136(%0), %%r15"

       "\n	# processing input words x[5]=rcx and x[7]=rdx "
       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	movq	grsoT0(,%%rdi,8), %%mm4"
       "\n	pxor	grsoT1(,%%rsi,8), %%mm2"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	pxor	grsoT0(,%%rsi,8), %%mm6"
       "\n	pxor	grsoT1(,%%rdi,8), %%mm4"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	pxor	grsoT2(,%%rdi,8), %%mm0"
       "\n	xorq	grsoT3(,%%rsi,8), %%r10"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	pxor	grsoT2(,%%rsi,8), %%mm2"
       "\n	xorq	grsoT3(,%%rdi,8), %%r12"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	pxor	grsoT4(,%%rdi,8), %%mm5"
       "\n	pxor	grsoT5(,%%rsi,8), %%mm3"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	pxor	grsoT4(,%%rsi,8), %%mm7"
       "\n	pxor	grsoT5(,%%rdi,8), %%mm5"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	xorq	8(%0), %%rsi"
       "\n	pxor	grsoT6(,%%rdi,8), %%mm1"
       "\n	xorq	grsoT7(,%%rsi,8), %%r15"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	8(%0), %%rdi"
       "\n	pxor	grsoT6(,%%rsi,8), %%mm3"
       "\n	pxor	grsoT7(,%%rdi,8), %%mm1"


       "\n	# processing input words x[4]=rax and x[6]=rbx "
       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	pxor	grsoT0(,%%rdi,8), %%mm3"
       "\n	pxor	grsoT1(,%%rsi,8), %%mm1"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	pxor	grsoT0(,%%rsi,8), %%mm5"
       "\n	pxor	grsoT1(,%%rdi,8), %%mm3"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	grsoT2(,%%rdi,8), %%r15"
       "\n	movq	grsoT3(,%%rsi,8), %%r9"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	pxor	grsoT2(,%%rsi,8), %%mm1"
       "\n	xorq	grsoT3(,%%rdi,8), %%r11"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	pxor	grsoT4(,%%rdi,8), %%mm4"
       "\n	pxor	grsoT5(,%%rsi,8), %%mm2"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	pxor	grsoT4(,%%rsi,8), %%mm6"
       "\n	pxor	grsoT5(,%%rdi,8), %%mm4"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	8(%0), %%rsi"
       "\n	pxor	grsoT6(,%%rdi,8), %%mm0"
       "\n	xorq	grsoT7(,%%rsi,8), %%r14"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	8(%0), %%rdi"
       "\n	pxor	grsoT6(,%%rsi,8), %%mm2"
       "\n	pxor	grsoT7(,%%rdi,8), %%mm0"

       /* read columns 8-11 from registers and add round constants to these */
       "\n	movq	%%r14, 128(%0)"
       "\n	movq	%%r15, 136(%0)"

       "\n	movq	64(%0), %%rax" /* read input column 8 */
       "\n	movq	72(%0), %%rcx" /* read input column 9 */
       "\n	movq	80(%0), %%rbx" /* read input column 10 */
       "\n	movq	88(%0), %%rdx" /* read input column 11 */

       "\n	movq	$0x7fffffffffffffff, %%r14"
       "\n	movq	$0x6fffffffffffffff, %%r15"
       "\n	xorq	%%r14, %%rax"
       "\n	xorq	%%r15, %%rcx"
       "\n	movq	$0x5fffffffffffffff, %%r14"
       "\n	movq	$0x4fffffffffffffff, %%r15"
       "\n	xorq	%%r14, %%rbx"
       "\n	xorq	%%r15, %%rdx"

       "\n	movq	128(%0), %%r14"
       "\n	movq	136(%0), %%r15"


       "\n	# processing input words x[9]=rcx and x[11]=rdx "
       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	xorq	grsoT0(,%%rdi,8), %%r8"
       "\n	pxor	grsoT1(,%%rsi,8), %%mm6"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	grsoT0(,%%rsi,8), %%r10"
       "\n	xorq	grsoT1(,%%rdi,8), %%r8"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	pxor	grsoT2(,%%rdi,8), %%mm4"
       "\n	xorq	grsoT3(,%%rsi,8), %%r14"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	pxor	grsoT2(,%%rsi,8), %%mm6"
       "\n	pxor	grsoT3(,%%rdi,8), %%mm0"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	xorq	grsoT4(,%%rdi,8), %%r9"
       "\n	pxor	grsoT5(,%%rsi,8), %%mm7"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	grsoT4(,%%rsi,8), %%r11"
       "\n	xorq	grsoT5(,%%rdi,8), %%r9"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	xorq	8(%0), %%rsi"
       "\n	pxor	grsoT6(,%%rdi,8), %%mm5"
       "\n	pxor	grsoT7(,%%rsi,8), %%mm3"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	8(%0), %%rdi"
       "\n	pxor	grsoT6(,%%rsi,8), %%mm7"
       "\n	pxor	grsoT7(,%%rdi,8), %%mm5"



       "\n	# processing input words x[8]=rax and x[10]=rbx "
       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	pxor	grsoT0(,%%rdi,8), %%mm7"
       "\n	pxor	grsoT1(,%%rsi,8), %%mm5"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	grsoT0(,%%rsi,8), %%r9"
       "\n	pxor	grsoT1(,%%rdi,8), %%mm7"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	pxor	grsoT2(,%%rdi,8), %%mm3"
       "\n	xorq	grsoT3(,%%rsi,8), %%r13"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	pxor	grsoT2(,%%rsi,8), %%mm5"
       "\n	xorq	grsoT3(,%%rdi,8), %%r15"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	grsoT4(,%%rdi,8), %%r8"
       "\n	pxor	grsoT5(,%%rsi,8), %%mm6"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	grsoT4(,%%rsi,8), %%r10"
       "\n	xorq	grsoT5(,%%rdi,8), %%r8"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	8(%0), %%rsi"
       "\n	pxor	grsoT6(,%%rdi,8), %%mm4"
       "\n	pxor	grsoT7(,%%rsi,8), %%mm2"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	8(%0), %%rdi"
       "\n	pxor	grsoT6(,%%rsi,8), %%mm6"
       "\n	pxor	grsoT7(,%%rdi,8), %%mm4"


       /* read columns 12-15 from registers and add round constants to these */
       "\n	movq	%%r14, 128(%0)"
       "\n	movq	%%r15, 136(%0)"

       "\n	movq	96(%0), %%rax"  /* read input column 12 */
       "\n	movq	104(%0), %%rcx" /* read input column 13 */
       "\n	movq	112(%0), %%rbx" /* read input column 14 */
       "\n	movq	120(%0), %%rdx" /* read input column 15 */

       "\n	movq	$0x3fffffffffffffff, %%r14"
       "\n	movq	$0x2fffffffffffffff, %%r15"
       "\n	xorq	%%r14, %%rax"
       "\n	xorq	%%r15, %%rcx"
       "\n	movq	$0x1fffffffffffffff, %%r14"
       "\n	movq	$0x0fffffffffffffff, %%r15"
       "\n	xorq	%%r14, %%rbx"
       "\n	xorq	%%r15, %%rdx"

       "\n	movq	128(%0), %%r14"
       "\n	movq	136(%0), %%r15"


       "\n	# processing input words x[13]=rcx and x[15]=rdx "
       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	xorq	grsoT0(,%%rdi,8), %%r12"
       "\n	xorq	grsoT1(,%%rsi,8), %%r10"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	grsoT0(,%%rsi,8), %%r14"
       "\n	xorq	grsoT1(,%%rdi,8), %%r12"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	xorq	grsoT2(,%%rdi,8), %%r8"
       "\n	pxor	grsoT3(,%%rsi,8), %%mm2"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	grsoT2(,%%rsi,8), %%r10"
       "\n	pxor	grsoT3(,%%rdi,8), %%mm4"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	xorq	grsoT4(,%%rdi,8), %%r13"
       "\n	xorq	grsoT5(,%%rsi,8), %%r11"
       "\n	shrq	$16, %%rcx"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	grsoT4(,%%rsi,8), %%r15"
       "\n	xorq	grsoT5(,%%rdi,8), %%r13"
       "\n	shrq	$16, %%rdx"



       "\n	movzbl	%%cl, %%edi"
       "\n	movzbl	%%ch, %%esi"
       "\n	xorq	8(%0), %%rsi"
       "\n	xorq	grsoT6(,%%rdi,8), %%r9"
       "\n	pxor	grsoT7(,%%rsi,8), %%mm7"
       "\n	movzbl	%%dl, %%esi"
       "\n	movzbl	%%dh, %%edi"
       "\n	xorq	8(%0), %%rdi"
       "\n	xorq	grsoT6(,%%rsi,8), %%r11"
       "\n	xorq	grsoT7(,%%rdi,8), %%r9"



       "\n	# processing input words x[12]=rax and x[14]=rbx "
       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	grsoT0(,%%rdi,8), %%r11"
       "\n	xorq	grsoT1(,%%rsi,8), %%r9"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	grsoT0(,%%rsi,8), %%r13"
       "\n	xorq	grsoT1(,%%rdi,8), %%r11"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	pxor	grsoT2(,%%rdi,8), %%mm7"
       "\n	pxor	grsoT3(,%%rsi,8), %%mm1"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	grsoT2(,%%rsi,8), %%r9"
       "\n	pxor	grsoT3(,%%rdi,8), %%mm3"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	grsoT4(,%%rdi,8), %%r12"
       "\n	xorq	grsoT5(,%%rsi,8), %%r10"
       "\n	shrq	$16, %%rax"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	grsoT4(,%%rsi,8), %%r14"
       "\n	xorq	grsoT5(,%%rdi,8), %%r12"
       "\n	shrq	$16, %%rbx"



       "\n	movzbl	%%al, %%edi"
       "\n	movzbl	%%ah, %%esi"
       "\n	xorq	8(%0), %%rsi"
       "\n	xorq	grsoT6(,%%rdi,8), %%r8"
       "\n	pxor	grsoT7(,%%rsi,8), %%mm6"
       "\n	movzbl	%%bl, %%esi"
       "\n	movzbl	%%bh, %%edi"
       "\n	xorq	8(%0), %%rdi"
       "\n	xorq	grsoT6(,%%rsi,8), %%r10"
       "\n	xorq	grsoT7(,%%rdi,8), %%r8"

       "\n	incq	8(%0)			#increment counter"

       "\n	movq	8(%0), %%rdi"
       "\n	cmp	$14, %%edi"
       "\n	je	2f"
       "\n	movq	%%mm1, %%rcx"
       "\n	movq	%%mm3, %%rdx"
       "\n	movq	%%mm0, 0(%0)"
       "\n	movq	%%mm2, 16(%0)"
       "\n	movq	%%mm4, 32(%0)"
       "\n	movq	%%mm5, 40(%0)"
       "\n	movq	%%mm6, 48(%0)"
       "\n	movq	%%mm7, 56(%0)"
       "\n	movq	%%r8 , 64(%0)"
       "\n	movq	%%r9 , 72(%0)"
       "\n	movq	%%r10, 80(%0)"
       "\n	movq	%%r11, 88(%0)"
       "\n	movq	%%r12, 96(%0)"
       "\n	movq	%%r13, 104(%0)"
       "\n	movq	%%r14, 112(%0)"
       "\n	movq	%%r15, 120(%0)"
       "\n	jmp	1b"
       "\n	2:"
       "\n	movq	%%mm0, 0(%0)"
       "\n	movq	%%mm1, 8(%0)"
       "\n	movq	%%mm2, 16(%0)"
       "\n	movq	%%mm3, 24(%0)"
       "\n	movq	%%mm4, 32(%0)"
       "\n	movq	%%mm5, 40(%0)"
       "\n	movq	%%mm6, 48(%0)"
       "\n	movq	%%mm7, 56(%0)"
       "\n	movq	%%r8 , 64(%0)"
       "\n	movq	%%r9 , 72(%0)"
       "\n	movq	%%r10, 80(%0)"
       "\n	movq	%%r11, 88(%0)"
       "\n	movq	%%r12, 96(%0)"
       "\n	movq	%%r13, 104(%0)"
       "\n	movq	%%r14, 112(%0)"
       "\n	movq	%%r15, 120(%0)"
       : /*no output, only memory is modified */
       : "r"(x)
       : "%rax", "%rbx", "%rcx", "%rdx", "%rdi", "%rsi", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15", "memory" , "%mm0", "%mm1", "%mm2" , "%mm3" , "%mm4" , "%mm5" , "%mm6" , "%mm7" );
}//Q512ASM()

