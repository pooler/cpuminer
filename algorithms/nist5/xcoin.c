// Nist5
#include <boost/preprocessor/cat.hpp> // to generate function names

// Define function names
#define XHASH_ALGO         BOOST_PP_CAT(Xhash,ALGO_NAME)
#define INIT_ALGO          BOOST_PP_CAT(init,ALGO_NAME)
#define THREAD_INIT_ALGO   BOOST_PP_CAT(thread_init,ALGO_NAME)
#define SCANHASH_ALGO      BOOST_PP_CAT(scanhash,ALGO_NAME)
#define PARAM_DEFAULT_ALGO BOOST_PP_CAT(param_default,ALGO_NAME)
#define PARAM_PARSE_ALGO   BOOST_PP_CAT(param_parse,ALGO_NAME)


#include "cpuminer-config.h"
#include "miner.h"


#include <string.h>
#include <stdint.h>

//--
#include "x5/luffa_for_sse2.h" //sse2 opt
//----
#include "x5/cubehash_sse2.h" //sse2 opt
//--------------------------
#include "x5/sph_shavite.h"
//-----simd vect128---------
#include "x5/vect128/nist.h"
//-----------

#if HAVE_AES_NI
#define AES_NI_GR
#include "x5/echo512/ccalik/aesni/hash_api.h"
#else
#include "x5/sph_echo.h"
#endif


//----
#include "x6/blake.c"
//#include "x5/blake/sse41/hash.c"
#include "x6/bmw.c"
#include "x6/keccak.c"
#include "x6/skein.c"
#include "x6/jh_sse2_opt64.h"
//#include "groestl.c"
#ifdef AES_NI_GR
#include "x6/groestl/aesni/hash-groestl.h"
#else
#if 1
#include "x6/grso.h"
#ifndef PROFILERUN
#include "x6/grso-asm.h"
#endif
#else
#include "x6/grss_api.h"
#endif
#endif  //AES-NI_GR


#ifdef X13
#include "hash/sph_hamsi.h"
#include "hash/sph_fugue.h"
#ifdef X15
#include "hash/sph_shabal.h"
#include "hash/sph_whirlpool.h"
#endif

#endif

/*define data alignment for different C compilers*/
#if defined(__GNUC__)
#define DATA_ALIGNXY(x,y) x __attribute__ ((aligned(y)))
#else
#define DATA_ALIGNXY(x,y) __declspec(align(y)) x
#endif

#if HAVE_AES_NI
#ifdef AES_NI_GR
typedef struct {
    sph_shavite512_context  shavite1;
    hashState_echo		echo1;
    hashState_groestl groestl;
    hashState_luffa luffa;
    cubehashParam cubehash;
    hashState_sd ctx_simd1;
//	hashState_blake	blake1;
#ifdef X13
    sph_hamsi512_context	hamsi1;
    sph_fugue512_context    fugue1;
#ifdef X15
    sph_shabal512_context    shabal;
    sph_whirlpool_context    whirlpool;
#endif
#endif
} Xhash_context_holder;
#else
typedef struct {
    sph_shavite512_context shavite1;
    hashState_echo	echo1;
    hashState_luffa	luffa;
    cubehashParam cubehash;
    hashState_sd ctx_simd1;
#ifdef X13
    sph_hamsi512_context	hamsi1;
    sph_fugue512_context    fugue1;
#ifdef X15
    sph_shabal512_context    shabal;
    sph_whirlpool_context    whirlpool;
#endif
#endif
} Xhash_context_holder;
#endif
#else
typedef struct {
    sph_shavite512_context  shavite1;
    sph_echo512_context		echo1;
    hashState_luffa	luffa;
    cubehashParam	cubehash;
    hashState_sd ctx_simd1;
//	hashState_blake	blake1;
#ifdef X13
    sph_hamsi512_context	hamsi1;
    sph_fugue512_context    fugue1;
#ifdef X15
    sph_shabal512_context    shabal;
    sph_whirlpool_context    whirlpool;
#endif
#endif
} Xhash_context_holder;
#endif

Xhash_context_holder base_contexts;


int INIT_ALGO() {

    //---local simd var ---
    init_sd(&base_contexts.ctx_simd1,512);

#ifdef X13
    sph_hamsi512_init(&base_contexts.hamsi1);
    sph_fugue512_init(&base_contexts.fugue1);
#ifdef X15
    sph_shabal512_init(&base_contexts.shabal);
    sph_whirlpool_init(&base_contexts.whirlpool);
#endif // X15
#endif // X13
    return 0; // 0 == success
}

void* THREAD_INIT_ALGO (int* error) {
    *error = 0;	// 0 == no error
    return NULL;
}

void XHASH_ALGO (void *state, const void *input)
{
    Xhash_context_holder ctx;

//	uint32_t hashA[16], hashB[16];


    memcpy(&ctx, &base_contexts, sizeof(base_contexts));
#ifdef AES_NI_GR
    init_groestl(&ctx.groestl);
#endif

    DATA_ALIGNXY(unsigned char hashbuf[128],16);
    size_t hashptr;
    DATA_ALIGNXY(sph_u64 hashctA,8);
    DATA_ALIGNXY(sph_u64 hashctB,8);

#ifndef AES_NI_GR
    grsoState sts_grs;
#endif


    DATA_ALIGNXY(unsigned char hash[128],16);
    /* proably not needed */
    memset(hash, 0, 128);
    //blake1-bmw2-grs3-skein4-jh5-keccak6-luffa7-cubehash8-shavite9-simd10-echo11
    //---blake1---
    /*	  //blake init
    	blake512_init(&base_contexts.blake1, 512);
    	blake512_update(&ctx.blake1, input, 512);
    	blake512_final(&ctx.blake1, hash);
    */
    DECL_BLK;
    BLK_I;
    BLK_W;
    BLK_C;


#ifdef AES_NI_GR
    update_groestl(&ctx.groestl, (char*)hash,512);
    final_groestl(&ctx.groestl, (char*)hash);
#else
    GRS_I;
    GRS_U;
    GRS_C;
#endif
    //---jh------
    DECL_JH;
    JH_H;
    //---keccak---
    DECL_KEC;
    KEC_I;
    KEC_U;
    KEC_C;
    //---skein---
    DECL_SKN;
    SKN_I;
    SKN_U;
    SKN_C;

    memcpy(state, hash, 32);
}

// plugin entry func
int SCANHASH_ALGO(int thr_id, uint32_t *pdata, void *scratchbuf, const uint32_t *ptarget, uint32_t max_nonce, unsigned long *hashes_done, void *extra_param) {

    uint32_t n = pdata[19] - 1;
    const uint32_t first_nonce = pdata[19];
    const uint32_t Htarg = ptarget[7];

    uint32_t hash64[8] __attribute__((aligned(32)));
    uint32_t endiandata[32];


    int kk=0;

#pragma unroll
    for (; kk < 32; kk++) {
        be32enc(&endiandata[kk], ((uint32_t*)pdata)[kk]);
    };

    if (ptarget[7]==0) {
        do {
            pdata[19] = ++n;
            be32enc(&endiandata[19], n);
            XHASH_ALGO(hash64, &endiandata);
            if (((hash64[7]&0xFFFFFFFF)==0) &&
                    fulltest(hash64, ptarget)) {
                *hashes_done = n - first_nonce + 1;
                return true;
            }
        } while (n < max_nonce && !work_restart[thr_id].restart);
    }
    else if (ptarget[7]<=0xF)
    {
        do {
            pdata[19] = ++n;
            be32enc(&endiandata[19], n);
            XHASH_ALGO(hash64, &endiandata);
            if (((hash64[7]&0xFFFFFFF0)==0) &&
                    fulltest(hash64, ptarget)) {
                *hashes_done = n - first_nonce + 1;
                return true;
            }
        } while (n < max_nonce && !work_restart[thr_id].restart);
    }
    else if (ptarget[7]<=0xFF)
    {
        do {
            pdata[19] = ++n;
            be32enc(&endiandata[19], n);
            XHASH_ALGO(hash64, &endiandata);
            if (((hash64[7]&0xFFFFFF00)==0) &&
                    fulltest(hash64, ptarget)) {
                *hashes_done = n - first_nonce + 1;
                return true;
            }
        } while (n < max_nonce && !work_restart[thr_id].restart);
    }
    else if (ptarget[7]<=0xFFF)
    {
        do {
            pdata[19] = ++n;
            be32enc(&endiandata[19], n);
            XHASH_ALGO(hash64, &endiandata);
            if (((hash64[7]&0xFFFFF000)==0) &&
                    fulltest(hash64, ptarget)) {
                *hashes_done = n - first_nonce + 1;
                return true;
            }
        } while (n < max_nonce && !work_restart[thr_id].restart);

    }
    else if (ptarget[7]<=0xFFFF)
    {
        do {
            pdata[19] = ++n;
            be32enc(&endiandata[19], n);
            XHASH_ALGO(hash64, &endiandata);
            if (((hash64[7]&0xFFFF0000)==0) &&
                    fulltest(hash64, ptarget)) {
                *hashes_done = n - first_nonce + 1;
                return true;
            }
        } while (n < max_nonce && !work_restart[thr_id].restart);

    }
    else
    {
        do {
            pdata[19] = ++n;
            be32enc(&endiandata[19], n);
            XHASH_ALGO(hash64, &endiandata);
            if (fulltest(hash64, ptarget)) {
                *hashes_done = n - first_nonce + 1;
                return true;
            }
        } while (n < max_nonce && !work_restart[thr_id].restart);
    }
    *hashes_done = n - first_nonce + 1;
    pdata[19] = n;
    return 0;
}



void *PARAM_DEFAULT_ALGO() {
    return NULL; // unused
}

void *PARAM_PARSE_ALGO( const char *str, int *error) {
    *error = 0;
    return NULL; // unused
}

