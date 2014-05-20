
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

#include "config.h"
#include "rounds.h"
/*
#ifndef NOT_SUPERCOP

#include "crypto_hash.h"
#include "crypto_uint64.h"
#include "crypto_uint32.h"
#include "crypto_uint8.h"

typedef crypto_uint64 u64;
typedef crypto_uint32 u32;
typedef crypto_uint8 u8; 

#else
*/
typedef unsigned long long u64; 
typedef unsigned int u32; 
typedef unsigned char u8; 

typedef struct  
{ 
	__m128i h[4];
  u64 s[4], t[2];
  u32 buflen, nullt;
  u8 buf[128];
} hashState_blake __attribute__ ((aligned (64)));
/*
#endif

#define U8TO32(p) \
  (((u32)((p)[0]) << 24) | ((u32)((p)[1]) << 16) | \
   ((u32)((p)[2]) <<  8) | ((u32)((p)[3])      ))
#define U8TO64(p) \
  (((u64)U8TO32(p) << 32) | (u64)U8TO32((p) + 4))
#define U32TO8(p, v) \
    (p)[0] = (u8)((v) >> 24); (p)[1] = (u8)((v) >> 16); \
    (p)[2] = (u8)((v) >>  8); (p)[3] = (u8)((v)      ); 
#define U64TO8(p, v) \
    U32TO8((p),     (u32)((v) >> 32));	\
    U32TO8((p) + 4, (u32)((v)      )); 
*/

/*
static const u8 padding[129] =
{ 
	0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

*/
static inline void blake512_init( hashState_blake * S, u64 datalen );


static void blake512_update( hashState_blake * S, const u8 * data, u64 datalen ) ;

static inline void blake512_final( hashState_blake * S, u8 * digest ) ;


int crypto_hash( unsigned char *out, const unsigned char *in, unsigned long long inlen ) ;






