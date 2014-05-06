#ifndef __hash_h
#define __hash_h

#include <stdio.h>
#include <stdlib.h>
#include "brg_endian.h"
#include "brg_types.h"

/* some sizes (number of bytes) */
#define grsoROWS 8
#define grsoLENGTHFIELDLEN grsoROWS
#define grsoCOLS 16
#define grsoSIZE (grsoROWS*grsoCOLS)
#define grsoDIGESTSIZE 64

#define grsoROUNDS 14

#define grsoROTL64(a,n) ((((a)<<(n))|((a)>>(64-(n))))&((u64)0xffffffffffffffffULL))

#if (PLATFORM_BYTE_ORDER == IS_BIG_ENDIAN)
#error
#endif /* IS_BIG_ENDIAN */

#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN)
#define EXT_BYTE(var,n) ((u8)((u64)(var) >> (8*n)))
#define grsoU64BIG(a)				\
  ((grsoROTL64(a, 8) & ((u64)0x000000ff000000ffULL)) |	\
   (grsoROTL64(a,24) & ((u64)0x0000ff000000ff00ULL)) |	\
   (grsoROTL64(a,40) & ((u64)0x00ff000000ff0000ULL)) |	\
   (grsoROTL64(a,56) & ((u64)0xff000000ff000000ULL)))
#endif /* IS_LITTLE_ENDIAN */

typedef struct {
  u64 grsstate[grsoCOLS];             /* actual state */
  u64 grsblock_counter;           /* message block counter */
  int grsbuf_ptr;                 /* data buffer pointer */
} grsoState;

extern int grsoInit(grsoState* ctx); 
extern int grsoUpdate(grsoState* ctx, const unsigned char* in,
	   unsigned long long len);
extern int grsoUpdateq(grsoState* ctx, const unsigned char* in);
extern int grsoFinal(grsoState* ctx,
	  unsigned char* out); 

extern int grsohash(unsigned char *out,
		const unsigned char *in,
		unsigned long long len);

#endif /* __hash_h */
