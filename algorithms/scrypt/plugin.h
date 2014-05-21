#ifndef SCRYPT_PLUGIN_H
#define SCRYPT_PLUGIN_H

#define PLUGIN_NAME_SCRYPT "scrypt"
#define PLUGIN_DESC_SCRYPT "Scrypt algorithm"

int init_SCRYPT();
void* thread_init_SCRYPT(int* error);

int scanhash_SCRYPT(int thr_id, uint32_t *pdata,
        void *thread_local_data, const uint32_t *ptarget,
        uint32_t max_nonce, unsigned long *hashes_done);

#endif // SCRYPT_PLUGIN_H
