/*-
 * Copyright 2009 Colin Percival, 2011 ArtForz, 2011 pooler
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file was originally written by Colin Percival as part of the Tarsnap
 * online backup system.
 */

#include "cpuminer-config.h"
#include "miner.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define byteswap(x) ((((x) << 24) & 0xff000000u) | (((x) << 8) & 0x00ff0000u) | (((x) >> 8) & 0x0000ff00u) | (((x) >> 24) & 0x000000ffu))

static inline void
byteswap_vec(uint32_t *dest, const uint32_t *src, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++)
		dest[i] = byteswap(src[i]);
}

static inline uint32_t be32dec(const void *pp)
{
	const uint8_t *p = (uint8_t const *)pp;

	return ((uint32_t)(p[3]) + ((uint32_t)(p[2]) << 8) +
	    ((uint32_t)(p[1]) << 16) + ((uint32_t)(p[0]) << 24));
}

static inline void be32enc(void *pp, uint32_t x)
{
	uint8_t * p = (uint8_t *)pp;

	p[3] = x & 0xff;
	p[2] = (x >> 8) & 0xff;
	p[1] = (x >> 16) & 0xff;
	p[0] = (x >> 24) & 0xff;
}

static inline uint32_t le32dec(const void *pp)
{
	const uint8_t *p = (uint8_t const *)pp;

	return ((uint32_t)(p[0]) + ((uint32_t)(p[1]) << 8) +
	    ((uint32_t)(p[2]) << 16) + ((uint32_t)(p[3]) << 24));
}

static inline void le32enc(void *pp, uint32_t x)
{
	uint8_t * p = (uint8_t *)pp;

	p[0] = x & 0xff;
	p[1] = (x >> 8) & 0xff;
	p[2] = (x >> 16) & 0xff;
	p[3] = (x >> 24) & 0xff;
}

typedef struct SHA256Context {
	uint32_t state[8];
	uint32_t buf[16];
} SHA256_CTX;

/* Elementary functions used by SHA256 */
#define Ch(x, y, z)	((x & (y ^ z)) ^ z)
#define Maj(x, y, z)	((x & (y | z)) | (y & z))
#define SHR(x, n)	(x >> n)
#define ROTR(x, n)	((x >> n) | (x << (32 - n)))
#define S0(x)		(ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define S1(x)		(ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define s0(x)		(ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3))
#define s1(x)		(ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10))

/* SHA256 round function */
#define RND(a, b, c, d, e, f, g, h, k)			\
	t0 = h + S1(e) + Ch(e, f, g) + k;		\
	t1 = S0(a) + Maj(a, b, c);			\
	d += t0;					\
	h  = t0 + t1;

/* Adjusted round function for rotating state */
#define RNDr(S, W, i, k)			\
	RND(S[(64 - i) % 8], S[(65 - i) % 8],	\
	    S[(66 - i) % 8], S[(67 - i) % 8],	\
	    S[(68 - i) % 8], S[(69 - i) % 8],	\
	    S[(70 - i) % 8], S[(71 - i) % 8],	\
	    W[i] + k)

/*
 * SHA256 block compression function.  The 256-bit state is transformed via
 * the 512-bit input block to produce a new state.
 */
static void
SHA256_Transform(uint32_t * state, const uint32_t block[16], int swap)
{
	uint32_t W[64];
	uint32_t S[8];
	uint32_t t0, t1;
	int i;

	/* 1. Prepare message schedule W. */
	if(swap)
		byteswap_vec(W, block, 16);
	else
		memcpy(W, block, 64);
	for (i = 16; i < 64; i += 2) {
		W[i] = s1(W[i - 2]) + W[i - 7] + s0(W[i - 15]) + W[i - 16];
		W[i+1] = s1(W[i - 1]) + W[i - 6] + s0(W[i - 14]) + W[i - 15];
	}

	/* 2. Initialize working variables. */
	memcpy(S, state, 32);

	/* 3. Mix. */
	RNDr(S, W, 0, 0x428a2f98);
	RNDr(S, W, 1, 0x71374491);
	RNDr(S, W, 2, 0xb5c0fbcf);
	RNDr(S, W, 3, 0xe9b5dba5);
	RNDr(S, W, 4, 0x3956c25b);
	RNDr(S, W, 5, 0x59f111f1);
	RNDr(S, W, 6, 0x923f82a4);
	RNDr(S, W, 7, 0xab1c5ed5);
	RNDr(S, W, 8, 0xd807aa98);
	RNDr(S, W, 9, 0x12835b01);
	RNDr(S, W, 10, 0x243185be);
	RNDr(S, W, 11, 0x550c7dc3);
	RNDr(S, W, 12, 0x72be5d74);
	RNDr(S, W, 13, 0x80deb1fe);
	RNDr(S, W, 14, 0x9bdc06a7);
	RNDr(S, W, 15, 0xc19bf174);
	RNDr(S, W, 16, 0xe49b69c1);
	RNDr(S, W, 17, 0xefbe4786);
	RNDr(S, W, 18, 0x0fc19dc6);
	RNDr(S, W, 19, 0x240ca1cc);
	RNDr(S, W, 20, 0x2de92c6f);
	RNDr(S, W, 21, 0x4a7484aa);
	RNDr(S, W, 22, 0x5cb0a9dc);
	RNDr(S, W, 23, 0x76f988da);
	RNDr(S, W, 24, 0x983e5152);
	RNDr(S, W, 25, 0xa831c66d);
	RNDr(S, W, 26, 0xb00327c8);
	RNDr(S, W, 27, 0xbf597fc7);
	RNDr(S, W, 28, 0xc6e00bf3);
	RNDr(S, W, 29, 0xd5a79147);
	RNDr(S, W, 30, 0x06ca6351);
	RNDr(S, W, 31, 0x14292967);
	RNDr(S, W, 32, 0x27b70a85);
	RNDr(S, W, 33, 0x2e1b2138);
	RNDr(S, W, 34, 0x4d2c6dfc);
	RNDr(S, W, 35, 0x53380d13);
	RNDr(S, W, 36, 0x650a7354);
	RNDr(S, W, 37, 0x766a0abb);
	RNDr(S, W, 38, 0x81c2c92e);
	RNDr(S, W, 39, 0x92722c85);
	RNDr(S, W, 40, 0xa2bfe8a1);
	RNDr(S, W, 41, 0xa81a664b);
	RNDr(S, W, 42, 0xc24b8b70);
	RNDr(S, W, 43, 0xc76c51a3);
	RNDr(S, W, 44, 0xd192e819);
	RNDr(S, W, 45, 0xd6990624);
	RNDr(S, W, 46, 0xf40e3585);
	RNDr(S, W, 47, 0x106aa070);
	RNDr(S, W, 48, 0x19a4c116);
	RNDr(S, W, 49, 0x1e376c08);
	RNDr(S, W, 50, 0x2748774c);
	RNDr(S, W, 51, 0x34b0bcb5);
	RNDr(S, W, 52, 0x391c0cb3);
	RNDr(S, W, 53, 0x4ed8aa4a);
	RNDr(S, W, 54, 0x5b9cca4f);
	RNDr(S, W, 55, 0x682e6ff3);
	RNDr(S, W, 56, 0x748f82ee);
	RNDr(S, W, 57, 0x78a5636f);
	RNDr(S, W, 58, 0x84c87814);
	RNDr(S, W, 59, 0x8cc70208);
	RNDr(S, W, 60, 0x90befffa);
	RNDr(S, W, 61, 0xa4506ceb);
	RNDr(S, W, 62, 0xbef9a3f7);
	RNDr(S, W, 63, 0xc67178f2);

	/* 4. Mix local working variables into global state */
	for (i = 0; i < 8; i++)
		state[i] += S[i];
}

static inline void
SHA256_InitState(uint32_t * state)
{
	/* Magic initialization constants */
	state[0] = 0x6A09E667;
	state[1] = 0xBB67AE85;
	state[2] = 0x3C6EF372;
	state[3] = 0xA54FF53A;
	state[4] = 0x510E527F;
	state[5] = 0x9B05688C;
	state[6] = 0x1F83D9AB;
	state[7] = 0x5BE0CD19;
}

static const uint32_t passwdpad[12] = {0x00000080, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x80020000};
static const uint32_t outerpad[8] = {0x80000000, 0, 0, 0, 0, 0, 0, 0x00000300};

static inline void
PBKDF2_SHA256_80_128_init(const uint32_t *passwd, uint32_t tstate[8], uint32_t ostate[8])
{
	uint32_t ihash[8];
	uint32_t pad[16];
	uint32_t i;

	SHA256_InitState(tstate);
	SHA256_Transform(tstate, passwd, 1);
	memcpy(pad, passwd+16, 16);
	memcpy(pad+4, passwdpad, 48);
	SHA256_Transform(tstate, pad, 1);
	memcpy(ihash, tstate, 32);

	SHA256_InitState(ostate);
	for (i = 0; i < 8; i++)
		pad[i] = ihash[i] ^ 0x5c5c5c5c;
	for (; i < 16; i++)
		pad[i] = 0x5c5c5c5c;
	SHA256_Transform(ostate, pad, 0);

	SHA256_InitState(tstate);
	for (i = 0; i < 8; i++)
		pad[i] = ihash[i] ^ 0x36363636;
	for (; i < 16; i++)
		pad[i] = 0x36363636;
	SHA256_Transform(tstate, pad, 0);
}

/**
 * PBKDF2_SHA256(passwd, passwdlen, salt, saltlen, c, buf, dkLen):
 * Compute PBKDF2(passwd, salt, c, dkLen) using HMAC-SHA256 as the PRF, and
 * write the output to buf.
 */
static inline void
PBKDF2_SHA256_80_128(const uint32_t *tstate, const uint32_t *ostate, const uint32_t *passwd, uint32_t *buf)
{
	static const uint32_t innerpad[11] = {0x00000080, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xa0040000};
	SHA256_CTX PShictx, PShoctx;
	uint32_t i;
	
	/* If Klen > 64, the key is really SHA256(K). */
	memcpy(PShictx.state, tstate, 32);
	memcpy(PShoctx.state, ostate, 32);
	
	memcpy(PShoctx.buf+8, outerpad, 32);

	SHA256_Transform(PShictx.state, passwd, 1);
	byteswap_vec(PShictx.buf, passwd+16, 4);
	byteswap_vec(PShictx.buf+5, innerpad, 11);

	/* Iterate through the blocks. */
	for (i = 0; i < 4; i++) {
		uint32_t ist[8];
		uint32_t ost[8];
		
		memcpy(ist, PShictx.state, 32);
		PShictx.buf[4] = i + 1;
		SHA256_Transform(ist, PShictx.buf, 0);
		memcpy(PShoctx.buf, ist, 32);

		memcpy(ost, PShoctx.state, 32);
		SHA256_Transform(ost, PShoctx.buf, 0);
		byteswap_vec(buf+i*8, ost, 8);
	}
}

static inline void
PBKDF2_SHA256_80_128_32(uint32_t *tstate, uint32_t *ostate, const uint32_t *passwd, const uint32_t *salt, uint32_t *output)
{
	static const uint32_t ihash_finalblk[16] = {0x00000001,0x80000000,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0x00000620};
	uint32_t pad[16];
	
	SHA256_Transform(tstate, salt, 1);
	SHA256_Transform(tstate, salt+16, 1);
	SHA256_Transform(tstate, ihash_finalblk, 0);
	memcpy(pad, tstate, 32);
	memcpy(pad+8, outerpad, 32);

	SHA256_Transform(ostate, pad, 0);
	byteswap_vec(output, ostate, 8);
}


/**
 * salsa20_8(B):
 * Apply the salsa20/8 core to the provided block.
 */
static inline void
salsa20_8(uint32_t B[16], const uint32_t Bx[16])
{
	uint32_t x00,x01,x02,x03,x04,x05,x06,x07,x08,x09,x10,x11,x12,x13,x14,x15;
	size_t i;

	x00 = (B[ 0] ^= Bx[ 0]);
	x01 = (B[ 1] ^= Bx[ 1]);
	x02 = (B[ 2] ^= Bx[ 2]);
	x03 = (B[ 3] ^= Bx[ 3]);
	x04 = (B[ 4] ^= Bx[ 4]);
	x05 = (B[ 5] ^= Bx[ 5]);
	x06 = (B[ 6] ^= Bx[ 6]);
	x07 = (B[ 7] ^= Bx[ 7]);
	x08 = (B[ 8] ^= Bx[ 8]);
	x09 = (B[ 9] ^= Bx[ 9]);
	x10 = (B[10] ^= Bx[10]);
	x11 = (B[11] ^= Bx[11]);
	x12 = (B[12] ^= Bx[12]);
	x13 = (B[13] ^= Bx[13]);
	x14 = (B[14] ^= Bx[14]);
	x15 = (B[15] ^= Bx[15]);
	for (i = 0; i < 8; i += 2) {
#define R(a,b) (((a) << (b)) | ((a) >> (32 - (b))))
		/* Operate on columns. */
		x04 ^= R(x00+x12, 7);	x09 ^= R(x05+x01, 7);	x14 ^= R(x10+x06, 7);	x03 ^= R(x15+x11, 7);
		x08 ^= R(x04+x00, 9);	x13 ^= R(x09+x05, 9);	x02 ^= R(x14+x10, 9);	x07 ^= R(x03+x15, 9);
		x12 ^= R(x08+x04,13);	x01 ^= R(x13+x09,13);	x06 ^= R(x02+x14,13);	x11 ^= R(x07+x03,13);
		x00 ^= R(x12+x08,18);	x05 ^= R(x01+x13,18);	x10 ^= R(x06+x02,18);	x15 ^= R(x11+x07,18);

		/* Operate on rows. */
		x01 ^= R(x00+x03, 7);	x06 ^= R(x05+x04, 7);	x11 ^= R(x10+x09, 7);	x12 ^= R(x15+x14, 7);
		x02 ^= R(x01+x00, 9);	x07 ^= R(x06+x05, 9);	x08 ^= R(x11+x10, 9);	x13 ^= R(x12+x15, 9);
		x03 ^= R(x02+x01,13);	x04 ^= R(x07+x06,13);	x09 ^= R(x08+x11,13);	x14 ^= R(x13+x12,13);
		x00 ^= R(x03+x02,18);	x05 ^= R(x04+x07,18);	x10 ^= R(x09+x08,18);	x15 ^= R(x14+x13,18);
#undef R
	}
	B[ 0] += x00;
	B[ 1] += x01;
	B[ 2] += x02;
	B[ 3] += x03;
	B[ 4] += x04;
	B[ 5] += x05;
	B[ 6] += x06;
	B[ 7] += x07;
	B[ 8] += x08;
	B[ 9] += x09;
	B[10] += x10;
	B[11] += x11;
	B[12] += x12;
	B[13] += x13;
	B[14] += x14;
	B[15] += x15;
}


#if defined(__x86_64__)

#define SCRYPT_3WAY
#define SCRYPT_BUFFER_SIZE (3 * 131072 + 63)

int scrypt_best_throughput();
void scrypt_core(uint32_t *X, uint32_t *V);
void scrypt_core_2way(uint32_t *X, uint32_t *Y, uint32_t *V);
void scrypt_core_3way(uint32_t *X, uint32_t *Y, uint32_t *Z, uint32_t *V);

#elif defined(__i386__)

#define SCRYPT_BUFFER_SIZE (131072 + 63)

void scrypt_core(uint32_t *X, uint32_t *V);

#else

#define SCRYPT_BUFFER_SIZE (131072 + 63)

static inline void scrypt_core(uint32_t *X, uint32_t *V)
{
	uint32_t i;
	uint32_t j;
	uint32_t k;
	uint64_t *p1, *p2;
	p1 = (uint64_t *)X;
	for (i = 0; i < 1024; i += 2) {
		memcpy(&V[i * 32], X, 128);

		salsa20_8(&X[0], &X[16]);
		salsa20_8(&X[16], &X[0]);

		memcpy(&V[(i + 1) * 32], X, 128);

		salsa20_8(&X[0], &X[16]);
		salsa20_8(&X[16], &X[0]);
	}
	for (i = 0; i < 1024; i += 2) {
		j = X[16] & 1023;
		p2 = (uint64_t *)(&V[j * 32]);
		for(k = 0; k < 16; k++)
			p1[k] ^= p2[k];

		salsa20_8(&X[0], &X[16]);
		salsa20_8(&X[16], &X[0]);

		j = X[16] & 1023;
		p2 = (uint64_t *)(&V[j * 32]);
		for(k = 0; k < 16; k++)
			p1[k] ^= p2[k];

		salsa20_8(&X[0], &X[16]);
		salsa20_8(&X[16], &X[0]);
	}
}

#endif

unsigned char *scrypt_buffer_alloc() {
	return malloc(SCRYPT_BUFFER_SIZE);
}

/* cpu and memory intensive function to transform a 80 byte buffer into a 32 byte output
   scratchpad size needs to be at least 63 + (128 * r * p) + (256 * r + 64) + (128 * r * N) bytes
   r = 1, p = 1, N = 1024
 */
static void scrypt_1024_1_1_256_sp(const uint32_t* input, uint32_t *output, unsigned char *scratchpad)
{
	uint32_t tstate[8], ostate[8];
	uint32_t *V;
	uint32_t X[32];
	V = (uint32_t *)(((uintptr_t)(scratchpad) + 63) & ~ (uintptr_t)(63));

	PBKDF2_SHA256_80_128_init(input, tstate, ostate);
	PBKDF2_SHA256_80_128(tstate, ostate, input, X);

	scrypt_core(X, V);

	return PBKDF2_SHA256_80_128_32(tstate, ostate, input, X, output);
}

#ifdef SCRYPT_3WAY

static void scrypt_1024_1_1_256_sp_2way(const uint32_t *input1, const uint32_t *input2,
	uint32_t *output1, uint32_t *output2, unsigned char *scratchpad)
{
	uint32_t tstate1[8], tstate2[8];
	uint32_t ostate1[8], ostate2[8];
	uint32_t *V;
	uint32_t X[32], Y[32];
	V = (uint32_t *)(((uintptr_t)(scratchpad) + 63) & ~ (uintptr_t)(63));

	PBKDF2_SHA256_80_128_init(input1, tstate1, ostate1);
	PBKDF2_SHA256_80_128_init(input2, tstate2, ostate2);
	PBKDF2_SHA256_80_128(tstate1, ostate1, input1, X);
	PBKDF2_SHA256_80_128(tstate2, ostate2, input2, Y);

	scrypt_core_2way(X, Y, V);

	PBKDF2_SHA256_80_128_32(tstate1, ostate1, input1, X, output1);
	PBKDF2_SHA256_80_128_32(tstate2, ostate2, input2, Y, output2);
}

static void scrypt_1024_1_1_256_sp_3way(const uint32_t *input1, const uint32_t *input2, const uint32_t *input3,
	uint32_t *output1, uint32_t *output2, uint32_t *output3, unsigned char *scratchpad)
{
	uint32_t tstate1[8], tstate2[8], tstate3[8];
	uint32_t ostate1[8], ostate2[8], ostate3[8];
	uint32_t *V;
	uint32_t X[32], Y[32], Z[32];
	V = (uint32_t *)(((uintptr_t)(scratchpad) + 63) & ~ (uintptr_t)(63));

	PBKDF2_SHA256_80_128_init(input1, tstate1, ostate1);
	PBKDF2_SHA256_80_128_init(input2, tstate2, ostate2);
	PBKDF2_SHA256_80_128_init(input3, tstate3, ostate3);
	PBKDF2_SHA256_80_128(tstate1, ostate1, input1, X);
	PBKDF2_SHA256_80_128(tstate2, ostate2, input2, Y);
	PBKDF2_SHA256_80_128(tstate3, ostate3, input3, Z);

	scrypt_core_3way(X, Y, Z, V);

	PBKDF2_SHA256_80_128_32(tstate1, ostate1, input1, X, output1);
	PBKDF2_SHA256_80_128_32(tstate2, ostate2, input2, Y, output2);
	PBKDF2_SHA256_80_128_32(tstate3, ostate3, input3, Z, output3);
}

#endif

__attribute__ ((noinline)) static int test_hash(const uint32_t *hash,
	const uint32_t *target)
{
	int i;
	for (i = 7; i >= 0; i--) {
		uint32_t t = le32dec(&target[i]);
		if (hash[i] > t)
			return 0;
		if (hash[i] < t)
			return 1;
	}
	return 1;
}

int scanhash_scrypt(int thr_id, unsigned char *pdata, unsigned char *scratchbuf,
	const unsigned char *ptarget,
	uint32_t max_nonce, uint32_t *next_nonce, unsigned long *hashes_done)
{
	uint32_t data[20], hash[8];
#ifdef SCRYPT_3WAY
	uint32_t data2[20], hash2[8];
	uint32_t data3[20], hash3[8];
	int throughput;
#endif
	unsigned long first_nonce = *next_nonce;
	uint32_t n = *next_nonce;
	uint32_t Htarg = le32dec(&((const uint32_t *)ptarget)[7]);
	int i;
	
	for (i = 0; i < 19; i++)
		data[i] = be32dec(&((const uint32_t *)pdata)[i]);
#ifdef SCRYPT_3WAY
	memcpy(data2, data, 80);
	memcpy(data3, data, 80);
	throughput = scrypt_best_throughput();
#endif
	
	do {
		data[19] = n++;
#ifdef SCRYPT_3WAY
		if (throughput >= 2 && n <= max_nonce) {
			data2[19] = n++;
			if (throughput >= 3 && n <= max_nonce) {
				data3[19] = n++;
				scrypt_1024_1_1_256_sp_3way(data, data2, data3, hash, hash2, hash3, scratchbuf);
				if (hash3[7] < Htarg || hash3[7] == Htarg && test_hash(hash3, (uint32_t *)ptarget)) {
					be32enc(&((uint32_t *)pdata)[19], data3[19]);
					*next_nonce = n;
					*hashes_done = n - first_nonce;
					return true;
				}
			} else {
				scrypt_1024_1_1_256_sp_2way(data, data2, hash, hash2, scratchbuf);
			}
			if (hash2[7] < Htarg || hash2[7] == Htarg && test_hash(hash2, (uint32_t *)ptarget)) {
				be32enc(&((uint32_t *)pdata)[19], data2[19]);
				*next_nonce = n;
				*hashes_done = n - first_nonce;
				return true;
			}
		} else {
			scrypt_1024_1_1_256_sp(data, hash, scratchbuf);
		}
#else
		scrypt_1024_1_1_256_sp(data, hash, scratchbuf);
#endif
		if (hash[7] < Htarg || hash[7] == Htarg && test_hash(hash, (uint32_t *)ptarget)) {
			be32enc(&((uint32_t *)pdata)[19], data[19]);
			*next_nonce = n;
			*hashes_done = n - first_nonce;
			return true;
		}
	} while (n <= max_nonce && !work_restart[thr_id].restart);
	
	*next_nonce = n;
	*hashes_done = n - first_nonce;
	return false;
}

