#ifndef NSCRYPT_PLUGIN_H
#define NSCRYPT_PLUGIN_H

#define PLUGIN_NAME_NSCRYPT "nscrypt"
#define PLUGIN_DESC_NSCRYPT "NScrypt algorithm"

int init_NSCRYPT();
void* thread_init_NSCRYPT(int* error);

int scanhash_NSCRYPT(int thr_id, uint32_t *pdata,
        void *thread_local_data, const uint32_t *ptarget,
        uint32_t max_nonce, unsigned long *hashes_done);

#endif // NSCRYPT_PLUGIN_H
