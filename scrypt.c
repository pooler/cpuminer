/*-
 * Copyright 2009 Colin Percival, 2011 ArtForz
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


static inline uint32_t
be32dec(const void *pp)
{
	const uint8_t *p = (uint8_t const *)pp;

	return ((uint32_t)(p[3]) + ((uint32_t)(p[2]) << 8) +
	    ((uint32_t)(p[1]) << 16) + ((uint32_t)(p[0]) << 24));
}

static inline void
be32enc(void *pp, uint32_t x)
{
	uint8_t * p = (uint8_t *)pp;

	p[3] = x & 0xff;
	p[2] = (x >> 8) & 0xff;
	p[1] = (x >> 16) & 0xff;
	p[0] = (x >> 24) & 0xff;
}


typedef struct SHA256Context {
	uint32_t state[8];
	uint32_t count[2];
	unsigned char buf[64];
} SHA256_CTX;

typedef struct HMAC_SHA256Context {
	SHA256_CTX ictx;
	SHA256_CTX octx;
} HMAC_SHA256_CTX;

/*
 * Encode a length len/4 vector of (uint32_t) into a length len vector of
 * (unsigned char) in big-endian form.  Assumes len is a multiple of 4.
 */
static inline void
be32enc_vect(unsigned char *dst, const uint32_t *src, size_t len)
{
	size_t i;

	for (i = 0; i < len / 4; i++)
		be32enc(dst + i * 4, src[i]);
}

/*
 * Decode a big-endian length len vector of (unsigned char) into a length
 * len/4 vector of (uint32_t).  Assumes len is a multiple of 4.
 */
static inline void
be32dec_vect(uint32_t *dst, const unsigned char *src, size_t len)
{
	size_t i;

	for (i = 0; i < len / 4; i++)
		dst[i] = be32dec(src + i * 4);
}

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
SHA256_Transform(uint32_t * state, const unsigned char block[64])
{
	uint32_t W[64];
	uint32_t S[8];
	uint32_t t0, t1;
	int i;

	/* 1. Prepare message schedule W. */
	be32dec_vect(W, block, 64);
	for (i = 16; i < 64; i++)
		W[i] = s1(W[i - 2]) + W[i - 7] + s0(W[i - 15]) + W[i - 16];

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

static unsigned char PAD[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* SHA-256 initialization.  Begins a SHA-256 operation. */
static inline void
SHA256_Init(SHA256_CTX * ctx)
{

	/* Zero bits processed so far */
	ctx->count[0] = ctx->count[1] = 0;

	/* Magic initialization constants */
	ctx->state[0] = 0x6A09E667;
	ctx->state[1] = 0xBB67AE85;
	ctx->state[2] = 0x3C6EF372;
	ctx->state[3] = 0xA54FF53A;
	ctx->state[4] = 0x510E527F;
	ctx->state[5] = 0x9B05688C;
	ctx->state[6] = 0x1F83D9AB;
	ctx->state[7] = 0x5BE0CD19;
}

/* Add bytes into the hash */
static inline void
SHA256_Update(SHA256_CTX * ctx, const void *in, size_t len)
{
	uint32_t bitlen[2];
	uint32_t r;
	const unsigned char *src = in;

	/* Number of bytes left in the buffer from previous updates */
	r = (ctx->count[1] >> 3) & 0x3f;

	/* Convert the length into a number of bits */
	bitlen[1] = ((uint32_t)len) << 3;
	bitlen[0] = (uint32_t)(len >> 29);

	/* Update number of bits */
	if ((ctx->count[1] += bitlen[1]) < bitlen[1])
		ctx->count[0]++;
	ctx->count[0] += bitlen[0];

	/* Handle the case where we don't need to perform any transforms */
	if (len < 64 - r) {
		memcpy(&ctx->buf[r], src, len);
		return;
	}

	/* Finish the current block */
	memcpy(&ctx->buf[r], src, 64 - r);
	SHA256_Transform(ctx->state, ctx->buf);
	src += 64 - r;
	len -= 64 - r;

	/* Perform complete blocks */
	while (len >= 64) {
		SHA256_Transform(ctx->state, src);
		src += 64;
		len -= 64;
	}

	/* Copy left over data into buffer */
	memcpy(ctx->buf, src, len);
}

/* Add padding and terminating bit-count. */
static inline void
SHA256_Pad(SHA256_CTX * ctx)
{
	unsigned char len[8];
	uint32_t r, plen;

	/*
	 * Convert length to a vector of bytes -- we do this now rather
	 * than later because the length will change after we pad.
	 */
	be32enc_vect(len, ctx->count, 8);

	/* Add 1--64 bytes so that the resulting length is 56 mod 64 */
	r = (ctx->count[1] >> 3) & 0x3f;
	plen = (r < 56) ? (56 - r) : (120 - r);
	SHA256_Update(ctx, PAD, (size_t)plen);

	/* Add the terminating bit-count */
	SHA256_Update(ctx, len, 8);
}

/*
 * SHA-256 finalization.  Pads the input data, exports the hash value,
 * and clears the context state.
 */
static inline void
SHA256_Final(unsigned char digest[32], SHA256_CTX * ctx)
{

	/* Add padding */
	SHA256_Pad(ctx);

	/* Write the hash */
	be32enc_vect(digest, ctx->state, 32);
}

/**
 * PBKDF2_SHA256(passwd, passwdlen, salt, saltlen, c, buf, dkLen):
 * Compute PBKDF2(passwd, salt, c, dkLen) using HMAC-SHA256 as the PRF, and
 * write the output to buf.  The value dkLen must be at most 32 * (2^32 - 1).
 */
static inline void
PBKDF2_SHA256_80_128(const uint8_t * passwd, uint8_t * buf)
{
	HMAC_SHA256_CTX PShctx, hctx;
	size_t i;
	uint8_t ivec[4];
	unsigned char ihash[32];

	/* Compute HMAC state after processing P and S. */
	unsigned char pad[64];
	unsigned char khash[32];

	/* If Klen > 64, the key is really SHA256(K). */
	SHA256_Init(&PShctx.ictx);
	SHA256_Update(&PShctx.ictx, passwd, 80);
	SHA256_Final(khash, &PShctx.ictx);

	SHA256_Init(&PShctx.ictx);
	memset(pad, 0x36, 64);
	for (i = 0; i < 32; i++)
		pad[i] ^= khash[i];
	SHA256_Update(&PShctx.ictx, pad, 64);

	SHA256_Init(&PShctx.octx);
	memset(pad, 0x5c, 64);
	for (i = 0; i < 32; i++)
		pad[i] ^= khash[i];
	SHA256_Update(&PShctx.octx, pad, 64);

	SHA256_Update(&PShctx.ictx, passwd, 80);

	/* Iterate through the blocks. */
	for (i = 0; i * 32 < 128; i++) {
		/* Generate INT(i + 1). */
		be32enc(ivec, (uint32_t)(i + 1));

		/* Compute U_1 = PRF(P, S || INT(i)). */
		memcpy(&hctx, &PShctx, sizeof(HMAC_SHA256_CTX));
		SHA256_Update(&hctx.ictx, ivec, 4);

		SHA256_Final(ihash, &hctx.ictx);
		/* Feed the inner hash to the outer SHA256 operation. */
		SHA256_Update(&hctx.octx, ihash, 32);
		/* Finish the outer SHA256 operation. */
		SHA256_Final(&buf[i*32], &hctx.octx);
	}
}


static inline void
PBKDF2_SHA256_80_128_32(const uint8_t * passwd, const uint8_t * salt, uint8_t * buf)
{
	HMAC_SHA256_CTX PShctx;
	size_t i;
	uint8_t ivec[4];
	unsigned char ihash[32];

	/* Compute HMAC state after processing P and S. */
	unsigned char pad[64];
	unsigned char khash[32];

	/* If Klen > 64, the key is really SHA256(K). */
	SHA256_Init(&PShctx.ictx);
	SHA256_Update(&PShctx.ictx, passwd, 80);
	SHA256_Final(khash, &PShctx.ictx);

	SHA256_Init(&PShctx.ictx);
	memset(pad, 0x36, 64);
	for (i = 0; i < 32; i++)
		pad[i] ^= khash[i];
	SHA256_Update(&PShctx.ictx, pad, 64);

	SHA256_Init(&PShctx.octx);
	memset(pad, 0x5c, 64);
	for (i = 0; i < 32; i++)
		pad[i] ^= khash[i];
	SHA256_Update(&PShctx.octx, pad, 64);

	SHA256_Update(&PShctx.ictx, salt, 128);

	/* Generate INT(i + 1). */
	be32enc(ivec, (uint32_t)(1));

	/* Compute U_1 = PRF(P, S || INT(i)). */
	SHA256_Update(&PShctx.ictx, ivec, 4);

	SHA256_Final(ihash, &PShctx.ictx);
	/* Feed the inner hash to the outer SHA256 operation. */
	SHA256_Update(&PShctx.octx, ihash, 32);
	/* Finish the outer SHA256 operation. */
	SHA256_Final(&buf[0], &PShctx.octx);
}


static inline void
blkcpy(void * dest, void * src, size_t len)
{
	size_t * D = dest;
	size_t * S = src;
	size_t L = len / sizeof(size_t);
	size_t i;

	for (i = 0; i < L; i++)
		D[i] = S[i];
}

static inline void
blkxor(void * dest, void * src, size_t len)
{
	size_t * D = dest;
	size_t * S = src;
	size_t L = len / sizeof(size_t);
	size_t i;

	for (i = 0; i < L; i++)
		D[i] ^= S[i];
}

/**
 * salsa20_8(B):
 * Apply the salsa20/8 core to the provided block.
 */
static inline void
salsa20_8(uint32_t B[16])
{
	uint32_t x00,x01,x02,x03,x04,x05,x06,x07,x08,x09,x10,x11,x12,x13,x14,x15;
	size_t i;

	x00 = B[ 0];
	x01 = B[ 1];
	x02 = B[ 2];
	x03 = B[ 3];
	x04 = B[ 4];
	x05 = B[ 5];
	x06 = B[ 6];
	x07 = B[ 7];
	x08 = B[ 8];
	x09 = B[ 9];
	x10 = B[10];
	x11 = B[11];
	x12 = B[12];
	x13 = B[13];
	x14 = B[14];
	x15 = B[15];
	for (i = 0; i < 8; i += 2) {
#define R(a,b) (((a) << (b)) | ((a) >> (32 - (b))))
		/* Operate on columns. */
		x04 ^= R(x00+x12, 7);	x08 ^= R(x04+x00, 9);	x12 ^= R(x08+x04,13);	x00 ^= R(x12+x08,18);
		x09 ^= R(x05+x01, 7);	x13 ^= R(x09+x05, 9);	x01 ^= R(x13+x09,13);	x05 ^= R(x01+x13,18);
		x14 ^= R(x10+x06, 7);	x02 ^= R(x14+x10, 9);	x06 ^= R(x02+x14,13);	x10 ^= R(x06+x02,18);
		x03 ^= R(x15+x11, 7);	x07 ^= R(x03+x15, 9);	x11 ^= R(x07+x03,13);	x15 ^= R(x11+x07,18);

		/* Operate on rows. */
		x01 ^= R(x00+x03, 7);	x02 ^= R(x01+x00, 9);	x03 ^= R(x02+x01,13);	x00 ^= R(x03+x02,18);
		x06 ^= R(x05+x04, 7);	x07 ^= R(x06+x05, 9);	x04 ^= R(x07+x06,13);	x05 ^= R(x04+x07,18);
		x11 ^= R(x10+x09, 7);	x08 ^= R(x11+x10, 9);	x09 ^= R(x08+x11,13);	x10 ^= R(x09+x08,18);
		x12 ^= R(x15+x14, 7);	x13 ^= R(x12+x15, 9);	x14 ^= R(x13+x12,13);	x15 ^= R(x14+x13,18);
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

/* cpu and memory intensive function to transform a 80 byte buffer into a 32 byte output
   scratchpad size needs to be at least 63 + (128 * r * p) + (256 * r + 64) + (128 * r * N) bytes
 */
static void scrypt_1024_1_1_256_sp(const char* input, char* output, char* scratchpad)
{
	uint32_t * V;
	uint32_t * X;
	uint32_t i;
	uint32_t j;

	X = (uint32_t *)(((uintptr_t)(scratchpad) + 63) & ~ (uintptr_t)(63));
	V = &X[32];

	PBKDF2_SHA256_80_128((const uint8_t*)input, (uint8_t *)X);

	for (i = 0; i < 1024; i += 2) {
		blkcpy(&V[i * 32], X, 128);

		blkxor(&X[0], &X[16], 64);
		salsa20_8(&X[0]);
		blkxor(&X[16], &X[0], 64);
		salsa20_8(&X[16]);

		blkcpy(&V[(i + 1) * 32], X, 128);

		blkxor(&X[0], &X[16], 64);
		salsa20_8(&X[0]);
		blkxor(&X[16], &X[0], 64);
		salsa20_8(&X[16]);
	}
	for (i = 0; i < 1024; i += 2) {
		j = X[16] & 1023;
		blkxor(X, &V[j * 32], 128);

		blkxor(&X[0], &X[16], 64);
		salsa20_8(&X[0]);
		blkxor(&X[16], &X[0], 64);
		salsa20_8(&X[16]);

		j = X[16] & 1023;
		blkxor(X, &V[j * 32], 128);

		blkxor(&X[0], &X[16], 64);
		salsa20_8(&X[0]);
		blkxor(&X[16], &X[0], 64);
		salsa20_8(&X[16]);
	}

	PBKDF2_SHA256_80_128_32((const uint8_t*)input, (const uint8_t *)X, (uint8_t*)output);
}

int scanhash_scrypt(int thr_id, unsigned char *pdata, unsigned char *scratchbuf,
	const unsigned char *ptarget,
	uint32_t max_nonce, unsigned long *hashes_done)
{
	unsigned char data[80];
	unsigned char tmp_hash[32];
	uint32_t *nonce = (uint32_t *)(data + 64 + 12);
	uint32_t n = 0;
	uint32_t Htarg = *(uint32_t *)(ptarget + 28);
	int i;

	work_restart[thr_id].restart = 0;
	
	for (i = 0; i < 80/4; i++)
		((uint32_t *)data)[i] = swab32(((uint32_t *)pdata)[i]);
	
	while(1) {
		n++;
		*nonce = n;
		scrypt_1024_1_1_256_sp(data, tmp_hash, scratchbuf);

		if (*(uint32_t *)(tmp_hash+28) <= Htarg) {
			*(uint32_t *)(pdata + 64 + 12) = swab32(n);
			*hashes_done = n;
			return true;
		}

		if ((n >= max_nonce) || work_restart[thr_id].restart) {
			*hashes_done = n;
			break;
		}
	}
	return false;
}

