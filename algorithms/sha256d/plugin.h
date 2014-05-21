#ifndef SHA256D_PLUGIN_H
#define SHA256D_PLUGIN_H

#define PLUGIN_NAME_SHA256D "sha256d"
#define PLUGIN_DESC_SHA256D "sha256d algorithm"

int init_SHA256D();
void* thread_init_SHA256D(int* error);

int scanhash_SHA256D(int thr_id, uint32_t *pdata,
        void *thread_local_data, const uint32_t *ptarget,
        uint32_t max_nonce, unsigned long *hashes_done);

#endif // SHA256D_PLUGIN_H
