#ifndef X11_PLUGIN_H
#define X11_PLUGIN_H

#define PLUGIN_NAME_X11 "X11"
#define PLUGIN_DESC_X11 "\tX11 algorithm"

int init_X11();
void* thread_init_X11(int* error);

int scanhash_X11(int thr_id, uint32_t *pdata,
        void *thread_local_data, const uint32_t *ptarget,
        uint32_t max_nonce, unsigned long *hashes_done);

#endif // X11_PLUGIN_H
