
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include "miner.h"

#ifdef WANT_VIA_PADLOCK

#define ___constant_swab32(x) ((uint32_t)(                       \
        (((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) |            \
        (((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) |            \
        (((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) |            \
        (((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24)))

static inline uint32_t swab32(uint32_t v)
{
	return ___constant_swab32(v);
}

static void via_sha256(void *hash, void *buf, unsigned len)
{
	unsigned stat = 0;
	asm volatile(".byte 0xf3, 0x0f, 0xa6, 0xd0"
		     :"+S"(buf), "+a"(stat)
		     :"c"(len), "D" (hash)
		     :"memory");
}

bool scanhash_via(const unsigned char *midstate, const unsigned char *data_in,
	          unsigned char *hash1, unsigned char *hash,
	          unsigned long *hashes_done)
{
	unsigned char data[128] __attribute__((aligned(128)));
	unsigned char tmp_hash1[32] __attribute__((aligned(128)));
	uint32_t *hash32 = (uint32_t *) hash;
	uint32_t *nonce = (uint32_t *)(data + 12);
	uint32_t n = 0;
	unsigned long stat_ctr = 0;
	int i;

	/* bitcoin gives us big endian input, but via wants LE,
	 * so we reverse the swapping bitcoin has already done (extra work)
	 * in order to permit the hardware to swap everything
	 * back to BE again (extra work).
	 */
	for (i = 0; i < 128/4; i++) {
		uint32_t *data32 = (uint32_t *) data;
		data32[i] = swab32(((uint32_t *)data_in)[i]);
	}

	while (1) {
		n++;
		*nonce = n;

		/* first SHA256 transform */
		memcpy(tmp_hash1, sha256_init_state, 32);
		via_sha256(tmp_hash1, data, 80);	/* or maybe 128? */

		for (i = 0; i < 32/4; i++)
			((uint32_t *)tmp_hash1)[i] =
				swab32(((uint32_t *)tmp_hash1)[i]);

		/* second SHA256 transform */
		memcpy(hash, sha256_init_state, 32);
		via_sha256(hash, tmp_hash1, 32);

		stat_ctr++;

		if (hash32[7] == 0) {
			char *hexstr;

			hexstr = bin2hex(hash, 32);
			fprintf(stderr,
				"DBG: found zeroes in hash:\n%s\n",
				hexstr);
			free(hexstr);

			*hashes_done = stat_ctr;
			return true;
		}

		if ((n & 0xffffff) == 0) {
			if (opt_debug)
				fprintf(stderr, "DBG: end of nonce range\n");
			*hashes_done = stat_ctr;
			return false;
		}
	}
}

#endif /* WANT_VIA_PADLOCK */

