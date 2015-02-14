/* hash.c     January 2011
 *
 * Groestl-512 implementation with inline assembly containing mmx and
 * sse instructions. Optimized for Opteron.
 * Authors: Krystian Matusiewicz and Soeren S. Thomsen
 *
 * This code is placed in the public domain
 */

#include "grso.h"
#include "grso-asm.h"
#include "grsotab.h"


/* digest up to len bytes of input (full blocks only) */
void grsoTransform(grsoState *ctx, 
	       const unsigned char *in, 
	       unsigned long long len) {
  u64 y[grsoCOLS+2] __attribute__ ((aligned (16)));
  u64 z[grsoCOLS+2] __attribute__ ((aligned (16)));
  u64 *m, *h = (u64*)ctx->grsstate;
  int i;
  
  /* increment block counter */
  ctx->grsblock_counter += len/grsoSIZE;
  
  /* digest message, one block at a time */
  for (; len >= grsoSIZE; len -= grsoSIZE, in += grsoSIZE) {
    m = (u64*)in;
    for (i = 0; i < grsoCOLS; i++) {
      y[i] = m[i];
      z[i] = m[i] ^ h[i];
    }

    grsoQ1024ASM(y);
    grsoP1024ASM(z);

    /* h' == h + Q(m) + P(h+m) */
    for (i = 0; i < grsoCOLS; i++) {
      h[i] ^= z[i] ^ y[i];
    }
  }
}

/* given state h, do h <- P(h)+h */
void grsoOutputTransformation(grsoState *ctx) {
  u64 z[grsoCOLS] __attribute__ ((aligned (16)));
  int j;

  for (j = 0; j < grsoCOLS; j++) {
    z[j] = ctx->grsstate[j];
  }
  grsoP1024ASM(z);
  for (j = 0; j < grsoCOLS; j++) {
    ctx->grsstate[j] ^= z[j];
  }
}

