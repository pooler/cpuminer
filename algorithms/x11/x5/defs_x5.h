
#ifndef DEFS_X5_H__
#define DEFS_X5_H__
#include <emmintrin.h>
typedef unsigned char BitSequence;
typedef unsigned long long DataLength;
typedef enum { SUCCESS = 0, FAIL = 1, BAD_HASHBITLEN = 2} HashReturn;

typedef unsigned char uint8;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef struct {
    uint32 buffer[8]; /* Buffer to be hashed */
    __m128i chainv[10];   /* Chaining values */
    uint64 bitlen[2]; /* Message length in bits */
    uint32 rembitlen; /* Length of buffer data to be hashed */
    int hashbitlen;
} hashState_luffa;


typedef unsigned char byte;
#endif