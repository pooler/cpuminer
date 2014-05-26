#ifndef CUBEHASH_SSE2_H__
#define CUBEHASH_SSE2_H__

#include <stdint.h>
#include "defs_x5.h"
//#include <beecrypt/beecrypt.h>

//#if defined(__SSE2__)
#define	OPTIMIZE_SSE2
//#endif

#if defined(OPTIMIZE_SSE2)
#include <emmintrin.h>
#endif

/*!\brief Holds all the parameters necessary for the CUBEHASH algorithm.
 * \ingroup HASH_cubehash_m
 */

struct _cubehashParam
//#endif
{
    int hashbitlen;
    int rounds;
    int blockbytes;
    int pos;		/* number of bits read into x from current block */
#if defined(OPTIMIZE_SSE2)
    __m128i x[8];
#else
    uint32_t x[32];
#endif
};

//#ifndef __cplusplus
typedef struct _cubehashParam cubehashParam;
//#endif

#ifdef __cplusplus
extern "C" {
#endif

/*!\var cubehash256
 * \brief Holds the full API description of the CUBEHASH algorithm.
 */
//extern BEECRYPTAPI const hashFunction cubehash256;

//BEECRYPTAPI
int cubehashInit(cubehashParam* sp, int hashbitlen, int rounds, int blockbytes);

//BEECRYPTAPI
int cubehashReset(cubehashParam* sp);

//BEECRYPTAPI
int cubehashUpdate(cubehashParam* sp, const byte *data, size_t size);

//BEECRYPTAPI
int cubehashDigest(cubehashParam* sp, byte *digest);

#ifdef __cplusplus
}
#endif

#endif /* H_CUBEHASH */