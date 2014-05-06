/*
 * luffa_for_sse2.c
 * Version 2.0 (Sep 15th 2009)
 *
 * Copyright (C) 2008-2009 Hitachi, Ltd. All rights reserved.
 *
 * Hitachi, Ltd. is the owner of this software and hereby grant
 * the U.S. Government and any interested party the right to use
 * this software for the purposes of the SHA-3 evaluation process,
 * notwithstanding that this software is copyrighted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <string.h>
#include <emmintrin.h>
#include "luffa_for_sse2.h"

#ifdef HASH_BIG_ENDIAN
# define BYTES_SWAP32(x) x
#else
# define BYTES_SWAP32(x) \
    ((x << 24) | ((x & 0x0000ff00) << 8) | ((x & 0x00ff0000) >> 8) | (x >> 24))
#endif /* HASH_BIG_ENDIAN */

/* BYTES_SWAP256(x) stores each 32-bit word of 256 bits data in little-endian convention */
#define BYTES_SWAP256(x) { \
    int _i = 8; while(_i--){x[_i] = BYTES_SWAP32(x[_i]);} \
}

#define MULT2(a0,a1,t0,t1)\
    t0 = _mm_load_si128(&a1);\
    t0 = _mm_and_si128(t0,MASK);\
    t0 = _mm_shuffle_epi32(t0,16);\
    a0 = _mm_xor_si128(a0,t0);\
    t0 = _mm_load_si128(&a0);\
    t1 = _mm_load_si128(&a1);\
    a0 = _mm_srli_si128(a0,4);\
    a1 = _mm_srli_si128(a1,4);\
    t0 = _mm_slli_si128(t0,12);\
    t1 = _mm_slli_si128(t1,12);\
    a0 = _mm_or_si128(a0,t1);\
    a1 = _mm_or_si128(a1,t0);

#define STEP_PART(x,c,t)\
    SUBCRUMB(*x,*(x+1),*(x+2),*(x+3),*t);\
    SUBCRUMB(*(x+5),*(x+6),*(x+7),*(x+4),*t);\
    MIXWORD(*x,*(x+4),*t,*(t+1));\
    MIXWORD(*(x+1),*(x+5),*t,*(t+1));\
    MIXWORD(*(x+2),*(x+6),*t,*(t+1));\
    MIXWORD(*(x+3),*(x+7),*t,*(t+1));\
    ADD_CONSTANT(*x, *(x+4), *c, *(c+1));

#define STEP_PART2(a0,a1,t0,t1,c0,c1,tmp0,tmp1)\
    a1 = _mm_shuffle_epi32(a1,147);\
    t0 = _mm_load_si128(&a1);\
    a1 = _mm_unpacklo_epi32(a1,a0);\
    t0 = _mm_unpackhi_epi32(t0,a0);\
    t1 = _mm_shuffle_epi32(t0,78);\
    a0 = _mm_shuffle_epi32(a1,78);\
    SUBCRUMB(t1,t0,a0,a1,tmp0);\
    t0 = _mm_unpacklo_epi32(t0,t1);\
    a1 = _mm_unpacklo_epi32(a1,a0);\
    a0 = _mm_load_si128(&a1);\
    a0 = _mm_unpackhi_epi64(a0,t0);\
    a1 = _mm_unpacklo_epi64(a1,t0);\
    a1 = _mm_shuffle_epi32(a1,57);\
    MIXWORD(a0,a1,tmp0,tmp1);\
    ADD_CONSTANT(a0,a1,c0,c1);

#define SUBCRUMB(a0,a1,a2,a3,t)\
    t  = _mm_load_si128(&a0);\
    a0 = _mm_or_si128(a0,a1);\
    a2 = _mm_xor_si128(a2,a3);\
    a1 = _mm_andnot_si128(a1,ALLONE);\
    a0 = _mm_xor_si128(a0,a3);\
    a3 = _mm_and_si128(a3,t);\
    a1 = _mm_xor_si128(a1,a3);\
    a3 = _mm_xor_si128(a3,a2);\
    a2 = _mm_and_si128(a2,a0);\
    a0 = _mm_andnot_si128(a0,ALLONE);\
    a2 = _mm_xor_si128(a2,a1);\
    a1 = _mm_or_si128(a1,a3);\
    t  = _mm_xor_si128(t,a1);\
    a3 = _mm_xor_si128(a3,a2);\
    a2 = _mm_and_si128(a2,a1);\
    a1 = _mm_xor_si128(a1,a0);\
    a0 = _mm_load_si128(&t);\

#define MIXWORD(a,b,t1,t2)\
    b = _mm_xor_si128(a,b);\
    t1 = _mm_slli_epi32(a,2);\
    t2 = _mm_srli_epi32(a,30);\
    a = _mm_or_si128(t1,t2);\
    a = _mm_xor_si128(a,b);\
    t1 = _mm_slli_epi32(b,14);\
    t2 = _mm_srli_epi32(b,18);\
    b = _mm_or_si128(t1,t2);\
    b = _mm_xor_si128(a,b);\
    t1 = _mm_slli_epi32(a,10);\
    t2 = _mm_srli_epi32(a,22);\
    a = _mm_or_si128(t1,t2);\
    a = _mm_xor_si128(a,b);\
    t1 = _mm_slli_epi32(b,1);\
    t2 = _mm_srli_epi32(b,31);\
    b = _mm_or_si128(t1,t2);

#define ADD_CONSTANT(a,b,c0,c1)\
    a = _mm_xor_si128(a,c0);\
    b = _mm_xor_si128(b,c1);\

#define NMLTOM768(r0,r1,r2,s0,s1,s2,s3,p0,p1,p2,q0,q1,q2,q3)\
    s2 = _mm_load_si128(&r1);\
    q2 = _mm_load_si128(&p1);\
    r2 = _mm_shuffle_epi32(r2,216);\
    p2 = _mm_shuffle_epi32(p2,216);\
    r1 = _mm_unpacklo_epi32(r1,r0);\
    p1 = _mm_unpacklo_epi32(p1,p0);\
    s2 = _mm_unpackhi_epi32(s2,r0);\
    q2 = _mm_unpackhi_epi32(q2,p0);\
    s0 = _mm_load_si128(&r2);\
    q0 = _mm_load_si128(&p2);\
    r2 = _mm_unpacklo_epi64(r2,r1);\
    p2 = _mm_unpacklo_epi64(p2,p1);\
    s1 = _mm_load_si128(&s0);\
    q1 = _mm_load_si128(&q0);\
    s0 = _mm_unpackhi_epi64(s0,r1);\
    q0 = _mm_unpackhi_epi64(q0,p1);\
    r2 = _mm_shuffle_epi32(r2,225);\
    p2 = _mm_shuffle_epi32(p2,225);\
    r0 = _mm_load_si128(&s1);\
    p0 = _mm_load_si128(&q1);\
    s0 = _mm_shuffle_epi32(s0,225);\
    q0 = _mm_shuffle_epi32(q0,225);\
    s1 = _mm_unpacklo_epi64(s1,s2);\
    q1 = _mm_unpacklo_epi64(q1,q2);\
    r0 = _mm_unpackhi_epi64(r0,s2);\
    p0 = _mm_unpackhi_epi64(p0,q2);\
    s2 = _mm_load_si128(&r0);\
    q2 = _mm_load_si128(&p0);\
    s3 = _mm_load_si128(&r2);\
    q3 = _mm_load_si128(&p2);\

#define MIXTON768(r0,r1,r2,r3,s0,s1,s2,p0,p1,p2,p3,q0,q1,q2)\
    s0 = _mm_load_si128(&r0);\
    q0 = _mm_load_si128(&p0);\
    s1 = _mm_load_si128(&r2);\
    q1 = _mm_load_si128(&p2);\
    r0 = _mm_unpackhi_epi32(r0,r1);\
    p0 = _mm_unpackhi_epi32(p0,p1);\
    r2 = _mm_unpackhi_epi32(r2,r3);\
    p2 = _mm_unpackhi_epi32(p2,p3);\
    s0 = _mm_unpacklo_epi32(s0,r1);\
    q0 = _mm_unpacklo_epi32(q0,p1);\
    s1 = _mm_unpacklo_epi32(s1,r3);\
    q1 = _mm_unpacklo_epi32(q1,p3);\
    r1 = _mm_load_si128(&r0);\
    p1 = _mm_load_si128(&p0);\
    r0 = _mm_unpackhi_epi64(r0,r2);\
    p0 = _mm_unpackhi_epi64(p0,p2);\
    s0 = _mm_unpackhi_epi64(s0,s1);\
    q0 = _mm_unpackhi_epi64(q0,q1);\
    r1 = _mm_unpacklo_epi64(r1,r2);\
    p1 = _mm_unpacklo_epi64(p1,p2);\
    s2 = _mm_load_si128(&r0);\
    q2 = _mm_load_si128(&p0);\
    s1 = _mm_load_si128(&r1);\
    q1 = _mm_load_si128(&p1);\

#define NMLTOM1024(r0,r1,r2,r3,s0,s1,s2,s3,p0,p1,p2,p3,q0,q1,q2,q3)\
    s1 = _mm_load_si128(&r3);\
    q1 = _mm_load_si128(&p3);\
    s3 = _mm_load_si128(&r3);\
    q3 = _mm_load_si128(&p3);\
    s1 = _mm_unpackhi_epi32(s1,r2);\
    q1 = _mm_unpackhi_epi32(q1,p2);\
    s3 = _mm_unpacklo_epi32(s3,r2);\
    q3 = _mm_unpacklo_epi32(q3,p2);\
    s0 = _mm_load_si128(&s1);\
    q0 = _mm_load_si128(&q1);\
    s2 = _mm_load_si128(&s3);\
    q2 = _mm_load_si128(&q3);\
    r3 = _mm_load_si128(&r1);\
    p3 = _mm_load_si128(&p1);\
    r1 = _mm_unpacklo_epi32(r1,r0);\
    p1 = _mm_unpacklo_epi32(p1,p0);\
    r3 = _mm_unpackhi_epi32(r3,r0);\
    p3 = _mm_unpackhi_epi32(p3,p0);\
    s0 = _mm_unpackhi_epi64(s0,r3);\
    q0 = _mm_unpackhi_epi64(q0,p3);\
    s1 = _mm_unpacklo_epi64(s1,r3);\
    q1 = _mm_unpacklo_epi64(q1,p3);\
    s2 = _mm_unpackhi_epi64(s2,r1);\
    q2 = _mm_unpackhi_epi64(q2,p1);\
    s3 = _mm_unpacklo_epi64(s3,r1);\
    q3 = _mm_unpacklo_epi64(q3,p1);

#define MIXTON1024(r0,r1,r2,r3,s0,s1,s2,s3,p0,p1,p2,p3,q0,q1,q2,q3)\
    NMLTOM1024(r0,r1,r2,r3,s0,s1,s2,s3,p0,p1,p2,p3,q0,q1,q2,q3);


static void Update512(hashState_luffa *state, const BitSequence *data, DataLength databitlen);

static void rnd512(hashState_luffa *state);

static void finalization512(hashState_luffa *state, uint32 *b);


/* initial values of chaining variables */
static const uint32 IV[40] = {
    0xdbf78465,0x4eaa6fb4,0x44b051e0,0x6d251e69,
    0xdef610bb,0xee058139,0x90152df4,0x6e292011,
    0xde099fa3,0x70eee9a0,0xd9d2f256,0xc3b44b95,
    0x746cd581,0xcf1ccf0e,0x8fc944b3,0x5d9b0557,
    0xad659c05,0x04016ce5,0x5dba5781,0xf7efc89d,
    0x8b264ae7,0x24aa230a,0x666d1836,0x0306194f,
    0x204b1f67,0xe571f7d7,0x36d79cce,0x858075d5,
    0x7cde72ce,0x14bcb808,0x57e9e923,0x35870c6a,
    0xaffb4363,0xc825b7c7,0x5ec41e22,0x6c68e9be,
    0x03e86cea,0xb07224cc,0x0fc688f1,0xf5df3999
};

/* Round Constants */
static const uint32 CNS_INIT[128] = {
    0xb213afa5,0xfc20d9d2,0xb6de10ed,0x303994a6,
    0xe028c9bf,0xe25e72c1,0x01685f3d,0xe0337818,
    0xc84ebe95,0x34552e25,0x70f47aae,0xc0e65299,
    0x44756f91,0xe623bb72,0x05a17cf4,0x441ba90d,
    0x4e608a22,0x7ad8818f,0x0707a3d4,0x6cc33a12,
    0x7e8fce32,0x5c58a4a4,0xbd09caca,0x7f34d442,
    0x56d858fe,0x8438764a,0x1c1e8f51,0xdc56983e,
    0x956548be,0x1e38e2e7,0xf4272b28,0x9389217f,
    0x343b138f,0xbb6de032,0x707a3d45,0x1e00108f,
    0xfe191be2,0x78e38b9d,0x144ae5cc,0xe5a8bce6,
    0xd0ec4e3d,0xedb780c8,0xaeb28562,0x7800423d,
    0x3cb226e5,0x27586719,0xfaa7ae2b,0x5274baf4,
    0x2ceb4882,0xd9847356,0xbaca1589,0x8f5b7882,
    0x5944a28e,0x36eda57f,0x2e48f1c1,0x26889ba7,
    0xb3ad2208,0xa2c78434,0x40a46f3e,0x96e1db12,
    0xa1c4c355,0x703aace7,0xb923c704,0x9a226e9d,
    0x00000000,0x00000000,0x00000000,0xf0d2e9e3,
    0x00000000,0x00000000,0x00000000,0x5090d577,
    0x00000000,0x00000000,0x00000000,0xac11d7fa,
    0x00000000,0x00000000,0x00000000,0x2d1925ab,
    0x00000000,0x00000000,0x00000000,0x1bcb66f2,
    0x00000000,0x00000000,0x00000000,0xb46496ac,
    0x00000000,0x00000000,0x00000000,0x6f2d9bc9,
    0x00000000,0x00000000,0x00000000,0xd1925ab0,
    0x00000000,0x00000000,0x00000000,0x78602649,
    0x00000000,0x00000000,0x00000000,0x29131ab6,
    0x00000000,0x00000000,0x00000000,0x8edae952,
    0x00000000,0x00000000,0x00000000,0x0fc053c3,
    0x00000000,0x00000000,0x00000000,0x3b6ba548,
    0x00000000,0x00000000,0x00000000,0x3f014f0c,
    0x00000000,0x00000000,0x00000000,0xedae9520,
    0x00000000,0x00000000,0x00000000,0xfc053c31
};

__m128i CNS128[32];
__m128i ALLONE;
__m128i MASK;



HashReturn init_luffa(hashState_luffa *state, int hashbitlen)
{
    int i;
    state->hashbitlen = hashbitlen;

    /* set the lower 32 bits to '1' */
    MASK= _mm_set_epi32(0x00000000, 0x00000000, 0x00000000, 0xffffffff);

    /* set all bits to '1' */
    ALLONE = _mm_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);

    /* set the 32-bit round constant values to the 128-bit data field */
    for (i=0;i<32;i++) {
        CNS128[i] = _mm_loadu_si128((__m128i*)&CNS_INIT[i*4]);
    }


	for (i=0;i<10;i++) 
		state->chainv[i] = _mm_loadu_si128((__m128i*)&IV[i*4]);
      

    state->bitlen[0] = 0;
    state->bitlen[1] = 0;
    state->rembitlen = 0;

    memset(state->buffer, 0, sizeof state->buffer );

    return SUCCESS;
}

HashReturn update_luffa(hashState_luffa *state, const BitSequence *data, DataLength databitlen)
{
    HashReturn ret=SUCCESS;
    int i;
    uint8 *p = (uint8*)state->buffer;
	for (i=0;i<8;i++) state->buffer[i] = BYTES_SWAP32(((uint32*)data)[i]);
	rnd512(state); 
	data += MSG_BLOCK_BYTE_LEN; 
	state->rembitlen = 0;
	for (i=0;i<8;i++) state->buffer[i] = BYTES_SWAP32(((uint32*)data)[i]);
	rnd512(state); 
	data += MSG_BLOCK_BYTE_LEN; 
	memset(p+1, 0, 31*sizeof(uint8));
	p[0] = 0x80;
	for (i=0;i<8;i++) state->buffer[i] = BYTES_SWAP32(state->buffer[i]);
    rnd512(state);
    return ret;
}
   

HashReturn final_luffa(hashState_luffa *state, BitSequence *hashval) 
{

    finalization512(state, (uint32*) hashval);
  
    return SUCCESS;
}

/***************************************************/
/* Round function         */
/* state: hash context    */


static void rnd512(hashState_luffa *state)
{
    __m128i t[2];
    __m128i chainv[10];
    __m128i msg[2];
    __m128i tmp[2];
    __m128i x[8];
    int i;

    chainv[0] = _mm_load_si128(&state->chainv[0]);
    chainv[1] = _mm_load_si128(&state->chainv[1]);
    chainv[2] = _mm_load_si128(&state->chainv[2]);
    chainv[3] = _mm_load_si128(&state->chainv[3]);
    chainv[4] = _mm_load_si128(&state->chainv[4]);
    chainv[5] = _mm_load_si128(&state->chainv[5]);
    chainv[6] = _mm_load_si128(&state->chainv[6]);
    chainv[7] = _mm_load_si128(&state->chainv[7]);
    chainv[8] = _mm_load_si128(&state->chainv[8]);
    chainv[9] = _mm_load_si128(&state->chainv[9]);

    t[0] = _mm_load_si128(&chainv[0]);
    t[1] = _mm_load_si128(&chainv[1]);
    t[0] = _mm_xor_si128(t[0], chainv[2]);
    t[1] = _mm_xor_si128(t[1], chainv[3]);
    t[0] = _mm_xor_si128(t[0], chainv[4]);
    t[1] = _mm_xor_si128(t[1], chainv[5]);
    t[0] = _mm_xor_si128(t[0], chainv[6]);
    t[1] = _mm_xor_si128(t[1], chainv[7]);
    t[0] = _mm_xor_si128(t[0], chainv[8]);
    t[1] = _mm_xor_si128(t[1], chainv[9]);

    MULT2(t[0],t[1],tmp[0],tmp[1]);

    msg[0] = _mm_loadu_si128 ((__m128i*)&state->buffer[0]);
    msg[1] = _mm_loadu_si128 ((__m128i*)&state->buffer[4]);
    msg[0] = _mm_shuffle_epi32(msg[0], 27);
    msg[1] = _mm_shuffle_epi32(msg[1], 27);

    for (i=0;i<5;i++){
        chainv[i*2] = _mm_xor_si128(chainv[i*2], t[0]);
        chainv[1+i*2] = _mm_xor_si128(chainv[1+i*2], t[1]);
    }

    t[0] = _mm_load_si128(&chainv[0]);
    t[1] = _mm_load_si128(&chainv[1]);

    MULT2(chainv[0],chainv[1],tmp[0],tmp[1]);
    chainv[0] = _mm_xor_si128(chainv[0], chainv[2]);
    chainv[1] = _mm_xor_si128(chainv[1], chainv[3]);

    MULT2(chainv[2],chainv[3],tmp[0],tmp[1]);
    chainv[2] = _mm_xor_si128(chainv[2], chainv[4]);
    chainv[3] = _mm_xor_si128(chainv[3], chainv[5]);

    MULT2(chainv[4],chainv[5],tmp[0],tmp[1]);
    chainv[4] = _mm_xor_si128(chainv[4], chainv[6]);
    chainv[5] = _mm_xor_si128(chainv[5], chainv[7]);

    MULT2(chainv[6],chainv[7],tmp[0],tmp[1]);
    chainv[6] = _mm_xor_si128(chainv[6], chainv[8]);
    chainv[7] = _mm_xor_si128(chainv[7], chainv[9]);

    MULT2(chainv[8],chainv[9],tmp[0],tmp[1]);
    chainv[8] = _mm_xor_si128(chainv[8], t[0]);
    chainv[9] = _mm_xor_si128(chainv[9], t[1]);

    t[0] = _mm_load_si128(&chainv[8]);
    t[1] = _mm_load_si128(&chainv[9]);

    MULT2(chainv[8],chainv[9],tmp[0],tmp[1]);
    chainv[8] = _mm_xor_si128(chainv[8], chainv[6]);
    chainv[9] = _mm_xor_si128(chainv[9], chainv[7]);

    MULT2(chainv[6],chainv[7],tmp[0],tmp[1]);
    chainv[6] = _mm_xor_si128(chainv[6], chainv[4]);
    chainv[7] = _mm_xor_si128(chainv[7], chainv[5]);

    MULT2(chainv[4],chainv[5],tmp[0],tmp[1]);
    chainv[4] = _mm_xor_si128(chainv[4], chainv[2]);
    chainv[5] = _mm_xor_si128(chainv[5], chainv[3]);

    MULT2(chainv[2],chainv[3],tmp[0],tmp[1]);
    chainv[2] = _mm_xor_si128(chainv[2], chainv[0]);
    chainv[3] = _mm_xor_si128(chainv[3], chainv[1]);

    MULT2(chainv[0],chainv[1],tmp[0],tmp[1]);
    chainv[0] = _mm_xor_si128(chainv[0], t[0]);
    chainv[1] = _mm_xor_si128(chainv[1], t[1]);

    for (i=0;i<5;i++){
        chainv[i*2] = _mm_xor_si128(chainv[i*2], msg[0]);
        chainv[1+i*2] = _mm_xor_si128(chainv[1+i*2], msg[1]);

        MULT2(msg[0],msg[1],tmp[0],tmp[1]);
    }

    /* Tweak() */
    t[0] = _mm_slli_epi32(chainv[3], 1); 
    t[1] = _mm_srli_epi32(chainv[3], 31); 
    chainv[3] = _mm_or_si128(t[0], t[1]);
    t[0] = _mm_slli_epi32(chainv[5], 2); 
    t[1] = _mm_srli_epi32(chainv[5], 30); 
    chainv[5] = _mm_or_si128(t[0], t[1]);
    t[0] = _mm_slli_epi32(chainv[7], 3); 
    t[1] = _mm_srli_epi32(chainv[7], 29); 
    chainv[7] = _mm_or_si128(t[0], t[1]);
    t[0] = _mm_slli_epi32(chainv[9], 4); 
    t[1] = _mm_srli_epi32(chainv[9], 28); 
    chainv[9] = _mm_or_si128(t[0], t[1]);

    NMLTOM1024(chainv[0],chainv[2],chainv[4],chainv[6], x[0],x[1],x[2],x[3],
            chainv[1],chainv[3],chainv[5],chainv[7], x[4],x[5],x[6],x[7]);

    for (i=0;i<8;i++) {
        STEP_PART(&x[0],&CNS128[i*2],&tmp[0]);
    }

    MIXTON1024(x[0],x[1],x[2],x[3], chainv[0],chainv[2],chainv[4],chainv[6],
            x[4],x[5],x[6],x[7], chainv[1],chainv[3],chainv[5],chainv[7]);

    /* Process last 256-bit block */
    for (i=0;i<8;i++) {
        STEP_PART2(chainv[8],chainv[9],t[0],t[1],CNS128[16+2*i],CNS128[17+2*i],tmp[0],tmp[1]);
    }

    state->chainv[0] = _mm_load_si128(&chainv[0]);
    state->chainv[1] = _mm_load_si128(&chainv[1]);
    state->chainv[2] = _mm_load_si128(&chainv[2]);
    state->chainv[3] = _mm_load_si128(&chainv[3]);
    state->chainv[4] = _mm_load_si128(&chainv[4]);
    state->chainv[5] = _mm_load_si128(&chainv[5]);
    state->chainv[6] = _mm_load_si128(&chainv[6]);
    state->chainv[7] = _mm_load_si128(&chainv[7]);
    state->chainv[8] = _mm_load_si128(&chainv[8]);
    state->chainv[9] = _mm_load_si128(&chainv[9]);

    return;
}

/***************************************************/
/* Finalization function  */
/* state: hash context    */
/* b[8]: hash values      */

static void finalization512(hashState_luffa *state, uint32 *b)
{
    __m128i t[2];
    uint32 hash[8];
    int i;

    /*---- blank round with m=0 ----*/
    memset(state->buffer, 0, sizeof state->buffer );
    rnd512(state);

    t[0] = _mm_load_si128(&state->chainv[0]);
    t[1] = _mm_load_si128(&state->chainv[1]);
    t[0] = _mm_xor_si128(t[0], state->chainv[2]);
    t[1] = _mm_xor_si128(t[1], state->chainv[3]);
    t[0] = _mm_xor_si128(t[0], state->chainv[4]);
    t[1] = _mm_xor_si128(t[1], state->chainv[5]);
    t[0] = _mm_xor_si128(t[0], state->chainv[6]);
    t[1] = _mm_xor_si128(t[1], state->chainv[7]);
    t[0] = _mm_xor_si128(t[0], state->chainv[8]);
    t[1] = _mm_xor_si128(t[1], state->chainv[9]);

    t[0] = _mm_shuffle_epi32(t[0], 27);
    t[1] = _mm_shuffle_epi32(t[1], 27);

    _mm_storeu_si128((__m128i*)&hash[0], t[0]);
    _mm_storeu_si128((__m128i*)&hash[4], t[1]);

    for (i=0;i<8;i++) b[i] = BYTES_SWAP32(hash[i]);

    memset(state->buffer, 0, sizeof state->buffer );
    rnd512(state);

    t[0] = _mm_load_si128(&state->chainv[0]);
    t[1] = _mm_load_si128(&state->chainv[1]);
    t[0] = _mm_xor_si128(t[0], state->chainv[2]);
    t[1] = _mm_xor_si128(t[1], state->chainv[3]);
    t[0] = _mm_xor_si128(t[0], state->chainv[4]);
    t[1] = _mm_xor_si128(t[1], state->chainv[5]);
    t[0] = _mm_xor_si128(t[0], state->chainv[6]);
    t[1] = _mm_xor_si128(t[1], state->chainv[7]);
    t[0] = _mm_xor_si128(t[0], state->chainv[8]);
    t[1] = _mm_xor_si128(t[1], state->chainv[9]);

    t[0] = _mm_shuffle_epi32(t[0], 27);
    t[1] = _mm_shuffle_epi32(t[1], 27);

    _mm_storeu_si128((__m128i*)&hash[0], t[0]);
    _mm_storeu_si128((__m128i*)&hash[4], t[1]);

    for (i=0;i<8;i++) b[8+i] = BYTES_SWAP32(hash[i]);

    return;
}





/***************************************************/
