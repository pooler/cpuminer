/* hash.c     Aug 2011
 *
 * Groestl implementation for different versions.
 * Author: Krystian Matusiewicz, Günther A. Roland, Martin Schläffer
 *
 * This code is placed in the public domain
 */

#include "groestl-version.h"

#ifdef TASM
  #ifdef VAES
    #include "groestl-asm-aes.h"
  #else
    #ifdef VAVX
      #include "groestl-asm-avx.h"
    #else
      #ifdef VVPERM
        #include "groestl-asm-vperm.h"
      #else
        #error NO VERSION SPECIFIED (-DV[AES/AVX/VVPERM])
      #endif
    #endif
  #endif
#else
  #ifdef TINTR
    #ifdef VAES
      #include "groestl-intr-aes.h"
    #else
      #ifdef VAVX
        #include "groestl-intr-avx.h"
      #else
        #ifdef VVPERM
          #include "groestl-intr-vperm.h"
        #else
          #error NO VERSION SPECIFIED (-DV[AES/AVX/VVPERM])
        #endif
      #endif
    #endif
  #else
    #error NO TYPE SPECIFIED (-DT[ASM/INTR])
  #endif
#endif


/* digest up to len bytes of input (full blocks only) */
void Transform(hashState_groestl *ctx,
	       const u8 *in, 
	       unsigned long long len) {

    /* increment block counter */
    ctx->block_counter += len/SIZE;

    /* digest message, one block at a time */
    for (; len >= SIZE; len -= SIZE, in += SIZE)
#if LENGTH<=256
      TF512((u64*)ctx->chaining, (u64*)in);
#else
      TF1024((u64*)ctx->chaining, (u64*)in);
#endif

    asm volatile ("emms");
}

/* given state h, do h <- P(h)+h */
void OutputTransformation(hashState_groestl *ctx) {

    /* determine variant */
#if (LENGTH <= 256)
    OF512((u64*)ctx->chaining);
#else
    OF1024((u64*)ctx->chaining);
#endif

    asm volatile ("emms");
}

/* initialise context */
HashReturn_gr init_groestl(hashState_groestl* ctx) {
  u8 i = 0;

  /* output size (in bits) must be a positive integer less than or
     equal to 512, and divisible by 8 */
  if (LENGTH <= 0 || (LENGTH%8) || LENGTH > 512)
    return BAD_HASHBITLEN_GR;

  /* set number of state columns and state size depending on
     variant */
  ctx->columns = COLS;
  ctx->statesize = SIZE;
#if (LENGTH <= 256)
    ctx->v = SHORT;
#else
    ctx->v = LONG;
#endif

  SET_CONSTANTS();

  for (i=0; i<SIZE/8; i++)
    ctx->chaining[i] = 0;
  for (i=0; i<SIZE; i++)
    ctx->buffer[i] = 0;

  if (ctx->chaining == NULL || ctx->buffer == NULL)
    return FAIL_GR;

  /* set initial value */
  ctx->chaining[ctx->columns-1] = U64BIG((u64)LENGTH);

  INIT(ctx->chaining);

  /* set other variables */
  ctx->buf_ptr = 0;
  ctx->block_counter = 0;
  ctx->bits_in_last_byte = 0;

  return SUCCESS_GR;
}

/* update state with databitlen bits of input */
HashReturn_gr update_groestl(hashState_groestl* ctx,
		  const BitSequence_gr* input,
		  DataLength_gr databitlen) {
  int index = 0;
  int msglen = (int)(databitlen/8);
  int rem = (int)(databitlen%8);

  /* non-integral number of message bytes can only be supplied in the
     last call to this function */
  if (ctx->bits_in_last_byte) return FAIL_GR;

  /* if the buffer contains data that has not yet been digested, first
     add data to buffer until full */
  if (ctx->buf_ptr) {
    while (ctx->buf_ptr < ctx->statesize && index < msglen) {
      ctx->buffer[(int)ctx->buf_ptr++] = input[index++];
    }
    if (ctx->buf_ptr < ctx->statesize) {
      /* buffer still not full, return */
      if (rem) {
        ctx->bits_in_last_byte = rem;
        ctx->buffer[(int)ctx->buf_ptr++] = input[index];
      }
      return SUCCESS_GR;
    }

    /* digest buffer */
    ctx->buf_ptr = 0;
    printf("error\n");
    Transform(ctx, ctx->buffer, ctx->statesize);
  }

  /* digest bulk of message */
  Transform(ctx, input+index, msglen-index);
  index += ((msglen-index)/ctx->statesize)*ctx->statesize;

  /* store remaining data in buffer */
  while (index < msglen) {
    ctx->buffer[(int)ctx->buf_ptr++] = input[index++];
  }

  /* if non-integral number of bytes have been supplied, store
     remaining bits in last byte, together with information about
     number of bits */
  if (rem) {
    ctx->bits_in_last_byte = rem;
    ctx->buffer[(int)ctx->buf_ptr++] = input[index];
  }
  return SUCCESS_GR;
}

#define BILB ctx->bits_in_last_byte

/* finalise: process remaining data (including padding), perform
   output transformation, and write hash result to 'output' */
HashReturn_gr final_groestl(hashState_groestl* ctx,
		 BitSequence_gr* output) {
  int i, j = 0, hashbytelen = LENGTH/8;
  u8 *s = (BitSequence_gr*)ctx->chaining;

  /* pad with '1'-bit and first few '0'-bits */
  if (BILB) {
    ctx->buffer[(int)ctx->buf_ptr-1] &= ((1<<BILB)-1)<<(8-BILB);
    ctx->buffer[(int)ctx->buf_ptr-1] ^= 0x1<<(7-BILB);
    BILB = 0;
  }
  else ctx->buffer[(int)ctx->buf_ptr++] = 0x80;

  /* pad with '0'-bits */
  if (ctx->buf_ptr > ctx->statesize-LENGTHFIELDLEN) {
    /* padding requires two blocks */
    while (ctx->buf_ptr < ctx->statesize) {
      ctx->buffer[(int)ctx->buf_ptr++] = 0;
    }
    /* digest first padding block */
    Transform(ctx, ctx->buffer, ctx->statesize);
    ctx->buf_ptr = 0;
  }
  while (ctx->buf_ptr < ctx->statesize-LENGTHFIELDLEN) {
    ctx->buffer[(int)ctx->buf_ptr++] = 0;
  }

  /* length padding */
  ctx->block_counter++;
  ctx->buf_ptr = ctx->statesize;
  while (ctx->buf_ptr > ctx->statesize-LENGTHFIELDLEN) {
    ctx->buffer[(int)--ctx->buf_ptr] = (u8)ctx->block_counter;
    ctx->block_counter >>= 8;
  }

  /* digest final padding block */
  Transform(ctx, ctx->buffer, ctx->statesize);
  /* perform output transformation */
  OutputTransformation(ctx);

  /* store hash result in output */
  for (i = ctx->statesize-hashbytelen; i < ctx->statesize; i++,j++) {
    output[j] = s[i];
  }

  /* zeroise relevant variables and deallocate memory */
  
  for (i = 0; i < ctx->columns; i++) {
    ctx->chaining[i] = 0;
  }
  
  for (i = 0; i < ctx->statesize; i++) {
    ctx->buffer[i] = 0;
  }
//  free(ctx->chaining);
//  free(ctx->buffer);

  return SUCCESS_GR;
}

/* hash bit sequence */
HashReturn_gr hash_groestl(int hashbitlen,
		const BitSequence_gr* data, 
		DataLength_gr databitlen,
		BitSequence_gr* hashval) {
  HashReturn_gr ret;
  hashState_groestl context;

  /* initialise */
  if ((ret = init_groestl(&context)) != SUCCESS_GR)
    return ret;

  /* process message */
  if ((ret = update_groestl(&context, data, databitlen)) != SUCCESS_GR)
    return ret;

  /* finalise */
  ret = final_groestl(&context, hashval);

  return ret;
}

/* eBash API */
#ifdef crypto_hash_BYTES
int crypto_hash(unsigned char *out, const unsigned char *in, unsigned long long inlen)
{
  if (hash_groestl(crypto_hash_BYTES * 8, in, inlen * 8,out) == SUCCESS_GR) return 0;
  return -1;
}
#endif
