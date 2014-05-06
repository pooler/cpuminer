/* hash.h     Aug 2011
 *
 * Groestl implementation for different versions.
 * Author: Krystian Matusiewicz, Günther A. Roland, Martin Schläffer
 *
 * This code is placed in the public domain
 */

#ifndef __grsi_h
#define __grsi_h

#include <stdio.h>
#include <stdlib.h>

#include "brg_endian.h"
#define NEED_UINT_64T
#include "brg_types.h"

#define grsiLENGTH 512

/* some sizes (number of bytes) */
#define grsiROWS 8
#define grsiLENGTHFIELDLEN grsiROWS
#define grsiCOLS512 8
#define grsiCOLS1024 16
#define grsiSIZE512 (grsiROWS*grsiCOLS512)
#define grsiSIZE1024 (grsiROWS*grsiCOLS1024)
#define grsiROUNDS512 10
#define grsiROUNDS1024 14

#if grsiLENGTH<=256
#define grsiCOLS grsiCOLS512
#define grsiSIZE grsiSIZE512
#define grsiROUNDS grsiROUNDS512
#else
#define grsiCOLS grsiCOLS1024
#define grsiSIZE grsiSIZE1024
#define grsiROUNDS grsiROUNDS1024
#endif

#define ROTL64(a,n) ((((a)<<(n))|((a)>>(64-(n))))&li_64(ffffffffffffffff))

#if (PLATFORM_BYTE_ORDER == IS_BIG_ENDIAN)
#define grsiEXT_BYTE(var,n) ((u8)((u64)(var) >> (8*(7-(n)))))
#define grsiU64BIG(a) (a)
#endif /* IS_BIG_ENDIAN */

#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN)
#define grsiEXT_BYTE(var,n) ((u8)((u64)(var) >> (8*n)))
#define grsiU64BIG(a) \
  ((ROTL64(a, 8) & li_64(000000FF000000FF)) | \
   (ROTL64(a,24) & li_64(0000FF000000FF00)) | \
   (ROTL64(a,40) & li_64(00FF000000FF0000)) | \
   (ROTL64(a,56) & li_64(FF000000FF000000)))
#endif /* IS_LITTLE_ENDIAN */

typedef enum { LONG, SHORT } grsiVar;

/* NIST API begin */
typedef unsigned char grsiBitSequence;
typedef unsigned long long grsiDataLength;
typedef struct {
  __attribute__ ((aligned (32))) u64 grsichaining[grsiSIZE/8];      /* actual state */
  __attribute__ ((aligned (32))) grsiBitSequence grsibuffer[grsiSIZE];  /* data buffer */
  u64 grsiblock_counter;        /* message block counter */
  int grsibuf_ptr;              /* data buffer pointer */
  int grsibits_in_last_byte;    /* no. of message bits in last byte of
                               data buffer */
  int grsicolumns;              /* no. of columns in state */
  int grsistatesize;            /* total no. of bytes in state */
  grsiVar grsiv;                    /* LONG or SHORT */
} grsiState;

void grsiInit(grsiState*);
void grsiUpdate(grsiState*, const grsiBitSequence*, grsiDataLength);
void grsiFinal(grsiState*, grsiBitSequence*);
/* NIST API end   */

#endif /* __hash_h */
