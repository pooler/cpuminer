#ifndef __MINER_H__
#define __MINER_H__

#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>
#include <jansson.h>

#ifdef __SSE2__
#define WANT_SSE2_4WAY 1
#endif

#if defined(__i386__) || defined(__x86_64__)
#define WANT_VIA_PADLOCK 1
#endif

#if defined(__i386__)
#define WANT_CRYPTOPP_ASM32
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

static inline uint32_t swab32(uint32_t v)
{
	return __builtin_bswap32(v);
}

static inline void swap256(void *dest_p, const void *src_p)
{
	uint32_t *dest = dest_p;
	const uint32_t *src = src_p;

	dest[0] = src[7];
	dest[1] = src[6];
	dest[2] = src[5];
	dest[3] = src[4];
	dest[4] = src[3];
	dest[5] = src[2];
	dest[6] = src[1];
	dest[7] = src[0];
}

extern bool opt_debug;
extern bool opt_protocol;
extern const uint32_t sha256_init_state[];
extern json_t *json_rpc_call(const char *url, const char *userpass,
			     const char *rpc_req);
extern char *bin2hex(unsigned char *p, size_t len);
extern bool hex2bin(unsigned char *p, const char *hexstr, size_t len);

extern unsigned int ScanHash_4WaySSE2(const unsigned char *pmidstate,
	unsigned char *pdata, unsigned char *phash1, unsigned char *phash,
	const unsigned char *ptarget,
	uint32_t max_nonce, unsigned long *nHashesDone);

extern bool scanhash_via(unsigned char *data_inout,
	const unsigned char *target,
	uint32_t max_nonce, unsigned long *hashes_done);

extern bool scanhash_c(const unsigned char *midstate, unsigned char *data,
	      unsigned char *hash1, unsigned char *hash,
	      const unsigned char *target,
	      uint32_t max_nonce, unsigned long *hashes_done);
extern bool scanhash_cryptopp(const unsigned char *midstate,unsigned char *data,
	      unsigned char *hash1, unsigned char *hash,
	      const unsigned char *target,
	      uint32_t max_nonce, unsigned long *hashes_done);
extern bool scanhash_asm32(const unsigned char *midstate,unsigned char *data,
	      unsigned char *hash1, unsigned char *hash,
	      const unsigned char *target,
	      uint32_t max_nonce, unsigned long *hashes_done);

extern int
timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y);

extern bool fulltest(const unsigned char *hash, const unsigned char *target);

#endif /* __MINER_H__ */
