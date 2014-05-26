/* hash.h     Aug 2011
 *
 * Groestl implementation for different versions.
 * Author: Krystian Matusiewicz, Günther A. Roland, Martin Schläffer
 *
 * This code is placed in the public domain
 */

#ifndef __grsv_h
#define __grsv_h

#include <stdio.h>
#include <stdlib.h>

#include "brg_endian.h"
#define NEED_UINT_64T
#include "brg_types.h"

#define grsvLENGTH 512

/* some sizes (number of bytes) */
#define grsvROWS 8
#define grsvLENGTHFIELDLEN grsvROWS
#define grsvCOLS512 8
#define grsvCOLS1024 16
#define grsvSIZE512 (grsvROWS*grsvCOLS512)
#define grsvSIZE1024 (grsvROWS*grsvCOLS1024)
#define grsvROUNDS512 10
#define grsvROUNDS1024 14

#if grsvLENGTH<=256
#define grsvCOLS grsvCOLS512
#define grsvSIZE grsvSIZE512
#define grsvROUNDS grsvROUNDS512
#else
#define grsvCOLS grsvCOLS1024
#define grsvSIZE grsvSIZE1024
#define grsvROUNDS grsvROUNDS1024
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

typedef enum { LONG, SHORT } grsvVar;

typedef unsigned char grsvBitSequence;
typedef unsigned long long grsvDataLength;
typedef struct {
  __attribute__ ((aligned (32))) u64 grsvchaining[grsvSIZE/8];      /* actual state */
  __attribute__ ((aligned (32))) grsvBitSequence grsvbuffer[grsvSIZE];  /* data buffer */
  u64 grsvblock_counter;        /* message block counter */
  int grsvbuf_ptr;              /* data buffer pointer */
  int grsvbits_in_last_byte;    /* no. of message bits in last byte of
                               data buffer */
  int grsvcolumns;              /* no. of columns in state */
  int grsvstatesize;            /* total no. of bytes in state */
  grsvVar grsvv;                    /* LONG or SHORT */
} grsvState;

void grsvInit(grsvState*);
void grsvUpdate(grsvState*, const grsvBitSequence*, grsvDataLength);
void grsvFinal(grsvState*, grsvBitSequence*);

#endif /* __grsv_h */
