/* CubeHash 16/32 is recommended for SHA-3 "normal", 16/1 for "formal" */
#define CUBEHASH_ROUNDS	16
#define CUBEHASH_BLOCKBYTES 32

#if defined(OPTIMIZE_SSE2)
#include <emmintrin.h>
#endif

#include "cubehash_sse2.h"
#include "defs_x5.h"

//enum { SUCCESS = 0, FAIL = 1, BAD_HASHBITLEN = 2 };

#if defined(OPTIMIZE_SSE2)

static void transform(cubehashParam *sp)
{
    int r;
    __m128i x0;
    __m128i x1;
    __m128i x2;
    __m128i x3;
    __m128i x4;
    __m128i x5;
    __m128i x6;
    __m128i x7;
    __m128i y0;
    __m128i y1;
    __m128i y2;
    __m128i y3;
#ifdef	UNUSED
    __m128i y4;
    __m128i y5;
    __m128i y6;
    __m128i y7;
#endif

    x0 = _mm_load_si128(0 + sp->x);
    x1 = _mm_load_si128(1 + sp->x);
    x2 = _mm_load_si128(2 + sp->x);
    x3 = _mm_load_si128(3 + sp->x);
    x4 = _mm_load_si128(4 + sp->x);
    x5 = _mm_load_si128(5 + sp->x);
    x6 = _mm_load_si128(6 + sp->x);
    x7 = _mm_load_si128(7 + sp->x);

    for (r = 0; r < sp->rounds; ++r) {
	x4 = _mm_add_epi32(x0, x4);
	x5 = _mm_add_epi32(x1, x5);
	x6 = _mm_add_epi32(x2, x6);
	x7 = _mm_add_epi32(x3, x7);
	y0 = x2;
	y1 = x3;
	y2 = x0;
	y3 = x1;
	x0 = _mm_xor_si128(_mm_slli_epi32(y0, 7), _mm_srli_epi32(y0, 25));
	x1 = _mm_xor_si128(_mm_slli_epi32(y1, 7), _mm_srli_epi32(y1, 25));
	x2 = _mm_xor_si128(_mm_slli_epi32(y2, 7), _mm_srli_epi32(y2, 25));
	x3 = _mm_xor_si128(_mm_slli_epi32(y3, 7), _mm_srli_epi32(y3, 25));
	x0 = _mm_xor_si128(x0, x4);
	x1 = _mm_xor_si128(x1, x5);
	x2 = _mm_xor_si128(x2, x6);
	x3 = _mm_xor_si128(x3, x7);
	x4 = _mm_shuffle_epi32(x4, 0x4e);
	x5 = _mm_shuffle_epi32(x5, 0x4e);
	x6 = _mm_shuffle_epi32(x6, 0x4e);
	x7 = _mm_shuffle_epi32(x7, 0x4e);
	x4 = _mm_add_epi32(x0, x4);
	x5 = _mm_add_epi32(x1, x5);
	x6 = _mm_add_epi32(x2, x6);
	x7 = _mm_add_epi32(x3, x7);
	y0 = x1;
	y1 = x0;
	y2 = x3;
	y3 = x2;
	x0 = _mm_xor_si128(_mm_slli_epi32(y0, 11), _mm_srli_epi32(y0, 21));
	x1 = _mm_xor_si128(_mm_slli_epi32(y1, 11), _mm_srli_epi32(y1, 21));
	x2 = _mm_xor_si128(_mm_slli_epi32(y2, 11), _mm_srli_epi32(y2, 21));
	x3 = _mm_xor_si128(_mm_slli_epi32(y3, 11), _mm_srli_epi32(y3, 21));
	x0 = _mm_xor_si128(x0, x4);
	x1 = _mm_xor_si128(x1, x5);
	x2 = _mm_xor_si128(x2, x6);
	x3 = _mm_xor_si128(x3, x7);
	x4 = _mm_shuffle_epi32(x4, 0xb1);
	x5 = _mm_shuffle_epi32(x5, 0xb1);
	x6 = _mm_shuffle_epi32(x6, 0xb1);
	x7 = _mm_shuffle_epi32(x7, 0xb1);
    }

    _mm_store_si128(0 + sp->x, x0);
    _mm_store_si128(1 + sp->x, x1);
    _mm_store_si128(2 + sp->x, x2);
    _mm_store_si128(3 + sp->x, x3);
    _mm_store_si128(4 + sp->x, x4);
    _mm_store_si128(5 + sp->x, x5);
    _mm_store_si128(6 + sp->x, x6);
    _mm_store_si128(7 + sp->x, x7);
}

#else	/* OPTIMIZE_SSE2 */

#define ROTATE(a,b) (((a) << (b)) | ((a) >> (32 - b)))

static void transform(cubehashParam *sp)
{
    uint32_t y[16];
    int i;
    int r;

    for (r = 0; r < sp->rounds; ++r) {
	for (i = 0; i < 16; ++i) sp->x[i + 16] += sp->x[i];
	for (i = 0; i < 16; ++i) y[i ^ 8] = sp->x[i];
	for (i = 0; i < 16; ++i) sp->x[i] = ROTATE(y[i],7);
	for (i = 0; i < 16; ++i) sp->x[i] ^= sp->x[i + 16];
	for (i = 0; i < 16; ++i) y[i ^ 2] = sp->x[i + 16];
	for (i = 0; i < 16; ++i) sp->x[i + 16] = y[i];
	for (i = 0; i < 16; ++i) sp->x[i + 16] += sp->x[i];
	for (i = 0; i < 16; ++i) y[i ^ 4] = sp->x[i];
	for (i = 0; i < 16; ++i) sp->x[i] = ROTATE(y[i],11);
	for (i = 0; i < 16; ++i) sp->x[i] ^= sp->x[i + 16];
	for (i = 0; i < 16; ++i) y[i ^ 1] = sp->x[i + 16];
	for (i = 0; i < 16; ++i) sp->x[i + 16] = y[i];
    }
}
#endif	/* OPTIMIZE_SSE2 */

int cubehashInit(cubehashParam *sp, int hashbitlen, int rounds, int blockbytes)
{
    int i;

    if (hashbitlen < 8) return BAD_HASHBITLEN;
    if (hashbitlen > 512) return BAD_HASHBITLEN;
    if (hashbitlen != 8 * (hashbitlen / 8)) return BAD_HASHBITLEN;

    /* Sanity checks */
    if (rounds <= 0 || rounds > 32) rounds = CUBEHASH_ROUNDS;
    if (blockbytes <= 0 || blockbytes >= 256) blockbytes = CUBEHASH_BLOCKBYTES;

    sp->hashbitlen = hashbitlen;
    sp->rounds = rounds;
    sp->blockbytes = blockbytes;
#if defined(OPTIMIZE_SSE2)
    for (i = 0; i < 8; ++i) sp->x[i] = _mm_set_epi32(0, 0, 0, 0);
    sp->x[0] = _mm_set_epi32(0, sp->rounds, sp->blockbytes, hashbitlen / 8);
#else
    for (i = 0; i < 32; ++i) sp->x[i] = 0;
    sp->x[0] = hashbitlen / 8;
    sp->x[1] = sp->blockbytes;
    sp->x[2] = sp->rounds;
#endif
    for (i = 0; i < 10; ++i) transform(sp);
    sp->pos = 0;
    return SUCCESS;
}

int
cubehashReset(cubehashParam *sp)
{
    return cubehashInit(sp, sp->hashbitlen, sp->rounds, sp->blockbytes);
}

int cubehashUpdate(cubehashParam *sp, const byte *data, size_t size)
{
    uint64_t databitlen = 8 * size;

    /* caller promises us that previous data had integral number of bytes */
    /* so sp->pos is a multiple of 8 */

    while (databitlen >= 8) {
#if defined(OPTIMIZE_SSE2)
	((unsigned char *) sp->x)[sp->pos / 8] ^= *data;
#else
	uint32_t u = *data;
	u <<= 8 * ((sp->pos / 8) % 4);
	sp->x[sp->pos / 32] ^= u;
#endif
	data += 1;
	databitlen -= 8;
	sp->pos += 8;
	if (sp->pos == 8 * sp->blockbytes) {
	    transform(sp);
	    sp->pos = 0;
	}
    }
    if (databitlen > 0) {
#if defined(OPTIMIZE_SSE2)
	((unsigned char *) sp->x)[sp->pos / 8] ^= *data;
#else
	uint32_t u = *data;
	u <<= 8 * ((sp->pos / 8) % 4);
	sp->x[sp->pos / 32] ^= u;
#endif
	sp->pos += databitlen;
    }
    return SUCCESS;
}

int cubehashDigest(cubehashParam *sp, byte *digest)
{
    int i;

#if defined(OPTIMIZE_SSE2)
    ((unsigned char *) sp->x)[sp->pos / 8] ^= (128 >> (sp->pos % 8));
    transform(sp);
    sp->x[7] = _mm_xor_si128(sp->x[7], _mm_set_epi32(1, 0, 0, 0));
    for (i = 0; i < 10; ++i) transform(sp);
    for (i = 0; i < sp->hashbitlen / 8; ++i)
	digest[i] = ((unsigned char *) sp->x)[i];
#else
    uint32_t u;

    u = (128 >> (sp->pos % 8));
    u <<= 8 * ((sp->pos / 8) % 4);
    sp->x[sp->pos / 32] ^= u;
    transform(sp);
    sp->x[31] ^= 1;
    for (i = 0; i < 10; ++i) transform(sp);
    for (i = 0; i < sp->hashbitlen / 8; ++i)
	digest[i] = sp->x[i / 4] >> (8 * (i % 4));
#endif

    return SUCCESS;
}