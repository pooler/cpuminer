/* hash.c     Aug 2011
 *
 * Groestl implementation for different versions.
 * Author: Krystian Matusiewicz, Günther A. Roland, Martin Schläffer
 *
 * This code is placed in the public domain
 */


#include "grsv.h"
#include "grsv-asm.h"

/* digest up to len bytes of input (full blocks only) */
void grsvTransform(grsvState *ctx,
	       const u8 *in, 
	       unsigned long long len) {

    /* increment block counter */
    ctx->grsvblock_counter += len/grsvSIZE;

    /* digest message, one block at a time */
    for (; len >= grsvSIZE; len -= grsvSIZE, in += grsvSIZE)
#if grsvLENGTH<=256
      grsvTF512((u64*)ctx->grsvchaining, (u64*)in);
#else
      grsvTF1024((u64*)ctx->grsvchaining, (u64*)in);
#endif

    asm volatile ("emms");
}

/* given state h, do h <- P(h)+h */
void grsvOutputTransformation(grsvState *ctx) {

    /* determine variant */
#if (grsvLENGTH <= 256)
    grsvOF512((u64*)ctx->grsvchaining);
#else
    grsvOF1024((u64*)ctx->grsvchaining);
#endif

    asm volatile ("emms");
}

/* initialise context */
void grsvInit(grsvState* ctx) {
  u8 i = 0;

  /* output size (in bits) must be a positive integer less than or
     equal to 512, and divisible by 8 */
  if (grsvLENGTH <= 0 || (grsvLENGTH%8) || grsvLENGTH > 512)
    return;

  /* set number of state columns and state size depending on
     variant */
  ctx->grsvcolumns = grsvCOLS;
  ctx->grsvstatesize = grsvSIZE;
#if (grsvLENGTH <= 256)
    ctx->grsvv = SHORT;
#else
    ctx->grsvv = LONG;
#endif

  SET_CONSTANTS();

  for (i=0; i<grsvSIZE/8; i++)
    ctx->grsvchaining[i] = 0;
  for (i=0; i<grsvSIZE; i++)
    ctx->grsvbuffer[i] = 0;

  if (ctx->grsvchaining == NULL || ctx->grsvbuffer == NULL)
    return;

  /* set initial value */
  ctx->grsvchaining[ctx->grsvcolumns-1] = U64BIG((u64)grsvLENGTH);

  grsvINIT(ctx->grsvchaining);

  /* set other variables */
  ctx->grsvbuf_ptr = 0;
  ctx->grsvblock_counter = 0;
  ctx->grsvbits_in_last_byte = 0;

  return; 
}

/* update state with databitlen bits of input */
void grsvUpdate(grsvState* ctx,
		  const grsvBitSequence* input,
		  grsvDataLength databitlen) {
  int index = 0;
  int msglen = (int)(databitlen/8);
  int rem = (int)(databitlen%8);

  /* non-integral number of message bytes can only be supplied in the
     last call to this function */
  if (ctx->grsvbits_in_last_byte) return;

  /* if the buffer contains data that has not yet been digested, first
     add data to buffer until full */
  if (ctx->grsvbuf_ptr) {
    while (ctx->grsvbuf_ptr < ctx->grsvstatesize && index < msglen) {
      ctx->grsvbuffer[(int)ctx->grsvbuf_ptr++] = input[index++];
    }
    if (ctx->grsvbuf_ptr < ctx->grsvstatesize) {
      /* buffer still not full, return */
      if (rem) {
        ctx->grsvbits_in_last_byte = rem;
        ctx->grsvbuffer[(int)ctx->grsvbuf_ptr++] = input[index];
      }
      return; 
    }

    /* digest buffer */
    ctx->grsvbuf_ptr = 0;
    printf("error\n");
    grsvTransform(ctx, ctx->grsvbuffer, ctx->grsvstatesize);
  }

  /* digest bulk of message */
  grsvTransform(ctx, input+index, msglen-index);
  index += ((msglen-index)/ctx->grsvstatesize)*ctx->grsvstatesize;

  /* store remaining data in buffer */
  while (index < msglen) {
    ctx->grsvbuffer[(int)ctx->grsvbuf_ptr++] = input[index++];
  }

  /* if non-integral number of bytes have been supplied, store
     remaining bits in last byte, together with information about
     number of bits */
  if (rem) {
    ctx->grsvbits_in_last_byte = rem;
    ctx->grsvbuffer[(int)ctx->grsvbuf_ptr++] = input[index];
  }
  return;
}

#define BILB ctx->grsvbits_in_last_byte

/* finalise: process remaining data (including padding), perform
   output transformation, and write hash result to 'output' */
void grsvFinal(grsvState* ctx,
		 grsvBitSequence* output) {
  int i, j = 0, grsvbytelen = grsvLENGTH/8;
  u8 *s = (grsvBitSequence*)ctx->grsvchaining;

  /* pad with '1'-bit and first few '0'-bits */
  if (BILB) {
    ctx->grsvbuffer[(int)ctx->grsvbuf_ptr-1] &= ((1<<BILB)-1)<<(8-BILB);
    ctx->grsvbuffer[(int)ctx->grsvbuf_ptr-1] ^= 0x1<<(7-BILB);
    BILB = 0;
  }
  else ctx->grsvbuffer[(int)ctx->grsvbuf_ptr++] = 0x80;

  /* pad with '0'-bits */
  if (ctx->grsvbuf_ptr > ctx->grsvstatesize-grsvLENGTHFIELDLEN) {
    /* padding requires two blocks */
    while (ctx->grsvbuf_ptr < ctx->grsvstatesize) {
      ctx->grsvbuffer[(int)ctx->grsvbuf_ptr++] = 0;
    }
    /* digest first padding block */
    grsvTransform(ctx, ctx->grsvbuffer, ctx->grsvstatesize);
    ctx->grsvbuf_ptr = 0;
  }
  while (ctx->grsvbuf_ptr < ctx->grsvstatesize-grsvLENGTHFIELDLEN) {
    ctx->grsvbuffer[(int)ctx->grsvbuf_ptr++] = 0;
  }

  /* length padding */
  ctx->grsvblock_counter++;
  ctx->grsvbuf_ptr = ctx->grsvstatesize;
  while (ctx->grsvbuf_ptr > ctx->grsvstatesize-grsvLENGTHFIELDLEN) {
    ctx->grsvbuffer[(int)--ctx->grsvbuf_ptr] = (u8)ctx->grsvblock_counter;
    ctx->grsvblock_counter >>= 8;
  }

  /* digest final padding block */
  grsvTransform(ctx, ctx->grsvbuffer, ctx->grsvstatesize);
  /* perform output transformation */
  grsvOutputTransformation(ctx);

  /* store hash result in output */
  for (i = ctx->grsvstatesize-grsvbytelen; i < ctx->grsvstatesize; i++,j++) {
    output[j] = s[i];
  }

  /* zeroise relevant variables and deallocate memory */
  
  for (i = 0; i < ctx->grsvcolumns; i++) {
    ctx->grsvchaining[i] = 0;
  }
  
  for (i = 0; i < ctx->grsvstatesize; i++) {
    ctx->grsvbuffer[i] = 0;
  }
//  free(ctx->grsvchaining);
//  free(ctx->buffer);

  return;
}

