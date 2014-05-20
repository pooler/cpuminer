#define PLUGIN_NAME_NSCRYPT "nscrypt"
#define PLUGIN_DESC_NSCRYPT "NScrypt algorithm"

int init_NSCRYPT() { return 0;}
void* thread_init_NSCRYPT(int* success) { return 0;}

int scanhash_NSCRYPT(int thr_id, uint32_t *pdata,
        void *scratchbuf, const uint32_t *ptarget,
        uint32_t max_nonce, unsigned long *hashes_done)
{ return 0; }

