/* hash.c     Aug 2011
 *
 * Groestl implementation for different versions.
 * Author: Krystian Matusiewicz, Günther A. Roland, Martin Schläffer
 *
 * This code is placed in the public domain
 */

#include "grsi.h"
#include "grsi-asm.h"

/* void grsiInit(grsiState* ctx) { */
#define GRS_I \
do { \
  grsiState *ctx = &sts_grs; \
  u8 i = 0; \
 \
  /* set number of state columns and state size depending on \
     variant */ \
  ctx->grsicolumns = grsiCOLS; \
  ctx->grsistatesize = grsiSIZE; \
    ctx->grsiv = LONG; \
 \
   grsiSET_CONSTANTS();  \
 \
  memset(ctx->grsichaining, 0, sizeof(u64)*grsiSIZE/8); \
  memset(ctx->grsibuffer, 0, sizeof(grsiBitSequence)*grsiSIZE); \
 \
  if (ctx->grsichaining == NULL || ctx->grsibuffer == NULL) \
    return;  \
 \
  /* set initial value */ \
  ctx->grsichaining[ctx->grsicolumns-1] = grsiU64BIG((u64)grsiLENGTH); \
 \
  grsiINIT(ctx->grsichaining); \
 \
  /* set other variables */ \
  ctx->grsibuf_ptr = 0; \
  ctx->grsiblock_counter = 0; \
  ctx->grsibits_in_last_byte = 0; \
 \
} while (0) 

/* digest up to len bytes of input (full blocks only) */
void grsiTransform(grsiState *ctx,
	       const u8 *in, 
	       unsigned long long len) {

    /* increment block counter */
    ctx->grsiblock_counter += len/grsiSIZE;

    /* digest message, one block at a time */
    for (; len >= grsiSIZE; len -= grsiSIZE, in += grsiSIZE)
      grsiTF1024((u64*)ctx->grsichaining, (u64*)in);

    asm volatile ("emms");
}

/* given state h, do h <- P(h)+h */
void grsiOutputTransformation(grsiState *ctx) {

    /* determine variant */
    grsiOF1024((u64*)ctx->grsichaining);

    asm volatile ("emms");
}

/* initialise context */
void grsiInit(grsiState* ctx) {
  u8 i = 0;

  /* output size (in bits) must be a positive integer less than or
     equal to 512, and divisible by 8 */
  if (grsiLENGTH <= 0 || (grsiLENGTH%8) || grsiLENGTH > 512)
    return; 

  /* set number of state columns and state size depending on
     variant */
  ctx->grsicolumns = grsiCOLS;
  ctx->grsistatesize = grsiSIZE;
    ctx->grsiv = LONG;

  grsiSET_CONSTANTS();

  for (i=0; i<grsiSIZE/8; i++)
    ctx->grsichaining[i] = 0;
  for (i=0; i<grsiSIZE; i++)
    ctx->grsibuffer[i] = 0;

  if (ctx->grsichaining == NULL || ctx->grsibuffer == NULL)
    return; 

  /* set initial value */
  ctx->grsichaining[ctx->grsicolumns-1] = grsiU64BIG((u64)grsiLENGTH);

  grsiINIT(ctx->grsichaining);

  /* set other variables */
  ctx->grsibuf_ptr = 0;
  ctx->grsiblock_counter = 0;
  ctx->grsibits_in_last_byte = 0;

  return;
}

/* update state with databitlen bits of input */
void grsiUpdate(grsiState* ctx,
		  const grsiBitSequence* input,
		  grsiDataLength databitlen) {
  int index = 0;
  int msglen = (int)(databitlen/8);
  int rem = (int)(databitlen%8);

  /* non-integral number of message bytes can only be supplied in the
     last call to this function */
  if (ctx->grsibits_in_last_byte) return;

  /* if the buffer contains data that has not yet been digested, first
     add data to buffer until full */
  if (ctx->grsibuf_ptr) {
    while (ctx->grsibuf_ptr < ctx->grsistatesize && index < msglen) {
      ctx->grsibuffer[(int)ctx->grsibuf_ptr++] = input[index++];
    }
    if (ctx->grsibuf_ptr < ctx->grsistatesize) {
      /* buffer still not full, return */
      if (rem) {
        ctx->grsibits_in_last_byte = rem;
        ctx->grsibuffer[(int)ctx->grsibuf_ptr++] = input[index];
      }
      return;
    }

    /* digest buffer */
    ctx->grsibuf_ptr = 0;
    printf("error\n");
    grsiTransform(ctx, ctx->grsibuffer, ctx->grsistatesize);
  }

  /* digest bulk of message */
  grsiTransform(ctx, input+index, msglen-index);
  index += ((msglen-index)/ctx->grsistatesize)*ctx->grsistatesize;

  /* store remaining data in buffer */
  while (index < msglen) {
    ctx->grsibuffer[(int)ctx->grsibuf_ptr++] = input[index++];
  }

  /* if non-integral number of bytes have been supplied, store
     remaining bits in last byte, together with information about
     number of bits */
  if (rem) {
    ctx->grsibits_in_last_byte = rem;
    ctx->grsibuffer[(int)ctx->grsibuf_ptr++] = input[index];
  }
  return; 
}

/* update state with databitlen bits of input */
void grsiUpdateq(grsiState* ctx, const grsiBitSequence* input)
{
  grsiDataLength databitlen= 64*8;
  int index = 0;
  int msglen = (int)(databitlen/8);
  int rem = (int)(databitlen%8);

  /* non-integral number of message bytes can only be supplied in the
     last call to this function */
  if (ctx->grsibits_in_last_byte) return;

  /* if the buffer contains data that has not yet been digested, first
     add data to buffer until full */
  if (ctx->grsibuf_ptr) {
    while (ctx->grsibuf_ptr < ctx->grsistatesize && index < msglen) {
      ctx->grsibuffer[(int)ctx->grsibuf_ptr++] = input[index++];
    }
    if (ctx->grsibuf_ptr < ctx->grsistatesize) {
      /* buffer still not full, return */
      if (rem) {
        ctx->grsibits_in_last_byte = rem;
        ctx->grsibuffer[(int)ctx->grsibuf_ptr++] = input[index];
      }
      return;
    }

    /* digest buffer */
    ctx->grsibuf_ptr = 0;
    printf("error\n");
    grsiTransform(ctx, ctx->grsibuffer, ctx->grsistatesize);
  }

  /* digest bulk of message */
  grsiTransform(ctx, input+index, msglen-index);
  index += ((msglen-index)/ctx->grsistatesize)*ctx->grsistatesize;

  /* store remaining data in buffer */
  while (index < msglen) {
    ctx->grsibuffer[(int)ctx->grsibuf_ptr++] = input[index++];
  }

  /* if non-integral number of bytes have been supplied, store
     remaining bits in last byte, together with information about
     number of bits */
  if (rem) {
    ctx->grsibits_in_last_byte = rem;
    ctx->grsibuffer[(int)ctx->grsibuf_ptr++] = input[index];
  }
  return; 
}

#define BILB ctx->grsibits_in_last_byte

/* finalise: process remaining data (including padding), perform
   output transformation, and write hash result to 'output' */
void grsiFinal(grsiState* ctx,
		 grsiBitSequence* output) {
  int i, j = 0, grsibytelen = grsiLENGTH/8;
  u8 *s = (grsiBitSequence*)ctx->grsichaining;

  /* pad with '1'-bit and first few '0'-bits */
  if (BILB) {
    ctx->grsibuffer[(int)ctx->grsibuf_ptr-1] &= ((1<<BILB)-1)<<(8-BILB);
    ctx->grsibuffer[(int)ctx->grsibuf_ptr-1] ^= 0x1<<(7-BILB);
    BILB = 0;
  }
  else ctx->grsibuffer[(int)ctx->grsibuf_ptr++] = 0x80;

  /* pad with '0'-bits */
  if (ctx->grsibuf_ptr > ctx->grsistatesize-grsiLENGTHFIELDLEN) {
    /* padding requires two blocks */
    while (ctx->grsibuf_ptr < ctx->grsistatesize) {
      ctx->grsibuffer[(int)ctx->grsibuf_ptr++] = 0;
    }
    /* digest first padding block */
    grsiTransform(ctx, ctx->grsibuffer, ctx->grsistatesize);
    ctx->grsibuf_ptr = 0;
  }
  while (ctx->grsibuf_ptr < ctx->grsistatesize-grsiLENGTHFIELDLEN) {
    ctx->grsibuffer[(int)ctx->grsibuf_ptr++] = 0;
  }

  /* length padding */
  ctx->grsiblock_counter++;
  ctx->grsibuf_ptr = ctx->grsistatesize;
  while (ctx->grsibuf_ptr > ctx->grsistatesize-grsiLENGTHFIELDLEN) {
    ctx->grsibuffer[(int)--ctx->grsibuf_ptr] = (u8)ctx->grsiblock_counter;
    ctx->grsiblock_counter >>= 8;
  }

  /* digest final padding block */
  grsiTransform(ctx, ctx->grsibuffer, ctx->grsistatesize);
  /* perform output transformation */
  grsiOutputTransformation(ctx);

  /* store hash result in output */
  for (i = ctx->grsistatesize-grsibytelen; i < ctx->grsistatesize; i++,j++) {
    output[j] = s[i];
  }

  /* zeroise relevant variables and deallocate memory */
  
  for (i = 0; i < ctx->grsicolumns; i++) {
    ctx->grsichaining[i] = 0;
  }
  
  for (i = 0; i < ctx->grsistatesize; i++) {
    ctx->grsibuffer[i] = 0;
  }
//  free(ctx->grsichaining);
//  free(ctx->grsibuffer);

  return; 
}

