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

#define ___constant_swab32(x) ((uint32_t)(                       \
        (((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) |            \
        (((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) |            \
        (((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) |            \
        (((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24)))

static inline uint32_t swab32(uint32_t v)
{
	return ___constant_swab32(v);
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
	unsigned long *nHashesDone);

extern bool scanhash_via(unsigned char *data_inout, unsigned long *hashes_done);

extern bool scanhash_c(const unsigned char *midstate, unsigned char *data,
	      unsigned char *hash1, unsigned char *hash,
	      unsigned long *hashes_done);
extern bool scanhash_cryptopp(const unsigned char *midstate,unsigned char *data,
	      unsigned char *hash1, unsigned char *hash,
	      unsigned long *hashes_done);
extern bool scanhash_asm32(const unsigned char *midstate,unsigned char *data,
	      unsigned char *hash1, unsigned char *hash,
	      unsigned long *hashes_done);

extern int
timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y);

#endif /* __MINER_H__ */
