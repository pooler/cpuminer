#ifndef SCRYPT_PLUGIN_H
#define SCRYPT_PLUGIN_H

#define PLUGIN_NAME_SCRYPT "scrypt"
#define PLUGIN_DESC_SCRYPT "scrypt(N, 1, 1), default: N=1024"

int init_SCRYPT();
void* thread_init_SCRYPT(int* error, void *extra_param);

int scanhash_SCRYPT(int thr_id, uint32_t *pdata,
        void *thread_local_data, const uint32_t *ptarget,
        uint32_t max_nonce, unsigned long *hashes_done, void* extra_param);

void *param_default_SCRYPT();

void *param_parse_SCRYPT( const char *str, int *error);

#endif // SCRYPT_PLUGIN_H
