#if (__cplusplus)
extern "C" {
#endif

void neoscrypt(const unsigned char *input, unsigned char *output,
  unsigned int profile);

void neoscrypt_blake2s(const void *input, const unsigned int input_size,
  const void *key, const unsigned char key_size,
  void *output, const unsigned char output_size);

void neoscrypt_copy(void *dstp, const void *srcp, unsigned int len);
void neoscrypt_erase(void *dstp, unsigned int len);
void neoscrypt_xor(void *dstp, const void *srcp, unsigned int len);

#if (ASM) && (MINER_4WAY)
void neoscrypt_4way(const unsigned char *input, unsigned char *output,
  unsigned int profile);

void neoscrypt_blake2s_4way(const unsigned char *input,
  const unsigned char *key, unsigned char *output);

void neoscrypt_fastkdf_4way(const unsigned char *password,
  const unsigned char *salt, unsigned char *output, unsigned int mode);
#endif

unsigned int cpu_vec_exts(void);

#if (__cplusplus)
}
#else

#if (WINDOWS) && (__APPLE__)
/* sizeof(unsigned long) = 4 for MinGW64 and Mac GCC */
typedef unsigned long long ulong;
#else
typedef unsigned long ulong;
#endif
typedef unsigned int  uint;
typedef unsigned char uchar;

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? a : b)
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? a : b)
#endif

#define SCRYPT_BLOCK_SIZE 64
#define SCRYPT_HASH_BLOCK_SIZE 64
#define SCRYPT_HASH_DIGEST_SIZE 32

typedef uchar hash_digest[SCRYPT_HASH_DIGEST_SIZE];

#define ROTL32(a,b) (((a) << (b)) | ((a) >> (32 - b)))
#define ROTR32(a,b) (((a) >> (b)) | ((a) << (32 - b)))

#define U8TO32_BE(p) \
    (((uint)((p)[0]) << 24) | ((uint)((p)[1]) << 16) | \
    ((uint)((p)[2]) <<  8) | ((uint)((p)[3])))

#define U32TO8_BE(p, v) \
    (p)[0] = (uchar)((v) >> 24); (p)[1] = (uchar)((v) >> 16); \
    (p)[2] = (uchar)((v) >>  8); (p)[3] = (uchar)((v)      );

#define U64TO8_BE(p, v) \
    U32TO8_BE((p),     (uint)((v) >> 32)); \
    U32TO8_BE((p) + 4, (uint)((v)      ));

#endif
