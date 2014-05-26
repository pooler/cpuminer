#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nist.h"
#include "simd_iv.h"


/* #define NO_PRECOMPUTED_IV */


/* 
 * Increase the counter.
 */
void IncreaseCounter(hashState_sd *state, DataLength databitlen) {
#ifdef HAS_64
      state->count += databitlen;
#else
      u32 old_count = state->count_low;
      state->count_low += databitlen;
      if (state->count_low < old_count)
        state->count_high++;
#endif
}


/* 
 * Initialize the hashState_sd with a given IV.
 * If the IV is NULL, initialize with zeros.
 */
HashReturn InitIV(hashState_sd *state, int hashbitlen, const u32 *IV) {

  int n;

  if (!SupportedLength(hashbitlen))
    return BAD_HASHBITLEN;

  n =  8;

  state->hashbitlen = hashbitlen;
  state->n_feistels = n;
  state->blocksize = 128*8;
  
#ifdef HAS_64
  state->count = 0;
#else
  state->count_low  = 0;
  state->count_high = 0;
#endif  

//  state->buffer = malloc(16*n + 16);
  /*
   * Align the buffer to a 128 bit boundary.
   */
//  state->buffer += ((unsigned char*)NULL - state->buffer)&15;

//  state->A = malloc((4*n+4)*sizeof(u32));
  /*
   * Align the buffer to a 128 bit boundary.
   */
//  state->A += ((u32*)NULL - state->A)&3;

  state->B = state->A+n;
  state->C = state->B+n;
  state->D = state->C+n;

  if (IV)
    memcpy(state->A, IV, 4*n*sizeof(u32));
  else
    memset(state->A, 0, 4*n*sizeof(u32));

   // free(state->buffer);
  //  free(state->A);	
  return SUCCESS;
  
}

/* 
 * Initialize the hashState_sd.
 */
HashReturn init_sd(hashState_sd *state, int hashbitlen) {
  HashReturn r;
  char *init;

#ifndef NO_PRECOMPUTED_IV
  if (hashbitlen == 224)
    r=InitIV(state, hashbitlen, IV_224);
  else if (hashbitlen == 256)
    r=InitIV(state, hashbitlen, IV_256);
  else if (hashbitlen == 384)
    r=InitIV(state, hashbitlen, IV_384);
  else if (hashbitlen == 512)
    r=InitIV(state, hashbitlen, IV_512);
  else
#endif
    {
      /* 
       * Nonstandart length: IV is not precomputed.
       */
      r=InitIV(state, hashbitlen, NULL);
      if (r != SUCCESS)
        return r;
      
      init = malloc(state->blocksize);
      memset(init, 0, state->blocksize);
#if defined __STDC__ && __STDC_VERSION__ >= 199901L
      snprintf(init, state->blocksize, "SIMD-%i v1.1", hashbitlen);
#else
      sprintf(init, "SIMD-%i v1.1", hashbitlen);
#endif
      SIMD_Compress(state, (unsigned char*) init, 0);
      free(init);
    }
  return r;
}



HashReturn update_sd(hashState_sd *state, const BitSequence *data, DataLength databitlen) {
  unsigned current;
  unsigned int bs = state->blocksize;
  static int align = -1;

  if (align == -1)
    align = RequiredAlignment();

#ifdef HAS_64
  current = state->count & (bs - 1);
#else
  current = state->count_low & (bs - 1);
#endif
  
  if (current & 7) {
    /*
     * The number of hashed bits is not a multiple of 8.
     * Very painfull to implement and not required by the NIST API.
     */
    return FAIL;
  }

  while (databitlen > 0) {
    if (IS_ALIGNED(data,align) && current == 0 && databitlen >= bs) {
      /* 
       * We can hash the data directly from the input buffer.
       */
      SIMD_Compress(state, data, 0);
      databitlen -= bs;
      data += bs/8;
      IncreaseCounter(state, bs);
    } else {
      /* 
       * Copy a chunk of data to the buffer
       */
      unsigned int len = bs - current;
      if (databitlen < len) {
        memcpy(state->buffer+current/8, data, (databitlen+7)/8);
        IncreaseCounter(state, databitlen);        
        return SUCCESS;
      } else {
        memcpy(state->buffer+current/8, data, len/8);
        IncreaseCounter(state,len);
        databitlen -= len;
        data += len/8;
        current = 0;
        SIMD_Compress(state, state->buffer, 0);
      }
    }
  }
  return SUCCESS;
}

HashReturn final_sd(hashState_sd *state, BitSequence *hashval) {
#ifdef HAS_64
  u64 l;
  int current = state->count & (state->blocksize - 1);
#else
  u32 l;
  int current = state->count_low & (state->blocksize - 1);
#endif
  unsigned int i;
  BitSequence bs[64];
  int isshort = 1;

  /* 
   * If there is still some data in the buffer, hash it
   */
  if (current) {
    /* 
     * We first need to zero out the end of the buffer.
     */
    if (current & 7) {
      BitSequence mask = 0xff >> (current&7);
      state->buffer[current/8] &= ~mask;
    }
    current = (current+7)/8;
    memset(state->buffer+current, 0, state->blocksize/8 - current);
    SIMD_Compress(state, state->buffer, 0);
  }

  /* 
   * Input the message length as the last block
   */
  memset(state->buffer, 0, state->blocksize/8);
#ifdef HAS_64
  l = state->count;
  for (i=0; i<8; i++) {
    state->buffer[i] = l & 0xff;
    l >>= 8;
  }
  if (state->count < 16384)
    isshort = 2;
#else
  l = state->count_low;
  for (i=0; i<4; i++) {
    state->buffer[i] = l & 0xff;
    l >>= 8;
  }
  l = state->count_high;
  for (i=0; i<4; i++) {
    state->buffer[4+i] = l & 0xff;
    l >>= 8;
  }
  if (state->count_high == 0 && state->count_low < 16384)
    isshort = 2;
#endif

  SIMD_Compress(state, state->buffer, isshort);
    

  /*
   * Decode the 32-bit words into a BitSequence
   */
  for (i=0; i<2*state->n_feistels; i++) {
    u32 x = state->A[i];
    bs[4*i  ] = x&0xff;
    x >>= 8;
    bs[4*i+1] = x&0xff;
    x >>= 8;
    bs[4*i+2] = x&0xff;
    x >>= 8;
    bs[4*i+3] = x&0xff;
  }

  memcpy(hashval, bs, state->hashbitlen/8);
  if (state->hashbitlen%8) {
    BitSequence mask = 0xff << (8 - (state->hashbitlen%8));
    hashval[state->hashbitlen/8 + 1] = bs[state->hashbitlen/8 + 1] & mask;
  }
//free(state->buffer);
//free(state->A);
  return SUCCESS;
}



/*HashReturn Hash(int hashbitlen, const BitSequence *data, DataLength databitlen,
                BitSequence *hashval) {
  hashState_sd s;
  HashReturn r;
  r = Init(&s, hashbitlen);
  if (r != SUCCESS)
    return r;
  r = Update(&s, data, databitlen);
  if (r != SUCCESS)
    return r;
  r = Final(&s, hashval);
  return r;
}
*/