/* hash.h     Aug 2011
 *
 * Groestl implementation for different versions.
 * Author: Krystian Matusiewicz, Günther A. Roland, Martin Schläffer
 *
 * This code is placed in the public domain
 */

#ifndef __grsn_h
#define __grsn_h

#include <stdio.h>
#include <stdlib.h>

#include "brg_endian.h"
#define NEED_UINT_64T
#include "brg_types.h"

#ifndef grsnLENGTH
#define grsnLENGTH 512
#endif

/* some sizes (number of bytes) */
#define grsnROWS 8
#define grsnLENGTHFIELDLEN grsnROWS
#define grsnCOLS512 8
#define grsnCOLS1024 16
#define grsnSIZE512 (grsnROWS*grsnCOLS512)
#define grsnSIZE1024 (grsnROWS*grsnCOLS1024)
#define grsnROUNDS512 10
#define grsnROUNDS1024 14

#if grsnLENGTH<=256
#define grsnCOLS grsnCOLS512
#define grsnSIZE grsnSIZE512
#define grsnROUNDS grsnROUNDS512
#else
#define grsnCOLS grsnCOLS1024
#define grsnSIZE grsnSIZE1024
#define grsnROUNDS grsnROUNDS1024
#endif

#define ROTL64(a,n) ((((a)<<(n))|((a)>>(64-(n))))&li_64(ffffffffffffffff))

#if (PLATFORM_BYTE_ORDER == IS_BIG_ENDIAN)
#define EXT_BYTE(var,n) ((u8)((u64)(var) >> (8*(7-(n)))))
#define U64BIG(a) (a)
#endif /* IS_BIG_ENDIAN */

#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN)
#define EXT_BYTE(var,n) ((u8)((u64)(var) >> (8*n)))
#define U64BIG(a) \
  ((ROTL64(a, 8) & li_64(000000FF000000FF)) | \
   (ROTL64(a,24) & li_64(0000FF000000FF00)) | \
   (ROTL64(a,40) & li_64(00FF000000FF0000)) | \
   (ROTL64(a,56) & li_64(FF000000FF000000)))
#endif /* IS_LITTLE_ENDIAN */

typedef enum { LONG, SHORT } Var;

/* NIST API begin */
typedef unsigned char BitSequence;
typedef unsigned long long DataLength;
typedef struct {
  __attribute__ ((aligned (32))) u64 chaining[grsnSIZE/8];      /* actual state */
  __attribute__ ((aligned (32))) BitSequence buffer[grsnSIZE];  /* data buffer */
  u64 block_counter;        /* message block counter */
  int buf_ptr;              /* data buffer pointer */
  int bits_in_last_byte;    /* no. of message bits in last byte of
                               data buffer */
  int columns;              /* no. of columns in state */
  int statesize;            /* total no. of bytes in state */
  Var v;                    /* LONG or SHORT */
} grsnState;

void grsnInit(grsnState*);
void grsnUpdate(grsnState*, const BitSequence*, DataLength);
void grsnFinal(grsnState*, BitSequence*);

#endif /* __hash_h */
