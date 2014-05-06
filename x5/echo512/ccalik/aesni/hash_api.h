/*
 * file        : hash_api.h
 * version     : 1.0.208
 * date        : 14.12.2010
 * 
 * ECHO vperm implementation Hash API
 *
 * Cagdas Calik
 * ccalik@metu.edu.tr
 * Institute of Applied Mathematics, Middle East Technical University, Turkey.
 *
 */


#ifndef HASH_API_H
#define HASH_API_H

#define AES_NI
#ifdef AES_NI
#define HASH_IMPL_STR	"ECHO-aesni"
#else
#define HASH_IMPL_STR	"ECHO-vperm"
#endif


#include "sha3_common.h"

#include <emmintrin.h>


typedef struct
{
	__m128i			state[4][4];
	__m128i			k;
	__m128i			hashsize;
	__m128i			const1536;

	unsigned int	uRounds;
	unsigned int	uHashSize;
	unsigned int	uBlockLength;
	unsigned int	uBufferBytes;
	DataLength		processed_bits;
	BitSequence		buffer[192];

} hashState_echo;

HashReturn init_echo(hashState_echo *state, int hashbitlen);

HashReturn update_echo(hashState_echo *state, const BitSequence *data, DataLength databitlen);

HashReturn final_echo(hashState_echo *state, BitSequence *hashval);

HashReturn hash_echo(int hashbitlen, const BitSequence *data, DataLength databitlen, BitSequence *hashval);


#endif // HASH_API_H

