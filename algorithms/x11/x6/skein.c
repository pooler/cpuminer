/* $Id: skein.c 254 2011-06-07 19:38:58Z tp $ */
/*
 * Skein implementation.
 *
 * ==========================(LICENSE BEGIN)============================
 *
 * Copyright (c) 2007-2010  Projet RNRT SAPHIR
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ===========================(LICENSE END)=============================
 *
 * @author   Thomas Pornin <thomas.pornin@cryptolog.com>
 */

#include <stddef.h>
#include <string.h>

#include "sph_skein.h"

#ifdef __cplusplus
extern "C"{
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4146)
#endif



/*
 * M9_ ## s ## _ ## i  evaluates to s+i mod 9 (0 <= s <= 18, 0 <= i <= 7).
 */

#define M9_0_0    0
#define M9_0_1    1
#define M9_0_2    2
#define M9_0_3    3
#define M9_0_4    4
#define M9_0_5    5
#define M9_0_6    6
#define M9_0_7    7

#define M9_1_0    1
#define M9_1_1    2
#define M9_1_2    3
#define M9_1_3    4
#define M9_1_4    5
#define M9_1_5    6
#define M9_1_6    7
#define M9_1_7    8

#define M9_2_0    2
#define M9_2_1    3
#define M9_2_2    4
#define M9_2_3    5
#define M9_2_4    6
#define M9_2_5    7
#define M9_2_6    8
#define M9_2_7    0

#define M9_3_0    3
#define M9_3_1    4
#define M9_3_2    5
#define M9_3_3    6
#define M9_3_4    7
#define M9_3_5    8
#define M9_3_6    0
#define M9_3_7    1

#define M9_4_0    4
#define M9_4_1    5
#define M9_4_2    6
#define M9_4_3    7
#define M9_4_4    8
#define M9_4_5    0
#define M9_4_6    1
#define M9_4_7    2

#define M9_5_0    5
#define M9_5_1    6
#define M9_5_2    7
#define M9_5_3    8
#define M9_5_4    0
#define M9_5_5    1
#define M9_5_6    2
#define M9_5_7    3

#define M9_6_0    6
#define M9_6_1    7
#define M9_6_2    8
#define M9_6_3    0
#define M9_6_4    1
#define M9_6_5    2
#define M9_6_6    3
#define M9_6_7    4

#define M9_7_0    7
#define M9_7_1    8
#define M9_7_2    0
#define M9_7_3    1
#define M9_7_4    2
#define M9_7_5    3
#define M9_7_6    4
#define M9_7_7    5

#define M9_8_0    8
#define M9_8_1    0
#define M9_8_2    1
#define M9_8_3    2
#define M9_8_4    3
#define M9_8_5    4
#define M9_8_6    5
#define M9_8_7    6

#define M9_9_0    0
#define M9_9_1    1
#define M9_9_2    2
#define M9_9_3    3
#define M9_9_4    4
#define M9_9_5    5
#define M9_9_6    6
#define M9_9_7    7

#define M9_10_0   1
#define M9_10_1   2
#define M9_10_2   3
#define M9_10_3   4
#define M9_10_4   5
#define M9_10_5   6
#define M9_10_6   7
#define M9_10_7   8

#define M9_11_0   2
#define M9_11_1   3
#define M9_11_2   4
#define M9_11_3   5
#define M9_11_4   6
#define M9_11_5   7
#define M9_11_6   8
#define M9_11_7   0

#define M9_12_0   3
#define M9_12_1   4
#define M9_12_2   5
#define M9_12_3   6
#define M9_12_4   7
#define M9_12_5   8
#define M9_12_6   0
#define M9_12_7   1

#define M9_13_0   4
#define M9_13_1   5
#define M9_13_2   6
#define M9_13_3   7
#define M9_13_4   8
#define M9_13_5   0
#define M9_13_6   1
#define M9_13_7   2

#define M9_14_0   5
#define M9_14_1   6
#define M9_14_2   7
#define M9_14_3   8
#define M9_14_4   0
#define M9_14_5   1
#define M9_14_6   2
#define M9_14_7   3

#define M9_15_0   6
#define M9_15_1   7
#define M9_15_2   8
#define M9_15_3   0
#define M9_15_4   1
#define M9_15_5   2
#define M9_15_6   3
#define M9_15_7   4

#define M9_16_0   7
#define M9_16_1   8
#define M9_16_2   0
#define M9_16_3   1
#define M9_16_4   2
#define M9_16_5   3
#define M9_16_6   4
#define M9_16_7   5

#define M9_17_0   8
#define M9_17_1   0
#define M9_17_2   1
#define M9_17_3   2
#define M9_17_4   3
#define M9_17_5   4
#define M9_17_6   5
#define M9_17_7   6

#define M9_18_0   0
#define M9_18_1   1
#define M9_18_2   2
#define M9_18_3   3
#define M9_18_4   4
#define M9_18_5   5
#define M9_18_6   6
#define M9_18_7   7

/*
 * M3_ ## s ## _ ## i  evaluates to s+i mod 3 (0 <= s <= 18, 0 <= i <= 1).
 */

#define M3_0_0    0
#define M3_0_1    1
#define M3_1_0    1
#define M3_1_1    2
#define M3_2_0    2
#define M3_2_1    0
#define M3_3_0    0
#define M3_3_1    1
#define M3_4_0    1
#define M3_4_1    2
#define M3_5_0    2
#define M3_5_1    0
#define M3_6_0    0
#define M3_6_1    1
#define M3_7_0    1
#define M3_7_1    2
#define M3_8_0    2
#define M3_8_1    0
#define M3_9_0    0
#define M3_9_1    1
#define M3_10_0   1
#define M3_10_1   2
#define M3_11_0   2
#define M3_11_1   0
#define M3_12_0   0
#define M3_12_1   1
#define M3_13_0   1
#define M3_13_1   2
#define M3_14_0   2
#define M3_14_1   0
#define M3_15_0   0
#define M3_15_1   1
#define M3_16_0   1
#define M3_16_1   2
#define M3_17_0   2
#define M3_17_1   0
#define M3_18_0   0
#define M3_18_1   1

#define XCAT(x, y)     XCAT_(x, y)
#define XCAT_(x, y)    x ## y

#define SKBI(k, s, i)   XCAT(k, XCAT(XCAT(XCAT(M9_, s), _), i))
#define SKBT(t, s, v)   XCAT(t, XCAT(XCAT(XCAT(M3_, s), _), v))

#define TFBIG_KINIT(k0, k1, k2, k3, k4, k5, k6, k7, k8, t0, t1, t2)   do { \
		k8 = ((k0 ^ k1) ^ (k2 ^ k3)) ^ ((k4 ^ k5) ^ (k6 ^ k7)) \
			^ SPH_C64(0x1BD11BDAA9FC1A22); \
		t2 = t0 ^ t1; \
	} while (0)

#define TFBIG_ADDKEY(w0, w1, w2, w3, w4, w5, w6, w7, k, t, s)   do { \
		w0 = SPH_T64(w0 + SKBI(k, s, 0)); \
		w1 = SPH_T64(w1 + SKBI(k, s, 1)); \
		w2 = SPH_T64(w2 + SKBI(k, s, 2)); \
		w3 = SPH_T64(w3 + SKBI(k, s, 3)); \
		w4 = SPH_T64(w4 + SKBI(k, s, 4)); \
		w5 = SPH_T64(w5 + SKBI(k, s, 5) + SKBT(t, s, 0)); \
		w6 = SPH_T64(w6 + SKBI(k, s, 6) + SKBT(t, s, 1)); \
		w7 = SPH_T64(w7 + SKBI(k, s, 7) + (sph_u64)s); \
	} while (0)


#define TFBIG_MIX(x0, x1, rc)   do { \
		x0 = SPH_T64(x0 + x1); \
		x1 = SPH_ROTL64(x1, rc) ^ x0; \
	} while (0)

#define TFBIG_MIX8(w0, w1, w2, w3, w4, w5, w6, w7, rc0, rc1, rc2, rc3)  do { \
		TFBIG_MIX(w0, w1, rc0); \
		TFBIG_MIX(w2, w3, rc1); \
		TFBIG_MIX(w4, w5, rc2); \
		TFBIG_MIX(w6, w7, rc3); \
	} while (0)

#define TFBIG_4e(s)   do { \
		TFBIG_ADDKEY(p0, p1, p2, p3, p4, p5, p6, p7, sknh, t, s); \
		TFBIG_MIX8(p0, p1, p2, p3, p4, p5, p6, p7, 46, 36, 19, 37); \
		TFBIG_MIX8(p2, p1, p4, p7, p6, p5, p0, p3, 33, 27, 14, 42); \
		TFBIG_MIX8(p4, p1, p6, p3, p0, p5, p2, p7, 17, 49, 36, 39); \
		TFBIG_MIX8(p6, p1, p0, p7, p2, p5, p4, p3, 44,  9, 54, 56); \
	} while (0)

#define TFBIG_4o(s)   do { \
		TFBIG_ADDKEY(p0, p1, p2, p3, p4, p5, p6, p7, sknh, t, s); \
		TFBIG_MIX8(p0, p1, p2, p3, p4, p5, p6, p7, 39, 30, 34, 24); \
		TFBIG_MIX8(p2, p1, p4, p7, p6, p5, p0, p3, 13, 50, 10, 17); \
		TFBIG_MIX8(p4, p1, p6, p3, p0, p5, p2, p7, 25, 29, 39, 43); \
		TFBIG_MIX8(p6, p1, p0, p7, p2, p5, p4, p3,  8, 35, 56, 22); \
	} while (0)

#define UBI_BIG(etype, extra)  do { \
		sph_u64 sknh8, t0, t1, t2; \
		sph_u64 m0 = sph_dec64le_aligned(buf +  0); \
		sph_u64 m1 = sph_dec64le_aligned(buf +  8); \
		sph_u64 m2 = sph_dec64le_aligned(buf + 16); \
		sph_u64 m3 = sph_dec64le_aligned(buf + 24); \
		sph_u64 m4 = sph_dec64le_aligned(buf + 32); \
		sph_u64 m5 = sph_dec64le_aligned(buf + 40); \
		sph_u64 m6 = sph_dec64le_aligned(buf + 48); \
		sph_u64 m7 = sph_dec64le_aligned(buf + 56); \
		sph_u64 p0 = m0; \
		sph_u64 p1 = m1; \
		sph_u64 p2 = m2; \
		sph_u64 p3 = m3; \
		sph_u64 p4 = m4; \
		sph_u64 p5 = m5; \
		sph_u64 p6 = m6; \
		sph_u64 p7 = m7; \
		t0 = SPH_T64(hashctA << 6) + (sph_u64)(extra); \
		t1 = (hashctA >> 58) + ((sph_u64)(etype) << 55); \
		TFBIG_KINIT(sknh0, sknh1, sknh2, sknh3, sknh4, sknh5, sknh6, sknh7, sknh8, t0, t1, t2); \
		TFBIG_4e(0); \
		TFBIG_4o(1); \
		TFBIG_4e(2); \
		TFBIG_4o(3); \
		TFBIG_4e(4); \
		TFBIG_4o(5); \
		TFBIG_4e(6); \
		TFBIG_4o(7); \
		TFBIG_4e(8); \
		TFBIG_4o(9); \
		TFBIG_4e(10); \
		TFBIG_4o(11); \
		TFBIG_4e(12); \
		TFBIG_4o(13); \
		TFBIG_4e(14); \
		TFBIG_4o(15); \
		TFBIG_4e(16); \
		TFBIG_4o(17); \
		TFBIG_ADDKEY(p0, p1, p2, p3, p4, p5, p6, p7, sknh, t, 18); \
		sknh0 = m0 ^ p0; \
		sknh1 = m1 ^ p1; \
		sknh2 = m2 ^ p2; \
		sknh3 = m3 ^ p3; \
		sknh4 = m4 ^ p4; \
		sknh5 = m5 ^ p5; \
		sknh6 = m6 ^ p6; \
		sknh7 = m7 ^ p7; \
	} while (0)


#define sknDECL_STATE_BIG \
	sph_u64 sknh0, sknh1, sknh2, sknh3, sknh4, sknh5, sknh6, sknh7; \

#define DECL_SKN \
	sph_u64 sknh0, sknh1, sknh2, sknh3, sknh4, sknh5, sknh6, sknh7; \
	unsigned char sknbuf[64]; \

#define sknREAD_STATE_BIG(sc)   do { \
		sknh0 = (sc)->sknh0; \
		sknh1 = (sc)->sknh1; \
		sknh2 = (sc)->sknh2; \
		sknh3 = (sc)->sknh3; \
		sknh4 = (sc)->sknh4; \
		sknh5 = (sc)->sknh5; \
		sknh6 = (sc)->sknh6; \
		sknh7 = (sc)->sknh7; \
	} while (0)

#define sknWRITE_STATE_BIG(sc)   do { \
		(sc)->sknh0 = sknh0; \
		(sc)->sknh1 = sknh1; \
		(sc)->sknh2 = sknh2; \
		(sc)->sknh3 = sknh3; \
		(sc)->sknh4 = sknh4; \
		(sc)->sknh5 = sknh5; \
		(sc)->sknh6 = sknh6; \
		(sc)->sknh7 = sknh7; \
	} while (0)


/* not used */
#define SKN_H \
do { \
    sph_skein512_init(&ctx_skein); \
    skein_big_core(&ctx_skein, hash,64); \
    sph_skein512_close(&ctx_skein, hash); \
} while (0)

/* load initial constants */
#define SKN_I \
do { \
	sknh0 = sknIV512[0]; \
	sknh1 = sknIV512[1]; \
	sknh2 = sknIV512[2]; \
	sknh3 = sknIV512[3]; \
	sknh4 = sknIV512[4]; \
	sknh5 = sknIV512[5]; \
	sknh6 = sknIV512[6]; \
	sknh7 = sknIV512[7]; \
	hashctA = 0; \
	hashptr = 0; \
} while (0)

/* load hash for loop */
#define SKN_U \
do { \
	unsigned char *buf; \
	size_t ptr; \
	unsigned first; \
        size_t len = 64; \
        const void *data = hash; \
	buf = hashbuf; \
	ptr = hashptr; \
        memcpy(buf + ptr, data, len); \
	ptr += len; \
	hashptr = ptr; \
} while (0)

/* skein512 hash loaded */
/* hash = skein512(loaded) */
#define SKN_C \
do { \
	unsigned char *buf; \
	size_t ptr; \
	unsigned et; \
	int i; \
 \
	buf = hashbuf; \
	ptr = hashptr; \
 \
	memset(buf + ptr, 0, (sizeof(char)*64) - ptr); \
        /* for break loop */ \
        /* one copy of inline UBI_BIG */ \
	    et = 352 + ((hashctA == 0) << 7) + (0 != 0); \
	for (;;) { \
        UBI_BIG(et, ptr); \
            /* et gets changed for 2nd run */ \
            if (et == 510) break; \
            memset(buf, 0, (sizeof(char)*64)); \
            hashctA = 0; \
            et = 510; \
            ptr = 8; \
	} \
 \
	sph_enc64le_aligned(buf +  0, sknh0); \
	sph_enc64le_aligned(buf +  8, sknh1); \
	sph_enc64le_aligned(buf + 16, sknh2); \
	sph_enc64le_aligned(buf + 24, sknh3); \
	sph_enc64le_aligned(buf + 32, sknh4); \
	sph_enc64le_aligned(buf + 40, sknh5); \
	sph_enc64le_aligned(buf + 48, sknh6); \
	sph_enc64le_aligned(buf + 56, sknh7); \
	memcpy(hash, buf, 64); \
 \
} while (0)

static const sph_u64 sknIV512[] = {
	SPH_C64(0x4903ADFF749C51CE), SPH_C64(0x0D95DE399746DF03),
	SPH_C64(0x8FD1934127C79BCE), SPH_C64(0x9A255629FF352CB1),
	SPH_C64(0x5DB62599DF6CA7B0), SPH_C64(0xEABE394CA9D5C3F4),
	SPH_C64(0x991112C71A75B523), SPH_C64(0xAE18A40B660FCC33)
};


#ifdef __cplusplus
}
#endif
